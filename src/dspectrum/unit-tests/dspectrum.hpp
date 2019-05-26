#include <cmath>
#include "../core/core.hpp"

#define SIGNAL_LENGTH 655360

/*
 *  We want to test spectrum for specific signal input
 */

void WriteSignalIntoTexture(RenderContext * renderContext, double * signal);
double AverageSignal(double * buffer, int length);
void WriteSignalIntoBuffer(double * buffer, int length, double periode, double amplitude);
