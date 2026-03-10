vec3 point_at_parameter(Ray r, float t) {
    return r.origin + t*r.direction;
}
