#version 410 core 

out vec4 FragColor; 

uniform vec2 uResolution;
uniform float uTime;

void main() { 
    vec2 uv = gl_FragCoord.xy / uResolution;

    float red   = 0.5 + 0.5 * sin(uTime);
    float green = 0.5 + 0.5 * sin(uTime*2);
    

    FragColor = vec4(
        red, 
        green,
        1.0 - uv.x,
        1.0
    ); 
}