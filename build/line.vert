#version 330 core

layout (location = 0) in vec2 aPosition;

// uniform vec4 color;

uniform mat3 projection;
uniform mat3 view;
uniform mat3 model;

uniform float depth;

out vec4 fragColor;

void main() {
	vec4 pos = vec4(projection * view * model * vec3(aPosition, 1.f), 1.f);
	pos.z = depth;
	gl_Position = pos;
	
	fragColor = vec4(0, 1, 0, 1);
}