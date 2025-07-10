#version 330 core

uniform vec4 u_Color;
uniform vec2 u_UVScroll;

uniform bool u_UseTexture;
uniform sampler2D u_Texture;

in vec2 TexCoords;
out vec4 FragColor;

void main() {
    vec2 uv = TexCoords + u_UVScroll;
    vec4 texColor = u_UseTexture ? texture(u_Texture, uv) : vec4(1.0);
    FragColor = u_Color * texColor;
}