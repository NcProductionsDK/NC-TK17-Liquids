/* Contact topology and an actual D3D11 WARP shader-opacity regression. */
#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #c); exit(1); } } while (0)

static void near_value(float a, float b)
{
    CHECK(fabsf(a - b) < 0.002f);
}

static void initialize_contact(int index, float x)
{
    liquid_particle_t *p = &liquid_particles[index];
    memset(p, 0, sizeof(*p));
    p->active = p->collided = p->contact_normal_valid = 1;
    p->emission_id = p->stream_id = 1;
    p->spawn_order = index + 1;
    p->contact_physx_person = 1;
    p->contact_normal[2] = 1;
    p->position[0] = x; p->position[2] = -1;
    p->size_scale = p->opacity = 1;
    p->age = 0.2f; p->lifetime = 2;
}

static void test_contacts(void)
{
    liquid_particle_t *a = &liquid_particles[0], *b = &liquid_particles[1];
    liquid_d3d11_vertex_t vertices[48];
    D3DMATRIX projection = {0};
    D3DVIEWPORT8 viewport = {0, 0, 1000, 1000, 0, 1};
    UINT count = 0;
    memset(liquid_particles, 0, sizeof(liquid_particles));
    cfg.particle_limit = 8;
    cfg.collision_connect_droplets = 1;
    cfg.collision_connection_distance = 0.03f;
    cfg.collision_connection_thickness = 0.55f;
    cfg.contact_droplet_size = 0.003f;
    cfg.contact_droplet_opacity = 0.6f;
    cfg.collision_surface_hold = 1;
    initialize_contact(0, 0);
    initialize_contact(1, 0.02f);
    CHECK(liquid_contact_pair_compatible(a, b));
    liquid_link_contact(b, NULL, NULL);
    CHECK(b->contact_link_order == a->spawn_order);
    initialize_contact(2, 0.021f);
    liquid_link_contact(&liquid_particles[2], NULL, NULL);
    CHECK(liquid_particles[2].contact_link_order == b->spawn_order);
    CHECK(a->contact_successor_order == b->spawn_order); /* No branching. */
    liquid_particles[2].active = 0;
    b->contact_physx_person = 2; CHECK(!liquid_contact_pair_compatible(a, b));
    b->contact_physx_person = 1;
    b->pulse_index = 1; CHECK(!liquid_contact_pair_compatible(a, b)); b->pulse_index = 0;
    b->stream_id = 2; CHECK(!liquid_contact_pair_compatible(a, b)); b->stream_id = 1;
    b->secondary = 1; CHECK(!liquid_contact_pair_compatible(a, b)); b->secondary = 0;
    b->contact_normal_valid = 0; CHECK(!liquid_contact_pair_compatible(a, b)); b->contact_normal_valid = 1;
    b->contact_normal[2] = -1; CHECK(!liquid_contact_pair_compatible(a, b)); b->contact_normal[2] = 1;
    b->position[0] = 0.04f; CHECK(!liquid_contact_pair_compatible(a, b)); b->position[0] = 0.02f;
    a->contact_anchor = 1; b->contact_anchor = 2;
    CHECK(!liquid_contact_pair_compatible(a, b)); a->contact_anchor = b->contact_anchor = 0;
    a->contact_physx_person = b->contact_physx_person = -1;
    b->position[2] = -1.01f; CHECK(!liquid_contact_pair_compatible(a, b)); b->position[2] = -1;
    CHECK(!liquid_contact_path_supported(a, b, NULL, NULL));
    a->contact_physx_person = b->contact_physx_person = 1;
    memset(captured_camera_inverse, 0, sizeof(captured_camera_inverse));
    captured_camera_inverse[0] = captured_camera_inverse[5] = captured_camera_inverse[10] =
        captured_camera_inverse[15] = 1;
    captured_camera_inverse_valid = 1;
    projection._11 = projection._22 = 1; projection._33 = -1.001f;
    projection._34 = -1; projection._43 = -0.1001f;
    liquid_append_contact_connections(vertices, &count, 1000, 1000, 1, 1, 1,
                                      &projection, &viewport, 1);
    CHECK(count == 24);
    /* The connector's middle is narrower than its endpoints. */
    CHECK(fabsf(vertices[7].position[1] - vertices[8].position[1]) <
          fabsf(vertices[0].position[1] - vertices[5].position[1]));
    count = LIQUID_D3D11_VERTEX_CAP - cfg.particle_limit * 6;
    liquid_append_contact_connections(vertices, &count, 1000, 1000, 1, 1, 1,
                                      &projection, &viewport, 1);
    CHECK(count == LIQUID_D3D11_VERTEX_CAP - cfg.particle_limit * 6);
    cfg.collision_connect_droplets = 0; count = 0;
    liquid_append_contact_connections(vertices, &count, 1000, 1000, 1, 1, 1,
                                      &projection, &viewport, 1);
    CHECK(count == 0);
    cfg.collision_connect_droplets = 1; a->spawn_order = 99;
    liquid_append_contact_connections(vertices, &count, 1000, 1000, 1, 1, 1,
                                      &projection, &viewport, 1);
    CHECK(count == 0); /* Recycled endpoint cannot keep an old connector. */
    puts("PASS: pinned contact chains; tapered geometry; ownership/pulse/normal/gap guards; vertex reserve; disable and slot reuse");
}

