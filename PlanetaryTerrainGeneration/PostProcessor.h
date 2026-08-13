#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Shader.h"

class PostProcessor {
public:
    PostProcessor() = default;

    void init(int width, int height);
    void begin();
	void end();
    void render(const Shader& shader, float pixelScale = 6.0f);
    void resize(int newWidth, int newHeight);
    void cleanup();

private:
    unsigned int fbo = 0;
    unsigned int fboTexture = 0;
    unsigned int rbo = 0;

    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;

    int width = 0;
    int height = 0;

    void initFrameBuffer();
    void initQuad();
};

inline PostProcessor postProcessor;