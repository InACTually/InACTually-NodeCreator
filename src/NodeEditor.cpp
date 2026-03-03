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

#include "NodeEditor.hpp"

#include "./../../../3rd/IconFontCppHeaders/IconsFontAwesome5.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>


static std::string procSourceBasePath = "..//..//src//processing//";
static std::string procIncludeBasePath = "..//..//include//processing//";
static std::string projBasePath = "..//..//vc2022//";
static std::string procProjPath = projBasePath + "IA_Processing.vcxproj";
static std::string procProjFilterPath = procProjPath + ".filters";

static std::string getCurrentYear();
static std::string buildFileHeader(const std::string& contributorName, const std::string& contributorContact);


act::nc::NodeEditor::NodeEditor(std::map<std::string, std::vector<std::string>>& registryEntry,
								bool& isRegistryModified,
								int& selectedProcNode,
								std::string& selectedProcNodeName,
								std::string& selectedProcNodeGroup)
	: m_registryEntry(registryEntry)
	, m_isRegistryModified(isRegistryModified)
	, m_selectedProcNode(selectedProcNode)
	, m_selectedProcNodeName(selectedProcNodeName)
	, m_selectedProcNodeGroup(selectedProcNodeGroup)
	, m_vcxprojManager(procProjPath)
	, m_vcxprojFiltersManager(procProjFilterPath)
{
}

void act::nc::NodeEditor::setOnGenerateCallback(std::function<void()> callback)
{
	m_onGenerateCallback = std::move(callback);
}

const char* act::nc::NodeEditor::paramTypeToCppType(int typeIndex)
{
	switch (typeIndex) {
		case 0: return "float";
		case 1: return "int";
		case 2: return "bool";
		case 3: return "std::string";
		case 4: return "double";
		case 5: return "ci::vec2";
		case 6: return "ci::ivec2";
		case 7: return "ci::vec3";
		case 8: return "ci::Color";
		case 9: return "ci::quat";
		default: return "float";
	}
}

std::string act::nc::NodeEditor::portTypeToOutputDecl(proc::PortType type)
{
	switch (type) {
		case proc::PT_JSON:        return "OutputPortRef<ci::Json>";
		case proc::PT_BOOL:        return "OutputPortRef<bool>";
		case proc::PT_NUMBER:      return "OutputPortRef<number>";
		case proc::PT_NUMBERLIST:  return "OutputPortRef<std::vector<number>>";
		case proc::PT_VEC2:        return "OutputPortRef<vec2>";
		case proc::PT_VEC2LIST:    return "OutputPortRef<std::vector<vec2>>";
		case proc::PT_VEC3:        return "OutputPortRef<vec3>";
		case proc::PT_VEC3LIST:    return "OutputPortRef<std::vector<vec3>>";
		case proc::PT_QUAT:        return "OutputPortRef<quat>";
		case proc::PT_COLOR:       return "OutputPortRef<ci::Color>";
		case proc::PT_COLORLIST:   return "OutputPortRef<std::vector<ci::Color>>";
		case proc::PT_TEXT:        return "OutputPortRef<std::string>";
		case proc::PT_IMAGE:       return "ImageOutputPortRef";
		case proc::PT_AUDIO:       return "OutputPortRef<ci::audio::BufferRef>";
		case proc::PT_AUDIONODE:   return "OutputPortRef<ci::audio::NodeRef>";
		case proc::PT_FEATURE:     return "OutputPortRef<feature>";
		case proc::PT_BODY:        return "OutputPortRef<room::BodyRef>";
		case proc::PT_BODYLIST:    return "OutputPortRef<room::BodyRefList>";
		default: return "OutputPortRef<float>";
	}
}

