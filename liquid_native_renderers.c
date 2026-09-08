/* Included by NC-TK17-Liquids.c. Native scene renderers share the production
   D3D11 liquid shader/geometry via a transparent offscreen compositor. Hook5
   never enters these paths. Scene depth remains owned by the native renderer. */
typedef HRESULT (WINAPI *liquid_create_d3d11_t)(IDXGIAdapter *, D3D_DRIVER_TYPE,
    HMODULE, UINT, const D3D_FEATURE_LEVEL *, UINT, UINT, ID3D11Device **,
    D3D_FEATURE_LEVEL *, ID3D11DeviceContext **);

static struct {
    ID3D11Device *device;
    ID3D11DeviceContext *context;
    ID3D11Texture2D *color, *readback;
    ID3D11RenderTargetView *target;
    ID3D11BlendState *blend;
    UINT width, height;
    float *depth;
    BYTE *rgba;
    int busy;
    D3DMATRIX projection;
    D3DVIEWPORT8 viewport;
    int projection_valid;
} liquid_native;

static int liquid_native_allowed(void)
{
    return !hook5_present_registered && !hook5_d3d11_scene_registered &&
           !GetModuleHandleA("d3d8_heffects.dll");
}

static void liquid_native_release_targets(void)
{
    if (liquid_native.target) ID3D11RenderTargetView_Release(liquid_native.target);
    if (liquid_native.color) ID3D11Texture2D_Release(liquid_native.color);
    if (liquid_native.readback) ID3D11Texture2D_Release(liquid_native.readback);
    liquid_native.target = NULL;
    liquid_native.color = liquid_native.readback = NULL;
    free(liquid_native.depth); free(liquid_native.rgba);
    liquid_native.depth = NULL; liquid_native.rgba = NULL;
    liquid_native.width = liquid_native.height = 0;
}

static int liquid_native_ensure(UINT width, UINT height)
{
    D3D11_TEXTURE2D_DESC desc = {0};
    if (!width || !height || width > 8192 || height > 8192) return 0;
    if (!liquid_native.device) {
        HMODULE dll = LoadLibraryA("d3d11.dll");
        liquid_create_d3d11_t create = NULL;
        D3D_FEATURE_LEVEL level;
        D3D11_BLEND_DESC blend = {0};
        HRESULT result;
        if (!dll) return 0;
        copy_proc_address(&create, GetProcAddress(dll, "D3D11CreateDevice"), sizeof(create));
        if (!create) { FreeLibrary(dll); return 0; }
        result = create(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0,
            D3D11_SDK_VERSION, &liquid_native.device, &level, &liquid_native.context);
        if (FAILED(result)) result = create(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0,
            NULL, 0, D3D11_SDK_VERSION, &liquid_native.device, &level, &liquid_native.context);
        if (FAILED(result)) { FreeLibrary(dll); return 0; }
        /* Retain the runtime module for the lifetime of its COM objects. */
        blend.RenderTarget[0].BlendEnable = TRUE;
        blend.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        blend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        if (FAILED(ID3D11Device_CreateBlendState(liquid_native.device, &blend,
                                                &liquid_native.blend))) {
            ID3D11DeviceContext_Release(liquid_native.context);
            ID3D11Device_Release(liquid_native.device);
            liquid_native.context = NULL; liquid_native.device = NULL;
            FreeLibrary(dll);
            return 0;
        }
        log_line("native liquid shared compositor created feature_level=0x%x", level);
    }
    if (liquid_native.width == width && liquid_native.height == height &&
        liquid_native.target && liquid_native.readback && liquid_native.depth &&
        liquid_native.rgba) return 1;
    liquid_native_release_targets();
    desc.Width = width; desc.Height = height; desc.MipLevels = desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1; desc.BindFlags = D3D11_BIND_RENDER_TARGET;
    if (FAILED(ID3D11Device_CreateTexture2D(liquid_native.device, &desc, NULL,
                                          &liquid_native.color))) goto fail;
    if (FAILED(ID3D11Device_CreateRenderTargetView(liquid_native.device,
        (ID3D11Resource*)liquid_native.color, NULL, &liquid_native.target))) goto fail;
    desc.BindFlags = 0; desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    if (FAILED(ID3D11Device_CreateTexture2D(liquid_native.device, &desc, NULL,
                                          &liquid_native.readback))) goto fail;
    liquid_native.depth = malloc((size_t)width * height * sizeof(float));
    liquid_native.rgba = malloc((size_t)width * height * 4);
    if (!liquid_native.depth || !liquid_native.rgba) goto fail;
    liquid_native.width = width; liquid_native.height = height;
    return 1;
fail:
    liquid_native_release_targets();
    return 0;
}

static void liquid_native_snapshot(liquid_depth_snapshot_t *snapshot)
{
    int i;
    memset(snapshot, 0, sizeof(*snapshot));
    snapshot->valid = 1;
    snapshot->width = snapshot->scene_width = liquid_native.width;
    snapshot->height = snapshot->scene_height = liquid_native.height;
    snapshot->projection = liquid_native.projection;
    snapshot->viewport = liquid_native.viewport;
    snapshot->viewport.X = snapshot->viewport.Y = 0;
    memcpy(snapshot->camera_inverse, captured_camera_inverse, sizeof(snapshot->camera_inverse));
    for (i = 0; i < cfg.particle_limit; i++) {
        const liquid_particle_t *p = &liquid_particles[i];
        liquid_depth_particle_sample_t *s = &snapshot->particles[i];
        if (!p->active || p->collided || p->source_kind != 1) continue;
        s->valid = 1; s->spawn_order = p->spawn_order; s->emission_id = p->emission_id;
        s->age = p->age; s->has_last_visible = p->has_last_visible;
        memcpy(s->position, p->position, sizeof(s->position));
        memcpy(s->previous, p->previous, sizeof(s->previous));
        memcpy(s->velocity, p->velocity, sizeof(s->velocity));
        memcpy(s->last_visible, p->last_visible, sizeof(s->last_visible));
    }
}

