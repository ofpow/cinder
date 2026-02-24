#include <raylib.h>
#include <rlgl.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

typedef struct vec4 {
    float x, y, z, w;
} vec4;

#define SPHERE 1

typedef struct Hitable {
    unsigned int type;
    float data[4];
} Hitable;

define_array(Hitables, Hitable);

int main(void) {
    const int X = 1024;
    const int Y = X/2;

    const Vector2 resolution = { (float)X, (float)Y };

    InitWindow(X*SCALE, Y*SCALE, "");
    SetExitKey(KEY_Q);

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
    //printf("%s\n", compute_code);

    unsigned int compute_shader = rlCompileShader(compute_code, RL_COMPUTE_SHADER);
    unsigned int compute_program = rlLoadComputeShaderProgram(compute_shader);
    
    Hitables hitables = (Hitables){
        calloc(10, sizeof(Hitable)),
        0,
        10
    };

    append(hitables, ((Hitable){
        .type = SPHERE,
        .data = {0, 0, -1, 0.5}
    }));
    append(hitables, ((Hitable){
        .type = SPHERE,
        .data = {0, 100.5, -1, 100}
    }));

    rlEnableShader(compute_program);
    rlSetUniform(rlGetLocationUniform(compute_program, "X"), &X, RL_SHADER_UNIFORM_INT, 1);
    rlSetUniform(rlGetLocationUniform(compute_program, "Y"), &Y, RL_SHADER_UNIFORM_INT, 1);
    rlSetUniform(rlGetLocationUniform(compute_program, "num_hitables"), &hitables.index, RL_SHADER_UNIFORM_INT, 1);
    rlDisableShader();

    UnloadFileText(compute_code);

    Shader frag_shader = LoadShader(NULL, "compute_shaders/frag.glsl");
    SetShaderValue(frag_shader, GetShaderLocation(frag_shader, "resolution"), &resolution, SHADER_UNIFORM_VEC2);
    SetShaderValue(frag_shader, GetShaderLocation(frag_shader, "X"), &X, SHADER_UNIFORM_INT);

    unsigned int screen = rlLoadShaderBuffer(X*Y*sizeof(vec4), NULL, RL_DYNAMIC_COPY);

    unsigned int world = rlLoadShaderBuffer(hitables.index*sizeof(Hitable), hitables.data, RL_DYNAMIC_COPY);

    Image img = GenImageColor(X, Y, WHITE);
    Texture tex = LoadTextureFromImage(img);
    UnloadImage(img);

    rlEnableShader(compute_program);
    rlBindShaderBuffer(screen, 1);
    rlBindShaderBuffer(world, 2);

    double start = GetTime();

    rlComputeShaderDispatch(X/16, Y/16, 1);
    vec4 *buf = calloc(X*Y, sizeof(vec4));
    while (buf[0].w == 0.0) {
        rlReadShaderBuffer(screen, buf, X*Y*sizeof(vec4), 0);
    }
    free(buf);
    printf("render time: %.3f ms\n", (GetTime() - start) * 1000.0);
    rlDisableShader();

    while (!WindowShouldClose()) {

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
