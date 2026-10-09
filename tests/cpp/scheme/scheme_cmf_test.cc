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
#include <stdexcept>
#include <vector>

#include "cpp/blend/blend.h"
#include "cpp/cam/hct.h"
#include "cpp/dynamiccolor/color_spec.h"
#include "cpp/dynamiccolor/dynamic_scheme.h"
#include "cpp/dynamiccolor/variant.h"
#include <gtest/gtest.h>

namespace material_color_utilities {
namespace {

TEST(SchemeCmfTest, UsesOfficial2026Defaults) {
  Hct source(0xff4f46e5);
  SchemeCmf scheme(source, false, 0.0);

  EXPECT_EQ(scheme.variant, Variant::kCmf);
  EXPECT_EQ(scheme.spec_version, SpecVersion::k2026);
  EXPECT_EQ(scheme.platform, Platform::kPhone);
  EXPECT_EQ(scheme.profile, CmfProfile::k2026);
  EXPECT_FALSE(scheme.neutral_chroma_percent.has_value());
  EXPECT_FALSE(scheme.neutral_chroma_cap.has_value());
  EXPECT_NEAR(scheme.neutral_palette.get_chroma(),
              source.get_chroma() * 0.2, 0.001);
}

TEST(SchemeCmfTest, UsesCustomNeutralDefaults) {
  Hct source(0xff4f46e5);
  SchemeCmf scheme(source, true, 0.5, Platform::kWatch,
                   CmfProfile::k2026Custom);

  EXPECT_EQ(scheme.platform, Platform::kWatch);
  EXPECT_EQ(scheme.neutral_chroma_percent, 13.0);
  EXPECT_EQ(scheme.neutral_chroma_cap, 50.0);
  EXPECT_NEAR(scheme.neutral_palette.get_chroma(),
              std::min(source.get_chroma(), 50.0) * 0.13, 0.001);
  EXPECT_NEAR(scheme.neutral_variant_palette.get_chroma(),
              scheme.neutral_palette.get_chroma(), 0.001);
}

TEST(SchemeCmfTest, UsesSecondSourceForTertiaryPalette) {
  Hct primary(0xff3d8090);
  Hct secondary(0xffffa500);
  SchemeCmf scheme(std::vector<Hct>{primary, secondary}, false, 0.0);

  ASSERT_EQ(scheme.source_color_hcts.size(), 2);
  EXPECT_EQ(scheme.source_color_hcts[1].ToInt(), secondary.ToInt());
  EXPECT_NEAR(scheme.tertiary_palette.get_hue(), secondary.get_hue(), 0.001);
  EXPECT_NEAR(scheme.tertiary_palette.get_chroma(), secondary.get_chroma(),
              0.001);
}

TEST(SchemeCmfTest, ValidatesSourcesAndNeutralOptions) {
  Hct source(0xff4f46e5);

  EXPECT_THROW(SchemeCmf(std::vector<Hct>{}, false, 0.0),
               std::invalid_argument);
  EXPECT_THROW(SchemeCmf(source, false, 0.0, Platform::kPhone,
                         CmfProfile::k2026, 13.0),
               std::invalid_argument);
  EXPECT_THROW(SchemeCmf(source, false, 0.0, Platform::kPhone,
                         CmfProfile::k2026Custom, 101.0),
               std::out_of_range);
  EXPECT_THROW(SchemeCmf(source, false, 0.0, Platform::kPhone,
                         CmfProfile::k2026Custom, 13.0, -1.0),
               std::out_of_range);
}

TEST(SchemeCmfTest, AddsHarmonizedExtendedColor) {
  Hct source(0xff4f46e5);
  Hct custom(0xffffa500);
  SchemeCmf scheme(source, false, 0.0);

  scheme.AddExtendedColor({"warning", custom, true});

  ASSERT_EQ(scheme.raw_extended_colors.size(), 1);
  ASSERT_EQ(scheme.extended_palette.size(), 1);
  Hct harmonized(BlendHarmonize(custom.ToInt(), source.ToInt()));
  EXPECT_NEAR(scheme.extended_palette.at("warning").get_hue(),
              harmonized.get_hue(), 0.001);
  EXPECT_NEAR(scheme.extended_palette.at("warning").get_chroma(),
              harmonized.get_chroma(), 0.001);
}

}  // namespace
}  // namespace material_color_utilities
