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

#include <array>
#include <memory>
#include <vector>

#include "cpp/cam/hct.h"
#include "cpp/dynamiccolor/color_spec.h"
#include "cpp/dynamiccolor/dynamic_scheme.h"
#include "cpp/dynamiccolor/variant.h"
#include "cpp/palettes/tones.h"
#include "cpp/scheme/scheme_expressive.h"
#include "cpp/scheme/scheme_neutral.h"
#include "cpp/scheme/scheme_tonal_spot.h"
#include "cpp/scheme/scheme_vibrant.h"
#include <gtest/gtest.h>

namespace material_color_utilities {
namespace {

struct PaletteValue {
  double hue;
  double chroma;
};

struct PaletteFixture {
  Variant variant;
  Platform platform;
  bool is_dark;
  std::array<PaletteValue, 6> palettes;
};

std::unique_ptr<DynamicScheme> MakeScheme(const PaletteFixture& fixture) {
  Hct source(0xff6750a4);
  switch (fixture.variant) {
    case Variant::kNeutral:
      return std::make_unique<SchemeNeutral>(
          source, fixture.is_dark, 0.0, SpecVersion::k2025,
          fixture.platform);
    case Variant::kTonalSpot:
      return std::make_unique<SchemeTonalSpot>(
          source, fixture.is_dark, 0.0, SpecVersion::k2025,
          fixture.platform);
    case Variant::kExpressive:
      return std::make_unique<SchemeExpressive>(
          source, fixture.is_dark, 0.0, SpecVersion::k2025,
          fixture.platform);
    case Variant::kVibrant:
      return std::make_unique<SchemeVibrant>(
          source, fixture.is_dark, 0.0, SpecVersion::k2025,
          fixture.platform);
    default:
      return nullptr;
  }
}

TEST(DynamicSchemePalettes2025Test, MatchesTypeScriptFixtures) {
  constexpr double source_hue = 298.980997211;
  constexpr double error_hue = 12.0;
  const std::vector<PaletteFixture> fixtures = {
      {Variant::kNeutral, Platform::kPhone, false,
       {{{source_hue, 8}, {source_hue, 4}, {283.980997211, 20},
         {source_hue, 1.4}, {source_hue, 3.08}, {error_hue, 50}}}},
      {Variant::kNeutral, Platform::kPhone, true,
       {{{source_hue, 8}, {source_hue, 4}, {283.980997211, 20},
         {source_hue, 1.4}, {source_hue, 3.08}, {error_hue, 50}}}},
      {Variant::kNeutral, Platform::kWatch, false,
       {{{source_hue, 12}, {source_hue, 6}, {283.980997211, 36},
         {source_hue, 6}, {source_hue, 13.2}, {error_hue, 40}}}},
      {Variant::kNeutral, Platform::kWatch, true,
       {{{source_hue, 12}, {source_hue, 6}, {283.980997211, 36},
         {source_hue, 6}, {source_hue, 13.2}, {error_hue, 40}}}},
      {Variant::kTonalSpot, Platform::kPhone, false,
       {{{source_hue, 32}, {source_hue, 16}, {338.980997211, 28},
         {source_hue, 5}, {source_hue, 8.5}, {error_hue, 60}}}},
      {Variant::kTonalSpot, Platform::kPhone, true,
       {{{source_hue, 26}, {source_hue, 16}, {338.980997211, 28},
         {source_hue, 5}, {source_hue, 8.5}, {error_hue, 60}}}},
      {Variant::kTonalSpot, Platform::kWatch, false,
       {{{source_hue, 32}, {source_hue, 16}, {338.980997211, 32},
         {source_hue, 10}, {source_hue, 17}, {error_hue, 48}}}},
      {Variant::kTonalSpot, Platform::kWatch, true,
       {{{source_hue, 32}, {source_hue, 16}, {338.980997211, 32},
         {source_hue, 10}, {source_hue, 17}, {error_hue, 48}}}},
      {Variant::kExpressive, Platform::kPhone, false,
       {{{source_hue, 48}, {142.980997211, 24}, {138.980997211, 48},
         {308.980997211, 18}, {308.980997211, 41.4}, {error_hue, 64}}}},
      {Variant::kExpressive, Platform::kPhone, true,
       {{{source_hue, 36}, {142.980997211, 16}, {138.980997211, 48},
         {308.980997211, 14}, {308.980997211, 32.2}, {error_hue, 64}}}},
      {Variant::kExpressive, Platform::kWatch, false,
       {{{source_hue, 40}, {142.980997211, 24}, {138.980997211, 48},
         {308.980997211, 12}, {308.980997211, 27.6}, {error_hue, 48}}}},
      {Variant::kExpressive, Platform::kWatch, true,
       {{{source_hue, 40}, {142.980997211, 24}, {138.980997211, 48},
         {308.980997211, 12}, {308.980997211, 27.6}, {error_hue, 48}}}},
      {Variant::kVibrant, Platform::kPhone, false,
       {{{source_hue, 74}, {308.980997211, 56}, {0.980997211, 56},
         {308.980997211, 28}, {308.980997211, 36.12}, {error_hue, 80}}}},
      {Variant::kVibrant, Platform::kPhone, true,
       {{{source_hue, 74}, {308.980997211, 56}, {0.980997211, 56},
         {308.980997211, 28}, {308.980997211, 36.12}, {error_hue, 80}}}},
      {Variant::kVibrant, Platform::kWatch, false,
       {{{source_hue, 56}, {308.980997211, 36}, {0.980997211, 56},
         {308.980997211, 20}, {308.980997211, 25.8}, {error_hue, 60}}}},
      {Variant::kVibrant, Platform::kWatch, true,
       {{{source_hue, 56}, {308.980997211, 36}, {0.980997211, 56},
         {308.980997211, 20}, {308.980997211, 25.8}, {error_hue, 60}}}},
  };

  for (const PaletteFixture& fixture : fixtures) {
    std::unique_ptr<DynamicScheme> scheme = MakeScheme(fixture);
    ASSERT_NE(scheme, nullptr);
    const std::array<TonalPalette, 6> palettes = {
        scheme->primary_palette,         scheme->secondary_palette,
        scheme->tertiary_palette,        scheme->neutral_palette,
        scheme->neutral_variant_palette, scheme->error_palette,
    };
    for (size_t i = 0; i < palettes.size(); ++i) {
      EXPECT_NEAR(palettes[i].get_hue(), fixture.palettes[i].hue, 0.001);
      EXPECT_NEAR(palettes[i].get_chroma(), fixture.palettes[i].chroma, 0.001);
    }
  }
}

TEST(DynamicSchemePalettes2025Test, LegacyConstructorsKeep2021Defaults) {
  Hct source(0xff6750a4);
  SchemeNeutral neutral(source, false, 0.0);
  SchemeTonalSpot tonal_spot(source, false, 0.0);
  SchemeExpressive expressive(source, false, 0.0);
  SchemeVibrant vibrant(source, false, 0.0);
  const std::array<const DynamicScheme*, 4> schemes = {
      &neutral, &tonal_spot, &expressive, &vibrant};
  const std::array<std::array<double, 5>, 4> chromas = {{
      {{12, 8, 16, 2, 2}},
      {{36, 16, 24, 6, 8}},
      {{40, 24, 32, 8, 12}},
      {{200, 24, 32, 10, 12}},
  }};

  for (size_t i = 0; i < schemes.size(); ++i) {
    EXPECT_EQ(schemes[i]->spec_version, SpecVersion::k2021);
    EXPECT_EQ(schemes[i]->platform, Platform::kPhone);
    EXPECT_NEAR(schemes[i]->primary_palette.get_chroma(), chromas[i][0],
                0.001);
    EXPECT_NEAR(schemes[i]->secondary_palette.get_chroma(), chromas[i][1],
                0.001);
    EXPECT_NEAR(schemes[i]->tertiary_palette.get_chroma(), chromas[i][2],
                0.001);
    EXPECT_NEAR(schemes[i]->neutral_palette.get_chroma(), chromas[i][3],
                0.001);
    EXPECT_NEAR(schemes[i]->neutral_variant_palette.get_chroma(),
                chromas[i][4], 0.001);
  }
}

TEST(DynamicSchemePalettes2025Test, UsesLowerInclusiveUpperExclusiveRanges) {
  Hct source(0xff6750a4);
  double source_hue = source.get_hue();

  EXPECT_EQ(DynamicScheme::GetPiecewiseHue(
                source, {0, source_hue, 360}, {12, 22}),
            22.0);
  EXPECT_NEAR(DynamicScheme::GetRotatedHue(
                  source, {0, source_hue, 360}, {10, 20}),
              SanitizeDegreesDouble(source_hue + 20.0), 0.001);
}

TEST(DynamicSchemePalettes2025Test, UsesBlueSpecificNeutralPalettes) {
  Hct source(255.0, 50.0, 50.0);
  ASSERT_TRUE(Hct::IsBlue(source.get_hue()));
  SchemeNeutral neutral_phone(source, false, 0.0, SpecVersion::k2025,
                              Platform::kPhone);
  SchemeNeutral neutral_watch(source, false, 0.0, SpecVersion::k2025,
                              Platform::kWatch);
  SchemeVibrant vibrant_watch(source, false, 0.0, SpecVersion::k2025,
                              Platform::kWatch);

  EXPECT_NEAR(neutral_phone.primary_palette.get_chroma(), 12.0, 0.001);
  EXPECT_NEAR(neutral_phone.secondary_palette.get_chroma(), 6.0, 0.001);
  EXPECT_NEAR(neutral_watch.primary_palette.get_chroma(), 16.0, 0.001);
  EXPECT_NEAR(neutral_watch.secondary_palette.get_chroma(), 10.0, 0.001);
  EXPECT_NEAR(vibrant_watch.neutral_palette.get_chroma(), 28.0, 0.001);
}

}  // namespace
}  // namespace material_color_utilities
