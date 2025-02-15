#include "sampler.h"
#include "hal_light_sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// Begin/end the background thread which samples light levels.
void Sampler_init(void);
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
#endif
