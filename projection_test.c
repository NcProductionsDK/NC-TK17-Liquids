/* Exercise the production projection with a wide camera and a stream
   crossing its near plane. No game process is required. */
#include "NC-TK17-Liquids.c"
#include <assert.h>

int main(void)
{
    D3DMATRIX projection = {0};
    D3DVIEWPORT8 viewport = {0, 0, 1600, 900, 0, 1};
    float world[3] = {0.01f, 0, -0.20f};
    float x, y, depth, screen_depth;
    int exact;
    memset(captured_camera_inverse, 0, sizeof(captured_camera_inverse));
    captured_camera_inverse[0] = captured_camera_inverse[5] =
        captured_camera_inverse[10] = captured_camera_inverse[15] = 1;
    captured_camera_inverse_valid = 1;
    projection._11 = 0.5f;
    projection._22 = 0.5f * 1600 / 900;
    projection._33 = -1.001f;
    projection._34 = -1;
    projection._43 = -0.1001f;
    assert(liquid_project_world_d3d11(world, 1600, 900, 1600.0f / 900,
        0.41421356f, 1, &projection, &viewport,
        &x, &y, &depth, &screen_depth, &exact));
    assert(exact && fabsf(x - 820) < 0.01f);
    /* A clipped point must not reappear at the default 45-degree FOV. */
    world[2] = -0.05f;
    if (liquid_project_world_d3d11(world, 1600, 900, 1600.0f / 900,
        0.41421356f, 1, &projection, &viewport,
        &x, &y, &depth, &screen_depth, &exact)) {
        printf("FAIL: near-clipped point reappeared at x=%.3f exact=%d depth=%.3f\n",
               x, exact, screen_depth);
        return 1;
    }
    /* Installations without a captured projection retain their fallback. */
    assert(liquid_project_world_d3d11(world, 1600, 900, 1600.0f / 900,
        0.41421356f, 0, NULL, NULL,
        &x, &y, &depth, &screen_depth, &exact));
    assert(!exact);
    puts("PASS: wide-FOV projection; near clipping cannot switch FOV; unavailable-matrix fallback");
    return 0;
}
