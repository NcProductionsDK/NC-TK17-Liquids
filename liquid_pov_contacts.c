typedef int (THISCALL *liquid_pick_view_t)(void *, const void *, void *, const float *, void *, int);
static liquid_pick_view_t tramp_PovPickView;
static int liquid_pov_pick_installed;

static int liquid_pov_matrix_version(void *camera, unsigned int *version)
{
    typedef unsigned int (THISCALL *version_get_t)(void *, unsigned int);
    void *parent = NULL;
    version_get_t getter;
    /* Match AppPick::PickView (APP+22440). PickRay's final argument is
       the camera matrix version, NOT flags. A mismatch skips all geometry. */
    if (!liquid_node_parent(camera, &parent)) return 0;
    getter = (version_get_t)liquid_node_method(parent, 0x124, 0x140);
    if (!getter) return 0;
    *version = getter(parent, 0x05fff049u);
    return 1;
}

/* Returns a miss unless an actual droplet contact picks the same surface.
   Never fall back to the cursor ray on this replacement path. */
static int liquid_pick_tool_contact(void *self, const void *geometry, void *camera, void *results,
                                   int include_hidden, void *frame, void *descriptor, DWORD now)
{
    static unsigned int logged_emission;
    static DWORD logged_tick;
    liquid_native_contact_t contact;
    float origin[3], direction[3];
    void *data = NULL;
    int count = 0, result = -1;
    unsigned int matrix_version;
    if (!tramp_AppPick_PickRay || !tool_emitter.emission_id ||
        descriptor != tool_emitter.native_stain_descriptor ||
        !liquid_pov_matrix_version(camera, &matrix_version)) return -1;
    liquid_native_finish_frozen_capture();
    if (liquid_native_freeze_capture) liquid_native_freeze_capture->projector_frame = NULL;
    liquid_verify_recent_body_contacts(self, geometry, results, include_hidden, matrix_version,
                                      tool_emitter.emission_id, now);
    if (!(cfg.collision_spawn_model_stains || cfg.collision_spawn_room_stains) ||
        !liquid_pop_native_model_contact(tool_emitter.emission_id, now, &contact) ||
        !liquid_native_contact_view_ray(&contact, origin, direction)) return -1;
    if (contact.target_kind != 1 && cfg.collision_spawn_model_stains)
        result = liquid_measured_pick(0, self, geometry, origin, direction, results, include_hidden, matrix_version);
    liquid_native_pick_result_data(results, &data, &count);
    if (contact.target_kind != 1 && cfg.collision_spawn_model_stains && (result < 0 || count <= 0) &&
        liquid_native_contact_camera_ray(&contact, origin, direction)) {
        result = liquid_measured_pick(0, self, geometry, origin, direction, results, include_hidden, matrix_version);
        liquid_native_pick_result_data(results, &data, &count);
    }
    result = liquid_try_room_stain_pick(self, &contact, frame, origin, direction, results, matrix_version, result);
    data = NULL; count = 0;
    liquid_native_pick_result_data(results, &data, &count);
    if (result >= 0 && data && count > 0 && ptr_readable((BYTE*)data + 0x14, 12)) {
        float gap;
        int outcome = liquid_contact_attachment_outcome(&contact, (float*)((BYTE*)data + 0x14), &gap);
        if (!(contact.target_kind == 1 && outcome == LIQUID_ATTACHMENT_ROOM) &&
            (outcome < LIQUID_ATTACHMENT_EXACT || outcome > LIQUID_ATTACHMENT_VERIFIED))
            result = -1; /* Body behind a room hit is not the droplet contact. */
    } else result = -1;
    if (result >= 0) {
        liquid_native_stage_surface_projector(frame, data);
        liquid_native_track_custom_stain_pick(descriptor, &contact, result, results);
    } else {
        liquid_native_retry_stain_miss(&contact, now, 1, -1, 0);
    }
    /* Sample diagnostics at most once per second within an emission. Contact
       processing itself always runs, regardless of this logging limit. */
    if (cfg.enabled && (logged_emission != contact.emission_id || now - logged_tick >= 1000)) {
        logged_emission = contact.emission_id;
        logged_tick = now;
        log_line("liquid POV contact pick sample emission=%u particle=%u result=%d matrix_version=%u cursor_fallback=0",
                 contact.emission_id, contact.particle_id, result, matrix_version);
    }
    return result;
}

