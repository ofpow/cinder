#include <stdio.h>

#include "vec3.h"
#include "ray.h"

#define X 600
#define Y 300

float hit_sphere(vec3 center, float radius, Ray r) {
    vec3 oc = subtract_vec3(r.origin, center);
    
    float a = dot(r.direction, r.direction);
    float b = dot(oc, r.direction) * 2.0;
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - 4*a*c;

    if (discriminant < 0) return -1.0;

    return (-b - sqrtf(discriminant)) / (2.0 * a);
}

vec3 color(Ray r) {
    float t = hit_sphere((vec3){0,0,-1}, 0.5, r);
    if (t > 0) {
        vec3 n = subtract_vec3(unit_vector(point_at_parameter(&r, t)), (vec3){0, 0, -1});
        return scale_vec3((vec3){n.x + 1, n.y + 1, n.z + 1}, 0.5);
    }

    vec3 unit_direction = unit_vector(r.direction);
    t = 0.5 * (unit_direction.y + 1.0);
    return add_vec3(
        scale_vec3((vec3){1.0, 1.0, 1.0}, (1.0-t)),
        scale_vec3((vec3){0.5, 0.7, 1.0}, t)
    );
}

int main(void) {
    printf("P3\n");
    printf("%d %d\n", X, Y);
    printf("255\n");

    vec3 lower_left_corner = {-2.0, -1.0, -1.0};
    vec3 horizontal = {4.0, 0.0, 0.0};
    vec3 vertical = {0.0, 2.0, 0.0};
    vec3 origin = {0.0, 0.0, 0.0};

    for (int j = Y - 1; j >= 0; j--) {
        for (int i = 0; i < X; i++) {
            float u = (float)i / (float)X;
            float v = (float)j / (float)Y;

            Ray r = {
                origin,
                add_vec3(
                    lower_left_corner,
                    add_vec3(
                        scale_vec3(horizontal, u),
                        scale_vec3(vertical, v)
                    )
                )
            };

            vec3 col = color(r);

            printf("%d %d %d\n", (int)(255.99*col.x), (int)(255.99*col.y), (int)(255.99*col.z));
        }
    }
    
    return 0;
}
