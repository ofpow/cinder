bool scatter_lambertian(vec3 albedo, hit_record rec, inout vec3 attenuation, inout Ray scattered) {
    vec3 target = rec.p + rec.normal + random_in_unit_sphere();
    scattered = Ray(rec.p, target-rec.p);
    attenuation = albedo;
    return true;
}

bool scatter_metal(vec3 albedo, Ray r_in, hit_record rec, inout vec3 attenuation, inout Ray scattered, float fuzz) {
    vec3 reflected = reflect(unit_vector(r_in.direction), normalize(rec.normal));
    scattered = Ray(rec.p, reflected + fuzz*random_in_unit_sphere());
    attenuation = albedo;
    return (dot(scattered.direction, rec.normal) > 0);
}

bool refract(vec3 v, vec3 n, float ni_over_nt, inout vec3 refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);

    if (discriminant > 0) {
        refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
        return true;
    }
    return false;
}

float schlick(float cosine, float ref_idx) {
    float r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0 * r0;
    return r0 + (1-r0)*pow((1-cosine), 5);
}

bool scatter_dielectric(Ray r_in, hit_record rec, inout vec3 attenuation, inout Ray scattered, float ref_idx) {
    vec3 outward_normal = vec3(0);
    vec3 reflected = reflect(r_in.direction, rec.normal);

    float ni_over_nt = 0;
    attenuation = vec3(1);
    vec3 refracted = vec3(0);;
    float reflect_prob = 0;
    float cosine = 0;

    if (dot(r_in.direction, rec.normal) > 0) {
        outward_normal = -rec.normal;
        ni_over_nt = ref_idx;
        cosine = ref_idx * dot(r_in.direction, rec.normal) / length(r_in.direction);
    } else {
        outward_normal = rec.normal;
        ni_over_nt = 1.0 / ref_idx;
        cosine = -dot(r_in.direction, rec.normal) / length(r_in.direction);
    }
    
    if (refract(r_in.direction, outward_normal, ni_over_nt, refracted)) {
        reflect_prob = schlick(cosine, ref_idx);
    } else {
        scattered = Ray(rec.p, reflected);
        reflect_prob = 1.0;
    }

    if (rand() < reflect_prob) {
        scattered = Ray(rec.p, reflected);
    } else {
        scattered = Ray(rec.p, refracted);
    }

    return true;
}

bool scatter(MaterialData mat, inout Ray r, inout hit_record rec, inout vec3 attenuation, inout Ray scattered) {
    switch (mat.type) {
        case LAMBERTIAN: return scatter_lambertian(mat.albedo, rec, attenuation, scattered);
        case METAL: return scatter_metal(mat.albedo, r, rec, attenuation, scattered, mat.data);
        case DIELECTRIC: return scatter_dielectric(r, rec, attenuation, scattered, mat.data);
    }
}
