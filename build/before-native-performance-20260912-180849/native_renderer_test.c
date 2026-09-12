#include "NC-TK17-Liquids.c"
#include <stddef.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #c); exit(1); } } while (0)

static void setup_liquid(void)
{
    liquid_particle_t *p = &liquid_particles[0];
    memset(&cfg, 0, sizeof(cfg)); memset(liquid_particles, 0, sizeof(liquid_particles));
    cfg.liquids_enabled = 1; cfg.particle_limit = 4;
    cfg.particle_size = 0.06f; cfg.stream_opacity = 0.8f; cfg.core_opacity = 0.8f;
    cfg.model_stream_cohesion = 0.5f; cfg.draw_layer = -1;
    p->active = p->satellite = 1; p->source_kind = 1;
    p->position[2] = -1; p->age = 0.3f; p->lifetime = 2;
    p->size_scale = p->opacity = 1;
    captured_camera_inverse_valid = 1;
    memset(captured_camera_inverse, 0, sizeof(captured_camera_inverse));
    captured_camera_inverse[0] = captured_camera_inverse[5] =
        captured_camera_inverse[10] = captured_camera_inverse[15] = 1;
    memset(&liquid_native.projection, 0, sizeof(liquid_native.projection));
    liquid_native.projection._11 = liquid_native.projection._22 = 1;
    liquid_native.projection._33 = -1.001f; liquid_native.projection._34 = -1;
    liquid_native.projection._43 = -0.1001f;
    liquid_native.viewport = (D3DVIEWPORT8){0, 0, 128, 128, 0, 1};
    liquid_native.projection_valid = 1;
}

static unsigned int alpha_sum(void)
{
    unsigned int sum = 0, i;
    for (i = 0; i < liquid_native.width * liquid_native.height; i++) sum += liquid_native.rgba[i * 4 + 3];
    return sum;
}

static void test_compositor(void)
{
    unsigned int i, visible;
    ID3D11BlendState *original;
    setup_liquid();
    CHECK(liquid_native_ensure(128, 128));
    for (i = 0; i < 128 * 128; i++) liquid_native.depth[i] = 1;
    CHECK(liquid_native_composite(D3D11_COMPARISON_LESS_EQUAL));
    visible = alpha_sum(); CHECK(visible > 0);
    original = liquid_d3d11_blend_state;
    CHECK(original != liquid_native.blend); /* Hook5's RGB-only state survives. */
    for (i = 0; i < 128 * 128; i++) liquid_native.depth[i] = 0.5f;
    CHECK(liquid_native_composite(D3D11_COMPARISON_LESS_EQUAL));
    CHECK(alpha_sum() == 0); CHECK(liquid_d3d11_blend_state == original);
    liquid_native.projection._33 = 0.001f;
    liquid_native.projection._43 = 0.1001f;
    for (i = 0; i < 128 * 128; i++) liquid_native.depth[i] = 0;
    CHECK(liquid_native_composite(D3D11_COMPARISON_GREATER_EQUAL));
    CHECK(alpha_sum() > 0);
    for (i = 0; i < 128 * 128; i++) liquid_native.depth[i] = 0.5f;
    CHECK(liquid_native_composite(D3D11_COMPARISON_GREATER_EQUAL));
    CHECK(alpha_sum() == 0);
    CHECK(liquid_native_ensure(64, 96));
    CHECK(liquid_native.width == 64 && liquid_native.height == 96);
    puts("PASS: shared native compositor renders; normal/reversed scene depth occludes; Hook5 blend preserved; resize");
}

int main(void)
{
    liquid_depth_replay_t replay;
    CHECK(offsetof(IDirect3DDevice8Vtbl, Reset) / sizeof(void*) == 14);
    CHECK(offsetof(IDirect3DDevice8Vtbl, Clear) / sizeof(void*) == 36);
    CHECK(offsetof(IDirect3DDevice8Vtbl, DrawPrimitive) / sizeof(void*) == 70);
    CHECK(offsetof(IDirect3DDevice8Vtbl, DrawIndexedPrimitive) / sizeof(void*) == 71);
    CHECK(offsetof(IDirect3DDevice8Vtbl, DrawPrimitiveUP) / sizeof(void*) == 72);
    CHECK(offsetof(IDirect3DDevice8Vtbl, DrawIndexedPrimitiveUP) / sizeof(void*) == 73);
    hook5_d3d11_scene_registered = 1;
    hook5_present_registered = 1;
    CHECK(liquid_native_allowed()); /* Installed extension is not a renderer. */
    hook5_frame_seen = 1;
    CHECK(!liquid_native_allowed());
    /* NULL deliberately verifies that the Hook5 guard precedes API access. */
    liquid_native_gl_frame(); liquid_native_d3d8_frame(NULL);
    liquid_native_replay_begin(NULL, &replay); CHECK(!replay.ready);
    hook5_d3d11_scene_registered = 0; hook5_present_registered = 1;
    CHECK(!liquid_native_allowed());
    liquid_native_gl_frame(); liquid_native_d3d8_frame(NULL);
    hook5_present_registered = 0;
    hook5_frame_seen = 0;
    puts("PASS: registration alone permits native rendering; active Hook5 bypasses native rendering and depth replay");
    test_compositor();
    return 0;
}
