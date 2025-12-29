#pragma once

typedef struct Camera {
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
} Camera;

Camera cam = {
    .origin={0.0, 0.0, 0.0},
    .lower_left_corner={-2.0, -1.0, -1.0},
    .horizontal={4.0, 0.0, 0.0},
    .vertical={0.0, 2.0, 0.0}
};

void init_camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect) {
    vec3 u, v, w = {0};
    float theta = vfov * M_PI / 180; 
    float half_height = tan(theta / 2);
    float half_width = aspect * half_height;

    cam.origin = lookfrom;
    w = unit_vector(subtract_vec3(lookfrom, lookat));
    u = unit_vector(cross(vup, w));
    v = cross(w, u);
    
    cam.lower_left_corner = subtract_vec3(cam.origin, w);
    cam.lower_left_corner = subtract_vec3(cam.lower_left_corner, scale_vec3(u, half_width));
    cam.lower_left_corner = subtract_vec3(cam.lower_left_corner, scale_vec3(v, half_height));

    cam.horizontal = scale_vec3(u, 2*half_width);
    cam.vertical = scale_vec3(v, 2*half_height);
}

Ray get_ray(float u, float v) {
    return (Ray){
        cam.origin,
        add_vec3(
            cam.lower_left_corner,
            subtract_vec3(
                add_vec3(
                    scale_vec3(cam.horizontal, u),
                    scale_vec3(cam.vertical, v)
                ),
                cam.origin
            )
        )
    };
}
