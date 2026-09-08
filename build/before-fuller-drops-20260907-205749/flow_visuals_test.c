/* Numerical/geometry regressions using production helpers, without TK17. */
#include "NC-TK17-Liquids.c"
#include <assert.h>
#undef assert
/* Report failures to the terminal without the legacy CRT's modal dialog. */
#define assert(condition) do { if (!(condition)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #condition); exit(1); \
} } while (0)

static void close_to(float actual, float expected)
{
    if (fabsf(actual - expected) > 0.0001f) {
        fprintf(stderr, "Expected %.7f, got %.7f\n", expected, actual);
        exit(1);
    }
}

static void reset_scene(void)
{
    memset(&cfg, 0, sizeof(cfg));
    memset(liquid_particles, 0, sizeof(liquid_particles));
    memset(model_emitters, 0, sizeof(model_emitters));
    cfg.particle_limit = 64;
    cfg.particle_size = 0.005f;
    cfg.particle_lifetime = 2.0f;
    cfg.speed = 2.0f;
    cfg.model_stream_cohesion = 0.72f;
    model_emitters[0].source_kind = model_emitters[0].person_index = 1;
    model_emitters[0].emission_id = 42;
}

static void test_emitter_sampling(void)
{
    float old_position[3] = {0, 0, 0}, position[3] = {0.12f, 0, 0};
    float old_direction[3] = {1, 0, 0}, direction[3] = {0, 1, 0};
    float p[3], d[3];
    liquid_sample_emitter_birth(old_position, old_direction, position, direction, 1, 0.5f, p, d);
    close_to(p[0], 0.06f);
    close_to(d[0], sqrtf(0.5f)); close_to(d[1], sqrtf(0.5f));
    liquid_sample_emitter_birth(old_position, old_direction, position, direction, 0, 0.5f, p, d);
    close_to(p[0], position[0]); close_to(d[1], 1);
    position[0] = 2; /* Teleport must not create a connecting trail. */
    liquid_sample_emitter_birth(old_position, old_direction, position, direction, 1, 0.25f, p, d);
    close_to(p[0], 2);
    position[0] = 0.12f; direction[0] = -1; direction[1] = 0;
    liquid_sample_emitter_birth(old_position, old_direction, position, direction, 1, 0.5f, p, d);
    close_to(p[0], 0.12f); close_to(d[0], -1);
    puts("PASS: moving/rotating emitter interpolation; missing history, teleport and reversal guards");
}

static void test_stretch_and_material_coordinates(void)
{
    float position[3] = {0}, direction[3] = {1, 0, 0};
    liquid_particle_t *older, *newer;
    float phase, blend;
    int i;
    reset_scene();
    liquid_spawn_particle(&model_emitters[0], position, direction, 0, 1, 0, 0.01f);
    older = &liquid_particles[0];
    liquid_integrate_flight(older, 0.01f);
    liquid_spawn_particle(&model_emitters[0], position, direction, 0, 1, 0, 0.02f);
    newer = &liquid_particles[1];
    assert(newer->stream_neighbor_order == older->spawn_order);
    assert(newer->stream_rest_length > 0);
    liquid_update_stream_stretch(0.01f);
    close_to(liquid_model_breakup_blend(newer), 0);
    phase = liquid_stream_bulge_at(newer->emission_time, newer->pulse_index);
    newer->age = 0.2f;
    older->position[0] = newer->stream_rest_length * 5;
    for (i = 0; i < 20; i++) liquid_update_stream_stretch(0.01f);
    assert(newer->stretch_breakup > 0.99f);
    assert(newer->separation_age >= 0);
    blend = liquid_model_breakup_blend(newer);
    /* An arbitrary camera change and a compressed link cannot reconnect it. */
    captured_camera_inverse[12] = 1000;
    older->position[0] = 0;
    liquid_update_stream_stretch(0.02f);
    close_to(liquid_model_breakup_blend(newer), blend);
    close_to(liquid_stream_bulge_at(newer->emission_time, newer->pulse_index), phase);
    /* Pool reuse must not connect a surviving particle to an unrelated drop. */
    older->spawn_order++;
    newer->stream_stretch = 1; newer->stretch_breakup = 0; newer->separation_age = -1;
    older->position[0] = 100;
    liquid_update_stream_stretch(0.05f);
    close_to(newer->stretch_breakup, 0);
    /* Cohesion still provides the configured upper age bound. */
    newer->age = cfg.model_stream_cohesion;
    close_to(liquid_model_breakup_blend(newer), 1);
    /* Material phase interpolates identically under curve subdivision. */
    close_to(liquid_stream_bulge_at(0.04f + (0.12f - 0.04f) * 0.25f, 2),
             liquid_stream_bulge_at(0.04f + (0.08f - 0.04f) * 0.5f, 2));
    puts("PASS: coherent launch; stretch detachment; no camera/recompression reconnection; pool reuse; stable bulge phase");
}

