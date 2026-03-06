bool sphere_gui(struct nk_context* ctx, Hitable *h) {
    bool reset = false;
    Vector3 center = {h->data[0], h->data[1], h->data[2]};
    float radius = h->data[3];
    if (nk_begin(ctx, "Sphere", nk_rect(100, 100, 500, 500),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE)) {
        Vector3 new_center = center;
        float new_radius = radius;
        nk_layout_row_dynamic(ctx, 60, 3);
        nk_property_float(ctx, "X:", -100, &new_center.x, 100, 0.01, 0.01);
        nk_property_float(ctx, "Y:", -100, &new_center.y, 100, 0.01, 0.01);
        nk_property_float(ctx, "Z:", -100, &new_center.z, 100, 0.01, 0.01);

        nk_layout_row_dynamic(ctx, 60, 1);
        nk_property_float(ctx, "Radius:", -100, &new_radius, 100, 0.01, 0.01);
        
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
    }
    return reset;
}
