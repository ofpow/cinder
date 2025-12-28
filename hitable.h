#pragma once

typedef enum {
    Lambertian,
    Metal,
} Material_Type;

typedef struct Material {
    Material_Type type;
    vec3 albedo;
    float fuzz;
} Material;

typedef struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    Material mat;
} hit_record;

