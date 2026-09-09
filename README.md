# NC-TK17-Liquids

## Native decal freeze (0.8.8, pending gameplay verification)

The 0.8.7 gameplay retest still showed animated decals. Its diagnostic log
confirmed only 19 frozen controls: many custom picks returned a native hit
(`result=9 count=1`, also `result=0 count=1`), failed the stricter particle
attachment check, and immediately entered TK17's stain creation path anyway.
The freeze capture incorrectly depended on that attachment check succeeding.

0.8.8 captures successful custom native picks independently of particle
attachment. The native routine at EXE+0x1f24ae skips creation for negative
results; nonnegative hits now enroll the resulting new controls for freezing.
The exact body-contact checks and existing contact retry policy are retained.
Diagnostics now report `native_hit`, `body_confirmed`, and `retried` separately.
The regression test replays the logged success/rejected-attachment combination:
it fails with the old capture gate and passes with the corrected gate. Negative
native results remain excluded even when result storage contains an old hit.
The room/body ownership regression also passes.

`[liquid_collision] native_decal_drip = false` captures each successful
custom pick's actual stain group during the native pick. Before 0.8.7, the
snapshot used the emitter selected before the update: the first pick could
discover its descriptor too late, another emitter could use a different group,
and native expiration could change the control count before creation.

Frozen controls use TK17's `BlendControl.Weight` setter at full weight.
The old direct write to `control+0x10` skipped the setter's notifications to
dependent animation evaluators. The setter is resolved through the same
object dispatch as the native update, without requiring PhysX. Before later
writes, the control must still belong to the live update's group and its weak
reference must still resolve to the captured object. Ordinary native contacts
are excluded; `true` allows native animation again.

`native_decal_freeze_test.c` passes against the real installed SYS weight setter
and simulated native updates covering descriptor discovery on first contact,
multiple groups, expiration before creation, ordinary-contact isolation,
configuration toggling, and stale controls. Build with the same MinGW flags as
the DLL, replacing `-shared` with the test source/executable paths; run from the
workspace root so the test can map `The Klub 17/Binaries/ThriXXX010278-SYS.dll`.
Visual confirmation in game is still required: start a fresh session with
`false`, create new liquid contacts, and check that their initial textures hold.
Switching to `true` should allow the native drip animation again. The log emits
`liquid native stain frozen ... mode=property-setter` for the first 24 captures.

## Native graphics import recovery (0.8.6, pending gameplay verification)

PhysX repeats graphics import installation in `on_create`. This can replace
Liquids' already-installed hook with its saved predecessor. The 0.8.3 retry
guard treated that replacement as a later wrapper and left Liquids disconnected.
The failed native session logged emissions but `graphics_device_hook=0`.

Retries now restore an existing hook only when the import is its exact saved
predecessor. Unknown later wrappers remain untouched; predecessor records are
not duplicated or rewritten. `hook_chain_test` reproduces this startup sequence
(fails before the fix, passes afterward). Native OpenGL and D3D8 real-device
tests, shared compositor/Hook5 isolation tests, and room/body ownership tests
pass. Native gameplay still requires a fresh user test. The paired PhysX DLL
and all configuration files remain unchanged.

## Explicit PhysX device registration (0.8.5, body following user-verified)

Live logs confirmed active PhysX updates, ready body colliders and attached
particle movement; the user confirmed correct body following and stationary
room contacts. A subsequent native-renderer test exposed the startup issue
described above.

Live inspection after 0.8.4 still showed no PhysX render tick and no ready
collider sets. The factory-chain changes did not restore the missing device
registration. This build pairs Liquids with a small PhysX API extension:
`NCTK17PhysX_RegisterD3D8DeviceV1`. Liquids passes the actual created device
before installing its own Present/EndScene hooks, so PhysX can install its
existing callbacks even if startup renderer discovery missed the device.
Repeated registration preserves subsequent plugin wrappers.

