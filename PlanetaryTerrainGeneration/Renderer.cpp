#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>

#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"
#include "Planet.h"

void drawPlanet() {
    planet = Planet(64);
    planet.build();

    mainShader.use();
    mainShader.setMat4("model", glm::mat4(1.0f));
    mainShader.setMat4("view", camera.getViewMatrix());
    mainShader.setMat4("projection", camera.getProjectionMatrix());

    mainShader.setVec3("lightPos", glm::vec3(5.0f, 5.0f, 5.0f));
    mainShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    mainShader.setVec3("objectColor", glm::vec3(0.3f, 0.6f, 1.0f));

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
	planet.draw();
}