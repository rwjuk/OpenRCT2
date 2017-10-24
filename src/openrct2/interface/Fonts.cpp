#pragma region Copyright(c) 2014 - 2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../config/Config.h"
#include "../core/Console.hpp"
#include "../core/String.hpp"
#include "../localisation/LanguagePack.h"
#include "Fonts.h"

#include "../drawing/drawing.h"
#include "../drawing/ttf.h"
#include "../localisation/language.h"

#ifndef NO_TTF
uint8 const HINTING_THRESHOLD_LOW    = 40;
uint8 const HINTING_THRESHOLD_MEDIUM = 60;

TTFFontSetDescriptor TTFFontHiraginoMaru = { {
    { "ヒラギノ丸ゴ ProN W4.ttc", "Hiragino Maru Gothic ProN W4",  8, 1, -1, 10, HINTING_THRESHOLD_MEDIUM, nullptr },
    { "ヒラギノ丸ゴ ProN W4.ttc", "Hiragino Maru Gothic ProN W4", 10, 1,  0, 14, HINTING_THRESHOLD_MEDIUM, nullptr },
    { "ヒラギノ丸ゴ ProN W4.ttc", "Hiragino Maru Gothic ProN W4", 11, 1,  0, 14, HINTING_THRESHOLD_MEDIUM, nullptr },
    { "ヒラギノ丸ゴ ProN W4.ttc", "Hiragino Maru Gothic ProN W4", 12, 1,  0, 15, HINTING_THRESHOLD_MEDIUM, nullptr },
} };

TTFFontSetDescriptor TTFFontMSGothic = { {
    { "msgothic.ttc", "MS PGothic",  9, 1, -1, 10, HINTING_THRESHOLD_MEDIUM, nullptr },
    { "msgothic.ttc", "MS PGothic", 12, 1,  0, 14, HINTING_THRESHOLD_MEDIUM, nullptr },
    { "msgothic.ttc", "MS PGothic", 12, 1,  0, 14, HINTING_THRESHOLD_MEDIUM, nullptr },
    { "msgothic.ttc", "MS PGothic", 13, 1,  0, 15, HINTING_THRESHOLD_MEDIUM, nullptr },
} };

TTFFontSetDescriptor TTFFontMingLiu = { {
    {    "msjh.ttc", "JhengHei",  9, -1, -3,  6, HINTING_THRESHOLD_MEDIUM, nullptr },
    { "mingliu.ttc",  "MingLiU", 11,  1,  1, 14, HINTING_THRESHOLD_MEDIUM, nullptr },
    { "mingliu.ttc",  "MingLiU", 12,  1,  0, 15, HINTING_THRESHOLD_MEDIUM, nullptr },
    { "mingliu.ttc",  "MingLiU", 13,  1,  0, 15, HINTING_THRESHOLD_MEDIUM, nullptr },
} };

TTFFontSetDescriptor TTFFontSimSun = { {
    {   "msyh.ttc",  "YaHei",  9, -1, -3,  6, HINTING_THRESHOLD_MEDIUM, nullptr },
    { "simsun.ttc", "SimSun", 11,  1, -1, 14, HINTING_THRESHOLD_MEDIUM, nullptr },
    { "simsun.ttc", "SimSun", 12,  1, -2, 15, HINTING_THRESHOLD_MEDIUM, nullptr },
    { "simsun.ttc", "SimSun", 13,  1,  0, 16, HINTING_THRESHOLD_MEDIUM, nullptr },
} };

TTFFontSetDescriptor TTFFontGulim = { {
    { "gulim.ttc", "Gulim", 11, 1, 0, 13, HINTING_THRESHOLD_MEDIUM, nullptr },
    { "gulim.ttc", "Gulim", 12, 1, 0, 15, HINTING_THRESHOLD_MEDIUM, nullptr },
    { "gulim.ttc", "Gulim", 12, 1, 0, 15, HINTING_THRESHOLD_MEDIUM, nullptr },
    { "gulim.ttc", "Gulim", 13, 1, 0, 16, HINTING_THRESHOLD_MEDIUM, nullptr },
} };

TTFFontSetDescriptor TTFFontArial = { {
    { "arial.ttf", "Arial",  8, 0, -1,  6, HINTING_THRESHOLD_LOW, nullptr },
    { "arial.ttf", "Arial", 10, 0, -1, 12, HINTING_THRESHOLD_LOW, nullptr },
    { "arial.ttf", "Arial", 11, 0, -1, 12, HINTING_THRESHOLD_LOW, nullptr },
    { "arial.ttf", "Arial", 12, 0, -1, 14, HINTING_THRESHOLD_LOW, nullptr },
} };
#endif // NO_TTF

static void LoadSpriteFont()
{
    ttf_dispose();
    gUseTrueTypeFont   = false;
#ifndef NO_TTF
    gCurrentTTFFontSet = nullptr;
#endif // NO_TTF
}

#ifndef NO_TTF
static bool LoadFont(TTFFontSetDescriptor * font)
{
    gUseTrueTypeFont   = true;
    gCurrentTTFFontSet = font;

    ttf_dispose();
    bool fontInitialised = ttf_initialise();
    return fontInitialised;
}

static bool LoadCustomConfigFont()
{
    static TTFFontSetDescriptor TTFFontCustom = { {
        { gConfigFonts.file_name, gConfigFonts.font_name, gConfigFonts.size_tiny, gConfigFonts.x_offset, gConfigFonts.y_offset,
          gConfigFonts.height_tiny, gConfigFonts.hinting_threshold, nullptr },
        { gConfigFonts.file_name, gConfigFonts.font_name, gConfigFonts.size_small, gConfigFonts.x_offset, gConfigFonts.y_offset,
          gConfigFonts.height_small, gConfigFonts.hinting_threshold, nullptr },
        { gConfigFonts.file_name, gConfigFonts.font_name, gConfigFonts.size_medium, gConfigFonts.x_offset,
          gConfigFonts.y_offset, gConfigFonts.height_medium, gConfigFonts.hinting_threshold, nullptr },
        { gConfigFonts.file_name, gConfigFonts.font_name, gConfigFonts.size_big, gConfigFonts.x_offset, gConfigFonts.y_offset,
          gConfigFonts.height_big, gConfigFonts.hinting_threshold, nullptr },
    } };

    ttf_dispose();
    gUseTrueTypeFont   = true;
    gCurrentTTFFontSet = &TTFFontCustom;

    bool fontInitialised = ttf_initialise();
    return fontInitialised;
}
#endif // NO_TTF

void TryLoadFonts()
{
#ifndef NO_TTF
    TTFFontSetDescriptor * font = LanguagesDescriptors[gCurrentLanguage].font;
    if (font != FONT_OPENRCT2_SPRITE)
    {
        if (!String::IsNullOrEmpty(gConfigFonts.file_name))
        {
            if (LoadCustomConfigFont())
            {
                return;
            }
            Console::Error::WriteLine("Unable to initialise configured TrueType font -- falling back to Language default.");
        }

        if (LoadFont(font))
        {
            return;
        }
        Console::Error::WriteLine("Unable to initialise preferred TrueType font -- falling back to Arial.");

        if (LoadFont(&TTFFontArial))
        {
            return;
        }
        Console::Error::WriteLine("Unable to initialise preferred TrueType font -- Falling back to sprite font.");
    }
#endif // NO_TTF
    LoadSpriteFont();
}
