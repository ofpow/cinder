#version 430

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(std430, binding = 1) restrict buffer buf_layout {
    vec4 buf[];
};

uniform int X;
uniform int Y;

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    buf[x + y*X] = vec4(float(x) / X, float(y) / Y, 0.2, 1.0);
}
