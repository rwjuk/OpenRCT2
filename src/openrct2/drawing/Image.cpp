/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <list>
#include "../core/Console.hpp"
#include "../core/Guard.hpp"
#include "../OpenRCT2.h"

#include "Drawing.h"

constexpr uint32 BASE_IMAGE_ID = 29294;
constexpr uint32 MAX_IMAGES = 262144;
constexpr uint32 INVALID_IMAGE_ID = UINT32_MAX;

struct ImageList
{
    uint32 BaseId;
    uint32 Count;
};

static bool                 _initialised = false;
static std::list<ImageList> _freeLists;
static uint32               _allocatedImageCount;

#ifdef DEBUG
static std::list<ImageList> _allocatedLists;

// MSVC's compiler doesn't support the [[maybe_unused]] attribute for unused static functions. Until this has been resolved, we
// need to explicitly tell the compiler to temporarily disable the warning.
// See discussion at https://github.com/OpenRCT2/OpenRCT2/pull/7617
#pragma warning(push)
#pragma warning(disable : 4505) // unreferenced local function has been removed

[[maybe_unused]] static bool AllocatedListContains(uint32 baseImageId, uint32 count)
{
    bool contains = std::any_of(
        _allocatedLists.begin(),
        _allocatedLists.end(),
        [baseImageId, count](const ImageList &imageList) -> bool
        {
            return imageList.BaseId == baseImageId && imageList.Count == count;
        });
    return contains;
}

#pragma warning(pop)

static bool AllocatedListRemove(uint32 baseImageId, uint32 count)
{
    auto foundItem = std::find_if(
        _allocatedLists.begin(),
        _allocatedLists.end(),
        [baseImageId, count](const ImageList &imageList) -> bool
        {
            return imageList.BaseId == baseImageId && imageList.Count == count;
        });
    if (foundItem != _allocatedLists.end())
    {
        _allocatedLists.erase(foundItem);
        return true;
    }
    return false;
}
#endif

static uint32 GetNumFreeImagesRemaining()
{
    return MAX_IMAGES - _allocatedImageCount;
}

static void InitialiseImageList()
{
    Guard::Assert(!_initialised, GUARD_LINE);

    _freeLists.clear();
    _freeLists.push_back({ BASE_IMAGE_ID, MAX_IMAGES });
#ifdef DEBUG
    _allocatedLists.clear();
#endif
    _allocatedImageCount = 0;
    _initialised = true;
}

/**
 * Merges all the free lists into one, a process of defragmentation.
 */
static void MergeFreeLists()
{
    _freeLists.sort(
        [](const ImageList &a, const ImageList &b) -> bool
        {
            return a.BaseId < b.BaseId;
        });
    for (auto it = _freeLists.begin(); it != _freeLists.end(); it++)
    {
        bool mergeHappened;
        do
        {
            mergeHappened = false;
            auto nextIt = std::next(it);
            if (nextIt != _freeLists.end())
            {
                if (it->BaseId + it->Count == nextIt->BaseId)
                {
                    // Merge next list into this list
                    it->Count += nextIt->Count;
                    _freeLists.erase(nextIt);
                    mergeHappened = true;
                }
            }
        } while (mergeHappened);
    }
}

static uint32 TryAllocateImageList(uint32 count)
{
    for (auto it = _freeLists.begin(); it != _freeLists.end(); it++)
    {
        ImageList imageList = *it;
        if (imageList.Count >= count)
        {
            _freeLists.erase(it);
            if (imageList.Count > count)
            {
                ImageList remainder = { imageList.BaseId + count,
                                        imageList.Count - count };
                _freeLists.push_back(remainder);
            }

#ifdef DEBUG
            _allocatedLists.push_back({ imageList.BaseId, count });
#endif
            _allocatedImageCount += count;
            return imageList.BaseId;
        }
    }
    return INVALID_IMAGE_ID;
}

static uint32 AllocateImageList(uint32 count)
{
    Guard::Assert(count != 0, GUARD_LINE);

    if (!_initialised)
    {
        InitialiseImageList();
    }

    uint32 baseImageId = INVALID_IMAGE_ID;
    uint32 freeImagesRemaining = GetNumFreeImagesRemaining();
    if (freeImagesRemaining >= count)
    {
        baseImageId = TryAllocateImageList(count);
        if (baseImageId == INVALID_IMAGE_ID)
        {
            // Defragment and try again
            MergeFreeLists();
            baseImageId = TryAllocateImageList(count);
        }
    }
    return baseImageId;
}

static void FreeImageList(uint32 baseImageId, uint32 count)
{
    Guard::Assert(_initialised, GUARD_LINE);
    Guard::Assert(baseImageId >= BASE_IMAGE_ID, GUARD_LINE);

#ifdef DEBUG
    bool contains = AllocatedListRemove(baseImageId, count);
    Guard::Assert(contains, GUARD_LINE);
#endif
    _allocatedImageCount -= count;

    for (auto it = _freeLists.begin(); it != _freeLists.end(); it++)
    {
        if (it->BaseId + it->Count == baseImageId)
        {
            it->Count += count;
            return;
        }
        else if (baseImageId + count == it->BaseId)
        {
            it->BaseId = baseImageId;
            it->Count += count;
            return;
        }
    }

    _freeLists.push_back({ baseImageId, count });
}

uint32 gfx_object_allocate_images(const rct_g1_element * images, uint32 count)
{
    if (count == 0 || gOpenRCT2NoGraphics)
    {
        return INVALID_IMAGE_ID;
    }

    uint32 baseImageId = AllocateImageList(count);
    if (baseImageId == INVALID_IMAGE_ID)
    {
        log_error("Reached maximum image limit.");
        return INVALID_IMAGE_ID;
    }

    uint32 imageId = baseImageId;
    for (uint32 i = 0; i < count; i++)
    {
        gfx_set_g1_element(imageId, &images[i]);
        drawing_engine_invalidate_image(imageId);
        imageId++;
    }

    return baseImageId;
}

void gfx_object_free_images(uint32 baseImageId, uint32 count)
{
    if (baseImageId != 0 && baseImageId != INVALID_IMAGE_ID)
    {
        // Zero the G1 elements so we don't have invalid pointers
        // and data lying about
        for (uint32 i = 0; i < count; i++)
        {
            uint32 imageId = baseImageId + i;
            rct_g1_element g1 = {};
            gfx_set_g1_element(imageId, &g1);
            drawing_engine_invalidate_image(imageId);
        }

        FreeImageList(baseImageId, count);
    }
}

void gfx_object_check_all_images_freed()
{
    if (_allocatedImageCount != 0)
    {
#ifdef DEBUG
        Guard::Assert(_allocatedImageCount == 0, "%u images were not freed", _allocatedImageCount);
#else
        Console::Error::WriteLine("%u images were not freed", _allocatedImageCount);
#endif
    }
}

