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

#ifndef CPP_DYNAMICCOLOR_DYNAMIC_SCHEME_H_
#define CPP_DYNAMICCOLOR_DYNAMIC_SCHEME_H_

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "cpp/cam/hct.h"
#include "cpp/dynamiccolor/color_spec.h"
#include "cpp/dynamiccolor/variant.h"
#include "cpp/palettes/tones.h"
#include "cpp/utils/utils.h"

namespace material_color_utilities {

enum class Platform {
  kPhone,
  kWatch,
};

enum class CmfProfile {
  k2026,
  k2026Custom,
};

struct ExtendedColor {
  std::string name;
  Hct color;
  bool harmonize = false;
};

struct DynamicSchemeOptions {
  std::vector<Hct> source_color_hcts;
  Variant variant;
  double contrast_level;
  bool is_dark;
  TonalPalette primary_palette;
  TonalPalette secondary_palette;
  TonalPalette tertiary_palette;
  TonalPalette neutral_palette;
  TonalPalette neutral_variant_palette;
  std::optional<TonalPalette> error_palette = std::nullopt;
  Platform platform = Platform::kPhone;
  SpecVersion spec_version = SpecVersion::k2021;
  std::optional<CmfProfile> profile = std::nullopt;
  std::optional<double> neutral_chroma_percent = std::nullopt;
  std::optional<double> neutral_chroma_cap = std::nullopt;
  std::vector<ExtendedColor> extended_colors;
};

struct DynamicScheme {
  static constexpr double kDefaultCmfNeutralChromaPercent = 13.0;
  static constexpr double kDefaultCmfNeutralChromaCap = 50.0;

  Hct source_color_hct;
  std::vector<Hct> source_color_hcts;
  Variant variant;
  bool is_dark;
  double contrast_level;
  Platform platform;
  SpecVersion spec_version;
  std::optional<CmfProfile> profile;
  std::optional<double> neutral_chroma_percent;
  std::optional<double> neutral_chroma_cap;

  TonalPalette primary_palette;
  TonalPalette secondary_palette;
  TonalPalette tertiary_palette;
  TonalPalette neutral_palette;
  TonalPalette neutral_variant_palette;
  TonalPalette error_palette;
  std::vector<ExtendedColor> raw_extended_colors;
  std::unordered_map<std::string, TonalPalette> extended_palette;

  DynamicScheme(Hct source_color_hct, Variant variant, double contrast_level,
                bool is_dark, TonalPalette primary_palette,
                TonalPalette secondary_palette, TonalPalette tertiary_palette,
                TonalPalette neutral_palette,
                TonalPalette neutral_variant_palette,
                std::optional<TonalPalette> error_palette = std::nullopt);
  explicit DynamicScheme(DynamicSchemeOptions options);

  void AddExtendedColor(ExtendedColor extended_color);

  static double GetRotatedHue(Hct source_color, std::vector<double> hues,
                              std::vector<double> rotations);
  static double GetPiecewiseHue(Hct source_color,
                                std::vector<double> hue_breakpoints,
                                std::vector<double> hues);

  Argb SourceColorArgb() const;

  Argb GetPrimaryPaletteKeyColor() const;
  Argb GetSecondaryPaletteKeyColor() const;
  Argb GetTertiaryPaletteKeyColor() const;
  Argb GetNeutralPaletteKeyColor() const;
  Argb GetNeutralVariantPaletteKeyColor() const;
  Argb GetErrorPaletteKeyColor() const;
  Argb GetBackground() const;
  Argb GetOnBackground() const;
  Argb GetSurface() const;
  Argb GetSurfaceDim() const;
  Argb GetSurfaceBright() const;
  Argb GetSurfaceContainerLowest() const;
  Argb GetSurfaceContainerLow() const;
  Argb GetSurfaceContainer() const;
  Argb GetSurfaceContainerHigh() const;
  Argb GetSurfaceContainerHighest() const;
  Argb GetOnSurface() const;
  Argb GetSurfaceVariant() const;
  Argb GetOnSurfaceVariant() const;
  Argb GetInverseSurface() const;
  Argb GetInverseOnSurface() const;
  Argb GetOutline() const;
  Argb GetOutlineVariant() const;
  Argb GetShadow() const;
  Argb GetScrim() const;
  Argb GetSurfaceTint() const;
  Argb GetPrimary() const;
  Argb GetPrimaryDim() const;
  Argb GetOnPrimary() const;
  Argb GetPrimaryContainer() const;
  Argb GetOnPrimaryContainer() const;
  Argb GetInversePrimary() const;
  Argb GetSecondary() const;
  Argb GetSecondaryDim() const;
  Argb GetOnSecondary() const;
  Argb GetSecondaryContainer() const;
  Argb GetOnSecondaryContainer() const;
  Argb GetTertiary() const;
  Argb GetTertiaryDim() const;
  Argb GetOnTertiary() const;
  Argb GetTertiaryContainer() const;
  Argb GetOnTertiaryContainer() const;
  Argb GetError() const;
  Argb GetErrorDim() const;
  Argb GetOnError() const;
  Argb GetErrorContainer() const;
  Argb GetOnErrorContainer() const;
  Argb GetPrimaryFixed() const;
  Argb GetPrimaryFixedDim() const;
  Argb GetOnPrimaryFixed() const;
  Argb GetOnPrimaryFixedVariant() const;
  Argb GetSecondaryFixed() const;
  Argb GetSecondaryFixedDim() const;
  Argb GetOnSecondaryFixed() const;
  Argb GetOnSecondaryFixedVariant() const;
  Argb GetTertiaryFixed() const;
  Argb GetTertiaryFixedDim() const;
  Argb GetOnTertiaryFixed() const;
  Argb GetOnTertiaryFixedVariant() const;
  Argb GetErrorFixed() const;
  Argb GetErrorFixedDim() const;
  Argb GetOnErrorFixed() const;
  Argb GetOnErrorFixedVariant() const;
  Argb GetInverseError() const;
  Argb GetExtended(const std::string& name) const;
  Argb GetExtendedDim(const std::string& name) const;
  Argb GetOnExtended(const std::string& name) const;
  Argb GetExtendedContainer(const std::string& name) const;
  Argb GetOnExtendedContainer(const std::string& name) const;
  Argb GetExtendedFixed(const std::string& name) const;
  Argb GetExtendedFixedDim(const std::string& name) const;
  Argb GetOnExtendedFixed(const std::string& name) const;
  Argb GetOnExtendedFixedVariant(const std::string& name) const;
  Argb GetInverseExtended(const std::string& name) const;
};

}  // namespace material_color_utilities

#endif  // CPP_DYNAMICCOLOR_DYNAMIC_SCHEME_H_
