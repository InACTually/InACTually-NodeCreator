
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

#include "NodeCreator.hpp"

#include "imgui.h"

#include "./../../3rd/IconFontCppHeaders/IconsFontAwesome5.h";
#include <cmath>

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm> 
#include <cctype> 

std::string removeWhitespace(std::string s) { 
    s.erase(std::remove_if(s.begin(), s.end(), [](unsigned char c) { 
        return std::isspace(c); 
    }), s.end()); 
    return s;
}

#include <regex>

static std::string procSourceBasePath = "..//..//src//processing//";
static std::string procIncludeBasePath = "..//..//include//processing//";
static std::string procFilePath = procSourceBasePath + "ProcNodeRegistry.cpp";



void act::nc::NodeCreator::init()
{
	NodeCreatorStyle::apply();
	loadProcNodeRegistry();
	m_nodeSelector = std::make_unique<NodeSelector>(m_registryEntry, m_isRegistryModified, m_selectedProcNode, m_selectedProcNodeName, m_selectedProcNodeGroup);
	m_nodeSelector->setOnSaveCallback([this]() { saveProcNodeRegistry(); });
	m_nodeSelector->setOnRemoveNodeCallback([this](const std::string& nodeName) { m_nodeEditor->vcxproj().removeNode(nodeName); });
	m_nodeEditor = std::make_unique<NodeEditor>(m_registryEntry, m_isRegistryModified, m_selectedProcNode, m_selectedProcNodeName, m_selectedProcNodeGroup);
	m_nodeEditor->setOnGenerateCallback([this]() { saveProcNodeRegistry(); m_isRegistryModified = false; });
}

void act::nc::NodeCreator::draw() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    ImGui::Begin("Node Creator", nullptr, window_flags);

    float totalWidth = ImGui::GetContentRegionAvail().x;
    float totalHeight = ImGui::GetContentRegionAvail().y;
    float selectorWidth = totalWidth * 0.3f;

    m_nodeSelector->draw(selectorWidth, totalHeight);
    ImGui::SameLine();
    m_nodeEditor->draw(totalHeight);

    ImGui::End();
    ImGui::PopStyleVar();
}

void act::nc::NodeCreator::loadProcNodeRegistry()
{
    std::fstream procFile(procFilePath, std::ios::in);

    if (!procFile.is_open()) {
        std::cerr << "Failed to open file: " << procFilePath << std::endl;
        return;
    }

    std::string line;
    int lineNo = 0;

    // [1] = Dateiname innerhalb
    std::regex includeRegex("^\\s*#\\s*include\\s*\"([^\"]+)\"");
    // [1] = group, [G2] = name
    std::regex registryRegex(R"regex(act::proc::ProcNodeRegistry::add\s*\(\s*"([^"]*)"\s*,\s*"([^"]*)")regex");

	ReadingState readingState = ReadingState::RS_READING_BEGIN;

    while (std::getline(procFile, line)) {
        ++lineNo;
        std::smatch match;

        switch(readingState) {
            case ReadingState::RS_READING_BEGIN:
                if (line.find("#include \"ProcNodeRegistry.hpp\"") == std::string::npos) {
					m_beginLines.push_back(line);
                    break;
                }
				m_beginLines.push_back(line);
				readingState = ReadingState::RS_READING_INCLUDES;
                break;
            case ReadingState::RS_READING_INCLUDES:
                if (removeWhitespace(line).empty() || line.find("//") != std::string::npos)
                    break;
                if (std::regex_search(line, match, includeRegex) && match.size() > 1) {
                    m_includes.push_back(match[1].str());
                    break;
                }
                else {
					readingState = ReadingState::RS_READING_MIDDLE;
                }
            case ReadingState::RS_READING_MIDDLE:
                if (removeWhitespace(line).empty() || line.find("//") != std::string::npos)
                    break;
                if (line.find("act::proc::ProcNodeRegistry::add") == std::string::npos) {
                    m_middlegLines.push_back(line);
                    break;
                }
                else
                    readingState = ReadingState::RS_READING_REGISTRY;

			case ReadingState::RS_READING_REGISTRY:
                if (removeWhitespace(line).empty() || line.find("//") != std::string::npos)
                    break;
                if (std::regex_search(line, match, registryRegex) && match.size() > 2) {
                    if (m_registryEntry.find(match[1].str()) == m_registryEntry.end()) {
                        m_registryEntry[match[1].str()] = std::vector<std::string>();
                    }
                    m_registryEntry[match[1].str()].push_back(match[2].str());
                    break;
                }
                else
					readingState = ReadingState::RS_READING_END;
				
            case ReadingState::RS_READING_END:
                m_endLines.push_back(line);
                break; 
		}
        
    }

    procFile.close();

    // Sort nodes within each group alphabetically
    for (auto& [group, nodes] : m_registryEntry) {
        std::sort(nodes.begin(), nodes.end());
    }
}

void act::nc::NodeCreator::saveProcNodeRegistry()
{
    std::ofstream out(procFilePath, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Failed to open file for writing: " << procFilePath << std::endl;
        return;
    }

    // 1) write begin lines (everything up to and including the ProcNodeRegistry.hpp include)
    for (const auto& l : m_beginLines) {
        out << l << '\n';
    }
    out << '\n';

    // 2) write includes (quoted) — keep original order
    for (const auto& pair : m_registryEntry) {
        const std::string& group = pair.first;
        out << "// " << group << '\n';
        for (const auto& name : pair.second) {
            out << "#include \"" << name << "ProcNode.hpp\"" << '\n';
        }
        out << '\n';
    }

    // 3) write middle lines (between includes and registry start)
    for (const auto& l : m_middlegLines) {
        out << l << '\n';
    }

    out << '\n';

    // 4) write registry entries grouped by group name
    //    We reconstruct lines as: act::proc::ProcNodeRegistry::add("group", "Name", act::proc::NameProcNode::create);
    for (const auto& pair : m_registryEntry) {
        const std::string& group = pair.first;
        for (const auto& name : pair.second) {
            // build class name from node name: Name -> NameProcNode
            std::string className = name + "ProcNode";
            out << "    act::proc::ProcNodeRegistry::add(\"" << group << "\", \"" << name << "\", act::proc::" << className << "::create);" << '\n';
        }
        out << '\n';
    }

    // 5) write end lines (everything after registry)
    for (const auto& l : m_endLines) {
        out << l << '\n';
    }

    out.close();

    m_nodeEditor->vcxprojFilters().sync(m_registryEntry);
}
