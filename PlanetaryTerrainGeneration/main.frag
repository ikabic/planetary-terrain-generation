#version 330 core

in float elevation;
in vec3 worldPos;
in vec3 instanceColour;

out vec4 fragColour;

uniform bool useLighting;
uniform vec3 lightDir;

uniform bool useInstancing;

uniform vec3 colours[16];
uniform float upperBounds[16];
uniform int paletteSize;

float calculateToonLighting() {
    vec3 normal = normalize(worldPos);

    float light = max(dot(normal, lightDir), 0.0);

    if (light > 0.45) return 1.00;
    else if (light > 0.1) return 0.9;
    else return 0.8;
}

vec3 terrainColor(float e) {
    for (int i = 0; i < paletteSize-1; i++) {
        if (elevation < upperBounds[i]) return colours[i];
    }
    return colours[paletteSize-1];
}

void main() {
    vec3 colour = useInstancing ? instanceColour : terrainColor(elevation);
    float lightFactor = calculateToonLighting();
    
    fragColour = vec4(colour * (useLighting ? lightFactor : 1.0), 1.0);
}
