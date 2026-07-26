#version 330 core

out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;
uniform float pixelScale;
uniform vec2 resolution; // screen res

void main() {
    vec2 dxdy = pixelScale / resolution;
    vec2 snappedUV = floor(texCoords / dxdy) * dxdy;
    fragColor = texture(screenTexture, snappedUV);
}