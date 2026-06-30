#include "LoadFont.h"
#include <yaml-cpp/yaml.h>
#include "doukutsu/window.h"

std::vector<FONT_DATA> gFontData;

std::vector<FONT_DATA> LoadFontDataFromYAML(const char* yamlPath)
{
    std::vector<FONT_DATA> font_data;

    try
    {
        // Parse the YAML file
        YAML::Node config = YAML::LoadFile(yamlPath);

        // Iterate over the YAML nodes and extract font data
        for (const auto& node : config["fonts"])
        {
            FONT_DATA f_data;
            f_data.x_scaling = node["x_scaling"].as<int>();
            f_data.y_scaling = node["y_scaling"].as<int>();
            strncpy(f_data.name, node["name"].as<std::string>().c_str(), 0x20);
            strncpy(f_data.filename, node["filename"].as<std::string>().c_str(), 0x20);
            font_data.push_back(f_data);
        }
    }
    catch (const YAML::Exception& e)
    {
        
    }

    return font_data;
}

FONT_DATA GetFirstFontData(const std::vector<FONT_DATA>& font_data)
{
    FONT_DATA empty_font_data;
    // Init font first
    empty_font_data.name[0] = '\0';
    empty_font_data.filename[0] = '\0';
    empty_font_data.x_scaling = 0;
    empty_font_data.y_scaling = 0;

    if (!font_data.empty()) {
        return font_data.front();
    }
    else {
        // Return the initialized empty_font_data
        return empty_font_data;
    }
}