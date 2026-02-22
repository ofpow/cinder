struct Sphere {
    vec3 center;
    float radius;
};

bool sphere_hit(Sphere s, Ray r, float t_min, float t_max, inout hit_record rec) {
    vec3 oc = r.origin - s.center;

    float a = dot(r.direction, r.direction);
    float b = dot(oc, r.direction);
    float c = dot(oc, oc) - s.radius*s.radius;
    float discriminant = b*b - a*c;

    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = point_at_parameter(r, temp);
            rec.normal = (rec.p - s.center) / s.radius;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = point_at_parameter(r, temp);
            rec.normal = (rec.p - s.center) / s.radius;
            return true;
        }
    }

    return false;
}
