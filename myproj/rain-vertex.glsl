#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoord>

out vec4 texCoords;
out vec4 particleColor;

uniform mat4 projection;
uniform vec2 offset;
uniform vec4 color;

void main() {
	float scale = 10.0f;
	textCoords = vertex.zw;
	particleColor = color;
	gl_Position = projection * vec4((vertex.xy * scale) + ofsset, 0.0, 1.0);
}