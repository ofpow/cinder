#define LAMBERTIAN 1
#define METAL 2
#define DIELECTRIC 3

struct Material {
    uint type;
    vec3 albedo;
    float data;
    vec3 emission_col;
    float emission_str;
};

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    Material mat;
};
