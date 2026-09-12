#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
static HMODULE test_effects, test_d3d8;
static HMODULE WINAPI test_module(const char *name)
{
    if (name && !_stricmp(name,"d3d8_heffects.dll")) return test_effects;
    if (name && !_stricmp(name,"d3d8.dll")) return test_d3d8;
    return GetModuleHandleA(name);
}
#define GetModuleHandleA test_module
#include "NC-TK17-Liquids.c"
#undef GetModuleHandleA
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while (0)

static BYTE module_images[3][1024];
static HMODULE module_image(int index, DWORD timestamp)
{
    IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER*)module_images[index];
    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS*)(module_images[index]+128);
    dos->e_magic = IMAGE_DOS_SIGNATURE; dos->e_lfanew=128;
    nt->Signature=IMAGE_NT_SIGNATURE; nt->FileHeader.TimeDateStamp=timestamp;
    return (HMODULE)module_images[index];
}

int main(void)
{
    hook5_present_registered=hook5_d3d11_scene_registered=hook5_d3d11_composite_registered=1;
    CHECK(liquid_native_allowed()); /* Loaded extension, no Hook5. */
    test_d3d8=module_image(0,0x12345678);
    CHECK(liquid_native_allowed()); /* System/native D3D8. */
    test_d3d8=module_image(1,0x603cf2b9);
    CHECK(!liquid_native_allowed()); /* Hook5 before first callback. */
    test_d3d8=module_image(2,0x603cf2c4);
    CHECK(!liquid_native_allowed()); /* Supported patched Hook5. */
    test_d3d8=NULL;
    CHECK(liquid_native_allowed());
    test_effects=(HMODULE)1;
    CHECK(!liquid_native_allowed()); /* Effects DLL evidence. */
    test_effects=NULL;
    /* Callback execution also protects unfamiliar Hook5 builds. */
    liquid_hook5_present_callback(NULL);
    CHECK(hook5_frame_seen && !liquid_native_allowed());
    hook5_frame_seen=0; liquid_renderer_tick=hook5_scene_tick=0;
    cfg.enabled=cfg.liquids_enabled=1;
    model_emitters[0].emission_id=10;
    model_emitters[0].start_tick=1000; model_emitters[0].end_tick=10000;
    liquid_report_renderer_health(2500);
    CHECK(liquid_renderer_warning_emission==10);
    model_emitters[0].emission_id=11;
    liquid_renderer_tick=2500;
    liquid_report_renderer_health(2600);
    CHECK(liquid_renderer_warning_emission==10); /* Native frame healthy. */
    test_effects=(HMODULE)1;
    liquid_report_renderer_health(2600);
    CHECK(liquid_renderer_warning_emission==11); /* Hook5 missing scene. */
    model_emitters[0].emission_id=12; hook5_scene_tick=2500;
    liquid_report_renderer_health(2600);
    CHECK(liquid_renderer_warning_emission==11);
    puts("PASS: extension-only native fallback; system D3D8; both Hook5 builds; delayed effects load; actual callback evidence; stalled/healthy diagnostics");
    return 0;
}
