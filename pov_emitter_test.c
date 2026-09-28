/* Run from any writable directory. All configuration lives in a temporary INI. */
#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #c); exit(1); } } while (0)

static int visibility[2];
static void *nodes[2];
static void THISCALL retained_tool_frame(void *object, unsigned int property, float *view)
{
    CHECK(object == nodes[0]);
    CHECK(property == 0x02fff049u);
    memset(view, 0, 16*sizeof(float));
    view[0] = view[5] = view[10] = view[15] = 1;
    view[12] = 0.1f; view[13] = -0.2f; view[14] = -0.5f;
}
static void THISCALL set_visibility(void *object, DWORD property, unsigned int value)
{
    CHECK(property == SCRIPT_PROPERTY_VISIBILITY);
    if (object == nodes[0]) visibility[0] = value;
    else if (object == nodes[1]) visibility[1] = value;
    else CHECK(0);
}

static void make_node(int index)
{
    BYTE *allocation = calloc(1, 512), *meta = calloc(1, 512), *dispatch = calloc(1, 512);
    nodes[index] = (void*)(((uintptr_t)allocation + 79u) & ~(uintptr_t)15u);
    *(BYTE**)((BYTE*)nodes[index] - SCRIPT_OBJECT_META_BACK_OFFSET) = meta;
    *(BYTE**)(meta + SCRIPT_OBJECT_DISPATCH_TABLE_OFFSET) = dispatch;
    *(script_u32_set_t*)(dispatch + SCRIPT_OBJECT_BOOL_SET_OFFSET) = set_visibility;
}

static int count_particles(void)
{
    int count = 0;
    for (int i = 0; i < cfg.particle_limit; ++i) count += !!liquid_particles[i].active;
    return count;
}

static void __cdecl mock_mouse_device(void *device, int x, int y, float *u, float *v, int mode)
{
    CHECK(device == nodes[0] && x == 456 && y == 217 && mode == 1);
    *u = -0.30f; *v = 0.42f;
}
static void __cdecl mock_mouse_channel(void *channel, float *u, float *v, int mode)
{
    CHECK(channel == nodes[1] && mode == 1);
    *u -= 0.03f; *v -= 0.02f;
}

