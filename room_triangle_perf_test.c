#include <windows.h>
static unsigned int queries;
static SIZE_T WINAPI measured_query(LPCVOID p,PMEMORY_BASIC_INFORMATION m,SIZE_T s)
{ ++queries; return VirtualQuery(p,m,s); }
#define VirtualQuery measured_query
#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static float THISCALL original_distance(const float *a,const float *b)
{ float d[3]; for(int i=0;i<3;i++) d[i]=a[i]-b[i]; return liquid_vec3_dot(d,d); }
static float __attribute__((naked)) run_site(void *frame,void *site,const float *vertex,const float *sphere,unsigned int index)
{
    __asm__ __volatile__(
        "pushl %ebp\n\tpushl %ebx\n\tmovl 12(%esp), %ebp\n\t"
        "movl 20(%esp), %ecx\n\tmovl 28(%esp), %ebx\n\t"
        "pushl 24(%esp)\n\tjmp *20(%esp)\n\t");
}
int main(void)
{
    const int triangles=20000;
    int *data=malloc(4+triangles*9*sizeof(float)); CHECK(data);
    *data=triangles*3; float *p=(float*)(data+1);
    for(int t=0;t<triangles;++t) {
        float x=t%1000 ? 10+(t%100) : 0;
        float tri[9]={x-4,0,-4,x+4,0,-4,x,0,4}; memcpy(p+9*t,tri,sizeof(tri));
    }
    BYTE *site=VirtualAlloc(NULL,64,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE); CHECK(site);
    site[0]=0xff; site[1]=0x15; uintptr_t addr=(uintptr_t)&liquid_room_distance_bridge_pointer;
    memcpy(site+2,&addr,4); memcpy(site+6,"\x5b\x5d\xc3",3);
    liquid_room_distance_returns[0]=site+6; liquid_room_original_distance=original_distance;
    BYTE storage[256]={0},outer[16]; BYTE *frame=storage+128;
    liquid_native_freeze_capture_t capture={0}; liquid_native_freeze_capture=&capture;
    capture.room_projection_frame=outer; capture.room_projection_target=outer+4;
    *(void**)frame=outer; *(void**)(frame-0x54)=outer+4; *(float**)(frame-0x60)=p;
    cfg.liquids_enabled=cfg.collision_spawn_room_stains=1; cfg.enabled=0;
    float sphere[4]={0,0,0,0.04f}; LARGE_INTEGER freq,start,end;
    QueryPerformanceFrequency(&freq);
    for(int pass=0;pass<5;++pass) {
        liquid_room_reset_triangle_validation(&capture);
        capture.room_triangles_recovered=0; queries=0;
        QueryPerformanceCounter(&start);
        for(int t=0;t<triangles;++t) {
            *(int*)(frame-0x10)=t*3; *(int*)(frame-0x0c)=t*3+1;
            float result=run_site(frame,site,p+9*t,sphere,t*3+2);
            CHECK(result==(t%1000 ? original_distance(p+9*t,sphere) : 0));
        }
        QueryPerformanceCounter(&end);
        CHECK(capture.room_triangles_recovered==20);
        CHECK(queries<=8); /* Validation scales with buffers, not triangles. */
        printf("pass=%d triangles=%d recovered=%u VirtualQuery=%u elapsed_ms=%.3f\n",pass,triangles,
            capture.room_triangles_recovered,queries,(end.QuadPart-start.QuadPart)*1000.0/freq.QuadPart);
    }
    free(data); VirtualFree(site,0,MEM_RELEASE);
    puts("PASS: executed native ABI bridge returns unchanged selection for near/distant triangles");
    return 0;
}
