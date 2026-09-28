#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static void (__cdecl *native_delete_array)(void*);
static BYTE types[3][32];
static int released;
static BYTE mesh_storage[128] __attribute__((aligned(16))), geo_storage[128] __attribute__((aligned(16))), uv_storage[128] __attribute__((aligned(16)));
static BYTE mesh_meta[1024],geo_meta[1024],uv_meta[1024],node_table[512],poly_table[640],uv_table[256],usage_table[256];
static void *mesh,*geo,*uv,*arrays[4];
static int primitive_count,usage_value;
static void *THISCALL get_node(void *obj,unsigned int member) { CHECK(obj==mesh && member==0x01fff043); return geo; }
static int array_slot(void *obj,unsigned int member) {
    if(obj==uv) { CHECK(member==0x01fff0a2); return 3; }
    CHECK(obj==geo); if(member==0x06fff076) return 0; if(member==0x07fff076) return 1; CHECK(member==0x04fff076); return 2;
}
static void THISCALL get_array(void *obj,unsigned int member,void **out) {
    int s=array_slot(obj,member); liquid_room_array_release(*out); *out=arrays[s]; InterlockedIncrement((LONG*)((BYTE*)*out-16));
}
static void THISCALL assign_array(void *obj,unsigned int member,const void *data) {
    int s=array_slot(obj,member); InterlockedIncrement((LONG*)((BYTE*)data-16)); liquid_room_array_release(arrays[s]); arrays[s]=(void*)data;
}
static int THISCALL get_count(void *obj,unsigned int member) { CHECK(obj==geo && member==0x05fff076); return 1; }
static void *THISCALL get_item(void *obj,unsigned int member,int i) { CHECK(obj==geo && member==0x05fff076 && i==0); return uv; }
static void THISCALL set_item(void *obj,unsigned int member,int i,void *data) { CHECK(obj==geo && member==0x05fff076 && i==0 && data==uv); }
static int THISCALL get_usage(void *obj,unsigned int member) { CHECK(obj==uv && member==0x01fff0a0); return usage_value; }
static void THISCALL set_usage(void *obj,unsigned int member,int value) { CHECK(obj==uv && member==0x01fff0a0); usage_value=value; }
static void THISCALL set_prim(void *obj,unsigned int member,int value) { CHECK(obj==geo && member==0x02fff076); primitive_count=value; }
static void init_objects(void) {
    mesh=mesh_storage+64; geo=geo_storage+64; uv=uv_storage+64;
    *(void**)((BYTE*)mesh-24)=mesh_meta; *(void**)((BYTE*)geo-24)=geo_meta; *(void**)((BYTE*)uv-24)=uv_meta;
    BYTE *nt=node_table+32,*pt=poly_table+32,*ut=uv_table+32,*vt=usage_table+32;
    *(int*)(nt-28)=*(int*)(pt-28)=*(int*)(ut-28)=*(int*)(vt-28)=1;
    *(void**)(mesh_meta+0x10c)=nt; *(void**)(geo_meta+0x1d8)=pt; *(void**)(uv_meta+0x288)=ut; *(void**)(uv_meta+0x280)=vt;
    *(void**)(nt+64)=get_node; *(void**)(pt+128+4)=set_prim;
    for(int i=4;i<=7;i++) if(i!=5) { *(void**)(pt+i*64)=get_array; *(void**)(pt+i*64+4)=assign_array; }
    *(void**)(pt+320)=get_item; *(void**)(pt+324)=set_item; *(void**)(pt+328)=get_count;
    *(void**)(ut+64)=get_array; *(void**)(ut+68)=assign_array; *(void**)(vt+64)=get_usage; *(void**)(vt+68)=set_usage;
}
static void __attribute__((naked)) run_clip_site(void *frame,void *source,void *site,float *out) {
    __asm__ __volatile__("pushl %ebp\n\tpushl %ebx\n\tpushl %esi\n\t"
        "movl 16(%esp), %ebp\n\tmovl 20(%esp), %ebx\n\tmovl 28(%esp), %edx\n\t"
        "fld1\n\tcall *24(%esp)\n\tfstps (%edx)\n\tpopl %esi\n\tpopl %ebx\n\tpopl %ebp\n\tret\n\t");
}
static void __cdecl delete_test_array(void *p) { released++; native_delete_array(p); }
static void *make_array(int width,int count,const void *data)
{
    int slot=width==4 ? 0 : width==8 ? 1 : 2;
    *(void**)(types[slot]+8)=delete_test_array; *(int*)(types[slot]+16)=width;
    void *a=liquid_room_array_new(count,count,types[slot]); CHECK(a);
    CHECK(*((int*)a-1)==count && *((int*)a-2)==count && *((int*)a-4)==1);
    memcpy(a,data,width*count); return a;
}
static void set_array(void *obj,unsigned int member,int width,int count,const void *data)
{
    liquid_room_array_set_t set=(liquid_room_array_set_t)liquid_room_member(obj,member,4); CHECK(set);
    void *a=make_array(width,count,data); set(obj,member,a); liquid_room_array_release(a);
}
int main(int argc,char **argv)
{
    CHECK(argc==2 && SetDllDirectoryA(argv[1]));
    HMODULE sys=LoadLibraryA("ThriXXX010278-SYS.dll"); CHECK(sys);
    liquid_room_array_new=(liquid_room_array_new_t)GetProcAddress(sys,"?UninitializedNewArrayRef1@Memory@Bionic@@SAPAXHHABVTypeInfo@2@@Z");
    void **empty=(void**)GetProcAddress(sys,"?G_NullArray@Bionic@@3QBXB"); CHECK(empty); liquid_room_empty_array=*empty;
    native_delete_array=(void (__cdecl *)(void*))GetProcAddress(sys,"?UninitializedDeleteArray@Memory@Bionic@@SAXPAX@Z");
    CHECK(liquid_room_array_new && liquid_room_empty_array && native_delete_array);
    init_objects();
    float positions[9]={-4,0,-4,4,0,-4,0,0,4}, normals[9]={0,1,0,0,1,0,0,1,0};
    float uvs[6]={-40,-40,40,-40,0,40}; int ix[3]={0,1,2};
    set_array(geo,0x06fff076,12,3,positions); set_array(geo,0x07fff076,12,3,normals);
    set_array(geo,0x04fff076,4,3,ix); set_array(uv,0x01fff0a2,8,3,uvs);
    liquid_room_int_set_t usage=(liquid_room_int_set_t)liquid_room_member(uv,0x01fff0a0,4); CHECK(usage); usage(uv,0x01fff0a0,0x10000);
    typedef void (THISCALL *set_item_t)(void*,unsigned int,int,void*);
    set_item_t set_item=(set_item_t)liquid_room_member(geo,0x05fff076,4); CHECK(set_item); set_item(geo,0x05fff076,0,uv);
    int before=0,after=0;
    CHECK(liquid_room_clip_mesh(mesh,&before,&after)); CHECK(before==3 && after==6);
    liquid_room_clip_stream_t pos={0},tex={0},norm={0},idx={0};
    CHECK(liquid_room_read_stream(&pos,geo,0x06fff076,3));
    CHECK(liquid_room_read_stream(&tex,uv,0x01fff0a2,2));
    CHECK(liquid_room_read_stream(&norm,geo,0x07fff076,3));
    CHECK(liquid_room_read_stream(&idx,geo,0x04fff076,1));
    CHECK(liquid_native_diag_array_count(pos.input,12)==6);
    double area=0;
    for(int i=0;i<6;i++) {
        float *p=(float*)pos.input+3*i,*t=(float*)tex.input+2*i,*n=(float*)norm.input+3*i;
        CHECK(t[0]>=0 && t[0]<=1 && t[1]>=0 && t[1]<=1);
        CHECK(fabsf(p[0]-t[0]*0.1f)<1e-5f && fabsf(p[2]-t[1]*0.1f)<1e-5f && fabsf(p[1])<1e-6f);
        CHECK(n[0]==0 && n[1]==1 && n[2]==0 && ((int*)idx.input)[i]==i);
    }
    for(int i=0;i<6;i+=3) { float *a=(float*)tex.input+2*i,*b=a+2,*c=a+4; area+=fabs((b[0]-a[0])*(c[1]-a[1])-(b[1]-a[1])*(c[0]-a[0]))*0.5; }
    CHECK(fabs(area-1)<1e-6);
    liquid_room_array_release(pos.input); liquid_room_array_release(tex.input); liquid_room_array_release(norm.input); liquid_room_array_release(idx.input);
    CHECK(released>=4);
    /* Check the exact supported EXE hook instruction and execute its bridge. */
    char exe_path[MAX_PATH]; snprintf(exe_path,sizeof(exe_path),"%s/TK17-158.001.exe",argv[1]);
    HMODULE exe=LoadLibraryExA(exe_path,NULL,DONT_RESOLVE_DLL_REFERENCES); CHECK(exe);
    BYTE expected[5]={0x8b,0x75,0xf0,0x8b,0xc6}; CHECK(!memcmp((BYTE*)exe+0x1f27c4,expected,5));
    BYTE *site=VirtualAlloc(NULL,64,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE); CHECK(site);
    memcpy(site,expected,5); site[5]=0xc3;
    CHECK(install_inline_hook(site,liquid_room_clip_bridge,5,&liquid_room_clip_trampoline));
    BYTE frame_storage[128]={0}; void *frame=frame_storage+64; *(void**)((BYTE*)frame-16)=mesh;
    liquid_native_freeze_capture_t capture={0}; capture.room_projection_frame=frame; capture.room_projection_target=geo;
    liquid_native_freeze_capture=&capture; float sentinel=0;
    int old_released=released;
    run_clip_site(frame,uv,site,&sentinel); CHECK(sentinel==1 && released==old_released); /* wrong source */
    run_clip_site(frame,geo,site,&sentinel); CHECK(sentinel==1 && released>old_released && primitive_count>0);
    liquid_native_freeze_capture=NULL;
    VirtualFree(site,0,MEM_RELEASE); FreeLibrary(exe);
    /* Invalid geometry never reaches the renderer as a full floor triangle. */
    int invalid[3]={0,1,999}; set_array(geo,0x04fff076,4,3,invalid);
    CHECK(!liquid_room_clip_mesh(mesh,&before,&after) && primitive_count==0);
    CHECK(liquid_native_diag_array_count(arrays[2],4)==0);
    float outside[6]={2,2,3,2,2,3}; liquid_room_clip_vertex_t polygon[12];
    CHECK(!liquid_room_clip_triangle(outside,polygon)); outside[0]=NAN; CHECK(!liquid_room_clip_triangle(outside,polygon));
    for(int i=0;i<4;i++) liquid_room_array_release(arrays[i]);
    puts("PASS: real SYS arrays with fixture property dispatch; large floor clipped to one UV tile, interpolated streams, valid indices/area; native hook bytes/executed bridge/x87; source isolation; invalid geometry suppressed");
    return 0;
}