static int THISCALL hook_PovPickView(void *self, const void *geometry, void *camera,
                                   const float *cursor, void *results, int include_hidden)
{
    const void *caller = __builtin_return_address(0);
    if (cfg.liquids_enabled && cfg.pov_enabled &&
        caller == (BYTE*)GetModuleHandleA(NULL) + 0x001f22c7u) {
        void *frame = __builtin_frame_address(1), *descriptor = NULL;
        if (ptr_readable((BYTE*)frame - 0x14, sizeof(void*)))
            descriptor = *(void**)((BYTE*)frame - 0x14);
        return liquid_pick_tool_contact(self, geometry, camera, results, include_hidden,
                                       frame, descriptor, GetTickCount());
    }
    return tramp_PovPickView ? tramp_PovPickView(self, geometry, camera, cursor, results, include_hidden) : -1;
}

static void liquid_install_pov_hooks(HMODULE exe, HMODULE app)
{
    BYTE *target;
    if (!cfg.liquids_enabled) return;
    if (!liquid_pov_tool_aim_installed) {
        static const BYTE expected[] = {0x8b,0x43,0x78,0x8d,0x55,0x9c};
        target = (BYTE*)exe + 0x0006fff4u;
        if (ptr_executable(target) && !memcmp(target, expected, sizeof(expected)) &&
            install_inline_hook(target, (void*)hook_PovToolAim, sizeof(expected), &tramp_PovToolAim)) {
            liquid_pov_tool_aim_installed = 1;
            log_line("liquid POV native camera aim hook installed; preserves mode-3 tool coordinates");
        } else {
            log_line("liquid POV native camera aim hook unavailable; native tool placement retained");
        }
    }
    if (!liquid_pov_tracking_installed) {
        static const BYTE expected[] = {0x85,0xc0,0x0f,0x8e,0xfe,0x01,0x00,0x00};
        void *unused_trampoline = NULL;
        target = (BYTE*)exe + 0x0006fec0u;
        liquid_pov_tracking_active = (BYTE*)exe + 0x0006fec8u;
        liquid_pov_tracking_idle = (BYTE*)exe + 0x000700c6u;
        if (ptr_executable(target) && !memcmp(target, expected, sizeof(expected)) &&
            install_inline_hook(target, (void*)hook_PovTracking, sizeof(expected), &unused_trampoline)) {
            /* The bridge branches to absolute continuations; never execute
               the copied relative conditional jump in the trampoline. */
            liquid_pov_tracking_installed = 1;
            log_line("liquid POV cursor tracking gate installed; follows custom lifetime");
        }
    }
    if (!liquid_pov_pick_installed) {
        static const BYTE expected[] = {0x55,0x8b,0xec,0x83,0xe4,0xf8};
        target = (BYTE*)GetProcAddress(app,
            "?PickView@AppPick@@QAEHABVStringRef@Bionic@@PAVScriptObject@3@ABVVector2f@3@AAV?$Array@VPickResult@@@3@_N@Z");
        if (target && ptr_executable(target) && !memcmp(target, expected, sizeof(expected)) &&
            install_inline_hook(target, (void*)hook_PovPickView, sizeof(expected), (void**)&tramp_PovPickView)) {
            liquid_pov_pick_installed = 1;
            log_line("liquid POV contact-only PickView hook installed");
        }
    }
    if (!liquid_pov_cleanup_installed) {
        static const BYTE expected[] = {0x8b,0x8b,0xc0,0x04,0x00,0x00};
        target = (BYTE*)exe + 0x00070128u;
        liquid_pov_cleanup_resume = (BYTE*)exe + 0x00070281u;
        if (ptr_executable(target) && !memcmp(target, expected, sizeof(expected)) &&
            install_inline_hook(target, (void*)hook_PovCleanup, sizeof(expected), &tramp_PovCleanup)) {
            liquid_pov_cleanup_installed = 1;
            log_line("liquid POV cleanup gate installed; lifetime follows pulses and airborne liquid");
        }
    }
}
