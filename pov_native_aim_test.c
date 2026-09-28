/* Execute TK17's real mode-3 reset followed by the native tool-aim bridge. */
#include <windows.h>
static SHORT WINAPI no_key(int key) { (void)key; return 0; }
#define GetAsyncKeyState no_key
#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static BYTE app[0x4c8], update[0x20], descriptor[0x40], context[0x14];
static struct { int count; void *items[1]; } descriptors;
static void setup(void)
{
    cfg.liquids_enabled=cfg.pov_enabled=1; cfg.enabled=0; cfg.particle_limit=8;
    memset(app,0,sizeof(app)); memset(update,0,sizeof(update));
    descriptors.count=1; descriptors.items[0]=descriptor;
    *(void**)(update+0x14)=descriptors.items; *(void**)update=context;
    *(void**)(app+0x468)=update; *(int*)(app+0x4c0)=1;
    *(int*)(app+0x78)=0x12345678;
    liquid_begin_emitter(&tool_emitter,2,0,GetTickCount(),60);
    liquid_tool_native_app=app; tool_emitter.native_stain_descriptor=descriptor;
    liquid_tool_free_crosshair_valid=1; liquid_tool_camera_button=0;
    liquid_tool_free_crosshair[0]=0.5f; liquid_tool_free_crosshair[1]=0.25f;
}
static void __attribute__((naked)) run_site(void *frame,void *app,void *site,void *out)
{
    __asm__ __volatile__(
        "pushl %ebp\n\tpushl %ebx\n\tpushl %esi\n\tpushl %edi\n\t"
        "movl 20(%esp), %ebp\n\tmovl 24(%esp), %ebx\n\tmovl 32(%esp), %edi\n\t"
        "fld1\n\tcall *28(%esp)\n\tfstps (%edi)\n\t"
        "movl %eax, 4(%edi)\n\tmovl %edx, 8(%edi)\n\t"
        "popl %edi\n\tpopl %esi\n\tpopl %ebx\n\tpopl %ebp\n\tret\n\t");
}
int main(int argc,char **argv)
{
    CHECK(argc==2);
    HMODULE exe=LoadLibraryExA(argv[1],NULL,DONT_RESOLVE_DLL_REFERENCES); CHECK(exe);
    const BYTE original[]={0x83,0xbb,0xac,0,0,0,3,0x75,8,0xd9,0xee,0xd9,0x55,0xf0,0xd9,0x5d,0xf8,
                           0x8b,0x43,0x78,0x8d,0x55,0x9c};
    CHECK(!memcmp((BYTE*)exe+0x6ffe3,original,sizeof(original)));
    BYTE *site=VirtualAlloc(NULL,64,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE); CHECK(site);
    memcpy(site,original,sizeof(original)); site[sizeof(original)]=0xc3;
    BYTE storage[160]={0}; void *frame=storage+128;
    float *x=(float*)((BYTE*)frame-16), *y=(float*)((BYTE*)frame-8);
    struct { float sentinel; DWORD eax; void *edx; } out;
    setup(); *x=0.4f; *y=-0.3f; *(int*)(app+0xac)=3;
    run_site(frame,app,site,&out);
    CHECK(*x==0 && *y==0); /* Reproduces the old native centering reset. */
    CHECK(install_inline_hook(site+17,hook_PovToolAim,6,&tramp_PovToolAim));
    *(int*)(app+0xac)=0; *x=0.4f; *y=-0.3f;
    run_site(frame,app,site,&out);
    CHECK(liquid_pov_native_aim_valid && *x==0.4f && *y==-0.3f);
    CHECK(out.sentinel==1 && out.eax==0x12345678 && out.edx==(BYTE*)frame-0x64);
    for(int i=0;i<100;++i) {
        *(int*)(app+0xac)=3; *x=-0.9f; *y=0.7f; /* Camera cursor moves/recenters. */
        run_site(frame,app,site,&out);
        CHECK(*x==0.4f && *y==-0.3f && out.sentinel==1);
        /* Native camera mode holds custom screen aim even between OS messages. */
        CHECK(liquid_pov_camera_look_active(update,app+0x378));
        liquid_capture_tool_crosshair(update,app+0x378,100+i);
        CHECK(liquid_tool_crosshair_source==3 && liquid_tool_crosshair_tick==100+i);
        CHECK(liquid_tool_crosshair[0]==0.5f && liquid_tool_crosshair[1]==0.25f);
    }
    CHECK(!liquid_pov_camera_look_active(context,app+0x378));
    CHECK(!liquid_pov_camera_look_active(update,app+0x3a0));
    *(int*)(app+0xac)=0; *x=-0.2f; *y=0.6f;
    run_site(frame,app,site,&out); CHECK(*x==-0.2f && *y==0.6f);
    *(int*)(app+0xac)=3; run_site(frame,app,site,&out); CHECK(*x==-0.2f && *y==0.6f);
    /* Preserve the working native right-button path. */
    *(int*)(app+0xac)=1; liquid_tool_camera_button=MK_RBUTTON; *x=0.1f; *y=0.2f;
    run_site(frame,app,site,&out); CHECK(*x==0.1f && *y==0.2f);
    *(int*)(app+0xac)=3; run_site(frame,app,site,&out); CHECK(*x==-0.2f && *y==0.6f);
    liquid_tool_camera_button=0;
    /* No cached aim leaks across new emissions, disabled POV, or focus loss. */
    ++tool_emitter.emission_id; run_site(frame,app,site,&out); CHECK(*x==0 && *y==0);
    *(int*)(app+0xac)=0; *x=0.25f; *y=0.5f; run_site(frame,app,site,&out);
    cfg.pov_enabled=0; *(int*)(app+0xac)=3; run_site(frame,app,site,&out); CHECK(*x==0 && *y==0);
    cfg.pov_enabled=1; *(int*)(app+0xac)=0; *x=0.25f; *y=0.5f; run_site(frame,app,site,&out);
    liquid_tool_free_crosshair_valid=0; *(int*)(app+0xac)=3; run_site(frame,app,site,&out); CHECK(*x==0 && *y==0);
    liquid_tool_free_crosshair_valid=1; *(int*)(app+0xac)=0; *x=0.25f; *y=0.5f; run_site(frame,app,site,&out);
    tool_emitter.native_stain_descriptor=NULL; *(int*)(app+0xac)=3; run_site(frame,app,site,&out); CHECK(*x==0 && *y==0);
    VirtualFree(site,0,MEM_RELEASE); FreeLibrary(exe);
    puts("PASS: actual EXE mode-3 reset reproduced; bridge preserves tool aim, x87 and stolen instructions; repeated look, matching stream target, release, right-button isolation, new emission/disable/focus/descriptor guards");
    return 0;
}
