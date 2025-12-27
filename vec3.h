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

vec3 subtract_vec3(vec3 a, vec3 b) {
    return (vec3){
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

vec3 mult_vec3(vec3 a, vec3 b) {
    return (vec3) {
        a.x * b.x,
        a.y * b.y,
        a.z * b.z
    };
}

vec3 scale_vec3(vec3 v, float s) {
    return (vec3){v.x * s, v.y * s, v.z * s};
}

float vec3_length(vec3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

float squared_length(vec3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;   
}

vec3 unit_vector(vec3 v) {
    return scale_vec3(v, 1 / vec3_length(v));
}

float dot(vec3 v1, vec3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; 
}

vec3 reflect(vec3 v, vec3 n) {
    return subtract_vec3(
        v,
        scale_vec3(
            n,
            2*dot(v, n)
        )
    );
}
