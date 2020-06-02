#version 330 core
in vec4 texCoords;
in vec4 particlecolor;
out vec4 color;

uniform sampler2D sprite;

void main() {
	color = (texture(sprite, texCoords) * particleColor);
}