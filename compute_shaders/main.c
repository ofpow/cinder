#include <raylib.h>
#include <rlgl.h>

#include <stdlib.h>

#define SCALE (2048 / X)

typedef struct vec4 {
    float x, y, z, w;
} vec4;

int main(void)
{
    const int X = 1024;
    const int Y = X/2;

    const Vector2 resolution = { (float)X, (float)Y };

    InitWindow(X*SCALE, Y*SCALE, "");
    SetExitKey(KEY_Q);

    char *compute_code = LoadFileText("compute_shaders/compute.glsl");
    unsigned int compute_shader = rlCompileShader(compute_code, RL_COMPUTE_SHADER);
    unsigned int compute_program = rlLoadComputeShaderProgram(compute_shader);

    rlEnableShader(compute_program);
    rlSetUniform(rlGetLocationUniform(compute_program, "X"), &X, RL_SHADER_UNIFORM_INT, 1);
    rlSetUniform(rlGetLocationUniform(compute_program, "Y"), &Y, RL_SHADER_UNIFORM_INT, 1);
    rlDisableShader();

    UnloadFileText(compute_code);

    Shader frag_shader = LoadShader(NULL, "compute_shaders/frag.glsl");
    SetShaderValue(frag_shader, GetShaderLocation(frag_shader, "resolution"), &resolution, SHADER_UNIFORM_VEC2);
    SetShaderValue(frag_shader, GetShaderLocation(frag_shader, "X"), &X, SHADER_UNIFORM_INT);

    unsigned int ssbo = rlLoadShaderBuffer(X*Y*sizeof(vec4), NULL, RL_DYNAMIC_COPY);

    Image img = GenImageColor(X, Y, WHITE);
    Texture tex = LoadTextureFromImage(img);
    UnloadImage(img);

    while (!WindowShouldClose())
    {
        rlEnableShader(compute_program);
        rlBindShaderBuffer(ssbo, 1);
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

        EndDrawing();
    }

    rlUnloadShaderBuffer(ssbo);

    rlUnloadShaderProgram(compute_program);

    UnloadTexture(tex);
    UnloadShader(frag_shader);

    CloseWindow();

    return 0;
}
