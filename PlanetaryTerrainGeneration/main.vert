#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aElevation;

out float elevation;
out vec3 worldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    elevation = aElevation;
    worldPos = vec3(model * vec4(aPos, 1.0));

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
