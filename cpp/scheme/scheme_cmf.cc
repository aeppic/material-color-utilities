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

#include "cpp/scheme/scheme_cmf.h"

#include <algorithm>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "cpp/cam/hct.h"
#include "cpp/dynamiccolor/color_spec.h"
#include "cpp/dynamiccolor/dynamic_scheme.h"
#include "cpp/dynamiccolor/variant.h"
#include "cpp/palettes/tones.h"

namespace material_color_utilities {

namespace {

const Hct& PrimarySourceColor(const std::vector<Hct>& source_colors) {
  if (source_colors.empty()) {
    throw std::invalid_argument("SchemeCmf requires at least one source color");
  }
  return source_colors.front();
}

const Hct& SecondarySourceColor(const std::vector<Hct>& source_colors) {
  return source_colors.size() > 1 ? source_colors[1] : source_colors.front();
}

double ErrorHue(double primary_hue, double tertiary_hue) {
  if (primary_hue <= 8.0) {
    return tertiary_hue <= 24.0 ? 28.0
                                : (tertiary_hue <= 32.0 ? 16.0 : 20.0);
  }
  if (primary_hue <= 16.0) {
    return tertiary_hue <= 24.0 ? 32.0
                                : (tertiary_hue <= 32.0 ? 20.0 : 24.0);
  }
  if (primary_hue <= 20.0) {
    return tertiary_hue <= 28.0 ? 32.0
                                : (tertiary_hue <= 32.0 ? 24.0 : 28.0);
  }
  if (primary_hue <= 28.0) {
    return tertiary_hue <= 24.0 ? 32.0 : 16.0;
  }
  if (primary_hue <= 32.0) {
    return tertiary_hue <= 20.0 ? 24.0
                                : (tertiary_hue <= 28.0 ? 16.0 : 20.0);
  }
  if (primary_hue <= 40.0) {
    return tertiary_hue > 20.0 && tertiary_hue <= 28.0 ? 16.0 : 24.0;
  }
  if (primary_hue <= 152.0) {
    return tertiary_hue > 24.0 && tertiary_hue <= 36.0 ? 20.0 : 32.0;
  }
  if (primary_hue <= 272.0) {
    return tertiary_hue > 20.0 && tertiary_hue <= 28.0 ? 16.0 : 24.0;
  }
  return tertiary_hue > 12.0 && tertiary_hue <= 28.0 ? 32.0 : 16.0;
}

double NeutralChroma(const Hct& source_color, CmfProfile profile,
                     std::optional<double> neutral_chroma_percent,
                     std::optional<double> neutral_chroma_cap) {
  if (profile == CmfProfile::k2026) {
    if (neutral_chroma_percent.has_value() || neutral_chroma_cap.has_value()) {
      throw std::invalid_argument(
          "neutral chroma options require the custom CMF 2026 profile");
    }
    return source_color.get_chroma() * 0.2;
  }

  double percent = neutral_chroma_percent.value_or(
      DynamicScheme::kDefaultCmfNeutralChromaPercent);
  double cap = neutral_chroma_cap.value_or(
      DynamicScheme::kDefaultCmfNeutralChromaCap);
  if (!std::isfinite(percent) || percent < 0.0 || percent > 100.0) {
    throw std::out_of_range(
        "neutral_chroma_percent must be finite and between 0 and 100");
  }
  if (!std::isfinite(cap) || cap < 0.0) {
    throw std::out_of_range(
        "neutral_chroma_cap must be finite and non-negative");
  }
  return std::min(source_color.get_chroma(), cap) * percent / 100.0;
}

DynamicSchemeOptions CmfOptions(
    std::vector<Hct> source_colors, bool is_dark, double contrast_level,
    Platform platform, CmfProfile profile,
    std::optional<double> neutral_chroma_percent,
    std::optional<double> neutral_chroma_cap) {
  const Hct& primary = PrimarySourceColor(source_colors);
  const Hct& secondary = SecondarySourceColor(source_colors);
  double primary_hue = primary.get_hue();
  double primary_chroma = primary.get_chroma();
  double secondary_hue = secondary.get_hue();
  double secondary_chroma = secondary.get_chroma();
  bool has_distinct_secondary = primary.ToInt() != secondary.ToInt();
  double tertiary_hue =
      has_distinct_secondary ? secondary_hue : primary_hue;
  double tertiary_chroma =
      has_distinct_secondary ? secondary_chroma : primary_chroma * 0.75;
  double neutral_chroma = NeutralChroma(
      primary, profile, neutral_chroma_percent, neutral_chroma_cap);

  DynamicSchemeOptions options{
      std::move(source_colors),
      Variant::kCmf,
      contrast_level,
      is_dark,
      TonalPalette(primary_hue, primary_chroma),
      TonalPalette(primary_hue, primary_chroma * 0.5),
      TonalPalette(tertiary_hue, tertiary_chroma),
      TonalPalette(primary_hue, neutral_chroma),
      TonalPalette(primary_hue, neutral_chroma),
      TonalPalette(ErrorHue(primary_hue, tertiary_hue),
                   std::max(primary_chroma, 50.0)),
  };
  options.platform = platform;
  options.spec_version = SpecVersion::k2026;
  options.profile = profile;
  options.neutral_chroma_percent = neutral_chroma_percent;
  options.neutral_chroma_cap = neutral_chroma_cap;
  return options;
}

}  // namespace

SchemeCmf::SchemeCmf(Hct source_color_hct, bool is_dark,
                     double contrast_level, Platform platform,
                     CmfProfile profile,
                     std::optional<double> neutral_chroma_percent,
                     std::optional<double> neutral_chroma_cap)
    : SchemeCmf(std::vector<Hct>{source_color_hct}, is_dark, contrast_level,
                platform, profile, neutral_chroma_percent,
                neutral_chroma_cap) {}

SchemeCmf::SchemeCmf(std::vector<Hct> source_color_hcts, bool is_dark,
                     double contrast_level, Platform platform,
                     CmfProfile profile,
                     std::optional<double> neutral_chroma_percent,
                     std::optional<double> neutral_chroma_cap)
    : DynamicScheme(CmfOptions(
          std::move(source_color_hcts), is_dark, contrast_level, platform,
          profile, neutral_chroma_percent, neutral_chroma_cap)) {}

}  // namespace material_color_utilities
