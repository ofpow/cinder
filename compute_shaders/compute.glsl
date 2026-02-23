layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(std430, binding = 1) restrict buffer screen {
    vec4 buf[];
};

uniform int X;
uniform int Y;

uint rand_state = 0;

float rand() {
    rand_state = 1664525u * rand_state + 1013904223u;
    return float(rand_state) / 4294967296.0;
}

vec3 random_in_unit_sphere() {
    vec3 p = vec3(0);
    do {
        p = 2.0*vec3(rand(), rand(), rand()) - vec3(1);
    } while ((p.x*p.x + p.y*p.y + p.z*p.z) >= 1.0);
    return p;
}

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

Camera c = Camera(origin, lower_left_corner, horizontal, vertical);

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    rand_state = (x*1488 + y*6883) & 1878723;

    int aa_steps = 10;
    if (aa_steps > 0) { 
        vec3 col = vec3(0);
        for (int i = 0; i < aa_steps; i++) {
            float u = float(x + rand()) / X;
            float v = float(y + rand()) / Y;
            Ray ray = get_ray(c, u, v);
            col += color(ray);
        }
        buf[x + y*X] = vec4(col / float(aa_steps), 1.0);
    } else {
        float u = float(x) / X;
        float v = float(y) / Y;
        Ray ray = get_ray(c, u, v);
        buf[x + y*X] = vec4(color(ray), 1.0);
    }
}
