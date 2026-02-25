layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(std430, binding = 1) restrict buffer screen {
    vec4 buf[];
};

uniform int X;
uniform int Y;

const float FLT_MAX = 3.40282347e+38;
vec3 color(Ray r) {
    vec3 final_color = vec3(1.0);
    for (int i = 0; i < 50; ++i) {
        hit_record rec = hit_record(0, vec3(0), vec3(0), Material(LAMBERTIAN, vec3(0)));
        if (hit(r, 0.001, FLT_MAX, rec)) {
            vec3 attenuation = vec3(0);
            Ray scattered = Ray(vec3(0), vec3(0));
            if (scatter(rec.mat, r, rec, attenuation, scattered)) {
                final_color *= attenuation;
                r = scattered;
            } else {
                final_color *= 0.5;
                vec3 target = rec.p + rec.normal + random_in_unit_sphere();
                r = Ray(rec.p, normalize(target - rec.p));
            }
        } else {
            vec3 unit_dir = normalize(r.direction);
            float t = 0.5 * (-unit_dir.y + 1.0);
            vec3 col = mix(vec3(1.0), vec3(0.5, 0.7, 1.0), t);
            return final_color * col;
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
