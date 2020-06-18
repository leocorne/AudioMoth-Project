/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/lite/experimental/micro/examples/micro_speech/audio_provider.h"

#include "tensorflow/lite/experimental/micro/examples/micro_speech/model_settings.h"

#include "audioMoth.h"

#define NUMBER_OF_SAMPLES_IN_DMA_TRANSFER   1024

namespace {
bool g_is_audio_initialized = false;
// An internal buffer able to fit 16x our sample size

constexpr int kAudioCaptureBufferSize = NUMBER_OF_SAMPLES_IN_DMA_TRANSFER * 16;
int16_t* g_audio_capture_buffer;
// A buffer that holds our output
int16_t g_audio_output_buffer[kMaxAudioSampleSize];
// Mark as volatile so we can check in a while loop to see if
// any samples have arrived yet.
volatile int32_t g_latest_audio_timestamp = 0;
}  // namespace

void CaptureSamples(int16_t *samples_source) {
  // This is how many bytes of new data we have each time this is called
  const int number_of_samples = NUMBER_OF_SAMPLES_IN_DMA_TRANSFER;
  // Calculate what timestamp the last audio sample represents
  const int32_t time_in_ms =
      g_latest_audio_timestamp +
      (number_of_samples / (kAudioSampleFrequency / 1000));
  // Determine the index, in the history of all samples, of the last sample
  const int32_t start_sample_offset =
      g_latest_audio_timestamp * (kAudioSampleFrequency / 1000);
  // Determine the index of this sample in our ring buffer
  const int capture_index = start_sample_offset % kAudioCaptureBufferSize;
  // Read the data to the correct place in our buffer
  for (int i = 0; i<NUMBER_OF_SAMPLES_IN_DMA_TRANSFER; i++){
    g_audio_capture_buffer[capture_index + i] = samples_source[i];
  }
  // This is how we let the outside world know that new audio data has arrived.
  g_latest_audio_timestamp = time_in_ms;
}

TfLiteStatus InitAudioRecording(tflite::ErrorReporter* error_reporter) {

  //error_reporter->Report("Initializing audio recorder");
  AudioMoth_startupMicrophone();
  //error_reporter->Report("Enabled microphone");
  AudioMoth_startMicrophoneSamples(16000);
  //error_reporter->Report("Started recordings");
  // Block until we have our first audio sample
  while (!g_latest_audio_timestamp) {
    //error_reporter->Report("Waiting for first recording");
    AudioMoth_sleep();
  }

  return kTfLiteOk;
}

TfLiteStatus GetAudioSamples(tflite::ErrorReporter* error_reporter,
                             int start_ms, int duration_ms,
                             int* audio_samples_size, int16_t** audio_samples) {
  // Set everything up to start receiving audio

  //error_reporter->Report("Getting audio slice...");
  if (!g_is_audio_initialized) {

    g_audio_capture_buffer = getAddressOfAudioCaptureBuffer();
    TfLiteStatus init_status = InitAudioRecording(error_reporter);
    if (init_status != kTfLiteOk) {
      return init_status;
    }
    g_is_audio_initialized = true;
    //error_reporter->Report("Initialized audiomoth recordings");
  }
  // This next part should only be called when the main thread notices that the
  // latest audio sample data timestamp has changed, so that there's new data
  // in the capture ring buffer. The ring buffer will eventually wrap around and
  // overwrite the data, but the assumption is that the main thread is checking
  // often enough and the buffer is large enough that this call will be made
  // before that happens.

  // Determine the index, in the history of all samples, of the first
  // sample we want
  //error_reporter->Report("Start_ms: %d, Frequency: %d", start_ms, kAudioSampleFrequency);
  const int start_offset = start_ms * (kAudioSampleFrequency / 1000);
  // Determine how many samples we want in total
  const int duration_sample_count =
      duration_ms * (kAudioSampleFrequency / 1000);
  //error_reporter->Report("Start offset: %d", start_offset);
  //error_reporter->Report("Going to return %d samples", duration_sample_count);
  for (int i = 0; i < duration_sample_count; ++i) {
    // For each sample, transform its index in the history of all samples into
    // its index in g_audio_capture_buffer
    const int capture_index = (start_offset + i) % kAudioCaptureBufferSize;
    // Write the sample to the output buffer
    g_audio_output_buffer[i] = g_audio_capture_buffer[capture_index];
    //error_reporter->Report("Found %d sample at position %d", i, capture_index);
  }
  //error_reporter->Report("Found all the samples and put them in output buffer");
  // Set pointers to provide access to the audio
  *audio_samples_size = kMaxAudioSampleSize;
  *audio_samples = g_audio_output_buffer;
  //error_reporter->Report("Recording ok");
  return kTfLiteOk;
}

int32_t LatestAudioTimestamp() { return g_latest_audio_timestamp; }
