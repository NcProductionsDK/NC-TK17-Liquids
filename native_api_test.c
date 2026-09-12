/* Creates hidden windows to exercise the real native graphics APIs. */
#define main native_compositor_test_main
#include "native_renderer_test.c"
#undef main

static HWND test_window(void)
{
    WNDCLASSA cls = {0};
    cls.style = CS_OWNDC; cls.lpfnWndProc = DefWindowProcA;
    cls.hInstance = GetModuleHandleA(NULL); cls.lpszClassName = "NCLiquidNativeTest";
    RegisterClassA(&cls);
    return CreateWindowA(cls.lpszClassName, "Hidden liquid regression", WS_POPUP,
                         0, 0, 128, 128, NULL, NULL, cls.hInstance, NULL);
}

static void test_gl(void)
{
    HWND window = test_window();
    HDC dc = GetDC(window);
    PIXELFORMATDESCRIPTOR pfd = {0};
    HGLRC context;
    BYTE before[4], after[4];
    GLfloat expected_projection[16], actual_projection[16];
    GLint viewport[4];
    int format, i;
    CHECK(window && dc);
    pfd.nSize = sizeof(pfd); pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA; pfd.cColorBits = 32; pfd.cDepthBits = 24;
    format = ChoosePixelFormat(dc, &pfd); CHECK(format);
    CHECK(SetPixelFormat(dc, format, &pfd));
    context = wglCreateContext(dc); CHECK(context); CHECK(wglMakeCurrent(dc, context));
    setup_liquid();
    glViewport(0, 0, 128, 128);
    glClearColor(0.1f, 0.2f, 0.3f, 1); glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); glFrustum(-0.1, 0.1, -0.1, 0.1, 0.1, 100);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity(); glEnable(GL_DEPTH_TEST);
    glDepthRange(0.2, 0.8);
    liquid_native_gl_capture();
    CHECK(liquid_native.projection_valid);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); glOrtho(0, 128, 128, 0, -1, 1);
    glGetFloatv(GL_PROJECTION_MATRIX, expected_projection);
    glViewport(1, 2, 120, 118); /* GUI viewport must be restored. */
    glReadBuffer(GL_BACK); glReadPixels(64, 64, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, before);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    liquid_last_tick = GetTickCount();
    liquid_native_gl_capture(); /* First GUI draw flushes the scene layer. */
    CHECK(!liquid_native.projection_valid);
    CHECK(glGetError() == GL_NO_ERROR);
    {
        GLboolean mask[4]; GLint polygon[2];
        glGetBooleanv(GL_COLOR_WRITEMASK, mask); glGetIntegerv(GL_POLYGON_MODE, polygon);
        CHECK(!mask[0] && !mask[1] && !mask[2] && !mask[3]);
        CHECK(polygon[0] == GL_LINE && polygon[1] == GL_LINE);
    }
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glReadPixels(64, 64, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, after);
    CHECK(after[0] > before[0] + 20);
    {
        const BYTE *src = liquid_native.rgba + (63 * 128 + 64) * 4;
        for (i = 0; i < 3; i++) {
            int expected = (int)(src[i] + before[i] * (1 - src[3] / 255.0f) + 0.5f);
            CHECK(abs((int)after[i] - expected) <= 2);
        }
        CHECK(after[3] == before[3]);
    }
    glGetFloatv(GL_PROJECTION_MATRIX, actual_projection);
    for (i = 0; i < 16; i++) CHECK(fabsf(expected_projection[i] - actual_projection[i]) < 0.00001f);
    glGetIntegerv(GL_VIEWPORT, viewport);
    CHECK(viewport[0] == 1 && viewport[1] == 2 && viewport[2] == 120 && viewport[3] == 118);
    glViewport(0, 0, 128, 128);
    glDepthRange(0, 1);
    glClearColor(0.1f, 0.2f, 0.3f, 1); glClearDepth(0.5);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); glFrustum(-0.1, 0.1, -0.1, 0.1, 0.1, 100);
    liquid_native_gl_capture();
    liquid_last_tick = GetTickCount(); liquid_native_gl_frame();
    glReadPixels(64, 64, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, after);
    CHECK(abs((int)after[0] - before[0]) <= 1);
    wglMakeCurrent(NULL, NULL); wglDeleteContext(context); ReleaseDC(window, dc); DestroyWindow(window);
    puts("PASS: native OpenGL real context renders liquid, respects depth, preserves projection and viewport");
}

