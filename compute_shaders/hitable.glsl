#define LAMBERTIAN 1
#define METAL 2

struct Material {
    uint type;
    vec3 albedo;
};

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    Material mat;
};