std::string act::nc::NodeEditor::portTypeToCreateOutput(proc::PortType type)
{
	switch (type) {
		case proc::PT_JSON:        return "createJsonOutput";
		case proc::PT_BOOL:        return "createBoolOutput";
		case proc::PT_NUMBER:      return "createNumberOutput";
		case proc::PT_NUMBERLIST:  return "createNumberListOutput";
		case proc::PT_VEC2:        return "createVec2Output";
		case proc::PT_VEC2LIST:    return "createVec2ListOutput";
		case proc::PT_VEC3:        return "createVec3Output";
		case proc::PT_VEC3LIST:    return "createVec3ListOutput";
		case proc::PT_QUAT:        return "createQuatOutput";
		case proc::PT_COLOR:       return "createColorOutput";
		case proc::PT_COLORLIST:   return "createColorListOutput";
		case proc::PT_TEXT:        return "createTextOutput";
		case proc::PT_IMAGE:       return "createImageOutput";
		case proc::PT_AUDIO:       return "createAudioOutput";
		case proc::PT_AUDIONODE:   return "createAudioNodeOutput";
		case proc::PT_FEATURE:     return "createFeatureOutput";
		case proc::PT_BODY:        return "createBodyOutput";
		case proc::PT_BODYLIST:    return "createBodyListOutput";
		default: return "";
	}
}

std::string act::nc::NodeEditor::portTypeToCreateInput(proc::PortType type)
{
	switch (type) {
		case proc::PT_JSON:        return "createJsonInput";
		case proc::PT_BOOL:        return "createBoolInput";
		case proc::PT_NUMBER:      return "createNumberInput";
		case proc::PT_NUMBERLIST:  return "createNumberListInput";
		case proc::PT_VEC2:        return "createVec2Input";
		case proc::PT_VEC2LIST:    return "createVec2ListInput";
		case proc::PT_VEC3:        return "createVec3Input";
		case proc::PT_VEC3LIST:    return "createVec3ListInput";
		case proc::PT_QUAT:        return "createQuatInput";
		case proc::PT_COLOR:       return "createColorInput";
		case proc::PT_COLORLIST:   return "createColorListInput";
		case proc::PT_TEXT:        return "createTextInput";
		case proc::PT_IMAGE:       return "createImageInput";
		case proc::PT_AUDIO:       return "createAudioInput";
		case proc::PT_AUDIONODE:   return "createAudioNodeInput";
		case proc::PT_FEATURE:     return "createFeatureInput";
		case proc::PT_FEATURELIST: return "createFeatureListInput";
		case proc::PT_BODY:        return "createBodyInput";
		case proc::PT_BODYLIST:    return "createBodyListInput";
		default: return "";
	}
}

