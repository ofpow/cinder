#pragma once

typedef struct vec3 {
    float x;
    float y;
    float z;
} vec3;

void print_vec3(vec3 v) {
    printf("{%f, %f, %f}\n", v.x, v.y, v.z);
}
