/* Standalone test: includes production resolver, maps the real engine without
   running its entry point, and calls its pure local matrix evaluator with
   controlled scene nodes. No game process or installation is modified. */
#include "NC-TK17-Liquids.c"
#include <assert.h>

static void *tip, *joint, *root;
static void *tip_source, *joint_source, *root_source;
static void *native_eval;
static int dirty_updates;

static void *make_object(void)
{
    BYTE *allocation = calloc(1, 0x190);
    BYTE *object = (BYTE*)(((uintptr_t)allocation + 0x2fu) & ~(uintptr_t)15u);
    *(void**)(object - 0x18) = calloc(1, 0x180);
    assert(!((uintptr_t)object & 8));
    return object;
}

static BYTE *add_interface(void *object, unsigned int offset)
{
    BYTE *meta = *(BYTE**)((BYTE*)object - 0x18);
    BYTE *table = calloc(1, 0x220);
    *(unsigned int*)(table + 4) = 1;
    table += 0x20;
    *(BYTE**)(meta + offset) = table;
    return table;
}

static void *THISCALL get_parent(void *object, unsigned int property)
{
    assert(property == 0x03fff043u);
    return *(void**)((BYTE*)object + 0x110);
}

static void *THISCALL get_source(void *object, unsigned int property)
{
    assert(property == 0x01fff043u);
    return *(void**)((BYTE*)object + 0x118);
}

static void THISCALL get_translation(void *object, unsigned int property,
                                      float *out)
{
    assert(property == 0x01fff04au);
    memcpy(out, (BYTE*)object + 0x60, 3 * sizeof(float));
}

static void THISCALL update_local(void *self)
{
    BYTE *source = (BYTE*)self - 8;
    /* Simulate a changed source transform after physics; the native
       evaluator must invoke this rather than use the stale local matrix. */
    ((float*)(source + 0x18))[12] = 4;
    *(int*)(source + 0x5c) = 2;
    dirty_updates++;
}

static void setup(void **object_out, void **source_out, void *parent)
{
    BYTE *node = make_object(), *source = make_object();
    BYTE *table = add_interface(node, 0x10c);
    void **vtable = calloc(2, sizeof(void*));
    void **local_vtable = calloc(2, sizeof(void*));
    float identity[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    *(void**)(table + 0xc0) = get_parent;
    *(void**)(table + 0x40) = get_source;
    add_interface(node, 0x12c);
    add_interface(source, 0x120);
    table = add_interface(source, 0x128);
    *(void**)(table + 0x40) = get_translation;
    *(void**)(node + 0x110) = parent;
    *(void**)(node + 0x118) = source;
    vtable[1] = native_eval;
    *(void***)source = vtable;
    local_vtable[1] = update_local;
    *(void***)(source + 8) = local_vtable;
    memcpy(source + 0x18, identity, sizeof(identity));
    *(int*)(source + 0x5c) = 2;
    /* Poison the old render caches; the resolver must never read them. */
    for (int i = 0; i < 16; ++i) ((float*)(node + 0x78))[i] = NAN;
    *object_out = node;
    *source_out = source;
}

static void *__cdecl find_node(const char *path)
{
    if (!strcmp(path, "Person01Anim:Model01:penis_jointEnd")) return tip;
    if (!strcmp(path, "Person01Anim:Model01:penis_joint03")) return joint;
    return NULL;
}

static void close_to(float a, float b) { assert(fabsf(a - b) < 0.0001f); }

int main(void)
{
    HMODULE sys = LoadLibraryExA(
        "The Klub 17/Binaries/ThriXXX010278-SYS.dll", NULL,
        DONT_RESOLVE_DLL_REFERENCES);
    liquid_emitter_t emitter = {0};
    float position[3], direction[3];
    float *m;
    assert(sys);
    native_eval = (BYTE*)sys + 0xdac70;
    /* Assert the evaluator ABI's verified prologue before executing it. */
    assert(!memcmp(native_eval, "\x55\x8b\xec\x56\x8b\xf1", 6));
    setup(&root, &root_source, (void*)8);
    setup(&joint, &joint_source, root);
    setup(&tip, &tip_source, joint);
    engine_FindObjC = find_node;
    emitter.source_kind = 1;
    emitter.person_index = 1;
    ((float*)((BYTE*)tip_source + 0x60))[0] = 0.04f;
    /* Parent: 90-degree Z rotation, nonuniform scale, world translation. */
    m = (float*)((BYTE*)root_source + 0x18);
    m[0] = 0; m[1] = 2; m[4] = -3; m[5] = 0;
    m[12] = 10; m[13] = 20; m[14] = 30;
    assert(liquid_emitter_transform(&emitter, position, direction));
    close_to(position[0], 10); close_to(position[1], 20.08f);
    close_to(position[2], 30); close_to(direction[1], 1);
    /* Camera invalid or moved/offscreen has no effect on a model source. */
    for (int i = 0; i < 16; ++i) captured_camera_inverse[i] = NAN;
    captured_camera_inverse_valid = 0;
    assert(liquid_emitter_transform(&emitter, position, direction));
    close_to(position[1], 20.08f);
    /* Identity evaluator returns its input pointer, not scratch. */
    *(int*)((BYTE*)joint_source + 0x5c) = 1;
    assert(liquid_emitter_transform(&emitter, position, direction));
    close_to(position[1], 20.08f);
    *(int*)((BYTE*)joint_source + 0x5c) = 2;
    /* Live dirty physics transform is recomputed, not frozen. */
    *(int*)((BYTE*)root_source + 0x5c) = 0;
    assert(liquid_emitter_transform(&emitter, position, direction));
    assert(dirty_updates == 1); close_to(position[0], 4);
    /* Missing person never falls back to another person's/global joint. */
    emitter.person_index = 2;
    assert(!liquid_emitter_transform(&emitter, position, direction));
    emitter.person_index = 1;
    ((float*)((BYTE*)tip_source + 0x60))[0] = 0;
    assert(!liquid_emitter_transform(&emitter, position, direction));
    ((float*)((BYTE*)tip_source + 0x60))[0] = 0.04f;
    *(void**)((BYTE*)root + 0x110) = tip;
    assert(!liquid_emitter_transform(&emitter, position, direction));
    *(void**)((BYTE*)root + 0x110) = (void*)8;
    m[0] = NAN;
    assert(!liquid_emitter_transform(&emitter, position, direction));
    puts("PASS: native evaluator; hierarchy order/scale; camera/cache independence; identity return; dirty physics; missing person; collapsed chain; cycle; invalid matrix");
    return 0;
}
