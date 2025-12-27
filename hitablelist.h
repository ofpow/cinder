#pragma once

typedef enum {
    SPHERE,
} Hitable_Type;

typedef struct Hitable_Entry {
    Hitable_Type type;
    void *data;
} Hitable_Entry;

define_array(Hitable_List, Hitable_Entry);

bool hit(Hitable_List hitables, Ray r, float t_min, float t_max, hit_record *rec) {
    hit_record temp_rec = {0};
    bool hit_anything = false;
    double closest_so_far = t_max;
    for (int i = 0; i < hitables.index; i++) {
        bool result = false;
        switch (hitables.data[i].type) {
            case SPHERE: result = sphere_hit((Sphere*)hitables.data[i].data, r, t_min, closest_so_far, &temp_rec); break;
            default: fprintf(stderr, "cant render shape %d\n", hitables.data[i].type); exit(1); 
        }
        if (result) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            *rec = temp_rec;
        }
    }

    return hit_anything;
}