static void test_crosshair_aim(void)
{
    float frame[16] = {0}, position[3], direction[3];
    BYTE input[12] = {0};
    void *context[5] = {0}, *update = context;
    DWORD now = GetTickCount();
    /* Visible tool frame, including its live rotation and scale. */
    frame[0] = -1.2f; frame[1] = 1.6f;
    frame[4] = -0.8f; frame[5] = -0.6f;
    frame[10] = frame[15] = 1;
    frame[12] = 0.1f; frame[13] = -0.2f; frame[14] = -0.5f;
    CHECK(liquid_tool_outlet_view(frame, position, direction));
    CHECK(fabsf(position[0] - (0.1f + 0.8f*0.00045815884f)) < 0.000001f);
    CHECK(fabsf(position[1] - (-0.2f + 0.6f*0.00045815884f)) < 0.000001f);
    CHECK(direction[0] == 0 && direction[1] == 0 && direction[2] == -1);

    captured_d3d_projection_valid = 1;
    memset(&captured_d3d_projection, 0, sizeof(captured_d3d_projection));
    captured_d3d_projection._11 = 1.5f;
    captured_d3d_projection._22 = 2.4f;
    captured_d3d_projection._34 = -1;
    captured_d3d_projection._31 = 0.03f;
    captured_d3d_projection._32 = -0.02f;
    liquid_tool_crosshair[0] = -0.33f; liquid_tool_crosshair[1] = 0.4f;
    liquid_tool_crosshair_tick = now;
    cfg.pov_reach = 1.0f;
    CHECK(liquid_tool_crosshair_direction(position, direction, now));
    CHECK(direction[0] < 0 && direction[1] > 0 && direction[2] < 0);
    /* A line launched from the OFFSET tip must intersect the cursor.
       The old camera-parallel direction fails this regression. */
    float t = (-1 - position[2])/direction[2];
    float hit[3] = {position[0] + t*direction[0], position[1] + t*direction[1], -1};
    CHECK(fabsf(hit[0]*1.5f - 0.03f + 0.33f) < 0.000001f);
    CHECK(fabsf(hit[1]*2.4f + 0.02f - 0.4f) < 0.000001f);

    CHECK(!liquid_tool_crosshair_direction(position, direction, now + 251));
    captured_d3d_projection._11 = 0;
    CHECK(!liquid_tool_crosshair_direction(position, direction, now));

    /* TK17's locked-centre input path does not consult the OS cursor. */
    cfg.pov_enabled = cfg.liquids_enabled = 1;
    input[1] = 1;
    liquid_capture_tool_crosshair(&update, input, now + 20);
    CHECK(liquid_tool_crosshair[0] == 0 && liquid_tool_crosshair[1] == 0);
    CHECK(liquid_tool_crosshair_tick == now + 20);
    /* The free crosshair uses TK17's input sample and channel conversion. */
    {
        BYTE *meta0 = calloc(1, 1024), *meta1 = calloc(1, 1024);
        BYTE *table = calloc(1, 64), *dispatch = table + 32;
        BYTE *old0 = *(BYTE**)((BYTE*)nodes[0] - 0x18);
        BYTE *old1 = *(BYTE**)((BYTE*)nodes[1] - 0x18);
        *(unsigned int*)(dispatch - 0x1c) = 1;
        *(BYTE**)(meta0 + 0x200) = dispatch;
        *(BYTE**)(meta1 + 0x204) = dispatch;
        *(BYTE**)((BYTE*)nodes[0] - 0x18) = meta0;
        *(BYTE**)((BYTE*)nodes[1] - 0x18) = meta1;
        context[4] = nodes[0]; context[3] = nodes[1];
        liquid_mouse_device = mock_mouse_device;
        liquid_mouse_channel = mock_mouse_channel;
        input[1] = 0;
        *(int*)(input + 4) = 456; *(int*)(input + 8) = 217;
        liquid_capture_tool_crosshair(&update, input, now + 30);
        CHECK(fabsf(liquid_tool_crosshair[0] + 0.33f) < 0.000001f);
        CHECK(fabsf(liquid_tool_crosshair[1] - 0.4f) < 0.000001f);
        CHECK(liquid_tool_crosshair_tick == now + 30);
        *(BYTE**)((BYTE*)nodes[0] - 0x18) = old0;
        *(BYTE**)((BYTE*)nodes[1] - 0x18) = old1;
        free(meta0); free(meta1); free(table);
        liquid_mouse_device = NULL; liquid_mouse_channel = NULL;
    }
    cfg.pov_enabled = 0;
    liquid_capture_tool_crosshair(&update, input, now + 40);
    CHECK(liquid_tool_crosshair_tick == now + 30);
    liquid_tool_crosshair_tick = 0;
    captured_d3d_projection_valid = 0;
    puts("PASS: visible tip rotation/scale, tip-to-cursor convergence at fixed reach, stale/singular aim rejection and disabled isolation");
}

