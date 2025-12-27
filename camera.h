#pragma once

typedef struct Camera {
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
} Camera;

Camera cam = {
    {0.0, 0.0, 0.0},
    {-2.0, -1.0, -1.0},
    {4.0, 0.0, 0.0},
    {0.0, 2.0, 0.0}
};

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
