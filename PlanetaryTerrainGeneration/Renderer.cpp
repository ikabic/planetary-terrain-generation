#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"
#include "Planet.h"
#include "SurfaceGenerator.h"
#include "PaletteManager.h"
#include "Randomiser.h"

static PaletteManager paletteManager("assets/palettes");

void initImGUI(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();
}

void initPlanet() {
    planet = Planet(128, 24241);
    planet.build();
}

void drawPlanet() {
    float rotationAngle = glfwGetTime() * 0.2f/*rotationSpeed*/;

    mainShader.use();
    mainShader.setMat4("model", glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f)));
    mainShader.setMat4("view", camera.getViewMatrix());
    mainShader.setMat4("projection", camera.getProjectionMatrix());

    glm::vec3 lightDir = glm::normalize(glm::vec3(-0.5f, 0.7f, 0.8f));
    mainShader.setVec3("lightDir", lightDir);
    mainShader.setBool("useLighting", false);

    mainShader.setVec3Array("colours", planet.getPalette().colours);
    mainShader.setFloatArray("upperBounds", planet.getPalette().upperBounds);
    mainShader.setInt("paletteSize", static_cast<int>(planet.getPalette().colours.size()));

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
	planet.draw();
}

void drawDebugUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Noise Parameters");

    bool changed = false;

    auto releaseInt = [&](const char* label, int* v, int mn, int mx) {
        ImGui::SliderInt(label, v, mn, mx);
        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
        };
    auto releaseFloat = [&](const char* label, float* v, float mn, float mx) {
        ImGui::SliderFloat(label, v, mn, mx);
        if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
        };
	auto releaseEnum = [&](const char* label, PaletteType* value, int min, int max) {
		int temp = static_cast<int>(*value);
		ImGui::SliderInt(label, &temp, min, max);
		if (ImGui::IsItemDeactivatedAfterEdit()) changed = true;
		*value = static_cast<PaletteType>(temp);
		};


    releaseInt(" Octaves", &params.octaves, 1, 10);
    releaseInt(" Color Num", &params.colorNum, 1, 15);

    releaseInt("Seed", &params.seed, 9, 4000000000);

    releaseInt("Type", &params.type, 0, 8);

    releaseFloat("Base Frequency", &params.baseFrequency, 0.001f, 10.0f);

	releaseFloat("Crater Frequency", &params.craterFrequency, 0.001f, 10.0f);
	releaseFloat("Crater Depth Multiplier", &params.craterDepthMultiplier, 0.0f, 5.0f);
	releaseFloat("Crater Threshold", &params.craterThreshold, 0.0f, 1.0f);

	releaseInt("Use Warp", (int*)&params.useWarp, 0, 1);
    releaseFloat("Warp Strength", &params.warpStrength, 0.0f, 1.0f);
    releaseEnum("Palette Type", &params.paletteType, 1, 7);

	releaseFloat("Dome Frequency", &params.domeFrequency, 0.001f, 10.0f);
	releaseFloat("Dome Threshold", &params.domeThreshold, 0.0f, 1.0f);

	releaseFloat("Hue Min", &params.hueMin, 0.0f, 360.0f);
	releaseFloat("Hue Max", &params.hueMax, 0.0f, 360.0f);
	releaseFloat("Saturation Min", &params.saturationMin, 0.0f, 1.0f);
	releaseFloat("Saturation Max", &params.saturationMax, 0.0f, 1.0f);
	releaseFloat("Value Min", &params.valueMin, 0.0f, 1.0f);
	releaseFloat("Value Max", &params.valueMax, 0.0f, 1.0f);

	releaseFloat("Height Multiplier", &params.height, 0.01f, 10.0f);

    if (changed) {
        randomiser.setSeed(params.seed);
        planet.build();
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}