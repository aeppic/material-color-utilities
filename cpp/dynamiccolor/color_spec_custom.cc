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

#include "cpp/dynamiccolor/color_spec_custom.h"

#include <algorithm>
#include <functional>
#include <optional>
#include <string>

#include "cpp/dynamiccolor/color_spec_helpers.h"
#include "cpp/dynamiccolor/color_spec_internal.h"
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
    std::optional<std::function<bool(const DynamicScheme&)>>
        background_condition = std::nullopt,
    std::optional<std::function<bool(const DynamicScheme&)>> pair_condition =
        std::nullopt,
    std::optional<std::function<bool(const DynamicScheme&)>> curve_condition =
        std::nullopt) {
  return DynamicColor(name, palette, tone, is_background, background,
                      std::nullopt, std::nullopt, pair, std::nullopt,
                      background_condition, std::nullopt, curve,
                      pair_condition, curve_condition);
}

PaletteFunction ExtendedPalette(const std::string& name) {
  return [name](const DynamicScheme& s) {
    return s.extended_palette.at(name);
  };
}

ToneFunction InitialTone(ColorFunction background) {
  return [background](const DynamicScheme& s) {
    return background(s).GetTone(s);
  };
}

DynamicColor Custom2025(CustomColorRole role, const std::string& name);
DynamicColor Custom2026(CustomColorRole role, const std::string& name);

DynamicColor HighestSurface2025(const DynamicScheme& s) {
  return s.is_dark ? GetColor2025("surface_bright")
                   : GetColor2025("surface_dim");
}

DynamicColor HighestSurface2026(const DynamicScheme& s) {
  return s.is_dark ? GetColor2026("surface_bright")
                   : GetColor2026("surface_dim");
}

