#pragma once

#include <map>
#include <string>

#include "Palette.h"

class PaletteManager {
private:
    std::map<int, PlanetPaletteConfig> configPalettes;
    std::map<std::string, Palette> customPalettes;

public:
    PaletteManager(const std::string& configFolder, const std::string& customFolder);

    std::map<int, PlanetPaletteConfig> loadConfig(const std::string& folder);
    std::map<std::string, Palette> loadCustom(const std::string& folder);

    Palette getCustom(const std::string& name = "");
    Palette get(int typeId, float temperature = 0.5f);
};

extern PaletteManager paletteManager;