static int mismatched_primitive;
static int __cdecl plane_query(const float *point, float tolerance,
    nc_tk17_physx_body_hit_v1_t *hit, unsigned int size)
{
    (void)tolerance;
    assert(size == sizeof(*hit));
    hit->person_index = 1;
    hit->primitive_kind = 1;
    hit->node_start = mismatched_primitive && point[0] > 0 ? 4 : 3;
    hit->node_end = -1;
    hit->signed_distance = point[1];
    return 1;
}

static void test_surface_normals(void)
{
    float point[3] = {0}, normal[3];
    nc_tk17_physx_body_hit_v1_t center = {0};
    liquid_depth_snapshot_t *snapshot = calloc(1, sizeof(*snapshot));
    float pixels[25];
    D3D11_MAPPED_SUBRESOURCE mapped = {pixels, 5 * sizeof(float), 0};
    int i, reversed;
    assert(snapshot);
    snapshot->valid = 1;
    plane_query(point, 0, &center, sizeof(center));
    liquid_physx_query_body_collider = plane_query;
    assert(liquid_body_surface_normal(point, &center, normal));
    close_to(normal[0], 0); close_to(normal[1], 1); close_to(normal[2], 0);
    mismatched_primitive = 1;
    assert(!liquid_body_surface_normal(point, &center, normal));
    mismatched_primitive = 0;
    liquid_physx_query_body_collider = NULL;
    assert(!liquid_body_surface_normal(point, &center, normal));
    snapshot->left = snapshot->top = 48;
    snapshot->width = snapshot->height = 5;
    snapshot->scene_width = snapshot->scene_height = 100;
    snapshot->viewport.Width = snapshot->viewport.Height = 100;
    snapshot->camera_inverse[0] = snapshot->camera_inverse[5] =
        snapshot->camera_inverse[10] = snapshot->camera_inverse[15] = 1;
    snapshot->projection._11 = snapshot->projection._22 = 1;
    snapshot->projection._34 = -1;
    for (reversed = 0; reversed < 2; reversed++) {
        float plane_depth;
        snapshot->projection._33 = reversed ? 0.001f : -1.001f;
        snapshot->projection._43 = reversed ? 0.1001f : -0.1001f;
        plane_depth = -snapshot->projection._33 + snapshot->projection._43;
        for (i = 0; i < 25; i++) pixels[i] = plane_depth;
        assert(liquid_unproject_depth_snapshot(snapshot, 50.5f, 50.5f, plane_depth, point));
        assert(liquid_depth_surface_normal(snapshot, &mapped, point, normal));
        close_to(normal[0], 0); close_to(normal[1], 0); close_to(normal[2], 1);
        pixels[13] = reversed ? 0.0011f : 0.9989f;
        assert(!liquid_depth_surface_normal(snapshot, &mapped, point, normal));
        pixels[13] = NAN;
        assert(!liquid_depth_surface_normal(snapshot, &mapped, point, normal));
        assert(liquid_unproject_depth_snapshot(snapshot, 48.5f, 50.5f, plane_depth, point));
        assert(!liquid_depth_surface_normal(snapshot, &mapped, point, normal));
    }
    free(snapshot);
    puts("PASS: body normal; collider seam rejection; normal/reversed depth; silhouette, invalid sample and tile-edge rejection");
}

static int secondary_count(void)
{
    int i, count = 0;
    for (i = 0; i < cfg.particle_limit; i++)
        if (liquid_particles[i].active && liquid_particles[i].secondary) count++;
    return count;
}

