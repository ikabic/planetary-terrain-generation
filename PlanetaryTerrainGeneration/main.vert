#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aElevation;
layout (location = 2) in mat4 aInstanceModel; // locations 2, 3, 4, 5
layout (location = 6) in vec3 aInstanceColor;

out float elevation;
out vec3 worldPos;
out vec3 instanceColour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool useInstancing;

void main() {
    elevation = aElevation;
    worldPos = vec3(model * vec4(aPos, 1.0));
    mat4 finalModel = useInstancing ? aInstanceModel : model;

    gl_Position = projection * view * finalModel * vec4(aPos, 1.0);
    if (useInstancing) instanceColour = aInstanceColor;
}
