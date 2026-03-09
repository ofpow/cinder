const char *material_types[] = {"Lambertian", "Metal", "Dielectric"};

bool vec3_editor(Vector3 *v, struct nk_context *ctx, float min, float max, float inc) {
    Vector3 new_v = *v;
    
    nk_layout_row_dynamic(ctx, 60, 3);
    nk_property_float(ctx, "#X:", min, &new_v.x, max, inc, inc);
    nk_property_float(ctx, "#Y:", min, &new_v.y, max, inc, inc);
    nk_property_float(ctx, "#Z:", min, &new_v.z, max, inc, inc);

    if (!Vector3Equals(new_v, *v)) {
        v->x = new_v.x;
        v->y = new_v.y;
        v->z = new_v.z;
        return true;
    }
    return false;
}

bool float_editor(float *f, struct nk_context *ctx, char *text, float min, float max, float inc) {
    float new_f = *f;

    nk_layout_row_dynamic(ctx, 60, 1);
    nk_property_float(ctx, text, min, &new_f, max, inc, inc);
    
    if (*f != new_f) {
        *f = new_f;
        return true;
    }
    return false;
}

bool vec3_color_editor(Vector3 *c, struct nk_context *ctx) {
    struct nk_colorf color = {c->x, c->y, c->z, 0};
    nk_layout_row_dynamic(ctx, 120, 1);
    struct nk_colorf new_c = nk_color_picker(ctx, color, NK_RGB);
    struct nk_color new_c_255 = nk_rgb_cf(new_c);
    nk_layout_row_dynamic(ctx, 30, 3);
    new_c_255.r = nk_propertyi(ctx, "R:", 0, new_c_255.r, 255, 1, 1);
    new_c_255.g = nk_propertyi(ctx, "G:", 0, new_c_255.g, 255, 1, 1);
    new_c_255.b = nk_propertyi(ctx, "B:", 0, new_c_255.b, 255, 1, 1);
    new_c = nk_color_cf(new_c_255);

    if (!Vector3Equals(*c, (Vector3){new_c.r, new_c.g, new_c.b})) {
        c->x = new_c.r;
        c->y = new_c.g;
        c->z = new_c.b;
        return true;
    }
    return false;
}

bool sphere_gui(struct nk_context* ctx, Hitable *h) {
    if (h->type != SPHERE) return false;
    bool reset = false;

    Vector3 center = {h->data[0], h->data[1], h->data[2]};
    float radius = h->data[3];
    int material_type = h->data[4] - 1;
    Vector3 albedo = {h->data[5], h->data[6], h->data[7]};
    float data = h->data[8];
    Vector3 emission_col = {h->data[9], h->data[10], h->data[11]};
    float emission_str = h->data[12];

    if (nk_begin(ctx, "Sphere", nk_rect(0, 0, 600, 700),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE)) {

        if (vec3_editor(&center, ctx, -100, 100, 0.01)) {
            h->data[0] = center.x; 
            h->data[1] = center.y; 
            h->data[2] = center.z; 
            reset = true;
        }

        if (float_editor(&radius, ctx, "Radius:", -100, 100, 0.01)) {
            h->data[3] = radius;
            reset = true;
        }

        nk_layout_row_dynamic(ctx, 60, 1);
        int new_material_type = nk_combo(ctx, material_types, 3, material_type, 30, nk_vec2(200,200));
        if (new_material_type != material_type) {
            reset = true;
            h->data[4] = new_material_type + 1;
        }

        if (vec3_color_editor(&albedo, ctx)) {
            reset = true;
            h->data[5] = albedo.x;
            h->data[6] = albedo.y;
            h->data[7] = albedo.z;
        }

        if (float_editor(&data, ctx, "Mat data:", -100, 100, 0.01)) {
            h->data[8] = data;
            reset = true;
        }

        if (vec3_color_editor(&emission_col, ctx)) {
            reset = true;
            h->data[9] = emission_col.x;
            h->data[10] = emission_col.y;
            h->data[11] = emission_col.z;
        }

        if (float_editor(&emission_str, ctx, "Emit str:", -100, 100, 0.01)) {
            h->data[12] = emission_str;
            reset = true;
        }
        
    }
    nk_end(ctx);
    return reset;
}

bool triangle_gui(struct nk_context *ctx, Hitable *h) {
    if (h->type != TRIANGLE) return false;
    bool reset = false;

    Vector3 a = {h->data[0], h->data[1], h->data[2]};
    Vector3 b = {h->data[3], h->data[4], h->data[5]};
    Vector3 c = {h->data[6], h->data[7], h->data[8]};
    int material_type = h->data[9] - 1;
    Vector3 albedo = {h->data[10], h->data[11], h->data[12]};
    float data = h->data[13];
    Vector3 emission_col = {h->data[14], h->data[15], h->data[16]};
    float emission_str = h->data[17];

    if (nk_begin(ctx, "Triangle", nk_rect(0, 700, 600, 770),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE)) {
        if (vec3_editor(&a, ctx, -100, 100, 0.01)) {
            h->data[0] = a.x; 
            h->data[1] = a.y; 
            h->data[2] = a.z; 
            reset = true;
        }
        if (vec3_editor(&b, ctx, -100, 100, 0.01)) {
            h->data[3] = b.x; 
            h->data[4] = b.y; 
            h->data[5] = b.z; 
            reset = true;
        }
        if (vec3_editor(&c, ctx, -100, 100, 0.01)) {
            h->data[6] = c.x; 
            h->data[7] = c.y; 
            h->data[8] = c.z; 
            reset = true;
        }
        nk_layout_row_dynamic(ctx, 60, 1);
        int new_material_type = nk_combo(ctx, material_types, 3, material_type, 30, nk_vec2(200,200));
        if (new_material_type != material_type) {
            h->data[9] = new_material_type + 1;
            reset = true;
        }
        if (vec3_color_editor(&albedo, ctx)) {
            h->data[10] = albedo.x;
            h->data[11] = albedo.y;
            h->data[12] = albedo.z;
            reset = true;
        }
        if (float_editor(&data, ctx, "Mat data:", -100, 100, 0.01)) {
            h->data[13] = data;
            reset = true;
        }
        if (vec3_color_editor(&emission_col, ctx)) {
            h->data[14] = emission_col.x;
            h->data[15] = emission_col.y;
            h->data[16] = emission_col.z;
            reset = true;
        }
        if (float_editor(&emission_str, ctx, "Emit str:", -100, 100, 0.01)) {
            h->data[17] = emission_str;
            reset = true;
        }
    }
    nk_end(ctx);
    return reset;
}
