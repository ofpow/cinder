bool scatter_lambertian(vec3 albedo, inout Ray r_in, inout hit_record rec, inout vec3 attenuation, inout Ray scattered) {
    vec3 target = rec.p + rec.normal + random_in_unit_sphere();
    scattered = Ray(rec.p, target-rec.p);
    attenuation = albedo;
    return true;
}

bool scatter_metal(vec3 albedo, inout Ray r_in, inout hit_record rec, inout vec3 attenuation, inout Ray scattered, float fuzz) {
    vec3 reflected = reflect(unit_vector(r_in.direction), normalize(rec.normal));
    scattered = Ray(rec.p, reflected + fuzz*random_in_unit_sphere());
    attenuation = albedo;
    return (dot(scattered.direction, rec.normal) > 0);
}

bool scatter(Material mat, inout Ray r, inout hit_record rec, inout vec3 attenuation, inout Ray scattered) {
    switch (mat.type) {
        case LAMBERTIAN: return scatter_lambertian(mat.albedo, r, rec, attenuation, scattered);
        case METAL: return scatter_metal(mat.albedo, r, rec, attenuation, scattered, mat.fuzz);
    }
}
