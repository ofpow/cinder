#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define append(_array, _element) do {                                               \
    if (_array.index >= _array.capacity) {                                              \
        _array.capacity *= 2;                                                           \
        _array.data = realloc(_array.data, _array.capacity * sizeof(_array.data[0]));   \
    }                                                                                   \
                                                                                        \
    _array.data[_array.index++] = _element;                                             \
} while (0)

#define define_array(_name, _type) \
    typedef struct _name {         \
        _type *data;               \
        int64_t index;             \
        int64_t capacity;          \
    } _name                        \

#define SCALE (2048 / X)

#define SPHERE 1

#define LAMBERTIAN 1
#define METAL 2
#define DIELECTRIC 3

typedef struct Hitable {
    unsigned int type;
    float data[9];
} Hitable;

define_array(Hitables, Hitable);

Hitables setup_world(void) {
    Hitables hitables = (Hitables){
        calloc(10, sizeof(Hitable)),
        0,
        10
    };

    append(hitables, ((Hitable){
        .type = SPHERE,
        .data = {
            0, -1000, -2,     // center
            1000,          // radius
            LAMBERTIAN,   // mat.type
            0.5, 0.5, 0.5 // mat.albedo
        }
    }));
    append(hitables, ((Hitable){
        .type = SPHERE,
        .data = {
            0, 1, 0,     // center
            1,          // radius
            LAMBERTIAN,   // mat.type
            0.4, 0.2, 0.1 // mat.albedo
        }
    }));
    append(hitables, ((Hitable){
        .type = SPHERE,
        .data = {
            4, 1, 0,     // center
            1,          // radius
            METAL,   // mat.type
            0.7, 0.6, 0.5,
            0
        }
    }));
    append(hitables, ((Hitable){
        .type = SPHERE,
        .data = {
            -4, 1, 0,     // center
            1,          // radius
            DIELECTRIC,   // mat.type
            0.7, 0.6, 0.5,
            1.5
        }
    }));
    
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float mat = drand48();
            Vector3 center = {a+0.9+drand48(), 0.2, b*0.9*drand48()};
            if (Vector3Length(Vector3Subtract(center, (Vector3){4, 0.2, 0})) > 0.9) {
                if (mat < 0.8) {
                    append(hitables, ((Hitable){
                        .type = SPHERE,
                        .data = {
                            center.x, center.y, center.z,     // center
                            0.2,          // radius
                            LAMBERTIAN,   // mat.type
                            drand48()*drand48(), drand48()*drand48(), drand48()*drand48()
                        }
                    }));
                } else if (mat < 0.95) {
                    append(hitables, ((Hitable){
                        .type = SPHERE,
                        .data = {
                            center.x, center.y, center.z,     // center
                            0.2,          // radius
                            METAL,   // mat.type
                            drand48()*drand48(), drand48()*drand48(), drand48()*drand48(),
                            0.5*drand48()
                        }
                    }));
                } else {
                    append(hitables, ((Hitable){
                        .type = SPHERE,
                        .data = {
                            center.x, center.y, center.z,     // center
                            0.2,          // radius
                            DIELECTRIC,   // mat.type
                            drand48()*drand48(), drand48()*drand48(), drand48()*drand48(),
                            1.5
                        }
                    }));
                }
            }
        }
    }
    return hitables;
}

char *assemble_compute_shader(void) {
    int compute_code_length = 14;
    char *compute_code = calloc(compute_code_length, sizeof(char));
    sprintf(compute_code, "%s", "#version 430\n");

    char *compute_includes[] = {
        "compute_shaders/ray.glsl",
        "compute_shaders/vec3.glsl", 
        "compute_shaders/hitable.glsl", 
        "compute_shaders/sphere.glsl", 
        "compute_shaders/hitablelist.glsl", 
        "compute_shaders/camera.glsl", 
        "compute_shaders/material.glsl", 
        "compute_shaders/compute.glsl"
    };
    for (int i = 0; i < (sizeof(compute_includes) / sizeof(char*)); i++) {
        char *s = LoadFileText(compute_includes[i]);
        int len = strlen(s);
        compute_code = realloc(compute_code, compute_code_length + len);
        strcat(compute_code, s);
        free(s);
        compute_code_length += len;
    }
    return compute_code;
}

