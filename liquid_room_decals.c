/* Native room-decal prototype. Included after native group/array helpers.
   The room pick is never exposed to TK17 until geometry, impact and normal
   validation succeed. All work here runs inside the game-thread stain update. */
static engine_string_construct_cstr_t liquid_room_string_construct;
static engine_string_release_t liquid_room_string_release;
static DWORD liquid_room_last_stain_tick;

#include "liquid_room_triangles.c"
#include "liquid_room_clip.c"

static void liquid_room_clear_pending(void)
{
    for (int i = 0; i < LIQUID_NATIVE_CONTACT_SLOTS; ++i) {
        liquid_native_contact_t *slot = &liquid_native_contacts[i];
        if (InterlockedCompareExchange(&slot->state, 0, 0) == 1 && slot->target_kind)
            InterlockedCompareExchange(&slot->state, 0, 1);
    }
    liquid_room_last_stain_tick = 0;
    /* Created decals are owned and cleared by the original native CleanUp. */
}

static int liquid_room_stain_object_count(void)
{
    void **groups;
    int count, total = 0;
    void *update = liquid_native_freeze_capture ? liquid_native_freeze_capture->update : NULL;
    if (!update || !ptr_readable((BYTE*)update + 0x18, sizeof(groups))) return -1;
    groups = *(void***)((BYTE*)update + 0x18);
    count = liquid_native_diag_array_count(groups, sizeof(*groups));
    if (count < 0 || count > 4096) return -1;
    for (int i = 0; i < count; ++i) {
        int controls, nodes;
        if (!groups[i]) continue;
        controls = liquid_native_stain_control_count(groups[i]);
        if (!ptr_readable((BYTE*)groups[i] + 0x0c, sizeof(void*))) return -1;
        nodes = liquid_native_diag_array_count(*(void**)((BYTE*)groups[i] + 0x0c), sizeof(void*));
        if (controls < 0 || nodes < 0) return -1;
        /* Static room decals have geometry nodes but no animation controls. */
        if (nodes > controls) controls = nodes;
        if (controls >= 256 - total) return 256;
        total += controls;
    }
    return total;
}

static void liquid_room_stain_creation_report(const liquid_native_freeze_capture_t *capture)
{
    static unsigned int logged;
    if (cfg.enabled && capture->room_picks && logged++ < 16)
        log_line("liquid room decal native update picks=%d objects_before=%d objects_after=%d recovered_triangles=%u",
            capture->room_picks, capture->room_objects_before, liquid_room_stain_object_count(),
            capture->room_triangles_recovered);
}

static int liquid_native_contact_pick_gap(const liquid_native_contact_t *contact,
                                        const void *data, float limit, float *gap)
{
    float expected[3], delta[3];
    if (!data || !ptr_readable(data, 0x2c) ||
        !liquid_native_contact_particle_alive(contact) ||
        !liquid_world_to_view_point(contact->impact_world, expected)) return 0;
    for (int i = 0; i < 3; ++i) {
        delta[i] = ((const float*)((const BYTE*)data + 0x14))[i] - expected[i];
        if (!_finite(delta[i])) return 0;
    }
    *gap = sqrtf(liquid_vec3_dot(delta, delta));
    return *gap <= limit;
}

static int liquid_room_pick_matches(const liquid_native_contact_t *contact,
                                   const void *data, float *gap)
{
    const BYTE *object, *meta, *type;
    float expected_normal[3], picked_normal[3], world_normal[3];
    const float *camera = captured_camera_inverse;
    if (!liquid_native_contact_pick_gap(contact, data, 0.015f, gap)) return 0;
    /* EXE projection helper requires TPolygonGeometry (class slot 0x77).
       Keep this first prototype limited to directly picked polygon meshes. */
    object = *(const BYTE*const*)((const BYTE*)data + 4);
    if (!object || ((uintptr_t)object & 8) ||
        !ptr_readable(object - 0x18, sizeof(meta))) return 0;
    meta = *(const BYTE*const*)(object - 0x18);
    if (!meta || !ptr_readable(meta + 0x1dc, sizeof(type))) return 0;
    type = *(const BYTE*const*)(meta + 0x1dc);
    if (!type || !ptr_readable(type - 0x1c, sizeof(int)) || !*(const int*)(type - 0x1c)) return 0;
    memcpy(world_normal, contact->normal_world, sizeof(world_normal));
    memcpy(picked_normal, (const BYTE*)data + 0x20, sizeof(picked_normal));
    if (!liquid_vec3_normalize(world_normal) || !liquid_vec3_normalize(picked_normal)) return 0;
    for (int i = 0; i < 3; ++i)
        expected_normal[i] = liquid_vec3_dot(world_normal, camera + i*4);
    if (!liquid_vec3_normalize(expected_normal)) return 0;
    return liquid_vec3_dot(expected_normal, picked_normal) >= 0.5f;
}

