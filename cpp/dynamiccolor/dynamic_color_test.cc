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

#include "cpp/dynamiccolor/dynamic_color.h"

#include <optional>

#include "cpp/cam/hct.h"
#include "cpp/dynamiccolor/color_spec.h"
#include "cpp/dynamiccolor/tone_delta_pair.h"
#include "cpp/palettes/tones.h"
#include "cpp/scheme/scheme_tonal_spot.h"
#include <gtest/gtest.h>

namespace material_color_utilities {
namespace {

TEST(DynamicColorTest, UsesExactToneDeltaFor2025) {
  SchemeTonalSpot scheme(Hct(0xff6750a4), false, 0.0);
  scheme.spec_version = SpecVersion::k2025;
  auto palette = [](const DynamicScheme& s) { return s.primary_palette; };
  DynamicColor role_a = DynamicColor::FromPalette(
      "a", palette, [](const DynamicScheme&) { return 60.0; });
  DynamicColor role_b = DynamicColor::FromPalette(
      "b", palette, [](const DynamicScheme&) { return 40.0; });
  DynamicColor constrained(
      "a", palette, [](const DynamicScheme&) { return 60.0; }, false,
      std::nullopt, std::nullopt, std::nullopt,
      [role_a, role_b](const DynamicScheme&) {
        return ToneDeltaPair(role_a, role_b, 10.0, TonePolarity::kLighter,
                             false, DeltaConstraint::kExact);
      });

  EXPECT_EQ(constrained.GetTone(scheme), 50.0);
}

TEST(DynamicColorTest, ReversesRelativeLighterDeltaInDarkMode) {
  SchemeTonalSpot scheme(Hct(0xff6750a4), true, 0.0);
  scheme.spec_version = SpecVersion::k2025;
  auto palette = [](const DynamicScheme& s) { return s.primary_palette; };
  DynamicColor role_a = DynamicColor::FromPalette(
      "a", palette, [](const DynamicScheme&) { return 60.0; });
  DynamicColor role_b = DynamicColor::FromPalette(
      "b", palette, [](const DynamicScheme&) { return 40.0; });
  DynamicColor constrained(
      "a", palette, [](const DynamicScheme&) { return 60.0; }, false,
      std::nullopt, std::nullopt, std::nullopt,
      [role_a, role_b](const DynamicScheme&) {
        return ToneDeltaPair(role_a, role_b, 10.0,
                             TonePolarity::kRelativeLighter, false,
                             DeltaConstraint::kExact);
      });

  EXPECT_EQ(constrained.GetTone(scheme), 30.0);
}

TEST(DynamicColorTest, AppliesChromaMultiplierFor2025) {
  SchemeTonalSpot scheme(Hct(0xff6750a4), false, 0.0);
  scheme.spec_version = SpecVersion::k2025;
  auto palette = [](const DynamicScheme&) {
    return TonalPalette(30.0, 80.0);
  };
  DynamicColor color(
      "multiplied", palette, [](const DynamicScheme&) { return 50.0; }, false,
      std::nullopt, std::nullopt, std::nullopt, std::nullopt,
      [](const DynamicScheme&) { return 0.5; });

  EXPECT_LT(color.GetHct(scheme).get_chroma(), 50.0);
}

}  // namespace
}  // namespace material_color_utilities
