
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

#include <string>

namespace act {
	namespace nc {

		class VcxprojManager {
		public:
			VcxprojManager(const std::string& vcxprojPath);

			void addNode(const std::string& className);
			void removeNode(const std::string& nodeName);

		private:
			std::string m_vcxprojPath;

			void insertAlphabetically(std::string& content, const std::string& blockPattern, const std::string& newEntry);
		};

	}
}