/* Both native APIs provide top-down normalized depth here. The exact same
   contact classifier, spray and attachment code used by Hook5 consumes it. */
static int liquid_native_composite(D3D11_COMPARISON_FUNC comparison)
{
    static liquid_depth_snapshot_t snapshot;
    D3D11_TEXTURE2D_DESC desc = {0};
    D3D11_SUBRESOURCE_DATA initial = {0};
    D3D11_MAPPED_SUBRESOURCE mapped = {0};
    D3D11_DEPTH_STENCIL_DESC depth_state_desc = {0};
    ID3D11Texture2D *depth = NULL;
    ID3D11DepthStencilView *dsv = NULL;
    ID3D11DepthStencilState *state = NULL;
    ID3D11BlendState *saved_blend;
    const float clear[4] = {0};
    UINT y;
    int ok = 0;
    if (!liquid_native.projection_valid || !captured_camera_inverse_valid ||
        !liquid_native.device || !liquid_native.depth || !liquid_native.target) return 0;
    captured_d3d_projection = liquid_native.projection;
    captured_d3d_viewport = liquid_native.viewport;
    captured_d3d_viewport.X = captured_d3d_viewport.Y = 0;
    captured_d3d_projection_valid = 1;
    if (cfg.collision_enabled) {
        D3D11_COMPARISON_FUNC saved_comparison = liquid_d3d11_depth_test_func;
        liquid_d3d11_depth_test_func = comparison;
        liquid_native_snapshot(&snapshot);
        mapped.pData = liquid_native.depth;
        mapped.RowPitch = liquid_native.width * sizeof(float);
        liquid_apply_depth_collisions(&snapshot, &mapped);
        liquid_d3d11_depth_test_func = saved_comparison;
    }
    desc.Width = liquid_native.width; desc.Height = liquid_native.height;
    desc.ArraySize = desc.MipLevels = 1; desc.SampleDesc.Count = 1;
    desc.Format = DXGI_FORMAT_D32_FLOAT; desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    initial.pSysMem = liquid_native.depth;
    initial.SysMemPitch = desc.Width * sizeof(float);
    if (FAILED(ID3D11Device_CreateTexture2D(liquid_native.device, &desc, &initial, &depth)) ||
        FAILED(ID3D11Device_CreateDepthStencilView(liquid_native.device,
            (ID3D11Resource*)depth, NULL, &dsv))) goto cleanup;
    depth_state_desc.DepthEnable = TRUE;
    depth_state_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depth_state_desc.DepthFunc = comparison;
    if (FAILED(ID3D11Device_CreateDepthStencilState(liquid_native.device,
        &depth_state_desc, &state))) goto cleanup;
    if (!liquid_ensure_d3d11_resources(liquid_native.device)) goto cleanup;
    ID3D11DeviceContext_OMSetRenderTargets(liquid_native.context, 1, &liquid_native.target, dsv);
    ID3D11DeviceContext_OMSetDepthStencilState(liquid_native.context, state, 0);
    ID3D11DeviceContext_ClearRenderTargetView(liquid_native.context, liquid_native.target, clear);
    /* Hook5 preserves scene alpha; an independent transparent layer instead
       must accumulate alpha. Restore the Hook5 state object after this call. */
    saved_blend = liquid_d3d11_blend_state;
    liquid_d3d11_blend_state = liquid_native.blend;
    liquid_draw_particles_d3d11_target(NULL, liquid_native.context, liquid_native.target,
                                      dsv, desc.Width, desc.Height);
    liquid_d3d11_blend_state = saved_blend;
    ID3D11DeviceContext_OMSetRenderTargets(liquid_native.context, 0, NULL, NULL);
    ID3D11DeviceContext_CopyResource(liquid_native.context,
        (ID3D11Resource*)liquid_native.readback, (ID3D11Resource*)liquid_native.color);
    if (FAILED(ID3D11DeviceContext_Map(liquid_native.context,
        (ID3D11Resource*)liquid_native.readback, 0, D3D11_MAP_READ, 0, &mapped))) goto cleanup;
    for (y = 0; y < desc.Height; y++)
        memcpy(liquid_native.rgba + (size_t)y * desc.Width * 4,
               (BYTE*)mapped.pData + (size_t)y * mapped.RowPitch, desc.Width * 4);
    ID3D11DeviceContext_Unmap(liquid_native.context, (ID3D11Resource*)liquid_native.readback, 0);
    ok = 1;
cleanup:
    ID3D11DeviceContext_OMSetRenderTargets(liquid_native.context, 0, NULL, NULL);
    ID3D11DeviceContext_OMSetDepthStencilState(liquid_native.context, NULL, 0);
    if (state) ID3D11DepthStencilState_Release(state);
    if (dsv) ID3D11DepthStencilView_Release(dsv);
    if (depth) ID3D11Texture2D_Release(depth);
    return ok;
}

/* OpenGL 1.1 entry points are loaded dynamically, preserving the standalone
   DLL's existing link requirements and the native game's fixed-function API. */
