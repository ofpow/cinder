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
    vec3 attenuated = vec3(1.0);
    for (int i = 0; i < 50; ++i) {
        hit_record rec = hit_record(0, vec3(0), vec3(0));
        if (hit(r, 0.001, FLT_MAX, rec)) {
            vec3 target = rec.p + rec.normal + random_in_unit_sphere();
            attenuated *= 0.5;
            r = Ray(rec.p, normalize(target - rec.p));
        } else {
            vec3 unit_dir = normalize(r.direction);
            float t = 0.5 * (-unit_dir.y + 1.0);
            vec3 col = mix(vec3(1.0), vec3(0.5, 0.7, 1.0), t);
            return attenuated * col;
        }
    }
}

vec3 lower_left_corner = vec3(-2.0, -1.0, -1.0);
vec3 horizontal = vec3(4.0, 0.0, 0.0);
vec3 vertical = vec3(0.0, 2.0, 0.0);
vec3 origin = vec3(0.0, 0.0, 0.0);

Camera c = Camera(origin, lower_left_corner, horizontal, vertical);

uniform int rand_seed;

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    rand_state = ((x*1488 + y*6883) & 1878723) * rand_seed;

    int aa_steps = 10;
    vec3 col = vec3(0);
    for (int i = 0; i < aa_steps; i++) {
        float u = float(x + rand()) / X;
        float v = float(y + rand()) / Y;
        Ray ray = get_ray(c, u, v);
        col += color(ray);
    }
    vec4 current = buf[x + y*X];
    vec3 new_color = current.xyz + sqrt(col / float(aa_steps));
    buf[x + y*X] = vec4(new_color, current.w + 1);
}
