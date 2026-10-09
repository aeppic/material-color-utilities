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

#include <functional>
#include <optional>
#include <stdexcept>
#include <string>

#include "cpp/cam/hct.h"
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
  return GetColor2025(name);
}

DynamicColor HighestSurface(const DynamicScheme& s) {
  return s.is_dark ? Role("surface_bright") : Role("surface_dim");
}

ToneFunction InitialTone(ColorFunction background) {
  return [background](const DynamicScheme& s) {
    return background(s).GetTone(s);
  };
}

double SurfaceChromaMultiplier(const DynamicScheme& s, double neutral,
                               double tonal_spot, double expressive_yellow,
                               double expressive, double vibrant) {
  if (s.variant == Variant::kNeutral) return neutral;
  if (s.variant == Variant::kTonalSpot) return tonal_spot;
  if (s.variant == Variant::kExpressive) {
    return Hct::IsYellow(s.neutral_palette.get_hue()) ? expressive_yellow
                                                      : expressive;
  }
  if (s.variant == Variant::kVibrant) return vibrant;
  return 1.0;
}

double ForegroundChromaMultiplier(const DynamicScheme& s) {
  if (s.platform != Platform::kPhone) return 1.0;
  if (s.variant == Variant::kNeutral) return 2.2;
  if (s.variant == Variant::kTonalSpot) return 1.7;
  if (s.variant != Variant::kExpressive) return 1.0;
  if (!Hct::IsYellow(s.neutral_palette.get_hue())) return 1.6;
  return s.is_dark ? 3.0 : 2.3;
}

