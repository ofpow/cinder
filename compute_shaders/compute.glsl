layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(std430, binding = 1) restrict buffer screen {
    vec4 buf[];
};

uniform int X;
uniform int Y;
const float FLT_MAX = 3.40282347e+38;
vec3 color(Ray r) {
    hit_record rec = hit_record(0, vec3(0), vec3(0));
    if (hit(r, 0.0, FLT_MAX, rec)) {
        return vec3(rec.normal.x + 1, rec.normal.y + 1, rec.normal.z + 1) * 0.5;
    } else {
        vec3 unit_dir = unit_vector(r.direction);
        float t = 0.5*(-unit_dir.y + 1.0);
        return (1.0-t)*vec3(1.0) + t*vec3(0.5, 0.7, 1.0);
    }
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
