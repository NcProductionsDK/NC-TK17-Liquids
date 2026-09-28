#include <windows.h>
static LONGLONG clock_ticks;
static int clock_calls;
static BOOL WINAPI fake_counter(LARGE_INTEGER *v) { ++clock_calls; v->QuadPart=clock_ticks; return TRUE; }
#define QueryPerformanceCounter fake_counter
#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static int THISCALL fake_pick(void *self,const void *geometry,const float *o,const float *d,void *r,int h,unsigned int v)
{
    CHECK(self==(void*)1 && geometry==(void*)2 && o==(void*)3 && d==(void*)4 && r==(void*)5 && h==6 && v==7);
    clock_ticks+=10000; return 19;
}
static int call_pick(int room) { return liquid_measured_pick(room,(void*)1,(void*)2,(void*)3,(void*)4,(void*)5,6,7); }
int main(void)
{
    liquid_native_freeze_capture_t capture={0};
    liquid_native_freeze_capture=&capture; tramp_AppPick_PickRay=fake_pick;
    cfg.enabled=0; CHECK(call_pick(0)==19 && !clock_calls && !capture.body_pick_calls);
    cfg.enabled=1;
    CHECK(call_pick(0)==19 && clock_calls==2 && capture.body_pick_calls==1 && capture.body_pick_ticks==10000);
    CHECK(call_pick(1)==19 && clock_calls==4 && capture.room_pick_calls==1 && capture.room_pick_ticks==10000);
    liquid_native_freeze_capture=NULL; CHECK(call_pick(1)==19 && clock_calls==4);
    liquid_native_freeze_capture=&capture; tramp_AppPick_PickRay=NULL; CHECK(call_pick(0)==-1 && clock_calls==4);
    InitializeCriticalSection(&log_lock); log_ready=1;
    strcpy(log_path,"contact-perf-fixture.log"); FILE *f=fopen(log_path,"wb"); CHECK(f); fclose(f);
    capture.room_extract_ticks=3000; capture.room_clip_ticks=1000;
    capture.room_triangles_tested=20000; capture.room_triangles_distant=19980; capture.room_triangles_recovered=20;
    liquid_contact_slow_report(1000,&capture,25000,0.001);
    liquid_contact_slow_report(1100,&capture,25000,0.001); /* throttled */
    liquid_contact_slow_report(2000,&capture,7000,0.001); /* too fast */
    liquid_contact_slow_report(2100,&capture,25000,0.001);
    liquid_flush_log(1);
    f=fopen(log_path,"rb"); CHECK(f); char line[2048]; int lines=0;
    while(fgets(line,sizeof(line),f)) {
        CHECK(strstr(line,"body_pick_ms=10.000 body_calls=1 room_pick_ms=10.000 room_calls=1"));
        CHECK(strstr(line,"room_extract_ms=3.000 room_clip_ms=1.000 remaining_native_ms=1.000"));
        CHECK(strstr(line,"room_triangles=20000 distant_rejected=19980 recovered=20")); ++lines;
    }
    CHECK(lines==2); fclose(f); DeleteCriticalSection(&log_lock);
    puts("PASS: timing preserves picker args/result; separate body/room accounting; disabled/no-capture skips clocks; slow threshold and one-second limit; extraction/clip/residual fields");
    return 0;
}