DynamicColor Surface2025(const std::string& name) {
  PaletteFunction palette = [](const DynamicScheme& s) {
    return s.neutral_palette;
  };
  if (name == "surface") {
    return Color(name, palette, [](const DynamicScheme& s) {
      if (s.platform == Platform::kWatch) return 0.0;
      if (s.is_dark) return 4.0;
      if (Hct::IsYellow(s.neutral_palette.get_hue())) return 99.0;
      return s.variant == Variant::kVibrant ? 97.0 : 98.0;
    }, true);
  }
  if (name == "surface_dim") {
    return Color(name, palette, [](const DynamicScheme& s) {
      if (s.is_dark) return 4.0;
      if (Hct::IsYellow(s.neutral_palette.get_hue())) return 90.0;
      return s.variant == Variant::kVibrant ? 85.0 : 87.0;
    }, true, std::nullopt, std::nullopt, std::nullopt,
    [](const DynamicScheme& s) {
      return s.is_dark ? 1.0 : SurfaceChromaMultiplier(s, 2.5, 1.7, 2.7, 1.75, 1.36);
    });
  }
  if (name == "surface_bright") {
    return Color(name, palette, [](const DynamicScheme& s) {
      if (s.is_dark) return 18.0;
      if (Hct::IsYellow(s.neutral_palette.get_hue())) return 99.0;
      return s.variant == Variant::kVibrant ? 97.0 : 98.0;
    }, true, std::nullopt, std::nullopt, std::nullopt,
    [](const DynamicScheme& s) {
      return s.is_dark ? SurfaceChromaMultiplier(s, 2.5, 1.7, 2.7, 1.75, 1.36)
                       : 1.0;
    });
  }
  if (name == "surface_container_lowest") {
    return Color(name, palette,
                 [](const DynamicScheme& s) { return s.is_dark ? 0.0 : 100.0; },
                 true);
  }
  if (name == "surface_container_low") {
    return Color(name, palette, [](const DynamicScheme& s) {
      if (s.platform == Platform::kWatch) return 15.0;
      if (s.is_dark) return 6.0;
      if (Hct::IsYellow(s.neutral_palette.get_hue())) return 98.0;
      return s.variant == Variant::kVibrant ? 95.0 : 96.0;
    }, true, std::nullopt, std::nullopt, std::nullopt,
    [](const DynamicScheme& s) {
      return s.platform == Platform::kPhone
                 ? SurfaceChromaMultiplier(s, 1.3, 1.25, 1.3, 1.15, 1.08)
                 : 1.0;
    });
  }
  if (name == "surface_container") {
    return Color(name, palette, [](const DynamicScheme& s) {
      if (s.platform == Platform::kWatch) return 20.0;
      if (s.is_dark) return 9.0;
      if (Hct::IsYellow(s.neutral_palette.get_hue())) return 96.0;
      return s.variant == Variant::kVibrant ? 92.0 : 94.0;
    }, true, std::nullopt, std::nullopt, std::nullopt,
    [](const DynamicScheme& s) {
      return s.platform == Platform::kPhone
                 ? SurfaceChromaMultiplier(s, 1.6, 1.4, 1.6, 1.3, 1.15)
                 : 1.0;
    });
  }
  if (name == "surface_container_high") {
    return Color(name, palette, [](const DynamicScheme& s) {
      if (s.platform == Platform::kWatch) return 25.0;
      if (s.is_dark) return 12.0;
      if (Hct::IsYellow(s.neutral_palette.get_hue())) return 94.0;
      return s.variant == Variant::kVibrant ? 90.0 : 92.0;
    }, true, std::nullopt, std::nullopt, std::nullopt,
    [](const DynamicScheme& s) {
      return s.platform == Platform::kPhone
                 ? SurfaceChromaMultiplier(s, 1.9, 1.5, 1.95, 1.45, 1.22)
                 : 1.0;
    });
  }
  if (name == "surface_container_highest") {
    return Color(name, palette, [](const DynamicScheme& s) {
      if (s.is_dark) return 15.0;
      if (Hct::IsYellow(s.neutral_palette.get_hue())) return 92.0;
      return s.variant == Variant::kVibrant ? 88.0 : 90.0;
    }, true, std::nullopt, std::nullopt, std::nullopt,
    [](const DynamicScheme& s) {
      return SurfaceChromaMultiplier(s, 2.2, 1.7, 2.3, 1.6, 1.29);
    });
  }
  if (name == "inverse_surface") {
    return Color(name, palette,
                 [](const DynamicScheme& s) { return s.is_dark ? 98.0 : 4.0; },
                 true);
  }
  throw std::invalid_argument("Unknown 2025 surface role: " + name);
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

double AccentTone2025(const std::string& family, const DynamicScheme& s) {
  const TonalPalette palette = AccentPalette(family)(s);
  if (family == "primary") {
    if (s.variant == Variant::kNeutral) {
      return s.platform == Platform::kWatch ? 90.0 : s.is_dark ? 80.0 : 40.0;
    }
    if (s.variant == Variant::kTonalSpot) {
      if (s.platform == Platform::kWatch) return TMaxC(palette, 0.0, 90.0);
      return s.is_dark ? 80.0 : TMaxC(palette);
    }
    if (s.variant == Variant::kExpressive) {
      if (s.platform == Platform::kWatch) return TMaxC(palette);
      return TMaxC(palette, 0.0,
                   s.is_dark ? Hct::IsCyan(palette.get_hue()) ? 88.0 : 98.0
                             : Hct::IsYellow(palette.get_hue()) ? 25.0 : 98.0);
    }
    return TMaxC(palette, 0.0,
                 s.platform == Platform::kPhone && Hct::IsCyan(palette.get_hue())
                     ? 88.0
                     : s.platform == Platform::kPhone ? 98.0 : 100.0);
  }
  if (family == "secondary") {
    if (s.platform == Platform::kWatch) {
      return s.variant == Variant::kNeutral ? 90.0 : TMaxC(palette, 0.0, 90.0);
    }
    if (s.variant == Variant::kNeutral) {
      return s.is_dark ? TMinC(palette, 0.0, 98.0) : TMaxC(palette);
    }
    if (s.variant == Variant::kVibrant) {
      return TMaxC(palette, 0.0, s.is_dark ? 90.0 : 98.0);
    }
    return s.is_dark ? 80.0 : TMaxC(palette);
  }
  if (family == "tertiary") {
    if (s.platform == Platform::kWatch) {
      return s.variant == Variant::kTonalSpot ? TMaxC(palette, 0.0, 90.0)
                                              : TMaxC(palette);
    }
    if (s.variant == Variant::kExpressive || s.variant == Variant::kVibrant) {
      return TMaxC(palette, 0.0,
                   Hct::IsCyan(palette.get_hue()) ? 88.0
                                                  : s.is_dark ? 98.0 : 100.0);
    }
    return s.is_dark ? TMaxC(palette, 0.0, 98.0) : TMaxC(palette);
  }
  return s.platform == Platform::kPhone
             ? s.is_dark ? TMinC(palette, 0.0, 98.0) : TMaxC(palette)
             : TMinC(palette);
}

double ContainerTone2025(const std::string& family, const DynamicScheme& s) {
  const TonalPalette palette = AccentPalette(family)(s);
  if (s.platform == Platform::kWatch) {
    if (family == "tertiary") return AccentTone2025(family, s);
    return 30.0;
  }
  if (family == "primary") {
    if (s.variant == Variant::kNeutral) return s.is_dark ? 30.0 : 90.0;
    if (s.variant == Variant::kTonalSpot) {
      return s.is_dark ? TMinC(palette, 35.0, 93.0)
                       : TMaxC(palette, 0.0, 90.0);
    }
    if (s.variant == Variant::kExpressive) {
      return s.is_dark
                 ? TMinC(palette, 30.0, 93.0)
                 : TMaxC(palette, 78.0,
                         Hct::IsCyan(palette.get_hue()) ? 88.0 : 90.0);
    }
    return s.is_dark
               ? TMinC(palette, 66.0, 93.0)
               : TMaxC(palette, 66.0,
                       Hct::IsCyan(palette.get_hue()) ? 88.0 : 93.0);
  }
  if (family == "secondary") {
    if (s.variant == Variant::kVibrant) {
      return s.is_dark ? TMinC(palette, 30.0, 40.0)
                       : TMaxC(palette, 84.0, 90.0);
    }
    if (s.variant == Variant::kExpressive) {
      return s.is_dark ? 15.0 : TMaxC(palette, 90.0, 95.0);
    }
    return s.is_dark ? 25.0 : 90.0;
  }
  if (family == "tertiary") {
    if (s.variant == Variant::kNeutral) {
      return TMaxC(palette, 0.0, s.is_dark ? 93.0 : 96.0);
    }
    if (s.variant == Variant::kTonalSpot) {
      return TMaxC(palette, 0.0, s.is_dark ? 93.0 : 100.0);
    }
    if (s.variant == Variant::kExpressive) {
      return TMaxC(palette, 75.0,
                   Hct::IsCyan(palette.get_hue()) ? 88.0
                                                  : s.is_dark ? 93.0 : 100.0);
    }
    return s.is_dark ? TMaxC(palette, 0.0, 93.0)
                     : TMaxC(palette, 72.0, 100.0);
  }
  return s.is_dark ? TMinC(palette, 30.0, 93.0)
                   : TMaxC(palette, 0.0, 90.0);
}

DynamicColor Accent2025(const std::string& name,
                        const std::string& family) {
  PaletteFunction palette = AccentPalette(family);
  std::string container = family + "_container";
  std::string dim = family + "_dim";
  if (name == family) {
    ColorFunction background = [](const DynamicScheme& s) {
      return s.platform == Platform::kPhone ? HighestSurface(s)
                                            : Role("surface_container_high");
    };
    return Color(name, palette,
                 [family](const DynamicScheme& s) {
                   return AccentTone2025(family, s);
                 },
                 true, background,
                 [](const DynamicScheme& s) {
                   return GetCurve(s.platform == Platform::kPhone ? 4.5 : 7.0);
                 },
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
  if (name == dim) {
    return Color(name, palette,
                 [family](const DynamicScheme& s) {
                   if (family == "primary") {
                     if (s.variant == Variant::kNeutral) return 85.0;
                     return s.variant == Variant::kTonalSpot
                                ? TMaxC(s.primary_palette, 0.0, 90.0)
                                : TMaxC(s.primary_palette);
                   }
                   if (family == "secondary") {
                     return s.variant == Variant::kNeutral
                                ? 85.0
                                : TMaxC(s.secondary_palette, 0.0, 90.0);
                   }
                   if (family == "tertiary") {
                     return s.variant == Variant::kTonalSpot
                                ? TMaxC(s.tertiary_palette, 0.0, 90.0)
                                : TMaxC(s.tertiary_palette);
                   }
                   return TMinC(s.error_palette);
                 },
                 true,
                 [](const DynamicScheme&) {
                   return Role("surface_container_high");
                 },
                 [](const DynamicScheme&) { return GetCurve(4.5); },
                 [family, dim](const DynamicScheme&) {
                   return ToneDeltaPair(Role(dim), Role(family), 5.0,
                                        TonePolarity::kDarker, true,
                                        DeltaConstraint::kFarther);
                 });
  }
  if (name == "on_" + family) {
    ColorFunction background = [family, dim](const DynamicScheme& s) {
      return s.platform == Platform::kPhone ? Role(family) : Role(dim);
    };
    return Color(name, palette, InitialTone(background), false, background,
                 [](const DynamicScheme& s) {
                   return GetCurve(s.platform == Platform::kPhone ? 6.0 : 7.0);
                 });
  }
  if (name == container) {
    return Color(name, palette,
                 [family](const DynamicScheme& s) {
                   return ContainerTone2025(family, s);
                 },
                 true, [](const DynamicScheme& s) { return HighestSurface(s); },
                 [](const DynamicScheme&) { return GetCurve(1.5); },
                 [container, dim](const DynamicScheme&) {
                   return ToneDeltaPair(Role(container), Role(dim), 10.0,
                                        TonePolarity::kDarker, true,
                                        DeltaConstraint::kFarther);
                 },
                 std::nullopt,
                 [](const DynamicScheme& s) {
                   return s.platform == Platform::kPhone;
                 },
                 [](const DynamicScheme& s) {
                   return s.platform == Platform::kWatch;
                 },
                 [](const DynamicScheme& s) {
                   return s.platform == Platform::kPhone &&
                          s.contrast_level > 0.0;
                 });
  }
  if (name == "on_" + container) {
    ColorFunction background = [container](const DynamicScheme&) {
      return Role(container);
    };
    return Color(name, palette, InitialTone(background), false, background,
                 [family](const DynamicScheme& s) {
                   double phone_curve = family == "error" ? 4.5 : 6.0;
                   return GetCurve(s.platform == Platform::kPhone ? phone_curve
                                                                 : 7.0);
                 });
  }
  std::string fixed = family + "_fixed";
  std::string fixed_dim = family + "_fixed_dim";
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
                 std::nullopt, std::nullopt,
                 [](const DynamicScheme& s) {
                   return s.platform == Platform::kPhone;
                 }, std::nullopt,
                 [](const DynamicScheme& s) {
                   return s.platform == Platform::kPhone &&
                          s.contrast_level > 0.0;
                 });
  }
  if (name == fixed_dim) {
    return Color(name, palette,
                 [fixed](const DynamicScheme& s) {
                   return Role(fixed).GetTone(s);
                 },
                 true, std::nullopt, std::nullopt,
                 [fixed, fixed_dim](const DynamicScheme&) {
                   return ToneDeltaPair(Role(fixed_dim), Role(fixed), 5.0,
                                        TonePolarity::kDarker, true,
                                        DeltaConstraint::kExact);
                 });
  }
  if (name == "on_" + fixed || name == "on_" + fixed + "_variant") {
    ColorFunction background = [fixed_dim](const DynamicScheme&) {
      return Role(fixed_dim);
    };
    bool variant = name.find("_variant") != std::string::npos;
    return Color(name, palette, InitialTone(background), false, background,
                 [variant](const DynamicScheme&) {
                   return GetCurve(variant ? 4.5 : 7.0);
                 });
  }
  throw std::invalid_argument("Unknown 2025 accent role: " + name);
}

bool StartsWith(const std::string& value, const std::string& prefix) {
  return value.rfind(prefix, 0) == 0;
}

std::string Family(const std::string& name) {
  for (const std::string family : {"primary", "secondary", "tertiary", "error"}) {
    if (name.find(family) != std::string::npos) return family;
  }
  return "";
}

}  // namespace

