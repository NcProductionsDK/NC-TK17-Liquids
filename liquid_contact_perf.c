/* Detailed timings share the existing diagnostic switch. No per-triangle
   clocks or file writes: at most one slow-update report per second. */
static int liquid_measured_pick(int room, void *self, const void *geometry,
    const float *origin, const float *direction, void *results, int hidden, unsigned int version)
{
    liquid_native_freeze_capture_t *capture=liquid_native_freeze_capture;
    LARGE_INTEGER start,end;
    int measure=cfg.enabled && capture;
    if (!tramp_AppPick_PickRay) return -1;
    if (measure) QueryPerformanceCounter(&start);
    int result=tramp_AppPick_PickRay(self,geometry,origin,direction,results,hidden,version);
    if (measure) {
        QueryPerformanceCounter(&end);
        if (room) { capture->room_pick_ticks+=end.QuadPart-start.QuadPart; capture->room_pick_calls++; }
        else { capture->body_pick_ticks+=end.QuadPart-start.QuadPart; capture->body_pick_calls++; }
    }
    return result;
}

static void liquid_contact_slow_report(DWORD now, const liquid_native_freeze_capture_t *capture,
                                      LONGLONG elapsed, double scale)
{
    static DWORD last;
    if (elapsed*scale < 8 || (last && now-last<1000)) return;
    last=now;
    LONGLONG accounted=capture->body_pick_ticks+capture->room_pick_ticks+
        capture->room_extract_ticks+capture->room_clip_ticks;
    double other=(elapsed-accounted)*scale;
    log_line("liquid slow contact update total_ms=%.3f body_pick_ms=%.3f body_calls=%u room_pick_ms=%.3f room_calls=%u room_extract_ms=%.3f room_clip_ms=%.3f remaining_native_ms=%.3f room_triangles=%u distant_rejected=%u recovered=%u",
        elapsed*scale,capture->body_pick_ticks*scale,capture->body_pick_calls,
        capture->room_pick_ticks*scale,capture->room_pick_calls,
        capture->room_extract_ticks*scale,capture->room_clip_ticks*scale,other>0?other:0,
        capture->room_triangles_tested,capture->room_triangles_distant,capture->room_triangles_recovered);
}