static void test_impacts_and_limits(void)
{
    liquid_particle_t parent = {0};
    float surface[3] = {0}, normal[3] = {0, 1, 0};
    float slow_radius, fast_radius, aspect;
    int i;
    reset_scene();
    parent.emission_id = 42; parent.spawn_order = 99;
    parent.size_scale = parent.opacity = 1;
    parent.impact_velocity[1] = -0.1f;
    liquid_set_impact_response(&parent, surface, normal, 1, 1000);
    liquid_impact_footprint(&parent, &slow_radius, &aspect);
    assert(secondary_count() == 0);
    parent.impact_velocity[1] = -4;
    liquid_set_impact_response(&parent, surface, normal, 0, 1000);
    assert(secondary_count() == 0); /* Untrusted normal: footprint only. */
    liquid_set_impact_response(&parent, surface, normal, 1, 1000);
    liquid_impact_footprint(&parent, &fast_radius, &aspect);
    assert(fast_radius > slow_radius);
    assert(secondary_count() == 2);
    for (i = 0; i < 2; i++) {
        liquid_particle_t *p = &liquid_particles[i];
        assert(liquid_vec3_dot(p->velocity, normal) > 0);
        assert(liquid_vec3_dot(p->position, normal) > 0);
        assert(p->satellite && p->lifetime <= 0.45f);
    }
    liquid_set_impact_response(&parent, surface, normal, 1, 1050);
    assert(secondary_count() == 2); /* Cooldown. */
    parent.secondary = 1;
    liquid_set_impact_response(&parent, surface, normal, 1, 1200);
    assert(secondary_count() == 2); /* No recursive spray. */
    parent.secondary = 0;
    for (i = 1; i <= 30; i++)
        liquid_set_impact_response(&parent, surface, normal, 1, 1200 + i * 100);
    assert(secondary_count() == 24);
    /* A glancing hit briefly flattens, then settles without a moving drip. */
    parent.impact_velocity[0] = 4; parent.impact_velocity[1] = -1;
    liquid_set_impact_response(&parent, surface, normal, 1, 5000);
    liquid_impact_footprint(&parent, &fast_radius, &aspect);
    assert(aspect > 1 && aspect <= 1.45f);
    parent.collision_age = 0.1f;
    liquid_impact_footprint(&parent, &fast_radius, &aspect);
    close_to(aspect, 1);
    reset_scene();
    for (i = 0; i < 62; i++) liquid_particles[i].active = 1;
    liquid_set_impact_response(&parent, surface, normal, 1, 6000);
    assert(secondary_count() == 0); /* Reserve slots for the main flow. */
    for (i = 0; i < 62; i++) assert(liquid_particles[i].active);
    puts("PASS: speed/angle impact response; outward spray; cooldown; no recursion; 24-drop cap and main-flow reserve");
}

static void test_rounding(void)
{
    liquid_particle_t p = {0};
    float tx = 10, ty = 20, td = 0.4f, hx = 10, hy = 20;
    p.separation_age = 0.2f; p.age = 0.2f;
    close_to(liquid_droplet_rounding(&p), 0);
    p.age = 0.29f; close_to(liquid_droplet_rounding(&p), 0.5f);
    p.age = 0.4f; close_to(liquid_droplet_rounding(&p), 1);
    liquid_round_droplet_extent(3, &tx, &ty, &td, &hx, &hy, 0.6f);
    close_to(hypotf(hx - tx, hy - ty), 6);
    close_to((tx + hx) * 0.5f, 10); close_to((ty + hy) * 0.5f, 20);
    close_to(td, 0.6f);
    /* Crossing the minimum diameter must not jump the capsule's endpoints. */
    tx = 0; ty = 0; hx = 5.999f; hy = 0; td = 0.4f;
    liquid_round_droplet_extent(3, &tx, &ty, &td, &hx, &hy, 0.6f);
    assert(fabsf(tx) < 0.001f && fabsf(hx - 6.0f) < 0.001f);
    puts("PASS: gradual detached-drop relaxation; round quad has correct diameter, center and depth");
}

int main(void)
{
    test_emitter_sampling();
    test_stretch_and_material_coordinates();
    test_surface_normals();
    test_impacts_and_limits();
    test_rounding();
    return 0;
}
