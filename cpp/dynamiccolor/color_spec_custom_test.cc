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

#include "cpp/dynamiccolor/dynamic_scheme.h"

#include <array>
#include <stdexcept>

#include "cpp/cam/hct.h"
#include "cpp/dynamiccolor/color_spec.h"
#include "cpp/scheme/scheme_cmf.h"
#include "cpp/scheme/scheme_tonal_spot.h"
#include <gtest/gtest.h>

namespace material_color_utilities {
namespace {

using ExpectedColors = std::array<Argb, 17>;

void ExpectColors(DynamicScheme& scheme, const ExpectedColors& expected) {
  EXPECT_EQ(scheme.GetErrorFixed(), expected[0]);
  EXPECT_EQ(scheme.GetErrorFixedDim(), expected[1]);
  EXPECT_EQ(scheme.GetOnErrorFixed(), expected[2]);
  EXPECT_EQ(scheme.GetOnErrorFixedVariant(), expected[3]);
  EXPECT_EQ(scheme.GetInverseError(), expected[4]);
  EXPECT_EQ(scheme.GetExtended("warning"), expected[5]);
  EXPECT_EQ(scheme.GetExtendedDim("warning"), expected[6]);
  EXPECT_EQ(scheme.GetOnExtended("warning"), expected[7]);
  EXPECT_EQ(scheme.GetExtendedContainer("warning"), expected[8]);
  EXPECT_EQ(scheme.GetOnExtendedContainer("warning"), expected[9]);
  EXPECT_EQ(scheme.GetExtendedFixed("warning"), expected[10]);
  EXPECT_EQ(scheme.GetExtendedFixedDim("warning"), expected[11]);
  EXPECT_EQ(scheme.GetOnExtendedFixed("warning"), expected[12]);
  EXPECT_EQ(scheme.GetOnExtendedFixedVariant("warning"), expected[13]);
  EXPECT_EQ(scheme.GetInverseExtended("warning"), expected[14]);
  EXPECT_EQ(scheme.GetInverseSurface(), expected[15]);
  EXPECT_EQ(scheme.GetInverseOnSurface(), expected[16]);
}

TEST(ColorSpecCustomTest, Matches2025PhoneLightHarmonizedFixture) {
  SchemeTonalSpot scheme(Hct(0xff4f46e5), false, -1.0, SpecVersion::k2025,
                         Platform::kPhone);
  scheme.AddExtendedColor({"warning", Hct(0xffffa500), true});

  ExpectColors(scheme, {0xfff97386, 0xffe8667a, 0xff000000, 0xff5b001a,
                        0xffe8667a, 0xff934c01, 0xff9e550c, 0xfffff7f4,
                        0xfff99e53, 0xff6d3700, 0xfff99e53, 0xffe99148,
                        0xff2e1400, 0xff5e2f00, 0xffd07c35, 0xff0e0e12,
                        0xff9e9ca2});
}

TEST(ColorSpecCustomTest, Matches2025WatchDarkFixture) {
  SchemeTonalSpot scheme(Hct(0xff4f46e5), true, 0.5, SpecVersion::k2025,
                         Platform::kWatch);
  scheme.AddExtendedColor({"warning", Hct(0xffffa500), false});

  ExpectColors(scheme, {0xff7d2938, 0xff6e1d2d, 0xfffffeff, 0xffffbdc3,
                        0xff6a1a2a, 0xfffffeff, 0xffffc378, 0xff281600,
                        0xff653e00, 0xffffffff, 0xff653e00, 0xff553400,
                        0xfffffeff, 0xffffc378, 0xff513100, 0xfffcf8ff,
                        0xff383742});
}

TEST(ColorSpecCustomTest, Matches2026StandardPhoneLightHarmonizedFixture) {
  SchemeCmf scheme(Hct(0xff4f46e5), false, 0.0, Platform::kPhone,
                   CmfProfile::k2026);
  scheme.AddExtendedColor({"warning", Hct(0xffffa500), true});

  ExpectColors(scheme, {0xfff85969, 0xffe64d5d, 0xff000000, 0xff41000c,
                        0xfff85969, 0xff934c01, 0xff934c01, 0xfffff7f4,
                        0xffffa358, 0xff572b00, 0xffffa358, 0xffef964c,
                        0xff361800, 0xff643200, 0xfff99e53, 0xff0b0a2a,
                        0xff9d9bb0});
}

TEST(ColorSpecCustomTest, Matches2026StandardWatchDarkFixture) {
  SchemeCmf scheme(Hct(0xff4f46e5), true, 1.0, Platform::kWatch,
                   CmfProfile::k2026);
  scheme.AddExtendedColor({"warning", Hct(0xffffa500), false});

  ExpectColors(scheme, {0xffff7a83, 0xffff7a83, 0xff000000, 0xff000000,
                        0xff000000, 0xffffe0bf, 0xffffe0bf, 0xff402600,
                        0xffffa500, 0xff000000, 0xffffa500, 0xffed9900,
                        0xff000000, 0xff000000, 0xff000000, 0xfffcf8ff,
                        0xff000000});
}

TEST(ColorSpecCustomTest, Matches2026CustomWatchLightHarmonizedFixture) {
  SchemeCmf scheme(Hct(0xff4f46e5), false, -0.5, Platform::kWatch,
                   CmfProfile::k2026Custom);
  scheme.AddExtendedColor({"warning", Hct(0xffffa500), true});

  ExpectColors(scheme, {0xfff85969, 0xffe64d5d, 0xff000000, 0xff41000c,
                        0xffffa6aa, 0xff934c01, 0xff934c01, 0xfffff7f4,
                        0xffffa358, 0xff572b00, 0xffffa358, 0xffef964c,
                        0xff361800, 0xff643200, 0xffffab6a, 0xff303036,
                        0xffaeabb4});
}

TEST(ColorSpecCustomTest, Matches2026CustomPhoneDarkFixture) {
  SchemeCmf scheme(Hct(0xff4f46e5), true, 0.5, Platform::kPhone,
                   CmfProfile::k2026Custom);
  scheme.AddExtendedColor({"warning", Hct(0xffffa500), false});

  ExpectColors(scheme, {0xffff6f7a, 0xfff85969, 0xff000000, 0xff000000,
                        0xff930627, 0xffffb95c, 0xffffb95c, 0xff4e2f00,
                        0xffffa500, 0xff402600, 0xffffa500, 0xffed9900,
                        0xff000000, 0xff311c00, 0xff663f00, 0xffe5e1ea,
                        0xff2a2930});
}

TEST(ColorSpecCustomTest, RejectsUnknownExtendedColorName) {
  SchemeCmf scheme(Hct(0xff4f46e5), false, 0.0);

  EXPECT_THROW(scheme.GetExtended("missing"), std::out_of_range);
}

}  // namespace
}  // namespace material_color_utilities
