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

int main(void)
{
    InitializeCriticalSection(&log_lock); InitializeCriticalSection(&contact_lock);
    test_room_near_body();
#ifndef LIQUID_BASELINE
    test_native_confirmation();
#endif
    DeleteCriticalSection(&contact_lock); DeleteCriticalSection(&log_lock);
    return 0;
}
