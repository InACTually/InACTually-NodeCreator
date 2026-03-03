
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

#include "VcxprojManager.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

act::nc::VcxprojManager::VcxprojManager(const std::string& vcxprojPath)
	: m_vcxprojPath(vcxprojPath)
{
}

void act::nc::VcxprojManager::insertAlphabetically(std::string& content, const std::string& blockPattern, const std::string& newEntry)
{
    // Find all lines matching the block pattern and determine the correct alphabetical position
    size_t searchPos = 0;
    size_t insertPos = std::string::npos;

    while (true) {
        auto pos = content.find(blockPattern, searchPos);
        if (pos == std::string::npos)
            break;

        // Find the full line for comparison
        auto lineStart = content.rfind('\n', pos);
        lineStart = (lineStart == std::string::npos) ? 0 : lineStart + 1;
        auto lineEnd = content.find('\n', pos);

        std::string existingLine = content.substr(lineStart, lineEnd - lineStart);

        if (newEntry < existingLine) {
            // Insert before this line
            content.insert(lineStart, newEntry + "\n");
            return;
        }

        insertPos = (lineEnd == std::string::npos) ? content.size() : lineEnd + 1;
        searchPos = (lineEnd == std::string::npos) ? content.size() : lineEnd + 1;
    }

    // Insert after the last matching line
    if (insertPos != std::string::npos) {
        content.insert(insertPos, newEntry + "\n");
    }
}

void act::nc::VcxprojManager::addNode(const std::string& className)
{
    std::fstream file(m_vcxprojPath, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Failed to open vcxproj: " << m_vcxprojPath << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::string content = buffer.str();

    // Skip if already present
    if (content.find(className + ".hpp") != std::string::npos)
        return;

    std::string hppEntry = "    <ClInclude Include=\"..\\include\\processing\\" + className + ".hpp\" />";
    std::string cppEntry = "    <ClCompile Include=\"..\\src\\processing\\" + className + ".cpp\" />";

    // Insert hpp alphabetically among processing ClInclude entries
    insertAlphabetically(content, "<ClInclude Include=\"..\\include\\processing\\", hppEntry);

    // Insert cpp alphabetically among processing ClCompile entries
    insertAlphabetically(content, "<ClCompile Include=\"..\\src\\processing\\", cppEntry);

    std::ofstream out(m_vcxprojPath, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Failed to write vcxproj: " << m_vcxprojPath << std::endl;
        return;
    }
    out << content;
    out.close();
}

void act::nc::VcxprojManager::removeNode(const std::string& nodeName)
{
    std::string className = nodeName + "ProcNode";

    std::fstream file(m_vcxprojPath, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Failed to open vcxproj: " << m_vcxprojPath << std::endl;
        return;
    }

    std::string content;
    std::string line;
    std::string hppPattern = className + ".hpp";
    std::string cppPattern = className + ".cpp";

    while (std::getline(file, line)) {
        if (line.find(hppPattern) != std::string::npos && line.find("<ClInclude") != std::string::npos)
            continue;
        if (line.find(cppPattern) != std::string::npos && line.find("<ClCompile") != std::string::npos)
            continue;
        content += line + "\n";
    }
    file.close();

    std::ofstream out(m_vcxprojPath, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Failed to write vcxproj: " << m_vcxprojPath << std::endl;
        return;
    }
    out << content;
    out.close();
}
