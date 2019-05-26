#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <fftw3.h>
#include "core/core.hpp"
#include "dspectrum.hpp"
#include "unit-tests/dspectrum.hpp"

void PrepareSpectrogramRenderContext(RenderContext * renderContext) {
  renderContext->verticesAttributes[0].x = -1.0f;
  renderContext->verticesAttributes[0].y = 1.0f;
  renderContext->verticesAttributes[0].z = 0.0f;
  renderContext->verticesAttributes[1].x = -1.0f;
  renderContext->verticesAttributes[1].y = -1.0f;
  renderContext->verticesAttributes[1].z = 0.0f;
  renderContext->verticesAttributes[2].x = 1.0f;
  renderContext->verticesAttributes[2].y = 1.0f;
  renderContext->verticesAttributes[2].z = 0.0f;
  renderContext->verticesAttributes[3].x = 1.0f;
  renderContext->verticesAttributes[3].y = -1.0f;
  renderContext->verticesAttributes[3].z = 0.0f;

  renderContext->vertexIndex[0] = 0;
  renderContext->vertexIndex[1] = 1;
  renderContext->vertexIndex[2] = 2;
  renderContext->vertexIndex[3] = 3;

  renderContext->verticesAttributes[0].s = 0.0f;
  renderContext->verticesAttributes[0].t = 0.0f;
  renderContext->verticesAttributes[1].s = 0.0f;
  renderContext->verticesAttributes[1].t = 1.0f;
  renderContext->verticesAttributes[2].s = 1.0f;
  renderContext->verticesAttributes[2].t = 0.0f;
  renderContext->verticesAttributes[3].s = 1.0f;
  renderContext->verticesAttributes[3].t = 1.0f;

  renderContext->width = 320;
  renderContext->height = 320;
  
  SetString(renderContext->vertexShaderPath,"shaders/vertex.shader", 64);
  SetString(renderContext->fragmentShaderPath,"shaders/fragment.shader", 64);

  renderContext->texture = new RGBValues[102400](); // 320 x 320 
}

void RenderSpectrogram(RenderContext * renderContext) {
  glUseProgram(renderContext->programID);
    glBindTexture(GL_TEXTURE_2D, renderContext->textureID);
      glBindVertexArray(renderContext->VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderContext->IBO);
          glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (GLvoid *) 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
}

void UpdateSpectrum(RenderContext * renderContext, int signalLength, double * windowBuffer, double * signal) {
  int chunkPosition = 0;
  int signalPosition;
  int color;
  unsigned char amplitude;
  int x=0;

  fftw_complex * out = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * HAMMING_WINDOW_LENGTH );
  double * in = new double[HAMMING_WINDOW_LENGTH];
  fftw_plan p;

  p = fftw_plan_dft_r2c_1d(HAMMING_WINDOW_LENGTH, in, out, FFTW_ESTIMATE);

  while (chunkPosition < signalLength) {
    // Apply Hamming window on current chunk
    for (int t = 0; t < HAMMING_WINDOW_LENGTH; t++) {
      signalPosition = chunkPosition + t;
      if( signalPosition < signalLength) {
        in[t] = signal[signalPosition] * windowBuffer[t];
      }
      else {
        in[t] = 0.0;
      }
    }

    fftw_execute(p); // Process current chunk

    // Copy into texture
    for (int t = 0; t < HAMMING_WINDOW_LENGTH / 2.0; t++){
      if (t == renderContext->height) {
        break;
      }
      out[t][0] *= 2./HAMMING_WINDOW_LENGTH;
      out[t][1] *= 2./HAMMING_WINDOW_LENGTH;

      amplitude = (unsigned char) (255* sqrt(out[t][0]*out[t][0] + out[t][1]*out[t][1]));
      
      renderContext->texture[x + renderContext->width * t].Blue   += amplitude;
      renderContext->texture[x + renderContext->width * t].Red    += amplitude;
      renderContext->texture[x + renderContext->width * t].Green  += amplitude;
    }
    x++;
    chunkPosition += HAMMING_WINDOW_LENGTH;
  }

  fftw_destroy_plan(p);
  fftw_free(in);
  fftw_free(out);
}

void InitHammingWindow(int length, double ** buffer) {
 (*buffer) = new double[length];
 for(int i = 0; i < length; i++) {
   (*buffer)[i] = 0.54 - (0.46 * cos( PI2 * ((double) i / (double)(length - 1))));
 }
}

int main(int argc, char ** argv) {
  GLFWwindow * window;
  RenderContext rcSpectrogram;
  double * signal = new double[SIGNAL_LENGTH](); 
  double * hammingWindow;
  
  InitWindow(&window, WINDOW_WIDTH, WINDOW_HEIGHT, "D S P E C T R U M");
  InitHammingWindow(HAMMING_WINDOW_LENGTH, &hammingWindow);
  
  /*WriteSignalIntoBuffer(signal, SIGNAL_LENGTH, 12, 1.0);  // UNIT-TESTING 
  WriteSignalIntoBuffer(signal, SIGNAL_LENGTH, 20, 1.0); // UNIT-TESTING
  WriteSignalIntoBuffer(signal, SIGNAL_LENGTH, 36, 1.0); // UNIT-TESTING
  WriteSignalIntoBuffer(signal, SIGNAL_LENGTH, 70, 1.0); // UNIT-TESTING
  WriteSignalIntoBuffer(signal, SIGNAL_LENGTH, 130, 1.0); // UNIT-TESTING
  WriteSignalIntoBuffer(signal, SIGNAL_LENGTH, 260, 1.0); // UNIT-TESTING
  WriteSignalIntoBuffer(signal, SIGNAL_LENGTH, 520, 1.0); // UNIT-TESTING
  WriteSignalIntoBuffer(signal, SIGNAL_LENGTH, 1035, 1.0); // UNIT-TESTING
  WriteSignalIntoBuffer(signal, SIGNAL_LENGTH, 2062, 1.0); // UNIT-TESTING*/
  
  PrepareSpectrogramRenderContext(&rcSpectrogram);
  
  /* WriteSignalIntoTexture(&rcSpectrogram, signal); // UNIT-TESTING */
  UpdateSpectrum(&rcSpectrogram, SIGNAL_LENGTH, hammingWindow, signal);
  PrepareRender(&rcSpectrogram);
  
  while(!glfwWindowShouldClose(window)) {
    usleep(40000);
    glEnable(GL_CULL_FACE);  
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    RenderSpectrogram(&rcSpectrogram);

    glfwSwapBuffers(window);
  }

  return 0;
}
