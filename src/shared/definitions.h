#ifndef GLSL
typedef Vector3 vec3;
typedef struct Hitable Hitable;
typedef struct MaterialData MaterialData;
typedef struct Sphere Sphere;
typedef struct Triangle Triangle;
typedef struct MeshInfo MeshInfo;
#endif

#define LAMBERTIAN 0
#define METAL 1
#define DIELECTRIC 2

#define SPHERE 1
#define TRIANGLE 2

struct MaterialData {
    float type;
    vec3 albedo;
    float data;
    vec3 emission_col;
    float emission_str;
};

#ifdef GLSL
struct Ray {
    vec3 origin;
    vec3 direction;
};
#endif

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    MaterialData mat;
};

struct Sphere {
    vec3 center;
    float radius;
    MaterialData mat;
};

struct Triangle {
    vec3 a, b, c;
    MaterialData mat;
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

struct MeshInfo {
    vec3 bounds_min;
    uint first_triangle_index;
    vec3 bounds_max;
    uint num_triangles;
};
