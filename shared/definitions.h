#ifndef GLSL
typedef Vector3 vec3;
typedef struct Hitable Hitable;
#endif

#define LAMBERTIAN 1
#define METAL 2
#define DIELECTRIC 3

#define SPHERE 1
#define TRIANGLE 2

#ifdef GLSL
struct Material {
    uint type;
    vec3 albedo;
    float data;
    vec3 emission_col;
    float emission_str;
};

struct Ray {
    vec3 origin;
    vec3 direction;
};
#endif

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    Material mat;
};

struct Sphere {
    vec3 center;
    float radius;
    Material mat;
};

struct Triangle {
    vec3 a, b, c;
    Material mat;
};

struct Hitable {
    uint type;
    float data[18];
};

struct Camera {
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;

    vec3 u, v, w;
    float lens_radius;
};

