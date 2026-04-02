layout(std430, binding = 2) restrict buffer Hitables {
    Hitable hitables[];
};

layout(std430, binding = 3) restrict buffer Meshes {
    MeshInfo meshes[];
};

uniform int num_hitables;
uniform int num_meshes;

bool ray_bounding_box(Ray r, vec3 box_min, vec3 box_max) {
    vec3 inv_dir = 1 / r.direction;
    vec3 t_min = (box_min - r.origin) * inv_dir;
    vec3 t_max = (box_max - r.origin) * inv_dir;
    vec3 t1 = min(t_min, t_max);
    vec3 t2 = max(t_min, t_max);
    float t_near = max(max(t1.x, t1.y), t1.z);
    float t_far = min(min(t2.x, t2.y), t2.z);
    return t_near <= t_far;
};


bool hit(Ray r, float t_min, float t_max, inout hit_record rec) {
    hit_record temp_rec = hit_record(0, vec3(0), vec3(0), MaterialData(LAMBERTIAN, vec3(0), 0, vec3(0), 0));
    bool hit_anything = false;
    float closest_so_far = t_max;

    for (int i = 0; i < num_meshes; i++) {
        MeshInfo mesh = meshes[i];
        if (!ray_bounding_box(r, mesh.bounds_min, mesh.bounds_max)) continue;
        for (int j = 0; j < mesh.num_triangles; j++) {
            Triangle t = hitable_triangle(hitables[j + mesh.first_triangle_index].data);
            bool result = triangle_hit(t, r, t_min, closest_so_far, temp_rec);
            if (result) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
    }
    return hit_anything;
}