`NCTK17PhysX_BodyColliderStatusV1` reports update-loop activity, camera validity,
global enablement, and ready-person mask. Liquids logs status changes while
requesting colliders, enabling verification without hard-coded memory offsets.
The original PhysX source was rebuilt at the installed image base before any
changes: its machine-code section SHA-256 matched the installed DLL exactly
(`51DC33C10C016BFEDAFCAAC07BD4F53A75C2CFCE231630E83F2BB62810293689`).

`Development/NC-TK17-PhysX/device_registration_test.c` verifies callback
installation, predecessor preservation, idempotence, and status reporting.
The paired DLLs are staged in this project's build folder; both must be
installed for the new registration path. The installed originals are backed
up under `build/before-physx-registration-20260908-083756`.
No collider dimensions, physics settings, or room-contact guards are changed.

## Preserve resolved device factories (0.8.4, pending live verification)

The live 0.8.3 check still showed no PhysX render tick. Its per-module resolver
table correctly retained a PhysX resolver, but the globally cached device
factory still pointed to WebM. GetProcAddress results now receive distinct
wrappers bound to their exact previous Direct3DCreate8 factory. Direct imports
also retain their module-specific previous factory. The regression checks
two independently resolved factories invoked in reverse order, stable wrapper
reuse, and protection against wrapping the same wrapper recursively.

## Preserve renderer hook chains (0.8.3, pending live verification)

Read-only inspection of the user's running 0.8.2 session showed PhysX enabled
with a valid camera and fresh external-collider requests, but all four
collider sets remained unready. PhysX's render-loop entry flag was still zero.
Liquids' saved GetProcAddress and Present callbacks pointed into WebM rather
than reaching the PhysX renderer path.

Liquids used one saved GetProcAddress for every intercepted engine module,
although their imports could point at different plugin wrappers. It also
intercepted Direct3DCreate8 imports in other plugins, allowing a private/raw
import to populate the global saved creation function before the engine's
wrapped import was reached. This could bypass PhysX device initialization.

The resolver and SwapBuffers now dispatch to the previous import belonging
to the actual calling engine module. Startup retries do not replace wrappers
installed above Liquids later. Direct3DCreate8 import interception is limited
to the engine modules; other plugins retain their private imports. No PhysX
binary, attachment tolerance, or room-ownership guard is changed.

`hook_chain_test.c` exercises actual calls from two synthetic PE modules with
different previous resolvers and checks that later wrappers survive retries.
The existing ownership and visual regression tests are retained. The live
PhysX readiness and rendered body following must be rechecked after restart.

## Individual body verification (0.8.2, awaiting in-game verification)

The user confirmed 0.8.1 fixed room attachment but reported genuine body
contacts remaining in world space. Native confirmation was still limited to
contacts selected for cosmetic stains. With PhysX returning unavailable in
the recorded session, removing neighborhood inference left other body
contacts without an individual ownership check.

The live native picker callback now checks recent unclassified contacts
independently of stain selection and configured stain rate. It uses the
camera ray through each impact and retains the exact-hit distance guard;
it does not inherit ownership from nearby droplets. Queries use the current
game-thread geometry list before the caller's normal pick overwrites the
scratch results. Each callback allows 16 probes, with round-robin selection,
a 250 ms contact-age window, at most three attempts per contact, and at least
16 ms between attempts. No extra decals are generated by these probes.

The regression test now supplies multiple model hits and a room impact with
a model behind it through a fake picker: all genuine model contacts follow
the moving anchor, the room impact remains fixed, and retry limits hold even
with cosmetic stains disabled. This proves the shared logic, not the live
game's pick cadence or animated-mesh agreement. Rejected live probes log their
hit-to-impact distance to support that remaining in-game check. The hit
tolerance has not been widened.

## Room contact ownership (0.8.1, awaiting in-game verification)

