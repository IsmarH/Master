#define BAUD_RATE_SERIAL_DATA (115200)
#include "accelerometer_handler_serial.h"
#define RING_BUFFER_SIZE 600

#include <Arduino.h>

#include "magic_wand_model_data.h"
#include "constants.h"

#define BAUD_RATE_SERIAL_DEBUG (115200 * 8)

String inString = "";
int accelData[4];

int i = 0;

float save_data[600] = { 0.0 }; // this can be float as it still works for a quantized model 
// Most recent position in the save_data buffer
int begin_index = 0;
// True if there is not yet enough data to run inference
bool pending_initial_data = true;
int sample_every_n;
// The number of measurements since we last saved one
int sample_skip_counter = 1;
//int8_t x, y, z;
float x, y, z;
// How often we should save a measurement during downsampling
//int sample_every_n;
// The number of measurements since we last saved one
//int sample_skip_counter = 1;
//float sample_rate = 119.0;
//int sample_every_n = static_cast<int>(roundf(sample_rate / kTargetHz));

TfLiteStatus SetupAccelerometer(tflite::ErrorReporter* error_reporter) {
  return kTfLiteOk;
}


bool ReadAccelerometer(tflite::ErrorReporter* error_reporter, float* input,
                       //bool ReadAccelerometer(tflite::ErrorReporter* error_reporter, float* input,
                       int length) {

  bool new_data = false;

  while (Serial.available() > 0) {

    //if (Serial.available()) {
    int inChar = Serial.read();

    if (isDigit(inChar) or inChar == '-') {

      inString += (char)inChar;
    }

    if (inChar == ',' or inChar == '\n') {
      accelData[i] = inString.toInt();
      i += 1;
      inString = "";
      if (i == 4) {
      
        i = 0;
        x = accelData[1];
        y = accelData[2];
        z = accelData[3];
        new_data = true;
        break;
      }

      // }
      //break;
    }
    if (!new_data) {
      //return false;
      continue;
    }
  }
  if (new_data) {
    const float norm_x = x/10;  //do not change x and z for read out data from serial 
    const float norm_y = y/10;
    const float norm_z = z/10;

    save_data[begin_index++] = norm_x;
    save_data[begin_index++] = norm_y;
    save_data[begin_index++] = norm_z;


        Serial.print("Number=");
        Serial.print(int(accelData[0]));
        Serial.print(", x=");
        Serial.print(norm_x);
        Serial.print(", y=");
        Serial.print(norm_y);
        Serial.print(", z=");
        Serial.println(norm_z);
  }
  if (begin_index >= 600) {

    begin_index = 0;
  }
  if (!new_data) {
    return false;
  }
  new_data = false;

  if (pending_initial_data && begin_index >= 200) {
    pending_initial_data = false;
  }

  if (pending_initial_data) {
    return false;
  }
  for (int i = 0; i < length; ++i) {
    int ring_array_index = begin_index + i - length;
    if (ring_array_index < 0) {
      ring_array_index += 600;
    }
    input[i] = save_data[ring_array_index];
  }

  return 1;
}