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

#include "gesture_predictor.h"
#include <Arduino.h>
#include "constants.h"

namespace {
// State for the averaging algorithm we're using.
int8_t prediction_history[kGestureCount][kPredictionHistoryLength] = {};
int prediction_history_index = 0;
int prediction_suppression_count = 0;
}  // namespace

// Return the result of the last prediction
// 0: wing("W"), 1: ring("O"), 2: slope("angle"), 3: unknown
int PredictGesture(int8_t* output) {
  for (int i=0; i<3;i++){
          
  //  Serial.print(output[i]);
   // Serial.print(" ");
   // if (i==2)Serial.println("  ");
    
  }
 
  // Record the latest predictions in our rolling history buffer.
  for (int i = 0; i < kGestureCount; ++i) {
    prediction_history[i][prediction_history_index] = output[i];
  }
  // Figure out which slot to put the next predictions into.
  ++prediction_history_index;
  if (prediction_history_index >= kPredictionHistoryLength) {
    prediction_history_index = 0;
  }

  // Average the last n predictions for each gesture, and find which has the
  // highest score.
  float max_predict_index = 0.0;
  int max_predict_score = 0;
  for (int i = 0; i < kGestureCount; i++) {
    int prediction_sum = 0;
    for (int j = 0; j < kPredictionHistoryLength; ++j) {
      prediction_sum += prediction_history[i][j];
    }
    const float prediction_average_int = prediction_sum / kPredictionHistoryLength;
    const float prediction_average = prediction_average_int;
    if ((max_predict_index == -1) || (prediction_average > max_predict_score)) {
      max_predict_index = i;
      max_predict_score = prediction_average;
    }
  }
     Serial.print(max_predict_index);
    Serial.print(" index has an average prediction score of ");
     Serial.println(max_predict_score);
  // If there's been a recent prediction, don't trigger a new one too soon.
  if (prediction_suppression_count > 0) {
    --prediction_suppression_count;
  }
  // If we're predicting no gesture, or the average score is too low, or there's
  // been a gesture recognised too recently, return no gesture.
  if ((max_predict_index == kNoGesture) ||
      (max_predict_score < kDetectionThreshold) ||
      (prediction_suppression_count > 0)) {
    return kNoGesture;
  } else {
    // Reset the suppression counter so we don't come up with another prediction
    // too soon.
    prediction_suppression_count = kPredictionSuppressionDuration;
    return max_predict_index;
  }
}
