/**
 * @license
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

import {clampDouble} from '../utils/math_utils.js';

import {ColorSpecDelegateImpl2025} from './color_spec_2025.js';
import {ColorSpecDelegateImpl2026} from './color_spec_2026.js';
import {getCurve, tMaxC, tMinC} from './color_spec_helpers.js';
import {ContrastCurve} from './contrast_curve.js';
import {DynamicColor, extendSpecVersion} from './dynamic_color.js';
import {ToneDeltaPair} from './tone_delta_pair.js';

const CUSTOM_CMF_PROFILE = 'cmf-2026-custom';

class ColorSpecDelegateImplCustom2025 extends ColorSpecDelegateImpl2025 {
  errorFixed(): DynamicColor {
    return DynamicColor.fromPalette({
      name: 'error_fixed',
      palette: (s) => s.errorPalette,
      tone: (s) => this.errorContainer().getTone(
          Object.assign({}, s, {isDark: false, contrastLevel: 0})),
      isBackground: true,
      background: (s) =>
        s.platform === 'phone' ? this.highestSurface(s) : undefined,
      contrastCurve: (s) =>
        s.platform === 'phone' && s.contrastLevel > 0 ?
          getCurve(1.5) :
          undefined,
    });
  }

  errorFixedDim(): DynamicColor {
    return DynamicColor.fromPalette({
      name: 'error_fixed_dim',
      palette: (s) => s.errorPalette,
      tone: (s) => this.errorFixed().getTone(s),
      isBackground: true,
      toneDeltaPair: (s) => new ToneDeltaPair(
          this.errorFixedDim(), this.errorFixed(), 5, 'darker', true, 'exact'),
    });
  }

  onErrorFixed(): DynamicColor {
    return DynamicColor.fromPalette({
      name: 'on_error_fixed',
      palette: (s) => s.errorPalette,
      background: (s) => this.errorFixedDim(),
      contrastCurve: (s) => getCurve(7),
    });
  }

  onErrorFixedVariant(): DynamicColor {
    return DynamicColor.fromPalette({
      name: 'on_error_fixed_variant',
      palette: (s) => s.errorPalette,
      background: (s) => this.errorFixedDim(),
      contrastCurve: (s) => getCurve(4.5),
    });
  }

  inverseError(): DynamicColor {
    return DynamicColor.fromPalette({
      name: 'inverse_error',
      palette: (s) => s.errorPalette,
      tone: (s) => tMaxC(s.errorPalette),
      background: (s) => this.inverseSurface(),
      contrastCurve: (s) =>
        s.platform === 'phone' ? getCurve(6) : getCurve(7),
    });
  }

  extended(name: string): DynamicColor {
    return DynamicColor.fromPalette({
      name,
      palette: (s) => s.extendedPalette[name],
      tone: (s) => {
        if (s.platform === 'phone') {
          return s.isDark ?
            tMinC(s.extendedPalette[name], 0, 98) :
            tMaxC(s.extendedPalette[name]);
        }
        return tMinC(s.extendedPalette[name]);
      },
      isBackground: true,
      background: (s) =>
        s.platform === 'phone' ? this.highestSurface(s) :
                                 this.surfaceContainerHigh(),
      contrastCurve: (s) =>
        s.platform === 'phone' ? getCurve(4.5) : getCurve(7),
      toneDeltaPair: (s) => s.platform === 'phone' ?
        new ToneDeltaPair(
            this.extendedContainer(name), this.extended(name), 5,
            'relative_lighter', true, 'farther') :
        undefined,
    });
  }

  extendedDim(name: string): DynamicColor {
    return DynamicColor.fromPalette({
      name: `${name}_dim`,
      palette: (s) => s.extendedPalette[name],
      tone: (s) => tMinC(s.extendedPalette[name]),
      isBackground: true,
      background: (s) => this.surfaceContainerHigh(),
      contrastCurve: (s) => getCurve(4.5),
      toneDeltaPair: (s) => new ToneDeltaPair(
          this.extendedDim(name), this.extended(name), 5, 'darker', true,
          'farther'),
    });
  }

  onExtended(name: string): DynamicColor {
    return DynamicColor.fromPalette({
      name: `on_${name}`,
      palette: (s) => s.extendedPalette[name],
      background: (s) =>
        s.platform === 'phone' ? this.extended(name) : this.extendedDim(name),
      contrastCurve: (s) =>
        s.platform === 'phone' ? getCurve(6) : getCurve(7),
    });
  }

  extendedContainer(name: string): DynamicColor {
    return DynamicColor.fromPalette({
      name: `${name}_container`,
      palette: (s) => s.extendedPalette[name],
      tone: (s) => {
        if (s.platform === 'watch') {
          return 30;
        }
        return s.isDark ?
          tMinC(s.extendedPalette[name], 30, 93) :
          tMaxC(s.extendedPalette[name], 0, 90);
      },
      isBackground: true,
      background: (s) =>
        s.platform === 'phone' ? this.highestSurface(s) : undefined,
      toneDeltaPair: (s) => s.platform === 'watch' ?
        new ToneDeltaPair(
            this.extendedContainer(name), this.extendedDim(name), 10, 'darker',
            true, 'farther') :
        undefined,
      contrastCurve: (s) =>
        s.platform === 'phone' && s.contrastLevel > 0 ?
          getCurve(1.5) :
          undefined,
    });
  }

  onExtendedContainer(name: string): DynamicColor {
    return DynamicColor.fromPalette({
      name: `on_${name}_container`,
      palette: (s) => s.extendedPalette[name],
      background: (s) => this.extendedContainer(name),
      contrastCurve: (s) =>
        s.platform === 'phone' ? getCurve(4.5) : getCurve(7),
    });
  }

  extendedFixed(name: string): DynamicColor {
    return DynamicColor.fromPalette({
      name: `${name}_fixed`,
      palette: (s) => s.extendedPalette[name],
      tone: (s) => this.extendedContainer(name).getTone(
          Object.assign({}, s, {isDark: false, contrastLevel: 0})),
      isBackground: true,
      background: (s) =>
        s.platform === 'phone' ? this.highestSurface(s) : undefined,
      contrastCurve: (s) =>
        s.platform === 'phone' && s.contrastLevel > 0 ?
          getCurve(1.5) :
          undefined,
    });
  }

  extendedFixedDim(name: string): DynamicColor {
    return DynamicColor.fromPalette({
      name: `${name}_fixed_dim`,
      palette: (s) => s.extendedPalette[name],
      tone: (s) => this.extendedFixed(name).getTone(s),
      isBackground: true,
      toneDeltaPair: (s) => new ToneDeltaPair(
          this.extendedFixedDim(name), this.extendedFixed(name), 5, 'darker',
          true, 'exact'),
    });
  }

  onExtendedFixed(name: string): DynamicColor {
    return DynamicColor.fromPalette({
      name: `on_${name}_fixed`,
      palette: (s) => s.extendedPalette[name],
      background: (s) => this.extendedFixedDim(name),
      contrastCurve: (s) => getCurve(7),
    });
  }

  onExtendedFixedVariant(name: string): DynamicColor {
    return DynamicColor.fromPalette({
      name: `on_${name}_fixed_variant`,
      palette: (s) => s.extendedPalette[name],
      background: (s) => this.extendedFixedDim(name),
      contrastCurve: (s) => getCurve(4.5),
    });
  }

  inverseExtended(name: string): DynamicColor {
    return DynamicColor.fromPalette({
      name: `inverse_${name}`,
      palette: (s) => s.extendedPalette[name],
      tone: (s) => tMaxC(s.extendedPalette[name]),
      background: (s) => this.inverseSurface(),
      contrastCurve: (s) =>
        s.platform === 'phone' ? getCurve(6) : getCurve(7),
    });
  }
}

const custom2025 = new ColorSpecDelegateImplCustom2025();

export class ColorSpecDelegateImplCustom extends ColorSpecDelegateImpl2026 {
  override inverseSurface(): DynamicColor {
    const standardInverseSurface = super.inverseSurface();
    return DynamicColor.fromPalette({
      name: standardInverseSurface.name,
      palette: standardInverseSurface.palette,
      tone: (s) => {
        if (s.profile === CUSTOM_CMF_PROFILE) {
          return s.isDark ? 90 : 20;
        }
        return standardInverseSurface.tone(s);
      },
      isBackground: standardInverseSurface.isBackground,
      chromaMultiplier: (s) => {
        if (s.profile === CUSTOM_CMF_PROFILE) {
          return 1;
        }
        return standardInverseSurface.chromaMultiplier?.(s) ?? 1;
      },
    });
  }

  override inverseOnSurface(): DynamicColor {
    const standardInverseOnSurface = super.inverseOnSurface();
    return DynamicColor.fromPalette({
      name: standardInverseOnSurface.name,
      palette: standardInverseOnSurface.palette,
      tone: (s) => {
        if (s.profile === CUSTOM_CMF_PROFILE) {
          return s.isDark ? 20 : 95;
        }
        return standardInverseOnSurface.tone(s);
      },
      background: (s) => this.inverseSurface(),
      contrastCurve: (s) => {
        if (s.profile === CUSTOM_CMF_PROFILE) {
          return new ContrastCurve(4.5, 7, 11, 21);
        }
        return standardInverseOnSurface.contrastCurve?.(s);
      },
    });
  }

  errorFixed(): DynamicColor {
    const color2026 = DynamicColor.fromPalette({
      name: 'error_fixed',
      palette: (s) => s.errorPalette,
      tone: (s) => this.errorContainer().getTone(
          Object.assign({}, s, {isDark: false, contrastLevel: 0})),
      isBackground: true,
      background: (s) => this.highestSurface(s),
      contrastCurve: (s) => s.contrastLevel > 0 ? getCurve(1.5) : undefined,
    });
    return extendSpecVersion(custom2025.errorFixed(), '2026', color2026);
  }

  errorFixedDim(): DynamicColor {
    const color2026 = DynamicColor.fromPalette({
      name: 'error_fixed_dim',
      palette: (s) => s.errorPalette,
      tone: (s) => this.errorFixed().getTone(s),
      isBackground: true,
      background: (s) => this.highestSurface(s),
      toneDeltaPair: (s) => new ToneDeltaPair(
          this.errorFixedDim(), this.errorFixed(), 5, 'darker', true, 'exact'),
      contrastCurve: (s) => s.contrastLevel > 0 ? getCurve(1.5) : undefined,
    });
    return extendSpecVersion(custom2025.errorFixedDim(), '2026', color2026);
  }

  onErrorFixed(): DynamicColor {
    const color2026 = DynamicColor.fromPalette({
      name: 'on_error_fixed',
      palette: (s) => s.errorPalette,
      background: (s) => this.errorFixed().getTone(s) > 57 ?
          this.errorFixedDim() : this.errorFixed(),
      contrastCurve: (s) => getCurve(7),
    });
    return extendSpecVersion(custom2025.onErrorFixed(), '2026', color2026);
  }

  onErrorFixedVariant(): DynamicColor {
    const color2026 = DynamicColor.fromPalette({
      name: 'on_error_fixed_variant',
      palette: (s) => s.errorPalette,
      background: (s) => this.errorFixed().getTone(s) > 57 ?
          this.errorFixedDim() : this.errorFixed(),
      contrastCurve: (s) => getCurve(4.5),
    });
    return extendSpecVersion(
        custom2025.onErrorFixedVariant(), '2026', color2026);
  }

  inverseError(): DynamicColor {
    const color2026 = DynamicColor.fromPalette({
      name: 'inverse_error',
      palette: (s) => s.errorPalette,
      tone: (s) => tMaxC(s.errorPalette),
      background: (s) => this.inverseSurface(),
      contrastCurve: (s) => s.platform === 'phone' ?
        getCurve(6) :
        getCurve(7),
    });
    return extendSpecVersion(custom2025.inverseError(), '2026', color2026);
  }

  extended(name: string): DynamicColor {
    const color2026 = DynamicColor.fromPalette({
      name,
      palette: (s) => s.extendedPalette[name],
      tone: (s) => {
        const source = s.extendedPalette[name].keyColor;
        return source.chroma <= 12 ? (s.isDark ? 80 : 40) : source.tone;
      },
      isBackground: true,
      background: (s) => this.highestSurface(s),
      contrastCurve: (s) => getCurve(4.5),
      toneDeltaPair: (s) => s.platform === 'phone' ?
        new ToneDeltaPair(
            this.extendedContainer(name), this.extended(name), 5,
            'relative_lighter', true, 'farther') :
        undefined,
    });
    return extendSpecVersion(custom2025.extended(name), '2026', color2026);
  }

  extendedDim(name: string): DynamicColor {
    const color2026 = Object.assign(this.extended(name).clone(), {
      name: `${name}_dim`,
    });
    return extendSpecVersion(custom2025.extendedDim(name), '2026', color2026);
  }

  onExtended(name: string): DynamicColor {
    const color2026 = DynamicColor.fromPalette({
      name: `on_${name}`,
      palette: (s) => s.extendedPalette[name],
      background: (s) => this.extended(name),
      contrastCurve: (s) => getCurve(6),
    });
    return extendSpecVersion(custom2025.onExtended(name), '2026', color2026);
  }

  extendedContainer(name: string): DynamicColor {
    const color2026 = DynamicColor.fromPalette({
      name: `${name}_container`,
      palette: (s) => s.extendedPalette[name],
      tone: (s) => {
        const source = s.extendedPalette[name].keyColor;
        if (!s.isDark && source.chroma <= 12) {
          return 90;
        }
        return source.tone > 55 ? clampDouble(61, 90, source.tone) :
                                  clampDouble(30, 49, source.tone);
      },
      isBackground: true,
      background: (s) => this.highestSurface(s),
      contrastCurve: (s) => s.contrastLevel > 0 ? getCurve(1.5) : undefined,
    });
    return extendSpecVersion(
        custom2025.extendedContainer(name), '2026', color2026);
  }

  onExtendedContainer(name: string): DynamicColor {
    const color2026 = DynamicColor.fromPalette({
      name: `on_${name}_container`,
      palette: (s) => s.extendedPalette[name],
      background: (s) => this.extendedContainer(name),
      contrastCurve: (s) => getCurve(6),
    });
    return extendSpecVersion(
        custom2025.onExtendedContainer(name), '2026', color2026);
  }

  extendedFixed(name: string): DynamicColor {
    const color2026 = DynamicColor.fromPalette({
      name: `${name}_fixed`,
      palette: (s) => s.extendedPalette[name],
      tone: (s) => this.extendedContainer(name).getTone(
          Object.assign({}, s, {isDark: false, contrastLevel: 0})),
      isBackground: true,
      background: (s) => this.highestSurface(s),
      contrastCurve: (s) => s.contrastLevel > 0 ? getCurve(1.5) : undefined,
    });
    return extendSpecVersion(
        custom2025.extendedFixed(name), '2026', color2026);
  }

  extendedFixedDim(name: string): DynamicColor {
    const color2026 = DynamicColor.fromPalette({
      name: `${name}_fixed_dim`,
      palette: (s) => s.extendedPalette[name],
      tone: (s) => this.extendedFixed(name).getTone(s),
      isBackground: true,
      background: (s) => this.highestSurface(s),
      toneDeltaPair: (s) => new ToneDeltaPair(
          this.extendedFixedDim(name), this.extendedFixed(name), 5, 'darker',
          true, 'exact'),
      contrastCurve: (s) => s.contrastLevel > 0 ? getCurve(1.5) : undefined,
    });
    return extendSpecVersion(
        custom2025.extendedFixedDim(name), '2026', color2026);
  }

  onExtendedFixed(name: string): DynamicColor {
    const color2026 = DynamicColor.fromPalette({
      name: `on_${name}_fixed`,
      palette: (s) => s.extendedPalette[name],
      background: (s) => this.extendedFixed(name).getTone(s) > 57 ?
          this.extendedFixedDim(name) : this.extendedFixed(name),
      contrastCurve: (s) => getCurve(7),
    });
    return extendSpecVersion(
        custom2025.onExtendedFixed(name), '2026', color2026);
  }

  onExtendedFixedVariant(name: string): DynamicColor {
    const color2026 = DynamicColor.fromPalette({
      name: `on_${name}_fixed_variant`,
      palette: (s) => s.extendedPalette[name],
      background: (s) => this.extendedFixed(name).getTone(s) > 57 ?
          this.extendedFixedDim(name) : this.extendedFixed(name),
      contrastCurve: (s) => getCurve(4.5),
    });
    return extendSpecVersion(
        custom2025.onExtendedFixedVariant(name), '2026', color2026);
  }

  inverseExtended(name: string): DynamicColor {
    const color2026 = DynamicColor.fromPalette({
      name: `inverse_${name}`,
      palette: (s) => s.extendedPalette[name],
      tone: (s) => tMaxC(s.extendedPalette[name]),
      background: (s) => this.inverseSurface(),
      contrastCurve: (s) => s.platform === 'phone' ?
        getCurve(6) :
        getCurve(7),
    });
    return extendSpecVersion(
        custom2025.inverseExtended(name), '2026', color2026);
  }
}