std::string act::nc::NodeEditor::portTypeToInputCallbackType(proc::PortType type)
{
	switch (type) {
		case proc::PT_JSON:        return "ci::Json";
		case proc::PT_BOOL:        return "bool";
		case proc::PT_NUMBER:      return "number";
		case proc::PT_NUMBERLIST:  return "std::vector<number>";
		case proc::PT_VEC2:        return "vec2";
		case proc::PT_VEC2LIST:    return "std::vector<vec2>";
		case proc::PT_VEC3:        return "vec3";
		case proc::PT_VEC3LIST:    return "std::vector<vec3>";
		case proc::PT_QUAT:        return "quat";
		case proc::PT_COLOR:       return "ci::Color";
		case proc::PT_COLORLIST:   return "std::vector<ci::Color>";
		case proc::PT_TEXT:        return "std::string";
		case proc::PT_IMAGE:       return "image";
		case proc::PT_AUDIO:       return "ci::audio::BufferRef";
		case proc::PT_AUDIONODE:   return "ci::audio::NodeRef";
		case proc::PT_FEATURE:     return "feature";
		case proc::PT_FEATURELIST: return "featureList";
		case proc::PT_BODY:        return "room::BodyRef";
		case proc::PT_BODYLIST:    return "room::BodyRefList";
		default: return "float";
	}
}
void act::nc::NodeEditor::draw(float height)
{
    ImGui::BeginChild("##Editor", ImVec2(0, height), true);

    ImGui::Text("Derive from node \"%s\"", m_selectedProcNodeName.c_str());
    ImGui::Separator();

    ImGui::Spacing();

    // --- Node Name ---
    ImGui::Text("Node Name");
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputTextWithHint("##NodeName", "Enter node name...", m_newNodeNameBuf, IM_ARRAYSIZE(m_newNodeNameBuf))) {
        m_successMessage.clear();
    }

    ImGui::Spacing();

    // --- Contributor ---
    ImGui::Text("Contributor");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##ContributorName", "Name...", m_contributorNameBuf, IM_ARRAYSIZE(m_contributorNameBuf));
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##ContributorContact", "Contact (e.g. email)...", m_contributorContactBuf, IM_ARRAYSIZE(m_contributorContactBuf));

    ImGui::Spacing();

    // --- Group Selection ---
    ImGui::Text("Group");
    ImGui::SetNextItemWidth(-1);

    std::vector<const char*> groupNames;
    for (const auto& pair : m_registryEntry) {
        groupNames.push_back(pair.first.c_str());
    }

    if (!groupNames.empty()) {
        // Sync group index from NodeSelector only when selection changes
        if (m_selectedProcNodeGroup != m_lastSyncedGroup) {
            m_lastSyncedGroup = m_selectedProcNodeGroup;
            if (!m_selectedProcNodeGroup.empty()) {
                for (int i = 0; i < (int)groupNames.size(); ++i) {
                    if (m_selectedProcNodeGroup == groupNames[i]) {
                        m_selectedGroupIndex = i;
                        break;
                    }
                }
            } else if (isBasicTemplate()) {
                m_selectedGroupIndex = 0;
            }
        }

        if (m_selectedGroupIndex >= (int)groupNames.size())
            m_selectedGroupIndex = 0;
        ImGui::Combo("##GroupCombo", &m_selectedGroupIndex, groupNames.data(), (int)groupNames.size());
    } else {
        ImGui::TextDisabled("No groups available");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // --- Basic Template: Parameters & Ports ---
    if (isBasicTemplate()) {
        drawBasicTemplateUI();
    }

    // --- Generate Button ---
    std::string nodeName(m_newNodeNameBuf);
    std::string contributorName(m_contributorNameBuf);
    std::string contributorContact(m_contributorContactBuf);
    bool canGenerate = !nodeName.empty() && !groupNames.empty() && !contributorName.empty() && !contributorContact.empty();

    if (!canGenerate) ImGui::BeginDisabled();
    if (ImGui::Button(ICON_FA_PLUS " Generate Node", ImVec2(200, 40))) {
        std::string selectedGroup = groupNames[m_selectedGroupIndex];
        m_registryEntry[selectedGroup].push_back(nodeName);
        std::sort(m_registryEntry[selectedGroup].begin(), m_registryEntry[selectedGroup].end());
        m_isRegistryModified = true;
        generateNode(nodeName);
        clearAfterGenerate(nodeName);
        if (m_onGenerateCallback) m_onGenerateCallback();
    }
    if (!canGenerate) ImGui::EndDisabled();

    if (!m_successMessage.empty()) {
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%s", m_successMessage.c_str());
    }

    ImGui::EndChild();
}