static void test_rendered_ribbon(void)
{
    BYTE reference[128 * 128 * 4];
    liquid_depth_snapshot_t snapshot;
    unsigned int alpha = 0;
    memset(liquid_particles, 0, sizeof(liquid_particles));
    memset(&tool_emitter, 0, sizeof(tool_emitter));
    memset(model_emitters, 0, sizeof(model_emitters));
    cfg.particle_limit = 4;
    cfg.liquids_enabled = 1;
    cfg.collision_enabled = 0;
    cfg.model_stream_cohesion = 0.5f;
    cfg.particle_size = 0.025f;
    cfg.stream_thickness = cfg.stream_opacity = cfg.core_opacity = 1;
    cfg.draw_layer = -1;
    captured_camera_inverse[12] = 0;
    memset(&liquid_native.projection, 0, sizeof(liquid_native.projection));
    liquid_native.projection._11 = liquid_native.projection._22 = 1;
    liquid_native.projection._33 = -1.001f; liquid_native.projection._34 = -1;
    liquid_native.projection._43 = -0.1001f;
    liquid_native.viewport = (D3DVIEWPORT8){0, 0, 128, 128, 0, 1};
    liquid_native.projection_valid = 1;
    CHECK(liquid_native_ensure(128, 128));
    for (int i = 0; i < 128 * 128; ++i) liquid_native.depth[i] = 1;
    for (int i = 0; i < 4; ++i) {
        liquid_particle_t *p = &liquid_particles[i];
        p->active = 1; p->source_kind = 1; p->emission_id = 71;
        p->spawn_order = i + 1; p->stream_id = 1;
        p->position[0] = i * 0.08f - 0.12f; p->position[2] = -1;
        p->age = 0.1f; p->lifetime = 2; p->size_scale = p->opacity = 1;
        p->emission_time = i * 0.01f;
    }
    model_emitters[0].source_kind = model_emitters[0].person_index = 1;
    model_emitters[0].emission_id = 71;
    model_emitters[0].stream_id = 1;
    CHECK(liquid_native_composite(D3D11_COMPARISON_LESS_EQUAL));
    memcpy(reference, liquid_native.rgba, sizeof(reference));
    for (int i = 0; i < 128 * 128; ++i) alpha += reference[i * 4 + 3];
    CHECK(alpha > 0);
    tool_emitter = model_emitters[0];
    tool_emitter.source_kind = 2; tool_emitter.person_index = 0;
    memset(model_emitters, 0, sizeof(model_emitters));
    for (int i = 0; i < 4; ++i) liquid_particles[i].source_kind = 2;
    liquid_native_snapshot(&snapshot);
    for (int i = 0; i < 4; ++i) CHECK(snapshot.particles[i].valid);
    CHECK(liquid_native_composite(D3D11_COMPARISON_LESS_EQUAL));
    CHECK(memcmp(reference, liquid_native.rgba, sizeof(reference)) == 0);
    liquid_native_release_targets();
    puts("PASS: real D3D11 compositor produces identical model/POV ribbon pixels; native depth snapshots include POV");
}

static void test_shared_flow(DWORD now)
{
    liquid_emitter_t model;
    liquid_particle_t expected[LIQUID_PARTICLE_CAP];
    float position[3] = {0, -0.72f, -1.25f}, direction[3] = {0, 0, -1};
    float nozzle[3];
    cfg.pov_enabled = 1;
    cfg.tool_start_delay = 0.2f;
    cfg.model_pulse_duration = 0.2f;
    cfg.model_pulse_interval = 0.3f;
    cfg.model_pulse_count = 2;
    cfg.spawn_rate = 100.0f;
    cfg.ejaculation_sound_preset[0] = 0;
    memset(captured_camera_inverse, 0, sizeof(captured_camera_inverse));
    captured_camera_inverse[0] = captured_camera_inverse[5] =
        captured_camera_inverse[10] = captured_camera_inverse[15] = 1;
    captured_camera_inverse_valid = 1;
    liquid_note_tool_command(now);
    CHECK(tool_emitter.active && tool_emitter.source_kind == 2 && tool_emitter.person_index == 0);
    CHECK(tool_emitter.start_tick == now + 200 && tool_emitter.end_tick == now + 900);
    liquid_update_emitter(&tool_emitter, now + 100, 0.05f);
    CHECK(count_particles() == 0);
    liquid_update_emitter(&tool_emitter, now + 300, 0.05f);
    CHECK(count_particles() > 0 && tool_emitter.pulse_emitting && tool_emitter.current_pulse_index == 0);
    CHECK(liquid_model_nozzle(&tool_emitter, nozzle));
    CHECK(fabsf(nozzle[1] + 0.20f / 2.41421356f) < 0.0001f);
    int first_count = count_particles();
    liquid_update_emitter(&tool_emitter, now + 500, 0.05f);
    CHECK(!tool_emitter.pulse_emitting && first_count == count_particles());
    liquid_update_emitter(&tool_emitter, now + 800, 0.05f);
    CHECK(tool_emitter.pulse_emitting && tool_emitter.current_pulse_index == 1);
    CHECK(count_particles() > first_count);
    int second_count = count_particles();
    liquid_update_emitter(&tool_emitter, now + 1300, 0.05f);
    CHECK(!tool_emitter.pulse_emitting && count_particles() == second_count);
    CHECK(liquid_model_emitter_for_emission(tool_emitter.emission_id) == &tool_emitter);
    captured_camera_inverse[12] = 2.0f;
    CHECK(liquid_emitter_transform(&tool_emitter, nozzle, direction));
    CHECK(fabsf(nozzle[0] - 2.0f) < 0.0001f && direction[2] == -1);
    CHECK(liquid_particles[0].position[0] == 0); /* Existing liquid stays in world space. */
    liquid_update_emitter(&tool_emitter, now + 3200, 0.05f);
    CHECK(!tool_emitter.active);
    puts("PASS: POV delay, pulses/gaps/count, timeout, nozzle, camera motion and world-space particles");

    /* Same random seed and settings must produce the same physical flow. */
    memset(liquid_particles, 0, sizeof(liquid_particles));
    liquid_begin_emitter(&model, 1, 1, now, 3.0f);
    liquid_random_state = 123;
    for (int i = 0; i < 20; ++i)
        liquid_spawn_particle(&model, position, direction, 1, 0.7f, 0, i * 0.01f);
    memcpy(expected, liquid_particles, sizeof(expected));
    memset(liquid_particles, 0, sizeof(liquid_particles));
    liquid_begin_emitter(&tool_emitter, 2, 0, now, 3.0f);
    liquid_random_state = 123;
    for (int i = 0; i < 20; ++i) {
        liquid_spawn_particle(&tool_emitter, position, direction, 1, 0.7f, 0, i * 0.01f);
        CHECK(liquid_particles[i].source_kind == 2);
        CHECK(liquid_particles[i].satellite == expected[i].satellite);
        CHECK(liquid_particles[i].launch_speed > expected[i].launch_speed);
        CHECK(liquid_particles[i].size_scale == expected[i].size_scale);
        CHECK(liquid_model_breakup_blend(&liquid_particles[i]) == liquid_model_breakup_blend(&expected[i]));
    }
    CHECK(liquid_has_airborne_model_particles());
    liquid_particles[0].collided = 1;
    liquid_particles[0].contact_person = 3;
    cfg.collision_follow_bodies = 1;
    CHECK(liquid_body_follow_person_mask() == 4);
    cfg.collision_spawn_model_stains = 1;
    liquid_particles[0].impact_velocity[2] = -1;
    liquid_queue_native_model_contact(&liquid_particles[0]);
    liquid_native_contact_t contact;
    CHECK(liquid_pop_native_model_contact(tool_emitter.emission_id, GetTickCount(), &contact));
    CHECK(contact.particle_id == liquid_particles[0].spawn_order);
    puts("PASS: stronger POV speed, shared spread/satellites/size/breakup; body-follow and native contact queue");
}

