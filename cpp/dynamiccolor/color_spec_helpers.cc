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

#include "cpp/dynamiccolor/color_spec_helpers.h"

#include <algorithm>

#include "cpp/cam/hct.h"
#include "cpp/dynamiccolor/contrast_curve.h"
#include "cpp/palettes/tones.h"

namespace material_color_utilities {

double FindBestToneForChroma(double hue, double chroma, double tone,
                             bool by_decreasing_tone) {
  double answer = tone;
  Hct best_candidate(hue, chroma, answer);

  while (best_candidate.get_chroma() < chroma) {
    if (tone < 0.0 || tone > 100.0) {
      break;
    }

    tone += by_decreasing_tone ? -1.0 : 1.0;
    Hct new_candidate(hue, chroma, tone);
    if (best_candidate.get_chroma() < new_candidate.get_chroma()) {
      best_candidate = new_candidate;
      answer = tone;
    }
  }

  return answer;
}

double TMaxC(const TonalPalette& palette, double lower_bound,
             double upper_bound, double chroma_multiplier) {
  double answer = FindBestToneForChroma(
      palette.get_hue(), palette.get_chroma() * chroma_multiplier, 100.0, true);
  return std::clamp(answer, lower_bound, upper_bound);
}

double TMinC(const TonalPalette& palette, double lower_bound,
             double upper_bound) {
  double answer = FindBestToneForChroma(
      palette.get_hue(), palette.get_chroma(), 0.0, false);
  return std::clamp(answer, lower_bound, upper_bound);
}

ContrastCurve GetCurve(double default_contrast) {
  if (default_contrast == 1.5) {
    return ContrastCurve(1.5, 1.5, 3.0, 5.5);
  }
  if (default_contrast == 3.0) {
    return ContrastCurve(3.0, 3.0, 4.5, 7.0);
  }
  if (default_contrast == 4.5) {
    return ContrastCurve(4.5, 4.5, 7.0, 11.0);
  }
  if (default_contrast == 6.0) {
    return ContrastCurve(6.0, 6.0, 7.0, 11.0);
  }
  if (default_contrast == 7.0) {
    return ContrastCurve(7.0, 7.0, 11.0, 21.0);
  }
  if (default_contrast == 9.0) {
    return ContrastCurve(9.0, 9.0, 11.0, 21.0);
  }
  if (default_contrast == 11.0) {
    return ContrastCurve(11.0, 11.0, 21.0, 21.0);
  }
  if (default_contrast == 21.0) {
    return ContrastCurve(21.0, 21.0, 21.0, 21.0);
  }
  return ContrastCurve(default_contrast, default_contrast, 7.0, 21.0);
}

}  // namespace material_color_utilities
