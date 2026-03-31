vec3 unit_vector(vec3 v) {
    return v / length(v);
}
uint rand_state = 0;

float rand() {
    rand_state = 1664525u * rand_state + 1013904223u;
    return float(rand_state) / 4294967296.0;
}

vec3 random_in_unit_sphere() {
    vec3 p = vec3(0);
    do {
        p = 2.0*vec3(rand(), rand(), rand()) - vec3(1);
    } while ((p.x*p.x + p.y*p.y + p.z*p.z) >= 1.0);
    return p;
}