static struct {
    void (APIENTRY *ReadPixels)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void*);
    void (APIENTRY *PixelStorei)(GLenum, GLint);
    void (APIENTRY *PixelTransferf)(GLenum, GLfloat);
    void (APIENTRY *PushClientAttrib)(GLbitfield);
    void (APIENTRY *PopClientAttrib)(void);
    void (APIENTRY *GenTextures)(GLsizei, GLuint*);
    void (APIENTRY *DeleteTextures)(GLsizei, const GLuint*);
    void (APIENTRY *BindTexture)(GLenum, GLuint);
    void (APIENTRY *TexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*);
    void (APIENTRY *TexParameteri)(GLenum, GLenum, GLint);
    void (APIENTRY *TexEnvi)(GLenum, GLenum, GLint);
    void (APIENTRY *TexCoord2f)(GLfloat, GLfloat);
    void (APIENTRY *DepthMask)(GLboolean);
    void (APIENTRY *ColorMask)(GLboolean, GLboolean, GLboolean, GLboolean);
    void (APIENTRY *PolygonMode)(GLenum, GLenum);
    void (APIENTRY *BlendEquation)(GLenum);
    HGLRC (WINAPI *GetCurrentContext)(void);
    void (APIENTRY *GetDoublev)(GLenum, GLdouble*);
    void (APIENTRY *Viewport)(GLint, GLint, GLsizei, GLsizei);
    void (APIENTRY *ActiveTexture)(GLenum);
    void (APIENTRY *UseProgram)(GLuint);
    PROC (WINAPI *GetProcAddress)(LPCSTR);
} liquid_ngl;
static void (APIENTRY *liquid_native_real_glDrawElements)(GLenum, GLsizei, GLenum, const void*);
static void (APIENTRY *liquid_native_real_glDrawArrays)(GLenum, GLint, GLsizei);
static void (APIENTRY *liquid_native_real_glBegin)(GLenum);
static D3D11_COMPARISON_FUNC liquid_native_gl_depth_func = D3D11_COMPARISON_LESS_EQUAL;

static int liquid_native_gl_load(void)
{
    HMODULE dll = GetModuleHandleA("opengl32.dll");
    if (liquid_ngl.GetCurrentContext) return liquid_ngl.GetCurrentContext() != NULL;
    if (!dll || !liquid_resolve_gl_api()) return 0;
#define NGL(name) do { copy_proc_address(&liquid_ngl.name, GetProcAddress(dll, "gl" #name), sizeof(liquid_ngl.name)); if (!liquid_ngl.name) return 0; } while (0)
    NGL(ReadPixels); NGL(PixelStorei); NGL(PixelTransferf); NGL(PushClientAttrib);
    NGL(PopClientAttrib); NGL(GenTextures); NGL(DeleteTextures); NGL(BindTexture);
    NGL(TexImage2D); NGL(TexParameteri); NGL(TexEnvi); NGL(TexCoord2f); NGL(DepthMask); NGL(GetDoublev); NGL(Viewport);
    NGL(ColorMask); NGL(PolygonMode);
#undef NGL
    copy_proc_address(&liquid_ngl.GetCurrentContext, GetProcAddress(dll, "wglGetCurrentContext"), sizeof(liquid_ngl.GetCurrentContext));
    copy_proc_address(&liquid_ngl.GetProcAddress, GetProcAddress(dll, "wglGetProcAddress"), sizeof(liquid_ngl.GetProcAddress));
    return liquid_ngl.GetCurrentContext && liquid_ngl.GetCurrentContext();
}

static void liquid_native_gl_capture(void)
{
    GLfloat p[16]; GLint viewport[4], depth_func, depth_test, depth_write;
    GLdouble range[2];
    int row;
    if (liquid_native.busy || !cfg.liquids_enabled || !liquid_native_allowed() ||
        !liquid_runtime_work_active(GetTickCount()) || !liquid_native_gl_load()) return;
    liquid_gl_get_floatv(GL_PROJECTION_MATRIX, p);
    if (fabsf(p[11]) < 0.1f || fabsf(p[15]) > 0.001f) {
        /* Finish the scene before the first orthographic interface draw. */
        if (liquid_native.projection_valid) liquid_native_gl_frame();
        return;
    }
    liquid_gl_get_integerv(GL_DEPTH_TEST, &depth_test);
    liquid_gl_get_integerv(GL_DEPTH_WRITEMASK, &depth_write);
    if (!depth_test || !depth_write) return;
    liquid_gl_get_integerv(GL_VIEWPORT, viewport);
    liquid_gl_get_integerv(GL_DEPTH_FUNC, &depth_func);
    liquid_ngl.GetDoublev(GL_DEPTH_RANGE, range);
    memcpy(&liquid_native.projection, p, sizeof(p));
    for (row = 0; row < 4; row++)
        ((float*)&liquid_native.projection)[row * 4 + 2] =
            p[row * 4 + 2] * (float)((range[1] - range[0]) * 0.5) +
            p[row * 4 + 3] * (float)((range[1] + range[0]) * 0.5);
    liquid_native.viewport.X = viewport[0]; liquid_native.viewport.Y = viewport[1];
    liquid_native.viewport.Width = viewport[2]; liquid_native.viewport.Height = viewport[3];
    liquid_native.viewport.MinZ = 0; liquid_native.viewport.MaxZ = 1;
    liquid_native_gl_depth_func = depth_func >= GL_NEVER && depth_func <= GL_ALWAYS ?
        (D3D11_COMPARISON_FUNC)(depth_func - GL_NEVER + 1) : D3D11_COMPARISON_LESS_EQUAL;
    liquid_native.projection_valid = viewport[2] > 0 && viewport[3] > 0;
    InterlockedExchange(&graphics_hook_ready, 1);
}

