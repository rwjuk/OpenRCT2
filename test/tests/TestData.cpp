/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/core/Path.hpp>
#include "TestData.h"

namespace TestData
{
    std::string GetBasePath()
    {
        return "testdata";
    }

    std::string GetParkPath(std::string name)
    {
        std::string path = Path::Combine(GetBasePath(), "parks", name);
        return path;
    }
} // namespace TestData
