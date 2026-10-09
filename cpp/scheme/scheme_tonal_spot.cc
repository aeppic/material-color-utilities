/*
 * Copyright 2023 Google LLC
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

#include "cpp/scheme/scheme_tonal_spot.h"

#include "cpp/cam/hct.h"
#include "cpp/dynamiccolor/dynamic_scheme.h"
#include "cpp/dynamiccolor/dynamic_scheme_palettes_2025.h"
#include "cpp/dynamiccolor/variant.h"
#include "cpp/palettes/tones.h"

namespace material_color_utilities {

namespace {

DynamicSchemeOptions LegacyOptions(Hct source, bool is_dark,
                                   double contrast_level) {
  return DynamicSchemeOptions{
      {source},
      Variant::kTonalSpot,
      contrast_level,
      is_dark,
      TonalPalette(source.get_hue(), 36.0),
      TonalPalette(source.get_hue(), 16.0),
      TonalPalette(SanitizeDegreesDouble(source.get_hue() + 60.0), 24.0),
      TonalPalette(source.get_hue(), 6.0),
      TonalPalette(source.get_hue(), 8.0)};
}

}  // namespace

SchemeTonalSpot::SchemeTonalSpot(Hct set_source_color_hct, bool set_is_dark,
                                  double set_contrast_level)
    : SchemeTonalSpot(set_source_color_hct, set_is_dark, set_contrast_level,
                      SpecVersion::k2021, Platform::kPhone) {}

SchemeTonalSpot::SchemeTonalSpot(Hct source, bool is_dark,
                                 double contrast_level,
                                 SpecVersion spec_version, Platform platform)
    : DynamicScheme(ResolveSchemeOptions2025(
          LegacyOptions(source, is_dark, contrast_level), spec_version,
          platform)) {}

SchemeTonalSpot::SchemeTonalSpot(Hct set_source_color_hct, bool set_is_dark)
    : SchemeTonalSpot::SchemeTonalSpot(set_source_color_hct, set_is_dark, 0.0) {
}

}  // namespace material_color_utilities
