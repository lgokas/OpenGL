#version 430
layout (location=0) in vec3 position;

uniform mat4 model;

out vec2 localPos;

void main(void)
{
    gl_Position = model * vec4(position, 1.0);
    localPos = position.xy;
}