/*
 * Copyright (c) 2024 EdgeImpulse Inc.
 *
 * Generated by Edge Impulse and licensed under the applicable Edge Impulse
 * Terms of Service. Community and Professional Terms of Service
 * (https://docs.edgeimpulse.com/page/terms-of-service) or Enterprise Terms of
 * Service (https://docs.edgeimpulse.com/page/enterprise-terms-of-service),
 * according to your product plan subscription (the “License”).
 *
 * This software, documentation and other associated files (collectively referred
 * to as the “Software”) is a single SDK variation generated by the Edge Impulse
 * platform and requires an active paid Edge Impulse subscription to use this
 * Software for any purpose.
 *
 * You may NOT use this Software unless you have an active Edge Impulse subscription
 * that meets the eligibility requirements for the applicable License, subject to
 * your full and continued compliance with the terms and conditions of the License,
 * including without limitation any usage restrictions under the applicable License.
 *
 * If you do not have an active Edge Impulse product plan subscription, or if use
 * of this Software exceeds the usage limitations of your Edge Impulse product plan
 * subscription, you are not permitted to use this Software and must immediately
 * delete and erase all copies of this Software within your control or possession.
 * Edge Impulse reserves all rights and remedies available to enforce its rights.
 *
 * Unless required by applicable law or agreed to in writing, the Software is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing
 * permissions, disclaimers and limitations under the License.
 */
// Generated on: 26.06.2024 17:34:34

#ifndef tflite_learn_8_GEN_H
#define tflite_learn_8_GEN_H

#include "edge-impulse-sdk/tensorflow/lite/c/common.h"

// Sets up the model with init and prepare steps.
TfLiteStatus tflite_learn_8_init( void*(*alloc_fnc)(size_t,size_t) );
// Returns the input tensor with the given index.
TfLiteStatus tflite_learn_8_input(int index, TfLiteTensor* tensor);
// Returns the output tensor with the given index.
TfLiteStatus tflite_learn_8_output(int index, TfLiteTensor* tensor);
// Runs inference for the model.
TfLiteStatus tflite_learn_8_invoke();
//Frees memory allocated
TfLiteStatus tflite_learn_8_reset( void (*free)(void* ptr) );


// Returns the number of input tensors.
inline size_t tflite_learn_8_inputs() {
  return 1;
}
// Returns the number of output tensors.
inline size_t tflite_learn_8_outputs() {
  return 1;
}

#endif
