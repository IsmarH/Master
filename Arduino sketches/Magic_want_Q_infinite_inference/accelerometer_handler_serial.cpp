#define BAUD_RATE_SERIAL_DATA (115200)
#include "accelerometer_handler_serial.h"
#define RING_BUFFER_SIZE 600

#include <Arduino.h>
//#include <Arduino_LSM9DS1.h>
#include "magic_wand_model_data.h"
#include "constants.h"
String inString = "";

int position = 0;
int i = 0;

int8_t save_data[600] = { 0 };
// Most recent position in the save_data buffer
int begin_index = 0;
// True if there is not yet enough data to run inference
bool pending_initial_data = true;
int sample_every_n;
// The number of measurements since we last saved one
int sample_skip_counter = 1;
// How often we should save a measurement during downsampling
//int sample_every_n;
// The number of measurements since we last saved one
//int sample_skip_counter = 1;
//float sample_rate = 119.0;
//int sample_every_n = static_cast<int>(roundf(sample_rate / kTargetHz));

TfLiteStatus SetupAccelerometer(tflite::ErrorReporter* error_reporter) {
  // Switch on the IMU
 // if (!IMU.begin()) {
  //  TF_LITE_REPORT_ERROR(error_reporter, "Failed to initialize IMU");
   // return kTfLiteError;
//  }

  // Make sure we are pulling measurements into a FIFO.
  // If you see an error on this line, make sure you have at least v1.1.0 of the
  // Arduino_LSM9DS1 library installed.
 // IMU.setContinuousMode();

  // Determine how many measurements to keep in order to
  // meet kTargetHz
 // float sample_rate = IMU.accelerationSampleRate();
  //sample_every_n = static_cast<int>(roundf(sample_rate / kTargetHz));

//  TF_LITE_REPORT_ERROR(error_reporter, "Magic starts!");

  return kTfLiteOk;
}


bool ReadAccelerometer(tflite::ErrorReporter* error_reporter, int8_t* input,
                       //bool ReadAccelerometer(tflite::ErrorReporter* error_reporter, float* input,
                       int length) {
  // Keep track of whether we stored any new data

  // Keep track of whether we stored any new data
  bool new_data = false;

  // Loop through new samples and add to buffer
  // float x, y, z;
  while (1) {

    int8_t x, y, z;
    //float xx,yy,zz;
     // if (!IMU.readAcceleration(xx, yy, zz)) {
      //TF_LITE_REPORT_ERROR(error_reporter, "Failed to read data");
      //break;
    
    x = test_data[position][0];
    y = test_data[position][1];
    z = test_data[position][2];
    position++;
    if (position == test_data_size) {
      position = 0;
    }

   
    const int8_t norm_x = z * -1;
    const int8_t norm_y = y;
    const int8_t norm_z = x;

 //   Serial.print(norm_x);
  //  Serial.print("        ");
   // Serial.print(norm_y);
    //Serial.print("        ");
  //  Serial.println(norm_z);

    save_data[begin_index++] = norm_x;
    save_data[begin_index++] = norm_y;
    save_data[begin_index++] = norm_z;
  
    // Since we took a sample, reset the skip counter
    //  sample_skip_counter = 1;
    // If we reached the end of the circle buffer, reset
    if (begin_index >= 600) {
      //  if (begin_index >= 4500) {
        
      begin_index = 0;
      new_data = true;
      break;
    }
    new_data = true;
  }



  // Copy the requested number of bytes to the provided input tensor
  for (int i = 0; i < length; ++i) {
    int ring_array_index = begin_index + i - length;
    if (ring_array_index < 0) {
      ring_array_index += 600;
    }
    input[i] = save_data[ring_array_index];
  }

  return 1;
}
                       