int main(void) {
    const int X = 1024;
    const int Y = X/2;

    const Vector2 resolution = { (float)X, (float)Y };

    InitWindow(X*SCALE, Y*SCALE, "");
    SetExitKey(KEY_Q);
    
    char *compute_code = assemble_compute_shader();
    //printf("%s\n", compute_code);

    unsigned int compute_shader = rlCompileShader(compute_code, RL_COMPUTE_SHADER);
    unsigned int compute_program = rlLoadComputeShaderProgram(compute_shader);
    
    Hitables hitables = setup_world();

    rlEnableShader(compute_program);
    rlSetUniform(rlGetLocationUniform(compute_program, "X"), &X, RL_SHADER_UNIFORM_INT, 1);
    rlSetUniform(rlGetLocationUniform(compute_program, "Y"), &Y, RL_SHADER_UNIFORM_INT, 1);
    rlSetUniform(rlGetLocationUniform(compute_program, "num_hitables"), &hitables.index, RL_SHADER_UNIFORM_INT, 1);
    rlDisableShader();

    UnloadFileText(compute_code);

    Shader frag_shader = LoadShader(NULL, "compute_shaders/frag.glsl");
    SetShaderValue(frag_shader, GetShaderLocation(frag_shader, "resolution"), &resolution, SHADER_UNIFORM_VEC2);
    SetShaderValue(frag_shader, GetShaderLocation(frag_shader, "X"), &X, SHADER_UNIFORM_INT);

    unsigned int screen = rlLoadShaderBuffer(X*Y*sizeof(Vector4), NULL, RL_DYNAMIC_COPY);
    unsigned int world = rlLoadShaderBuffer(hitables.index*sizeof(Hitable), hitables.data, RL_DYNAMIC_COPY);

    Image img = GenImageColor(X, Y, WHITE);
    Texture tex = LoadTextureFromImage(img);
    UnloadImage(img);

    int frame = 1;

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_O)) {
            printf("outputting\n");
            Vector4 *buf = calloc(X*Y, sizeof(Vector4));
            rlReadShaderBuffer(screen, buf, X*Y*sizeof(Vector4), 0);
            FILE *f = fopen("compute_shaders/out.ppm", "w");

            fprintf(f, "P3\n");
            fprintf(f, "%d %d\n", X, Y);
            fprintf(f, "255\n");

            for (int i = 0; i < X*Y; i++) {
                Vector4 col = buf[i];
                fprintf(f, "%d %d %d\n", (int)(255.99*col.x/col.w), (int)(255.99*col.y/col.w), (int)(255.99*col.z/col.w));
            }
            free(buf);
            printf("output done\n");
        }

        frame++;

        rlEnableShader(compute_program);
        rlBindShaderBuffer(screen, 1);
        rlBindShaderBuffer(world, 2);
        rlSetUniform(rlGetLocationUniform(compute_program, "rand_seed"), &frame, RL_SHADER_UNIFORM_INT, 1);
        rlComputeShaderDispatch(X/16, Y/16, 1);
        rlDisableShader();

        BeginDrawing();
        BeginShaderMode(frag_shader);
        DrawTexture(tex, 0, 0, WHITE);
        DrawTexturePro(tex,
            (Rectangle){0, 0, tex.width, tex.height},
            (Rectangle){0, 0, SCALE*tex.width, SCALE*tex.height},
            (Vector2){0, 0},
            0.0f,
            WHITE
        );
        EndShaderMode();

        DrawFPS(GetScreenWidth() - 100, 10);

        printf("%f\n", GetFrameTime() * 1000);

        EndDrawing();
    }

    rlUnloadShaderBuffer(screen);
    rlUnloadShaderBuffer(world);

    rlUnloadShaderProgram(compute_program);

    UnloadTexture(tex);
    UnloadShader(frag_shader);

    CloseWindow();

    return 0;
}