void act::nc::NodeEditor::drawBasicTemplateUI()
{
    // --- Ports ---
    if (ImGui::CollapsingHeader("Ports", ImGuiTreeNodeFlags_DefaultOpen)) {
        static const proc::PortType inputPortTypes[] = {
            proc::PT_JSON, proc::PT_BOOL, proc::PT_NUMBER, proc::PT_NUMBERLIST,
            proc::PT_VEC2, proc::PT_VEC2LIST, proc::PT_VEC3, proc::PT_VEC3LIST,
            proc::PT_QUAT, proc::PT_COLOR, proc::PT_COLORLIST, proc::PT_TEXT,
            proc::PT_IMAGE, proc::PT_AUDIO, proc::PT_AUDIONODE,
            proc::PT_FEATURE, proc::PT_FEATURELIST,
            proc::PT_BODY, proc::PT_BODYLIST
        };
        static constexpr int inputPortTypeCount = sizeof(inputPortTypes) / sizeof(inputPortTypes[0]);

        static const proc::PortType outputPortTypes[] = {
            proc::PT_JSON, proc::PT_BOOL, proc::PT_NUMBER, proc::PT_NUMBERLIST,
            proc::PT_VEC2, proc::PT_VEC2LIST, proc::PT_VEC3, proc::PT_VEC3LIST,
            proc::PT_QUAT, proc::PT_COLOR, proc::PT_COLORLIST, proc::PT_TEXT,
            proc::PT_IMAGE, proc::PT_AUDIO, proc::PT_AUDIONODE,
            proc::PT_FEATURE,
            proc::PT_BODY, proc::PT_BODYLIST
        };
        static constexpr int outputPortTypeCount = sizeof(outputPortTypes) / sizeof(outputPortTypes[0]);

        static const char* portDirNames[] = { "Input", "Output" };

        for (int i = 0; i < (int)m_ports.size(); ++i) {
            ImGui::PushID(i);
            auto& port = m_ports[i];

            ImGui::SetNextItemWidth(200);
            ImGui::InputTextWithHint("##PortName", "name...", port.name, IM_ARRAYSIZE(port.name));
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);
            int dirIdx = (int)port.direction;
            if (ImGui::Combo("##Dir", &dirIdx, portDirNames, IM_ARRAYSIZE(portDirNames))) {
                port.direction = (PortDirection)dirIdx;
            }
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);

            const proc::PortType* types = (port.direction == PortDirection::Input) ? inputPortTypes : outputPortTypes;
            int typeCount = (port.direction == PortDirection::Input) ? inputPortTypeCount : outputPortTypeCount;

            std::string preview = proc::portTypeToString(port.type);
            if (ImGui::BeginCombo("##Type", preview.c_str())) {
                for (int t = 0; t < typeCount; ++t) {
                    bool selected = (types[t] == port.type);
                    std::string label = proc::portTypeToString(types[t]);
                    if (ImGui::Selectable(label.c_str(), selected)) {
                        port.type = types[t];
                    }
                    if (selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_TIMES "##RemPort")) {
                m_ports.erase(m_ports.begin() + i);
                ImGui::PopID();
                break;
            }
            ImGui::PopID();
        }
        if (ImGui::Button(ICON_FA_PLUS " Add Port")) {
            m_ports.push_back({});
        }
    }

    ImGui::Spacing();

    // --- Parameters ---
    if (ImGui::CollapsingHeader("Parameters (toParams / fromParams)", ImGuiTreeNodeFlags_DefaultOpen)) {
        static const char* paramTypeNames[] = {
            "float", "int", "bool", "std::string", "double",
            "vec2", "ivec2", "vec3", "Color", "quat"
        };

        for (int i = 0; i < (int)m_params.size(); ++i) {
            ImGui::PushID(1000 + i);
            auto& param = m_params[i];

            ImGui::SetNextItemWidth(200);
            ImGui::InputTextWithHint("##ParamName", "name...", param.name, IM_ARRAYSIZE(param.name));
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);
            ImGui::Combo("##ParamType", &param.typeIndex, paramTypeNames, IM_ARRAYSIZE(paramTypeNames));
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);
            ImGui::InputTextWithHint("##DefVal", "default...", param.defaultValue, IM_ARRAYSIZE(param.defaultValue));
            ImGui::SameLine();

            // Preview: m_<name>
            std::string paramName(param.name);
            if (!paramName.empty()) {
                ImGui::SameLine();
                ImGui::TextDisabled("-> m_%s", param.name);
            }

            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_TIMES "##RemParam")) {
                m_params.erase(m_params.begin() + i);
                ImGui::PopID();
                break;
            }
            ImGui::PopID();
        }
        if (ImGui::Button(ICON_FA_PLUS " Add Parameter")) {
            m_params.push_back({});
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
}

void act::nc::NodeEditor::clearAfterGenerate(const std::string& nodeName)
{
    m_newNodeNameBuf[0] = '\0';
    m_ports.clear();
    m_params.clear();
    m_successMessage = "Node \"" + nodeName + "\" generated successfully!";
}

