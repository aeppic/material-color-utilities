/*
 * Copyright 2026 Google LLC
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

#include "cpp/dynamiccolor/dynamic_scheme_palettes_2025.h"

#include <stdexcept>
#include <utility>
#include <vector>

#include "cpp/cam/hct.h"
#include "cpp/dynamiccolor/dynamic_scheme.h"
#include "cpp/dynamiccolor/variant.h"
#include "cpp/palettes/tones.h"

namespace material_color_utilities {
namespace {

double RotatedHue(const Hct& source, std::vector<double> breakpoints,
                  std::vector<double> rotations) {
  return DynamicScheme::GetRotatedHue(source, std::move(breakpoints),
                                      std::move(rotations));
}

double ExpressiveNeutralHue(const Hct& source) {
  return RotatedHue(source, {0, 71, 124, 253, 278, 300, 360},
                    {10, 0, 10, 0, 10, 0});
}

double ExpressiveNeutralChroma(const Hct& source, bool is_dark,
                               Platform platform) {
  double hue = ExpressiveNeutralHue(source);
  if (platform == Platform::kWatch) return 12.0;
  if (!is_dark) return 18.0;
  return Hct::IsYellow(hue) ? 6.0 : 14.0;
}

double VibrantNeutralHue(const Hct& source) {
  return RotatedHue(source, {0, 38, 105, 140, 333, 360},
                    {-14, 10, -14, 10, -14});
}

double VibrantNeutralChroma(const Hct& source, Platform platform) {
  double hue = VibrantNeutralHue(source);
  if (platform == Platform::kPhone) return 28.0;
  return Hct::IsBlue(hue) ? 28.0 : 20.0;
}

TonalPalette PrimaryPalette(Variant variant, const Hct& source, bool is_dark,
                            Platform platform) {
  double hue = source.get_hue();
  switch (variant) {
    case Variant::kNeutral:
      return TonalPalette(
          hue, platform == Platform::kPhone
                   ? Hct::IsBlue(hue) ? 12.0 : 8.0
                   : Hct::IsBlue(hue) ? 16.0 : 12.0);
    case Variant::kTonalSpot:
      return TonalPalette(
          hue, platform == Platform::kPhone && is_dark ? 26.0 : 32.0);
    case Variant::kExpressive:
      return TonalPalette(
          hue, platform == Platform::kPhone ? is_dark ? 36.0 : 48.0 : 40.0);
    case Variant::kVibrant:
      return TonalPalette(hue, platform == Platform::kPhone ? 74.0 : 56.0);
    default:
      throw std::invalid_argument("Spec 2025 palettes require an eligible variant");
  }
}

TonalPalette SecondaryPalette(Variant variant, const Hct& source, bool is_dark,
                              Platform platform) {
  double hue = source.get_hue();
  switch (variant) {
    case Variant::kNeutral:
      return TonalPalette(
          hue, platform == Platform::kPhone
                   ? Hct::IsBlue(hue) ? 6.0 : 4.0
                   : Hct::IsBlue(hue) ? 10.0 : 6.0);
    case Variant::kTonalSpot:
      return TonalPalette(hue, 16.0);
    case Variant::kExpressive:
      return TonalPalette(
          RotatedHue(source, {0, 105, 140, 204, 253, 278, 300, 333, 360},
                     {-160, 155, -100, 96, -96, -156, -165, -160}),
          platform == Platform::kPhone ? is_dark ? 16.0 : 24.0 : 24.0);
    case Variant::kVibrant:
      return TonalPalette(
          RotatedHue(source, {0, 38, 105, 140, 333, 360},
                     {-14, 10, -14, 10, -14}),
          platform == Platform::kPhone ? 56.0 : 36.0);
    default:
      throw std::invalid_argument("Spec 2025 palettes require an eligible variant");
  }
}

TonalPalette TertiaryPalette(Variant variant, const Hct& source,
                             Platform platform) {
  switch (variant) {
    case Variant::kNeutral:
      return TonalPalette(
          RotatedHue(source, {0, 38, 105, 161, 204, 278, 333, 360},
                     {-32, 26, 10, -39, 24, -15, -32}),
          platform == Platform::kPhone ? 20.0 : 36.0);
    case Variant::kTonalSpot:
      return TonalPalette(
          RotatedHue(source, {0, 20, 71, 161, 333, 360},
                     {-40, 48, -32, 40, -32}),
          platform == Platform::kPhone ? 28.0 : 32.0);
    case Variant::kExpressive:
      return TonalPalette(
          RotatedHue(source, {0, 105, 140, 204, 253, 278, 300, 333, 360},
                     {-165, 160, -105, 101, -101, -160, -170, -165}),
          48.0);
    case Variant::kVibrant:
      return TonalPalette(
          RotatedHue(source, {0, 38, 71, 105, 140, 161, 253, 333, 360},
                     {-72, 35, 24, -24, 62, 50, 62, -72}),
          56.0);
    default:
      throw std::invalid_argument("Spec 2025 palettes require an eligible variant");
  }
}

TonalPalette NeutralPalette(Variant variant, const Hct& source, bool is_dark,
                            Platform platform) {
  switch (variant) {
    case Variant::kNeutral:
      return TonalPalette(source.get_hue(),
                          platform == Platform::kPhone ? 1.4 : 6.0);
    case Variant::kTonalSpot:
      return TonalPalette(source.get_hue(),
                          platform == Platform::kPhone ? 5.0 : 10.0);
    case Variant::kExpressive:
      return TonalPalette(ExpressiveNeutralHue(source),
                          ExpressiveNeutralChroma(source, is_dark, platform));
    case Variant::kVibrant:
      return TonalPalette(VibrantNeutralHue(source),
                          VibrantNeutralChroma(source, platform));
    default:
      throw std::invalid_argument("Spec 2025 palettes require an eligible variant");
  }
}

TonalPalette NeutralVariantPalette(Variant variant, const Hct& source,
                                   bool is_dark, Platform platform) {
  if (variant == Variant::kNeutral) {
    return TonalPalette(source.get_hue(),
                        (platform == Platform::kPhone ? 1.4 : 6.0) * 2.2);
  }
  if (variant == Variant::kTonalSpot) {
    return TonalPalette(source.get_hue(),
                        (platform == Platform::kPhone ? 5.0 : 10.0) * 1.7);
  }
  if (variant == Variant::kExpressive) {
    double hue = ExpressiveNeutralHue(source);
    double chroma = ExpressiveNeutralChroma(source, is_dark, platform);
    return TonalPalette(hue, chroma * (Hct::IsYellow(hue) ? 1.6 : 2.3));
  }
  if (variant == Variant::kVibrant) {
    return TonalPalette(VibrantNeutralHue(source),
                        VibrantNeutralChroma(source, platform) * 1.29);
  }
  throw std::invalid_argument("Spec 2025 palettes require an eligible variant");
}

TonalPalette ErrorPalette(Variant variant, const Hct& source,
                          Platform platform) {
  double hue = DynamicScheme::GetPiecewiseHue(
      source, {0, 3, 13, 23, 33, 43, 153, 273, 360},
      {12, 22, 32, 12, 22, 32, 22, 12});
  double chroma;
  switch (variant) {
    case Variant::kNeutral:
      chroma = platform == Platform::kPhone ? 50.0 : 40.0;
      break;
    case Variant::kTonalSpot:
      chroma = platform == Platform::kPhone ? 60.0 : 48.0;
      break;
    case Variant::kExpressive:
      chroma = platform == Platform::kPhone ? 64.0 : 48.0;
      break;
    case Variant::kVibrant:
      chroma = platform == Platform::kPhone ? 80.0 : 60.0;
      break;
    default:
      throw std::invalid_argument("Spec 2025 palettes require an eligible variant");
  }
  return TonalPalette(hue, chroma);
}

}  // namespace

DynamicSchemeOptions ResolveSchemeOptions2025(
    DynamicSchemeOptions legacy_options, SpecVersion spec_version,
    Platform platform) {
  legacy_options.spec_version = spec_version;
  legacy_options.platform = platform;
  if (spec_version == SpecVersion::k2021) return legacy_options;

  const Hct& source = legacy_options.source_color_hcts.front();
  Variant variant = legacy_options.variant;
  bool is_dark = legacy_options.is_dark;
  legacy_options.primary_palette =
      PrimaryPalette(variant, source, is_dark, platform);
  legacy_options.secondary_palette =
      SecondaryPalette(variant, source, is_dark, platform);
  legacy_options.tertiary_palette = TertiaryPalette(variant, source, platform);
  legacy_options.neutral_palette =
      NeutralPalette(variant, source, is_dark, platform);
  legacy_options.neutral_variant_palette =
      NeutralVariantPalette(variant, source, is_dark, platform);
  legacy_options.error_palette = ErrorPalette(variant, source, platform);
  return legacy_options;
}

}  // namespace material_color_utilities
