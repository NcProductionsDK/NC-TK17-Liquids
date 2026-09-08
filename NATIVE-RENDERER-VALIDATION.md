# Native renderer candidate

Version: `0.8.0-native-renderer-candidate`.
Status: API and regression tests passed; TK17 gameplay and full parity are unverified.

## Implementation

`liquid_native_renderers.c` is included by the main plugin source. Both native
adapters call the existing liquid geometry builder, D3D11 shader, and depth
collision classifier. They render to a transparent offscreen D3D11 texture
and composite its premultiplied RGBA into the game's native renderer.
No duplicate reduced-feature particle renderer is used.

OpenGL captures the perspective scene projection at drawing entry points,
converts its depth range, and reads the scene depth. Direct3D 8 mirrors
depth-writing backbuffer draws into private color and readable depth surfaces.
The original depth buffer and presentation parameters are preserved. Readable
D32F format 82 is probed first, with D16_LOCKABLE as a fallback.

At the first orthographic draw, the scene's liquid layer is submitted before
that draw so subsequent interface rendering can cover it. SwapBuffers/Present
provide a fallback for frames without this transition. Render state is restored
after submission. Perspective draws without depth writes do not replace the
captured scene projection. Offscreen D3D8 targets are excluded from the mirror.

Hook5 registration or a loaded `d3d8_heffects.dll` bypasses the native adapters.
Hook5 devices do not receive the extra native depth-replay hooks. The existing
Hook5 callbacks, liquid shader, and physics remain the authoritative Hook5 path.

## Verified

- `flight_test`: frame-rate agreement and collision sweep origins.
- `flow_visuals_test`: interpolation, breakup, contact classification,
  impact response, bounded drop shapes, and relaxation.
- `projection_test`: wide FOV, clipping, and projection fallback.
- `world_emitter_test`: native transforms, source anchoring, and cache handling.
- `contact_render_test`: pinned contact links, supported room paths, actual
  D3D11 WARP shader opacity, and soft edges.
- `contact_settings_test`: generated defaults, independent UI settings,
  and preservation of existing configuration.
- `native_renderer_test`: actual shared compositor output, normal/reversed
  depth occlusion, resize, D3D8 vtable offsets, Hook5 blend-state preservation,
  and early native bypass when Hook5 is registered.
- `native_api_test`: hidden real OpenGL and D3D8 devices; numeric premultiplied
  opacity compositing; foreground depth occlusion in GL; actual depth-writing
  triangle replay in D3D8; submission inside the scene before GUI draws;
  preservation of viewport and render states; non-default depth ranges;
  D3D8 reset and resource recreation. D3D8 was exercised with no multisampling
  and 2x multisampling on NVIDIA GeForce RTX 5080 / `nvldumd.dll`.

The multisample test checks depth replay, shared-layer output, and state/reset
behavior. Final multisampled backbuffer pixel values and edge coverage have
not been compared against Hook5.

The DLL builds as 32-bit using MinGW. Its static imports remain KERNEL32,
msvcrt, ole32, and USER32; graphics runtimes are loaded dynamically.
Compiler warnings remain for function-pointer conversions, legacy unused
renderers, and an existing nonzero `__builtin_frame_address` call.

## Outstanding validation and limitations

- Conditional stencil-tested D3D8 depth writes invalidate that frame's mirror
  and suppress native liquid submission. The plugin logs this condition.
  Stencil-disabled draws, ALWAYS/NEVER stencil comparisons, and stencil passes
  that do not write depth do not require conditional stencil reconstruction.
  This is an outstanding compatibility limitation, not full parity.
- The D3D8 mirror is single-sampled even when the game uses antialiasing.
  Depth edge coverage and fallback D16 precision can differ from the scene.
- Native rendering needs a working D3D11 device (hardware or WARP) and the
  existing shader compiler. D3D8 additionally needs a usable readable depth
  format. Older driver/hardware configurations have not been tested.
- Native rendering currently copies full scene-viewport depth and color
  between APIs, and D3D8 replays depth-writing geometry. Gameplay frame time
  and memory use have not been measured. Performance is not claimed equal.
- Test programs exercise the adapter entry points. They do not prove that
  every TK17 startup path or cached graphics function table reaches those
  hooks. Other extensions' hook ordering and actual GUI transitions need
  in-game checking. Non-default GL framebuffer/PBO extension state and
  alternate camera/offscreen pipelines are outside current test coverage.
- Restart TK17 separately in native OpenGL, native DirectX, and Hook5. Compare
  the same scene and settings for visibility, opacity, attachment, collisions,
  contact links, interface layering, and frame rate. Also check resize/Alt-Tab.
  The existing Hook5 integration must remain enabled when testing Hook5.

## Build and rollback

Run from this directory with `C:\msys64\mingw32\bin` on PATH:

```powershell
gcc -m32 -shared -O2 -s -Wall -Wextra -static-libgcc NC-TK17-Liquids.c -o build/NC-TK17-Liquids.native-candidate.dll -lole32
gcc -m32 -O2 -static-libgcc native_renderer_test.c -o build/native_renderer_test.exe -lole32
gcc -m32 -O2 -static-libgcc native_api_test.c -o build/native_api_test.exe -lole32 -lopengl32 -lgdi32 -ld3d8
```

Real D3D8 tests need access to the desktop graphics driver; this environment's
sandbox returned D3DERR_NOTAVAILABLE, while execution outside it passed.

The previous installed DLL is backed up in
`build/before-native-renderers-20260907-223727/NC-TK17-Liquids.dll` with SHA-256
`61D427273766738E9C80A21E7A3A0D215ED39C70D34C2895EB73712B4837934C`.
To roll back, close TK17 and copy that DLL to
`The Klub 17/Binaries/NC-TK17-Liquids.dll` under the workspace root.
Current INI settings and renderer selection are not changed by installation.
