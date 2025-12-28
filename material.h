#pragma once

bool scatter_lambertian(vec3 albedo, hit_record *rec, vec3 *attenuation, Ray *scattered) {
    vec3 target = add_vec3(
        add_vec3(
            rec->p,
            rec->normal
        ),
        random_in_unit_sphere()
    );
    *scattered = (Ray){rec->p, subtract_vec3(target, rec->p)};
    *attenuation = albedo;
    return true;
}

bool scatter_metal(vec3 albedo, Ray *r_in, hit_record *rec, vec3 *attenuation, Ray *scattered, float fuzz) {
    vec3 reflected = reflect(unit_vector(r_in->direction), rec->normal);
    *scattered = (Ray){rec->p, add_vec3(reflected, scale_vec3(random_in_unit_sphere(), fuzz))};
    *attenuation = albedo;
    return (dot(scattered->direction, rec->normal) > 0);
}

bool scatter(Material mat, Ray *r, hit_record *rec, vec3 *attenuation, Ray *scattered) {
    switch (mat.type) {
        case Lambertian: return scatter_lambertian(mat.albedo, rec, attenuation, scattered);
        case Metal: return scatter_metal(mat.albedo, r, rec, attenuation, scattered, mat.fuzz);
        default: fprintf(stderr, "cant scatter material type %d\n", mat.type); exit(1);
    }
}