Fixed a reproduced shared attachment bug: an unclassified depth contact could
inherit body attachment from another contact of the same emission up to 0.5
world units away. Removed that inference. Each droplet now needs its own
PhysX body classification or a native model pick at its actual impact point.
Unknown contacts stay fixed until individually confirmed; confirmed room
contacts cannot be reassigned by native picks or stale confirmation flags.

Native model rays are unbounded, so a successful pick alone was insufficient.
The native hit position must now match the queued impact within a small
particle-size tolerance (0.002 to 0.006 world units). This prevents a model
farther along the impact/camera ray from authorizing body attachment. Native
decal geometry handling is otherwise unchanged. When PhysX is unavailable,
unsampled contacts no longer follow based solely on a nearby confirmed droplet.

`room_ownership_test.c` reproduces the erroneous attachment against the 0.8.0
checkpoint and passes with the fix. It checks moving-body/nearby-room behavior
with connections on/off, individually confirmed moving contacts, distant and
invalid native hits, room ownership, stale flags, and recycled identities.
The connection drawing code and native renderer adapters are unchanged.

The user confirmed OpenGL, native DirectX, and Hook5 working in 0.8.0. That
installed build, source, configuration, and log are preserved in
`build/before-room-ownership-20260907-234238` before this fix.

## Native renderers (0.8.0, user-tested in all three modes)

Added OpenGL and Direct3D 8 adapters that use the existing D3D11 liquid
geometry, shader, and collision code through a transparent offscreen layer.
Hook5 keeps its existing callbacks and bypasses native depth capture and drawing.
The native modes therefore require the Windows D3D11 runtime and the shader
compiler used by the existing renderer, but do not require Hook5.

Real API tests pass for depth occlusion, opacity compositing, interface
transition, state restoration, non-default depth ranges, Direct3D 8 device
reset, and 2x multisampling on the tested NVIDIA driver. Existing simulation,
contact, settings, projection, and shader tests also pass.

The user subsequently confirmed operation in OpenGL, native DirectX, and Hook5
and accepted this build as a checkpoint. It is not a public release.
Conditional stencil-tested depth writes remain unsupported in the native
Direct3D 8 depth mirror. Native performance, actual game draw-hook coverage,
and exhaustive visual parity still require broader validation.
See [NATIVE-RENDERER-VALIDATION.md](NATIVE-RENDERER-VALIDATION.md) for coverage,
limitations, build commands, and rollback information.

## Contact UI bindings (0.7.91)

Registered the contact connection switch and both sliders in the DLL settings
table so in-game changes persist to their respective INI keys. Corrected the
Lua thickness key and label, normalized the Thickness control name, and matched
the distance range and fallback values to the plugin defaults. Existing saved
values remain unchanged. `contact_settings_test.c` generates an isolated INI,
checks all three defaults, exercises independent UI writes, and confirms that
startup preserves an existing config. In-game widget interaction still needs
confirmation after restarting TK17.

## Exact source anchor and contact settings (0.7.90, awaiting in-game verification)

The three connection settings now belong to `[liquid_contact_droplets]`.
Values in that section take precedence; old `[liquid_collision]` values are
still accepted when the new keys are absent. This installation preserves
`connect_droplets = true`, `connection_distance = 0.035`, and
`connection_thickness = 0.25`. Base opacity 0.8 and fade length 0.2 are retained.

The rendered stream root now uses the exact sampled end-joint position.
Removed the old render-only nozzle lag and screen-space bridge fading/relocation
that could detach the visible root from the source. Physical emission already
used the exact joint; particle trajectories and direction are unchanged.
Birth interpolation and strand discontinuity guards remain active. The anchor
test checks immediate motion even within the same clock tick. In-game checking
is still needed for the illustrated gap, particularly during fast movement.

## Contact connections and absolute base opacity (0.7.89, awaiting in-game verification)

