#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #c); exit(1); } } while (0)

/* Model D3D8's render-target viewport reset and Draw*UP binding side effects.
   The original game draw must see exactly the state it would see unhooked. */
static IDirect3DDevice8 device;
static IDirect3DSurface8 scene_color, scene_depth, mirror_color, mirror_depth;
static IDirect3DVertexBuffer8 vb;
static IDirect3DIndexBuffer8 ib;
static IDirect3DSurface8 *target, *depth;
static IDirect3DVertexBuffer8 *stream;
static IDirect3DIndexBuffer8 *indices;
static D3DVIEWPORT8 viewport, original_viewport;
static DWORD color_write, stencil, stencil_func;
static UINT stride, base_vertex;
static int mirrored, forwarded, fail_bindings;

static ULONG WINAPI release_surface(IDirect3DSurface8 *p) { (void)p; return 1; }
static ULONG WINAPI release_vb(IDirect3DVertexBuffer8 *p) { (void)p; return 1; }
static ULONG WINAPI release_ib(IDirect3DIndexBuffer8 *p) { (void)p; return 1; }
static HRESULT WINAPI get_target(IDirect3DDevice8 *d, IDirect3DSurface8 **p) { *p = target; return S_OK; }
static HRESULT WINAPI get_back(IDirect3DDevice8 *d, UINT i, D3DBACKBUFFER_TYPE t, IDirect3DSurface8 **p) { *p = &scene_color; return S_OK; }
static HRESULT WINAPI get_depth(IDirect3DDevice8 *d, IDirect3DSurface8 **p) { *p = depth; return S_OK; }
static HRESULT WINAPI set_target(IDirect3DDevice8 *d, IDirect3DSurface8 *t, IDirect3DSurface8 *z)
{ target = t; depth = z; viewport = (D3DVIEWPORT8){0,0,640,480,0,1}; return S_OK; }
static HRESULT WINAPI get_viewport(IDirect3DDevice8 *d, D3DVIEWPORT8 *v) { *v = viewport; return S_OK; }
static HRESULT WINAPI set_viewport(IDirect3DDevice8 *d, const D3DVIEWPORT8 *v) { viewport = *v; return S_OK; }
static HRESULT WINAPI get_transform(IDirect3DDevice8 *d, D3DTRANSFORMSTATETYPE t, D3DMATRIX *m)
{ memset(m, 0, sizeof(*m)); m->_11 = m->_22 = 1; m->_34 = -1; return S_OK; }
static HRESULT WINAPI get_state(IDirect3DDevice8 *d, D3DRENDERSTATETYPE t, DWORD *v)
{
    switch (t) {
    case D3DRS_ZENABLE: case D3DRS_ZWRITEENABLE: *v = TRUE; break;
    case D3DRS_ZFUNC: *v = D3DCMP_LESSEQUAL; break;
    case D3DRS_STENCILENABLE: *v = stencil; break;
    case D3DRS_STENCILFUNC: *v = stencil_func; break;
    case D3DRS_COLORWRITEENABLE: *v = color_write; break;
    default: CHECK(0);
    } return S_OK;
}
static HRESULT WINAPI set_state(IDirect3DDevice8 *d, D3DRENDERSTATETYPE t, DWORD v)
{
    if (t == D3DRS_COLORWRITEENABLE) color_write = v;
    else if (t == D3DRS_STENCILENABLE) stencil = v;
    else CHECK(0);
    return S_OK;
}
static HRESULT WINAPI get_stream(IDirect3DDevice8 *d, UINT i, IDirect3DVertexBuffer8 **b, UINT *s)
{ if (fail_bindings) return E_FAIL; *b = stream; *s = stride; return S_OK; }
static HRESULT WINAPI set_stream(IDirect3DDevice8 *d, UINT i, IDirect3DVertexBuffer8 *b, UINT s)
{ stream = b; stride = s; return S_OK; }
static HRESULT WINAPI get_indices(IDirect3DDevice8 *d, IDirect3DIndexBuffer8 **b, UINT *n)
{ *b = indices; *n = base_vertex; return S_OK; }
static HRESULT WINAPI set_indices(IDirect3DDevice8 *d, IDirect3DIndexBuffer8 *b, UINT n)
{ indices = b; base_vertex = n; return S_OK; }

