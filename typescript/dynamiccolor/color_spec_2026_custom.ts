import {ColorSpecDelegateImpl2026} from './color_spec_2026.js'
import {ContrastCurve} from './contrast_curve.js'
import {DynamicColor} from './dynamic_color.js'

/** Local CMF profile: inherit 2026 roles and override only deliberate changes. */
export class ColorSpecDelegateImpl2026Custom extends ColorSpecDelegateImpl2026 {
  override surfaceContainerLowest(): DynamicColor {
    const base = super.surfaceContainerLowest()
    const classicDarkTone = new ContrastCurve(4, 4, 2, 0)
    return DynamicColor.fromPalette({
      ...base,
      tone: (scheme) => scheme.isDark
        ? classicDarkTone.get(scheme.contrastLevel)
        : base.tone(scheme),
    })
  }
}
