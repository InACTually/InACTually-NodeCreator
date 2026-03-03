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

#include "VcxprojFiltersManager.hpp"

#include <cctype>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

act::nc::VcxprojFiltersManager::VcxprojFiltersManager(const std::string& filtersPath)
	: m_filtersPath(filtersPath)
{
}

std::string act::nc::VcxprojFiltersManager::groupToFilterPath(const std::string& group)
{
    std::string filtered;
    for (char c : group) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            filtered += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        } else if (std::isspace(static_cast<unsigned char>(c))) {
            if (!filtered.empty() && filtered.back() != ' ')
                filtered += ' ';
        }
    }
    while (!filtered.empty() && filtered.back() == ' ')
        filtered.pop_back();

    if (filtered.empty())
        return "Source Files";

    return "Source Files\\" + filtered;
}

void act::nc::VcxprojFiltersManager::ensureFilterExists(std::string& content, const std::string& filterPath)
{
    std::string filterTag = "<Filter Include=\"" + filterPath + "\">";
    if (content.find(filterTag) != std::string::npos)
        return;

    std::string selfClosingTag = "<Filter Include=\"" + filterPath + "\" />";
    if (content.find(selfClosingTag) != std::string::npos)
        return;

    std::string filterEntry =
        "    <Filter Include=\"" + filterPath + "\" />\n";

    auto filterBlockPos = content.find("<Filter Include=");
    if (filterBlockPos != std::string::npos) {
        auto endOfBlock = content.find("</ItemGroup>", filterBlockPos);
        if (endOfBlock != std::string::npos) {
            content.insert(endOfBlock, filterEntry);
        }
    }
}

void act::nc::VcxprojFiltersManager::sync(const std::map<std::string, std::vector<std::string>>& registryEntry)
{
    std::fstream file(m_filtersPath, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Failed to open vcxproj.filters: " << m_filtersPath << std::endl;
        return;
    }

    std::stringstream buf;
    buf << file.rdbuf();
    file.close();
    std::string content = buf.str();

    // Strip \r for consistent processing, re-add on write
    bool hasCR = (content.find('\r') != std::string::npos);
    if (hasCR) {
        std::string cleaned;
        cleaned.reserve(content.size());
        for (char c : content) {
            if (c != '\r') cleaned += c;
        }
        content = std::move(cleaned);
    }

    // Parse into lines
    std::vector<std::string> lines;
    {
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line))
            lines.push_back(line);
    }

    // Rebuild content, stripping all ProcNode ClCompile/ClInclude entries (single-line and multi-line)
    // but keeping everything else intact
    std::vector<std::string> strippedLines;
    bool skipping = false;

    for (const auto& line : lines) {
        if (!skipping) {
            bool isProcNodeEntry = (line.find("ProcNode") != std::string::npos
                                    && line.find("ProcNodeBase") == std::string::npos)
                                   && (line.find("<ClInclude ") != std::string::npos
                                    || line.find("<ClCompile ") != std::string::npos);

            if (isProcNodeEntry) {
                if (line.find("/>") != std::string::npos) {
                    continue; // single-line, skip
                }
                skipping = true; // multi-line, skip until closing tag
                continue;
            }
            strippedLines.push_back(line);
        } else {
            if (line.find("</ClCompile>") != std::string::npos || line.find("</ClInclude>") != std::string::npos) {
                skipping = false;
            }
        }
    }

    // Collect all needed filter paths and build filter entries
    std::set<std::string> neededFilters;
    for (const auto& [group, nodes] : registryEntry) {
        if (!nodes.empty()) {
            neededFilters.insert(groupToFilterPath(group));
        }
    }

    // Ensure filter definitions exist in the Filter ItemGroup
    std::string rebuilt;
    for (const auto& line : strippedLines) {
        rebuilt += line + "\n";
    }

    for (const auto& filterPath : neededFilters) {
        ensureFilterExists(rebuilt, filterPath);
    }

    // Re-parse into lines after filter insertion
    strippedLines.clear();
    {
        std::istringstream stream(rebuilt);
        std::string line;
        while (std::getline(stream, line))
            strippedLines.push_back(line);
    }

    // Build new ProcNode entries grouped by type
    // ClCompile entries for .cpp files
    std::vector<std::string> newCppLines;
    for (const auto& [group, nodes] : registryEntry) {
        std::string filterPath = groupToFilterPath(group);
        for (const auto& name : nodes) {
            std::string className = name + "ProcNode";
            newCppLines.push_back("    <ClCompile Include=\"..\\src\\processing\\" + className + ".cpp\">");
            newCppLines.push_back("      <Filter>" + filterPath + "</Filter>");
            newCppLines.push_back("    </ClCompile>");
        }
    }

    // ClInclude entries for .hpp files
    std::vector<std::string> newHppLines;
    for (const auto& [group, nodes] : registryEntry) {
        std::string filterPath = groupToFilterPath(group);
        for (const auto& name : nodes) {
            std::string className = name + "ProcNode";
            newHppLines.push_back("    <ClInclude Include=\"..\\include\\processing\\" + className + ".hpp\">");
            newHppLines.push_back("      <Filter>" + filterPath + "</Filter>");
            newHppLines.push_back("    </ClInclude>");
        }
    }

    // Find the ClCompile and ClInclude ItemGroup sections and insert new entries
    // Strategy: find the </ItemGroup> that closes each section and insert before it
    std::vector<std::string> finalLines;
    bool insertedCpp = false;
    bool insertedHpp = false;
    bool inCppSection = false;
    bool inHppSection = false;

    for (size_t i = 0; i < strippedLines.size(); ++i) {
        const auto& line = strippedLines[i];

        // Detect which ItemGroup section we're in
        if (line.find("<ClCompile ") != std::string::npos)
            inCppSection = true;
        if (line.find("<ClInclude ") != std::string::npos)
            inHppSection = true;

        // Insert new entries before the </ItemGroup> that closes the respective section
        if (line.find("</ItemGroup>") != std::string::npos) {
            if (inCppSection && !insertedCpp) {
                for (const auto& nl : newCppLines)
                    finalLines.push_back(nl);
                insertedCpp = true;
                inCppSection = false;
            }
            if (inHppSection && !insertedHpp) {
                for (const auto& nl : newHppLines)
                    finalLines.push_back(nl);
                insertedHpp = true;
                inHppSection = false;
            }
        }

        finalLines.push_back(line);
    }

    // Write output
    std::string lineEnding = hasCR ? "\r\n" : "\n";

    std::ofstream out(m_filtersPath, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Failed to write vcxproj.filters: " << m_filtersPath << std::endl;
        return;
    }
    for (size_t i = 0; i < finalLines.size(); ++i) {
        out << finalLines[i];
        if (i + 1 < finalLines.size())
            out << lineEnding;
    }
    out.close();
}
