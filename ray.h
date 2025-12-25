#pragma once

typedef struct Ray {
    vec3 origin;
    vec3 direction;
} Ray;

vec3 point_at_parameter(Ray *r, float t) {
    return add_vec3(r->origin, scale_vec3(r->direction, t));
}
