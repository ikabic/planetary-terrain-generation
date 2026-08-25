#version 330 core

out vec2 vUV;

void main() {
    // full-screen triangle covering [-1, 1] NDC
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);

    vUV = vec2(x, y) * 0.5 + 0.5;
    gl_Position = vec4(x, y, 0.999f, 1.0);
}