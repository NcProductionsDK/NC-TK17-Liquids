/* Exercise window messages and cursor ownership without touching the OS cursor. */
#include <windows.h>
static HWND foreground=(HWND)1;
static POINT mouse={100,100};
static HCURSOR cursor=(HCURSOR)123;
static LONG_PTR window_proc;
static int forwarded, cursor_changes;
static DWORD owner_thread;
static HWND WINAPI fake_foreground(void) { return foreground; }
static DWORD WINAPI fake_window_thread(HWND w,DWORD *pid)
{ (void)w; if(pid) *pid=GetCurrentProcessId(); return owner_thread; }
static BOOL WINAPI fake_position(POINT *p) { *p=mouse; return TRUE; }
static BOOL WINAPI fake_client(HWND w,POINT *p) { (void)w;(void)p; return TRUE; }
static BOOL WINAPI fake_rect(HWND w,RECT *r) { (void)w; *r=(RECT){0,0,800,600}; return TRUE; }
static HCURSOR WINAPI fake_cursor(HCURSOR value)
{ HCURSOR old=cursor; cursor=value; ++cursor_changes; return old; }
static LONG WINAPI fake_get_window(HWND w,int index) { (void)w;(void)index; return window_proc; }
static LONG WINAPI fake_set_window(HWND w,int index,LONG value)
{ LONG old=window_proc; (void)w;(void)index; window_proc=value; return old; }
static LRESULT CALLBACK original_window(HWND w,UINT m,WPARAM wp,LPARAM lp)
{ (void)w;(void)m;(void)wp;(void)lp; ++forwarded; return 77; }
#define GetForegroundWindow fake_foreground
#define GetWindowThreadProcessId fake_window_thread
#define GetCursorPos fake_position
#define ScreenToClient fake_client
#define GetClientRect fake_rect
#define SetCursor fake_cursor
#define GetWindowLongA fake_get_window
#define SetWindowLongA fake_set_window
#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static BYTE app[0x4c8], update[0x20], descriptor[0x40];
static struct { int count; void *items[1]; } descriptors;

static void setup(void)
{
    memset(&cfg,0,sizeof(cfg));
    cfg.liquids_enabled=cfg.pov_enabled=1; cfg.particle_limit=8;
    memset(app,0,sizeof(app)); memset(update,0,sizeof(update)); memset(descriptor,0,sizeof(descriptor));
    memset(liquid_particles,0,sizeof(liquid_particles));
    memset(liquid_native_contacts,0,sizeof(liquid_native_contacts));
    memset(liquid_audio_pending,0,sizeof(liquid_audio_pending));
    memset(liquid_audio_voices,0,sizeof(liquid_audio_voices));
    descriptors.count=1; descriptors.items[0]=descriptor;
    *(void**)(update+0x14)=descriptors.items;
    *(void**)(app+0x468)=update; *(int*)(app+0x4c0)=1;
    liquid_begin_emitter(&tool_emitter,2,0,GetTickCount(),10);
    tool_emitter.native_stain_descriptor=descriptor; liquid_tool_native_app=app;
    *(int*)(descriptor+0x10)=2; descriptor[0x2c]=1; *(int*)(descriptor+0x30)=30;
    liquid_pov_cursor_window=NULL; liquid_pov_previous_window_proc=NULL;
    liquid_pov_pointer_hidden=liquid_pov_pointer_requested=0;
    liquid_tool_camera_button=liquid_tool_free_crosshair_valid=0;
    cursor=(HCURSOR)123; foreground=(HWND)1; mouse=(POINT){100,100};
    window_proc=(LONG_PTR)original_window; owner_thread=GetCurrentThreadId();
    forwarded=cursor_changes=0;
}

