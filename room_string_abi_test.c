/* Run with the game's Binaries directory as argv[1]. Uses the actual engine
   String implementation to check the room query's StringRef boundary. */
#define main room_fixture_main
#include "room_decal_test.c"
#undef main
typedef const void *(THISCALL *string_ref_t)(const void *);
typedef const char *(THISCALL *string_text_t)(const void *);
static string_ref_t native_ref;
static string_text_t native_text;
static engine_string_construct_cstr_t native_construct;
static engine_string_release_t native_release;
static char *live_string;
static void THISCALL construct_room(char **out, const char *text)
{
    native_construct(out,text); live_string=*out;
    CHECK(live_string && native_ref(out)==live_string);
    strings_created++;
}
static void THISCALL release_room(char **out)
{
    CHECK(*out==live_string); native_release(out); live_string=NULL;
    strings_released++;
}
static int THISCALL pick_room(void *self, const void *geometry,
    const float *origin, const float *direction, void *results, int mode, unsigned int version)
{
    CHECK(geometry==live_string);
    CHECK(!strcmp(native_text(geometry),"Room|PoseEditRoom"));
    return room_pick(self,geometry,origin,direction,results,mode,version);
}
int main(int argc, char **argv)
{
    CHECK(argc==2 && SetDllDirectoryA(argv[1]));
    HMODULE sys=LoadLibraryA("ThriXXX010278-SYS.dll");
    CHECK(sys);
    native_construct=(engine_string_construct_cstr_t)GetProcAddress(sys,"??0String@Bionic@@QAE@PBD@Z");
    native_release=(engine_string_release_t)GetProcAddress(sys,"??1String@Bionic@@QAE@XZ");
    native_ref=(string_ref_t)GetProcAddress(sys,"??BString@Bionic@@QBEABVStringRef@1@XZ");
    native_text=(string_text_t)GetProcAddress(sys,"?GetCPtr@StringRef@Bionic@@QBEPBDXZ");
    CHECK(native_construct && native_release && native_ref && native_text);
    setup(-1);
    liquid_room_string_construct=construct_room;
    liquid_room_string_release=release_room;
    tramp_AppPick_PickRay=pick_room;
    CHECK(run(-1)>=0 && pick_calls==1 && !live_string);
    puts("PASS: real SYS String construction, StringRef conversion and text accessor accept room query argument; released after pick");
    return 0;
}
