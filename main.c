#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <float.h>

#define append(_array, _element) do {                                               \
    if (_array.index >= _array.capacity) {                                              \
        _array.capacity *= 2;                                                           \
        _array.data = realloc(_array.data, _array.capacity * sizeof(_array.data[0]));   \
    }                                                                                   \
                                                                                        \
    _array.data[_array.index++] = _element;                                             \
} while (0)                                                                             \

#define free_array(_array, _free) do {        \
    for (int i = 0; i < _array.index; i++) {  \
        _free(_array.data[i]);                \
    }                                         \
    free(_array.data);                        \
} while (0)                                   \

#define define_array(_name, _type) \
    typedef struct _name {         \
        _type *data;               \
        int64_t index;             \
        int64_t capacity;          \
    } _name                        \

#include "vec3.h"
#include "ray.h"
#include "hitable.h"
#include "sphere.h"
#include "hitablelist.h"
#include "camera.h"

#define X 600
#define Y 300
#define S 50

vec3 color(Ray r, Hitable_List world) {
    hit_record rec = {0};
    if (hit(world, r, 0.0, FLT_MAX, &rec)) {
        return scale_vec3((vec3){rec.normal.x + 1, rec.normal.y + 1, rec.normal.z + 1}, 0.5);
    } else {
        vec3 unit_direction = unit_vector(r.direction);
        float t = 0.5 * (unit_direction.y + 1.0);
        return add_vec3(
            scale_vec3((vec3){1.0, 1.0, 1.0}, (1.0-t)),
            scale_vec3((vec3){0.5, 0.7, 1.0}, t)
        );
    }
}

int main(void) {
    printf("P3\n");
    printf("%d %d\n", X, Y);
    printf("255\n");

    Hitable_List world = {
        calloc(10, sizeof(Hitable_Entry)),
        0,
        10
    };

    Sphere *s = new_sphere((vec3){0, 0, -1}, 0.5);
    append(world, ((Hitable_Entry){SPHERE, s}));
    s = new_sphere((vec3){0, -100.5, -1}, 100);
    append(world, ((Hitable_Entry){SPHERE, s}));

    for (int j = Y - 1; j >= 0; j--) {
        for (int i = 0; i < X; i++) {
            vec3 col = {0, 0, 0};
            for (int s = 0; s < S; s++) {
                float u = (float)(i + drand48()) / (float)X;
                float v = (float)(j + drand48()) / (float)Y;
                Ray r = get_ray(u, v); 
                col = add_vec3(col, color(r, world));
            }
            
            col = scale_vec3(col, 1.0/S);
            printf("%d %d %d\n", (int)(255.99*col.x), (int)(255.99*col.y), (int)(255.99*col.z));
        }
    }
    
    return 0;
}
