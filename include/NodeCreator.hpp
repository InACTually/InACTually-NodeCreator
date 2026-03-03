
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

#include <memory>
#include <string>
#include <vector>
#include <map>

#include "NodeCreatorStyle.hpp"
#include "NodeSelector.hpp"
#include "NodeEditor.hpp"

namespace act {
	namespace nc {
		class NodeCreator;
		using NodeCreatorRef = std::shared_ptr<NodeCreator>;

		enum ReadingState {
			RS_READING_BEGIN = 0,
			RS_READING_INCLUDES,
			RS_READING_MIDDLE,
			RS_READING_REGISTRY,
			RS_READING_END
		};

		class NodeCreator {
		public:
			NodeCreator() { init(); }
			~NodeCreator() {}

			static NodeCreatorRef create() { return std::make_shared<NodeCreator>(); }
		
			void init();

			void draw();

		private:
			std::vector<std::string> m_beginLines;
			std::vector<std::string> m_middlegLines;
			std::vector<std::string> m_endLines;

			std::vector<std::string> m_includes;
			std::map<std::string, std::vector<std::string>> m_registryEntry;

			int m_selectedProcNode = 0;
			std::string m_selectedProcNodeName = "Template for BasicProcNode";
			std::string m_selectedProcNodeGroup = "";

			bool m_isRegistryModified = false;

			std::unique_ptr<NodeSelector> m_nodeSelector;
			std::unique_ptr<NodeEditor> m_nodeEditor;

			void loadProcNodeRegistry();
			void saveProcNodeRegistry();
		};

	}
}