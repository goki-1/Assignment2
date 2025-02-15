#include "hal/sampler.h"
#include "hal/i2c_bus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

static double currentBuffer[1000];  // Current second samples
static double historyBuffer[1000];  // Last second samples
static int currentSampleCount = 0;         // Number of samples in current second
static int historySampleCount = 0;         // Number of samples in last second


static pthread_t samplerThread;
static pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;
static bool isRunning = false;

static double currentAverage = 0.0;
static bool isFirstSample = true;

static long long totalSamples = 0;
// Begin/end the background thread which samples light levels.
void Sampler_init(void){
  int_
}
void Sampler_cleanup(void);
// Must be called once every 1s.
// Moves the samples that it has been collecting this second into
// the history, which makes the samples available for reads (below).
void Sampler_moveCurrentDataToHistory(void);
// Get the number of samples collected during the previous complete second.
int Sampler_getHistorySize(void);
// Get a copy of the samples in the sample history.
// Returns a newly allocated array and sets `size` to be the
// number of elements in the returned array (output-only parameter).
// The calling code must call free() on the returned pointer.
// Note: It provides both data and size to ensure consistency.
double* Sampler_getHistory(int *size);
// Get the average light level (not tied to the history).
double Sampler_getAverageReading(void);
// Get the total number of light level samples taken so far.
long long Sampler_getNumSamplesTaken(void);

