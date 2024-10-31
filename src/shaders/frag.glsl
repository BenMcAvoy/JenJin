#version 460

out vec4 FragColor;

uniform vec3 colour;
uniform bool has_texture;
uniform sampler2D tex;

in vec2 texCoord;

void main() {
	FragColor = has_texture ? texture(tex, texCoord) : vec4(colour, 1.0);

	if (FragColor.a < 0.1) {
		discard;
	}
}
