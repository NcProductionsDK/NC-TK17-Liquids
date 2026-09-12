#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL %d: %s\n", __LINE__, #c); exit(1); } } while (0)

typedef struct pointer_array_t { int count; void *items[8]; } pointer_array_t;
static BYTE update[0x20], groups[2][0x20], descriptors[2][0x40];
static pointer_array_t group_array, descriptor_array, controls[2];
static BYTE meta[0x240], dispatch_storage[0x80];
static BYTE *dispatch = dispatch_storage + 0x20;
static void *objects[6], *wrappers[6];
static float evaluated[6];
static int setter_calls;

static void THISCALL fake_set_weight(void *object, unsigned int member, float weight)
{
    int i;
    CHECK(member == 0x01fff08e);
    setter_calls++;
    /* Like SYS+0xd7cb0, unchanged raw weights do not notify consumers. */
    if (*(float*)((BYTE*)object + 0x10) == weight) return;
    *(float*)((BYTE*)object + 0x10) = weight;
    for (i = 0; i < 6; i++) if (objects[i] == object) evaluated[i] = weight;
}

static void append(int group, int object)
{
    controls[group].items[controls[group].count++] = wrappers[object];
}

static void confirmed_pick(int descriptor)
{
    liquid_native_finish_frozen_capture();
    liquid_native_begin_frozen_capture(descriptors[descriptor]);
}

static void native_pick(int descriptor, int native_result, int exact_contact)
{
    struct { int count; BYTE data[0x20]; } pick = {0};
    void *results = pick.data;
    liquid_native_contact_t contact = {0};
    float hit[3] = {exact_contact ? 0.0f : 0.04f, 0, -1};
    liquid_particle_t *particle = &liquid_particles[0];
    int confirmed;
    cfg.particle_limit = 1; cfg.particle_size = 0.0035f;
    captured_camera_inverse_valid = 1;
    memset(captured_camera_inverse, 0, sizeof(captured_camera_inverse));
    captured_camera_inverse[0] = captured_camera_inverse[5] =
        captured_camera_inverse[10] = captured_camera_inverse[15] = 1;
    memset(particle, 0, sizeof(*particle));
    particle->active = particle->collided = 1;
    particle->emission_id = contact.emission_id = 42;
    particle->spawn_order = contact.particle_id = 1;
    contact.impact_world[2] = -1;
    pick.count = 1;
    memcpy(pick.data + 0x14, hit, sizeof(hit));
    liquid_native_finish_frozen_capture();
    confirmed = liquid_native_track_custom_stain_pick(
        descriptors[descriptor], &contact, native_result, &results);
    CHECK(confirmed == (native_result >= 0 && exact_contact));
    CHECK(particle->model_contact_verified == confirmed);
    if (native_result >= 0 && !cfg.collision_native_decal_drip)
        CHECK(liquid_native_freeze_capture->group == groups[descriptor]);
    else
        CHECK(!liquid_native_freeze_capture->group);
}

static void THISCALL fake_native_update(void *self, void *context, void *receiver, void *result)
{
    (void)context; (void)receiver; (void)result;
    CHECK(self == update);
    /* Native expiration happens BEFORE the first pick. Net control count
       stays unchanged, and no model emitter knows a descriptor at entry. */
    controls[0].count = 0;
    /* Replay the live failure: result=9, count=1, body confirmation rejected.
       TK17 still creates a stain for this nonnegative native pick result. */
    native_pick(0, 9, 0); append(0, 1);
    native_pick(1, 0, 1); append(1, 2);
    /* A later ordinary native pick must not be included in the capture. */
    liquid_native_finish_frozen_capture(); append(1, 3);
    /* A native miss stays a miss even with stale, matching result data. */
    native_pick(0, -1, 1);
    native_pick(0, 0, 0); append(0, 4);
}

static void init_fixture(void)
{
    int i;
    memset(&cfg, 0, sizeof(cfg));
    *(int*)(dispatch - 0x1c) = 1;
    *(void**)(meta + 0x238) = dispatch;
    *(void**)(dispatch + 0x44) = (void*)fake_set_weight;
    group_array.count = descriptor_array.count = 2;
    *(void***)(update + 0x18) = group_array.items;
    *(void***)(update + 0x14) = descriptor_array.items;
    for (i = 0; i < 2; i++) {
        group_array.items[i] = groups[i]; descriptor_array.items[i] = descriptors[i];
        *(int*)(descriptors[i] + 0x28) = i;
        *(void***)(groups[i] + 0x10) = controls[i].items;
    }
    for (i = 0; i < 6; i++) {
        BYTE *storage = _aligned_malloc(0xa0, 16);
        CHECK(storage);
        memset(storage, 0, 0xa0);
        objects[i] = storage + 0x20;
        wrappers[i] = _aligned_malloc(16, 16);
        CHECK(wrappers[i]);
        *(void**)wrappers[i] = objects[i];
        *(void**)((BYTE*)objects[i] - 0x18) = meta;
        *(float*)((BYTE*)objects[i] + 0x10) = evaluated[i] = 0.4f;
    }
    append(0, 0);
}

