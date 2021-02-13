#version 330 core

out vec4 color;
in vec2 TexCoord;

uniform sampler2D outTexture;

void main() {
    color = texture(outTexture, TexCoord);
}