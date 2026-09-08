# NC-TK17-Liquids

## World-space model emitter (0.7.84, awaiting in-game verification)

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
The PoseEditor J-key reproduction remains open pending in-game confirmation.
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
