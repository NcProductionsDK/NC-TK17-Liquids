/* Shared collision ownership regression; no renderer or live game required. */
#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #c); exit(1); } } while (0)

static void setup(void)
{
    int i;
    memset(&cfg, 0, sizeof(cfg));
    memset(liquid_particles, 0, sizeof(liquid_particles));
    memset(liquid_contact_people, 0, sizeof(liquid_contact_people));
    memset(liquid_contact_people_previous, 0, sizeof(liquid_contact_people_previous));
    memset(liquid_contact_people_older, 0, sizeof(liquid_contact_people_older));
    cfg.liquids_enabled = cfg.collision_follow_bodies = 1;
    cfg.particle_limit = 4; cfg.particle_size = 0.005f;
    cfg.collision_surface_hold = 10;
    /* Keep synthetic anchors; a live engine is deliberately not loaded. */
    cfg.collision_enabled = 0;
    liquid_contact_people[0].valid = 1;
    liquid_contact_people[0].anchors[0].valid = 1;
    liquid_contact_people[0].anchors[0].world[2] = -1;
    liquid_contact_people[0].horizontal[0] = 1;
    liquid_contact_people[0].vertical[1] = 1;
    liquid_contact_people[0].depth[2] = 1;
    captured_camera_inverse_valid = 1;
    memset(captured_camera_inverse, 0, sizeof(captured_camera_inverse));
    captured_camera_inverse[0] = captured_camera_inverse[5] =
        captured_camera_inverse[10] = captured_camera_inverse[15] = 1;
    for (i = 0; i < 3; i++) {
        liquid_particle_t *p = &liquid_particles[i];
        p->active = p->collided = p->source_kind = 1;
        p->emission_id = 42; p->spawn_order = i + 1;
        p->position[0] = i * 0.02f; p->position[2] = -1;
        p->age = 0.2f; p->lifetime = 10;
    }
    liquid_particles[0].contact_physx_person = 1;
    liquid_particles[2].contact_physx_person = -1;
}

static void tick(void)
{
    liquid_last_tick = GetTickCount() - 16;
    liquid_simulation_tick();
}

static void test_room_near_body(void)
{
    int connected;
    for (connected = 0; connected <= 1; connected++) {
        setup(); cfg.collision_connect_droplets = connected;
        tick();
        CHECK(liquid_particles[0].contact_person == 1);
        CHECK(liquid_particles[1].contact_person == 0);
        CHECK(liquid_particles[2].contact_person == 0);
        liquid_contact_people[0].anchors[0].world[0] = 0.25f;
        tick();
        CHECK(fabsf(liquid_particles[0].position[0] - 0.25f) < 0.0001f);
        CHECK(fabsf(liquid_particles[1].position[0] - 0.02f) < 0.0001f);
        CHECK(fabsf(liquid_particles[2].position[0] - 0.04f) < 0.0001f);
    }
    puts("PASS: nearby room/unknown impacts stay fixed while confirmed body contact moves; connections on/off");
}

#ifndef LIQUID_BASELINE
static void test_native_confirmation(void)
{
    liquid_native_contact_t contact = {0};
    float hit[3];
    setup();
    contact.emission_id = 42; contact.particle_id = 2;
    memcpy(contact.impact_world, liquid_particles[1].position, sizeof(hit));
    memcpy(hit, contact.impact_world, sizeof(hit));
    /* An unbounded impact/camera ray finds a body behind the room surface. */
    hit[2] -= 0.04f;
    CHECK(!liquid_confirm_particle_model_contact(&contact, hit));
    CHECK(!liquid_particles[1].model_contact_verified);
    CHECK(!liquid_attach_contact_to_body(&liquid_particles[1], 0, 2));
    memcpy(hit, contact.impact_world, sizeof(hit)); hit[0] += 0.01f;
    CHECK(!liquid_confirm_particle_model_contact(&contact, hit));
    hit[0] = NAN; CHECK(!liquid_confirm_particle_model_contact(&contact, hit));
    memcpy(hit, contact.impact_world, sizeof(hit)); hit[0] += 0.001f;
    CHECK(liquid_confirm_particle_model_contact(&contact, hit));
    tick(); CHECK(liquid_particles[1].contact_person == 1);
    liquid_contact_people[0].anchors[0].world[0] = 0.25f;
    tick(); CHECK(fabsf(liquid_particles[1].position[0] - 0.27f) < 0.0001f);
    /* A room classification cannot be overruled by a native model pick. */
    contact.particle_id = 3;
    memcpy(contact.impact_world, liquid_particles[2].position, sizeof(hit));
    memcpy(hit, contact.impact_world, sizeof(hit));
    CHECK(!liquid_confirm_particle_model_contact(&contact, hit));
    liquid_particles[2].model_contact_confirmed = 1;
    liquid_particles[2].model_contact_verified = 1;
    CHECK(!liquid_attach_contact_to_body(&liquid_particles[2], 1, 2));
    tick(); CHECK(!liquid_particles[2].contact_person);
    CHECK(!liquid_particles[2].model_contact_confirmed);
    /* Recycled/dead particles cannot consume another contact's result. */
    contact.particle_id = 99; CHECK(!liquid_confirm_particle_model_contact(&contact, hit));
    contact.particle_id = 3; liquid_particles[2].active = 0;
    CHECK(!liquid_confirm_particle_model_contact(&contact, hit));
    puts("PASS: native hit must match exact impact; body contact still follows; room classification and particle identity preserved");
}
#endif

