#version 410 core 

out vec4 FragColor; 

void main() { 
    vec2 uv = gl_FragCoord.xy / vec2(1280.0, 720.0);

    FragColor = vec4(
        uv.x, 
        uv.y,
        1.0 - uv.x,
        1.0
    ); 
}