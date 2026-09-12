/* Capacity and CPU-cost regression using the same property-dispatch fixture. */
#define main freeze_fixture_main
#include "native_decal_freeze_test.c"
#undef main

#define LOAD_COUNT 768
static struct { int count; void *items[LOAD_COUNT]; } load_controls;
static void *load_objects[LOAD_COUNT];

/* Recreate the real call site's stack: member, weight, then continuation.
   The stolen instructions load/call the thiscall setter, which pops 8 bytes. */
static void __attribute__((naked)) run_weight_site(
    void *group, int index, void *object, void *table, void *site, float *out)
{
    __asm__ __volatile__(
        "pushl %ebp\n\tpushl %ebx\n\tpushl %esi\n\tpushl %edi\n\t"
        "movl 20(%esp), %edi\n\tmovl 24(%esp), %esi\n\t"
        "movl 28(%esp), %ecx\n\tmovl 32(%esp), %eax\n\t"
        "movl 36(%esp), %edx\n\tmovl 40(%esp), %ebx\n\t"
        "fld1\n\tpushl $1f\n\tpushl $0x3f000000\n\tpushl $0x01fff08e\n\t"
        "jmp *%edx\n\t1:\n\tfstps (%ebx)\n\t"
        "popl %edi\n\tpopl %esi\n\tpopl %ebx\n\tpopl %ebp\n\tret\n\t");
}

static BYTE *install_weight_fixture(void)
{
    BYTE expected[] = {0x8b,0x40,0x44,0xff,0xd0};
    HMODULE exe = LoadLibraryExA("The Klub 17/Binaries/TK17-158.001.exe",
                                 NULL,DONT_RESOLVE_DLL_REFERENCES);
    BYTE *site = VirtualAlloc(NULL,64,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
    CHECK(exe && site);
    CHECK(!memcmp((BYTE*)exe + LIQUID_NATIVE_STAIN_WEIGHT_RVA,expected,sizeof(expected)));
    memcpy(site,(BYTE*)exe + LIQUID_NATIVE_STAIN_WEIGHT_RVA,sizeof(expected));
    site[5] = 0xc3;
    CHECK(install_inline_hook(site,hook_NativeStainWeight,5,&tramp_NativeStainWeight));
    native_stain_weight_hook_installed = 1;
    FreeLibrary(exe);
    return site;
}

int main(void)
{
    int i, frame, lost = 0;
    LARGE_INTEGER start, end, frequency;
    init_fixture();
    *(void***)(groups[0] + 0x10) = load_controls.items;
    for (i = 0; i < LOAD_COUNT; i++) {
        BYTE *storage = _aligned_malloc(0xa0, 16);
        void **wrapper = _aligned_malloc(16, 16);
        CHECK(storage && wrapper);
        memset(storage, 0, 0xa0);
        load_objects[i] = storage + 0x20;
        *(void**)((BYTE*)load_objects[i] - 0x18) = meta;
        *wrapper = load_objects[i];
        load_controls.items[load_controls.count++] = wrapper;
        liquid_native_track_frozen_control(update, groups[0], wrapper);
    }
    /* All live decals must stay frozen after more than 512 creations. */
    for (i = 0; i < LOAD_COUNT; i++)
        *(float*)((BYTE*)load_objects[i] + 0x10) = 0.5f;
    liquid_native_hold_custom_stain_weights(update);
    for (i = 0; i < LOAD_COUNT; i++)
        if (*(float*)((BYTE*)load_objects[i] + 0x10) != 1.0f) lost++;
    printf("Live decals: %d; lost freeze tracking: %d\n", LOAD_COUNT, lost);
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    for (frame = 0; frame < 100; frame++)
        liquid_native_hold_custom_stain_weights(update);
    QueryPerformanceCounter(&end);
    printf("Post-update freeze maintenance: %.3f ms/update\n",
           (double)(end.QuadPart-start.QuadPart)*1000.0/frequency.QuadPart/100);
    CHECK(lost == 0);
    {
        liquid_native_freeze_capture_t capture = {0};
        BYTE *site = install_weight_fixture();
        float sentinel;
        int calls;
        capture.update = update;
        liquid_native_freeze_capture = &capture;
        QueryPerformanceCounter(&start);
        for (frame = 0; frame < 100; frame++) {
            capture.freeze_generation++;
            for (i = LOAD_COUNT-1; i >= 0; i--) {
                run_weight_site(groups[0],i,load_objects[i],dispatch,site,&sentinel);
                CHECK(sentinel == 1);
            }
            calls = setter_calls;
            liquid_native_hold_custom_stain_weights(update);
            CHECK(setter_calls == calls); /* No second setter pass. */
            CHECK(liquid_native_frozen_control_limit == LOAD_COUNT);
        }
        QueryPerformanceCounter(&end);
        printf("Native hook + setter + cleanup: %.3f ms/update (%d decals)\n",
               (double)(end.QuadPart-start.QuadPart)*1000.0/frequency.QuadPart/100,LOAD_COUNT);
        for (i = 0; i < LOAD_COUNT; i++)
            CHECK(*(float*)((BYTE*)load_objects[i] + 0x10) == 1.0f);
        /* Animation toggle and absent TLS must pass the native 0.5 weight. */
        cfg.collision_native_decal_drip = 1;
        run_weight_site(groups[0],0,load_objects[0],dispatch,site,&sentinel);
        CHECK(*(float*)((BYTE*)load_objects[0]+0x10) == 0.5f);
        cfg.collision_native_decal_drip = 0;
        liquid_native_freeze_capture = NULL;
        run_weight_site(groups[0],1,load_objects[1],dispatch,site,&sentinel);
        CHECK(*(float*)((BYTE*)load_objects[1]+0x10) == 0.5f);
        liquid_native_freeze_capture = &capture;
        /* Reused wrapper resolving to a different object remains native. */
        *(void**)load_controls.items[0] = objects[3];
        run_weight_site(groups[0],0,objects[3],dispatch,site,&sentinel);
        CHECK(evaluated[3] == 0.5f);
        /* Only one live tracked entry visited: sweep all other stale entries. */
        capture.freeze_generation++;
        run_weight_site(groups[0],1,load_objects[1],dispatch,site,&sentinel);
        liquid_native_hold_custom_stain_weights(update);
        CHECK(liquid_native_frozen_control_limit == 1);
        CHECK(liquid_native_frozen_controls[0].object == load_objects[1]);
        /* Other updates and ordinary native controls retain native behavior. */
        capture.update = descriptors[0];
        run_weight_site(groups[0],1,load_objects[1],dispatch,site,&sentinel);
        CHECK(*(float*)((BYTE*)load_objects[1]+0x10) == 0.5f);
        liquid_native_freeze_capture = NULL;
        puts("PASS: >512 live controls; real native weight hook/trampoline; x87 preservation; no animation/reset churn; toggle; native isolation; stale pruning");
    }
    return 0;
}