DynamicColor GetColor2025(const std::string& name) {
  if (name.find("_palette_key_color") != std::string::npos) {
    PaletteFunction palette = [name](const DynamicScheme& s) {
      if (name == "primary_palette_key_color") return s.primary_palette;
      if (name == "secondary_palette_key_color") return s.secondary_palette;
      if (name == "tertiary_palette_key_color") return s.tertiary_palette;
      if (name == "neutral_palette_key_color") return s.neutral_palette;
      if (name == "neutral_variant_palette_key_color") {
        return s.neutral_variant_palette;
      }
      return s.error_palette;
    };
    return DynamicColor::FromPalette(
        name, palette,
        [palette](const DynamicScheme& s) {
          return palette(s).get_key_color().get_tone();
        });
  }
  if (name == "shadow" || name == "scrim") {
    return DynamicColor::FromPalette(
        name, [](const DynamicScheme& s) { return s.neutral_palette; },
        [](const DynamicScheme&) { return 0.0; });
  }
  if (name == "surface_tint") {
    DynamicColor color = Accent2025("primary", "primary");
    color.name_ = name;
    return color;
  }
  if (StartsWith(name, "surface") || name == "inverse_surface") {
    DynamicColor color = Surface2025(
        name == "surface_variant" ? "surface_container_highest" : name);
    color.name_ = name;
    return color;
  }
  if (name == "background") {
    DynamicColor color = Surface2025("surface");
    color.name_ = name;
    return color;
  }
  if (name == "on_surface" || name == "on_background" ||
      name == "on_surface_variant" || name == "outline" ||
      name == "outline_variant") {
    PaletteFunction palette = [](const DynamicScheme& s) {
      return s.neutral_palette;
    };
    ColorFunction background = [](const DynamicScheme& s) {
      return s.platform == Platform::kPhone ? HighestSurface(s)
                                            : Role("surface_container_high");
    };
    ToneFunction tone = name == "on_background"
                            ? ToneFunction([](const DynamicScheme& s) {
                                return s.platform == Platform::kWatch
                                           ? 100.0
                                           : Role("on_surface").GetTone(s);
                              })
                            : name == "on_surface"
                                  ? ToneFunction([background](
                                                     const DynamicScheme& s) {
                                      return s.variant == Variant::kVibrant
                                                 ? TMaxC(s.neutral_palette, 0.0,
                                                         100.0, 1.1)
                                                 : background(s).GetTone(s);
                                    })
                                  : InitialTone(background);
    return Color(name, palette, tone, false, background,
                 [name](const DynamicScheme& s) {
                   if (name == "on_surface" || name == "on_background") {
                     return GetCurve(s.is_dark && s.platform == Platform::kPhone
                                         ? 11.0
                                         : 9.0);
                   }
                   if (name == "on_surface_variant") {
                     return GetCurve(s.platform == Platform::kWatch
                                         ? 7.0
                                         : s.is_dark ? 6.0 : 4.5);
                   }
                   if (name == "outline") {
                     return GetCurve(s.platform == Platform::kPhone ? 3.0 : 4.5);
                   }
                   if (name == "outline_variant") {
                     return GetCurve(s.platform == Platform::kPhone ? 1.5 : 3.0);
                   }
                   return GetCurve(9.0);
                 },
                 std::nullopt,
                 [](const DynamicScheme& s) {
                   return ForegroundChromaMultiplier(s);
                 });
  }
  if (name == "inverse_on_surface") {
    ColorFunction background = [](const DynamicScheme&) {
      return Role("inverse_surface");
    };
    return Color(name,
                 [](const DynamicScheme& s) { return s.neutral_palette; },
                 InitialTone(background), false, background,
                 [](const DynamicScheme&) { return GetCurve(7.0); });
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
  if (!family.empty()) return Accent2025(name, family);
  throw std::invalid_argument("Unknown 2025 Material color role: " + name);
}

}  // namespace material_color_utilities