static int liquid_try_room_stain_pick(void *self, liquid_native_contact_t *contact,
    void *native_frame, const float *origin, const float *direction, void *results,
    unsigned int matrix_version, int previous_result)
{
    void *data = NULL;
    int count = 0, result = -1, object_count;
    float gap = -1, world_origin[3], world_direction[3], view_origin[3], view_direction[3];
    char *room_list = NULL;
    DWORD now = GetTickCount();
    static unsigned int diagnostic_count;
    if (!contact->target_kind) return previous_result;
    liquid_native_pick_result_data(results, &data, &count);
    /* Unclassified depth hits can still be bodies. Preserve a close body
       pick; a body behind the actual impact must not steal a room decal. */
    if (contact->target_kind == 2 && cfg.collision_spawn_model_stains &&
        previous_result >= 0 && count > 0 &&
        liquid_native_contact_pick_gap(contact, data, 0.006f, &gap)) return previous_result;
    if (!cfg.collision_spawn_room_stains || !native_stain_projector_hook_installed ||
        !liquid_room_clip_trampoline ||
        !liquid_room_string_construct || !liquid_room_string_release ||
        (liquid_room_last_stain_tick && now - liquid_room_last_stain_tick < 100)) return -1;
    object_count = liquid_room_stain_object_count();
    if (object_count < 0 || object_count >= 256) {
        if (cfg.enabled && diagnostic_count++ < 24)
            log_line("liquid room decal skipped reason=native-object-budget objects=%d", object_count);
        return -1;
    }
    memcpy(world_direction, contact->normal_world, sizeof(world_direction));
    if (!liquid_vec3_normalize(world_direction)) return -1;
    for (int i = 0; i < 3; ++i) {
        world_origin[i] = contact->impact_world[i] + world_direction[i] * 0.03f;
        world_direction[i] = -world_direction[i];
        view_direction[i] = 0;
    }
    if (!liquid_world_to_view_point(world_origin, view_origin)) return -1;
    for (int i = 0; i < 3; ++i)
        view_direction[i] = liquid_vec3_dot(world_direction, captured_camera_inverse + i*4);
    /* PickView has not initialized the native projector at this point.
       The installed post-pick surface-projector hook supplies its final
       normal-aligned matrix for both person and POV paths, including floors. */
    if (!native_frame || !ptr_writable((BYTE*)native_frame - LIQUID_NATIVE_STAIN_MATRIX_EBP_OFFSET, 64) ||
        !liquid_vec3_normalize(view_direction) ||
        !liquid_native_replace_caller_ray(origin, direction, view_origin, view_direction)) return -1;
    liquid_room_string_construct(&room_list, "Room|PoseEditRoom");
    if (!room_list) return -1;
    /* Bionic StringRef is the constructed string value, not String's address.
       Match EXE+1F24A3 (push [ebp-48]) and String::operator StringRef(). */
    result = liquid_measured_pick(1, self, room_list, origin, direction, results, 0, matrix_version);
    liquid_room_string_release(&room_list);
    data = NULL; count = 0;
    liquid_native_pick_result_data(results, &data, &count);
    if (result < 0 || count <= 0 || !liquid_room_pick_matches(contact, data, &gap)) {
        if (cfg.enabled && diagnostic_count++ < 24)
            log_line("liquid room decal rejected particle=%u result=%d hits=%d gap=%.5f", contact->particle_id, result, count, gap);
        return -1; /* Native caller checks the signed result before reading data. */
    }
    contact->target_kind = 1;
    liquid_room_reset_triangle_validation(liquid_native_freeze_capture);
    liquid_native_freeze_capture->room_projection_target = *(void**)((BYTE*)data + 4);
    liquid_native_freeze_capture->room_projection_frame = native_frame;
    liquid_room_last_stain_tick = now;
    if (!liquid_native_freeze_capture->room_picks++)
        liquid_native_freeze_capture->room_objects_before = object_count;
    for (int i = 0; i < cfg.particle_limit; ++i) {
        liquid_particle_t *p = &liquid_particles[i];
        if (p->active && p->collided && p->emission_id == contact->emission_id &&
            p->spawn_order == contact->particle_id)
            InterlockedExchange(&p->room_contact_verified, 1);
    }
    if (cfg.enabled && diagnostic_count++ < 24)
        log_line("liquid room decal accepted particle=%u target=%p gap=%.5f native_projection=1", contact->particle_id, *(void**)((BYTE*)data+4), gap);
    if (cfg.enabled) {
        LARGE_INTEGER start;
        QueryPerformanceCounter(&start);
        liquid_native_freeze_capture->room_extract_start = start.QuadPart;
    }
    return result;
}
