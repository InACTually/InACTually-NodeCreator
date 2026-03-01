
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

#include <map>
#include <string>
#include <vector>

namespace act {
	namespace nc {

		class VcxprojFiltersManager {
		public:
			VcxprojFiltersManager(const std::string& filtersPath);

			void sync(const std::map<std::string, std::vector<std::string>>& registryEntry);

			static std::string groupToFilterPath(const std::string& group);

		private:
			std::string m_filtersPath;

			static void ensureFilterExists(std::string& content, const std::string& filterPath);
		};

	}
}
