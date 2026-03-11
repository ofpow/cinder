layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(std430, binding = 1) restrict buffer screen {
    vec4 buf[];
};

uniform int X;
uniform int Y;

const float FLT_MAX = 3.40282347e+38;
vec3 color(Ray r) {
    vec3 light = vec3(0);
    vec3 final_color = vec3(1.0);
    for (int i = 0; i < 4; ++i) {
        hit_record rec = hit_record(0, vec3(0), vec3(0), MaterialData(LAMBERTIAN, vec3(0), 0, vec3(0), 0));
        if (hit(r, 0.001, FLT_MAX, rec)) {
            vec3 attenuation = vec3(0);
            Ray scattered = Ray(vec3(0), vec3(0));
            if (scatter(rec.mat, r, rec, attenuation, scattered)) {
                vec3 emitted_light = rec.mat.emission_col * rec.mat.emission_str;
                light += emitted_light * final_color;
                final_color *= attenuation;
                r = scattered;
            } else {
                return vec3(0);
            }
        } else {
            vec3 unit_dir = normalize(r.direction);
            float t = 0.5 * (-unit_dir.y + 1.0);
            vec3 col = mix(vec3(1.0), vec3(0.5, 0.7, 1.0), t);
            light += col*final_color;
            break;
        }
    }
    return light;
}

uniform int rand_seed;

uint hash(uvec2 p) {
    p = 1103515245U*((p >> 1U)^(p.yx));
    uint h32 = 1103515245U*((p.x)^(p.y>>3U));
    return h32^(h32 >> 16);
}

uniform int reset;

uniform vec3 lookfrom;
uniform vec3 lookat;
uniform vec3 vup;
uniform float aperture;
uniform int vfov;

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    rand_state = hash(gl_GlobalInvocationID.xy);
    rand_state = hash(uvec2(rand_state, rand_seed));
    
    float dist_to_focus = length(lookfrom-lookat);
    init_camera(lookfrom, lookat, vup, vfov, float(X)/float(Y), aperture, dist_to_focus);

    int aa_steps = 1;
    vec3 col = vec3(0);
    for (int i = 0; i < aa_steps; i++) {
        float u = float(x + rand()) / X;
        float v = float(y + rand()) / Y;
        Ray ray = get_ray(c, u, v);
        col += color(ray);
    }
    vec4 current = buf[x + y*X];
    if (reset == 1) current = vec4(0);

    vec3 c = max(col / float(aa_steps), vec3(0));
    vec3 new_color = current.xyz + sqrt(c);
    buf[x + y*X] = vec4(new_color, current.w + 1);
}
