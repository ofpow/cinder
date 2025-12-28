#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>

#include <omp.h>

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
#include "material.h"

#define X 3000
#define Y 1500
#define S 25

void print_progress(size_t count, size_t max) {
    int bar_width = 50;
    float progress = (float) count / max;
    int bar_length = progress * bar_width;

    printf("\rProgress: ["); 
    for (int i = 0; i < bar_length; ++i) {
        printf("#");
    }
    for (int i = bar_length; i < bar_width; ++i) {
        printf(" ");
    }
    printf("] %.2f%%", progress * 100);

    fflush(stdout); 
}

vec3 color(Ray r, Hitable_List world, int depth, unsigned short xsubi[3]) {
    hit_record rec = {0};
    if (hit(world, r, 0.001, FLT_MAX, &rec)) {
        Ray scattered = {0};
        vec3 attenuation = {0};
        if (depth < 50 && scatter(rec.mat, &r, &rec, &attenuation, &scattered, xsubi)) {
            return mult_vec3(
                attenuation,
                color(scattered, world, depth + 1, xsubi)
            );
        } else {
            return (vec3){0, 0, 0};
        }
    } else {
        vec3 unit_direction = unit_vector(r.direction);
        float t = 0.5 * (unit_direction.y + 1.0);
        return add_vec3(
            scale_vec3((vec3){1.0, 1.0, 1.0}, (1.0-t)),
            scale_vec3((vec3){0.5, 0.7, 1.0}, t)
        );
    }
}

vec3 out[X][Y];

int main(void) {
    Hitable_List world = {
        calloc(10, sizeof(Hitable_Entry)),
        0,
        10
    };

    Sphere *s = new_sphere(
        (vec3){0, 0, -1}, 0.5,
        (Material){Lambertian, (vec3){0.1, 0.2, 0.5}, 0}
    );
    append(world, ((Hitable_Entry){SPHERE, s}));
    s = new_sphere(
        (vec3){0, -100.5, -1}, 100,
        (Material){Lambertian, (vec3){0.8, 0.8, 0}, 0}
    );
    append(world, ((Hitable_Entry){SPHERE, s}));
    s = new_sphere(
        (vec3){1, 0, -1}, 0.5,
        (Material){Metal, (vec3){0.8, 0.6, 0.2}, 0.15}
    );
    append(world, ((Hitable_Entry){SPHERE, s}));
    s = new_sphere(
        (vec3){-1, 0, -1}, 0.5,
        (Material){Dielectric, (vec3){0}, 1.5}
    );
    append(world, ((Hitable_Entry){SPHERE, s}));
    s = new_sphere(
        (vec3){-1, 0, -1}, -0.45,
        (Material){Dielectric, (vec3){0}, 1.5}
    );
    append(world, ((Hitable_Entry){SPHERE, s}));

    int num_threads = 16;

    int count = 0;
#pragma omp parallel for schedule(dynamic) num_threads(num_threads) \
    shared(world) shared(out) shared(count)
    for (int y = Y - 1; y >= 0; y--) {
        if (omp_get_thread_num() == 0)
            print_progress(count, X*Y);
        
        unsigned short xsubi[3];

        unsigned int seed = time(NULL) ^ omp_get_thread_num(); 
        xsubi[0] = (unsigned short)seed;
        xsubi[1] = (unsigned short)(seed >> 16);
        xsubi[2] = (unsigned short)(omp_get_thread_num());

        for (int x = 0; x < X; x++) {
            {
                vec3 col = {0, 0, 0};
                for (int s = 0; s < S; s++) {
                    float u = (float)(x + erand48(xsubi)) / (float)X;
                    float v = (float)(y + erand48(xsubi)) / (float)Y;
                    Ray r = get_ray(u, v); 
                    col = add_vec3(col, color(r, world, 0, xsubi));
                }
                
                col = scale_vec3(col, 1.0/S);
                col = (vec3){sqrtf(col.x), sqrtf(col.y), sqrtf(col.z)};
                out[x][y] = col;
            }
        }
        count += X;
    }
    print_progress(count, X*Y);
    printf("\n");

    FILE *f = fopen("out.ppm", "w");

    fprintf(f, "P3\n");
    fprintf(f, "%d %d\n", X, Y);
    fprintf(f, "255\n");

    for (int j = Y - 1; j >= 0; j--) {
        for (int i = 0; i < X; i++) {
            vec3 col = out[i][j];
            fprintf(f, "%d %d %d\n", (int)(255.99*col.x), (int)(255.99*col.y), (int)(255.99*col.z));
        }
    }

    fclose(f); 
    return 0;
}