static void test_room_path(void)
{
    liquid_depth_snapshot_t *snapshot = calloc(1, sizeof(*snapshot));
    float pixels[25], depth = 0.9009f;
    D3D11_MAPPED_SUBRESOURCE mapped = {pixels, 5 * sizeof(float), 0};
    liquid_particle_t a = {0}, b = {0};
    int i;
    CHECK(snapshot);
    snapshot->valid = 1;
    snapshot->left = snapshot->top = 48;
    snapshot->width = snapshot->height = 5;
    snapshot->scene_width = snapshot->scene_height = 100;
    snapshot->viewport.Width = snapshot->viewport.Height = 100;
    snapshot->camera_inverse[0] = snapshot->camera_inverse[5] =
        snapshot->camera_inverse[10] = snapshot->camera_inverse[15] = 1;
    snapshot->projection._11 = snapshot->projection._22 = 1;
    snapshot->projection._33 = -1.001f; snapshot->projection._34 = -1;
    snapshot->projection._43 = -0.1001f;
    for (i = 0; i < 25; i++) pixels[i] = depth;
    CHECK(liquid_unproject_depth_snapshot(snapshot, 49.5f, 50.5f, depth, a.position));
    CHECK(liquid_unproject_depth_snapshot(snapshot, 51.5f, 50.5f, depth, b.position));
    CHECK(liquid_contact_path_supported(&a, &b, snapshot, &mapped));
    pixels[12] = 0.9989f; /* Hole/distant background between two deposits. */
    CHECK(!liquid_contact_path_supported(&a, &b, snapshot, &mapped));
    free(snapshot);
    puts("PASS: room connector requires nearby surface throughout its path");
}

