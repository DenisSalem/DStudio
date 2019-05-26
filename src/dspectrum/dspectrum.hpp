#define PI 3.141592
#define PI2 6.283185

#define HAMMING_WINDOW_LENGTH 2048
#define SAMPLES_TO_MERGE 6
#define WINDOW_HEIGHT 320
#define WINDOW_WIDTH 320

/*
 * Iniate hammingWindow values, based on
 * http://ofdsp.blogspot.fr/2011/08/short-time-fourier-transform-with-fftw3.html
 */

void InitHammingWindow(int length, double ** buffer);

/* 
 * Processe signal for given input and window function. 
 */

void UpdateSpectrum(RenderContext * renderContext, int signalLength, double * windowBuffer, double * signal);
void PrepareSpectrogramRenderContext(RenderContext * renderContext);

/*
 * There is a lot of stuff to render, it's better to have a function
 * to do that for each kind of sprites.
 */

void RenderSpectrogram(RenderContext * renderContext);