std::vector<act::nc::PortEntry> act::nc::NodeEditor::getSortedPorts(PortDirection dir) const
{
    std::vector<PortEntry> filtered;
    for (const auto& port : m_ports) {
        if (port.direction == dir && port.name[0] != '\0')
            filtered.push_back(port);
    }
    std::sort(filtered.begin(), filtered.end(), [](const PortEntry& a, const PortEntry& b) {
        return std::strcmp(a.name, b.name) < 0;
    });
    return filtered;
}

std::vector<act::nc::ParamEntry> act::nc::NodeEditor::getSortedParams() const
{
    std::vector<ParamEntry> sorted;
    for (const auto& param : m_params) {
        if (param.name[0] != '\0')
            sorted.push_back(param);
    }
    std::sort(sorted.begin(), sorted.end(), [](const ParamEntry& a, const ParamEntry& b) {
        return std::strcmp(a.name, b.name) < 0;
    });
    return sorted;
}

void act::nc::NodeEditor::generateNode(const std::string& nodeName)
{
    std::string className = nodeName + "ProcNode";

    if (isBasicTemplate()) {
        generateHppFile(className);
        generateCppFile(className, nodeName);
    } else {
        generateFromTemplate(className, nodeName);
    }

    m_vcxprojManager.addNode(className);
}

void act::nc::NodeEditor::generateFromTemplate(const std::string& className, const std::string& nodeName)
{
    std::string sourceClassName = m_selectedProcNodeName + "ProcNode";

    // Read source hpp
    {
        std::string srcPath = procIncludeBasePath + sourceClassName + ".hpp";
        std::ifstream in(srcPath);
        if (!in.is_open()) {
            std::cerr << "Failed to open template hpp: " << srcPath << std::endl;
            return;
        }
        std::stringstream buf;
        buf << in.rdbuf();
        in.close();
        std::string content = buf.str();

        // Replace class name
        std::string::size_type pos = 0;
        while ((pos = content.find(sourceClassName, pos)) != std::string::npos) {
            content.replace(pos, sourceClassName.length(), className);
            pos += className.length();
        }

        // Write new hpp
        std::string dstPath = procIncludeBasePath + className + ".hpp";
        std::ofstream out(dstPath, std::ios::out | std::ios::trunc);
        if (!out.is_open()) {
            std::cerr << "Failed to create header file: " << dstPath << std::endl;
            return;
        }

        std::string contributorName(m_contributorNameBuf);
        std::string contributorContact(m_contributorContactBuf);

        // Replace original file header with new one
        auto pragmaPos = content.find("#pragma once");
        if (pragmaPos != std::string::npos) {
            out << buildFileHeader(contributorName, contributorContact) << "\n";
            out << content.substr(pragmaPos);
        } else {
            out << content;
        }
        out.close();
    }

    // Read source cpp
    {
        std::string srcPath = procSourceBasePath + sourceClassName + ".cpp";
        std::ifstream in(srcPath);
        if (!in.is_open()) {
            std::cerr << "Failed to open template cpp: " << srcPath << std::endl;
            return;
        }
        std::stringstream buf;
        buf << in.rdbuf();
        in.close();
        std::string content = buf.str();

        // Replace class name
        std::string::size_type pos = 0;
        while ((pos = content.find(sourceClassName, pos)) != std::string::npos) {
            content.replace(pos, sourceClassName.length(), className);
            pos += className.length();
        }

        // Replace the ProcNodeBase("SourceNodeName") with the new node name
        std::string oldBase = "ProcNodeBase(\"" + m_selectedProcNodeName + "\")";
        std::string newBase = "ProcNodeBase(\"" + nodeName + "\")";
        pos = content.find(oldBase);
        if (pos != std::string::npos) {
            content.replace(pos, oldBase.length(), newBase);
        }

        // Write new cpp
        std::string dstPath = procSourceBasePath + className + ".cpp";
        std::ofstream out(dstPath, std::ios::out | std::ios::trunc);
        if (!out.is_open()) {
            std::cerr << "Failed to create source file: " << dstPath << std::endl;
            return;
        }

        std::string contributorName(m_contributorNameBuf);
        std::string contributorContact(m_contributorContactBuf);

        // Replace original file header with new one
        auto includePos = content.find("#include");
        if (includePos != std::string::npos) {
            out << buildFileHeader(contributorName, contributorContact) << "\n";
            out << content.substr(includePos);
        } else {
            out << content;
        }
        out.close();
    }
}

