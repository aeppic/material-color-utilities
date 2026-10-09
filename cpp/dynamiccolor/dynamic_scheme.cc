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

#include "cpp/dynamiccolor/dynamic_scheme.h"

#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "cpp/blend/blend.h"
#include "cpp/cam/hct.h"
#include "cpp/dynamiccolor/color_spec.h"
#include "cpp/dynamiccolor/material_dynamic_colors.h"
#include "cpp/dynamiccolor/variant.h"
#include "cpp/palettes/tones.h"
#include "cpp/utils/utils.h"

namespace material_color_utilities {

namespace {

Hct PrimarySourceColor(const std::vector<Hct>& source_colors) {
  if (source_colors.empty()) {
    throw std::invalid_argument("source_color_hcts cannot be empty");
  }
  return source_colors.front();
}

SpecVersion ResolveSpecVersion(SpecVersion spec_version, Variant variant) {
  if (variant == Variant::kCmf) {
    return spec_version;
  }
  if (variant == Variant::kExpressive || variant == Variant::kVibrant ||
      variant == Variant::kTonalSpot || variant == Variant::kNeutral) {
    return spec_version == SpecVersion::k2026 ? SpecVersion::k2025
                                              : spec_version;
  }
  return SpecVersion::k2021;
}

std::optional<CmfProfile> ResolveProfile(
    std::optional<CmfProfile> profile, Variant variant,
    SpecVersion spec_version) {
  if (profile.has_value() &&
      (variant != Variant::kCmf || spec_version != SpecVersion::k2026)) {
    throw std::invalid_argument(
        "CMF profiles require the CMF variant and spec version 2026");
  }
  if (variant == Variant::kCmf && spec_version == SpecVersion::k2026) {
    return profile.value_or(CmfProfile::k2026);
  }
  return std::nullopt;
}

std::optional<double> ResolveNeutralChromaPercent(
    std::optional<double> value, std::optional<CmfProfile> profile) {
  if (value.has_value() && profile != CmfProfile::k2026Custom) {
    throw std::invalid_argument(
        "neutral_chroma_percent requires the custom CMF 2026 profile");
  }
  if (profile != CmfProfile::k2026Custom) {
    return std::nullopt;
  }
  double resolved =
      value.value_or(DynamicScheme::kDefaultCmfNeutralChromaPercent);
  if (!std::isfinite(resolved) || resolved < 0.0 || resolved > 100.0) {
    throw std::out_of_range(
        "neutral_chroma_percent must be finite and between 0 and 100");
  }
  return resolved;
}

std::optional<double> ResolveNeutralChromaCap(
    std::optional<double> value, std::optional<CmfProfile> profile) {
  if (value.has_value() && profile != CmfProfile::k2026Custom) {
    throw std::invalid_argument(
        "neutral_chroma_cap requires the custom CMF 2026 profile");
  }
  if (profile != CmfProfile::k2026Custom) {
    return std::nullopt;
  }
  double resolved = value.value_or(DynamicScheme::kDefaultCmfNeutralChromaCap);
  if (!std::isfinite(resolved) || resolved < 0.0) {
    throw std::out_of_range(
        "neutral_chroma_cap must be finite and non-negative");
  }
  return resolved;
}

DynamicSchemeOptions LegacyOptions(
    Hct source_color_hct, Variant variant, double contrast_level, bool is_dark,
    TonalPalette primary_palette, TonalPalette secondary_palette,
    TonalPalette tertiary_palette, TonalPalette neutral_palette,
    TonalPalette neutral_variant_palette,
    std::optional<TonalPalette> error_palette) {
  return DynamicSchemeOptions{
      {source_color_hct},       variant,
      contrast_level,          is_dark,
      primary_palette,         secondary_palette,
      tertiary_palette,        neutral_palette,
      neutral_variant_palette, error_palette,
  };
}

}  // namespace

DynamicScheme::DynamicScheme(Hct source_color_hct, Variant variant,
                             double contrast_level, bool is_dark,
                             TonalPalette primary_palette,
                             TonalPalette secondary_palette,
                             TonalPalette tertiary_palette,
                             TonalPalette neutral_palette,
                              TonalPalette neutral_variant_palette,
                              std::optional<TonalPalette> error_palette)
    : DynamicScheme(LegacyOptions(
          source_color_hct, variant, contrast_level, is_dark, primary_palette,
          secondary_palette, tertiary_palette, neutral_palette,
          neutral_variant_palette, error_palette)) {}

DynamicScheme::DynamicScheme(DynamicSchemeOptions options)
    : source_color_hct(PrimarySourceColor(options.source_color_hcts)),
      source_color_hcts(options.source_color_hcts),
      variant(options.variant),
      is_dark(options.is_dark),
      contrast_level(options.contrast_level),
      platform(options.platform),
      spec_version(ResolveSpecVersion(options.spec_version, options.variant)),
      profile(ResolveProfile(options.profile, options.variant, spec_version)),
      neutral_chroma_percent(ResolveNeutralChromaPercent(
          options.neutral_chroma_percent, profile)),
      neutral_chroma_cap(
          ResolveNeutralChromaCap(options.neutral_chroma_cap, profile)),
      primary_palette(options.primary_palette),
      secondary_palette(options.secondary_palette),
      tertiary_palette(options.tertiary_palette),
      neutral_palette(options.neutral_palette),
      neutral_variant_palette(options.neutral_variant_palette),
      error_palette(
          options.error_palette.value_or(TonalPalette(25.0, 84.0))) {
  for (ExtendedColor& extended_color : options.extended_colors) {
    AddExtendedColor(std::move(extended_color));
  }
}

void DynamicScheme::AddExtendedColor(ExtendedColor extended_color) {
  Hct source_color = extended_color.harmonize
                         ? Hct(BlendHarmonize(extended_color.color.ToInt(),
                                             source_color_hct.ToInt()))
                         : extended_color.color;
  extended_palette.insert_or_assign(extended_color.name,
                                    TonalPalette(source_color));
  raw_extended_colors.push_back(std::move(extended_color));
}

double DynamicScheme::GetRotatedHue(Hct source_color, std::vector<double> hues,
                                    std::vector<double> rotations) {
  size_t size = hues.empty() ? 0 : std::min(hues.size() - 1, rotations.size());
  if (size == 0) return source_color.get_hue();
  double rotation = GetPiecewiseHue(source_color, std::move(hues),
                                    std::move(rotations));
  return SanitizeDegreesDouble(source_color.get_hue() + rotation);
}

double DynamicScheme::GetPiecewiseHue(Hct source_color,
                                      std::vector<double> hue_breakpoints,
                                      std::vector<double> hues) {
  size_t size = hue_breakpoints.empty()
                    ? 0
                    : std::min(hue_breakpoints.size() - 1, hues.size());
  double source_hue = source_color.get_hue();
  for (size_t i = 0; i < size; ++i) {
    if (source_hue >= hue_breakpoints[i] &&
        source_hue < hue_breakpoints[i + 1]) {
      return SanitizeDegreesDouble(hues[i]);
    }
  }
  return source_hue;
}

Argb DynamicScheme::SourceColorArgb() const { return source_color_hct.ToInt(); }

Argb DynamicScheme::GetPrimaryPaletteKeyColor() const {
  return MaterialDynamicColors::PrimaryPaletteKeyColor().GetArgb(*this);
}

Argb DynamicScheme::GetSecondaryPaletteKeyColor() const {
  return MaterialDynamicColors::SecondaryPaletteKeyColor().GetArgb(*this);
}

Argb DynamicScheme::GetTertiaryPaletteKeyColor() const {
  return MaterialDynamicColors::TertiaryPaletteKeyColor().GetArgb(*this);
}

Argb DynamicScheme::GetNeutralPaletteKeyColor() const {
  return MaterialDynamicColors::NeutralPaletteKeyColor().GetArgb(*this);
}

Argb DynamicScheme::GetNeutralVariantPaletteKeyColor() const {
  return MaterialDynamicColors::NeutralVariantPaletteKeyColor().GetArgb(*this);
}

Argb DynamicScheme::GetErrorPaletteKeyColor() const {
  return MaterialDynamicColors::ErrorPaletteKeyColor().GetArgb(*this);
}

Argb DynamicScheme::GetBackground() const {
  return MaterialDynamicColors::Background().GetArgb(*this);
}

Argb DynamicScheme::GetOnBackground() const {
  return MaterialDynamicColors::OnBackground().GetArgb(*this);
}

Argb DynamicScheme::GetSurface() const {
  return MaterialDynamicColors::Surface().GetArgb(*this);
}

Argb DynamicScheme::GetSurfaceDim() const {
  return MaterialDynamicColors::SurfaceDim().GetArgb(*this);
}

Argb DynamicScheme::GetSurfaceBright() const {
  return MaterialDynamicColors::SurfaceBright().GetArgb(*this);
}

Argb DynamicScheme::GetSurfaceContainerLowest() const {
  return MaterialDynamicColors::SurfaceContainerLowest().GetArgb(*this);
}

Argb DynamicScheme::GetSurfaceContainerLow() const {
  return MaterialDynamicColors::SurfaceContainerLow().GetArgb(*this);
}

Argb DynamicScheme::GetSurfaceContainer() const {
  return MaterialDynamicColors::SurfaceContainer().GetArgb(*this);
}

Argb DynamicScheme::GetSurfaceContainerHigh() const {
  return MaterialDynamicColors::SurfaceContainerHigh().GetArgb(*this);
}

Argb DynamicScheme::GetSurfaceContainerHighest() const {
  return MaterialDynamicColors::SurfaceContainerHighest().GetArgb(*this);
}

Argb DynamicScheme::GetOnSurface() const {
  return MaterialDynamicColors::OnSurface().GetArgb(*this);
}

Argb DynamicScheme::GetSurfaceVariant() const {
  return MaterialDynamicColors::SurfaceVariant().GetArgb(*this);
}

Argb DynamicScheme::GetOnSurfaceVariant() const {
  return MaterialDynamicColors::OnSurfaceVariant().GetArgb(*this);
}

Argb DynamicScheme::GetInverseSurface() const {
  return MaterialDynamicColors::InverseSurface().GetArgb(*this);
}

Argb DynamicScheme::GetInverseOnSurface() const {
  return MaterialDynamicColors::InverseOnSurface().GetArgb(*this);
}

Argb DynamicScheme::GetOutline() const {
  return MaterialDynamicColors::Outline().GetArgb(*this);
}

Argb DynamicScheme::GetOutlineVariant() const {
  return MaterialDynamicColors::OutlineVariant().GetArgb(*this);
}

Argb DynamicScheme::GetShadow() const {
  return MaterialDynamicColors::Shadow().GetArgb(*this);
}

Argb DynamicScheme::GetScrim() const {
  return MaterialDynamicColors::Scrim().GetArgb(*this);
}

Argb DynamicScheme::GetSurfaceTint() const {
  return MaterialDynamicColors::SurfaceTint().GetArgb(*this);
}

Argb DynamicScheme::GetPrimary() const {
  return MaterialDynamicColors::Primary().GetArgb(*this);
}

Argb DynamicScheme::GetPrimaryDim() const {
  if (spec_version == SpecVersion::k2021) {
    throw std::logic_error("primary_dim is undefined before spec 2025");
  }
  return MaterialDynamicColors::PrimaryDim().GetArgb(*this);
}

Argb DynamicScheme::GetOnPrimary() const {
  return MaterialDynamicColors::OnPrimary().GetArgb(*this);
}

Argb DynamicScheme::GetPrimaryContainer() const {
  return MaterialDynamicColors::PrimaryContainer().GetArgb(*this);
}

Argb DynamicScheme::GetOnPrimaryContainer() const {
  return MaterialDynamicColors::OnPrimaryContainer().GetArgb(*this);
}

Argb DynamicScheme::GetInversePrimary() const {
  return MaterialDynamicColors::InversePrimary().GetArgb(*this);
}

Argb DynamicScheme::GetSecondary() const {
  return MaterialDynamicColors::Secondary().GetArgb(*this);
}

Argb DynamicScheme::GetSecondaryDim() const {
  if (spec_version == SpecVersion::k2021) {
    throw std::logic_error("secondary_dim is undefined before spec 2025");
  }
  return MaterialDynamicColors::SecondaryDim().GetArgb(*this);
}

Argb DynamicScheme::GetOnSecondary() const {
  return MaterialDynamicColors::OnSecondary().GetArgb(*this);
}

Argb DynamicScheme::GetSecondaryContainer() const {
  return MaterialDynamicColors::SecondaryContainer().GetArgb(*this);
}

Argb DynamicScheme::GetOnSecondaryContainer() const {
  return MaterialDynamicColors::OnSecondaryContainer().GetArgb(*this);
}

Argb DynamicScheme::GetTertiary() const {
  return MaterialDynamicColors::Tertiary().GetArgb(*this);
}

Argb DynamicScheme::GetTertiaryDim() const {
  if (spec_version == SpecVersion::k2021) {
    throw std::logic_error("tertiary_dim is undefined before spec 2025");
  }
  return MaterialDynamicColors::TertiaryDim().GetArgb(*this);
}

Argb DynamicScheme::GetOnTertiary() const {
  return MaterialDynamicColors::OnTertiary().GetArgb(*this);
}

Argb DynamicScheme::GetTertiaryContainer() const {
  return MaterialDynamicColors::TertiaryContainer().GetArgb(*this);
}

Argb DynamicScheme::GetOnTertiaryContainer() const {
  return MaterialDynamicColors::OnTertiaryContainer().GetArgb(*this);
}

Argb DynamicScheme::GetError() const {
  return MaterialDynamicColors::Error().GetArgb(*this);
}

Argb DynamicScheme::GetErrorDim() const {
  if (spec_version == SpecVersion::k2021) {
    throw std::logic_error("error_dim is undefined before spec 2025");
  }
  return MaterialDynamicColors::ErrorDim().GetArgb(*this);
}

Argb DynamicScheme::GetOnError() const {
  return MaterialDynamicColors::OnError().GetArgb(*this);
}

Argb DynamicScheme::GetErrorContainer() const {
  return MaterialDynamicColors::ErrorContainer().GetArgb(*this);
}

Argb DynamicScheme::GetOnErrorContainer() const {
  return MaterialDynamicColors::OnErrorContainer().GetArgb(*this);
}

Argb DynamicScheme::GetPrimaryFixed() const {
  return MaterialDynamicColors::PrimaryFixed().GetArgb(*this);
}

Argb DynamicScheme::GetPrimaryFixedDim() const {
  return MaterialDynamicColors::PrimaryFixedDim().GetArgb(*this);
}

Argb DynamicScheme::GetOnPrimaryFixed() const {
  return MaterialDynamicColors::OnPrimaryFixed().GetArgb(*this);
}

Argb DynamicScheme::GetOnPrimaryFixedVariant() const {
  return MaterialDynamicColors::OnPrimaryFixedVariant().GetArgb(*this);
}

Argb DynamicScheme::GetSecondaryFixed() const {
  return MaterialDynamicColors::SecondaryFixed().GetArgb(*this);
}

Argb DynamicScheme::GetSecondaryFixedDim() const {
  return MaterialDynamicColors::SecondaryFixedDim().GetArgb(*this);
}

Argb DynamicScheme::GetOnSecondaryFixed() const {
  return MaterialDynamicColors::OnSecondaryFixed().GetArgb(*this);
}

Argb DynamicScheme::GetOnSecondaryFixedVariant() const {
  return MaterialDynamicColors::OnSecondaryFixedVariant().GetArgb(*this);
}

Argb DynamicScheme::GetTertiaryFixed() const {
  return MaterialDynamicColors::TertiaryFixed().GetArgb(*this);
}

Argb DynamicScheme::GetTertiaryFixedDim() const {
  return MaterialDynamicColors::TertiaryFixedDim().GetArgb(*this);
}

Argb DynamicScheme::GetOnTertiaryFixed() const {
  return MaterialDynamicColors::OnTertiaryFixed().GetArgb(*this);
}

Argb DynamicScheme::GetOnTertiaryFixedVariant() const {
  return MaterialDynamicColors::OnTertiaryFixedVariant().GetArgb(*this);
}

Argb DynamicScheme::GetErrorFixed() const {
  return MaterialDynamicColors::ErrorFixed().GetArgb(*this);
}

Argb DynamicScheme::GetErrorFixedDim() const {
  return MaterialDynamicColors::ErrorFixedDim().GetArgb(*this);
}

Argb DynamicScheme::GetOnErrorFixed() const {
  return MaterialDynamicColors::OnErrorFixed().GetArgb(*this);
}

Argb DynamicScheme::GetOnErrorFixedVariant() const {
  return MaterialDynamicColors::OnErrorFixedVariant().GetArgb(*this);
}

Argb DynamicScheme::GetInverseError() const {
  return MaterialDynamicColors::InverseError().GetArgb(*this);
}

Argb DynamicScheme::GetExtended(const std::string& name) const {
  return MaterialDynamicColors::Extended(name).GetArgb(*this);
}

Argb DynamicScheme::GetExtendedDim(const std::string& name) const {
  return MaterialDynamicColors::ExtendedDim(name).GetArgb(*this);
}

Argb DynamicScheme::GetOnExtended(const std::string& name) const {
  return MaterialDynamicColors::OnExtended(name).GetArgb(*this);
}

Argb DynamicScheme::GetExtendedContainer(const std::string& name) const {
  return MaterialDynamicColors::ExtendedContainer(name).GetArgb(*this);
}

Argb DynamicScheme::GetOnExtendedContainer(const std::string& name) const {
  return MaterialDynamicColors::OnExtendedContainer(name).GetArgb(*this);
}

Argb DynamicScheme::GetExtendedFixed(const std::string& name) const {
  return MaterialDynamicColors::ExtendedFixed(name).GetArgb(*this);
}

Argb DynamicScheme::GetExtendedFixedDim(const std::string& name) const {
  return MaterialDynamicColors::ExtendedFixedDim(name).GetArgb(*this);
}

Argb DynamicScheme::GetOnExtendedFixed(const std::string& name) const {
  return MaterialDynamicColors::OnExtendedFixed(name).GetArgb(*this);
}

Argb DynamicScheme::GetOnExtendedFixedVariant(
    const std::string& name) const {
  return MaterialDynamicColors::OnExtendedFixedVariant(name).GetArgb(*this);
}

Argb DynamicScheme::GetInverseExtended(const std::string& name) const {
  return MaterialDynamicColors::InverseExtended(name).GetArgb(*this);
}

}  // namespace material_color_utilities
