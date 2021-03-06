/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

interface ITitleSequencePlayer
{
    virtual ~ITitleSequencePlayer() = default;

    virtual sint32 GetCurrentPosition() const abstract;

    virtual bool Begin(size_t titleSequenceId) abstract;
    virtual void Reset() abstract;
    virtual bool Update() abstract;
    virtual void Seek(sint32 position) abstract;
    virtual void Eject() abstract;
};