static std::string getCurrentYear()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    struct tm localTime;
    localtime_s(&localTime, &time);
    return std::to_string(1900 + localTime.tm_year);
}

static std::string buildFileHeader(const std::string& contributorName, const std::string& contributorContact)
{
    std::string year = getCurrentYear();
    std::string header =
        "/*\n"
        "\tInACTually\n"
        "\t> interactive theater for actual acts\n"
        "\t> this file is part of the \"InACTually Engine\", a MediaServer for driving all technology\n"
        "\n"
        "\tCopyright (c) " + year + " InACTually Community\n"
        "\tLicensed under the MIT License.\n"
        "\tSee LICENSE file in the project root for full license information.\n"
        "\n"
        "\tThis file is created and substantially modified: " + year + "\n";

    if (!contributorName.empty()) {
        header += "\n\tcontributors:\n";
        header += "\t" + contributorName;
        if (!contributorContact.empty())
            header += " - " + contributorContact;
        header += "\n";
    }

    header += "*/\n";
    return header;
}

std::string act::nc::NodeEditor::buildPortDeclarations() const
{
    std::string result;
    auto outputs = getSortedPorts(PortDirection::Output);
    for (const auto& port : outputs) {
        std::string name(port.name);
        std::string memberName = "m_" + name + "Port";
        result += "\t\t\t" + portTypeToOutputDecl(port.type) + "\t" + memberName + ";\n";
    }
    return result;
}

std::string act::nc::NodeEditor::buildParamDeclarations() const
{
    std::string result;
    auto sorted = getSortedParams();
    for (const auto& param : sorted) {
        std::string name(param.name);
        std::string type = paramTypeToCppType(param.typeIndex);
        result += "\t\t\t" + type + " m_" + name + ";\n";
    }
    return result;
}

std::string act::nc::NodeEditor::buildPortCreation() const
{
    std::string result;

    auto inputs = getSortedPorts(PortDirection::Input);
    for (const auto& port : inputs) {
        std::string name(port.name);
        std::string createFn = portTypeToCreateInput(port.type);
        std::string cbType = portTypeToInputCallbackType(port.type);
        if (!createFn.empty()) {
            result += "\t" + createFn + "(\"" + name + "\", [&](" + cbType + " val) { });\n";
        } else {
            result += "\t// TODO: create input port for \"" + name + "\"\n";
        }
    }

    auto outputs = getSortedPorts(PortDirection::Output);
    if (!inputs.empty() && !outputs.empty())
        result += "\n";

    for (const auto& port : outputs) {
        std::string name(port.name);
        std::string memberName = "m_" + name + "Port";
        std::string createFn = portTypeToCreateOutput(port.type);
        if (!createFn.empty()) {
            result += "\t" + memberName + " = " + createFn + "(\"" + name + "\");\n";
        } else {
            result += "\t// TODO: create output port for \"" + name + "\"\n";
        }
    }

    return result;
}

std::string act::nc::NodeEditor::buildParamDefaults() const
{
    std::string result;
    auto sorted = getSortedParams();
    for (const auto& param : sorted) {
        std::string name(param.name);
        std::string defVal(param.defaultValue);
        if (defVal.empty()) {
            switch (param.typeIndex) {
                case 0: defVal = "0.0f"; break;
                case 1: defVal = "0"; break;
                case 2: defVal = "false"; break;
                case 3: defVal = "\"\""; break;
                case 4: defVal = "0.0"; break;
                case 5: defVal = "ci::vec2(0)"; break;
                case 6: defVal = "ci::ivec2(0)"; break;
                case 7: defVal = "ci::vec3(0)"; break;
                case 8: defVal = "ci::Color(0, 0, 0)"; break;
                case 9: defVal = "ci::quat()"; break;
            }
        }
        result += "\tm_" + name + " = " + defVal + ";\n";
    }
    return result;
}

