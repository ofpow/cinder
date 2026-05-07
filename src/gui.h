const char *material_types[] = {"Lambertian", "Metal", "Dielectric"};

bool vec3_editor(Vector3 *v, char *name, struct nk_context *ctx, float min, float max, float inc) {
    Vector3 new_v = *v;
    
    nk_layout_row_dynamic(ctx, 60, 4);
    nk_label(ctx, name, NK_TEXT_LEFT | NK_TEXT_ALIGN_MIDDLE);
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

bool int_editor(int *i, struct nk_context *ctx, char *text, float min, float max, float inc) {
    int new_i = *i;

    nk_layout_row_dynamic(ctx, 60, 1);
    nk_property_int(ctx, text, min, &new_i, max, inc, inc);
    
    if (*i != new_i) {
        *i = new_i;
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
    new_c_255.r = nk_propertyi(ctx, "#R:", 0, new_c_255.r, 255, 1, 1);
    new_c_255.g = nk_propertyi(ctx, "#G:", 0, new_c_255.g, 255, 1, 1);
    new_c_255.b = nk_propertyi(ctx, "#B:", 0, new_c_255.b, 255, 1, 1);
    new_c = nk_color_cf(new_c_255);

    if (!Vector3Equals(*c, (Vector3){new_c.r, new_c.g, new_c.b})) {
        c->x = new_c.r;
        c->y = new_c.g;
        c->z = new_c.b;
        return true;
    }
    return false;
}

bool material_gui(struct nk_context* ctx, MaterialData *mat) {
    bool reset = false;

    nk_layout_row_dynamic(ctx, 60, 1);
    int new_material_type = nk_combo(ctx, material_types, 3, mat->type, 30, nk_vec2(200,200));
    if (new_material_type != mat->type) {
        reset = true;
        mat->type = new_material_type;
    }
    if (vec3_color_editor(&mat->albedo, ctx)) reset = true;
    if (float_editor(&mat->data, ctx, "Mat data:", -100, 100, 0.01)) reset = true;
    if (vec3_color_editor(&mat->emission_col, ctx)) reset = true;
    if (float_editor(&mat->emission_str, ctx, "Emit str:", -100, 100, 0.01)) reset = true;

    return reset;
}

bool sphere_gui(struct nk_context* ctx, Hitable *h) {
    bool reset = false;

    Sphere s = {0};
    memcpy(&s, h->data, sizeof(Sphere));

    if (vec3_editor(&s.center, "Center:", ctx, -100, 100, 0.01)) {
        reset = true;
    }
    if (float_editor(&s.radius, ctx, "Radius:", -100, 100, 0.01)) {
        reset = true;
    }
    
    reset |= material_gui(ctx, &s.mat);

        
    memcpy(h->data, &s, sizeof(Sphere));
    return reset;
}

bool triangle_gui(struct nk_context *ctx, Hitable *h) {
    bool reset = false;

    Triangle t = {0};
    memcpy(&t, h->data, sizeof(Triangle));

    if (vec3_editor(&t.a, "A:", ctx, -100, 100, 0.01)) {
        reset = true;
    }
    if (vec3_editor(&t.b, "B:", ctx, -100, 100, 0.01)) {
        reset = true;
    }
    if (vec3_editor(&t.c, "C:", ctx, -100, 100, 0.01)) {
        reset = true;
    }

    reset |= material_gui(ctx, &t.mat);
    
    memcpy(h->data, &t, sizeof(Triangle));
    return reset;
}

bool camera_gui(struct nk_context *ctx, Vector3 *lookfrom, Vector3 *lookat, Vector3 *vup, float *aperture, int *vfov) {
    bool reset = false;
    if (nk_begin(ctx, "Camera", nk_rect(0, 870, 800, 770),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE)) {
        if (vec3_editor(lookfrom, "lookfrom:", ctx, -100, 100, 0.01)) {
            reset = true;
        }
        if (vec3_editor(lookat, "lookat:", ctx, -100, 100, 0.01)) {
            reset = true;
        }
        if (vec3_editor(vup, "vup:", ctx, -100, 100, 0.01)) {
            reset = true;
        }
        if (float_editor(aperture, ctx, "Aperture:", -100, 100, 0.01)) {
            reset = true;
        }
        if (int_editor(vfov, ctx, "FOV:", -360, 360, 1)) {
            reset = true;
        }
    }
    nk_end(ctx);
    return reset;
}

bool object_editor(struct nk_context *ctx, int *selected_index, Hitables world) {
    bool reset = false;
    if (nk_begin(ctx, "Object Editor", nk_rect(0, 0, 800, 870),
             NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {

        nk_layout_row_dynamic(ctx, 60, 1);
        int new_index = *selected_index;
        nk_property_int(ctx, "Object id:", 0, &new_index, world.index - 1, 1, 0.5);
        if (new_index != *selected_index) 
            *selected_index = new_index;            

        nk_layout_row_dynamic(ctx, 730, 1);

        Hitable h = world.data[*selected_index];

        if (h.type == SPHERE && nk_group_begin(ctx, "SPHERE", NK_WINDOW_BORDER)) {
            reset |= sphere_gui(ctx, &h);
            nk_group_end(ctx);
        } else if (h.type == TRIANGLE && nk_group_begin(ctx, "TRIANGLE", NK_WINDOW_BORDER)) {
            reset |= triangle_gui(ctx, &h);
            nk_group_end(ctx);
        }

        world.data[*selected_index] = h;
    }
    nk_end(ctx);
    return reset;
}

void handle_output(void) {
    if (IsKeyPressed(KEY_O)) {
        printf("outputting\n");
        Vector4 *buf = calloc(X*Y, sizeof(Vector4));
        rlReadShaderBuffer(screen_ssbo, buf, X*Y*sizeof(Vector4), 0);
        FILE *f = fopen("out.ppm", "w");

        fprintf(f, "P3\n");
        fprintf(f, "%d %d\n", X, Y);
        fprintf(f, "255\n");

        for (int i = 0; i < X*Y; i++) {
            Vector4 col = buf[i];
            col.x = sqrt(col.x);
            col.y = sqrt(col.y);
            col.z = sqrt(col.z);
            col.w = sqrt(col.w);
            int r = (255.99*(col.x/col.w) > 255) ? 255 : 255.99*(col.x/col.w);
            int g = (255.99*(col.y/col.w) > 255) ? 255 : 255.99*(col.y/col.w);
            int b = (255.99*(col.z/col.w) > 255) ? 255 : 255.99*(col.z/col.w);
            fprintf(f, "%d %d %d\n", r, g, b);
        }
        free(buf);
        fclose(f);
        printf("output done\n");
    }
}