static int invalidations;
static void THISCALL invalidate(void *self) { (void)self; invalidations++; }

static void test_real_engine_setter(void)
{
    HMODULE sys = LoadLibraryExA("The Klub 17/Binaries/ThriXXX010278-SYS.dll",
                                NULL, DONT_RESOLVE_DLL_REFERENCES);
    BYTE expected[] = {0x55, 0x8b, 0xec, 0xd9, 0x45, 0x0c, 0x53, 0x8b, 0xd9};
    void *vtable[11] = {0};
    void **consumer = _aligned_malloc(16, 16);
    void *weak;
    pointer_array_t dependencies = {0};
    CHECK(sys && consumer);
    CHECK(memcmp((BYTE*)sys + 0xd7cb0, expected, sizeof(expected)) == 0);
    vtable[10] = (void*)invalidate; *consumer = vtable; weak = consumer;
    dependencies.count = 1; dependencies.items[0] = &weak;
    *(void**)((BYTE*)objects[5] + 0x18) = dependencies.items;
    *(void**)(dispatch + 0x44) = (BYTE*)sys + 0xd7cb0;
    /* Raw assignment demonstrably omits the native invalidation callback. */
    *(float*)((BYTE*)objects[5] + 0x10) = 1.0f;
    CHECK(invalidations == 0);
    *(float*)((BYTE*)objects[5] + 0x10) = 0.4f;
    CHECK(liquid_native_set_stain_weight(objects[5]));
    CHECK(*(float*)((BYTE*)objects[5] + 0x10) == 1.0f && invalidations == 1);
    *(void**)(dispatch + 0x44) = (void*)fake_set_weight;
    _aligned_free(consumer); FreeLibrary(sys);
}

static void test_retry_outcomes(void)
{
    liquid_native_contact_t contact = {0};
    DWORD now = GetTickCount();
    liquid_particle_t *particle = &liquid_particles[0];
    memset(liquid_native_contacts,0,sizeof(liquid_native_contacts));
    memset(particle,0,sizeof(*particle));
    cfg.particle_limit = 1;
    particle->active = particle->collided = 1;
    particle->emission_id = contact.emission_id = 42;
    particle->spawn_order = contact.particle_id = 1;
    contact.tick = now;
    model_emitters[0].emission_id = 42;
    model_emitters[0].end_tick = now + 1000;
    /* Logged result=12/count=1/body_confirmed=0 must not spawn retries. */
    CHECK(!liquid_native_retry_stain_miss(&contact,now,1,12,1));
    CHECK(!liquid_native_contacts[0].state && !particle->model_contact_verified);
    CHECK(liquid_native_retry_stain_miss(&contact,now,1,-1,0));
    CHECK(liquid_native_contacts[0].state == 1);
    CHECK(liquid_native_contacts[0].retry_count == 1);
    memset(liquid_native_contacts,0,sizeof(liquid_native_contacts));
    memset(model_emitters,0,sizeof(model_emitters));
}

int main(void)
{
    int calls;
    init_fixture();
    test_retry_outcomes();
    test_real_engine_setter();
    tramp_NativeStainUpdate = fake_native_update;
    hook_NativeStainUpdate(update, NULL, NULL, NULL);
    CHECK(!liquid_native_freeze_capture);
    CHECK(liquid_native_frozen_control_limit == 3);
    CHECK(evaluated[1] == 1 && evaluated[2] == 1 && evaluated[4] == 1);
    CHECK(evaluated[0] == 0.4f && evaluated[3] == 0.4f);

    fake_set_weight(objects[1], 0x01fff08e, 0.3f);
    liquid_native_hold_custom_stain_weights(update);
    CHECK(evaluated[1] == 1);
    cfg.collision_native_decal_drip = 1;
    fake_set_weight(objects[1], 0x01fff08e, 0.3f);
    calls = setter_calls;
    liquid_native_hold_custom_stain_weights(update);
    CHECK(evaluated[1] == 0.3f && setter_calls == calls);
    {
        liquid_native_freeze_capture_t capture = {0};
        capture.update = update; liquid_native_freeze_capture = &capture;
        confirmed_pick(1); append(1, 5); liquid_native_finish_frozen_capture();
        CHECK(setter_calls == calls && !capture.group);
        liquid_native_freeze_capture = NULL;
    }
    cfg.collision_native_decal_drip = 0;
    /* Removed controls, even with readable/reused wrappers, stay untouched. */
    controls[0].count = 0;
    liquid_native_hold_custom_stain_weights(update);
    CHECK(evaluated[1] == 0.3f);
    *(void**)wrappers[2] = objects[3];
    liquid_native_hold_custom_stain_weights(update);
    CHECK(evaluated[3] == 0.4f);
    CHECK(liquid_native_frozen_control_limit == 0);
    puts("native decal freeze: PASS (native success with rejected body contact, native miss, real engine invalidation, first contact, multiple groups, expiration, native isolation, toggle, stale controls)");
    return 0;
}
