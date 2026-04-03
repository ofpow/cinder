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
#include <float.h>

#include "shared/definitions.h"

#include "array.h"

unsigned int screen_ssbo;
unsigned int hitables_ssbo;
unsigned int meshes_ssbo;

unsigned int compute_shader;
unsigned int compute_program;

#define SCALE (3072 / X)
const int X = 768;
const int Y = X/2;

define_array(Hitables, Hitable);
define_array(Meshes, MeshInfo);

typedef struct World {
    Hitables hitables;
    Meshes meshes;
} World;

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

MeshInfo load_obj(char *obj_file, Hitables *hitables, Vector3 offset) {
    fastObjMesh *m = fast_obj_read(obj_file);
    if (!m) { printf("Couldn't read obj file `%s`\n", obj_file); exit(1); }

    MeshInfo mesh = {0};
    mesh.first_triangle_index = hitables->index;
    mesh.bounds_min = (Vector3){FLT_MAX, FLT_MAX, FLT_MAX};
    mesh.bounds_max = (Vector3){-FLT_MAX, -FLT_MAX, -FLT_MAX};
    
    uint index_offset = 0;
    for (int i = 0; i < m->face_count; i++) {
        uint num_vertices = m->face_vertices[i];
        assert(("Require models to be triangulated", m->face_vertices[i] == 3));
        fastObjIndex idx1 = m->indices[index_offset + 0];
        fastObjIndex idx2 = m->indices[index_offset + 1];
        fastObjIndex idx3 = m->indices[index_offset + 2];

        Vector3 a = Vector3Add(offset, ((Vector3*)m->positions)[idx1.p]);
        Vector3 b = Vector3Add(offset, ((Vector3*)m->positions)[idx2.p]);
        Vector3 c = Vector3Add(offset, ((Vector3*)m->positions)[idx3.p]);

        mesh.bounds_min = Vector3Min(mesh.bounds_min, a);
        mesh.bounds_min = Vector3Min(mesh.bounds_min, b);
        mesh.bounds_min = Vector3Min(mesh.bounds_min, c);
        mesh.bounds_max = Vector3Max(mesh.bounds_max, a);
        mesh.bounds_max = Vector3Max(mesh.bounds_max, b);
        mesh.bounds_max = Vector3Max(mesh.bounds_max, c);

        uint material_index = m->face_materials[i];
        fastObjMaterial *mat = &m->materials[material_index];

        Vector3 albedo = {0};
        if (material_index < m->material_count) {
            albedo = (Vector3){mat->Kd[0], mat->Kd[1], mat->Kd[2]};
        } else {
            albedo = (Vector3){0.5, 0.5, 0.5};
        }

        Vector3 emission = {0};
        if (material_index < m->material_count) {
            emission = (Vector3){mat->Ke[0], mat->Ke[1], mat->Ke[2]};
        } else {
            emission = (Vector3){0};
        }
        float emission_strength = fmaxf(fmaxf(emission.x, emission.y), emission.z);

        add_triangle(
            hitables,
            a, b, c,
            (MaterialData){
                LAMBERTIAN,   
                albedo,
                0,            
                emission,
                emission_strength
            }
        );

        index_offset += 3;
        mesh.num_triangles++;
    }

    fast_obj_destroy(m);

    return mesh;
}

World setup_world(void) {
    Hitables hitables = (Hitables){
        calloc(10, sizeof(Hitable)),
        0,
        10
    };

    Meshes meshes = (Meshes){
        calloc(10, sizeof(MeshInfo)),
        0,
        10
    };

    append(meshes, load_obj("assets/cornell-box.obj", &hitables, (Vector3){0}));

    return (World){hitables, meshes};
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
    
    World world = setup_world();

    rlEnableShader(compute_program);
    rlSetUniform(rlGetLocationUniform(compute_program, "X"), &X, RL_SHADER_UNIFORM_INT, 1);
    rlSetUniform(rlGetLocationUniform(compute_program, "Y"), &Y, RL_SHADER_UNIFORM_INT, 1);
    rlSetUniform(rlGetLocationUniform(compute_program, "num_hitables"), &world.hitables.index, RL_SHADER_UNIFORM_INT, 1);
    rlSetUniform(rlGetLocationUniform(compute_program, "num_meshes"), &world.meshes.index, RL_SHADER_UNIFORM_INT, 1);
    rlDisableShader();

    Shader frag_shader = LoadShaderFromMemory(NULL, frag_code);
    SetShaderValue(frag_shader, GetShaderLocation(frag_shader, "resolution"), &resolution, SHADER_UNIFORM_VEC2);
    SetShaderValue(frag_shader, GetShaderLocation(frag_shader, "X"), &X, SHADER_UNIFORM_INT);

    screen_ssbo = rlLoadShaderBuffer(X*Y*sizeof(Vector4), NULL, RL_DYNAMIC_COPY);
    hitables_ssbo = rlLoadShaderBuffer(world.hitables.index*sizeof(Hitable), world.hitables.data, RL_DYNAMIC_COPY);
    meshes_ssbo = rlLoadShaderBuffer(world.meshes.index*sizeof(MeshInfo), world.meshes.data, RL_DYNAMIC_COPY);

    Image img = GenImageColor(X, Y, WHITE);
    Texture tex = LoadTextureFromImage(img);
    UnloadImage(img);

    int frame = 1;
    int reset = 0;
    Vector3 lookfrom = {0, 1, 4};
    Vector3 lookat = {0, 0, -1};
    Vector3 vup = {0, 1, 0};
    float aperture = 0.0;
    int vfov = 90;
    int selected_index = 0;

    bool show_gui = false;

    while (!WindowShouldClose()) {
        frame++;

        handle_output();

        rlEnableShader(compute_program);
        if (reset == 1) {
            hitables_ssbo = rlLoadShaderBuffer(world.hitables.index*sizeof(Hitable), world.hitables.data, RL_DYNAMIC_COPY);
            meshes_ssbo = rlLoadShaderBuffer(world.meshes.index*sizeof(MeshInfo), world.meshes.data, RL_DYNAMIC_COPY);
        }
        rlBindShaderBuffer(screen_ssbo, 1);
        rlBindShaderBuffer(hitables_ssbo, 2);
        rlBindShaderBuffer(meshes_ssbo, 3);
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
            reset |= object_editor(ctx, &selected_index, world.hitables);

            DrawNuklear(ctx);
        }
    
        EndDrawing();
    }

    rlUnloadShaderBuffer(screen_ssbo);
    rlUnloadShaderBuffer(hitables_ssbo);
    rlUnloadShaderBuffer(meshes_ssbo);

    rlUnloadShaderProgram(compute_program);

    UnloadTexture(tex);
    UnloadShader(frag_shader);  
    UnloadNuklear(ctx);

    CloseWindow();

    return 0;
}
