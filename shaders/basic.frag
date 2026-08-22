#version 410 core 

in vec3 vNormal;

out vec4 FragColor; 

uniform vec3 uLightDirection;

void main() {
    vec3 normal = normalize(vNormal);

    vec3 lightDirection = normalize(uLightDirection);

    float brightness = max(
        dot(normal, lightDirection),
        0.0
    );

    vec3 baseColor = vec3(0.2, 0.5, 1.0);
    vec3 color = baseColor * brightness;

    FragColor = vec4(color, 1.0); 
}