static void test_base_shader(void)
{
    ID3D11Device *device = NULL;
    ID3D11DeviceContext *context = NULL;
    ID3D11Texture2D *target = NULL, *staging = NULL;
    ID3D11RenderTargetView *rtv = NULL;
    D3D_FEATURE_LEVEL level;
    D3D11_TEXTURE2D_DESC desc = {0};
    D3D11_VIEWPORT viewport = {0, 0, 32, 32, 0, 1};
    liquid_d3d11_vertex_t vertices[6];
    liquid_curve_point_t points[2] = {{0}};
    liquid_particle_t particle = {0};
    const float base_values[3] = {0, 0.4f, 1};
    UINT stride = sizeof(vertices[0]), offset = 0;
    int i;
    CHECK(SUCCEEDED(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0,
        NULL, 0, D3D11_SDK_VERSION, &device, &level, &context)));
    CHECK(liquid_ensure_d3d11_resources(device));
    desc.Width = desc.Height = 32; desc.MipLevels = desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    desc.SampleDesc.Count = 1; desc.BindFlags = D3D11_BIND_RENDER_TARGET;
    CHECK(SUCCEEDED(ID3D11Device_CreateTexture2D(device, &desc, NULL, &target)));
    CHECK(SUCCEEDED(ID3D11Device_CreateRenderTargetView(device, (ID3D11Resource*)target, NULL, &rtv)));
    desc.BindFlags = 0; desc.Usage = D3D11_USAGE_STAGING; desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    CHECK(SUCCEEDED(ID3D11Device_CreateTexture2D(device, &desc, NULL, &staging)));
    ID3D11DeviceContext_OMSetRenderTargets(context, 1, &rtv, NULL);
    /* Write alpha in this test to inspect the production pixel shader output. */
    ID3D11DeviceContext_OMSetBlendState(context, NULL, NULL, 0xffffffff);
    ID3D11DeviceContext_OMSetDepthStencilState(context, liquid_d3d11_depth_state, 0);
    ID3D11DeviceContext_RSSetState(context, liquid_d3d11_rasterizer_state);
    ID3D11DeviceContext_RSSetViewports(context, 1, &viewport);
    ID3D11DeviceContext_IASetInputLayout(context, liquid_d3d11_input_layout);
    ID3D11DeviceContext_IASetPrimitiveTopology(context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ID3D11DeviceContext_IASetVertexBuffers(context, 0, 1, &liquid_d3d11_vertex_buffer, &stride, &offset);
    ID3D11DeviceContext_VSSetShader(context, liquid_d3d11_vertex_shader, NULL, 0);
    ID3D11DeviceContext_PSSetShader(context, liquid_d3d11_pixel_shader, NULL, 0);
    particle.lifetime = 2; particle.opacity = 0.52f; particle.age = 0.001f;
    cfg.stream_base_fade_length = 0.4f; cfg.stream_opacity = 0.66f; cfg.core_opacity = 0.2f;
    points[0].x = points[1].x = 16;
    points[0].y = 0; points[1].y = 32;
    points[0].half_width = points[1].half_width = 14;
    for (i = 0; i < 3; i++) {
        D3D11_MAPPED_SUBRESOURCE mapped = {0};
        const float clear[4] = {0};
        const float *row;
        UINT count = 0;
        cfg.stream_base_opacity = base_values[i];
        liquid_stream_alpha(&particle, 0, 0, &points[0].alpha, &points[0].base_alpha);
        points[1].alpha = points[0].alpha; points[1].base_alpha = points[0].base_alpha;
        CHECK(liquid_append_smooth_ribbon(vertices, &count, points, 2, 32, 32) == 1);
        CHECK(SUCCEEDED(ID3D11DeviceContext_Map(context, (ID3D11Resource*)liquid_d3d11_vertex_buffer,
            0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)));
        memcpy(mapped.pData, vertices, sizeof(vertices));
        ID3D11DeviceContext_Unmap(context, (ID3D11Resource*)liquid_d3d11_vertex_buffer, 0);
        ID3D11DeviceContext_ClearRenderTargetView(context, rtv, clear);
        ID3D11DeviceContext_Draw(context, count, 0);
        ID3D11DeviceContext_CopyResource(context, (ID3D11Resource*)staging, (ID3D11Resource*)target);
        CHECK(SUCCEEDED(ID3D11DeviceContext_Map(context, (ID3D11Resource*)staging,
            0, D3D11_MAP_READ, 0, &mapped)));
        row = (const float*)((BYTE*)mapped.pData + 16 * mapped.RowPitch);
        near_value(row[16 * 4 + 3], base_values[i]);
        if (i == 2) CHECK(row[2 * 4 + 3] < 0.9f); /* Soft edge remains. */
        ID3D11DeviceContext_Unmap(context, (ID3D11Resource*)staging, 0);
    }
    particle.age = 0.2f;
    {
        float alpha, base;
        liquid_stream_alpha(&particle, 0.4f, 0, &alpha, &base);
        near_value(base, 0);
        near_value(alpha, liquid_particle_visual_alpha(&particle) * cfg.stream_opacity * 1.08f);
        liquid_stream_alpha(&particle, 0, 1, &alpha, &base);
        near_value(alpha, 0); near_value(base, 0);
        cfg.stream_base_fade_length = 0;
        liquid_stream_alpha(&particle, 0, 0, &alpha, &base);
        near_value(base, 0);
    }
    ID3D11DeviceContext_ClearState(context);
    liquid_release_d3d11_resources();
    ID3D11RenderTargetView_Release(rtv);
    ID3D11Texture2D_Release(staging); ID3D11Texture2D_Release(target);
    ID3D11DeviceContext_Release(context); ID3D11Device_Release(device);
    puts("PASS: actual WARP shader center alpha is 0/0.4/1 despite newborn/core opacity; soft edge and downstream behavior retained");
}

int main(void)
{
    {
        liquid_emitter_t emitter = {0};
        float position[3];
        emitter.person_index = emitter.emission_id = emitter.transform_valid = 1;
        emitter.current_position[0] = 0.12f;
        CHECK(liquid_model_nozzle(&emitter, position));
        near_value(position[0], 0.12f);
        /* Repeated calls in the same clock tick must follow without lag. */
        emitter.current_position[0] = -0.08f;
        emitter.current_position[1] = 0.04f;
        CHECK(liquid_model_nozzle(&emitter, position));
        near_value(position[0], -0.08f); near_value(position[1], 0.04f);
        emitter.transform_valid = 0;
        CHECK(!liquid_model_nozzle(&emitter, position));
        puts("PASS: render anchor follows exact source immediately; invalid transform rejected");
    }
    test_contacts(); test_room_path(); test_base_shader();
    return 0;
}
