#version 430

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(std430, binding = 1) restrict buffer buf_layout {
    vec4 buf[];
};

uniform int X;
uniform int Y;

struct Ray {
    vec3 origin;
    vec3 direction;
};

vec3 unit_vector(vec3 v) {
    return v / length(v);
}

vec3 color(Ray r) {
    vec3 unit_dir = unit_vector(r.direction);
    float t = 0.5*(-unit_dir.y + 1.0);
    return (1.0-t)*vec3(1.0) + t*vec3(0.5, 0.7, 1.0);
}

vec3 lower_left_corner = vec3(-2.0, -1.0, -1.0);
vec3 horizontal = vec3(4.0, 0.0, 0.0);
vec3 vertical = vec3(0.0, 2.0, 0.0);
vec3 origin = vec3(0.0, 0.0, 0.0);

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    float u = float(x) / X;
    float v = float(y) / Y;
    Ray r = Ray(origin, lower_left_corner + u*horizontal + v*vertical);
    buf[x + y*X] = vec4(color(r), 1.0);
}
