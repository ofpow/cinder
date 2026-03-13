#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#define RAYLIB_NUKLEAR_IMPLEMENTATION
#include "raylib-nuklear.h"

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

unsigned int screen_ssbo;
unsigned int world_ssbo;

unsigned int compute_shader;
unsigned int compute_program;

#define SCALE (3072 / X)
const int X = 768;
const int Y = X/2;

#include "shared/definitions.h"

define_array(Hitables, Hitable);

#include "gui.h"

void handle_output(void) {
    if (IsKeyPressed(KEY_O)) {
        printf("outputting\n");
        Vector4 *buf = calloc(X*Y, sizeof(Vector4));
        rlReadShaderBuffer(screen_ssbo, buf, X*Y*sizeof(Vector4), 0);
        FILE *f = fopen("out.ppm", "w");

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
}

void add_sphere(Hitables *hitables,
        Vector3 center,
        float radius,
        MaterialData mat
) {
    Sphere s = {
        center,
        radius,
        mat
    };
    Hitable h = {0};
    h.type = SPHERE;
    memcpy(h.data, &s, sizeof(Sphere));
    append((*(hitables)), h);
}
void add_triangle(Hitables *hitables,
        Vector3 a, Vector3 b, Vector3 c,
        MaterialData mat
) {
    Triangle t = {a, b, c, mat};
    Hitable h = {0};
    h.type = TRIANGLE;
    memcpy(h.data, &t, sizeof(Triangle));
    append((*(hitables)), h);
}

Hitables setup_world(void) {
    Hitables hitables = (Hitables){
        calloc(10, sizeof(Hitable)),
        0,
        10
    };
    add_sphere(&hitables,
        (Vector3){2, 0, -1},
        0.5,
        (MaterialData){
            LAMBERTIAN,   
            {0, 1, 0},
            0,            
            {0, 0, 0},
            0
        }
    );
    add_triangle(&hitables,
        (Vector3){0, 0, -1},
        (Vector3){0, 1, -1},
        (Vector3){1, 1, -1},
        (MaterialData){
            LAMBERTIAN,   
            {0, 0, 1},
            0,            
            {0, 0, 0},
            0
        }
    );
    add_triangle(&hitables,
        (Vector3){1, 0, -1},
        (Vector3){0, 0, -1},
        (Vector3){1, 1, -1},
        (MaterialData){
            LAMBERTIAN,   
            {1, 0, 0},
            0,            
            {0, 0, 0},
            0
        }
    );
    return hitables;
}

char *assemble_compute_shader(void) {
    int compute_code_length = 14;
    char *compute_code = calloc(compute_code_length, sizeof(char));
    sprintf(compute_code, "%s", "#version 430\n");

    char *compute_includes[] = {
        "compute_shaders/glsl.glsl",
        "shared/definitions.h",
        "compute_shaders/ray.glsl",
        "compute_shaders/vec3.glsl", 
        "compute_shaders/sphere.glsl", 
        "compute_shaders/triangle.glsl", 
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
    struct nk_context *ctx = InitNuklear(30);

    const Vector2 resolution = { (float)X, (float)Y };

    InitWindow(X*SCALE, Y*SCALE, "");
    SetExitKey(KEY_Q);
    
    char *compute_code = assemble_compute_shader();
    //printf("%s\n", compute_code);

    compute_shader = rlCompileShader(compute_code, RL_COMPUTE_SHADER);
    compute_program = rlLoadComputeShaderProgram(compute_shader);
    
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

    screen_ssbo = rlLoadShaderBuffer(X*Y*sizeof(Vector4), NULL, RL_DYNAMIC_COPY);
    world_ssbo = rlLoadShaderBuffer(hitables.index*sizeof(Hitable), hitables.data, RL_DYNAMIC_COPY);

    Image img = GenImageColor(X, Y, WHITE);
    Texture tex = LoadTextureFromImage(img);
    UnloadImage(img);

    int frame = 1;
    int reset = 0;
    Vector3 lookfrom = {0, 0, 2};
    Vector3 lookat = {0, 0, -1};
    Vector3 vup = {0, 1, 0};
    float aperture = 0.0;
    int vfov = 90;
    int selected_index = 0;

    bool show_gui = true;

    while (!WindowShouldClose()) {
        frame++;

        handle_output();

        rlEnableShader(compute_program);
        if (reset == 1) {
            world_ssbo = rlLoadShaderBuffer(hitables.index*sizeof(Hitable), hitables.data, RL_DYNAMIC_COPY);
        }
        rlBindShaderBuffer(screen_ssbo, 1);
        rlBindShaderBuffer(world_ssbo, 2);
        rlSetUniform(rlGetLocationUniform(compute_program, "reset"), &reset, RL_SHADER_UNIFORM_INT, 1);
        rlSetUniform(rlGetLocationUniform(compute_program, "rand_seed"), &frame, RL_SHADER_UNIFORM_INT, 1);
        rlSetUniform(rlGetLocationUniform(compute_program, "lookfrom"), &lookfrom, RL_SHADER_UNIFORM_VEC3, 1);
        rlSetUniform(rlGetLocationUniform(compute_program, "lookat"), &lookat, RL_SHADER_UNIFORM_VEC3, 1);
        rlSetUniform(rlGetLocationUniform(compute_program, "vup"), &vup, RL_SHADER_UNIFORM_VEC3, 1);
        rlSetUniform(rlGetLocationUniform(compute_program, "aperture"), &aperture, RL_SHADER_UNIFORM_FLOAT, 1);
        rlSetUniform(rlGetLocationUniform(compute_program, "vfov"), &vfov, RL_SHADER_UNIFORM_INT, 1);
        rlComputeShaderDispatch(X/16, Y/16, 1);
        rlDisableShader();

        if (reset == 1) reset = 0;

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

        if (IsKeyPressed(KEY_G)) show_gui = !show_gui;
        if (show_gui) {
            UpdateNuklear(ctx);
            reset |= camera_gui(ctx, &lookfrom, &lookat, &vup, &aperture, &vfov);
            reset |= object_editor(ctx, &selected_index, hitables);

            DrawNuklear(ctx);
        }
    
        EndDrawing();
    }

    rlUnloadShaderBuffer(screen_ssbo);
    rlUnloadShaderBuffer(world_ssbo);

    rlUnloadShaderProgram(compute_program);

    UnloadTexture(tex);
    UnloadShader(frag_shader);  
    UnloadNuklear(ctx);

    CloseWindow();

    return 0;
}