static void test_pointer(void)
{
    setup(); liquid_pov_cursor_tick();
    CHECK(!cursor && liquid_pov_pointer_hidden && liquid_pov_cursor_window==(HWND)1);
    CHECK(window_proc==(LONG_PTR)liquid_pov_window_proc);
    CHECK(liquid_pov_window_proc((HWND)1,WM_SETCURSOR,0,HTCLIENT)==TRUE && !forwarded);
    CHECK(liquid_pov_window_proc((HWND)1,WM_USER,0,0)==77 && forwarded==1);
    liquid_tool_free_crosshair[0]=-0.75f;
    liquid_tool_free_crosshair[1]=2.0f/3.0f;
    liquid_tool_free_crosshair_valid=1;
    mouse=(POINT){400,300}; /* Already warped before the button message. */
    CHECK(liquid_pov_window_proc((HWND)1,WM_RBUTTONDOWN,0,0)==77);
    CHECK(liquid_tool_camera_button && liquid_tool_free_crosshair_valid);
    CHECK(liquid_tool_free_crosshair[0]==-0.75f);
    CHECK(liquid_pov_window_proc((HWND)1,WM_RBUTTONUP,0,0)==77 && !liquid_tool_camera_button);
    liquid_pov_window_proc((HWND)1,WM_RBUTTONDOWN,0,0);
    liquid_pov_window_proc((HWND)1,WM_CAPTURECHANGED,0,0); CHECK(!liquid_tool_camera_button);
    /* Look-around and orbit share aim, including overlapping button holds. */
    for (int order=0; order<2; ++order) {
        UINT first=order ? WM_RBUTTONDOWN : WM_LBUTTONDOWN;
        UINT second=order ? WM_LBUTTONDOWN : WM_RBUTTONDOWN;
        UINT first_up=order ? WM_RBUTTONUP : WM_LBUTTONUP;
        UINT second_up=order ? WM_LBUTTONUP : WM_RBUTTONUP;
        CHECK(liquid_pov_window_proc((HWND)1,first,0,0)==77);
        CHECK(liquid_tool_camera_button==(order ? MK_RBUTTON : MK_LBUTTON));
        liquid_pov_window_proc((HWND)1,second,0,0);
        CHECK(liquid_tool_camera_button==(MK_RBUTTON|MK_LBUTTON));
        liquid_pov_window_proc((HWND)1,first_up,0,0);
        CHECK(liquid_tool_camera_button==(order ? MK_LBUTTON : MK_RBUTTON));
        CHECK(liquid_tool_free_crosshair_valid && liquid_tool_free_crosshair[0]==-0.75f);
        liquid_pov_window_proc((HWND)1,second_up,0,0);
        CHECK(!liquid_tool_camera_button);
    }
    liquid_pov_window_proc((HWND)1,WM_LBUTTONDBLCLK,0,0);
    CHECK(liquid_tool_camera_button==MK_LBUTTON);
    liquid_pov_window_proc((HWND)1,WM_RBUTTONDBLCLK,0,0);
    CHECK(liquid_tool_camera_button==(MK_LBUTTON|MK_RBUTTON));
    liquid_pov_window_proc((HWND)1,WM_CANCELMODE,0,0);
    CHECK(!liquid_tool_camera_button);
    liquid_pov_window_proc((HWND)1,WM_LBUTTONDOWN,0,0);
    liquid_pov_window_proc((HWND)1,WM_CAPTURECHANGED,0,0);
    CHECK(!liquid_tool_camera_button);
    liquid_pov_window_proc((HWND)1,WM_LBUTTONDOWN,0,0);
    /* A different game cursor gets restored instead of hardcoding the arrow. */
    cursor=(HCURSOR)456; liquid_pov_cursor_tick(); CHECK(!cursor);
    liquid_pov_window_proc((HWND)1,WM_ACTIVATEAPP,0,0); CHECK(cursor==(HCURSOR)456);
    CHECK(!liquid_tool_camera_button && !liquid_tool_free_crosshair_valid);
    foreground=(HWND)2; liquid_pov_cursor_tick(); CHECK(cursor==(HCURSOR)456);
    CHECK(liquid_pov_window_proc((HWND)1,WM_SETCURSOR,0,HTCLIENT)==77);
    foreground=(HWND)1; liquid_pov_cursor_tick(); CHECK(!cursor);
    liquid_pov_window_proc((HWND)1,WM_SETCURSOR,0,HTCAPTION); CHECK(cursor==(HCURSOR)456);
    mouse.x=900; liquid_pov_cursor_tick(); CHECK(cursor==(HCURSOR)456);
    mouse.x=100; liquid_pov_cursor_tick(); CHECK(!cursor);
    cfg.pov_enabled=0; liquid_pov_cursor_tick(); CHECK(cursor==(HCURSOR)456);
    cfg.pov_enabled=1; liquid_pov_cursor_tick(); CHECK(!cursor);
    tool_emitter.end_tick=GetTickCount()-1;
    CHECK(!liquid_pov_hold_cleanup(app));
    CHECK(cursor==(HCURSOR)456 && !liquid_pov_pointer_requested);
    CHECK(liquid_pov_window_proc((HWND)1,WM_SETCURSOR,0,HTCLIENT)==77);
    /* New sequence, then window destruction restores and removes subclass. */
    tool_emitter.end_tick=GetTickCount()+1000; liquid_pov_cursor_tick(); CHECK(!cursor);
    liquid_pov_window_proc((HWND)1,WM_NCDESTROY,0,0);
    CHECK(cursor==(HCURSOR)456 && !liquid_pov_cursor_window && window_proc==(LONG_PTR)original_window);
    setup(); cfg.liquids_enabled=0; liquid_pov_cursor_tick(); CHECK(cursor==(HCURSOR)123 && !liquid_pov_cursor_window);
    setup(); owner_thread++; liquid_pov_cursor_tick(); CHECK(cursor==(HCURSOR)123 && !liquid_pov_cursor_window);
    setup(); cursor=NULL; liquid_pov_cursor_tick(); cfg.pov_enabled=0; liquid_pov_cursor_tick(); CHECK(!cursor);
    puts("PASS: hide only active game client; restore saved pointer on completion, disable, focus loss, outside client and destroy; messages forwarded; pre-hidden cursor preserved");
}

