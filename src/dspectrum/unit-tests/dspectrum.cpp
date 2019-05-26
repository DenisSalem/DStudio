#include <iostream>
#include "dspectrum.hpp"

double AverageSignal(double * buffer, int length) {
  double average = 0.0;
  for (int t = 0; t < length; t++) {
    average += buffer[t];
  }
  return average / (double) length;
}

void WriteSignalIntoTexture(RenderContext * renderContext, double * signal) {
  for (int x = 0; x < renderContext->width; x++) {
    signal[x] += 0.5;
  }

  for (int x = 0; x < renderContext->width; x++) {
    renderContext->texture[ (int) (signal[x] * renderContext->height) * renderContext->width + x ].Blue   = 255;
    renderContext->texture[ (int) (signal[x] * renderContext->height) * renderContext->width + x ].Red    = 255;
    renderContext->texture[ (int) (signal[x] * renderContext->height) * renderContext->width + x ].Green  = 255;
  }
}

void WriteSignalIntoBuffer(double * buffer, int length, double periode, double amplitude) {
  for (int t = 0; t < length; t++) {
    buffer[t] += 0.5 * sin(6.283185 * ((double) t / periode)) * amplitude;
  }
}
