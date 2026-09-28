/* Exercise the OS-cursor capture branch without moving the user's cursor. */
#include <windows.h>
static int cursor_calls, cursor_available=1, own_window=1, right_down, left_down;
static SHORT WINAPI cursor_key(int key)
{ return ((key==VK_RBUTTON && right_down) || (key==VK_LBUTTON && left_down)) ? (SHORT)0x8000 : 0; }
static POINT desktop_cursor={-640,440};
static HWND WINAPI cursor_window(void) { return (HWND)1; }
static DWORD WINAPI cursor_process(HWND window,DWORD *pid)
{ (void)window; *pid=own_window ? GetCurrentProcessId() : 0; return 1; }
static BOOL WINAPI cursor_position(POINT *point)
{ ++cursor_calls; *point=desktop_cursor; return cursor_available; }
static BOOL WINAPI cursor_client(HWND window,POINT *point)
{ (void)window; point->x+=2560; point->y-=80; return TRUE; }
static BOOL WINAPI cursor_rect(HWND window,RECT *rect)
{ (void)window; *rect=(RECT){0,0,2560,1440}; return TRUE; }
#define GetForegroundWindow cursor_window
#define GetWindowThreadProcessId cursor_process
#define GetCursorPos cursor_position
#define ScreenToClient cursor_client
#define GetClientRect cursor_rect
#define GetAsyncKeyState cursor_key
#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); return 1; } } while(0)
int main(void)
{
    BYTE input[12]={0}; void *context[5]={0}, *update=context;
    cfg.liquids_enabled=cfg.pov_enabled=1;
    /* A different monitor, nonzero window origin, and deliberately unrelated
       native input coordinates must still resolve the actual client cursor. */
    *(int*)(input+4)=7; *(int*)(input+8)=9;
    liquid_capture_tool_crosshair(&update,input,100);
    CHECK(liquid_tool_crosshair_source==2 && liquid_tool_crosshair_tick==100);
    CHECK(liquid_tool_crosshair[0]==0.5f && liquid_tool_crosshair[1]==0.5f);
    CHECK(liquid_tool_cursor_pixel.x==1920 && liquid_tool_cursor_pixel.y==360);
    CHECK(cursor_calls==1);
    input[1]=1; liquid_capture_tool_crosshair(&update,input,101);
    CHECK(liquid_tool_crosshair_source==3 && liquid_tool_crosshair[0]==0.5f && liquid_tool_crosshair[1]==0.5f);
    CHECK(cursor_calls==1);
    /* Cursor warp and a long camera gesture must not replace or age out aim. */
    desktop_cursor=(POINT){-1280,800};
    liquid_capture_tool_crosshair(&update,input,3101);
    CHECK(liquid_tool_crosshair_source==3 && liquid_tool_crosshair_tick==3101);
    CHECK(liquid_tool_crosshair[0]==0.5f && liquid_tool_crosshair[1]==0.5f && cursor_calls==1);
    cfg.pov_reach=2.5f; captured_d3d_projection_valid=1;
    captured_d3d_projection=(D3DMATRIX){0};
    captured_d3d_projection._11=1; captured_d3d_projection._22=2;
    captured_d3d_projection._33=-1.001f; captured_d3d_projection._34=-1;
    captured_d3d_projection._43=-0.1001f;
    captured_camera_inverse_valid=1;
    const float origin[]={0,-0.03f,-0.23f}; float direction[3],world[3],sx,sy,depth,sd;
    D3DVIEWPORT8 viewport={0,0,800,600,0,1};
    for(int angle=0;angle<4;++angle) {
        float yaw=angle*0.6f;
        memset(captured_camera_inverse,0,sizeof(captured_camera_inverse));
        captured_camera_inverse[0]=captured_camera_inverse[10]=cosf(yaw);
        captured_camera_inverse[2]=-sinf(yaw); captured_camera_inverse[8]=sinf(yaw);
        captured_camera_inverse[5]=captured_camera_inverse[15]=1;
        captured_camera_inverse[12]=angle; captured_camera_inverse[13]=2;
        CHECK(liquid_tool_crosshair_direction(origin,direction,3101));
        CHECK(liquid_view_to_world_point(liquid_tool_aim_target_view,world));
        CHECK(liquid_project_world_d3d11(world,800,600,4.0f/3,0.414f,1,
            &captured_d3d_projection,&viewport,&sx,&sy,&depth,&sd,NULL));
        CHECK(fabsf(sx-600)<0.001f && fabsf(sy-150)<0.001f);
    }
    /* Release resumes the current pointer immediately. */
    input[1]=0; desktop_cursor=(POINT){-1920,1160};
    liquid_capture_tool_crosshair(&update,input,3102);
    CHECK(liquid_tool_crosshair_source==2 && liquid_tool_crosshair[0]==-0.5f && liquid_tool_crosshair[1]==-0.5f);
    CHECK(cursor_calls==2);
    /* Exercise both buttons before window callbacks and native lock flags. */
    for (int button=0; button<2; ++button) {
        int mask=button ? MK_LBUTTON : MK_RBUTTON;
        desktop_cursor=(POINT){-1920,1160};
        liquid_tool_camera_button=0;
        liquid_capture_tool_crosshair(&update,input,3102);
        int reads=cursor_calls;
        desktop_cursor=(POINT){-1280,800};
        right_down=!button; left_down=button;
        liquid_capture_tool_crosshair(&update,input,3103);
        CHECK(liquid_tool_crosshair_source==3 && liquid_tool_crosshair[0]==-0.5f && cursor_calls==reads);
        liquid_tool_begin_camera_gesture(mask);
        liquid_tool_begin_camera_gesture(mask);
        CHECK(cursor_calls==reads && liquid_tool_camera_button==mask);
        /* Capture change must not expose the warped cursor while held. */
        liquid_tool_camera_button=0;
        liquid_capture_tool_crosshair(&update,input,6104);
        CHECK(liquid_tool_crosshair_source==3 && liquid_tool_crosshair_tick==6104);
        CHECK(liquid_tool_crosshair[0]==-0.5f && liquid_tool_crosshair[1]==-0.5f && cursor_calls==reads);
        right_down=left_down=0; desktop_cursor=(POINT){-1600,620};
        liquid_capture_tool_crosshair(&update,input,6105);
        CHECK(liquid_tool_crosshair_source==2 && liquid_tool_crosshair[0]==-0.25f && liquid_tool_crosshair[1]==0.25f);
        CHECK(cursor_calls==reads+1);
        desktop_cursor=(POINT){-1280,800}; liquid_tool_begin_camera_gesture(mask);
        liquid_capture_tool_crosshair(&update,input,6106);
        CHECK(liquid_tool_crosshair_source==3 && liquid_tool_crosshair[0]==-0.25f && liquid_tool_crosshair[1]==0.25f);
        CHECK(cursor_calls==reads+1);
    }
    int final_reads=cursor_calls;
    liquid_tool_camera_button=0; liquid_tool_free_crosshair_valid=0;
    input[1]=1; liquid_capture_tool_crosshair(&update,input,3107);
    CHECK(liquid_tool_crosshair_source==0 && liquid_tool_crosshair[0]==0 && liquid_tool_crosshair[1]==0);
    float x,y;
    own_window=0; right_down=1; CHECK(!liquid_tool_camera_button_down());
    CHECK(!liquid_read_client_cursor(&x,&y)); CHECK(cursor_calls==final_reads); right_down=0;
    left_down=1; CHECK(!liquid_tool_camera_button_down()); left_down=0;
    own_window=1; cursor_available=0; CHECK(!liquid_read_client_cursor(&x,&y));
    cursor_available=1; desktop_cursor.x=-2561; CHECK(!liquid_read_client_cursor(&x,&y));
    cfg.pov_enabled=0; liquid_capture_tool_crosshair(&update,input,102);
    CHECK(liquid_tool_crosshair_tick==3107 && !liquid_tool_free_crosshair_valid);
    puts("PASS: left/right buttons, pre-callback/pre-lock cursor warp preserves cached aim; long orbit, camera transforms, capture change, release, repeated gestures and focus isolation");
    return 0;
}