static void test_cancel(void)
{
    setup(); unsigned int emission=tool_emitter.emission_id;
    liquid_particles[0].active=liquid_particles[1].active=1;
    liquid_particles[0].source_kind=2; liquid_particles[0].emission_id=emission;
    liquid_particles[1].source_kind=1; liquid_particles[1].emission_id=emission+1;
    liquid_particles[2]=liquid_particles[0]; liquid_particles[2].collided=1;
    liquid_native_contacts[0].state=liquid_native_contacts[1].state=1;
    liquid_native_contacts[0].emission_id=emission; liquid_native_contacts[1].emission_id=emission+1;
    liquid_audio_pending[0].active=liquid_audio_pending[1].active=1;
    liquid_audio_pending[1].person_index=1;
    liquid_audio_voices[0].active=liquid_audio_voices[1].active=1;
    liquid_audio_voices[1].person_index=1;
    last_tool_command_tick=100; liquid_tool_launch_target_valid=1;
    liquid_pov_cursor_tick(); CHECK(!cursor);
    liquid_pov_cancel();
    CHECK(!tool_emitter.active && !tool_emitter.emission_id && !liquid_pov_busy(GetTickCount()));
    CHECK(!liquid_particles[0].active && !liquid_particles[2].active && liquid_particles[1].active);
    CHECK(!liquid_native_contacts[0].state && liquid_native_contacts[1].state==1);
    CHECK(!liquid_audio_pending[0].active && liquid_audio_pending[1].active);
    CHECK(!liquid_audio_voices[0].active && liquid_audio_voices[1].active);
    CHECK(!last_tool_command_tick && !liquid_tool_launch_target_valid);
    CHECK(!liquid_tool_camera_button && !liquid_tool_free_crosshair_valid);
    CHECK(!*(int*)(descriptor+0x10) && !descriptor[0x2c] && !*(int*)(descriptor+0x30));
    CHECK(cursor==(HCURSOR)123 && !liquid_pov_pointer_requested);
    liquid_pov_cancel(); CHECK(cursor==(HCURSOR)123);
    /* Cancellation during start delay must prevent the first pulse too. */
    setup(); tool_emitter.start_tick=GetTickCount()+2000; liquid_pov_cancel(); CHECK(!tool_emitter.active);
    setup(); cfg.pov_enabled=0; liquid_pov_cancel(); CHECK(tool_emitter.active && *(int*)(descriptor+0x10)==2);
    puts("PASS: cancellation clears POV pulses, flight, contact queue and audio, releases native state, preserves person sources and disabled path, including start delay/repeated cancel");
}
int main(void) { test_pointer(); test_cancel(); return 0; }
