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
//#include <TensorFlowLite_ESP32.h>

//#include "tensorflow/lite/micro/system_setup.h"
//#include "main_functions.h"
//#include "Nicla_System.h"
#include "Arduino.h"
//#include "tensorflow/lite/micro/all_ops_resolver.h"
//#include "Arduino_BHY2.h"
#include "accelerometer_handler_serial.h"
#include "constants.h"
#include "gesture_predictor.h"
#include "magic_wand_model_data.h"
#include "output_handler.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
//#include "tensorflow/lite/version.h"
#include "tensorflow/lite/c/common.h"
#define kChannelNumber 3
#define BAUD_RATE_SERIAL_DATA   (115200*2)

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
constexpr int kTensorArenaSize = 150 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

// The name of this function is important for Arduino compatibility.
void setup() {

 Serial.begin(BAUD_RATE_SERIAL_DATA);
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  static tflite::MicroErrorReporter micro_error_reporter;  // NOLINT
  error_reporter = &micro_error_reporter;
  while (!Serial) {
  }
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_magic_wand_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  static tflite::MicroMutableOpResolver<11> micro_op_resolver;  // NOLINT
  micro_op_resolver.AddConv2D();
  micro_op_resolver.AddDepthwiseConv2D();
  micro_op_resolver.AddFullyConnected();
  micro_op_resolver.AddMaxPool2D();
  micro_op_resolver.AddSoftmax();
  micro_op_resolver.AddReshape();
  micro_op_resolver.AddDequantize();
  micro_op_resolver.AddExpandDims();
  micro_op_resolver.AddLogistic();
  micro_op_resolver.AddElu();
   micro_op_resolver.AddQuantize();

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
    model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  interpreter->AllocateTensors();

  // Obtain pointer to the model's input tensor.
  TfLiteTensor* model_output = interpreter->output(0);
  model_input = interpreter->input(0);

  if ((model_input->dims->size != 3) || (model_input->dims->data[0] != 1) || (model_input->dims->data[1] != 50) || (model_input->dims->data[2] != kChannelNumber) || (model_input->type != kTfLiteInt8)) {  //kChannelNumber
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Bad input tensor parameters in model");
    return;
  }   

  input_length = model_input->bytes / sizeof(int8_t);

  // TfLiteStatus setup_status = SetupAccelerometer(error_reporter);
  //  if (setup_status != kTfLiteOk) {
  //   TF_LITE_REPORT_ERROR(error_reporter, "Set up failed\n");
  //}
}

void loop() {
 // Serial.begin(115200);
 // while (!Serial);
 // nicla::begin();
  //BHY2.begin();
  //accel.begin();
  // Attempt to read new data from the accelerometer.
  int got_data =
    ReadAccelerometer(error_reporter, model_input->data.int8, input_length);
  // If there was no new data, wait until next time.
  if (!got_data) return;

  // Run inference, and report any error.
int printTime = millis();

  TfLiteStatus invoke_status = interpreter->Invoke();
  int time_difference = millis() - printTime;
  Serial.print(" Inference time took: ");
  Serial.print(time_difference);
  Serial.println(" milliseconds.");
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