DynamicColor Custom2025(CustomColorRole role, const std::string& name) {
  PaletteFunction error_palette = [](const DynamicScheme& s) {
    return s.error_palette;
  };
  if (role == CustomColorRole::kErrorFixed) {
    return Color(
        "error_fixed", error_palette,
        [](const DynamicScheme& s) {
          DynamicScheme light = s;
          light.is_dark = false;
          light.contrast_level = 0.0;
          return GetColor2025("error_container").GetTone(light);
        },
        true, [](const DynamicScheme& s) { return HighestSurface2025(s); },
        [](const DynamicScheme&) { return GetCurve(1.5); }, std::nullopt,
        [](const DynamicScheme& s) {
          return s.platform == Platform::kPhone;
        },
        std::nullopt, [](const DynamicScheme& s) {
          return s.platform == Platform::kPhone && s.contrast_level > 0.0;
        });
  }
  if (role == CustomColorRole::kErrorFixedDim) {
    return Color(
        "error_fixed_dim", error_palette,
        [](const DynamicScheme& s) {
          return Custom2025(CustomColorRole::kErrorFixed, "").GetTone(s);
        },
        true, std::nullopt, std::nullopt,
        [](const DynamicScheme&) {
          return ToneDeltaPair(
              Custom2025(CustomColorRole::kErrorFixedDim, ""),
              Custom2025(CustomColorRole::kErrorFixed, ""), 5.0,
              TonePolarity::kDarker, true, DeltaConstraint::kExact);
        });
  }
  if (role == CustomColorRole::kOnErrorFixed ||
      role == CustomColorRole::kOnErrorFixedVariant) {
    ColorFunction background = [](const DynamicScheme&) {
      return Custom2025(CustomColorRole::kErrorFixedDim, "");
    };
    bool variant = role == CustomColorRole::kOnErrorFixedVariant;
    return Color(variant ? "on_error_fixed_variant" : "on_error_fixed",
                 error_palette, InitialTone(background), false, background,
                 [variant](const DynamicScheme&) {
                   return GetCurve(variant ? 4.5 : 7.0);
                 });
  }
  if (role == CustomColorRole::kInverseError) {
    ColorFunction background = [](const DynamicScheme&) {
      return GetColor2025("inverse_surface");
    };
    return Color(
        "inverse_error", error_palette,
        [](const DynamicScheme& s) { return TMaxC(s.error_palette); }, false,
        background, [](const DynamicScheme& s) {
          return GetCurve(s.platform == Platform::kPhone ? 6.0 : 7.0);
        });
  }

  PaletteFunction palette = ExtendedPalette(name);
  if (role == CustomColorRole::kExtended) {
    return Color(
        name, palette,
        [name](const DynamicScheme& s) {
          const TonalPalette& extended = s.extended_palette.at(name);
          if (s.platform == Platform::kWatch) return TMinC(extended);
          return s.is_dark ? TMinC(extended, 0.0, 98.0) : TMaxC(extended);
        },
        true,
        [](const DynamicScheme& s) {
          return s.platform == Platform::kPhone
                     ? HighestSurface2025(s)
                     : GetColor2025("surface_container_high");
        },
        [](const DynamicScheme& s) {
          return GetCurve(s.platform == Platform::kPhone ? 4.5 : 7.0);
        },
        [name](const DynamicScheme&) {
          return ToneDeltaPair(
              Custom2025(CustomColorRole::kExtendedContainer, name),
              Custom2025(CustomColorRole::kExtended, name), 5.0,
              TonePolarity::kRelativeLighter, true,
              DeltaConstraint::kFarther);
        },
        std::nullopt, [](const DynamicScheme& s) {
          return s.platform == Platform::kPhone;
        });
  }
  if (role == CustomColorRole::kExtendedDim) {
    return Color(
        name + "_dim", palette,
        [name](const DynamicScheme& s) {
          return TMinC(s.extended_palette.at(name));
        },
        true,
        [](const DynamicScheme&) {
          return GetColor2025("surface_container_high");
        },
        [](const DynamicScheme&) { return GetCurve(4.5); },
        [name](const DynamicScheme&) {
          return ToneDeltaPair(
              Custom2025(CustomColorRole::kExtendedDim, name),
              Custom2025(CustomColorRole::kExtended, name), 5.0,
              TonePolarity::kDarker, true, DeltaConstraint::kFarther);
        });
  }
  if (role == CustomColorRole::kOnExtended) {
    ColorFunction background = [name](const DynamicScheme& s) {
      return s.platform == Platform::kPhone
                 ? Custom2025(CustomColorRole::kExtended, name)
                 : Custom2025(CustomColorRole::kExtendedDim, name);
    };
    return Color("on_" + name, palette, InitialTone(background), false,
                 background, [](const DynamicScheme& s) {
                   return GetCurve(s.platform == Platform::kPhone ? 6.0 : 7.0);
                 });
  }
  if (role == CustomColorRole::kExtendedContainer) {
    return Color(
        name + "_container", palette,
        [name](const DynamicScheme& s) {
          if (s.platform == Platform::kWatch) return 30.0;
          const TonalPalette& extended = s.extended_palette.at(name);
          return s.is_dark ? TMinC(extended, 30.0, 93.0)
                           : TMaxC(extended, 0.0, 90.0);
        },
        true, [](const DynamicScheme& s) { return HighestSurface2025(s); },
        [](const DynamicScheme&) { return GetCurve(1.5); },
        [name](const DynamicScheme&) {
          return ToneDeltaPair(
              Custom2025(CustomColorRole::kExtendedContainer, name),
              Custom2025(CustomColorRole::kExtendedDim, name), 10.0,
              TonePolarity::kDarker, true, DeltaConstraint::kFarther);
        },
        [](const DynamicScheme& s) {
          return s.platform == Platform::kPhone;
        },
        [](const DynamicScheme& s) {
          return s.platform == Platform::kWatch;
        },
        [](const DynamicScheme& s) {
          return s.platform == Platform::kPhone && s.contrast_level > 0.0;
        });
  }
  if (role == CustomColorRole::kOnExtendedContainer) {
    ColorFunction background = [name](const DynamicScheme&) {
      return Custom2025(CustomColorRole::kExtendedContainer, name);
    };
    return Color("on_" + name + "_container", palette,
                 InitialTone(background), false, background,
                 [](const DynamicScheme& s) {
                   return GetCurve(s.platform == Platform::kPhone ? 4.5 : 7.0);
                 });
  }
  if (role == CustomColorRole::kExtendedFixed) {
    return Color(
        name + "_fixed", palette,
        [name](const DynamicScheme& s) {
          DynamicScheme light = s;
          light.is_dark = false;
          light.contrast_level = 0.0;
          return Custom2025(CustomColorRole::kExtendedContainer, name)
              .GetTone(light);
        },
        true, [](const DynamicScheme& s) { return HighestSurface2025(s); },
        [](const DynamicScheme&) { return GetCurve(1.5); }, std::nullopt,
        [](const DynamicScheme& s) {
          return s.platform == Platform::kPhone;
        },
        std::nullopt, [](const DynamicScheme& s) {
          return s.platform == Platform::kPhone && s.contrast_level > 0.0;
        });
  }
  if (role == CustomColorRole::kExtendedFixedDim) {
    return Color(
        name + "_fixed_dim", palette,
        [name](const DynamicScheme& s) {
          return Custom2025(CustomColorRole::kExtendedFixed, name).GetTone(s);
        },
        true, std::nullopt, std::nullopt,
        [name](const DynamicScheme&) {
          return ToneDeltaPair(
              Custom2025(CustomColorRole::kExtendedFixedDim, name),
              Custom2025(CustomColorRole::kExtendedFixed, name), 5.0,
              TonePolarity::kDarker, true, DeltaConstraint::kExact);
        });
  }
  if (role == CustomColorRole::kOnExtendedFixed ||
      role == CustomColorRole::kOnExtendedFixedVariant) {
    ColorFunction background = [name](const DynamicScheme&) {
      return Custom2025(CustomColorRole::kExtendedFixedDim, name);
    };
    bool variant = role == CustomColorRole::kOnExtendedFixedVariant;
    return Color("on_" + name +
                     (variant ? "_fixed_variant" : "_fixed"),
                 palette, InitialTone(background), false, background,
                 [variant](const DynamicScheme&) {
                   return GetCurve(variant ? 4.5 : 7.0);
                 });
  }
  ColorFunction background = [](const DynamicScheme&) {
    return GetColor2025("inverse_surface");
  };
  return Color(
      "inverse_" + name, palette,
      [name](const DynamicScheme& s) {
        return TMaxC(s.extended_palette.at(name));
      },
      false, background, [](const DynamicScheme& s) {
        return GetCurve(s.platform == Platform::kPhone ? 6.0 : 7.0);
      });
}

