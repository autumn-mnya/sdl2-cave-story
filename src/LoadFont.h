#pragma once

#include <Windows.h>
#include <vector>

typedef struct FONT_DATA
{
    char name[0x20];
    char filename[0x20];
    int x_scaling;
    int y_scaling;
} FONT_DATA;

extern std::vector<FONT_DATA> gFontData;

std::vector<FONT_DATA> LoadFontDataFromYAML(const char* yamlPath);
FONT_DATA GetFirstFontData(const std::vector<FONT_DATA>& font_data);