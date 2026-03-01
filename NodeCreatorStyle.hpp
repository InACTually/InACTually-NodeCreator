
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

namespace act {
	namespace nc {

		struct NodeCreatorColors {
			static ImVec4 darkPrimary(float alpha = 1.0f);
			static ImVec4 primary(float alpha = 1.0f);
			static ImVec4 highlight(float alpha = 1.0f);
			static ImVec4 darkError(float alpha = 1.0f);
			static ImVec4 error(float alpha = 1.0f);
			static ImVec4 secondary(float alpha = 1.0f);
			static ImVec4 additional(float alpha = 1.0f);
			static ImVec4 gray(float alpha = 1.0f);
			static ImVec4 background(float alpha = 1.0f);
		};

		class NodeCreatorStyle {
		public:
			static void apply();
		};

	}
}
