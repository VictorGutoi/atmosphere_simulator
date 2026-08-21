#version 410 core 

out vec4 FragColor; 

uniform vec2 uResolution;
uniform float uTime;

void main() { 
    vec2 uv = gl_FragCoord.xy / uResolution;

    float red   = 0.5 + 0.5 * sin(uTime);
    float green = 0.5 + 0.5 * sin(uTime*2);
    float blue  = 0.5 + 0.5 * sin(uTime*3);
    

    FragColor = vec4(
        uv.x*red, 
        uv.y*green,
        (1.0 - uv.x)*blue,
        1.0
    ); 
}