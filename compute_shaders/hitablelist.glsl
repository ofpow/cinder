#define SPHERE 1

struct Hitable {
    uint type;
    float data[8];
};

layout(std430, binding = 2) restrict buffer Hitables {
    Hitable hitables[];
};

uniform int num_hitables;

bool hit(Ray r, float t_min, float t_max, inout hit_record rec) {
    hit_record temp_rec = hit_record(0, vec3(0), vec3(0), Material(LAMBERTIAN, vec3(0)));
    bool hit_anything = false;
    float closest_so_far = t_max;
    for (int i = 0; i < num_hitables; i++) {
        bool result = false;
        switch (hitables[i].type) {
            case SPHERE: 
                Sphere s = hitable_sphere(hitables[i].data);
                result = sphere_hit(s, r, t_min, closest_so_far, temp_rec); break;
            default:
                return false;
        }
        if (result) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}
