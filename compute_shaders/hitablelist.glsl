#define SPHERE 1

struct Hitable {
    uint type;
    float data[4];
};

layout(std430, binding = 2) restrict buffer Hitables {
    Hitable hitables[];
};

uniform int num_hitables;

bool hit(Ray r, float t_min, float t_max, inout hit_record rec) {
    hit_record temp_rec = hit_record(0, vec3(0), vec3(0));
    bool hit_anything = false;
    float closest_so_far = t_max;
    for (int i = 0; i < num_hitables; i++) {
        bool result = false;
        switch (hitables[i].type) {
            case SPHERE: 
                Sphere s = Sphere(vec3(hitables[i].data[0], hitables[i].data[1], hitables[i].data[2]), hitables[i].data[3]);
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
    rec.normal.y = -rec.normal.y;
    return hit_anything;
}
