#pragma once

#include <math.h>

typedef struct vec3 {
    float x;
    float y;
    float z;
} vec3;

void print_vec3(vec3 v) {
    printf("{%f, %f, %f}\n", v.x, v.y, v.z);
}

vec3 add_vec3(vec3 a, vec3 b) {
    return (vec3){
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

vec3 scale_vec3(vec3 v, float s) {
    return (vec3){v.x * s, v.y * s, v.z * s};
}

float vec3_length(vec3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3 unit_vector(vec3 v) {
    return scale_vec3(v, 1 / vec3_length(v));
}
