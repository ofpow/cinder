#define M_PI 3.14159265358979323846

vec3 random_in_unit_disk() {
    vec3 p;
    do {
        p = 2.0*vec3(rand(), rand(), 0) - vec3(1, 1, 0);
    } while (dot(p, p) >= 1.0);
    return p;
}

Ray get_ray(Camera c, float u, float v) {
    vec3 rd = c.lens_radius * random_in_unit_disk();
    vec3 offst = c.u * rd.x + v * rd.y;
    return Ray(c.origin + offst, c.lower_left_corner + u*c.horizontal + v*c.vertical - c.origin - offst);
}

Camera c;

void init_camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist) {
    lookfrom.y = -lookfrom.y;
    lookat.y = -lookat.y;

    c.lens_radius = aperture / 2;

    float theta = vfov * M_PI / 180; 
    float half_height = tan(theta / 2);
    float half_width = aspect * half_height;
    c.origin = lookfrom;
    c.w = unit_vector(lookfrom - lookat);
    c.u = unit_vector(cross(vup, c.w));
    c.v = cross(c.w, c.u);
    
    c.lower_left_corner = c.origin - half_width*focus_dist*c.u - half_height*focus_dist*c.v - focus_dist*c.w;
    c.horizontal = 2*half_width*focus_dist*c.u;
    c.vertical = 2*half_height*focus_dist*c.v;
}

