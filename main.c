#include <stdio.h>

#define X 200
#define Y 100

int main(void) {
    printf("P3\n");
    printf("%d %d\n", X, Y);
    printf("255\n");

    for (int j = Y - 1; j >= 0; j--) {
        for (int i = 0; i < X; i++) {
            float r = (float)i / (float)X;
            float g = (float)j / (float)Y;
            float b = 0.2;

            printf("%d %d %d\n", (int)(255.99*r), (int)(255.99*g), (int)(255.99*b));
        }
    }
    
    return 0;
}
