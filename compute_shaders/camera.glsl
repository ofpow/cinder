struct Camera {
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
};

Ray get_ray(Camera c, float u, float v) {
    return Ray(c.origin, c.lower_left_corner + u*c.horizontal + v*c.vertical - c.origin);
}
