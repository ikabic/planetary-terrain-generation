#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <string>

struct Shader {
    unsigned int ID = 0;

    Shader() = default;
    Shader(const char* vert, const char* frag) {
        auto load = [](const char* path, GLenum type) {
            std::ifstream f(path); std::stringstream ss; ss << f.rdbuf();
            std::string src = ss.str(); const char* c = src.c_str();

            unsigned int s = glCreateShader(type);
            glShaderSource(s, 1, &c, NULL);
            glCompileShader(s);

            int ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
            if (!ok) { char log[512]; glGetShaderInfoLog(s, 512, NULL, log); printf("SHADER ERR: %s\n", log); }
            return s;
        };

        unsigned int v = load(vert, GL_VERTEX_SHADER);
        unsigned int f = load(frag, GL_FRAGMENT_SHADER);
        ID = glCreateProgram();
        glAttachShader(ID, v); glAttachShader(ID, f);
        glLinkProgram(ID);
        glDeleteShader(v); glDeleteShader(f);
    }

    void use() const { glUseProgram(ID); }

    void setMat4(const char* n, const glm::mat4& v) const { glUniformMatrix4fv(glGetUniformLocation(ID, n), 1, GL_FALSE, glm::value_ptr(v)); }
    void setVec3(const char* n, const glm::vec3& v) const { glUniform3fv(glGetUniformLocation(ID, n), 1, glm::value_ptr(v)); }
    void setFloat(const char* n, float v) const { glUniform1f(glGetUniformLocation(ID, n), v); }
    void setInt(const char* n, int v) const { glUniform1i(glGetUniformLocation(ID, n), v); }
    void setVec2(const char* n, const glm::vec2& v) const { glUniform2fv(glGetUniformLocation(ID, n), 1, glm::value_ptr(v)); }
	void setBool(const char* n, bool v) const { glUniform1i(glGetUniformLocation(ID, n), (int)v); }
    void setVec3Array(const char* n, const std::vector<glm::vec3>& v) const { if (v.empty()) return; glUniform3fv(glGetUniformLocation(ID, n), static_cast<GLsizei>(v.size()), &v[0].x); }
    void setFloatArray(const char* n, const std::vector<float>& v) const { if (v.empty()) return;  glUniform1fv(glGetUniformLocation(ID, n), static_cast<GLsizei>(v.size()), v.data()); }
};

inline Shader mainShader, pixelationShader;
inline void initShaders() {
    mainShader = Shader("main.vert", "main.frag");
    pixelationShader = Shader("pixelation.vert", "pixelation.frag");
}