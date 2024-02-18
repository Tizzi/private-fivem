// Copyright (c) 2022-2023 tsl0922. All rights reserved.
// SPDX-License-Identifier: GPL-2.0-only

#include <vector>
#include <imgui.h>

namespace ImGui {
std::vector<const char*> Themes();
void SetTheme(const char* theme, ImGuiStyle* dst = nullptr, bool rounding = true, bool shadow = true);
void StyleColorsSpectrum(ImGuiStyle* dst = nullptr);
void StyleColorsDracula(ImGuiStyle* dst = nullptr);
void StyleColorsDeepDark(ImGuiStyle* dst = nullptr);
void StyleColorsTest1(ImGuiStyle* dst = nullptr);
void StyleColorsTest2(ImGuiStyle* dst = nullptr);
void StyleColorsTest3(ImGuiStyle* dst = nullptr);
void StyleDarkAndPurple(ImGuiStyle* dst);

}  // namespace ImGui