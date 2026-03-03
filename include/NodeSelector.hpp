
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

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace act {
	namespace nc {

		class NodeSelector {
		public:
			NodeSelector(std::map<std::string, std::vector<std::string>>& registryEntry,
						 bool& isRegistryModified,
						 int& selectedProcNode,
						 std::string& selectedProcNodeName,
						 std::string& selectedProcNodeGroup);

			void draw(float width, float height);

			void setOnSaveCallback(std::function<void()> callback);
			void setOnRemoveNodeCallback(std::function<void(const std::string&)> callback);

		private:
			std::map<std::string, std::vector<std::string>>& m_registryEntry;
			bool& m_isRegistryModified;
			int& m_selectedProcNode;
			std::string& m_selectedProcNodeName;
			std::string& m_selectedProcNodeGroup;

			char m_newGroupNameBuf[128] = {};

			std::string m_dragSourceGroup;
			std::string m_dragNodeName;

			std::string m_pendingDeleteGroup;
			std::string m_pendingRemoveNodeGroup;
			std::string m_pendingRemoveNodeName;
			std::string m_pendingMoveTargetGroup;

			std::function<void()> m_onSaveCallback;
			std::function<void(const std::string&)> m_onRemoveNodeCallback;

			void handlePendingDeletions();

			void handleDragDropSource(const std::string& groupName, const std::string& nodeName);
			void handleDragDropTarget(const std::string& targetGroup);

			void drawGroupContextMenu(const std::string& groupName, bool isEmpty);
			void drawNodeContextMenu(const std::string& groupName, const std::string& nodeName);
		};

	}
}
