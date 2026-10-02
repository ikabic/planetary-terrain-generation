#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"
#include "Planet.h"
#include "SurfaceGenerator.h"
#include "PaletteManager.h"
#include "Randomiser.h"
#include "Util.h"

std::unique_ptr<Planet> satelliteMesh;
GLuint instanceVBO = 0;

void initPlanet() {
    if (instanceVBO == 0) {
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, 700 * sizeof(ParticleInstance), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    randomiser.setSeed(params.seed);
    planet.build();

    satelliteMesh = std::make_unique<Planet>(4, 24240);
    satelliteMesh->build();
}

void drawPlanet() {
    float axialTilt = glm::radians(planet.getAxialTilt());
    float rotationAngle = glfwGetTime() * planet.getRotationSpeed();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, axialTilt, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(planet.getSize()));

    mainShader.use();

    mainShader.setMat4("model", model);
    mainShader.setMat4("view", camera.getViewMatrix());
    mainShader.setMat4("projection", camera.getProjectionMatrix());

    glm::vec3 lightDir = glm::normalize(glm::vec3(-0.5f, 0.7f, 0.8f));
    mainShader.setVec3("lightDir", lightDir);
    mainShader.setBool("useLighting", params.applyLighting);

    mainShader.setVec3Array("colours", planet.getPalette().colours);
    mainShader.setFloatArray("upperBounds", planet.getPalette().upperBounds);
    mainShader.setInt("paletteSize", static_cast<int>(planet.getPalette().colours.size()));

	planet.draw();

    drawSatellites();

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        params.seed = static_cast<int>(glfwGetTime() * 100000.0f);
        randomiser.setSeed(params.seed);
        planet.build();
    }
}

void drawSatellites() {
    float axialTilt = glm::radians(planet.getAxialTilt());

    for (const auto& satellite : planet.getSatellites()) {
        float currentAngle = satellite.phase + glfwGetTime() * satellite.orbitSpeed;

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::rotate(model, satellite.inclination, glm::vec3(0.0f, 0.0f, 1.0f)); // tilt orbital plane
		model = glm::rotate(model, currentAngle, glm::vec3(0.0f, 1.0f, 0.0f)); // move along orbit
        model = glm::translate(model, glm::vec3(satellite.orbitRadius, 0.0f, 0.0f)); // push out satellite from planet center to orbital radius
        model = glm::scale(model, glm::vec3(satellite.size));

        mainShader.setMat4("model", model);

        mainShader.setBool("useLighting", false);

        mainShader.setVec3Array("colours", std::vector<glm::vec3> { satellite.colour });
        mainShader.setFloatArray("upperBounds", std::vector<float> { 1.0f });
        mainShader.setInt("paletteSize", 1);

        satelliteMesh->draw();
    }

    if (!planet.getHasRings()) return;

    const auto& ringParticles = planet.getRingParticles();
    std::vector<ParticleInstance> instanceData;
    instanceData.reserve(ringParticles.size());

    for (const auto& particle : ringParticles) {
        float currentAngle = particle.phase + glfwGetTime() * particle.orbitSpeed;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, axialTilt, glm::vec3(0.0f, 0.0f, 1.0f));

        model = glm::rotate(model, particle.inclination, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, currentAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(particle.orbitRadius, particle.verticalOffset, 0.0f));
        model = glm::scale(model, glm::vec3(particle.size));

        instanceData.push_back({ model, particle.colour });
    }

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(ParticleInstance), instanceData.data(), GL_DYNAMIC_DRAW);

    mainShader.setBool("useInstancing", true);
    mainShader.setBool("useLighting", false);

    satelliteMesh->drawInstanced(static_cast<GLsizei>(instanceData.size()));

    mainShader.setBool("useInstancing", false);
}

static GLuint emptyVAO = 0;

void drawBackground() {
    if (emptyVAO == 0) glGenVertexArrays(1, &emptyVAO);

    float time = static_cast<float>(glfwGetTime());

    backgroundShader.use();
    backgroundShader.setFloat("time", time);
    backgroundShader.setFloat("seed", static_cast<float>(params.seed));

    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);

    glBindVertexArray(emptyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDepthMask(GL_TRUE);
}