DynamicColor Custom2026(CustomColorRole role, const std::string& name) {
  if (role == CustomColorRole::kErrorFixed ||
      role == CustomColorRole::kExtendedFixed) {
    bool error = role == CustomColorRole::kErrorFixed;
    PaletteFunction palette = error
                                  ? PaletteFunction([](const DynamicScheme& s) {
                                      return s.error_palette;
                                    })
                                  : ExtendedPalette(name);
    CustomColorRole container = error ? CustomColorRole::kErrorFixed
                                      : CustomColorRole::kExtendedContainer;
    return Color(
        error ? "error_fixed" : name + "_fixed", palette,
        [error, name, container](const DynamicScheme& s) {
          DynamicScheme light = s;
          light.is_dark = false;
          light.contrast_level = 0.0;
          return error ? GetColor2026("error_container").GetTone(light)
                       : Custom2026(container, name).GetTone(light);
        },
        true, [](const DynamicScheme& s) { return HighestSurface2026(s); },
        [](const DynamicScheme&) { return GetCurve(1.5); }, std::nullopt,
        std::nullopt, std::nullopt,
        [](const DynamicScheme& s) { return s.contrast_level > 0.0; });
  }
  if (role == CustomColorRole::kErrorFixedDim ||
      role == CustomColorRole::kExtendedFixedDim) {
    bool error = role == CustomColorRole::kErrorFixedDim;
    std::string role_name = error ? "error" : name;
    PaletteFunction palette = error
                                  ? PaletteFunction([](const DynamicScheme& s) {
                                      return s.error_palette;
                                    })
                                  : ExtendedPalette(name);
    CustomColorRole fixed = error ? CustomColorRole::kErrorFixed
                                  : CustomColorRole::kExtendedFixed;
    return Color(
        role_name + "_fixed_dim", palette,
        [fixed, name](const DynamicScheme& s) {
          return Custom2026(fixed, name).GetTone(s);
        },
        true, [](const DynamicScheme& s) { return HighestSurface2026(s); },
        [](const DynamicScheme&) { return GetCurve(1.5); },
        [role, fixed, name](const DynamicScheme&) {
          return ToneDeltaPair(Custom2026(role, name), Custom2026(fixed, name),
                               5.0, TonePolarity::kDarker, true,
                               DeltaConstraint::kExact);
        },
        std::nullopt, std::nullopt,
        [](const DynamicScheme& s) { return s.contrast_level > 0.0; });
  }
  if (role == CustomColorRole::kOnErrorFixed ||
      role == CustomColorRole::kOnErrorFixedVariant ||
      role == CustomColorRole::kOnExtendedFixed ||
      role == CustomColorRole::kOnExtendedFixedVariant) {
    bool error = role == CustomColorRole::kOnErrorFixed ||
                 role == CustomColorRole::kOnErrorFixedVariant;
    bool variant = role == CustomColorRole::kOnErrorFixedVariant ||
                   role == CustomColorRole::kOnExtendedFixedVariant;
    std::string role_name = error ? "error" : name;
    PaletteFunction palette = error
                                  ? PaletteFunction([](const DynamicScheme& s) {
                                      return s.error_palette;
                                    })
                                  : ExtendedPalette(name);
    CustomColorRole fixed = error ? CustomColorRole::kErrorFixed
                                  : CustomColorRole::kExtendedFixed;
    CustomColorRole fixed_dim = error ? CustomColorRole::kErrorFixedDim
                                      : CustomColorRole::kExtendedFixedDim;
    ColorFunction background = [fixed, fixed_dim, name](const DynamicScheme& s) {
      return Custom2026(fixed, name).GetTone(s) > 57.0
                 ? Custom2026(fixed_dim, name)
                 : Custom2026(fixed, name);
    };
    return Color("on_" + role_name +
                     (variant ? "_fixed_variant" : "_fixed"),
                 palette, InitialTone(background), false, background,
                 [variant](const DynamicScheme&) {
                   return GetCurve(variant ? 4.5 : 7.0);
                 });
  }
  if (role == CustomColorRole::kInverseError) {
    ColorFunction background = [](const DynamicScheme&) {
      return GetColor2026("inverse_surface");
    };
    return Color(
        "inverse_error",
        [](const DynamicScheme& s) { return s.error_palette; },
        [](const DynamicScheme& s) { return TMaxC(s.error_palette); }, false,
        background, [](const DynamicScheme& s) {
          return GetCurve(s.platform == Platform::kPhone ? 6.0 : 7.0);
        });
  }

  PaletteFunction palette = ExtendedPalette(name);
  if (role == CustomColorRole::kExtended) {
    return Color(
        name, palette,
        [name](const DynamicScheme& s) {
          Hct source = s.extended_palette.at(name).get_key_color();
          return source.get_chroma() <= 12.0
                     ? s.is_dark ? 80.0 : 40.0
                     : source.get_tone();
        },
        true, [](const DynamicScheme& s) { return HighestSurface2026(s); },
        [](const DynamicScheme&) { return GetCurve(4.5); },
        [name](const DynamicScheme&) {
          return ToneDeltaPair(
              Custom2026(CustomColorRole::kExtendedContainer, name),
              Custom2026(CustomColorRole::kExtended, name), 5.0,
              TonePolarity::kRelativeLighter, true,
              DeltaConstraint::kFarther);
        },
        std::nullopt, [](const DynamicScheme& s) {
          return s.platform == Platform::kPhone;
        });
  }
  if (role == CustomColorRole::kExtendedDim) {
    DynamicColor color = Custom2026(CustomColorRole::kExtended, name);
    color.name_ = name + "_dim";
    return color;
  }
  if (role == CustomColorRole::kOnExtended) {
    ColorFunction background = [name](const DynamicScheme&) {
      return Custom2026(CustomColorRole::kExtended, name);
    };
    return Color("on_" + name, palette, InitialTone(background), false,
                 background,
                 [](const DynamicScheme&) { return GetCurve(6.0); });
  }
  if (role == CustomColorRole::kExtendedContainer) {
    return Color(
        name + "_container", palette,
        [name](const DynamicScheme& s) {
          Hct source = s.extended_palette.at(name).get_key_color();
          if (!s.is_dark && source.get_chroma() <= 12.0) return 90.0;
          return source.get_tone() > 55.0
                     ? std::clamp(source.get_tone(), 61.0, 90.0)
                     : std::clamp(source.get_tone(), 30.0, 49.0);
        },
        true, [](const DynamicScheme& s) { return HighestSurface2026(s); },
        [](const DynamicScheme&) { return GetCurve(1.5); }, std::nullopt,
        std::nullopt, std::nullopt,
        [](const DynamicScheme& s) { return s.contrast_level > 0.0; });
  }
  if (role == CustomColorRole::kOnExtendedContainer) {
    ColorFunction background = [name](const DynamicScheme&) {
      return Custom2026(CustomColorRole::kExtendedContainer, name);
    };
    return Color("on_" + name + "_container", palette,
                 InitialTone(background), false, background,
                 [](const DynamicScheme&) { return GetCurve(6.0); });
  }
  ColorFunction background = [](const DynamicScheme&) {
    return GetColor2026("inverse_surface");
  };
  return Color(
      "inverse_" + name, palette,
      [name](const DynamicScheme& s) {
        return TMaxC(s.extended_palette.at(name));
      },
      false, background, [](const DynamicScheme& s) {
        return GetCurve(s.platform == Platform::kPhone ? 6.0 : 7.0);
      });
}

}  // namespace

DynamicColor GetCustomColor(CustomColorRole role, const std::string& name) {
  DynamicColor color = Custom2025(role, name);
  color.versioned_color_ = [role, name](const DynamicScheme& s) {
    return s.spec_version == SpecVersion::k2026 ? Custom2026(role, name)
                                                : Custom2025(role, name);
  };
  return color;
}

}  // namespace material_color_utilities
