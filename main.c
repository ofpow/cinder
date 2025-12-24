#include <stdio.h>

#include "vec3.h"

#define X 200
#define Y 100

int main(void) {
    printf("P3\n");
    printf("%d %d\n", X, Y);
    printf("255\n");

    for (int j = Y - 1; j >= 0; j--) {
        for (int i = 0; i < X; i++) {
            vec3 v = {
                (float)i / (float)X,
                (float)j / (float)Y,
                0.2
            };

            printf("%d %d %d\n", (int)(255.99*v.x), (int)(255.99*v.y), (int)(255.99*v.z));
        }
    }
    
    return 0;
}
