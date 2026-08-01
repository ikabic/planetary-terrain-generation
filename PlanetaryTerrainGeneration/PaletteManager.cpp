#include <vector>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include "PaletteManager.h"
#include "SurfaceGenerator.h"

PaletteManager paletteManager("assets/palettes");

PaletteManager::PaletteManager(const std::string& folder) : customPalettes(loadCustom(folder)) { }

using json = nlohmann::json;
namespace fs = std::filesystem;

std::map<std::string, Palette> PaletteManager::loadCustom(const std::string& folder) {
    std::map<std::string, Palette> palettes;

    for (const auto& file : fs::directory_iterator(folder)) {
        if (file.path().extension() != ".json") continue;

        std::ifstream input(file.path());

        json data;
        input >> data;

        Palette palette;

        for (auto& colour : data["colours"])
            palette.colours.emplace_back(colour[0], colour[1], colour[2]);
       
        if (data.contains("upperBounds"))
            palette.upperBounds = data["upperBounds"].get<std::vector<float>>();

        palettes[file.path().stem().string()] = palette;
    }

    return palettes;
}

Palette PaletteManager::get(const std::string& name) {
    if (!name.empty())
        return customPalettes.at(name);

    return generatePalette(params.colorNum, params.paletteType);
};