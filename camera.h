#pragma once

vec3 random_in_unit_disk(unsigned short xsubi[3]) {
    vec3 p = {0};

    do {
        p = subtract_vec3(
            scale_vec3((vec3){erand48(xsubi), erand48(xsubi), 0}, 2.0),
            (vec3){1, 1, 0}
        );
    } while (dot(p, p) >= 1.0);
    return p;
}

typedef struct Camera {
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    
    vec3 u, v, w;
    float lens_radius;
} Camera;

Camera cam = {
    .origin={0.0, 0.0, 0.0},
    .lower_left_corner={-2.0, -1.0, -1.0},
    .horizontal={4.0, 0.0, 0.0},
    .vertical={0.0, 2.0, 0.0},
    .u={0,0,0},
    .v={0,0,0},
    .w={0,0,0},
    .lens_radius=1
};

void init_camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist) {
    cam.lens_radius = aperture / 2;
    float theta = vfov * M_PI / 180; 
    float half_height = tan(theta / 2);
    float half_width = aspect * half_height;

    cam.origin = lookfrom;
    cam.w = unit_vector(subtract_vec3(lookfrom, lookat));
    cam.u = unit_vector(cross(vup, cam.w));
    cam.v = cross(cam.w, cam.u);
    
    cam.lower_left_corner = subtract_vec3(cam.origin, scale_vec3(cam.w, focus_dist));
    cam.lower_left_corner = subtract_vec3(cam.lower_left_corner, scale_vec3(cam.u, half_width*focus_dist));
    cam.lower_left_corner = subtract_vec3(cam.lower_left_corner, scale_vec3(cam.v, half_height*focus_dist));

    cam.horizontal = scale_vec3(cam.u, 2*half_width*focus_dist);
    cam.vertical = scale_vec3(cam.v, 2*half_height*focus_dist);
}

Ray get_ray(float s, float t, unsigned short xsubi[3]) {
    vec3 rd = scale_vec3(random_in_unit_disk(xsubi), cam.lens_radius);
    vec3 offset = add_vec3(
        scale_vec3(cam.u, rd.x),
        scale_vec3(cam.v, rd.y)
    );
    return (Ray){
        add_vec3(cam.origin, offset),
        subtract_vec3(
            subtract_vec3(
                add_vec3(
                    cam.lower_left_corner,
                    add_vec3(
                        scale_vec3(cam.horizontal, s),
                        scale_vec3(cam.vertical, t)
                    )
                ),
                cam.origin
            ),
            offset
        )
    };
}
