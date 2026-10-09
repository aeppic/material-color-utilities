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

#include "cpp/dynamiccolor/material_dynamic_colors.h"

#include <cstdint>
#include <vector>

#include "cpp/cam/hct.h"
#include "cpp/dynamiccolor/color_spec.h"
#include "cpp/dynamiccolor/dynamic_scheme.h"
#include "cpp/scheme/scheme_cmf.h"
#include "cpp/scheme/scheme_tonal_spot.h"
#include <gtest/gtest.h>

namespace material_color_utilities {
namespace {

std::vector<Argb> RepresentativeColors(DynamicScheme& scheme) {
  return {
      scheme.GetSurface(),          scheme.GetSurfaceContainerHigh(),
      scheme.GetOnSurface(),        scheme.GetPrimary(),
      scheme.GetPrimaryContainer(), scheme.GetOnPrimary(),
      scheme.GetPrimaryFixed(),     scheme.GetOnPrimaryFixed(),
      scheme.GetSecondary(),        scheme.GetTertiary(),
      scheme.GetError(),            scheme.GetInverseSurface(),
  };
}

std::vector<Argb> AllOfficialColors(DynamicScheme& scheme) {
  return {
      scheme.GetPrimaryPaletteKeyColor(),
      scheme.GetSecondaryPaletteKeyColor(),
      scheme.GetTertiaryPaletteKeyColor(),
      scheme.GetNeutralPaletteKeyColor(),
      scheme.GetNeutralVariantPaletteKeyColor(),
      scheme.GetErrorPaletteKeyColor(),
      scheme.GetBackground(),
      scheme.GetOnBackground(),
      scheme.GetSurface(),
      scheme.GetSurfaceDim(),
      scheme.GetSurfaceBright(),
      scheme.GetSurfaceContainerLowest(),
      scheme.GetSurfaceContainerLow(),
      scheme.GetSurfaceContainer(),
      scheme.GetSurfaceContainerHigh(),
      scheme.GetSurfaceContainerHighest(),
      scheme.GetOnSurface(),
      scheme.GetSurfaceVariant(),
      scheme.GetOnSurfaceVariant(),
      scheme.GetOutline(),
      scheme.GetOutlineVariant(),
      scheme.GetInverseSurface(),
      scheme.GetInverseOnSurface(),
      scheme.GetShadow(),
      scheme.GetScrim(),
      scheme.GetSurfaceTint(),
      scheme.GetPrimary(),
      scheme.GetPrimaryDim(),
      scheme.GetOnPrimary(),
      scheme.GetPrimaryContainer(),
      scheme.GetOnPrimaryContainer(),
      scheme.GetPrimaryFixed(),
      scheme.GetPrimaryFixedDim(),
      scheme.GetOnPrimaryFixed(),
      scheme.GetOnPrimaryFixedVariant(),
      scheme.GetInversePrimary(),
      scheme.GetSecondary(),
      scheme.GetSecondaryDim(),
      scheme.GetOnSecondary(),
      scheme.GetSecondaryContainer(),
      scheme.GetOnSecondaryContainer(),
      scheme.GetSecondaryFixed(),
      scheme.GetSecondaryFixedDim(),
      scheme.GetOnSecondaryFixed(),
      scheme.GetOnSecondaryFixedVariant(),
      scheme.GetTertiary(),
      scheme.GetTertiaryDim(),
      scheme.GetOnTertiary(),
      scheme.GetTertiaryContainer(),
      scheme.GetOnTertiaryContainer(),
      scheme.GetTertiaryFixed(),
      scheme.GetTertiaryFixedDim(),
      scheme.GetOnTertiaryFixed(),
      scheme.GetOnTertiaryFixedVariant(),
      scheme.GetError(),
      scheme.GetErrorDim(),
      scheme.GetOnError(),
      scheme.GetErrorContainer(),
      scheme.GetOnErrorContainer(),
      scheme.GetErrorFixed(),
      scheme.GetErrorFixedDim(),
      scheme.GetOnErrorFixed(),
      scheme.GetOnErrorFixedVariant(),
      scheme.GetInverseError(),
  };
}

uint64_t HashColors(const std::vector<Argb>& colors) {
  uint64_t hash = 14695981039346656037ULL;
  for (Argb color : colors) {
    for (int shift = 0; shift < 32; shift += 8) {
      hash ^= (color >> shift) & 0xff;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}

TEST(MaterialDynamicColorsTest, Spec2025PhoneFixtures) {
  SchemeTonalSpot light(Hct(0xff6750a4), false, 0.0, SpecVersion::k2025,
                        Platform::kPhone);
  SchemeTonalSpot dark(Hct(0xff6750a4), true, 0.0, SpecVersion::k2025,
                       Platform::kPhone);

  EXPECT_EQ(RepresentativeColors(light),
            std::vector<Argb>({0xfffdf7fe, 0xffece6f0, 0xff34313a,
                               0xff655789, 0xffd4c3fd, 0xfffdf7ff,
                               0xffd4c3fd, 0xff352857, 0xff625c71,
                               0xff7b5270, 0xffa8364b, 0xff0f0d12}));
  EXPECT_EQ(RepresentativeColors(dark),
            std::vector<Argb>({0xff0f0d12, 0xff211e26, 0xffeae3ef,
                               0xffcdc0ec, 0xff574d72, 0xff443a5f,
                               0xffded0fe, 0xff3c3256, 0xffcbc2db,
                               0xffffcfef, 0xfff97386, 0xfffdf7fe}));
}

TEST(MaterialDynamicColorsTest, Spec2025WatchFixtures) {
  SchemeTonalSpot light(Hct(0xff6750a4), false, 0.0, SpecVersion::k2025,
                        Platform::kWatch);
  SchemeTonalSpot dark(Hct(0xff6750a4), true, 0.0, SpecVersion::k2025,
                       Platform::kWatch);

  EXPECT_EQ(RepresentativeColors(light),
            std::vector<Argb>({0xff000000, 0xff3d3a45, 0xffede5f4,
                               0xffd6c5ff, 0xff4c3f6f, 0xff362a58,
                               0xff4c3f6f, 0xffd6c5ff, 0xffe8def8,
                               0xffffc2ec, 0xffffbdc3, 0xff0f0d16}));
  EXPECT_EQ(RepresentativeColors(dark),
            std::vector<Argb>({0xff000000, 0xff3d3a45, 0xffede5f4,
                               0xffd6c5ff, 0xff4c3f6f, 0xff362a58,
                               0xff4c3f6f, 0xffd6c5ff, 0xffe8def8,
                               0xffffc2ec, 0xffffbdc3, 0xfffdf7ff}));
}

TEST(MaterialDynamicColorsTest, Spec2026CmfPhoneFixtures) {
  SchemeCmf light(Hct(0xff6750a4), false, 0.0);
  SchemeCmf dark(Hct(0xff6750a4), true, 0.0);

  EXPECT_EQ(RepresentativeColors(light),
            std::vector<Argb>({0xfffdf7ff, 0xffede4fb, 0xff352f43,
                               0xff5b4497, 0xff6750a4, 0xffeadeff,
                               0xff6750a4, 0xffffffff, 0xff645a7d,
                               0xff594982, 0xff9f3f47, 0xff100b1d}));
  EXPECT_EQ(RepresentativeColors(dark),
            std::vector<Argb>({0xff0f0d16, 0xff221d2d, 0xffebe1fc,
                               0xffa890e9, 0xff6750a4, 0xff270762,
                               0xff6750a4, 0xffffffff, 0xffa398be,
                               0xffa694d3, 0xfffa868d, 0xfffdf7ff}));
}

TEST(MaterialDynamicColorsTest, Spec2026CmfWatchFixtures) {
  SchemeCmf light(Hct(0xff6750a4), false, 0.0, Platform::kWatch);
  SchemeCmf dark(Hct(0xff6750a4), true, 0.0, Platform::kWatch);

  EXPECT_EQ(RepresentativeColors(light),
            std::vector<Argb>({0xfffdf7ff, 0xffede4fb, 0xff352f43,
                               0xff6750a4, 0xff6750a4, 0xfffcf6ff,
                               0xff6750a4, 0xffffffff, 0xff645a7d,
                               0xff65558f, 0xff9f3f47, 0xff100b1d}));
  EXPECT_EQ(RepresentativeColors(dark),
            std::vector<Argb>({0xff0f0d16, 0xff221d2d, 0xffebe1fc,
                               0xffa890e9, 0xff6750a4, 0xff270762,
                               0xff6750a4, 0xffffffff, 0xffa398be,
                               0xffa694d3, 0xfffa868d, 0xfffdf7ff}));
}

TEST(MaterialDynamicColorsTest, DimRolesRequireSpec2025) {
  SchemeTonalSpot scheme(Hct(0xff6750a4), false, 0.0);

  EXPECT_THROW(scheme.GetPrimaryDim(), std::logic_error);
  SchemeTonalSpot scheme2025(Hct(0xff6750a4), false, 0.0,
                             SpecVersion::k2025, Platform::kPhone);
  EXPECT_NO_THROW(scheme2025.GetPrimaryDim());
  EXPECT_NO_THROW(scheme2025.GetSecondaryDim());
  EXPECT_NO_THROW(scheme2025.GetTertiaryDim());
  EXPECT_NO_THROW(scheme2025.GetErrorDim());
}

TEST(MaterialDynamicColorsTest, ResolvesEveryOfficialRole) {
  SchemeTonalSpot scheme2025(Hct(0xff6750a4), false, 0.0,
                             SpecVersion::k2025, Platform::kPhone);
  SchemeCmf scheme2026(Hct(0xff6750a4), true, 0.0, Platform::kWatch);

  EXPECT_EQ(AllOfficialColors(scheme2025).size(), 64);
  EXPECT_EQ(AllOfficialColors(scheme2026).size(), 64);
}

TEST(MaterialDynamicColorsTest, EveryOfficialRoleMatchesTypeScriptFixtures) {
  struct Fixture {
    SpecVersion version;
    Platform platform;
    bool is_dark;
    double contrast;
    uint64_t expected;
  };
  const std::vector<Fixture> fixtures = {
      {SpecVersion::k2025, Platform::kPhone, false, -1, 0x42c9cedd5e408e76ULL},
      {SpecVersion::k2025, Platform::kPhone, false, 0, 0xb3b697049732cb60ULL},
      {SpecVersion::k2025, Platform::kPhone, false, 0.5, 0x9a864dbe8ea5d5c3ULL},
      {SpecVersion::k2025, Platform::kPhone, false, 1, 0xf78dbdd3a4f3fa73ULL},
      {SpecVersion::k2025, Platform::kPhone, true, -1, 0x06f0c4ec0a9408eeULL},
      {SpecVersion::k2025, Platform::kPhone, true, 0, 0xa9ae4a8241363764ULL},
      {SpecVersion::k2025, Platform::kPhone, true, 0.5, 0x73200d3a8f8dee0cULL},
      {SpecVersion::k2025, Platform::kPhone, true, 1, 0xdfb83351b0e54f06ULL},
      {SpecVersion::k2025, Platform::kWatch, false, -1, 0x483bb52805378689ULL},
      {SpecVersion::k2025, Platform::kWatch, false, 0, 0x2bc6b39814864ccdULL},
      {SpecVersion::k2025, Platform::kWatch, false, 0.5, 0xc2a9bba246ea2f61ULL},
      {SpecVersion::k2025, Platform::kWatch, false, 1, 0xc4943dae7fcfa4f6ULL},
      {SpecVersion::k2025, Platform::kWatch, true, -1, 0x1d34037d6264ba91ULL},
      {SpecVersion::k2025, Platform::kWatch, true, 0, 0xbac73f663d6ea635ULL},
      {SpecVersion::k2025, Platform::kWatch, true, 0.5, 0x240f74312d517970ULL},
      {SpecVersion::k2025, Platform::kWatch, true, 1, 0xf9c36f23e7ff125bULL},
      {SpecVersion::k2026, Platform::kPhone, false, -1, 0x63e6486de2570db1ULL},
      {SpecVersion::k2026, Platform::kPhone, false, 0, 0xc77103a1322db95cULL},
      {SpecVersion::k2026, Platform::kPhone, false, 0.5, 0x43bee7d96dae22daULL},
      {SpecVersion::k2026, Platform::kPhone, false, 1, 0x329e06f900aa7900ULL},
      {SpecVersion::k2026, Platform::kPhone, true, -1, 0xf7f9229fe7919178ULL},
      {SpecVersion::k2026, Platform::kPhone, true, 0, 0x020faa6f79ced00cULL},
      {SpecVersion::k2026, Platform::kPhone, true, 0.5, 0xbd2654b4e37d06d6ULL},
      {SpecVersion::k2026, Platform::kPhone, true, 1, 0x81558670ccaf87a6ULL},
      {SpecVersion::k2026, Platform::kWatch, false, -1, 0x29f28d77df9f94a6ULL},
      {SpecVersion::k2026, Platform::kWatch, false, 0, 0x7637896e6bfed937ULL},
      {SpecVersion::k2026, Platform::kWatch, false, 0.5, 0xda03fc8d989f9211ULL},
      {SpecVersion::k2026, Platform::kWatch, false, 1, 0xc6424fad24b06b72ULL},
      {SpecVersion::k2026, Platform::kWatch, true, -1, 0x913efa21b1f032a1ULL},
      {SpecVersion::k2026, Platform::kWatch, true, 0, 0xa8bea43748fb1a98ULL},
      {SpecVersion::k2026, Platform::kWatch, true, 0.5, 0xf28f07c6fa57118eULL},
      {SpecVersion::k2026, Platform::kWatch, true, 1, 0xfb6a5bebaf76d6b1ULL},
  };

  for (const Fixture& fixture : fixtures) {
    std::vector<Argb> colors;
    if (fixture.version == SpecVersion::k2025) {
      SchemeTonalSpot scheme(Hct(0xff6750a4), fixture.is_dark,
                             fixture.contrast, fixture.version,
                             fixture.platform);
      colors = AllOfficialColors(scheme);
    } else {
      SchemeCmf scheme(Hct(0xff6750a4), fixture.is_dark, fixture.contrast,
                       fixture.platform);
      colors = AllOfficialColors(scheme);
    }
    EXPECT_EQ(HashColors(colors), fixture.expected);
  }
}

}  // namespace
}  // namespace material_color_utilities
