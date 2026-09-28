/* TK17 158.001: native POV completion is polled after NativeStainUpdate.
   Keep that cleanup pending while custom pulses/airborne liquid finish. */
static void *liquid_tool_native_app;
static void *tramp_PovCleanup;
static void *liquid_pov_cleanup_resume __attribute__((used));
static int liquid_pov_cleanup_installed;
static void *liquid_pov_tracking_active __attribute__((used));
static void *liquid_pov_tracking_idle __attribute__((used));
static int liquid_pov_tracking_installed;
static void *tramp_PovToolAim;
static int liquid_pov_tool_aim_installed;
static float liquid_pov_native_aim[2];
static unsigned int liquid_pov_native_aim_emission;
static int liquid_pov_native_aim_valid;

static int liquid_pov_camera_look_active(void *update, void *input)
{
    const BYTE *app = liquid_tool_native_app;
    return app && input == app + 0x378 && ptr_readable(app, 0x46c) &&
        *(void**)(app + 0x468) == update && *(int*)(app + 0xac) == 3;
}

static void liquid_pov_finish_descriptor(void *descriptor)
{
    if (ptr_writable((BYTE*)descriptor + 0x10, 0x24)) {
        *(LONG*)((BYTE*)descriptor + 0x10) = 0;
        *(BYTE*)((BYTE*)descriptor + 0x2c) = 0;
        *(LONG*)((BYTE*)descriptor + 0x30) = 0;
    }
}

static void *liquid_selected_tool_descriptor(void *app, void *update)
{
    void **items;
    int index, count;
    if (!ptr_readable(app, 0x4c8) || !ptr_readable(update, 0x18) ||
        *(void**)((BYTE*)app + 0x468) != update ||
        *(int*)((BYTE*)app + 0x4c0) <= 0) return NULL;
    index = *(int*)((BYTE*)app + 0x4c4);
    items = *(void***)((BYTE*)update + 0x14);
    if (!ptr_readable((BYTE*)items - 4, 4)) return NULL;
    count = *((int*)items - 1);
    if (index < 0 || count < 1 || count > 4096 || index >= count ||
        !ptr_readable(items, count*sizeof(void*)) ||
        !ptr_readable(items[index], 0x40)) return NULL;
    return items[index];
}

static void liquid_bind_tool_descriptor(void *update, void *input)
{
    void *app, *descriptor;
    if (!cfg.liquids_enabled || !cfg.pov_enabled || !tool_emitter.emission_id || !input) return;
    /* EXE+7011C supplies app+378 as NativeStainUpdate's input. */
    app = (BYTE*)input - 0x378;
    descriptor = liquid_selected_tool_descriptor(app, update);
    if (!descriptor || !liquid_node_interface(*(void**)descriptor, 0x154)) return;
    liquid_tool_native_app = app;
    if (tool_emitter.native_stain_descriptor != descriptor)
        log_line("liquid POV native descriptor bound emission=%u descriptor=%p app=%p",
                 tool_emitter.emission_id, descriptor, app);
    tool_emitter.native_stain_descriptor = descriptor;
}

static int liquid_pov_busy(DWORD now)
{
    int i;
    if (!cfg.liquids_enabled || !cfg.pov_enabled || !tool_emitter.emission_id) return 0;
    if ((LONG)(tool_emitter.end_tick - now) > 0) return 1;
    for (i = 0; i < cfg.particle_limit; ++i) {
        const liquid_particle_t *p = &liquid_particles[i];
        if (p->active && !p->collided && p->source_kind == 2 &&
            p->emission_id == tool_emitter.emission_id) return 1;
    }
    return liquid_native_model_contact_pending(tool_emitter.emission_id, now);
}

static int __attribute__((used, noinline)) liquid_pov_keep_tracking(void *app)
{
    void *update;
    if (!cfg.liquids_enabled || !cfg.pov_enabled || !tool_emitter.emission_id ||
        app != liquid_tool_native_app ||
        !ptr_readable(app, 0x46c)) return 0;
    update = *(void**)((BYTE*)app + 0x468);
    return tool_emitter.native_stain_descriptor &&
        liquid_selected_tool_descriptor(app, update) == tool_emitter.native_stain_descriptor &&
        liquid_pov_busy(GetTickCount());
}

/* EXE+6FFF4 follows the mode-3 reset of [ebp-10] and [ebp-8].
   Keep native channel coordinates, not screen NDC: TK17 applies its own
   aspect/placement conversion next. Never change global camera/input state. */
