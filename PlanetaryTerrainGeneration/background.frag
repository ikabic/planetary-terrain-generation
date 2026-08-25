#version 330 core

out vec4 fragColour;

uniform float time;
uniform float seed;

float hash31(vec3 p) {
    p = fract(p * vec3(0.1031, 0.1030, 0.0973));
    p += dot(p, p.yzx + 33.33);
    return fract((p.x + p.y) * p.z);
}

vec3 hsvToRgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - 3.0);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    ivec2 pixelCoord = ivec2(gl_FragCoord.xy);  // integer pixel coordinate of the current fragment

    int cellSize = 16;  // screen cell is 16x16 pixels
    ivec2 cellId = pixelCoord / cellSize;
    ivec2 localPos = pixelCoord % cellSize; // offset within the cell

    float s = mod(seed, 10000.0);

    float bgHue = hash31(vec3(9.87, 1.23, s));
    vec3 bgColour = hsvToRgb(vec3(bgHue, 0.55, 0.045));
    vec3 finalColour = bgColour;

    float starThreshold = 0.70; // 30% of cells contain a star
    float starChance = hash31(vec3(cellId, s));

    if (starChance > starThreshold) {
        ivec2 starPos = ivec2(1 + int(hash31(vec3(cellId, s + 1.0)) * float(cellSize - 2)), 1 + int(hash31(vec3(cellId, s + 2.0)) * float(cellSize - 2)));

        if (localPos == starPos) {
            float colourHash = hash31(vec3(cellId, s + 5.0));
            vec3 starColour;

            if (colourHash < 0.40)       starColour = vec3(1.00, 0.98, 0.94); // warm white
            else if (colourHash < 0.65)  starColour = vec3(0.88, 0.93, 1.00); // blue-white
            else if (colourHash < 0.80)  starColour = vec3(1.00, 0.94, 0.78); // yellow
            else if (colourHash < 0.93)  starColour = vec3(1.00, 0.82, 0.68); // orange
            else                         starColour = vec3(1.00, 0.70, 0.62); // red

            float speed = 0.5 + hash31(vec3(cellId, s + 3.0)) * 1.5;
            float phase = hash31(vec3(cellId, s + 4.0)) * 6.2831;
            float twinkle = pow(sin(time * speed + phase) * 0.5 + 0.5, 1.2);

            float brightness = 0.2 + 0.8 * twinkle;
            finalColour += starColour * brightness;
        }
    }

    fragColour = vec4(finalColour, 1.0);
}