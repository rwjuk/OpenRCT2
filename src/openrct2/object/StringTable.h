/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <string>
#include <vector>
#include "../common.h"
#include "../localisation/Language.h"

interface IReadObjectContext;
interface IStream;

enum OBJ_STRING_ID : uint8
{
    OBJ_STRING_ID_UNKNOWN = 255,
    OBJ_STRING_ID_NAME = 0,
    OBJ_STRING_ID_DESCRIPTION,
    OBJ_STRING_ID_SCENARIO_NAME = 0,
    OBJ_STRING_ID_PARK_NAME = 1,
    OBJ_STRING_ID_SCENARIO_DETAILS = 2,
    OBJ_STRING_ID_CAPACITY = 2,
    OBJ_STRING_ID_VEHICLE_NAME = 3,
};

struct StringTableEntry
{
    uint8       Id          = OBJ_STRING_ID_UNKNOWN;
    uint8       LanguageId  = LANGUAGE_UNDEFINED;
    std::string Text;
};

class StringTable
{
private:
    std::vector<StringTableEntry> _strings;

public:
    StringTable() = default;
    StringTable(const StringTable &) = delete;
    StringTable & operator=(const StringTable &) = delete;

    void            Read(IReadObjectContext * context, IStream * stream, uint8 id);
    void            Sort();
    std::string     GetString(uint8 id) const;
    std::string     GetString(uint8 language, uint8 id) const;
    void            SetString(uint8 id, uint8 language, const std::string &text);
};