Optional Hook5 connections join nearby landed droplets into a beaded deposit,
with a narrower middle between the fuller endpoint droplets. Configure under
`[liquid_contact_droplets]` (moved here in 0.7.90):

```ini
connect_droplets = true
connection_distance = 0.03
connection_thickness = 0.55
```

`connection_distance` is the maximum world-space gap (0.002 to 0.10).
`connection_thickness` controls the middle width relative to the endpoint
droplets (0.1 to 1.0). Missing `connect_droplets` defaults to false; it is enabled
in this installation. Connections are chosen only for new impacts and are
pinned to their endpoint particle identities. Each deposit has at most two
connections. They follow endpoint motion, fade with the weaker endpoint, and
stop drawing on expiration, pool reuse, excessive separation, or incompatible
body anchors. Secondary spray, different pulses/strands/owners, and unreliable
normals cannot link. Room connections additionally require supported depth
samples along the gap. Vertex space is reserved for the actual droplets.
This creates short local deposits, not flowing surface rivulets or persistent
connections between native stain decals.

`stream_base_opacity` now sets absolute center opacity at the live emitter.
The base contribution bypasses random particle alpha, newborn fade-in, the old
0.78/0.80 alpha caps, and the shader's core-opacity reduction. Thus 1.0 gives
an opaque center with a soft edge. `stream_base_fade_length` blends that
contribution into the existing downstream appearance over world-space distance;
zero disables the base region. Breakup and end-of-life fading still apply.
The user's values 1.0 and 0.4 are preserved.

`contact_render_test.c` checks contact-chain topology, unsupported room gaps,
tapered connector geometry, vertex reserve, and recycled endpoints. Its D3D11
WARP test runs the actual production shaders and reads back center alpha for
base opacity 0, 0.4, and 1, including a newborn particle and low core opacity.
It also checks soft edges, the downstream transition, and breakup suppression.
Build it with the usual MinGW flags plus `-lole32 -ld3d11`.

## Fuller viscous drops (0.7.88, user accepted as a checkpoint)

The visual target is a thick, cohesive fluid with bulbous detached drops.
Airborne trails are now capped relative to the drop's rendered world diameter
(2.4 diameters for emitted drops, 1.6 for secondary impact spray), so increasing
speed does not turn small drops into needles. Main drops are fuller, emitted
satellites no longer receive an additional serial-dependent width reduction,
and the tail starts at 76% of the head width instead of 50%. Existing 180 ms
rounding still ends in a round bead. Breakup necks retain at least 55% of their
width factor before the existing opacity transition completes.

This adjusts Hook5 geometry only; trajectories, detachment timing, emission,
impact response, spray counts, and the INI remain unchanged. It does not add a
physical viscosity solver. `flow_visuals_test.c` also checks bounded drop
proportions across speed/size ranges and the final round shape.

## Flow, breakup and impacts (0.7.87, user liked the result; requested fuller drops)

- New particles interpolate source position and direction at their birth
  times. Missing history, gaps over 100 ms, jumps over 0.35 world units, and
  direction reversals start a separate strand. Pulse gaps and capped emission
  work do not accumulate a delayed burst.
- Consecutive main-stream particles retain validated neighbor links and their
  launch spacing. Stretch narrows the ribbon and can detach it before the
  configured cohesion age. Detachment is persistent and uses world space;
  the former screen-gap breakup calculation is removed. This is a lightweight
  visual/particle model, not a volume-conserving fluid solver.
- Thickness bulges use emission time as their material coordinate. Taper
  follows particle age rather than changing curve indices.
- Contacts respond to impact speed, with a brief angle-dependent flattening
  that settles within 80 ms. Existing size, opacity, body attachment, and
  continued surface-spread settings still apply. Body normals are estimated
  from the existing PhysX signed-distance API; visible room normals are
  reconstructed from neighboring depth pixels. Unavailable or inconsistent
  normals retain the footprint response but produce no secondary spray.
