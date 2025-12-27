#pragma once

typedef struct Sphere {
    vec3 center;
    float radius;
} Sphere;

Sphere *new_sphere(vec3 center, float radius) {
    Sphere *s = calloc(1, sizeof(Sphere));
    s->center = center;
    s->radius = radius;
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
            return true;
        }
        temp = (-b + sqrtf(b*b - a*c))/a;
        if (temp < t_max && temp > t_min) {
            rec->t = temp;
            rec->p = point_at_parameter(&r, rec->t);
            rec->normal = scale_vec3(subtract_vec3(rec->p, s->center), (1 / s->radius));
            return true;
        }
    }

    return false;
}
