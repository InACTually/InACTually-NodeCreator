
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

#include "NodeSelector.hpp"

#include "./../../../3rd/IconFontCppHeaders/IconsFontAwesome5.h"
#include <algorithm>

act::nc::NodeSelector::NodeSelector(std::map<std::string, std::vector<std::string>>& registryEntry,
									bool& isRegistryModified,
									int& selectedProcNode,
									std::string& selectedProcNodeName,
									std::string& selectedProcNodeGroup)
	: m_registryEntry(registryEntry)
	, m_isRegistryModified(isRegistryModified)
	, m_selectedProcNode(selectedProcNode)
	, m_selectedProcNodeName(selectedProcNodeName)
	, m_selectedProcNodeGroup(selectedProcNodeGroup)
{
}

void act::nc::NodeSelector::setOnSaveCallback(std::function<void()> callback)
{
    m_onSaveCallback = std::move(callback);
}

void act::nc::NodeSelector::setOnRemoveNodeCallback(std::function<void(const std::string&)> callback)
{
    m_onRemoveNodeCallback = std::move(callback);
}

void act::nc::NodeSelector::draw(float width, float height)
{
    ImGui::BeginChild("##NodeSelector", ImVec2(width, height), true);

    // --- Add Group ---
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 40);

    ImGui::InputTextWithHint("##NewGroup", "New group name...", m_newGroupNameBuf, IM_ARRAYSIZE(m_newGroupNameBuf));
    std::string newGroupName(m_newGroupNameBuf);
    ImGui::SameLine();

    if (ImGui::Button(ICON_FA_PLUS "##AddGroup")) {
        m_registryEntry[newGroupName] = std::vector<std::string>();
        m_newGroupNameBuf[0] = '\0';
        m_isRegistryModified = true;
    }

    // --- Save Button ---
    bool wasModified = m_isRegistryModified;
    if (!wasModified) ImGui::BeginDisabled();
    if (ImGui::Button(ICON_FA_SAVE " Update Registry", ImVec2(-1, 0))) {
        if (m_onSaveCallback) m_onSaveCallback();
        m_isRegistryModified = false;
    }
    if (!wasModified) ImGui::EndDisabled();

    ImGui::Separator();

    int n = 0;

    if (ImGui::CollapsingHeader("Templates"))
    {
        std::string nodeName = "Template for BasicProcNode";
        bool isSelectedD = (m_selectedProcNode == 0);
        // Template selection
        if (ImGui::Selectable(nodeName.c_str(), isSelectedD)) {
            m_selectedProcNode = 0;
            m_selectedProcNodeName = nodeName;
            m_selectedProcNodeGroup = "";
        }
    }

    for (auto it = m_registryEntry.begin(); it != m_registryEntry.end(); ++it) {
        const std::string& groupName = it->first;
        auto& groupEntries = it->second;
        int nStart = n;

        if (ImGui::CollapsingHeader((groupName + "##group").c_str()))
        {
            handleDragDropTarget(groupName);
            drawGroupContextMenu(groupName, groupEntries.empty());

            n = nStart;
            for (int i = 0; i < (int)groupEntries.size(); ++i)
            {
                n++;
                const auto& nodeName = groupEntries[i];
                bool isSelected = (m_selectedProcNode == n);
                if (ImGui::Selectable(("  " + nodeName + "##" + std::to_string(n)).c_str(), isSelected)) {
                    m_selectedProcNode = n;
                    m_selectedProcNodeName = nodeName;
                    m_selectedProcNodeGroup = groupName;
                }

                drawNodeContextMenu(groupName, nodeName);
                handleDragDropSource(groupName, nodeName);
            }
        }
        else
        {
            handleDragDropTarget(groupName);
            drawGroupContextMenu(groupName, groupEntries.empty());

            n += (int)groupEntries.size();
        }
    }

    // --- Apply deferred modifications ---
    handlePendingDeletions();

    ImGui::EndChild();
}

void act::nc::NodeSelector::handlePendingDeletions()
{
    if (!m_pendingDeleteGroup.empty()) {
        m_registryEntry.erase(m_pendingDeleteGroup);
        m_pendingDeleteGroup.clear();
    }
    if (!m_pendingRemoveNodeName.empty()) {
        auto it = m_registryEntry.find(m_pendingRemoveNodeGroup);
        if (it != m_registryEntry.end()) {
            auto& vec = it->second;
            vec.erase(std::remove(vec.begin(), vec.end(), m_pendingRemoveNodeName), vec.end());
        }
        if (m_onRemoveNodeCallback) m_onRemoveNodeCallback(m_pendingRemoveNodeName);
        m_pendingRemoveNodeGroup.clear();
        m_pendingRemoveNodeName.clear();
    }
    if (!m_pendingMoveTargetGroup.empty()) {
        auto& srcVec = m_registryEntry[m_dragSourceGroup];
        srcVec.erase(std::remove(srcVec.begin(), srcVec.end(), m_dragNodeName), srcVec.end());
        m_registryEntry[m_pendingMoveTargetGroup].push_back(m_dragNodeName);
        std::sort(m_registryEntry[m_pendingMoveTargetGroup].begin(), m_registryEntry[m_pendingMoveTargetGroup].end());
        m_isRegistryModified = true;
        m_pendingMoveTargetGroup.clear();
    }
}

void act::nc::NodeSelector::handleDragDropSource(const std::string& groupName, const std::string& nodeName)
{
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
        m_dragSourceGroup = groupName;
        m_dragNodeName = nodeName;
        ImGui::SetDragDropPayload("NODE_DND", "x", 1);
        ImGui::Text("%s", nodeName.c_str());
        ImGui::EndDragDropSource();
    }
}

void act::nc::NodeSelector::handleDragDropTarget(const std::string& targetGroup)
{
    if (ImGui::BeginDragDropTarget()) {
        if (ImGui::AcceptDragDropPayload("NODE_DND")) {
            if (m_dragSourceGroup != targetGroup) {
                m_pendingMoveTargetGroup = targetGroup;
            }
        }
        ImGui::EndDragDropTarget();
    }
}

void act::nc::NodeSelector::drawGroupContextMenu(const std::string& groupName, bool isEmpty)
{
    if (ImGui::BeginPopupContextItem(("##ctx_group_" + groupName).c_str())) {
        if (isEmpty) {
            if (ImGui::MenuItem(ICON_FA_TRASH " Delete Group")) {
                m_pendingDeleteGroup = groupName;
                m_isRegistryModified = true;
            }
        } else {
            ImGui::BeginDisabled();
            ImGui::MenuItem(ICON_FA_TRASH " Delete Group (not empty)");
            ImGui::EndDisabled();
        }
        ImGui::EndPopup();
    }
}

void act::nc::NodeSelector::drawNodeContextMenu(const std::string& groupName, const std::string& nodeName)
{
    if (ImGui::BeginPopupContextItem(("##ctx_node_" + groupName + "_" + nodeName).c_str())) {
        if (ImGui::MenuItem(ICON_FA_TIMES " Remove Node")) {
            m_pendingRemoveNodeGroup = groupName;
            m_pendingRemoveNodeName = nodeName;
            m_isRegistryModified = true;
        }
        ImGui::EndPopup();
    }
}
