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

import {Hct} from '../hct/hct.js';
import {TonalPalette} from '../palettes/tonal_palette.js';
import {clampDouble} from '../utils/math_utils.js';

import {ContrastCurve} from './contrast_curve.js';

export function findBestToneForChroma(
    hue: number, chroma: number, tone: number,
    byDecreasingTone: boolean): number {
  let answer = tone;
  let bestCandidate = Hct.from(hue, chroma, answer);
  while (bestCandidate.chroma < chroma) {
    if (tone < 0 || tone > 100) {
      break;
    }
    tone += byDecreasingTone ? -1.0 : 1.0;
    const newCandidate = Hct.from(hue, chroma, tone);
    if (bestCandidate.chroma < newCandidate.chroma) {
      bestCandidate = newCandidate;
      answer = tone;
    }
  }

  return answer;
}

export function tMaxC(
    palette: TonalPalette, lowerBound: number = 0, upperBound: number = 100,
    chromaMultiplier: number = 1): number {
  const answer = findBestToneForChroma(
      palette.hue, palette.chroma * chromaMultiplier, 100, true);
  return clampDouble(lowerBound, upperBound, answer);
}

export function tMinC(
    palette: TonalPalette, lowerBound: number = 0,
    upperBound: number = 100): number {
  const answer = findBestToneForChroma(
      palette.hue, palette.chroma, 0, false);
  return clampDouble(lowerBound, upperBound, answer);
}

export function getCurve(defaultContrast: number): ContrastCurve {
  if (defaultContrast === 1.5) {
    return new ContrastCurve(1.5, 1.5, 3, 5.5);
  }
  if (defaultContrast === 3) {
    return new ContrastCurve(3, 3, 4.5, 7);
  }
  if (defaultContrast === 4.5) {
    return new ContrastCurve(4.5, 4.5, 7, 11);
  }
  if (defaultContrast === 6) {
    return new ContrastCurve(6, 6, 7, 11);
  }
  if (defaultContrast === 7) {
    return new ContrastCurve(7, 7, 11, 21);
  }
  if (defaultContrast === 9) {
    return new ContrastCurve(9, 9, 11, 21);
  }
  if (defaultContrast === 11) {
    return new ContrastCurve(11, 11, 21, 21);
  }
  if (defaultContrast === 21) {
    return new ContrastCurve(21, 21, 21, 21);
  }
  return new ContrastCurve(defaultContrast, defaultContrast, 7, 21);
}
