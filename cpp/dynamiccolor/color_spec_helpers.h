/*
 * Copyright 2026 curasystems GmbH
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CPP_DYNAMICCOLOR_COLOR_SPEC_HELPERS_H_
#define CPP_DYNAMICCOLOR_COLOR_SPEC_HELPERS_H_

#include "cpp/dynamiccolor/contrast_curve.h"
#include "cpp/palettes/tones.h"

namespace material_color_utilities {

double FindBestToneForChroma(double hue, double chroma, double tone,
                             bool by_decreasing_tone);

double TMaxC(const TonalPalette& palette, double lower_bound = 0.0,
             double upper_bound = 100.0, double chroma_multiplier = 1.0);

double TMinC(const TonalPalette& palette, double lower_bound = 0.0,
             double upper_bound = 100.0);

ContrastCurve GetCurve(double default_contrast);

}  // namespace material_color_utilities

#endif  // CPP_DYNAMICCOLOR_COLOR_SPEC_HELPERS_H_
