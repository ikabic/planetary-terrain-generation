#pragma once

#include <map>
#include <string>

#include "Palette.h"
#include "PaletteGenerator.h"

class PaletteManager {
private:
    std::map<std::string, Palette> customPalettes;

public:
    PaletteManager(const std::string& folder);

    std::map<std::string, Palette> loadCustom(const std::string& folder);

    Palette get(const std::string& name = "");
};

extern PaletteManager paletteManager;