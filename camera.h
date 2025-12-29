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

void init_camera(float vfov, float aspect) {
    float theta = vfov * M_PI / 180; 
    float half_height = tan(theta / 2);
    float half_width = aspect * half_height;
    cam.lower_left_corner = (vec3){-half_width, -half_height, -1.0};
    cam.horizontal = (vec3){2*half_width, 0.0, 0.0};
    cam.vertical = (vec3){0.0, 2 * half_height, 0.0};
    cam.origin = (vec3){0.0, 0.0, 0.0};
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
