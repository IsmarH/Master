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

#include <TensorFlowLite.h>
#include <Arduino.h>
#include "main_functions.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
//#include "accelerometer_handler.h"
#include "accelerometer_handler_serial.h"
#include "constants.h"
#include "gesture_predictor.h"
#include "magic_wand_model_data.h"
#include "output_handler.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
//#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
//#define LED_PWR (25u)
#define kChannelNumber 3
#include "tensorflow/lite/c/common.h"
#include <Arduino.h>
//#include <Arduino_LSM9DS1.h>
#define BAUD_RATE_SERIAL_DATA   (115200)

//#include <Arduino_LSM6DSOX.h>

// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* model_input = nullptr;
int input_length;

// Create an area of memory to use for input, output, and intermediate arrays.
// The size of this will depend on the model you're using, and may need to be
// determined by experimentation.
constexpr int kTensorArenaSize = 30 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

// The name of this function is important for Arduino compatibility.
void setup() {
  // Serial.begin(115200);
 //Serial.setTimeout(1);
 //  Serial1.begin(BAUD_RATE_SERIAL_DATA);  for serial mode only
// Serial1.setTimeout(1);
 // pinMode(LED_PWR, OUTPUT);
 // digitalWrite(LED_PWR, LOW);




  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  static tflite::MicroErrorReporter micro_error_reporter;  // NOLINT
  error_reporter = &micro_error_reporter;
 // while (!Serial) {
 // }
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
//  Serial.println("at least this works");
  model = tflite::GetModel(g_magic_wand_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }
//  Serial.println("second");
  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  //static tflite::AllOpsResolver resolver;

  static tflite::MicroMutableOpResolver<6> micro_op_resolver;  // NOLINT
  micro_op_resolver.AddConv2D();
  micro_op_resolver.AddDepthwiseConv2D();
  micro_op_resolver.AddFullyConnected();
  micro_op_resolver.AddMaxPool2D();
  micro_op_resolver.AddSoftmax();
  micro_op_resolver.AddReshape();
  micro_op_resolver.AddDequantize();
 // Serial.println("third");
  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
    //  model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
    model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;
 // Serial.println("fourth");
  // Allocate memory from the tensor_arena for the model's tensors.
  interpreter->AllocateTensors();
//  Serial.println("fifth");
  // Obtain pointer to the model's input tensor.

  TfLiteTensor* model_output = interpreter->output(0);

  //if (model_output->dims->size != 4)  {
  //  TF_LITE_REPORT_ERROR(error_reporter,
  //                       "Bad output tensor parameters in model1");
  //  return;
  //    }
   //   if (model_output->dims->data[0] != 1) {
    //      TF_LITE_REPORT_ERROR(error_reporter,
     //                    "Bad output tensor parameters in model2");
   // return;
    //  }
     // if (model_output->dims->data[1] != kChannelNumber) {
      //    TF_LITE_REPORT_ERROR(error_reporter,
       //                  "Bad output tensor parameters in model3");
    //return;
    //  }
    //  if (model_output->type != kTfLiteInt8){
    //      TF_LITE_REPORT_ERROR(error_reporter,
     //                    "Bad output tensor parameters in model4");
    //return;
    //  }

  model_input = interpreter->input(0);
  if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
      // (model_input->dims->data[1] != 128) ||
      (model_input->dims->data[1] != 128) || (model_input->dims->data[2] != kChannelNumber) || (model_input->type != kTfLiteInt8)) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Bad input tensor parameters in model");
    return;
  }
 // Serial.println("sixth");
  input_length = model_input->bytes / sizeof(int8_t);

 TfLiteStatus setup_status = SetupAccelerometer(error_reporter);
  if (setup_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "Set up failed\n");
  }
 // Serial.print("setup OK!");
}

void loop() {
  // Attempt to read new data from the accelerometer.
  int got_data =
   // ReadAccelerometer(error_reporter, model_input->data.f, input_length);
       ReadAccelerometer(error_reporter, model_input->data.int8, input_length);
  // If there was no new data, wait until next time.
  if (!got_data) return;

  // Run inference, and report any error.
  //delay(9999999999999);
  int printTime = millis();
while(1){
  TfLiteStatus invoke_status = interpreter->Invoke();
}
 TfLiteStatus invoke_status = interpreter->Invoke();
// Serial.print("inference was done");
  int time_difference = millis() - printTime;
 // Serial.print(" Inference time took: ");
 // Serial.print(time_difference);
 // Serial.println(" milliseconds.");
  if (invoke_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed on index: %d\n",
                         begin_index);
    return;
  }

  // Analyze the results to obtain a prediction
  int gesture_index = PredictGesture(interpreter->output(0)->data.int8);

  // Produce an output
  HandleOutput(error_reporter, gesture_index);
}