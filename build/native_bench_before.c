#define main native_renderer_tests_main
#include "before-native-performance-20260912-180849/native_renderer_test.c"
#undef main
int main(void) {
    LARGE_INTEGER a, b, frequency;
    unsigned int i, j;
    setup_liquid();
    liquid_native.viewport.Width = 2560; liquid_native.viewport.Height = 1368;
    CHECK(liquid_native_ensure(2560, 1368));
    for (i = 0; i < 70; i++) {
        if (i == 10) QueryPerformanceCounter(&a);
        for (j = 0; j < 2560 * 1368; j++) liquid_native.depth[j] = (i & 1) ? 0.5f : 1.0f;
        CHECK(liquid_native_composite(D3D11_COMPARISON_LESS_EQUAL));
    }
    QueryPerformanceCounter(&b); QueryPerformanceFrequency(&frequency);
    CHECK(alpha_sum() == 0);
    printf("2560x1368 compositor, 60 frames, average %.3f ms\n", (b.QuadPart-a.QuadPart)*1000.0/frequency.QuadPart/60);
    return 0;
}
