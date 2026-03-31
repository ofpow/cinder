#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#define RAYLIB_NUKLEAR_IMPLEMENTATION
#include "../external/raylib-nuklear.h"

#define FAST_OBJ_IMPLEMENTATION
#include "../external/fast_obj.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "shared/definitions.h"

#include "array.h"

unsigned int screen_ssbo;
unsigned int world_ssbo;

unsigned int compute_shader;
unsigned int compute_program;

#define SCALE (3072 / X)
const int X = 768;
const int Y = X/2;

define_array(Hitables, Hitable);

#include "gui.h"

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

void load_obj(char *obj_file, Hitables *hitables, Vector3 offset) {
    fastObjMesh *m = fast_obj_read(obj_file);
    if (!m) { printf("Couldn't read obj file `%s`\n", obj_file); exit(1); }
    
    uint index_offset = 0;
    for (int i = 0; i < m->face_count; i++) {
        uint num_vertices = m->face_vertices[i];
        assert(("Require models to be triangulated", m->face_vertices[i] == 3));
        fastObjIndex idx1 = m->indices[index_offset + 0];
        fastObjIndex idx2 = m->indices[index_offset + 1];
        fastObjIndex idx3 = m->indices[index_offset + 2];
    
        add_triangle(hitables,
                    Vector3Add(offset, ((Vector3*)m->positions)[idx1.p]),
                    Vector3Add(offset, ((Vector3*)m->positions)[idx2.p]),
                    Vector3Add(offset, ((Vector3*)m->positions)[idx3.p]),
                    (MaterialData){
                        LAMBERTIAN,   
                        {0.5, 0.5, 0.5},
                        0,            
                        {0, 0, 0},
                        0
                    }
        );

        index_offset += 3;
    }

    fast_obj_destroy(m);
}

Hitables setup_world(void) {
    Hitables hitables = (Hitables){
        calloc(10, sizeof(Hitable)),
        0,
        10
    };

    for (int i = -2; i < 3; i++) {
        load_obj("assets/suzanne.obj", &hitables, (Vector3){-i*2.5, 0, -1});
    }

    return hitables;
}

const char compute_code[] = {
#embed "shaders/glsl.glsl"
    ,
#embed "shared/definitions.h"
    ,
#embed "shaders/ray.glsl"
    ,
#embed "shaders/vec3.glsl" 
    ,
#embed "shaders/sphere.glsl" 
    ,
#embed "shaders/triangle.glsl" 
    ,
#embed "shaders/hitablelist.glsl" 
    ,
#embed "shaders/camera.glsl" 
    ,
#embed "shaders/material.glsl" 
    ,
#embed "shaders/compute.glsl"
    , 0
};

const char frag_code[] = {
#embed "shaders/frag.glsl"
    , 0
};

int main(void) {
    struct nk_context *ctx = InitNuklear(30);

    const Vector2 resolution = { (float)X, (float)Y };

    InitWindow(X*SCALE, Y*SCALE, "");
    SetExitKey(KEY_Q);
    
    compute_shader = rlCompileShader(compute_code, RL_COMPUTE_SHADER);
    compute_program = rlLoadComputeShaderProgram(compute_shader);
    
    Hitables hitables = setup_world();

    rlEnableShader(compute_program);
    rlSetUniform(rlGetLocationUniform(compute_program, "X"), &X, RL_SHADER_UNIFORM_INT, 1);
    rlSetUniform(rlGetLocationUniform(compute_program, "Y"), &Y, RL_SHADER_UNIFORM_INT, 1);
    rlSetUniform(rlGetLocationUniform(compute_program, "num_hitables"), &hitables.index, RL_SHADER_UNIFORM_INT, 1);
    rlDisableShader();

    Shader frag_shader = LoadShaderFromMemory(NULL, frag_code);
    SetShaderValue(frag_shader, GetShaderLocation(frag_shader, "resolution"), &resolution, SHADER_UNIFORM_VEC2);
    SetShaderValue(frag_shader, GetShaderLocation(frag_shader, "X"), &X, SHADER_UNIFORM_INT);

    screen_ssbo = rlLoadShaderBuffer(X*Y*sizeof(Vector4), NULL, RL_DYNAMIC_COPY);
    world_ssbo = rlLoadShaderBuffer(hitables.index*sizeof(Hitable), hitables.data, RL_DYNAMIC_COPY);

    Image img = GenImageColor(X, Y, WHITE);
    Texture tex = LoadTextureFromImage(img);
    UnloadImage(img);

    int frame = 1;
    int reset = 0;
    Vector3 lookfrom = {0, 0, 4};
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
