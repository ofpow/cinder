#pragma once

typedef struct Sphere {
    vec3 center;
    float radius;
    Material mat;
} Sphere;

Sphere *new_sphere(vec3 center, float radius, Material mat) {
    Sphere *s = calloc(1, sizeof(Sphere));
    s->center = center;
    s->radius = radius;
    s->mat = mat;
    return s;
}

bool sphere_hit(Sphere *s, Ray r, float t_min, float t_max, hit_record *rec) {
    vec3 oc = subtract_vec3(r.origin, s->center);
    
    float a = dot(r.direction, r.direction);
    float b = dot(oc, r.direction);
    float c = dot(oc, oc) - s->radius*s->radius;
    float discriminant = b*b - a*c;

    if (discriminant > 0) {
        float temp = (-b - sqrtf(b*b - a*c))/a;
        if (temp < t_max && temp > t_min) {
            rec->t = temp;
            rec->p = point_at_parameter(&r, rec->t);
            rec->normal = scale_vec3(subtract_vec3(rec->p, s->center), (1 / s->radius));
            rec->mat = s->mat;
            return true;
        }
        temp = (-b + sqrtf(b*b - a*c))/a;
        if (temp < t_max && temp > t_min) {
            rec->t = temp;
            rec->p = point_at_parameter(&r, rec->t);
            rec->normal = scale_vec3(subtract_vec3(rec->p, s->center), (1 / s->radius));
            rec->mat = s->mat;
            return true;
        }
    }

    return false;
}

vec3 random_in_unit_sphere(unsigned short xsubi[3]) {
    vec3 p = {0, 0, 0};
    
    do {
        p = subtract_vec3(
            scale_vec3((vec3){erand48(xsubi), erand48(xsubi), erand48(xsubi)}, 2.0),
            (vec3){1, 1, 1}
        );
    } while (squared_length(p) >= 1.0);

    return p;
}