static void test_fallback_projection(void)
{
    const float projection_scales[] = {5.671282f, 2.41421356f, 1.0f, 0.5773503f};
    float view[3];
    /* The original fallback was outside the normal 45-degree vertical FOV. */
    CHECK(fabsf(-0.72f * 2.41421356f / 1.25f) > 1.0f);
    captured_d3d_projection_valid = 1;
    for (int i = 0; i < 4; ++i) {
        captured_d3d_projection._22 = projection_scales[i];
        liquid_tool_fallback_view(view);
        CHECK(view[0] == 0 && view[2] < -0.01f);
        CHECK(fabsf(view[1] * projection_scales[i] / -view[2] + 0.80f) < 0.0001f);
    }
    captured_d3d_projection_valid = 0;
    liquid_tool_fallback_view(view);
    CHECK(fabsf(view[1] * 2.41421356f / -view[2] + 0.80f) < 0.0001f);
    puts("PASS: old fallback is off-screen at 45 degrees; new fallback remains inside 20/45/90/120-degree views");
}

int main(void)
{
    char directory[MAX_PATH], temporary[MAX_PATH];
    DWORD now = GetTickCount();
    CHECK(GetCurrentDirectoryA(MAX_PATH, directory));
    CHECK(GetTempFileNameA(directory, "pov", 0, temporary));
    CHECK(DeleteFileA(temporary));
    lstrcpynA(config_path, temporary, sizeof(config_path));
    create_default_config_if_missing();
    load_config();
    CHECK(!cfg.pov_enabled);
    cfg.enabled = 0;
    liquid_note_tool_command(now);
    CHECK(!tool_emitter.active && last_tool_command_tick == now);
    make_node(0); make_node(1);
    liquid_remember_recent_native_ray_node(nodes[0]);
    liquid_classify_recent_native_ray_nodes(1);
    CHECK(visibility[0] == 1);
    liquid_handle_setting_change("NcLiquidsPovEnabled", "ON");
    load_config();
    cfg.enabled = 0;
    CHECK(cfg.pov_enabled);
    /* The tool was already constructed with POV disabled. X does not name
       it again: switching on must hide the previously classified ray. */
    tool_spermray_group_object = nodes[1];
    remember_liquid_transform_node(nodes[0], "tool_mesh");
    remember_liquid_transform_node(nodes[1], "Tool01");
    CHECK(tool_mesh_object == nodes[0]);
    tool_spermray_root_object = nodes[1];
    liquid_note_tool_command(now);
    CHECK(visibility[0] == 0);
    CHECK(tool_spermray_group_object == nodes[1] && tool_spermray_root_object == nodes[1]);
    {
        float position[3], direction[3];
        captured_camera_inverse[0] = captured_camera_inverse[5] =
            captured_camera_inverse[10] = captured_camera_inverse[15] = 1;
        captured_camera_inverse_valid = 1;
        BYTE *meta = *(BYTE**)((BYTE*)nodes[0] - SCRIPT_OBJECT_META_BACK_OFFSET);
        BYTE *table = calloc(1, 512), *dispatch = table + 32;
        *(unsigned int*)(dispatch - 0x1c) = 1;
        *(BYTE**)(meta + 0x124) = dispatch;
        *(liquid_vector_get_t*)(dispatch + 0x80) = retained_tool_frame;
        CHECK(liquid_node_interface(nodes[0], 0x124));
        CHECK(liquid_node_method(nodes[0], 0x124, 0x80));
        CHECK(liquid_emitter_transform(&tool_emitter, position, direction));
        CHECK(fabsf(position[0] - 0.1f) < 0.00001f);
        CHECK(fabsf(position[1] + 0.20045815884f) < 0.00001f);
        CHECK(fabsf(position[2] + 0.50032760238f) < 0.00001f);
        CHECK(direction[0] == 0 && direction[1] == 0 && direction[2] == -1);
        *(BYTE**)(meta + 0x124) = NULL;
        free(table);
    }
    puts("PASS: X hides an already-created native ray after live enable and preserves tool transforms");
    liquid_remember_recent_native_ray_node(nodes[1]);
    liquid_classify_recent_native_ray_nodes(0);
    CHECK(visibility[0] == 0 && visibility[1] == 0);
    test_shared_flow(now);
    liquid_remember_spermray_source_person(nodes[0], 1);
    liquid_remember_spermray_source_person(nodes[0], -1);
    CHECK(liquid_spermray_source_person(nodes[0]) == -1);
    liquid_remember_spermray_source_person(nodes[1], 4);
    CHECK(liquid_spermray_source_person(nodes[1]) == 4);
    model_emitters[0].active = 1;
    liquid_particles[30].active = 1;
    liquid_particles[30].source_kind = 1;
    liquid_handle_setting_change("NcLiquidsPovEnabled", "OFF");
    liquid_config_write_time_valid = 1;
    memset(&liquid_config_write_time, 0, sizeof(liquid_config_write_time));
    liquid_check_config_reload(now + 5000);
    CHECK(!cfg.pov_enabled && !tool_emitter.active);
    CHECK(visibility[0] == 1 && visibility[1] == 0);
    CHECK(!liquid_particles[0].active && liquid_particles[30].active && model_emitters[0].active);
    cfg.pov_enabled = 1;
    liquid_note_tool_command(now + 5500);
    CHECK(visibility[0] == 0 && visibility[1] == 0);
    liquid_restore_tool_ray_nodes();
    CHECK(visibility[0] == 1 && visibility[1] == 0);
    memset(&tool_emitter, 0, sizeof(tool_emitter));
    cfg.liquids_enabled = 0; cfg.pov_enabled = 1;
    liquid_note_tool_command(now + 6000);
    CHECK(!tool_emitter.active);
    liquid_restore_native_ray_nodes();
    CHECK(visibility[1] == 1);
    CHECK(DeleteFileA(temporary));
    puts("PASS: default native POV, UI binding, owner isolation, live disable and master-off behavior");
    test_fallback_projection();
    test_crosshair_aim();
    test_rendered_ribbon();
    return 0;
}
