#pragma once

bool scatter_lambertian(vec3 albedo, hit_record *rec, vec3 *attenuation, Ray *scattered, unsigned short xsubi[3]) {
    vec3 target = add_vec3(
        add_vec3(
            rec->p,
            rec->normal
        ),
        random_in_unit_sphere(xsubi)
    );
    *scattered = (Ray){rec->p, subtract_vec3(target, rec->p)};
    *attenuation = albedo;
    return true;
}

bool scatter_metal(vec3 albedo, Ray *r_in, hit_record *rec, vec3 *attenuation, Ray *scattered, float fuzz, unsigned short xsubi[3]) {
    vec3 reflected = reflect(unit_vector(r_in->direction), rec->normal);
    *scattered = (Ray){rec->p, add_vec3(reflected, scale_vec3(random_in_unit_sphere(xsubi), fuzz))};
    *attenuation = albedo;
    return (dot(scattered->direction, rec->normal) > 0);
}

bool refract(vec3 v, vec3 n, float ni_over_nt, vec3 *refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);

    if (discriminant > 0) {
        *refracted = subtract_vec3(
            scale_vec3(subtract_vec3(uv,
                    scale_vec3(n, dt)
                ), ni_over_nt),
            scale_vec3(n, sqrtf(discriminant))
        );
        return true;
    }
    return false;
}

float schlick(float cosine, float ref_idx) {
    float r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0 * r0;
    return r0 + (1-r0)*pow((1-cosine), 5);
}

bool scatter_dielectric(Ray *r_in, hit_record *rec, vec3 *attenuation, Ray *scattered, float ref_idx, unsigned short xsubi[3]) {
    vec3 outward_normal = {0};
    vec3 reflected = reflect(r_in->direction, rec->normal);

    float ni_over_nt = 0;
    *attenuation = (vec3){1.0, 1.0, 1.0};
    vec3 refracted = {0};
    float reflect_prob = 0;
    float cosine = 0;

    if (dot(r_in->direction, rec->normal) > 0) {
        outward_normal = (vec3){-rec->normal.x, -rec->normal.y, -rec->normal.z};
        ni_over_nt = ref_idx;
        cosine = ref_idx * dot(r_in->direction, rec->normal) / vec3_length(r_in->direction);
    } else {
        outward_normal = rec->normal;
        ni_over_nt = 1.0 / ref_idx;
        cosine = -dot(r_in->direction, rec->normal) / vec3_length(r_in->direction);
    }
    
    if (refract(r_in->direction, outward_normal, ni_over_nt, &refracted)) {
        reflect_prob = schlick(cosine, ref_idx);
    } else {
        *scattered = (Ray){rec->p, reflected};
        reflect_prob = 1.0;
    }

    if (erand48(xsubi) < reflect_prob) {
        *scattered = (Ray){rec->p, reflected};
    } else {
        *scattered = (Ray){rec->p, refracted};
    }

    return true;
}

bool scatter(Material mat, Ray *r, hit_record *rec, vec3 *attenuation, Ray *scattered, unsigned short xsubi[3]) {
    switch (mat.type) {
        case Lambertian: return scatter_lambertian(mat.albedo, rec, attenuation, scattered, xsubi);
        case Metal: return scatter_metal(mat.albedo, r, rec, attenuation, scattered, mat.data, xsubi);
        case Dielectric: return scatter_dielectric(r, rec, attenuation, scattered, mat.data, xsubi);
        default: fprintf(stderr, "cant scatter material type %d\n", mat.type); exit(1);
    }
}
