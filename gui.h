const char *material_types[] = {"Lambertian", "Metal", "Dielectric"};

bool sphere_gui(struct nk_context* ctx, Hitable *h) {
    bool reset = false;

    Vector3 center = {h->data[0], h->data[1], h->data[2]};
    float radius = h->data[3];
    int material_type = h->data[4] - 1;
    struct nk_colorf albedo = {h->data[5], h->data[6], h->data[7], 0};
    float data = h->data[8];
    struct nk_colorf emission_col = {h->data[9], h->data[10], h->data[11], 0};
    float emission_str = h->data[12];

    if (nk_begin(ctx, "Sphere", nk_rect(100, 100, 500, 700),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE)) {
        Vector3 new_center = center;
        float new_radius = radius;
        float new_data = data;
        float new_emission_str = emission_str;

        nk_layout_row_dynamic(ctx, 60, 3);
        nk_property_float(ctx, "X:", -100, &new_center.x, 100, 0.01, 0.01);
        nk_property_float(ctx, "Y:", -100, &new_center.y, 100, 0.01, 0.01);
        nk_property_float(ctx, "Z:", -100, &new_center.z, 100, 0.01, 0.01);

        nk_layout_row_dynamic(ctx, 60, 1);
        nk_property_float(ctx, "Radius:", -100, &new_radius, 100, 0.01, 0.01);

        nk_layout_row_dynamic(ctx, 60, 1);
        int new_material_type = nk_combo(ctx, material_types, 3, material_type, 30, nk_vec2(200,200));

        nk_layout_row_dynamic(ctx, 120, 1);
        struct nk_colorf new_albedo = nk_color_picker(ctx, albedo, NK_RGB);
        struct nk_color a = nk_rgb_cf(new_albedo);
        nk_layout_row_dynamic(ctx, 30, 3);
        a.r = nk_propertyi(ctx, "R:", 0, a.r, 255, 1, 1);
        a.g = nk_propertyi(ctx, "G:", 0, a.g, 255, 1, 1);
        a.b = nk_propertyi(ctx, "B:", 0, a.b, 255, 1, 1);
        new_albedo = nk_color_cf(a);

        nk_layout_row_dynamic(ctx, 60, 1);
        nk_property_float(ctx, "Mat Data:", -100, &new_data, 100, 0.01, 0.01);

        nk_layout_row_dynamic(ctx, 120, 1);
        struct nk_colorf new_emission_col = nk_color_picker(ctx, emission_col, NK_RGB);
        struct nk_color e = nk_rgb_cf(new_emission_col);
        nk_layout_row_dynamic(ctx, 30, 3);
        e.r = nk_propertyi(ctx, "R:", 0, e.r, 255, 1, 1);
        e.g = nk_propertyi(ctx, "G:", 0, e.g, 255, 1, 1);
        e.b = nk_propertyi(ctx, "B:", 0, e.b, 255, 1, 1);
        new_emission_col = nk_color_cf(e);

        nk_layout_row_dynamic(ctx, 60, 1);
        nk_property_float(ctx, "Emit str:", -100, &new_emission_str, 100, 0.01, 0.01);
        
        if (!Vector3Equals(center, new_center)) {
            reset = true;
            center = new_center;
            h->data[0] = center.x;
            h->data[1] = center.y;
            h->data[2] = center.z;
        }
        if (new_radius != radius) {
            reset = true;
            h->data[3] = new_radius;
        }
        if (new_material_type != material_type) {
            reset = true;
            h->data[4] = new_material_type + 1;
        }
        if (new_albedo.r != albedo.r || new_albedo.g != albedo.g || new_albedo.b != albedo.b) {
            reset = true;
            h->data[5] = new_albedo.r;
            h->data[6] = new_albedo.g;
            h->data[7] = new_albedo.b;
        }
        if (new_data != data) {
            reset = true;
            h->data[8] = new_data;
        }
        if (new_emission_col.r != emission_col.r ||
            new_emission_col.g != emission_col.g || 
            new_emission_col.b != emission_col.b) {
            reset = true;
            h->data[9] = new_emission_col.r;
            h->data[10] = new_emission_col.g;
            h->data[11] = new_emission_col.b;
        }
        if (new_emission_str != emission_str) {
            reset = true;
            h->data[12] = new_emission_str;
        }
    }
    return reset;
}
