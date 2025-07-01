#version 330 core

uniform vec4 u_Color;

uniform bool u_UseTexture;
uniform sampler2D u_Texture;

uniform bool u_CircleMask;

in vec2 TexCoords;
out vec4 FragColor;

void main() {
    if(u_CircleMask) {
        vec2 centered = TexCoords - vec2(0.5);
        float dist = length(centered);
        if(dist > 0.5)
            discard;
    }

    vec4 texColor = u_UseTexture ? texture(u_Texture, TexCoords) : vec4(1.0);
    FragColor = u_Color * texColor;
}