- Strong inward impacts can emit two small outward-moving droplets, at most
  once per 100 ms per emitter and 24 active secondary droplets overall. They
  reserve four free pool slots before spawning, never evict existing liquid,
  do not create native stains or recursive spray, and have short lifetimes.
- Detached droplets shorten their trails and round out over 180 ms. A minimum
  diameter preserves their geometry when their velocity trail reaches zero.

`flow_visuals_test.c` covers moving-source interpolation, discontinuity guards,
stretch detachment, recycled particle slots, stable material coordinates,
normal and reversed-depth reconstruction, rejection of collider/depth seams,
impact response and spray limits, and continuous round-droplet geometry.
The existing flight, FOV projection, and native emitter-transform tests are
also part of validation. Tests do not establish appearance or performance in
game. Check a moving source, camera/FOV changes during breakup, gentle and
fast body/room contacts, and multiple simultaneous emitters.

No INI migration is required. Hook5 renders the new ribbon/contact/rounding
behavior; the D3D8/OpenGL fallback renderers retain their existing drawing
styles and share the improved particle motion.

## Smoother flight (0.7.86, visually improved in game per user confirmation)

Airborne particles now use an analytical gravity/linear-drag integration,
removing timestep-dependent trajectory differences during ordinary frames.
The existing 50 ms stall clamp remains. Particles born in one frame advance
only for the time since their individual accumulator crossing, reducing
same-frame bunching. Birth positions still use the current emitter transform;
historical moving-emitter transforms are not reconstructed.
Random launch-speed variation in the connected flow is reduced to keep
neighboring particles together; detached satellite spray retains its variation.
Existing pulse shaping, contact behavior, and visual settings remain available.

`flight_test.c` checks 30/60/144 Hz trajectory agreement, zero and very small
drag, and staggered newborn flight with collision sweep origins preserved.
`projection_test.c` also passes. These are numerical checks, not confirmation
of appearance in game. Compare stream continuity, arcs, and breakup at different
frame rates, then verify body and room contacts with a moving emitter.

## Consistent FOV projection (0.7.85, awaiting in-game verification)

Points rejected by the captured Hook5 projection now remain clipped instead
of being projected again with the default 45-degree FOV and zero depth.
`projection_test.c` reproduces the old near-plane failure with a wide-FOV
camera and checks the correction and the unavailable-matrix fallback.
The reported high-FOV double stream still requires in-game confirmation.

## World-space model emitter (0.7.84, confirmed in game)

Model emitters now evaluate the owning person's parent transforms from an
identity matrix through TK17's native local-transform evaluator. This avoids
using PoseEditor's cached model-view pivots when the GUI is hidden or the
emitter is offscreen. The evaluator preserves live source rotations, scale,
and pivot offsets. Missing or collapsed model chains skip emission instead
of substituting a camera direction or an unnamespaced person's joints.
The X-key viewpoint tool retains its existing camera-relative behavior.

`world_emitter_test.c` exercises the production resolver with the installed
SYS DLL's native evaluator, without starting the game. It checks hierarchy
order and scale, dirty local transforms, identity return behavior, independence
from camera/render caches, and rejection of missing or invalid chains.
Version 0.7.83 was insufficient: the native local evaluator also caches its
result, whereas PhysX writes source rotation fields directly without using
the property setter that invalidates that cache. Version 0.7.84 rebuilds
verified SJoint local matrices in private copies, leaving engine-owned data
unchanged. The regression test now reproduces stale identity and valid-cache
results using the real SJoint builder, verifies the corrected rotation, and
checks that the original source bytes remain unchanged.
The user confirmed the PoseEditor J-key reproduction fixed with 0.7.84.
With diagnostics enabled, `liquid world emitter sample` records position and
direction four times per second while a model emitter is active.

