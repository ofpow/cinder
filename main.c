#include <stdio.h>

#include "vec3.h"
#include "ray.h"

#define X 200
#define Y 100

vec3 color(Ray r) {
    vec3 unit_direction = unit_vector(r.direction);
    float t = 0.5 * (unit_direction.y + 1.0);
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
