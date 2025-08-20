#version 120

out vec4 outColor;

uniform vec4 color; // solid colour

void main(){

    vec2 uv = gl_TexCoord[0].xy;

    vec2 local = uv - vec2(0.5);
    if(length(local) > 0.5)
        discard;

    gl_FragColor = gl_Color;
}