std::string act::nc::NodeEditor::buildToParams() const
{
    std::string result;
    auto sorted = getSortedParams();
    for (const auto& param : sorted) {
        std::string name(param.name);
        std::string varName = "m_" + name;
        if (param.typeIndex >= 5) {
            result += "\tjson[\"" + name + "\"] = util::valueToJson(" + varName + ");\n";
        } else {
            result += "\tjson[\"" + name + "\"] = " + varName + ";\n";
        }
    }
    return result;
}

std::string act::nc::NodeEditor::buildFromParams() const
{
    std::string result;
    auto sorted = getSortedParams();
    for (const auto& param : sorted) {
        std::string name(param.name);
        result += "\tutil::setValueFromJson(json, \"" + name + "\", m_" + name + ");\n";
    }
    return result;
}

void act::nc::NodeEditor::generateHppFile(const std::string& className)
{
    std::string filePath = procIncludeBasePath + className + ".hpp";
    std::ofstream out(filePath, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Failed to create header file: " << filePath << std::endl;
        return;
    }

    std::string contributorName(m_contributorNameBuf);
    std::string contributorContact(m_contributorContactBuf);

    out << buildFileHeader(contributorName, contributorContact);
    out << R"(
#pragma once

#include "ProcNodeBase.hpp"

using namespace ci;
using namespace ci::app;


namespace act {
	namespace proc {

		class )" << className << R"( : public ProcNodeBase
		{
		public:
			)" << className << R"(();
			~)" << className << R"(();

			PROCNODECREATE()" << className << R"();

			void setup(act::room::RoomManagers roomMgrs)	override;
			void update()			override;
			void draw()				override;

			ci::Json toParams() override;
			void fromParams(ci::Json json) override;

		private:
)";
    out << buildPortDeclarations();
    out << buildParamDeclarations();

    out << R"(
		}; using )" << className << R"(Ref = std::shared_ptr<)" << className << R"(>;

	}
}
)";
    out.close();
}

void act::nc::NodeEditor::generateCppFile(const std::string& className, const std::string& nodeName)
{
    std::string filePath = procSourceBasePath + className + ".cpp";
    std::ofstream out(filePath, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Failed to create source file: " << filePath << std::endl;
        return;
    }

    std::string contributorName(m_contributorNameBuf);
    std::string contributorContact(m_contributorContactBuf);

    std::string paramDefaults = buildParamDefaults();
    std::string portCreation = buildPortCreation();

    // Add empty line between param defaults and port creation if both exist
    std::string constructorBody;
    if (!paramDefaults.empty()) {
        constructorBody += paramDefaults;
        if (!portCreation.empty())
            constructorBody += "\n";
    }
    constructorBody += portCreation;

    std::string paramUI = buildParamUI();

    out << buildFileHeader(contributorName, contributorContact);
    out << R"(
#include "procpch.hpp"
#include ")" << className << R"(.hpp"

act::proc::)" << className << "::" << className << R"(() : ProcNodeBase(")" << nodeName << R"(") {
	m_drawSize = ci::ivec2(200, 200);
)" << constructorBody << R"(}

act::proc::)" << className << "::~" << className << R"(() {
}

void act::proc::)" << className << R"(::setup(act::room::RoomManagers roomMgrs) {
}

void act::proc::)" << className << R"(::update() {
}

void act::proc::)" << className << R"(::draw() {
	beginNodeDraw();

	ImGui::Text(getName().c_str());

)" << paramUI << R"(
	endNodeDraw();
}

ci::Json act::proc::)" << className << R"(::toParams() {
	ci::Json json = ci::Json::object();
)" << buildToParams() << R"(	return json;
}

void act::proc::)" << className << R"(::fromParams(ci::Json json) {
)" << buildFromParams() << R"(}
)";
    out.close();
}

