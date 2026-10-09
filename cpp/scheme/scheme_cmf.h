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

#ifndef CPP_SCHEME_SCHEME_CMF_H_
#define CPP_SCHEME_SCHEME_CMF_H_

#include <optional>
#include <vector>

#include "cpp/cam/hct.h"
#include "cpp/dynamiccolor/dynamic_scheme.h"

namespace material_color_utilities {

struct SchemeCmf : public DynamicScheme {
  SchemeCmf(Hct source_color_hct, bool is_dark, double contrast_level,
            Platform platform = Platform::kPhone,
            CmfProfile profile = CmfProfile::k2026,
            std::optional<double> neutral_chroma_percent = std::nullopt,
            std::optional<double> neutral_chroma_cap = std::nullopt);
  SchemeCmf(std::vector<Hct> source_color_hcts, bool is_dark,
            double contrast_level, Platform platform = Platform::kPhone,
            CmfProfile profile = CmfProfile::k2026,
            std::optional<double> neutral_chroma_percent = std::nullopt,
            std::optional<double> neutral_chroma_cap = std::nullopt);
};

}  // namespace material_color_utilities

#endif  // CPP_SCHEME_SCHEME_CMF_H_
