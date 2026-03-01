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

#pragma once

#include "imgui.h"
#include "VcxprojManager.hpp"
#include "VcxprojFiltersManager.hpp"

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "./../../include/processing/PortType.hpp"

namespace act {
	namespace nc {

		enum class PortDirection { Input, Output };

		struct PortEntry {
			char name[64] = {};
			proc::PortType type = proc::PT_NUMBER;
			PortDirection direction = PortDirection::Output;
		};

		struct ParamEntry {
			char name[64] = {};  // param name → variable: m_<name>, json key: <name>
			int typeIndex = 0;   // 0=float, 1=int, 2=bool, 3=std::string, 4=double, 5=vec2, 6=ivec2, 7=vec3, 8=Color, 9=quat
			char defaultValue[64] = {};
		};

		class NodeEditor {
		public:
			NodeEditor(std::map<std::string, std::vector<std::string>>& registryEntry,
					   bool& isRegistryModified,
					   int& selectedProcNode,
					   std::string& selectedProcNodeName,
					   std::string& selectedProcNodeGroup);

			void draw(float height);

			void setOnGenerateCallback(std::function<void()> callback);

			VcxprojManager& vcxproj() { return m_vcxprojManager; }
			VcxprojFiltersManager& vcxprojFilters() { return m_vcxprojFiltersManager; }

		private:
			std::map<std::string, std::vector<std::string>>& m_registryEntry;
			bool& m_isRegistryModified;
			int& m_selectedProcNode;
			std::string& m_selectedProcNodeName;
			std::string& m_selectedProcNodeGroup;

			char m_newNodeNameBuf[128] = {};
			char m_contributorNameBuf[128] = {};
			char m_contributorContactBuf[128] = {};
			int m_selectedGroupIndex = 0;
			std::string m_lastSyncedGroup;

			std::vector<PortEntry> m_ports;
			std::vector<ParamEntry> m_params;

			std::string m_successMessage;

			std::function<void()> m_onGenerateCallback;

			VcxprojManager m_vcxprojManager;
			VcxprojFiltersManager m_vcxprojFiltersManager;

			bool isBasicTemplate() const { return m_selectedProcNode == 0; }

			void generateNode(const std::string& nodeName, const std::string& group);
			void generateHppFile(const std::string& className, const std::string& nodeName);
			void generateCppFile(const std::string& className, const std::string& nodeName);
			void generateFromTemplate(const std::string& className, const std::string& nodeName);

			void drawBasicTemplateUI();
			void clearAfterGenerate(const std::string& nodeName);

			std::vector<PortEntry> getSortedPorts(PortDirection dir) const;
			std::vector<ParamEntry> getSortedParams() const;

			static const char* paramTypeToCppType(int typeIndex);
			std::string buildPortDeclarations() const;
			std::string buildParamDeclarations() const;
			std::string buildPortCreation() const;
			std::string buildParamDefaults() const;
			std::string buildToParams() const;
			std::string buildFromParams() const;
			std::string buildParamUI() const;

			static std::string portTypeToOutputDecl(proc::PortType type);
			static std::string portTypeToCreateOutput(proc::PortType type);
			static std::string portTypeToCreateInput(proc::PortType type);
			static std::string portTypeToInputCallbackType(proc::PortType type);
		};

	}
}
