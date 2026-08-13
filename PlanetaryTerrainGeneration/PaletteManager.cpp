#include <vector>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include "PaletteManager.h"
#include "PaletteGenerator.h"
#include "SurfaceGenerator.h"

PaletteManager paletteManager("assets/palettes", "assets/custom_palettes");

PaletteManager::PaletteManager(const std::string& configFolder, const std::string& customFolder) : configPalettes(loadConfig(configFolder)), customPalettes(loadCustom(customFolder)) { }

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

std::map<int, PlanetPaletteConfig> PaletteManager::loadConfig(const std::string& folder) {
    std::map<int, PlanetPaletteConfig> configs;

    for (const auto& file : fs::directory_iterator(folder)) {
        if (file.path().extension() != ".json") continue;

        std::ifstream input(file.path());

        json data;
        input >> data;

        PlanetPaletteConfig planetConfig;
        planetConfig.id = data["id"].get<int>();

        for (const auto& p : data["palettes"]) {
            PaletteConfig palette;
            palette.type = p["type"].get<PaletteType>();
            palette.weight = p["weight"].get<float>();
            palette.minColours = p["numColours"][0].get<int>();
            palette.maxColours = p["numColours"][1].get<int>();

            if (p.contains("biomeStops")) {
                for (const auto& stop : p["biomeStops"]) {
                    BiomeStop biomeStop;
                    biomeStop.upperBound = stop["upperBound"].get<float>();
                    biomeStop.h = glm::vec2(stop["h"][0].get<float>(), stop["h"][1].get<float>());
                    biomeStop.s = glm::vec2(stop["s"][0].get<float>(), stop["s"][1].get<float>());
                    biomeStop.v = glm::vec2(stop["v"][0].get<float>(), stop["v"][1].get<float>());
                    palette.biomeStops.push_back(biomeStop);
                }
            }
            else if (p.contains("hsvStops")) {
                for (const auto& stop : p["hsvStops"]) {
                    HSVStop baseHSV;
                    baseHSV.t = stop["t"].get<float>();
                    baseHSV.h = glm::vec2(stop["h"][0].get<float>(), stop["h"][1].get<float>());
                    baseHSV.s = glm::vec2(stop["s"][0].get<float>(), stop["s"][1].get<float>());
                    baseHSV.v = glm::vec2(stop["v"][0].get<float>(), stop["v"][1].get<float>());
                    palette.baseHSV.push_back(baseHSV);
                }
            }

            std::sort(palette.baseHSV.begin(), palette.baseHSV.end(), [](const HSVStop& a, const HSVStop& b) { return a.t < b.t; });

            planetConfig.palettes.push_back(palette);
        }

        configs[planetConfig.id] = planetConfig;
    }

    return configs;
}

Palette PaletteManager::getCustom(const std::string& name) {
    if (!name.empty())
        return customPalettes.at(name);

    return Palette();
};

Palette PaletteManager::get(int typeId, float temperature) {
    auto it = configPalettes.find(typeId);
    if (it != configPalettes.end())
        return generatePalette(it->second, temperature);

    return generatePalette(PlanetPaletteConfig(), temperature);
};