static void check_draw(void)
{
    CHECK(!memcmp(&viewport, &original_viewport, sizeof(viewport)));
    CHECK(stream == &vb && stride == 24 && indices == &ib && base_vertex == 5);
    if (target == &mirror_color) {
        CHECK(depth == &mirror_depth && color_write == 0 && stencil == FALSE);
        mirrored++;
    } else {
        CHECK(target == &scene_color && depth == &scene_depth);
        CHECK(color_write == 11 && stencil == TRUE);
        forwarded++;
    }
}
static HRESULT WINAPI draw(IDirect3DDevice8 *d, D3DPRIMITIVETYPE t, UINT s, UINT n)
{ check_draw(); return S_OK; }
static HRESULT WINAPI draw_indexed(IDirect3DDevice8 *d, D3DPRIMITIVETYPE t, UINT m, UINT n, UINT s, UINT c)
{ check_draw(); return S_OK; }
static HRESULT WINAPI draw_up(IDirect3DDevice8 *d, D3DPRIMITIVETYPE t, UINT n, const void *v, UINT s)
{ check_draw(); stream = NULL; stride = 0; return S_OK; }
static HRESULT WINAPI draw_indexed_up(IDirect3DDevice8 *d, D3DPRIMITIVETYPE t, UINT m, UINT n,
    UINT c, const void *i, D3DFORMAT f, const void *v, UINT s)
{ check_draw(); stream = NULL; stride = 0; indices = NULL; base_vertex = 0; return S_OK; }

static void setup(void)
{
    target = &scene_color; depth = &scene_depth;
    viewport = original_viewport = (D3DVIEWPORT8){13,19,320,240,0.2f,0.9f};
    color_write = 11; stencil = TRUE; stencil_func = D3DCMP_ALWAYS;
    stream = &vb; stride = 24; indices = &ib; base_vertex = 5;
    mirrored = forwarded = fail_bindings = 0;
    liquid_native_depth8_valid = 1;
}

int main(void)
{
    static IDirect3DSurface8Vtbl sv = {.Release = release_surface};
    static IDirect3DVertexBuffer8Vtbl vv = {.Release = release_vb};
    static IDirect3DIndexBuffer8Vtbl iv = {.Release = release_ib};
    static IDirect3DDevice8Vtbl dv = {
        .GetRenderTarget = get_target, .GetBackBuffer = get_back,
        .GetDepthStencilSurface = get_depth, .SetRenderTarget = set_target,
        .GetViewport = get_viewport, .SetViewport = set_viewport,
        .GetTransform = get_transform, .GetRenderState = get_state, .SetRenderState = set_state,
        .GetStreamSource = get_stream, .SetStreamSource = set_stream,
        .GetIndices = get_indices, .SetIndices = set_indices
        /* No state-block functions: replay must work without full snapshots. */
    };
    device.lpVtbl = &dv;
    scene_color.lpVtbl = scene_depth.lpVtbl = mirror_color.lpVtbl = mirror_depth.lpVtbl = &sv;
    vb.lpVtbl = &vv; ib.lpVtbl = &iv;
    cfg.liquids_enabled = 1; cfg.particle_limit = 1; liquid_particles[0].active = 1;
    liquid_native_device8 = &device;
    liquid_native_color8 = &mirror_color; liquid_native_depth8 = &mirror_depth;
    liquid_ndraw = draw; liquid_ndrawi = draw_indexed;
    liquid_ndrawup = draw_up; liquid_ndrawiup = draw_indexed_up;
    setup(); CHECK(SUCCEEDED(liquid_native_Draw(&device, D3DPT_TRIANGLELIST, 0, 1)));
    CHECK(mirrored == 1 && forwarded == 1 && stream == &vb && indices == &ib);
    setup(); CHECK(SUCCEEDED(liquid_native_DrawIndexed(&device, D3DPT_TRIANGLELIST, 0, 3, 0, 1)));
    CHECK(mirrored == 1 && forwarded == 1 && stream == &vb && indices == &ib);
    setup(); CHECK(SUCCEEDED(liquid_native_DrawUP(&device, D3DPT_TRIANGLELIST, 1, NULL, 24)));
    CHECK(mirrored == 1 && forwarded == 1 && !stream && indices == &ib);
    setup(); CHECK(SUCCEEDED(liquid_native_DrawIndexedUP(&device, D3DPT_TRIANGLELIST, 0, 3, 1, NULL, D3DFMT_INDEX16, NULL, 24)));
    CHECK(mirrored == 1 && forwarded == 1 && !stream && !indices);
    setup(); fail_bindings = 1;
    CHECK(SUCCEEDED(liquid_native_DrawUP(&device, D3DPT_TRIANGLELIST, 1, NULL, 24)));
    CHECK(mirrored == 0 && forwarded == 1 && !liquid_native_depth8_valid);
    setup(); stencil_func = D3DCMP_EQUAL;
    CHECK(SUCCEEDED(liquid_native_Draw(&device, D3DPT_TRIANGLELIST, 0, 1)));
    CHECK(mirrored == 0 && forwarded == 1 && !liquid_native_depth8_valid);
    setup(); liquid_particles[0].active = 0;
    CHECK(SUCCEEDED(liquid_native_Draw(&device, D3DPT_TRIANGLELIST, 0, 1)));
    CHECK(mirrored == 0 && forwarded == 1);
    puts("PASS: native replay preserves targets, viewport, color/stencil, all four draw paths and UP bindings; failed capture and idle bypass");
    return 0;
}
