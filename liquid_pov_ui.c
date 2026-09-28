/* Native command/update thread owns the window subclass and cursor handle.
   SetCursor(NULL) avoids changing Windows' reference-counted ShowCursor state. */
static HWND liquid_pov_cursor_window;
static WNDPROC liquid_pov_previous_window_proc;
static HCURSOR liquid_pov_saved_cursor;
static int liquid_pov_pointer_hidden;
static int liquid_pov_pointer_requested;

static void liquid_pov_release_pointer(void)
{
    liquid_pov_pointer_requested = 0;
    liquid_pov_restore_pointer();
}

static void liquid_pov_restore_pointer(void)
{
    if (liquid_pov_pointer_hidden) {
        SetCursor(liquid_pov_saved_cursor);
        liquid_pov_pointer_hidden = 0;
        liquid_pov_saved_cursor = NULL;
    }
}

static int liquid_pov_pointer_in_client(HWND window)
{
    RECT client;
    POINT point;
    return window && GetForegroundWindow() == window &&
        GetCursorPos(&point) && ScreenToClient(window, &point) &&
        GetClientRect(window, &client) && PtInRect(&client, point);
}

static void liquid_pov_hide_pointer(void)
{
    HCURSOR previous = SetCursor(NULL);
    if (!liquid_pov_pointer_hidden) {
        liquid_pov_saved_cursor = previous;
        liquid_pov_pointer_hidden = 1;
    } else if (previous) {
        /* Remember a newer cursor requested by the game during this frame. */
        liquid_pov_saved_cursor = previous;
    }
}

static LRESULT CALLBACK liquid_pov_window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    WNDPROC previous = liquid_pov_previous_window_proc;
    if (message == WM_RBUTTONDOWN || message == WM_RBUTTONDBLCLK) {
        liquid_tool_begin_camera_gesture(MK_RBUTTON);
    } else if (message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK) {
        liquid_tool_begin_camera_gesture(MK_LBUTTON);
    } else if (message == WM_RBUTTONUP) {
        liquid_tool_camera_button &= ~MK_RBUTTON;
    } else if (message == WM_LBUTTONUP) {
        liquid_tool_camera_button &= ~MK_LBUTTON;
    } else if (message == WM_CAPTURECHANGED || message == WM_CANCELMODE) {
        liquid_tool_camera_button = 0;
    }
    if (message == WM_SETCURSOR) {
        if (LOWORD(lparam) == HTCLIENT && liquid_pov_pointer_requested &&
            liquid_pov_pointer_in_client(window)) {
            liquid_pov_hide_pointer();
            return TRUE;
        }
        liquid_pov_restore_pointer();
    } else if (message == WM_KILLFOCUS || (message == WM_ACTIVATEAPP && !wparam) ||
               message == WM_NCMOUSEMOVE || message == WM_NCDESTROY) {
        if (message != WM_NCMOUSEMOVE) {
            liquid_tool_camera_button = 0;
            liquid_tool_free_crosshair_valid = 0;
        }
        liquid_pov_restore_pointer();
    }
    if (message == WM_NCDESTROY) {
        if ((WNDPROC)GetWindowLongPtrA(window, GWLP_WNDPROC) == liquid_pov_window_proc)
            SetWindowLongPtrA(window, GWLP_WNDPROC, (LONG_PTR)previous);
        liquid_pov_cursor_window = NULL;
        liquid_pov_previous_window_proc = NULL;
        liquid_pov_pointer_requested = 0;
    }
    return previous ? CallWindowProcA(previous, window, message, wparam, lparam) :
                      DefWindowProcA(window, message, wparam, lparam);
}

static void liquid_pov_cursor_tick(void)
{
    HWND window;
    DWORD process = 0;
    liquid_pov_pointer_requested = liquid_pov_keep_tracking(liquid_tool_native_app);
    if (!liquid_pov_pointer_requested) {
        liquid_pov_restore_pointer();
        return;
    }
    window = GetForegroundWindow();
    if (!window || GetWindowThreadProcessId(window, &process) != GetCurrentThreadId() ||
        process != GetCurrentProcessId() || !liquid_pov_pointer_in_client(window)) {
        liquid_pov_restore_pointer();
        return;
    }
    if (!liquid_pov_cursor_window) {
        LONG_PTR previous;
        SetLastError(0);
        previous = SetWindowLongPtrA(window, GWLP_WNDPROC, (LONG_PTR)liquid_pov_window_proc);
        if (!previous) return;
        liquid_pov_previous_window_proc = (WNDPROC)previous;
        liquid_pov_cursor_window = window;
    }
    if (window == liquid_pov_cursor_window) liquid_pov_hide_pointer();
}

static void liquid_pov_cancel(void)
{
    unsigned int emission = tool_emitter.emission_id;
    void *descriptor = tool_emitter.native_stain_descriptor;
    if (!cfg.liquids_enabled || !cfg.pov_enabled || !emission) return;
    /* Release the native state before discarding the association. CleanUp
       still runs normally and handles the native tool/decal teardown. */
    if (liquid_pov_keep_tracking(liquid_tool_native_app))
        liquid_pov_finish_descriptor(descriptor);
    for (int i = 0; i < LIQUID_NATIVE_CONTACT_SLOTS; ++i) {
        liquid_native_contact_t *contact = &liquid_native_contacts[i];
        if (contact->emission_id == emission)
            InterlockedCompareExchange(&contact->state, 0, 1);
    }
    for (int i = 0; i < LIQUID_PARTICLE_CAP; ++i)
        if (liquid_particles[i].source_kind == 2)
            memset(&liquid_particles[i], 0, sizeof(liquid_particles[i]));
    for (int i = 0; i < LIQUID_AUDIO_PENDING_CAP; ++i)
        if (liquid_audio_pending[i].person_index == 0)
            liquid_audio_pending[i].active = 0;
    for (int i = 0; i < LIQUID_AUDIO_VOICE_CAP; ++i)
        if (liquid_audio_voices[i].active && liquid_audio_voices[i].person_index == 0)
            liquid_audio_voice_release(&liquid_audio_voices[i]);
    memset(&tool_emitter, 0, sizeof(tool_emitter));
    last_tool_command_tick = 0;
    liquid_tool_launch_target_valid = 0;
    liquid_tool_crosshair_tick = 0;
    liquid_tool_free_crosshair_valid = 0;
    liquid_tool_camera_button = 0;
    InterlockedExchange(&liquid_native_last_contact_queue_tick[LIQUID_MODEL_EMITTER_COUNT], 0);
    liquid_pov_release_pointer();
    log_line("liquid POV cancelled by CleanUp emission=%u; pending pulses, audio and liquid cleared", emission);
}
