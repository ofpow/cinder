bool triangle_hit(Triangle t, Ray r, float t_min, float t_max, inout hit_record rec) {
    vec3 edge1 = t.b - t.a;
    vec3 edge2 = t.c - t.a;

    vec3 pvec = cross(r.direction, edge2);
    float det = dot(edge1, pvec);

    float invdet = 1.0 / det;

    vec3 tvec = r.origin - t.a;

    float u = dot(tvec, pvec) * invdet;
    if (u < 0.0 || u > 1.0) {
        return false;
    }

    vec3 qvec = cross(tvec, edge1);

    float v = dot(r.direction, qvec) * invdet;
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }

    float t_hit = dot(edge2, qvec) * invdet;

    // Check if t is within valid range
    if (t_hit < t_min || t_hit > t_max) {
        return false;
    }

    // Record hit information
    rec.t = t_hit;
    rec.p = point_at_parameter(r, t_hit);
    rec.normal = normalize(cross(edge1, edge2));
    rec.mat = t.mat;

    return true;
}

Triangle hitable_triangle(float data[18]) {
    return Triangle(
            vec3(data[0], data[1], data[2]),
            vec3(data[3], data[4], data[5]),
            vec3(data[6], data[7], data[8]),
            MaterialData(
                uint(data[9]),
                vec3(data[10], data[11], data[12]), 
                data[13],
                vec3(data[14], data[15], data[16]),
                data[17]
            )
        );
}
