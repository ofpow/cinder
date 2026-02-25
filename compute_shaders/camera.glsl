#define M_PI 3.14159265358979323846

struct Camera {
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
};

Ray get_ray(Camera c, float u, float v) {
    return Ray(c.origin, c.lower_left_corner + u*c.horizontal + v*c.vertical - c.origin);
}

Camera c;

void init_camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect) {
    lookfrom.y = -lookfrom.y;
    vec3 u, v, w = vec3(0);

    float theta = vfov * M_PI / 180; 
    float half_height = tan(theta / 2);
    float half_width = aspect * half_height;
    c.origin = lookfrom;
    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);
    
    c.lower_left_corner = vec3(-half_width, -half_height, 1.0);
    c.lower_left_corner = c.origin - w - half_width*u - half_height*v;
    c.horizontal = 2*half_width*u;
    c.vertical = 2*half_height*v;
}

