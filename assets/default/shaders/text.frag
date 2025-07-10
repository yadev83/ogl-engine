#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D u_Text;
uniform vec3 u_Color;

void main() {
    float alpha = texture(u_Text, TexCoords).r;
    color = vec4(u_Color, alpha);
}