std::string act::nc::NodeEditor::buildParamUI() const
{
    std::string result;
    auto sorted = getSortedParams();
    if (sorted.empty())
        return result;

    result += "\tbool prvntDrag = false;\n\n";
    for (const auto& param : sorted) {
        std::string name(param.name);
        std::string varName = "m_" + name;
        switch (param.typeIndex) {
            case 0: // float
                result += "\tImGui::SetNextItemWidth(m_drawSize.x);\n";
                result += "\tif (ImGui::DragFloat(\"" + name + "\", &" + varName + ")) {\n";
				result += "\t\tprvntDrag = true;\n";
				result += "\t}\n";
                break;
            case 1: // int
                result += "\tImGui::SetNextItemWidth(m_drawSize.x);\n";
                result += "\tif (ImGui::InputInt(\"" + name + "\", &" + varName + ")) {\n";
				result += "\t\tprvntDrag = true;\n";
				result += "\t}\n";
                break;
            case 2: // bool
                result += "\tif (ImGui::Checkbox(\"" + name + "\", &" + varName + ")) {\n";
				result += "\t\tprvntDrag = true;\n";
				result += "\t}\n";
                break;
            case 3: // std::string
                result += "\t{\n";
                result += "\t\tchar buf[256];\n";
                result += "\t\tstrncpy_s(buf, " + varName + ".c_str(), sizeof(buf) - 1);\n";
                result += "\t\tImGui::SetNextItemWidth(m_drawSize.x);\n";
                result += "\t\tif (ImGui::InputText(\"" + name + "\", buf, sizeof(buf))) {\n";
                result += "\t\t\t" + varName + " = std::string(buf);\n";
                result += "\t\t\tprvntDrag = true;\n";
                result += "\t\t}\n";
                result += "\t}\n";
                break;
            case 4: // double
                result += "\t{\n";
                result += "\t\tfloat tmp_" + name + " = double(" + varName + ");\n";
                result += "\t\tImGui::SetNextItemWidth(m_drawSize.x);\n";
                result += "\t\tif (ImGui::DragFloat(\"" + name + "\", &tmp_" + name + ")) {\n";
                result += "\t\t\t" + varName + " = double(tmp_" + name + ");\n";
                result += "\t\t\tprvntDrag = true;\n";
                result += "\t\t}\n";
                result += "\t}\n";
                break;
            case 5: // vec2
                result += "\tImGui::SetNextItemWidth(m_drawSize);\n";
                result += "\tif (ImGui::DragFloat2(\"" + name + "\", &" + varName + ") {\n";
				result += "\t\tprvntDrag = true;\n";
				result += "\t}\n";
                break;
            case 6: // ivec2
                result += "\tImGui::SetNextItemWidth(m_drawSize);\n";
                result += "\tif (ImGui::InputInt2(\"" + name + "\", &" + varName + ") {\n";
                break;
            case 7: // vec3
                result += "\tImGui::SetNextItemWidth(m_drawSize);\n";
                result += "\tif (ImGui::DragFloat3(\"" + name + "\", &" + varName + ")) {\n";
				result += "\t\tprvntDrag = true;\n";
				result += "\t}\n";
                break;
            case 8: // Color
                result += "\tImGui::SetNextItemWidth(m_drawSize);\n";
                result += "\tif (ImGui::ColorEdit3(\"" + name + "\", &" + varName + ")) {\n";
				result += "\t\tprvntDrag = true;\n";
				result += "\t}\n";
                break;
            case 9: // quat
                result += "\t{\n";
                result += "\t\tci::vec3 euler_" + name + " = glm::eulerAngles(" + varName + ");\n";
                result += "\t\tImGui::SetNextItemWidth(m_drawSize);\n";
                result += "\t\tif (ImGui::DragFloat3(\"" + name + "\", &euler_" + name + ")) {\n";
                result += "\t\t\t" + varName + " = ci::quat(euler_" + name + ");\n";
                result += "\t\t\tprvntDrag = true;\n";
                result += "\t\t}\n";
                result += "\t}\n";
                break;
        }
    }
    result += "\n\tpreventDrag(prvntDrag);\n";
    return result;
}
