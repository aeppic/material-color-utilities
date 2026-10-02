import {ColorSpecDelegateImpl2026} from './color_spec_2026.js'
import {DynamicColor} from './dynamic_color.js'

function withFixedDarkTone(baseColor: DynamicColor, darkTone: number): DynamicColor {
  return DynamicColor.fromPalette({
    ...baseColor,
    tone: (scheme) => scheme.isDark ? darkTone : baseColor.tone(scheme),
  })
}

export class ColorSpecDelegateImpl2026Custom extends ColorSpecDelegateImpl2026 {
  override surfaceContainerLowest(): DynamicColor {
    return withFixedDarkTone(super.surfaceContainerLowest(), 7)
  }

  override surfaceContainerLow(): DynamicColor {
    return withFixedDarkTone(super.surfaceContainerLow(), 9)
  }

  override surfaceContainer(): DynamicColor {
    return withFixedDarkTone(super.surfaceContainer(), 12)
  }

  override surfaceContainerHigh(): DynamicColor {
    return withFixedDarkTone(super.surfaceContainerHigh(), 14)
  }

  override surfaceContainerHighest(): DynamicColor {
    return withFixedDarkTone(super.surfaceContainerHighest(), 16)
  }
}
