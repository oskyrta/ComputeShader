#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat3 projection;
uniform mat3 view;
uniform mat3 model;
uniform float depth;

void main() {
    gl_Position = vec4((projection * view * model * vec3(position, 1.0f)).xy, depth, 1.0f);
    TexCoord = aTexCoord;
}