#version 460

layout(location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    texCoord = position.xy + vec2(0.5, 0.5);
}
