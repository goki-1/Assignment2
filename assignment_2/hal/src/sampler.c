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
static int i2c_file_desc;
static long long totalSamples = 0;

static void updateAverage(double sample) {
    if (isFirstSample) {
        currentAverage = sample;
        isFirstSample = false;
    } else {
        currentAverage = 0.1 * sample + 99.9 * currentAverage;
    }
}

static void storeSample(double sample) {
    pthread_mutex_lock(&bufferMutex);
    if (currentSampleCount < 1000) {
        currentBuffer[currentSampleCount++] = sample;
    }
    pthread_mutex_unlock(&bufferMutex);
}

static void* read_voltage(void* arg) {
    while (isRunning) {
        float sample = getVoltage( i2c_file_desc );    // Read from ADC
        totalSamples++;                        // Count total samples
        updateAverage(sample);                // Update EMA
        storeSample(sample);          
        usleep(1000);                         // Sleep 1 millisecond
    }
    return NULL;
}
// Begin/end the background thread which samples light levels.
void Sampler_init(void){
    i2c_file_desc = init_i2c_bus(I2CDRV_LINUX_BUS, I2C_DEVICE_ADDRESS);
    write_i2c_reg16(i2c_file_desc, REG_CONFIGURATION, TLA2024_CHANNEL_CONF_2);
    isRunning = true;
    pthread_create(&samplerThread, NULL, read_voltage, NULL);
}
void Sampler_cleanup(void){
    isRunning = false;
    pthread_join(samplerThread, NULL);
    close_i2c_bus(i2c_file_desc);
}
// Must be called once every 1s.
// Moves the samples that it has been collecting this second into
// the history, which makes the samples available for reads (below).
void Sampler_moveCurrentDataToHistory(void) {
    pthread_mutex_lock(&bufferMutex);
    // Copy samples using a for loop
    for (int i = 0; i < currentSampleCount; i++) {
        historyBuffer[i] = currentBuffer[i];
    }
    historySampleCount = currentSampleCount;
    currentSampleCount = 0;  // Reset current buffer for the next second
    pthread_mutex_unlock(&bufferMutex);
}



// Get the number of samples collected during the previous complete second.
int Sampler_getHistorySize(void) {
    return historySampleCount;
}
// Get a copy of the samples in the sample history.
// Returns a newly allocated array and sets `size` to be the
// number of elements in the returned array (output-only parameter).
// The calling code must call free() on the returned pointer.
// Note: It provides both data and size to ensure consistency.
double* Sampler_getHistory(int *size){
pthread_mutex_lock(&bufferMutex);
*size = historySampleCount;
double* copy = malloc(sizeof(double) * historySampleCount);
if (copy) {
    memcpy(copy, historyBuffer, sizeof(double) * historySampleCount);
}
historySampleCount = 0;
pthread_mutex_unlock(&bufferMutex);
return copy;
}


// Get the average light level (not tied to the history).
double Sampler_getAverageReading(void) {
    return currentAverage;
}
// Get the total number of light level samples taken so far.
long long Sampler_getNumSamplesTaken(void) {
    return totalSamples;
}