static void test_d3d8(D3DMULTISAMPLE_TYPE samples)
{
    HWND window = test_window();
    IDirect3D8 *d3d = Direct3DCreate8(D3D_SDK_VERSION);
    IDirect3DDevice8 *device = NULL;
    IDirect3DSurface8 *color = NULL, *depth = NULL, *readback = NULL;
    D3DPRESENT_PARAMETERS pp = {0};
    D3DDISPLAYMODE mode;
    D3DLOCKED_RECT locked;
    D3DVIEWPORT8 viewport = {0,0,128,128,0.2f,0.8f};
    D3DMATRIX identity = {0};
    DWORD state;
    /* A view-space triangle behind the particle, writing native depth. */
    struct {float x,y,z; DWORD color;} tri[3] = {
        {-2,-2,-2,0xff102030},{0,2,-2,0xff102030},{2,-2,-2,0xff102030}};
    CHECK(window && d3d);
    CHECK(SUCCEEDED(IDirect3D8_GetAdapterDisplayMode(d3d, D3DADAPTER_DEFAULT, &mode)));
    if (samples && (FAILED(IDirect3D8_CheckDeviceMultiSampleType(d3d, 0, D3DDEVTYPE_HAL,
            mode.Format, TRUE, samples)) ||
        FAILED(IDirect3D8_CheckDeviceMultiSampleType(d3d, 0, D3DDEVTYPE_HAL,
            D3DFMT_D24S8, TRUE, samples)))) {
        printf("SKIP: native D3D8 sample count %u is unavailable on this driver\n", samples);
        IDirect3D8_Release(d3d); DestroyWindow(window); return;
    }
    {
        D3DADAPTER_IDENTIFIER8 adapter = {0}; D3DCAPS8 caps = {0};
        HRESULT caps_result = IDirect3D8_GetDeviceCaps(d3d, 0, D3DDEVTYPE_HAL, &caps);
        IDirect3D8_GetAdapterIdentifier(d3d, 0, 0, &adapter);
        printf("D3D8 adapter=%s driver=%s caps=0x%08lx windowed=0x%08lx depth=0x%08lx\n",
            adapter.Description, adapter.Driver, (unsigned long)caps_result,
            (unsigned long)IDirect3D8_CheckDeviceType(d3d, 0, D3DDEVTYPE_HAL, mode.Format, mode.Format, TRUE),
            (unsigned long)IDirect3D8_CheckDepthStencilMatch(d3d, 0, D3DDEVTYPE_HAL, mode.Format, mode.Format, D3DFMT_D24S8));
    }
    pp.Windowed = TRUE; pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pp.BackBufferCount = 1; pp.hDeviceWindow = window;
    pp.BackBufferWidth = pp.BackBufferHeight = 128; pp.BackBufferFormat = mode.Format;
    pp.EnableAutoDepthStencil = TRUE; pp.AutoDepthStencilFormat = D3DFMT_D24S8;
    pp.MultiSampleType = samples;
    {
        HRESULT hr = IDirect3D8_CreateDevice(d3d, 0, D3DDEVTYPE_HAL, window,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &device);
        if (FAILED(hr)) fprintf(stderr, "D3D8 CreateDevice hr=0x%08lx format=%u window=%p\n", (unsigned long)hr, mode.Format, window);
        CHECK(SUCCEEDED(hr));
    }
    setup_liquid();
    liquid_native_patch_d3d8(device);
    IDirect3DDevice8_SetViewport(device, &viewport);
    identity._11 = identity._22 = identity._33 = identity._44 = 1;
    IDirect3DDevice8_SetTransform(device, D3DTS_WORLD, &identity);
    IDirect3DDevice8_SetTransform(device, D3DTS_VIEW, &identity);
    IDirect3DDevice8_SetTransform(device, D3DTS_PROJECTION, &liquid_native.projection);
    IDirect3DDevice8_SetVertexShader(device, D3DFVF_XYZ | D3DFVF_DIFFUSE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_LIGHTING, FALSE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_CULLMODE, D3DCULL_NONE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_ZENABLE, TRUE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_ZWRITEENABLE, TRUE);
    {
        int mirror = liquid_native_ensure_depth8(device);
        IDirect3DSurface8 *rt = NULL, *ds = NULL;
        HRESULT bind = E_FAIL;
        if (mirror) {
            IDirect3DDevice8_GetRenderTarget(device, &rt);
            IDirect3DDevice8_GetDepthStencilSurface(device, &ds);
            bind = IDirect3DDevice8_SetRenderTarget(device, liquid_native_color8, liquid_native_depth8);
            IDirect3DDevice8_SetRenderTarget(device, rt, ds);
            IDirect3DSurface8_Release(rt); IDirect3DSurface8_Release(ds);
        }
        printf("D3D8 native allowed=%d mirror=%d bind=0x%08lx active=%d\n",
            liquid_native_allowed(), mirror, (unsigned long)bind, liquid_has_active_particles());
        if (!mirror) {
            const int formats[] = {70,71,75,80,82,84};
            unsigned int f;
            for (f = 0; f < sizeof(formats)/sizeof(formats[0]); f++) {
                IDirect3DSurface8 *surface = NULL;
                HRESULT hr = IDirect3DDevice8_CreateDepthStencilSurface(device, 128,128,
                    (D3DFORMAT)formats[f], D3DMULTISAMPLE_NONE, &surface);
                HRESULT lock = E_FAIL;
                if (surface) {
                    lock = IDirect3DSurface8_LockRect(surface, &locked, NULL, D3DLOCK_READONLY);
                    if (SUCCEEDED(lock)) IDirect3DSurface8_UnlockRect(surface);
                    IDirect3DSurface8_Release(surface);
                }
                printf("depth format=%d create=0x%08lx lock=0x%08lx\n",formats[f],(unsigned long)hr,(unsigned long)lock);
            }
        }
    }
    CHECK(SUCCEEDED(IDirect3DDevice8_Clear(device, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                                          0xff102030, 1, 0)));
    CHECK(liquid_native_depth8_valid);
    CHECK(SUCCEEDED(IDirect3DDevice8_BeginScene(device)));
    CHECK(SUCCEEDED(IDirect3DDevice8_DrawPrimitiveUP(device, D3DPT_TRIANGLELIST, 1, tri, sizeof(tri[0]))));
    CHECK(SUCCEEDED(IDirect3DDevice8_EndScene(device)));
    CHECK(SUCCEEDED(IDirect3DSurface8_LockRect(liquid_native_depth8, &locked, NULL, D3DLOCK_READONLY)));
    if (liquid_native_depth8_format == (D3DFORMAT)82)
        CHECK(((float*)((BYTE*)locked.pBits + 64 * locked.Pitch))[64] < 1.0f);
    else CHECK(((unsigned short*)((BYTE*)locked.pBits + 64 * locked.Pitch))[64] < 65535);
    IDirect3DSurface8_UnlockRect(liquid_native_depth8);
    IDirect3DDevice8_GetRenderTarget(device, &color); IDirect3DDevice8_GetDepthStencilSurface(device, &depth);
    /* Flush inside a scene when the game transitions to its GUI projection. */
    IDirect3DDevice8_SetTransform(device, D3DTS_PROJECTION, &identity);
    viewport.X = 1; viewport.Y = 2; viewport.Width = 120; viewport.Height = 118;
    IDirect3DDevice8_SetViewport(device, &viewport);
    IDirect3DDevice8_SetRenderState(device, D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    CHECK(SUCCEEDED(IDirect3DDevice8_BeginScene(device)));
    {
        liquid_depth_replay_t replay;
        liquid_last_tick = GetTickCount(); liquid_native_replay_begin(device, &replay);
        CHECK(!replay.ready); CHECK(!liquid_native_depth8_valid);
    }
    CHECK(SUCCEEDED(IDirect3DDevice8_EndScene(device)));
    CHECK(alpha_sum() > 0);
    IDirect3DDevice8_GetRenderState(device, D3DRS_FILLMODE, &state); CHECK(state == D3DFILL_WIREFRAME);
    {
        D3DVIEWPORT8 restored;
        IDirect3DDevice8_GetViewport(device, &restored);
        CHECK(!memcmp(&viewport, &restored, sizeof(viewport)));
    }
    IDirect3DDevice8_GetRenderState(device, D3DRS_ZWRITEENABLE, &state); CHECK(state == TRUE);
    {
        IDirect3DSurface8 *current = NULL;
        IDirect3DDevice8_GetDepthStencilSurface(device, &current); CHECK(current == depth);
        IDirect3DSurface8_Release(current);
    }
    if (!samples) {
    CHECK(SUCCEEDED(IDirect3DDevice8_CreateImageSurface(device, 128, 128, mode.Format, &readback)));
    CHECK(SUCCEEDED(IDirect3DDevice8_CopyRects(device, color, NULL, 0, readback, NULL)));
    CHECK(SUCCEEDED(IDirect3DSurface8_LockRect(readback, &locked, NULL, D3DLOCK_READONLY)));
    if (mode.Format == D3DFMT_X8R8G8B8 || mode.Format == D3DFMT_A8R8G8B8) {
        DWORD pixel = ((DWORD*)((BYTE*)locked.pBits + 64 * locked.Pitch))[64];
        const BYTE *src = liquid_native.rgba + (64 * 128 + 64) * 4;
        int expected = (int)(src[0] + 0x10 * (1 - src[3] / 255.0f) + 0.5f);
        CHECK(((pixel >> 16) & 255) > 0x30);
        CHECK(abs((int)((pixel >> 16) & 255) - expected) <= 2);
    }
    IDirect3DSurface8_UnlockRect(readback);
    IDirect3DSurface8_Release(readback);
    }
    IDirect3DSurface8_Release(color); IDirect3DSurface8_Release(depth);
    CHECK(SUCCEEDED(IDirect3DDevice8_Reset(device, &pp)));
    CHECK(!liquid_native_depth8 && !liquid_native_color8 && !liquid_native.projection_valid);
    CHECK(liquid_native_ensure_depth8(device));
    liquid_native_release_depth8(); IDirect3DDevice8_Release(device); IDirect3D8_Release(d3d);
    DestroyWindow(window);
    printf("PASS: native D3D8 samples=%u replays depth, composites before GUI, preserves scene/state, resets\n", samples);
}

int main(void)
{
    /* Reproduce successful extension registration without a Hook5 renderer. */
    hook5_present_registered = hook5_d3d11_scene_registered =
        hook5_d3d11_composite_registered = 1;
    CHECK(liquid_native_allowed());
    test_gl(); test_d3d8(D3DMULTISAMPLE_NONE); test_d3d8(D3DMULTISAMPLE_2_SAMPLES);
    return 0;
}
