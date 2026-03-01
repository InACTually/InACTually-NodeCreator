
/*
    InACTually
    > interactive theater for actual acts
    > this file is part of the "InACTually NodeCreator for InACTually Engine", a MediaServer for driving all technology

    Copyright (c) 2026 InACTually Community
    Licensed under the MIT License.
    See LICENSE file in the project root for full license information.

    This file is created and substantially modified: 2026

    contributors:
    Lars Engeln - mail@lars-engeln.de
*/

#include "NodeCreatorStyle.hpp"

#include "./../../3rd/IconFontCppHeaders/IconsFontAwesome5.h"
#include <cmath>

ImVec4 act::nc::NodeCreatorColors::darkPrimary(float alpha) {
    return ImVec4(0.0f, 0.7059f, 0.4710f, alpha);
}

ImVec4 act::nc::NodeCreatorColors::primary(float alpha) {
    return ImVec4(0.0f, 0.8745f, 0.498f, alpha);
}

ImVec4 act::nc::NodeCreatorColors::highlight(float alpha) {
    return ImVec4(0.1255f, 1.0f, 0.6235f, alpha);
}

ImVec4 act::nc::NodeCreatorColors::darkError(float alpha) {
    return ImVec4(0.9607f, 0.3451f, 0.3451f, alpha);
}

ImVec4 act::nc::NodeCreatorColors::error(float alpha) {
    return ImVec4(0.9412f, 0.4118f, 0.4118f, alpha);
}

ImVec4 act::nc::NodeCreatorColors::secondary(float alpha) {
    return ImVec4(0.0f, 0.8745f, 1.0f, alpha);
}

ImVec4 act::nc::NodeCreatorColors::additional(float alpha) {
    return ImVec4(0.8745f, 0.0f, 0.31175f, alpha);
}

ImVec4 act::nc::NodeCreatorColors::gray(float alpha) {
    return ImVec4(0.5f, 0.5f, 0.5f, alpha);
}

ImVec4 act::nc::NodeCreatorColors::background(float alpha) {
    return ImVec4(0.06f, 0.06f, 0.1f, alpha);
}

void act::nc::NodeCreatorStyle::apply() {
    float fontSize = 18.0f;

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    ImFontConfig config;
    config.SizePixels = 30;
    config.OversampleH = config.OversampleV = 2;
    config.PixelSnapH = true;

    io.Fonts->AddFontFromFileTTF("..//..//assets//3rd//fonts//OpenSans-Medium.ttf", config.SizePixels, &config);

    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMinAdvanceX = 22.0f;
    ImFont* font = io.Fonts->AddFontFromFileTTF("..//..//assets//3rd//fonts//fa-solid-900.ttf", config.SizePixels - 8, &icons_config, icons_ranges);

    font->Scale = fontSize / 30.0f;

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    /// 0 = FLAT APPEARENCE
    /// 1 = MORE "3D" LOOK
    int is3D = 0;

    ImVec4 bg = NodeCreatorColors::background(1.000f);
    ImVec4 primary = NodeCreatorColors::primary(1.000f);
    ImVec4 highlight = NodeCreatorColors::highlight(1.000f);
    ImVec4 secondary = NodeCreatorColors::secondary(1.000f);
    ImVec4 additional = NodeCreatorColors::additional(1.000f);

    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = bg;
    colors[ImGuiCol_FrameBgHovered] = bg;
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = primary;
    colors[ImGuiCol_CheckMark] = primary;
    colors[ImGuiCol_SliderGrab] = primary;
    colors[ImGuiCol_SliderGrabActive] = highlight;
    colors[ImGuiCol_Button] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.45f, 0.49f, 1.00f);
    colors[ImGuiCol_ButtonActive] = NodeCreatorColors::primary(1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = NodeCreatorColors::primary(0.78f);
    colors[ImGuiCol_SeparatorActive] = primary;
    colors[ImGuiCol_ResizeGrip] = NodeCreatorColors::primary(0.25f);
    colors[ImGuiCol_ResizeGripHovered] = NodeCreatorColors::primary(0.67f);
    colors[ImGuiCol_ResizeGripActive] = NodeCreatorColors::primary(0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.45f, 0.49f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.30f, 0.35f, 0.39f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = secondary;
    colors[ImGuiCol_PlotHistogram] = primary;
    colors[ImGuiCol_PlotHistogramHovered] = highlight;
    colors[ImGuiCol_TextSelectedBg] = NodeCreatorColors::primary(0.35f);
    colors[ImGuiCol_DragDropTarget] = secondary;
    colors[ImGuiCol_NavHighlight] = primary;
    colors[ImGuiCol_NavWindowingHighlight] = NodeCreatorColors::primary(0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    style.PopupRounding = 3;
    style.WindowPadding = ImVec2(4, 4);

    int paddingX = round(fontSize * (2.0f / 3.0f));
    int paddingY = round(paddingX * 0.625f);

    style.FramePadding = ImVec2(paddingX, paddingY);
    style.ItemSpacing = ImVec2(6, 2);
    style.ScrollbarSize = 18;

    style.WindowBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = is3D;

    style.WindowRounding = 3;
    style.ChildRounding = 3;
    style.FrameRounding = 3;
    style.ScrollbarRounding = 2;
    style.GrabRounding = 3;

#ifdef IMGUI_HAS_DOCK
    style.TabBorderSize = is3D;
    style.TabRounding = 3;

    colors[ImGuiCol_DockingEmptyBg] = bg;
    colors[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
#endif
}
