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
static void check_native_sites(const char *path)
{
    FILE *f=fopen(path,"rb"); IMAGE_DOS_HEADER dos; IMAGE_NT_HEADERS nt;
    IMAGE_SECTION_HEADER sections[32]; BYTE bytes[6];
    unsigned int sites[3]={0x1d3f00,0x1d40c6,0x1d4360};
    CHECK(f && fread(&dos,sizeof(dos),1,f)==1 && !fseek(f,dos.e_lfanew,SEEK_SET));
    CHECK(fread(&nt,sizeof(nt),1,f)==1 && nt.FileHeader.TimeDateStamp==TK17_EXE_TIMESTAMP);
    CHECK(nt.FileHeader.NumberOfSections<=32);
    CHECK(fread(sections,sizeof(*sections),nt.FileHeader.NumberOfSections,f)==nt.FileHeader.NumberOfSections);
    for(int i=0;i<3;i++) {
        int found=0;
        for(int j=0;j<nt.FileHeader.NumberOfSections;j++) {
            IMAGE_SECTION_HEADER *s=&sections[j];
            if(sites[i]<s->VirtualAddress || sites[i]+6>s->VirtualAddress+s->SizeOfRawData) continue;
            CHECK(!fseek(f,s->PointerToRawData+sites[i]-s->VirtualAddress,SEEK_SET));
            CHECK(fread(bytes,6,1,f)==1 && !memcmp(bytes,"\xff\x15\x94\xa7\x64\x00",6)); found=1;
        }
        CHECK(found);
    }
    fclose(f);
}
int main(int argc,char **argv)
{
    CHECK(argc==2); check_native_sites(argv[1]);
    struct { int count; float p[9]; } vertices={3,{-4,0,-4,4,0,-4,0,0,4}};
    float sphere[4]={0,0,0,0.04f};
    /* A large triangle under the hit: the old three vertex tests all fail. */
    for(int i=0;i<3;i++) CHECK(original_distance(vertices.p+3*i,sphere)>sphere[3]*sphere[3]);
    CHECK(liquid_room_triangle_distance2(sphere,vertices.p,vertices.p+3,vertices.p+6)==0);
    /* Rotated wall and a hit within the radius of a long edge. */
    float wall[9]={0,-4,-4,0,4,-4,0,0,4};
    CHECK(liquid_room_triangle_distance2(sphere,wall,wall+3,wall+6)==0);
    sphere[2]=-4.01f; CHECK(fabsf(liquid_room_triangle_distance2(sphere,vertices.p,vertices.p+3,vertices.p+6)-0.0001f)<1e-6f);
    sphere[2]=0;
    sphere[1]=0.1f; CHECK(fabsf(liquid_room_triangle_distance2(sphere,vertices.p,vertices.p+3,vertices.p+6)-0.01f)<1e-6f);
    sphere[0]=10; CHECK(liquid_room_triangle_distance2(sphere,vertices.p,vertices.p+3,vertices.p+6)>1);
    float a[3]={0,0,0},b[3]={1,0,0},c[3]={2,0,0},p[3]={0.5f,1,0};
    CHECK(liquid_room_triangle_distance2(p,a,b,c)==1); /* degenerate */
    CHECK(liquid_room_triangle_distance2(p,a,a,a)==1.25f);
    c[0]=NAN; CHECK(liquid_room_triangle_distance2(p,a,b,c)==FLT_MAX);
    BYTE *exe=VirtualAlloc(NULL,0x250000,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
    CHECK(exe); unsigned int sites[3]={0x1d3f00,0x1d40c6,0x1d4360};
    uintptr_t iat=(uintptr_t)(exe+0x24a794);
    *(liquid_room_distance_t*)iat=original_distance;
    for(int i=0;i<3;i++) {
        BYTE *site=exe+sites[i]; site[0]=0xff; site[1]=0x15; memcpy(site+2,&iat,4);
        memcpy(site+6,"\x5b\x5d\xc3",3); /* restore run_site's registers */
    }
    exe[sites[2]]=0x90; CHECK(!liquid_room_install_triangle_hooks(exe));
    CHECK(!memcmp(exe+sites[0]+2,&iat,4)); /* all signatures checked before any patch */
    exe[sites[2]]=0xff; CHECK(liquid_room_install_triangle_hooks(exe));
    CHECK(liquid_room_install_triangle_hooks(exe));
    BYTE storage[256]={0}, outer[16]; BYTE *frame=storage+128;
    liquid_native_freeze_capture_t capture={0}; liquid_native_freeze_capture=&capture;
    capture.room_projection_frame=outer; capture.room_projection_target=outer+4;
    *(void**)frame=outer; *(void**)(frame-0x54)=outer+4;
    *(float**)(frame-0x60)=vertices.p;
    cfg.liquids_enabled=cfg.collision_spawn_room_stains=1;
    for(int mode=0;mode<3;mode++) {
        *(int*)(frame-0x10)=mode==2 ? 2 : 0;
        *(int*)(frame-0x0c)=mode==1 ? 999 : 1;
        *(int*)(frame-0x14)=mode==2 ? 0 : 999;
        *(int*)(frame-0x18)=mode==1 ? 1 : 999;
        sphere[0]=sphere[1]=0;
        float result=run_site(frame,exe+sites[mode],vertices.p,sphere,2);
        CHECK(result==0);
        float saved_x87;
        __asm__ __volatile__("fld1");
        volatile float stacked_result=run_site(frame,exe+sites[mode],vertices.p,sphere,2);
        __asm__ __volatile__("fstps %0" : "=m"(saved_x87));
        CHECK(stacked_result==0 && saved_x87==1);
        cfg.collision_spawn_room_stains=0;
        CHECK(run_site(frame,exe+sites[mode],vertices.p,sphere,2)==32);
        cfg.collision_spawn_room_stains=1;
        capture.room_projection_target=NULL;
        CHECK(run_site(frame,exe+sites[mode],vertices.p,sphere,2)==32);
        capture.room_projection_target=outer+4;
        capture.room_projection_frame=NULL;
        CHECK(run_site(frame,exe+sites[mode],vertices.p,sphere,2)==32);
        capture.room_projection_frame=outer;
        vertices.count=2; CHECK(run_site(frame,exe+sites[mode],vertices.p,sphere,2)==32); vertices.count=3;
        sphere[1]=1; CHECK(run_site(frame,exe+sites[mode],vertices.p,sphere,2)==33);
    }
    CHECK(capture.room_triangles_recovered==6);
    /* Full buffer validation rejects a count extending into an unreadable
       page, and is repeated after extraction even when addresses are reused. */
    BYTE *pages=VirtualAlloc(NULL,8192,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE); CHECK(pages);
    float *edge=(float*)(pages+4096-36); memcpy(edge,vertices.p,36); *((int*)edge-1)=3;
    *(float**)(frame-0x60)=edge;
    *(int*)(frame-0x10)=0; *(int*)(frame-0x0c)=1; sphere[1]=0;
    liquid_room_reset_triangle_validation(&capture);
    CHECK(run_site(frame,exe+sites[0],edge,sphere,2)==0);
    DWORD old;
    CHECK(VirtualProtect(pages+4096,4096,PAGE_NOACCESS,&old));
    *((int*)edge-1)=4;
    CHECK(run_site(frame,exe+sites[0],edge,sphere,2)==32);
    *((int*)edge-1)=3;
    liquid_room_reset_triangle_validation(&capture);
    CHECK(!capture.room_checked_frame && !capture.room_checked_sphere && !capture.room_checked_positions);
    CHECK(run_site(frame,exe+sites[0],edge,sphere,2)==0);
    VirtualFree(pages,0,MEM_RELEASE);
    liquid_room_reset_triangle_validation(&capture);
    *(float**)(frame-0x60)=vertices.p;
    /* Deterministic broad-phase comparison against the previous exact rule. */
    unsigned int seed=12345;
    for(int sample=0;sample<30000;++sample) {
        for(int j=0;j<9;++j) {
            seed=1664525u*seed+1013904223u;
            vertices.p[j]=((int)(seed>>16)-32768)/8192.0f;
        }
        for(int j=0;j<3;++j) sphere[j]=vertices.p[j]+(sample%5)*0.03f;
        sphere[3]=(sample%7+1)*0.015f;
        float original=original_distance(vertices.p,sphere);
        float exact=liquid_room_triangle_distance2(sphere,vertices.p,vertices.p+3,vertices.p+6);
        float expected=original<=sphere[3]*sphere[3] ? original :
            exact<=sphere[3]*sphere[3] ? exact : original;
        CHECK(run_site(frame,exe+sites[0],vertices.p,sphere,2)==expected);
    }
    /* Sphere boundary, long edges, degenerate/NaN cases remain conservative. */
    const float floor[9]={-4,0,-4,4,0,-4,0,0,4}; memcpy(vertices.p,floor,sizeof(floor));
    sphere[0]=sphere[2]=0; sphere[3]=0.04f;
    for(int i=-10;i<=10;++i) {
        sphere[1]=sphere[3]+i*1e-8f;
        float exact=liquid_room_triangle_distance2(sphere,vertices.p,vertices.p+3,vertices.p+6);
        float expected=exact<=sphere[3]*sphere[3] ? exact : original_distance(vertices.p,sphere);
        CHECK(run_site(frame,exe+sites[0],vertices.p,sphere,2)==expected);
    }
    VirtualFree(exe,0,MEM_RELEASE);
    puts("PASS: room triangle interior/plane distance, outside/degenerate/nonfinite geometry; all three native call-site signatures and executed bridges; atomic signature rejection; disabled/body/wrong-frame/invalid-array fallback; selection radius unchanged");
    return 0;
}
