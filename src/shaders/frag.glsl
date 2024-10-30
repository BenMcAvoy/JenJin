#version 460

out vec4 FragColor;

uniform vec3 colour;

void main() {
    FragColor = vec4(colour, 1.0);
}