static void __attribute__((used, noinline)) liquid_pov_preserve_native_aim(void *app, void *frame)
{
    float *x = (float*)((BYTE*)frame - 0x10);
    float *y = (float*)((BYTE*)frame - 0x08);
    int look, held;
    static int logged_held, log_count;
    if (!liquid_pov_keep_tracking(app)) {
        liquid_pov_native_aim_valid = 0;
        return;
    }
    if (liquid_pov_native_aim_emission != tool_emitter.emission_id ||
        !liquid_tool_free_crosshair_valid) {
        liquid_pov_native_aim_valid = 0;
        liquid_pov_native_aim_emission = tool_emitter.emission_id;
    }
    if (!ptr_writable(x, 12)) return;
    look = *(int*)((BYTE*)app + 0xac) == 3;
    held = look || *((BYTE*)app + 0x379) || liquid_tool_camera_button ||
        liquid_tool_camera_button_down();
    if (!held && _finite(*x) && _finite(*y) && fabsf(*x) <= 2 && fabsf(*y) <= 2) {
        liquid_pov_native_aim[0] = *x;
        liquid_pov_native_aim[1] = *y;
        liquid_pov_native_aim_valid = 1;
    } else if (look && liquid_pov_native_aim_valid) {
        *x = liquid_pov_native_aim[0];
        *y = liquid_pov_native_aim[1];
    }
    if (cfg.enabled && look != logged_held && log_count < 64) {
        logged_held = look;
        ++log_count;
        log_line("liquid POV native camera aim emission=%u mode=%d held=%d cached=%d channel=(%.4f,%.4f)",
                 tool_emitter.emission_id, *(int*)((BYTE*)app + 0xac), look,
                 liquid_pov_native_aim_valid, *x, *y);
    }
}

static void __attribute__((naked)) hook_PovToolAim(void)
{
    __asm__ __volatile__(
        "pushfl\n\tpushal\n\tmovl %esp, %ebx\n\t"
        "subl $528, %esp\n\tandl $-16, %esp\n\tfxsave (%esp)\n\tfninit\n\t"
        "subl $16, %esp\n\tmovl 16(%ebx), %eax\n\tmovl %eax, (%esp)\n\t"
        "movl 8(%ebx), %eax\n\tmovl %eax, 4(%esp)\n\t"
        "call _liquid_pov_preserve_native_aim\n\taddl $16, %esp\n\t"
        "fxrstor (%esp)\n\tmovl %ebx, %esp\n\tpopal\n\tpopfl\n\t"
        "jmp *_tramp_PovToolAim\n\t");
}

static void __attribute__((naked)) hook_PovTracking(void)
{
    __asm__ __volatile__(
        "pushfl\n\tpushal\n\tmovl %esp, %ebx\n\t"
        "subl $528, %esp\n\tandl $-16, %esp\n\tfxsave (%esp)\n\tfninit\n\t"
        "subl $16, %esp\n\tmovl 16(%ebx), %eax\n\tmovl %eax, (%esp)\n\t"
        "call _liquid_pov_keep_tracking\n\taddl $16, %esp\n\t"
        "fxrstor (%esp)\n\tmovl %ebx, %esp\n\ttestl %eax, %eax\n\tjz 1f\n\t"
        "popal\n\tpopfl\n\tjmp *_liquid_pov_tracking_active\n\t"
        "1: popal\n\tpopfl\n\ttestl %eax, %eax\n\tjle 2f\n\t"
        "jmp *_liquid_pov_tracking_active\n\t"
        "2: jmp *_liquid_pov_tracking_idle\n\t");
}

static int __attribute__((used, noinline)) liquid_pov_hold_cleanup(void *app)
{
    void *descriptor, *update;
    if (!cfg.liquids_enabled || !cfg.pov_enabled || app != liquid_tool_native_app ||
        !ptr_readable(app, 0x46c)) return 0;
    update = *(void**)((BYTE*)app + 0x468);
    descriptor = liquid_selected_tool_descriptor(app, update);
    if (!descriptor || descriptor != tool_emitter.native_stain_descriptor) return 0;
    if (liquid_pov_busy(GetTickCount())) return 1;
    liquid_pov_release_pointer();
    /* The custom sequence owns completion; do not wait for native repeats. */
    liquid_pov_finish_descriptor(descriptor);
    return 0;
}

static void __attribute__((naked)) hook_PovCleanup(void)
{
    __asm__ __volatile__(
        "pushfl\n\tpushal\n\tmovl %esp, %ebx\n\t"
        "subl $528, %esp\n\tandl $-16, %esp\n\tfxsave (%esp)\n\tfninit\n\t"
        "subl $16, %esp\n\tmovl 16(%ebx), %eax\n\tmovl %eax, (%esp)\n\t"
        "call _liquid_pov_hold_cleanup\n\taddl $16, %esp\n\t"
        "fxrstor (%esp)\n\tmovl %ebx, %esp\n\ttestl %eax, %eax\n\tjz 1f\n\t"
        "popal\n\tpopfl\n\tjmp *_liquid_pov_cleanup_resume\n\t"
        "1: popal\n\tpopfl\n\tjmp *_tramp_PovCleanup\n\t");
}