static void APIENTRY liquid_native_glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices)
{
    liquid_native_gl_capture();
    if (liquid_native_real_glDrawElements) liquid_native_real_glDrawElements(mode, count, type, indices);
}
static void APIENTRY liquid_native_glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
    liquid_native_gl_capture();
    if (liquid_native_real_glDrawArrays) liquid_native_real_glDrawArrays(mode, first, count);
}
static void APIENTRY liquid_native_glBegin(GLenum mode)
{
    liquid_native_gl_capture();
    if (liquid_native_real_glBegin) liquid_native_real_glBegin(mode);
}
static FARPROC liquid_native_gl_proc(const char *name, FARPROC result)
{
#define NGL_HOOK(name_) if (!strcmp(name, "gl" #name_)) { \
    if (result != (FARPROC)liquid_native_gl##name_ && !liquid_native_real_gl##name_) \
        copy_proc_address(&liquid_native_real_gl##name_, result, sizeof(liquid_native_real_gl##name_)); \
    return (FARPROC)liquid_native_gl##name_; }
    NGL_HOOK(DrawElements); NGL_HOOK(DrawArrays); NGL_HOOK(Begin);
#undef NGL_HOOK
    return result;
}

static void liquid_native_gl_frame(void)
{
    GLint mode = GL_MODELVIEW, program = 0;
    GLuint texture = 0;
    UINT y, x;
    if (!liquid_native_allowed() || liquid_native.busy || !liquid_native_gl_load()) return;
    InterlockedExchange(&graphics_hook_ready, 1);
    liquid_simulation_tick();
    if (!cfg.liquids_enabled || !liquid_has_active_particles()) {
        liquid_native.projection_valid = 0;
        return;
    }
    if (!liquid_native.projection_valid ||
        !liquid_native_ensure(liquid_native.viewport.Width, liquid_native.viewport.Height)) return;
    liquid_native.busy = 1;
    liquid_gl_push_attrib(GL_ALL_ATTRIB_BITS);
    liquid_ngl.PushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
    liquid_ngl.Viewport(liquid_native.viewport.X, liquid_native.viewport.Y,
                       liquid_native.viewport.Width, liquid_native.viewport.Height);
    liquid_ngl.PixelStorei(GL_PACK_ALIGNMENT, 1);
    liquid_ngl.PixelStorei(GL_PACK_ROW_LENGTH, 0);
    liquid_ngl.PixelStorei(GL_PACK_SKIP_ROWS, 0); liquid_ngl.PixelStorei(GL_PACK_SKIP_PIXELS, 0);
    liquid_ngl.PixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
    liquid_ngl.PixelTransferf(GL_DEPTH_SCALE, 1); liquid_ngl.PixelTransferf(GL_DEPTH_BIAS, 0);
    liquid_ngl.ReadPixels(liquid_native.viewport.X, liquid_native.viewport.Y,
        liquid_native.width, liquid_native.height, GL_DEPTH_COMPONENT, GL_FLOAT, liquid_native.depth);
    for (y = 0; y < liquid_native.height / 2; y++)
        for (x = 0; x < liquid_native.width; x++) {
            size_t a = (size_t)y * liquid_native.width + x;
            size_t b = (size_t)(liquid_native.height - 1 - y) * liquid_native.width + x;
            float temp = liquid_native.depth[a]; liquid_native.depth[a] = liquid_native.depth[b];
            liquid_native.depth[b] = temp;
        }
    if (!liquid_native_composite(liquid_native_gl_depth_func)) goto cleanup;
    if (liquid_ngl.GetProcAddress) {
        PROC proc = liquid_ngl.GetProcAddress("glActiveTexture");
        liquid_ngl.ActiveTexture = NULL; liquid_ngl.UseProgram = NULL;
        liquid_ngl.BlendEquation = NULL;
        if ((ULONG_PTR)proc > 3 && (ULONG_PTR)proc != ~(ULONG_PTR)0)
            copy_proc_address(&liquid_ngl.ActiveTexture, (FARPROC)proc, sizeof(liquid_ngl.ActiveTexture));
        proc = liquid_ngl.GetProcAddress("glUseProgram");
        if ((ULONG_PTR)proc > 3 && (ULONG_PTR)proc != ~(ULONG_PTR)0)
            copy_proc_address(&liquid_ngl.UseProgram, (FARPROC)proc, sizeof(liquid_ngl.UseProgram));
        proc = liquid_ngl.GetProcAddress("glBlendEquation");
        if ((ULONG_PTR)proc > 3 && (ULONG_PTR)proc != ~(ULONG_PTR)0)
            copy_proc_address(&liquid_ngl.BlendEquation, (FARPROC)proc, sizeof(liquid_ngl.BlendEquation));
    }
    if (liquid_ngl.UseProgram) {
        liquid_gl_get_integerv(0x8B8D /* GL_CURRENT_PROGRAM */, &program);
        liquid_ngl.UseProgram(0);
    }
    if (liquid_ngl.ActiveTexture) {
        GLint units = 1, unit;
        liquid_gl_get_integerv(0x84E2 /* GL_MAX_TEXTURE_UNITS */, &units);
        for (unit = 0; unit < units; unit++) {
            liquid_ngl.ActiveTexture(0x84C0 + unit);
            liquid_gl_disable(GL_TEXTURE_1D); liquid_gl_disable(GL_TEXTURE_2D);
            liquid_gl_disable(GL_TEXTURE_GEN_S); liquid_gl_disable(GL_TEXTURE_GEN_T);
            liquid_gl_disable(GL_TEXTURE_GEN_R); liquid_gl_disable(GL_TEXTURE_GEN_Q);
        }
        liquid_ngl.ActiveTexture(0x84C0);
    }
    liquid_gl_get_integerv(GL_MATRIX_MODE, &mode);
    liquid_gl_disable(GL_DEPTH_TEST); liquid_gl_disable(GL_STENCIL_TEST);
    liquid_gl_disable(GL_ALPHA_TEST); liquid_gl_disable(GL_LIGHTING);
    liquid_gl_disable(GL_FOG); liquid_gl_disable(GL_CULL_FACE); liquid_gl_disable(GL_SCISSOR_TEST);
    liquid_gl_disable(GL_COLOR_LOGIC_OP); liquid_gl_disable(GL_POLYGON_STIPPLE);
    for (x = 0; x < 6; x++) liquid_gl_disable(GL_CLIP_PLANE0 + x);
    liquid_ngl.ColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
    liquid_ngl.PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (liquid_ngl.BlendEquation) liquid_ngl.BlendEquation(0x8006 /* GL_FUNC_ADD */);
    liquid_gl_enable(GL_BLEND); liquid_gl_blend_func(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    liquid_ngl.GenTextures(1, &texture); liquid_ngl.BindTexture(GL_TEXTURE_2D, texture);
    liquid_gl_enable(GL_TEXTURE_2D);
    liquid_ngl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    liquid_ngl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    liquid_ngl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    liquid_ngl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    liquid_ngl.PixelStorei(GL_UNPACK_ALIGNMENT, 1); liquid_ngl.PixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    liquid_ngl.PixelStorei(GL_UNPACK_SKIP_ROWS, 0); liquid_ngl.PixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    liquid_ngl.PixelTransferf(GL_RED_SCALE, 1); liquid_ngl.PixelTransferf(GL_RED_BIAS, 0);
    liquid_ngl.PixelTransferf(GL_GREEN_SCALE, 1); liquid_ngl.PixelTransferf(GL_GREEN_BIAS, 0);
    liquid_ngl.PixelTransferf(GL_BLUE_SCALE, 1); liquid_ngl.PixelTransferf(GL_BLUE_BIAS, 0);
    liquid_ngl.PixelTransferf(GL_ALPHA_SCALE, 1); liquid_ngl.PixelTransferf(GL_ALPHA_BIAS, 0);
    liquid_ngl.PixelTransferf(GL_MAP_COLOR, GL_FALSE);
    liquid_ngl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, liquid_native.width, liquid_native.height,
                        0, GL_RGBA, GL_UNSIGNED_BYTE, liquid_native.rgba);
    liquid_ngl.TexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    liquid_gl_matrix_mode(GL_TEXTURE); liquid_gl_push_matrix(); liquid_gl_load_identity();
    liquid_gl_matrix_mode(GL_PROJECTION); liquid_gl_push_matrix(); liquid_gl_load_identity();
    liquid_gl_ortho(0, 1, 1, 0, -1, 1);
    liquid_gl_matrix_mode(GL_MODELVIEW); liquid_gl_push_matrix(); liquid_gl_load_identity();
    liquid_gl_begin(GL_QUADS);
    liquid_ngl.TexCoord2f(0, 0); liquid_gl_vertex2f(0, 0);
    liquid_ngl.TexCoord2f(1, 0); liquid_gl_vertex2f(1, 0);
    liquid_ngl.TexCoord2f(1, 1); liquid_gl_vertex2f(1, 1);
    liquid_ngl.TexCoord2f(0, 1); liquid_gl_vertex2f(0, 1);
    liquid_gl_end(); liquid_gl_pop_matrix();
    liquid_gl_matrix_mode(GL_PROJECTION); liquid_gl_pop_matrix();
    liquid_gl_matrix_mode(GL_TEXTURE); liquid_gl_pop_matrix(); liquid_gl_matrix_mode(mode);
    liquid_ngl.DeleteTextures(1, &texture);
    if (liquid_ngl.UseProgram) liquid_ngl.UseProgram(program);
    { static int logged; if (!logged++) log_line("native OpenGL shared liquid renderer active depth=readback shader=shared"); }
cleanup:
    liquid_ngl.PopClientAttrib(); liquid_gl_pop_attrib(); liquid_native.busy = 0;
    liquid_native.projection_valid = 0;
}

/* D3D8 depth mirror: replay depth-writing draws into a separate lockable
   surface, preserving the original color/depth/stencil buffers and state.
   No renderer-wide precision change or presentation-parameter rewrite. */
typedef HRESULT (WINAPI *liquid_nclear_t)(IDirect3DDevice8*, DWORD, const D3DRECT*, DWORD, D3DCOLOR, float, DWORD);
typedef HRESULT (WINAPI *liquid_ndraw_t)(IDirect3DDevice8*, D3DPRIMITIVETYPE, UINT, UINT);
typedef HRESULT (WINAPI *liquid_ndrawi_t)(IDirect3DDevice8*, D3DPRIMITIVETYPE, UINT, UINT, UINT, UINT);
typedef HRESULT (WINAPI *liquid_ndrawup_t)(IDirect3DDevice8*, D3DPRIMITIVETYPE, UINT, const void*, UINT);
typedef HRESULT (WINAPI *liquid_ndrawiup_t)(IDirect3DDevice8*, D3DPRIMITIVETYPE, UINT, UINT, UINT, const void*, D3DFORMAT, const void*, UINT);
typedef HRESULT (WINAPI *liquid_nreset_t)(IDirect3DDevice8*, D3DPRESENT_PARAMETERS*);
static liquid_nclear_t liquid_nclear;
static liquid_ndraw_t liquid_ndraw;
static liquid_ndrawi_t liquid_ndrawi;
static liquid_ndrawup_t liquid_ndrawup;
static liquid_ndrawiup_t liquid_ndrawiup;
static liquid_nreset_t liquid_nreset;
static IDirect3DSurface8 *liquid_native_depth8;
static IDirect3DSurface8 *liquid_native_color8;
static D3DFORMAT liquid_native_depth8_format;
static D3DFORMAT liquid_native_color8_format;
static IDirect3DDevice8 *liquid_native_device8;
static UINT liquid_native_depth8_width, liquid_native_depth8_height;
static int liquid_native_depth8_valid;
static D3D11_COMPARISON_FUNC liquid_native_depth8_func = D3D11_COMPARISON_LESS_EQUAL;
static void liquid_native_d3d8_submit(IDirect3DDevice8 *device, int inside_scene);

static int liquid_native_is_backbuffer8(IDirect3DDevice8 *device)
{
    IDirect3DSurface8 *target = NULL, *back = NULL;
    int match = SUCCEEDED(IDirect3DDevice8_GetRenderTarget(device, &target)) &&
        SUCCEEDED(IDirect3DDevice8_GetBackBuffer(device, 0, D3DBACKBUFFER_TYPE_MONO, &back)) &&
        target == back;
    if (target) IDirect3DSurface8_Release(target);
    if (back) IDirect3DSurface8_Release(back);
    return match;
}

static void liquid_native_release_depth8(void)
{
    if (liquid_native_depth8) IDirect3DSurface8_Release(liquid_native_depth8);
    if (liquid_native_color8) IDirect3DSurface8_Release(liquid_native_color8);
    liquid_native_color8 = NULL;
    liquid_native_depth8 = NULL; liquid_native_device8 = NULL;
    liquid_native_depth8_valid = 0;
}

static int liquid_native_ensure_depth8(IDirect3DDevice8 *device)
{
    IDirect3DSurface8 *target = NULL;
    D3DSURFACE_DESC desc;
    HRESULT result;
    if (FAILED(IDirect3DDevice8_GetRenderTarget(device, &target))) return 0;
    result = IDirect3DSurface8_GetDesc(target, &desc); IDirect3DSurface8_Release(target);
    if (FAILED(result)) return 0;
    if (liquid_native_depth8 && liquid_native_device8 == device &&
        desc.Format == liquid_native_color8_format &&
        desc.Width == liquid_native_depth8_width && desc.Height == liquid_native_depth8_height) return 1;
    liquid_native_release_depth8();
    /* Modern D3D8 runtimes/drivers can expose D32F_LOCKABLE (82) despite the
       original SDK enum ending earlier. Probe it; never assume support. */
    liquid_native_depth8_format = (D3DFORMAT)82;
    result = IDirect3DDevice8_CreateDepthStencilSurface(device, desc.Width, desc.Height,
        liquid_native_depth8_format, D3DMULTISAMPLE_NONE, &liquid_native_depth8);
    if (FAILED(result)) {
        liquid_native_depth8_format = D3DFMT_D16_LOCKABLE;
        result = IDirect3DDevice8_CreateDepthStencilSurface(device, desc.Width, desc.Height,
            liquid_native_depth8_format, D3DMULTISAMPLE_NONE, &liquid_native_depth8);
    }
    if (FAILED(result)) {
        static int warned;
        if (!warned++) log_line("native D3D8 readable depth unavailable hr=0x%08lx", (unsigned long)result);
        return 0;
    }
    /* A private color target allows the readable single-sample depth pass
       even when the game's own target is multisampled. No color is written. */
    result = IDirect3DDevice8_CreateRenderTarget(device, desc.Width, desc.Height,
        desc.Format, D3DMULTISAMPLE_NONE, FALSE, &liquid_native_color8);
    if (FAILED(result)) { liquid_native_release_depth8(); return 0; }
    liquid_native_device8 = device; liquid_native_depth8_width = desc.Width;
    liquid_native_depth8_height = desc.Height;
    liquid_native_color8_format = desc.Format;
    return 1;
}

static HRESULT WINAPI liquid_native_Clear(IDirect3DDevice8 *device, DWORD count,
    const D3DRECT *rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil)
{
    HRESULT result = liquid_nclear(device, count, rects, flags, color, z, stencil);
    IDirect3DSurface8 *target = NULL, *depth = NULL;
    D3DVIEWPORT8 viewport;
    if (FAILED(result) || !(flags & D3DCLEAR_ZBUFFER) || liquid_native.busy ||
        !liquid_native_allowed() || !cfg.liquids_enabled ||
        !liquid_runtime_work_active(GetTickCount()) || !liquid_native_is_backbuffer8(device) ||
        !liquid_native_ensure_depth8(device)) return result;
    /* A failed mirror clear must not leave last frame's depth usable. */
    liquid_native_depth8_valid = 0;
    if (SUCCEEDED(IDirect3DDevice8_GetRenderTarget(device, &target)) &&
        SUCCEEDED(IDirect3DDevice8_GetDepthStencilSurface(device, &depth)) &&
        SUCCEEDED(IDirect3DDevice8_GetViewport(device, &viewport)) &&
        SUCCEEDED(IDirect3DDevice8_SetRenderTarget(device, liquid_native_color8, liquid_native_depth8))) {
        IDirect3DDevice8_SetViewport(device, &viewport);
        if (SUCCEEDED(liquid_nclear(device, count, rects, D3DCLEAR_ZBUFFER, 0, z, 0)))
            liquid_native_depth8_valid = 1;
        IDirect3DDevice8_SetRenderTarget(device, target, depth);
        IDirect3DDevice8_SetViewport(device, &viewport);
    }
    if (depth) IDirect3DSurface8_Release(depth);
    if (target) IDirect3DSurface8_Release(target);
    return result;
}

typedef struct liquid_depth_replay_t {
    IDirect3DSurface8 *target, *depth;
    DWORD state;
    D3DVIEWPORT8 viewport;
    int ready;
} liquid_depth_replay_t;

static void liquid_native_replay_begin(IDirect3DDevice8 *device, liquid_depth_replay_t *replay)
{
    D3DMATRIX projection;
    D3DVIEWPORT8 viewport;
    DWORD zwrite = 0, zenable = 0, stencil = 0, zfunc = D3DCMP_LESSEQUAL;
    memset(replay, 0, sizeof(*replay));
    if (liquid_native.busy || !liquid_native_allowed() || !cfg.liquids_enabled ||
        !liquid_runtime_work_active(GetTickCount()) || !liquid_native_is_backbuffer8(device)) return;
    if (FAILED(IDirect3DDevice8_GetTransform(device, D3DTS_PROJECTION, &projection))) return;
    if (fabsf(projection._34) < 0.1f || fabsf(projection._44) > 0.001f) {
        if (liquid_native.projection_valid && liquid_native_depth8_valid)
            liquid_native_d3d8_submit(device, 1);
        return;
    }
    if (FAILED(IDirect3DDevice8_GetViewport(device, &viewport))) return;
    IDirect3DDevice8_GetRenderState(device, D3DRS_ZENABLE, &zenable);
    IDirect3DDevice8_GetRenderState(device, D3DRS_ZWRITEENABLE, &zwrite);
    if (!zenable || !zwrite) return;
    liquid_native.projection = projection; liquid_native.viewport = viewport;
    {
        int row;
        for (row = 0; row < 4; row++)
            ((float*)&liquid_native.projection)[row * 4 + 2] =
                ((float*)&projection)[row * 4 + 2] * (viewport.MaxZ - viewport.MinZ) +
                ((float*)&projection)[row * 4 + 3] * viewport.MinZ;
        liquid_native.viewport.MinZ = 0; liquid_native.viewport.MaxZ = 1;
    }
    liquid_native.projection_valid = 1;
    if (!liquid_native_depth8_valid || device != liquid_native_device8) return;
    IDirect3DDevice8_GetRenderState(device, D3DRS_STENCILENABLE, &stencil);
    IDirect3DDevice8_GetRenderState(device, D3DRS_ZFUNC, &zfunc);
    if (stencil) {
        DWORD func = D3DCMP_ALWAYS;
        IDirect3DDevice8_GetRenderState(device, D3DRS_STENCILFUNC, &func);
        if (func == D3DCMP_NEVER) return; /* No scene depth is written. */
        if (func != D3DCMP_ALWAYS) {
            liquid_native_depth8_valid = 0;
            { static int warned; if (!warned++) log_line("native D3D8 frame rejected: conditional stencil-tested depth writes cannot be mirrored exactly"); }
            return;
        }
    }
    liquid_native_depth8_func = (D3D11_COMPARISON_FUNC)zfunc;
    if (FAILED(IDirect3DDevice8_GetRenderTarget(device, &replay->target)) ||
        FAILED(IDirect3DDevice8_GetDepthStencilSurface(device, &replay->depth)) ||
        FAILED(IDirect3DDevice8_CreateStateBlock(device, D3DSBT_ALL, &replay->state))) goto fail;
    if (FAILED(IDirect3DDevice8_SetRenderTarget(device, liquid_native_color8, liquid_native_depth8))) goto fail;
    replay->viewport = viewport;
    IDirect3DDevice8_SetViewport(device, &viewport);
    IDirect3DDevice8_SetRenderState(device, D3DRS_COLORWRITEENABLE, 0);
    IDirect3DDevice8_SetRenderState(device, D3DRS_STENCILENABLE, FALSE);
    replay->ready = 1;
    return;
fail:
    if (replay->state) IDirect3DDevice8_DeleteStateBlock(device, replay->state);
    if (replay->depth) IDirect3DSurface8_Release(replay->depth);
    if (replay->target) IDirect3DSurface8_Release(replay->target);
    memset(replay, 0, sizeof(*replay));
}
static void liquid_native_replay_end(IDirect3DDevice8 *device, liquid_depth_replay_t *replay)
{
    if (!replay->ready) return;
    IDirect3DDevice8_SetRenderTarget(device, replay->target, replay->depth);
    IDirect3DDevice8_ApplyStateBlock(device, replay->state);
    IDirect3DDevice8_SetViewport(device, &replay->viewport);
    IDirect3DDevice8_DeleteStateBlock(device, replay->state);
    IDirect3DSurface8_Release(replay->depth); IDirect3DSurface8_Release(replay->target);
}

static HRESULT WINAPI liquid_native_Draw(IDirect3DDevice8 *d, D3DPRIMITIVETYPE t, UINT s, UINT c)
{
    liquid_depth_replay_t r;
    liquid_native_replay_begin(d, &r);
    if (r.ready) { if (FAILED(liquid_ndraw(d, t, s, c))) liquid_native_depth8_valid = 0; liquid_native_replay_end(d, &r); }
    return liquid_ndraw(d, t, s, c);
}
static HRESULT WINAPI liquid_native_DrawIndexed(IDirect3DDevice8 *d, D3DPRIMITIVETYPE t, UINT m, UINT n, UINT s, UINT c)
{
    liquid_depth_replay_t r;
    liquid_native_replay_begin(d, &r);
    if (r.ready) { if (FAILED(liquid_ndrawi(d, t, m, n, s, c))) liquid_native_depth8_valid = 0; liquid_native_replay_end(d, &r); }
    return liquid_ndrawi(d, t, m, n, s, c);
}
static HRESULT WINAPI liquid_native_DrawUP(IDirect3DDevice8 *d, D3DPRIMITIVETYPE t, UINT c, const void *v, UINT stride)
{
    liquid_depth_replay_t r;
    liquid_native_replay_begin(d, &r);
    if (r.ready) { if (FAILED(liquid_ndrawup(d, t, c, v, stride))) liquid_native_depth8_valid = 0; liquid_native_replay_end(d, &r); }
    return liquid_ndrawup(d, t, c, v, stride);
}
static HRESULT WINAPI liquid_native_DrawIndexedUP(IDirect3DDevice8 *d, D3DPRIMITIVETYPE t, UINT m, UINT n, UINT c,
    const void *idx, D3DFORMAT f, const void *v, UINT stride)
{
    liquid_depth_replay_t r;
    liquid_native_replay_begin(d, &r);
    if (r.ready) { if (FAILED(liquid_ndrawiup(d, t, m, n, c, idx, f, v, stride))) liquid_native_depth8_valid = 0; liquid_native_replay_end(d, &r); }
    return liquid_ndrawiup(d, t, m, n, c, idx, f, v, stride);
}
static HRESULT WINAPI liquid_native_Reset(IDirect3DDevice8 *d, D3DPRESENT_PARAMETERS *p)
{
    liquid_native_release_depth8(); liquid_native.projection_valid = 0;
    return liquid_nreset(d, p);
}

static void liquid_native_patch_d3d8(IDirect3DDevice8 *device)
{
    liquid_patch_vtable_slot(device, 14, liquid_native_Reset, (void**)&liquid_nreset);
    liquid_patch_vtable_slot(device, 36, liquid_native_Clear, (void**)&liquid_nclear);
    liquid_patch_vtable_slot(device, 70, liquid_native_Draw, (void**)&liquid_ndraw);
    liquid_patch_vtable_slot(device, 71, liquid_native_DrawIndexed, (void**)&liquid_ndrawi);
    liquid_patch_vtable_slot(device, 72, liquid_native_DrawUP, (void**)&liquid_ndrawup);
    liquid_patch_vtable_slot(device, 73, liquid_native_DrawIndexedUP, (void**)&liquid_ndrawiup);
}

static void liquid_native_d3d8_submit(IDirect3DDevice8 *device, int inside_scene)
{
    D3DLOCKED_RECT locked;
    IDirect3DTexture8 *texture = NULL;
    DWORD state = 0;
    D3DVIEWPORT8 saved_viewport;
    UINT x, y;
    RECT crop;
    struct { float x,y,z,rhw,u,v; } quad[4];
    if (!liquid_native_allowed() || liquid_native.busy || !liquid_native_is_backbuffer8(device)) return;
    liquid_simulation_tick();
    if (!cfg.liquids_enabled || !liquid_has_active_particles() || !liquid_native_depth8_valid ||
        device != liquid_native_device8 || !liquid_native.projection_valid ||
        !liquid_native_ensure(liquid_native.viewport.Width, liquid_native.viewport.Height)) goto done;
    crop.left = liquid_native.viewport.X; crop.top = liquid_native.viewport.Y;
    crop.right = crop.left + liquid_native.width; crop.bottom = crop.top + liquid_native.height;
    if ((UINT)crop.right > liquid_native_depth8_width || (UINT)crop.bottom > liquid_native_depth8_height ||
        FAILED(IDirect3DSurface8_LockRect(liquid_native_depth8, &locked, &crop, D3DLOCK_READONLY))) goto done;
    for (y = 0; y < liquid_native.height; y++) {
        const BYTE *row = (BYTE*)locked.pBits + (size_t)y * locked.Pitch;
        for (x = 0; x < liquid_native.width; x++)
            liquid_native.depth[(size_t)y * liquid_native.width + x] =
                liquid_native_depth8_format == (D3DFORMAT)82 ? ((const float*)row)[x] :
                ((const unsigned short*)row)[x] / 65535.0f;
    }
    IDirect3DSurface8_UnlockRect(liquid_native_depth8);
    if (!liquid_native_composite(liquid_native_depth8_func)) goto done;
    if (FAILED(IDirect3DDevice8_CreateTexture(device, liquid_native.width, liquid_native.height,
        1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture))) goto done;
    if (FAILED(IDirect3DTexture8_LockRect(texture, 0, &locked, NULL, 0))) goto cleanup;
    for (y = 0; y < liquid_native.height; y++) {
        DWORD *row = (DWORD*)((BYTE*)locked.pBits + (size_t)y * locked.Pitch);
        const BYTE *src = liquid_native.rgba + (size_t)y * liquid_native.width * 4;
        for (x = 0; x < liquid_native.width; x++) row[x] = D3DCOLOR_ARGB(src[x*4+3],src[x*4],src[x*4+1],src[x*4+2]);
    }
    IDirect3DTexture8_UnlockRect(texture, 0);
    if (FAILED(IDirect3DDevice8_CreateStateBlock(device, D3DSBT_ALL, &state))) goto cleanup;
    if (FAILED(IDirect3DDevice8_GetViewport(device, &saved_viewport))) goto cleanup;
    if (!inside_scene && FAILED(IDirect3DDevice8_BeginScene(device))) goto cleanup;
    liquid_native.busy = 1;
    IDirect3DDevice8_SetViewport(device, &liquid_native.viewport);
    IDirect3DDevice8_SetVertexShader(device, D3DFVF_XYZRHW | D3DFVF_TEX1);
    IDirect3DDevice8_SetPixelShader(device, 0);
    IDirect3DDevice8_SetTexture(device, 0, (IDirect3DBaseTexture8*)texture);
    IDirect3DDevice8_SetTextureStageState(device, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    IDirect3DDevice8_SetTextureStageState(device, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    IDirect3DDevice8_SetTextureStageState(device, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    IDirect3DDevice8_SetTextureStageState(device, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    IDirect3DDevice8_SetTextureStageState(device, 0, D3DTSS_TEXCOORDINDEX, 0);
    IDirect3DDevice8_SetTextureStageState(device, 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
    IDirect3DDevice8_SetTextureStageState(device, 0, D3DTSS_MINFILTER, D3DTEXF_POINT);
    IDirect3DDevice8_SetTextureStageState(device, 0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    IDirect3DDevice8_SetTextureStageState(device, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    IDirect3DDevice8_SetTextureStageState(device, 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_ZENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_ZWRITEENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_STENCILENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_ALPHATESTENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_FOGENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_LIGHTING, FALSE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_CULLMODE, D3DCULL_NONE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_FILLMODE, D3DFILL_SOLID);
    IDirect3DDevice8_SetRenderState(device, D3DRS_CLIPPLANEENABLE, 0);
    IDirect3DDevice8_SetRenderState(device, D3DRS_ALPHABLENDENABLE, TRUE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_SRCBLEND, D3DBLEND_ONE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    IDirect3DDevice8_SetRenderState(device, D3DRS_BLENDOP, D3DBLENDOP_ADD);
    IDirect3DDevice8_SetRenderState(device, D3DRS_COLORWRITEENABLE, 7);
    for (x = 0; x < 4; x++) {
        quad[x].u = (x & 1) ? 1 : 0; quad[x].v = (x & 2) ? 1 : 0;
        quad[x].x = crop.left + quad[x].u * liquid_native.width - 0.5f;
        quad[x].y = crop.top + quad[x].v * liquid_native.height - 0.5f;
        quad[x].z = 0; quad[x].rhw = 1;
    }
    IDirect3DDevice8_DrawPrimitiveUP(device, D3DPT_TRIANGLESTRIP, 2, quad, sizeof(quad[0]));
    if (!inside_scene) IDirect3DDevice8_EndScene(device);
    IDirect3DDevice8_SetViewport(device, &saved_viewport);
    liquid_native.busy = 0;
    { static int logged; if (!logged++) log_line("native D3D8 shared liquid renderer active depth=separate-mirror format=%u shader=shared", liquid_native_depth8_format); }
cleanup:
    if (state) { IDirect3DDevice8_ApplyStateBlock(device, state); IDirect3DDevice8_DeleteStateBlock(device, state); }
    if (texture) IDirect3DTexture8_Release(texture);
done:
    liquid_native_depth8_valid = 0;
    liquid_native.projection_valid = 0;
}

static void liquid_native_d3d8_frame(IDirect3DDevice8 *device)
{
    liquid_native_d3d8_submit(device, 0);
}
