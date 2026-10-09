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

#include "cpp/dynamiccolor/color_spec_internal.h"

#include <algorithm>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>

#include "cpp/dynamiccolor/color_spec_helpers.h"
#include "cpp/dynamiccolor/tone_delta_pair.h"

namespace material_color_utilities {
namespace {

using ColorFunction = std::function<DynamicColor(const DynamicScheme&)>;
using PaletteFunction = std::function<TonalPalette(const DynamicScheme&)>;
using ToneFunction = std::function<double(const DynamicScheme&)>;

DynamicColor Color(
    std::string name, PaletteFunction palette, ToneFunction tone,
    bool is_background = false,
    std::optional<ColorFunction> background = std::nullopt,
    std::optional<std::function<ContrastCurve(const DynamicScheme&)>> curve =
        std::nullopt,
    std::optional<std::function<ToneDeltaPair(const DynamicScheme&)>> pair =
        std::nullopt,
    std::optional<ToneFunction> chroma_multiplier = std::nullopt,
    std::optional<std::function<bool(const DynamicScheme&)>>
        background_condition = std::nullopt,
    std::optional<std::function<bool(const DynamicScheme&)>> pair_condition =
        std::nullopt,
    std::optional<std::function<bool(const DynamicScheme&)>> curve_condition =
        std::nullopt) {
  return DynamicColor(name, palette, tone, is_background, background,
                      std::nullopt, std::nullopt, pair, chroma_multiplier,
                      background_condition, std::nullopt, curve,
                      pair_condition, curve_condition);
}

DynamicColor Role(const std::string& name) {
  return GetColor2026(name);
}

DynamicColor HighestSurface(const DynamicScheme& s) {
  return s.is_dark ? Role("surface_bright") : Role("surface_dim");
}

ToneFunction InitialTone(ColorFunction background) {
  return [background](const DynamicScheme& s) {
    return background(s).GetTone(s);
  };
}

PaletteFunction AccentPalette(const std::string& family) {
  if (family == "primary") {
    return [](const DynamicScheme& s) { return s.primary_palette; };
  }
  if (family == "secondary") {
    return [](const DynamicScheme& s) { return s.secondary_palette; };
  }
  if (family == "tertiary") {
    return [](const DynamicScheme& s) { return s.tertiary_palette; };
  }
  return [](const DynamicScheme& s) { return s.error_palette; };
}

double MainTone(const std::string& family, const DynamicScheme& s) {
  if (family == "primary") {
    return s.source_color_hct.get_chroma() <= 12.0
               ? s.is_dark ? 80.0 : 40.0
               : s.source_color_hct.get_tone();
  }
  if (family == "secondary") {
    return s.is_dark ? TMinC(s.secondary_palette) : TMaxC(s.secondary_palette);
  }
  if (family == "tertiary") {
    return s.source_color_hcts.size() > 1
               ? s.source_color_hcts[1].get_tone()
               : s.source_color_hct.get_tone();
  }
  return TMaxC(s.error_palette);
}

double ContainerTone(const std::string& family, const DynamicScheme& s) {
  if (family == "primary") {
    if (!s.is_dark && s.source_color_hct.get_chroma() <= 12.0) return 90.0;
    return s.source_color_hct.get_tone() > 55.0
               ? std::clamp(s.source_color_hct.get_tone(), 61.0, 90.0)
               : std::clamp(s.source_color_hct.get_tone(), 30.0, 49.0);
  }
  if (family == "secondary") {
    return s.is_dark ? TMinC(s.secondary_palette, 20.0, 49.0)
                     : TMaxC(s.secondary_palette, 61.0, 90.0);
  }
  if (family == "tertiary") {
    const Hct& source = s.source_color_hcts.size() > 1
                            ? s.source_color_hcts[1]
                            : s.source_color_hct;
    return source.get_tone() > 55.0
               ? std::clamp(source.get_tone(), 61.0, 90.0)
               : std::clamp(source.get_tone(), 20.0, 49.0);
  }
  return s.is_dark ? TMinC(s.error_palette) : TMaxC(s.error_palette);
}

DynamicColor Accent2026(const std::string& name,
                        const std::string& family) {
  PaletteFunction palette = AccentPalette(family);
  std::string container = family + "_container";
  std::string fixed = family + "_fixed";
  std::string fixed_dim = family + "_fixed_dim";
  if (name == family) {
    return Color(name, palette,
                 [family](const DynamicScheme& s) { return MainTone(family, s); },
                 true, [](const DynamicScheme& s) { return HighestSurface(s); },
                 [](const DynamicScheme&) { return GetCurve(4.5); },
                 [family, container](const DynamicScheme&) {
                   return ToneDeltaPair(Role(container), Role(family), 5.0,
                                        TonePolarity::kRelativeLighter, true,
                                        DeltaConstraint::kFarther);
                 },
                 std::nullopt, std::nullopt,
                 [](const DynamicScheme& s) {
                   return s.platform == Platform::kPhone;
                 });
  }
  if (name == "on_" + family) {
    ColorFunction background = [family](const DynamicScheme&) {
      return Role(family);
    };
    return Color(name, palette, InitialTone(background), false, background,
                 [](const DynamicScheme&) { return GetCurve(6.0); });
  }
  if (name == container) {
    return Color(name, palette,
                 [family](const DynamicScheme& s) {
                   return ContainerTone(family, s);
                 },
                 true, [](const DynamicScheme& s) { return HighestSurface(s); },
                 [](const DynamicScheme&) { return GetCurve(1.5); },
                 std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                 [](const DynamicScheme& s) { return s.contrast_level > 0.0; });
  }
  if (name == "on_" + container) {
    ColorFunction background = [container](const DynamicScheme&) {
      return Role(container);
    };
    return Color(name, palette, InitialTone(background), false, background,
                 [](const DynamicScheme&) { return GetCurve(6.0); });
  }
  if (name == fixed) {
    return Color(name, palette,
                 [container](const DynamicScheme& s) {
                   DynamicScheme light = s;
                   light.is_dark = false;
                   light.contrast_level = 0.0;
                   return Role(container).GetTone(light);
                 },
                 true, [](const DynamicScheme& s) { return HighestSurface(s); },
                 [](const DynamicScheme&) { return GetCurve(1.5); },
                 std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                 [](const DynamicScheme& s) { return s.contrast_level > 0.0; });
  }
  if (name == fixed_dim) {
    return Color(name, palette,
                 [fixed](const DynamicScheme& s) {
                   return Role(fixed).GetTone(s);
                 },
                 true, [](const DynamicScheme& s) { return HighestSurface(s); },
                 [](const DynamicScheme&) { return GetCurve(1.5); },
                 [fixed, fixed_dim](const DynamicScheme&) {
                   return ToneDeltaPair(Role(fixed_dim), Role(fixed), 5.0,
                                        TonePolarity::kDarker, true,
                                        DeltaConstraint::kExact);
                 },
                 std::nullopt, std::nullopt, std::nullopt,
                 [](const DynamicScheme& s) { return s.contrast_level > 0.0; });
  }
  if (name == "on_" + fixed || name == "on_" + fixed + "_variant") {
    ColorFunction background = [fixed, fixed_dim](const DynamicScheme& s) {
      return Role(fixed).GetTone(s) > 57.0 ? Role(fixed_dim) : Role(fixed);
    };
    bool variant = name.find("_variant") != std::string::npos;
    return Color(name, palette, InitialTone(background), false, background,
                 [variant](const DynamicScheme&) {
                   return GetCurve(variant ? 4.5 : 7.0);
                 });
  }
  if (name == family + "_dim") {
    DynamicColor color = Accent2026(family, family);
    color.name_ = name;
    return color;
  }
  return GetColor2025(name);
}

std::string Family(const std::string& name) {
  for (const std::string family : {"primary", "secondary", "tertiary", "error"}) {
    if (name.find(family) != std::string::npos) return family;
  }
  return "";
}

bool IsSurface(const std::string& name) {
  return name == "surface" || name == "surface_dim" ||
         name == "surface_bright" || name == "surface_container_lowest" ||
         name == "surface_container_low" || name == "surface_container" ||
         name == "surface_container_high" ||
         name == "surface_container_highest";
}

}  // namespace

DynamicColor GetColor2026(const std::string& name) {
  if (name == "background" || name == "surface_variant" ||
      name == "surface_tint") {
    std::string source = name == "background"
                             ? "surface"
                             : name == "surface_variant"
                                   ? "surface_container_highest"
                                   : "primary";
    DynamicColor color = GetColor2026(source);
    color.name_ = name;
    return color;
  }
  if (name == "on_background") {
    DynamicColor color = GetColor2026("on_surface");
    color.name_ = name;
    ToneFunction on_surface_tone = color.tone_;
    color.tone_ = [on_surface_tone](const DynamicScheme& s) {
      return s.platform == Platform::kWatch ? 100.0 : on_surface_tone(s);
    };
    return color;
  }
  if (IsSurface(name)) {
    double light_tone = 98.0;
    double dark_tone = 4.0;
    double multiplier = 1.0;
    if (name == "surface_dim") {
      light_tone = 87.0;
      multiplier = 1.7;
    } else if (name == "surface_bright") {
      dark_tone = 18.0;
      multiplier = 1.7;
    } else if (name == "surface_container_lowest") {
      light_tone = 100.0;
      dark_tone = 0.0;
    } else if (name == "surface_container_low") {
      light_tone = 96.0;
      dark_tone = 6.0;
      multiplier = 1.25;
    } else if (name == "surface_container") {
      light_tone = 94.0;
      dark_tone = 9.0;
      multiplier = 1.4;
    } else if (name == "surface_container_high") {
      light_tone = 92.0;
      dark_tone = 12.0;
      multiplier = 1.5;
    } else if (name == "surface_container_highest") {
      light_tone = 90.0;
      dark_tone = 15.0;
      multiplier = 1.7;
    }
    return Color(name,
                 [](const DynamicScheme& s) { return s.neutral_palette; },
                 [light_tone, dark_tone](const DynamicScheme& s) {
                   return s.is_dark ? dark_tone : light_tone;
                 },
                 true, std::nullopt, std::nullopt, std::nullopt,
                 [name, multiplier](const DynamicScheme& s) {
                   if (name == "surface_dim") return s.is_dark ? 1.0 : multiplier;
                   if (name == "surface_bright") return s.is_dark ? multiplier : 1.0;
                   return multiplier;
                 });
  }
  if (name == "inverse_surface") {
    return Color(name,
                 [](const DynamicScheme& s) { return s.neutral_palette; },
                 [](const DynamicScheme& s) {
                   if (s.profile == CmfProfile::k2026Custom) {
                     return s.is_dark ? 90.0 : 20.0;
                   }
                   return s.is_dark ? 98.0 : 4.0;
                 },
                 true, std::nullopt, std::nullopt, std::nullopt,
                 [](const DynamicScheme& s) {
                   return s.profile == CmfProfile::k2026Custom ? 1.0 : 1.7;
                 });
  }
  if (name == "on_surface" || name == "on_surface_variant" ||
      name == "outline" || name == "outline_variant") {
    ColorFunction background = [](const DynamicScheme& s) {
      return HighestSurface(s);
    };
    return Color(name,
                 [](const DynamicScheme& s) { return s.neutral_palette; },
                 InitialTone(background), false, background,
                 [name](const DynamicScheme& s) {
                   if (name == "on_surface") return GetCurve(s.is_dark ? 11.0 : 9.0);
                   if (name == "on_surface_variant") {
                     return GetCurve(s.is_dark ? 6.0 : 4.5);
                   }
                   return GetCurve(name == "outline" ? 3.0 : 1.5);
                 },
                 std::nullopt,
                 [](const DynamicScheme&) { return 1.7; });
  }
  if (name == "inverse_on_surface") {
    ColorFunction background = [](const DynamicScheme&) {
      return Role("inverse_surface");
    };
    return Color(name,
                 [](const DynamicScheme& s) { return s.neutral_palette; },
                 [background](const DynamicScheme& s) {
                   if (s.profile == CmfProfile::k2026Custom) {
                     return s.is_dark ? 20.0 : 95.0;
                   }
                   return background(s).GetTone(s);
                 },
                 false, background, [](const DynamicScheme& s) {
                   return s.profile == CmfProfile::k2026Custom
                              ? ContrastCurve(4.5, 7.0, 11.0, 21.0)
                              : GetCurve(7.0);
                 });
  }
  if (name == "inverse_primary") {
    ColorFunction background = [](const DynamicScheme&) {
      return Role("inverse_surface");
    };
    return Color(name,
                 [](const DynamicScheme& s) { return s.primary_palette; },
                 [](const DynamicScheme& s) { return TMaxC(s.primary_palette); },
                 false, background,
                 [](const DynamicScheme& s) {
                   return GetCurve(s.platform == Platform::kPhone ? 6.0 : 7.0);
                 });
  }
  std::string family = Family(name);
  if (!family.empty() && name != "inverse_primary") {
    return Accent2026(name, family);
  }
  return GetColor2025(name);
}

}  // namespace material_color_utilities