static int probe_calls;
static int THISCALL probe_model(void *self, const void *geometry, const float *origin,
    const float *direction, void *results, int hidden, unsigned int flags)
{
    static struct { int count; BYTE hit[0x20]; } storage;
    float point[3], t = -1.0f / direction[2];
    int axis;
    (void)self; (void)geometry; (void)hidden; (void)flags;
    probe_calls++;
    for (axis = 0; axis < 3; axis++) point[axis] = origin[axis] + t * direction[axis];
    /* The room droplet's ray sees a model four centimetres behind its impact. */
    if (point[0] > 0.03f) point[2] -= 0.04f;
    storage.count = 1;
    memcpy(storage.hit + 0x14, point, sizeof(point));
    *(void**)results = storage.hit;
    return 0;
}

static void test_individual_verification(void)
{
    void *results = NULL;
    DWORD now = GetTickCount();
    setup();
    liquid_particles[0].contact_physx_person = 0;
    liquid_particles[2].contact_physx_person = 0;
    cfg.collision_spawn_model_stains = 0;
    tramp_AppPick_PickRay = probe_model;
    probe_calls = 0;
    liquid_verify_recent_body_contacts(NULL, NULL, &results, 0, 0, 42, now);
    CHECK(probe_calls == 3);
    CHECK(liquid_particles[0].model_contact_verified);
    CHECK(liquid_particles[1].model_contact_verified);
    CHECK(!liquid_particles[2].model_contact_verified);
    tick();
    liquid_contact_people[0].anchors[0].world[0] = 0.25f;
    tick();
    CHECK(fabsf(liquid_particles[0].position[0] - 0.25f) < 0.0001f);
    CHECK(fabsf(liquid_particles[1].position[0] - 0.27f) < 0.0001f);
    CHECK(fabsf(liquid_particles[2].position[0] - 0.04f) < 0.0001f);
    liquid_verify_recent_body_contacts(NULL, NULL, &results, 0, 0, 42, now);
    CHECK(probe_calls == 3); /* No repeated queries within the throttle interval. */
    liquid_verify_recent_body_contacts(NULL, NULL, &results, 0, 0, 42, now + 16);
    CHECK(probe_calls == 4);
    liquid_verify_recent_body_contacts(NULL, NULL, &results, 0, 0, 42, now + 32);
    liquid_verify_recent_body_contacts(NULL, NULL, &results, 0, 0, 42, now + 48);
    CHECK(probe_calls == 5); /* A persistent room mismatch stops after three probes. */
    tramp_AppPick_PickRay = NULL;
    puts("PASS: every recent body impact is independently checked without decals; room-behind-body rejection and bounded retries");
}

int main(void)
{
    InitializeCriticalSection(&log_lock); InitializeCriticalSection(&contact_lock);
    test_room_near_body();
#ifndef LIQUID_BASELINE
    test_native_confirmation();
    test_individual_verification();
#endif
    DeleteCriticalSection(&contact_lock); DeleteCriticalSection(&log_lock);
    return 0;
}
