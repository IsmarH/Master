/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

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


#include "accelerometer_handler_serial.h"

//#include <Arduino.h>
//#include "Nicla_System.h"
//#include "Arduino.h"
#include "Arduino_BHY2.h"

#include "constants.h"

SensorXYZ accel(SENSOR_ID_ACC);
// A buffer holding the last 200 sets of 3-channel values
int8_t save_data[50] = {0};
// Most recent position in the save_data buffer
int begin_index = 0;
// True if there is not yet enough data to run inference
bool pending_initial_data = true;
// How often we should save a measurement during downsampling
int sample_every_n;
// The number of measurements since we last saved one
int sample_skip_counter = 1;

TfLiteStatus SetupAccelerometer(tflite::ErrorReporter* error_reporter) {
  // Switch on the IMU
BHY2.begin();
  
  if ((!accel.begin(100))) {
    TF_LITE_REPORT_ERROR(error_reporter, "Failed to initialize IMU");
    return kTfLiteError;
  }

  // Make sure we are pulling measurements into a FIFO.
  // If you see an error on this line, make sure you have at least v1.1.0 of the
  // Arduino_LSM9DS1 library installed.

  // Determine how many measurements to keep in order to
  // meet kTargetHz
 // int sample_rate = 100;
  //sample_every_n = static_cast<int>(roundf(sample_rate / kTargetHz));

  //TF_LITE_REPORT_ERROR(error_reporter, "Magic starts!");

 // return kTfLiteOk;
}

bool ReadAccelerometer(tflite::ErrorReporter* error_reporter, int8_t* input,
                       int length) {
  // Keep track of whether we stored any new data
  bool new_data = false;
  // Loop through new samples and add to buffer
  while (1) {
   // float x, y, z;
    // Read each sample, removing it from the device's FIFO buffer
BHY2.update();
    // Throw away this sample unless it's the nth
  //  if (sample_skip_counter != sample_every_n) {
   //   sample_skip_counter += 1;
    //  continue;
   // }
    // Write samples to our buffer, converting to milli-Gs and rotating the axis
    // order for compatibility with model (sensor orientation is different on
    // Arduino Nano BLE Sense compared with SparkFun Edge).
    // The expected orientation of the Arduino on the wand is with the USB port
    // facing down the shaft towards the user's hand, with the reset button
    // pointing at the user's face:
    //
    //                  ____
    //                 |    |<- Arduino board
    //                 |    |
    //                 | () |  <- Reset button
    //                 |    |
    //                  -TT-   <- USB port
    //                   ||
    //                   ||<- Wand
    //                  ....
    //                   ||
    //                   ||
    //                   ()
    //

    int8_t x = accel.x();
    int8_t y = accel.y();
    int8_t z = accel.z();


    const int8_t norm_x = -z;
    const int8_t norm_y = y;
    const int8_t norm_z = x;
    //save_data[begin_index++] = norm_x * 1000;
    //save_data[begin_index++] = norm_y * 1000;
    //save_data[begin_index++] = norm_z * 1000;
    input[begin_index++] =  norm_x;
    input[begin_index++] =  norm_y;
    input[begin_index++] =  norm_z;
    // Since we took a sample, reset the skip counter
   // sample_skip_counter = 1;
    // If we reached the end of the circle buffer, reset
    if (begin_index >= 50) {
      begin_index = 0;
    }
    new_data = true;
  }

  // Skip this round if data is not ready yet
  if (!new_data) {
    return false;
  }

  // Check if we are ready for prediction or still pending more initial data
  if (pending_initial_data && begin_index >= 32) {
    pending_initial_data = false;
  }

  // Return if we don't have enough data
  if (pending_initial_data) {
    return false;
  }

   //Copy the requested number of bytes to the provided input tensor
 for (int i = 0; i < length; ++i) {
    int ring_array_index = begin_index + i - length;
    if (ring_array_index < 0) {
      ring_array_index += 50;
    }
    input[i] = save_data[ring_array_index];
  }

  return true;
}


