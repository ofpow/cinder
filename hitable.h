#pragma once

typedef enum {
    Lambertian,
    Metal,
    Dielectric,
} Material_Type;

typedef struct Material {
    Material_Type type;
    vec3 albedo;
    float data; // fuzz for metal, refraction index for dielectric
} Material;

typedef struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    Material mat;
} hit_record;