This is the first visual prototype for the future TK17 liquid system. It
detects both the model and X-key tool routes, emits a small bounded set of
world-space droplets, and applies gravity and drag. In Hook5, each model pulse
is reconstructed as a smooth, variable-width Catmull-Rom liquid ribbon. It
stays coherent near the source, bends with the physical particles, and then
progressively separates into smaller semi-transparent droplets. TK17's
original ray and stain system remains active beside the prototype.

Model emission is divided into short pulses. Their count, duration, spacing,
and progressive strength loss can be adjusted with `model_pulse_count`,
`model_pulse_duration`, `model_pulse_interval`, and `model_pulse_decay`.
Each pulse initially forms a connected stream and then separates into
droplets. `model_stream_cohesion` controls how long it stays connected, while
`model_satellite_chance` controls the amount of smaller separated spray.

Optional ejaculation audio is configured under `[ejaculation_sound]`. Set
`preset` to a folder name under
`Extensions/Liquids/Sounds/Ejaculation` to play one random MP3 or WAV from
that folder for every liquid pulse. The sound follows the emitting person's
`penis_jointEnd` in 3D. Set `preset = false` to disable future sounds; sounds
that have already started are allowed to finish. `min_distance` sets where
volume fading begins in metres, and `max_distance` sets where it becomes
silent.

The INI is watched while TK17 is running, and saved changes apply within
about half a second. `stream_thickness`, `stream_opacity`, and
`stream_bulge` tune the connected stream without changing detached-droplet
size. `draw_layer` emulates TK17 transparent draw priority while retaining
true scene depth: higher values win near-equal-depth ordering, while physically
closer hair and objects remain in front. Set it to `-1` to disable liquid
depth priority. Liquid tests scene depth but never writes to it. `core_opacity` controls a denser milky
center independently from the softly blended outer edge. The layer value is
translated into a small adjustable depth priority, so ordinary values such
as `39` fix near-surface transparent ordering without turning the liquid into
an always-on-top overlay.

The `[liquid_contact_droplets]` section controls the appearance of the custom
droplet left after liquid contacts a body or room surface. `size` and `opacity`
are independent from `[liquid_visuals]`, so changing them does not change the
airborne liquid or connected stream. If the section is omitted, both settings
fall back to their corresponding `[liquid_visuals]` values for compatibility.

The Hook5 model-emission prototype now reads a small asynchronous tile from
the scene depth buffer. Liquid stops at the first visible character, clothing,
toy, or room surface, leaves a fixed temporary contact splash, and fades there
for the configured `surface_hold` time. The splash does not stretch or slide
downward. This first collision pass currently applies to model emission only.
The diagnostic build also observes TK17's native ray-pick path so persistent
`sperm00`-`sperm15` model stains can be connected to custom contact points
without guessing unsafe engine object layouts.

The DLL currently records:

- `CumShot`, `Person_CumShot`, `InternalCum_Start`, and related commands;
- raw command arguments and common named fields;
- creation of sperm-ray, liquid-jet, cum-controller, and emitter objects;
- resolved Person01-04 and tool sperm-ray objects around each trigger;
- the runtime addresses and executable references for known liquid commands;
- the engine's `InternalCumControl`, `InternalCumEmitterMap`, and
  `InternalCumZone` type markers.
- call stacks, object memory, and named-pointer links for the shared
  `spermray01_init` and `spermray01_splash` activation path.

The log is written to:

`The Klub 17/Logs/NC-TK17-Liquids.log`

## Prototype test

1. Start TK17 and load PoseEditor or FreeMode with Person01 as a male or
   shemale model.
2. Trigger the model's normal ejaculation and check that the added liquid
   appears as a sequence of short, tapered pulses.
3. Press `X`, trigger the viewpoint tool, and check its separate duration.
4. Rotate or move the camera while droplets are airborne; gravity should stay
   in world space.
5. Close the game and inspect or share `Logs/NC-TK17-Liquids.log`.

Keeping both trigger routes in one session lets the log show whether they
converge on the same engine controller, emitter, and stain path.
