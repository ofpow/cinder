#version 430

in vec2 fragTexCoord;

out vec4 finalColor;

layout(std430, binding = 1) readonly buffer buf_layout {
    vec4 buf[];
};

uniform vec2 resolution;
uniform int X;

void main() {
    ivec2 coords = ivec2(fragTexCoord*resolution);
    finalColor = buf[coords.x + X*coords.y];
}
