layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(std430, binding = 1) restrict buffer screen {
    vec4 buf[];
};

uniform int X;
uniform int Y;

vec3 rand_point_on_triangle(Triangle t) {
    float sqrt_rand = sqrt(rand());
    
    float u = 1.0 - sqrt_rand;
    float v = rand() * sqrt_rand;
    float w = 1.0 - u - v;

    return (u * t.a) + (v * t.b) + (w * t.c);
}

LightSample sample_light() {
    int i = int(rand() * num_lights);
    Triangle t = hitable_triangle(lights[i].data);
    vec3 point = rand_point_on_triangle(t);

    vec3 normal = normalize(cross(t.b - t.a, t.c - t.a));

    vec3 emission = t.mat.emission_col * t.mat.emission_str;

    float area = length(cross(t.b - t.a, t.c - t.a)) * 0.5;
    float pdf = 1.0 / (float(num_lights));

    return LightSample(point, normal, emission, pdf);
}

const float FLT_MAX = 3.40282347e+38;
vec3 color(Ray r) {
    vec3 light = vec3(0);
    vec3 final_color = vec3(1.0);
    for (int i = 0; i < 50; ++i) {
        hit_record rec;
        if (hit(r, 0.001, FLT_MAX, rec)) {
            vec3 emitted_light = rec.mat.emission_col * rec.mat.emission_str;
            if (length(emitted_light) > 0) {
                light += emitted_light * final_color;
                break;
            }
            
            if (rec.mat.emission_str <= 0.0) {
                LightSample ls = sample_light();

                vec3 to_light = ls.position - rec.p;
                float dist2 = dot(to_light, to_light);
                float dist = sqrt(dist2);
                vec3 wi = to_light / dist;

                float cos_surface = max(dot(rec.normal, wi), 0.0);
                float cos_light   = max(dot(ls.normal, -wi), 0.0);

                if (cos_surface > 0.0 && cos_light > 0.0)
                {
                    Ray shadow = Ray(rec.p + rec.normal * 0.001, wi);
                    hit_record shadow_rec;

                    if (!hit(shadow, 0.001, dist - 0.001, shadow_rec))
                    {
                        float pdfW = ls.pdf * dist2 / cos_light;
                        vec3 f = rec.mat.albedo / M_PI;
                        vec3 contribution = ls.emission * f * cos_surface / pdfW;

                        light += final_color * contribution;
                    }
                }
            }

            vec3 attenuation;
            Ray scattered;
            if (scatter(rec.mat, r, rec, attenuation, scattered)) {
                final_color *= attenuation;
                
                r = scattered;
            } else {
                return vec3(0);
            }

            if (i > 3) {
                float p = max(final_color.r, max(final_color.g, final_color.b));
                p = clamp(p, 0.05, 0.95);

                if (rand() > p) break;

                final_color /= p;
            }
        } else {
            break;
            //vec3 unit_dir = normalize(r.direction);
            //float t = 0.5 * (unit_dir.y + 1.0);
            //vec3 col = mix(vec3(1.0), vec3(0.5, 0.7, 1.0), t);
            //light += col*final_color;
            //break;
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
    vec3 new_color = current.xyz + c;
    buf[x + y*X] = vec4(new_color, current.w + 1);
}
