#version 330 core

layout (location = 0) in vec2 aPosition;

uniform uint id;

uniform mat3 projection;
uniform mat3 view;
uniform mat3 model;

uniform vec4 line_color;

uniform float depth;

out vec4 fragColor;

void main() {
	vec4 pos = vec4(projection * view * vec3(aPosition, 1.f), 1.f);
	pos.z = depth;
	gl_Position = pos;
	
	fragColor = line_color;
}