# POV implementation history

These notes record intermediate implementations and pending checks at the time.
The current behavior and user confirmation are documented in [README.md](README.md).

## Keep the pre-gesture crosshair sample (0.8.30)

The camera button-down callback now keeps the cached free-frame crosshair
instead of sampling a cursor that TK17 may already have recentered. The capture
path also checks the physical right-button state in the foreground game, so
recentering before the native lock flag or window callback cannot overwrite
that cache. Aim continues to follow the saved screen position through camera
movement and resumes live tracking when camera control releases.

Regression tests cover a warp before both the callback and native flag,
capture loss while the button remains held, repeated gestures and immediate
release tracking. Existing long-orbit, camera-transform, cancellation and
contact checks remain covered. In-game verification is still required.

## Preserve crosshair during camera gestures (0.8.29)

During TK17's locked-mouse camera state, custom POV aiming retains the last
free crosshair position in normalized screen coordinates instead of resetting
to the centre. The current camera transform still converts that screen target
to world space, so orbiting follows the saved screen location rather than a
fixed world point. Every camera-state update refreshes aim validity, including
gestures lasting longer than the ordinary stale-input timeout.

Right-button down captures the cursor before forwarding to TK17, covering the
frame where camera control warps it before the native locked flag arrives.
Button release resumes free tracking once the native lock also releases.
Focus loss, cancellation and disable clear saved state. With no prior free
cursor sample, locked input still defaults to centre. Camera input messages,
tool movement, launch physics and configuration remain unchanged.

Cursor regressions cover a recentered pointer, multi-second hold, camera
rotation/translation, release, native-lock fallback and the button-message
latch. Existing cancellation, cursor restoration, flight and POV emitter
checks remain covered; visual gameplay verification is still needed.

## POV cancellation and cursor visibility (0.8.28)

The existing `CleanUp` command (Shift+X) now cancels custom POV emission,
including during the start delay. It stops pending/current POV audio, clears
POV particles and queued contacts, and releases the native sequence state.
The original command still runs to clear native decals and finish native
tool teardown. Person emitters/audio are not cancelled by this addition.

While the selected custom POV tool remains active, hide the Windows pointer
inside the foreground game window and retain TK17's drawn crosshair. Restore
the saved pointer at completion, cancellation, disable, focus loss, non-client
movement or window destruction. A chained window procedure handles cursor
messages and forwards unrelated messages. Cursor position remains available
for aiming; Windows' ShowCursor reference count is never modified.

`pov_qol_test` covers cancellation, source isolation, start-delay cancellation,
cursor transitions and window-message forwarding with mocked Windows calls.
The native tracking/cleanup bridges and existing POV/flight tests also remain
covered. Actual crosshair/pointer appearance still needs in-game confirmation.

## Flatter POV arc (0.8.27)

POV launch uses twice the previous baseline speed (at least 12 units/s or
six times configured speed), with target flight time capped at 0.3 seconds.
Halving flight time reduces the upward gravity compensation while retaining
the fixed `pov_reach` aiming plane, shared pulse variation and normal gravity
after launch. The fallback tool-axis launch uses the same faster baseline.
Native tool movement, person launch speeds and config values are unchanged.

At the default 2.5-unit reach, trajectory tests sample closer surfaces at
0.75, 1.25 and 1.75 units. Arc lift falls by about 75% with drag 0.08 and by
more than 72% across tested drag values 0, 0.08 and 2 and pulse strengths
0.7 and 1.0. Target arrival remains accurate. This reduces close-range height
error; a fixed aiming plane still cannot align every nearer surface exactly.
Gameplay appearance remains to be checked after installing this version.

## Fixed POV reach (0.8.26)

`[liquids] pov_reach = 2.5` sets a fixed aiming plane ahead of the camera,
in game units (0.5–10). The actual mouse position controls direction, with
gravity/drag compensation toward that plane. This is a useful reach setting,
not a cutoff: liquid keeps flying until collision or lifetime expiry. Move
closer to distant surfaces, or increase the setting for a longer launch.

POV aiming no longer reads scene depth or switches to a one-unit fallback.
Moving the cursor, crossing a surface edge or delayed depth readback cannot
shorten the chosen reach. Depth collision and body-contact decals remain active.
The native tool motion, pulse timing, shared spread and person emitters are
unchanged. Pulse strength can vary flight time while keeping the fixed target.

The default is based on the approximately 2.5-unit model depth in the latest
gameplay log. Tests verify fixed reach across ten pulses and moving cursor
samples without depth readbacks, at 1, 2.5 and 4 units, plus config loading and
limits. The preferred visual balance still needs gameplay evaluation.

## Native tool motion and client-cursor targeting (0.8.25)

Remove the 0.8.24 controller-direction override. TK17 again owns the tool's
position and orientation; the lifetime gate simply lets its original cursor
tracking continue through all custom pulses and remaining flight.

Custom liquid now targets the actual cursor in the foreground game window's
client area, normalized to the full scene used for rendering and depth sampling.
It no longer preferentially treats native PickView channel-adjusted coordinates
as full-scene coordinates. This handles window borders, window position and
different client/render resolutions. Locked-centre input remains centred;
unavailable client input retains the existing native fallback. Other applications'
cursors are not sampled. Per-pulse logs include the selected source, cursor pixel
and client dimensions for in-game verification.

Tests cover client capture with synthetic Windows API inputs and pixel-to-aim,
world flight and the production renderer back to the expected cursor pixel,
including a turned camera and three client resolutions. Native tracking/cleanup,
POV settings, contacts and flight regressions are also covered. Visual gameplay
confirmation is still required; the tests do not establish in-game alignment.

## Continuous POV tracking and contact picking (0.8.24)

The native cursor-tracking branch previously stopped when its own pulse counter
reached zero. A signature-checked gate now keeps that branch active while custom
pulses, airborne particles or pending contacts remain, independently of decal
rearming. Native positioning continues, and the tool's orientation uses the same
tip-to-cursor, gravity-compensated launch calculation as custom particles.
Hook5's scene projection takes priority over the last D3D8 projection, which can
belong to the tool/UI pass. Native and disabled POV paths remain unchanged.

POV contact picks now supply the camera parent's MatrixVersion, matching native
PickView. The previous zero argument was incorrectly treated as flags; AppPick
actually rejects geometry whose matrix version differs. Both contact rays and
body verification use the correct version. No cursor-only decal fallback is used.

Regression coverage exercises the actual tracking/cleanup assembly bridges with
live register/x87 state, the exhausted native-counter case, disabled handling,
nonzero matrix versions, compensated tool alignment and projected cursor arrival
at left/center/right targets with an intentionally mismatched D3D8 projection.
Automated checks do not replace an in-game visual check of 0.8.24.

## POV lifetime, contact decals and stronger flight (0.8.23)

POV emission length is now the complete shared pulse sequence:
`count * duration + (count - 1) * interval`. The old `tool_duration` setting
is removed and ignored in older INIs. `tool_start_delay` still applies.
A signature-checked native cleanup gate keeps the selected tool visible
through the sequence, its remaining airborne particles and queued contacts.
It releases native cleanup when finished, and bypasses custom handling when
POV/master is off or a different tool is selected.

TK17 uses PickView for native POV stains and PickRay for person stains. A
caller-scoped PickView hook now replaces only native POV stain picks with
queued liquid impact rays. Cursor-only picks return a miss. Actual contacts
reuse body confirmation, mesh-normal projection and native decal freezing;
a failed contact never falls back to the cursor. The active POV descriptor
participates in the existing re-arm scheduler so late pulses still produce
contact decals after native animation timing ends.

POV launch targets the cursor scene point using the simulation's exact gravity
and linear-drag equation. Its speed baseline is at least 6 units/s or three
times configured speed, with flight time capped at 0.6 seconds and within the
particle lifetime. Pulse strength and spread retain variation; initial velocity
is capped at 100 units/s for extreme targets/settings. This changes only POV
launch velocity. Person speed, shared visuals, pulses and in-flight physics
remain unchanged.

`pov_contact_test` checks full pulse duration, remaining flight, disabled/other
tool isolation, the real cleanup hook/trampoline with live x87/register state,
contact-only decals, missed surfaces, descriptor scheduling and native fallback.
It integrates aimed launches at several distances, drag values and pulse
strengths and verifies arrival at the target. POV/menu, flight, room ownership,
native decal freeze and surface projector regressions pass. Gameplay validation
of this version's timing, visibility and visual strength remains pending.

## Visible POV tip and cursor convergence (0.8.22, pending gameplay verification)

The 0.8.21 gameplay capture showed a detached stream: the effect mesh is
offset from the visible tool, and its first ring is not the tool's tip.
Use the frontmost centre vertex of `Tool01:tool_mesh`, transformed by that
mesh's live ModelViewMatrix. Retain this mesh when Tool01 is named, with
the exact current mesh lookup taking priority. No ray-mesh offset is used.

Unproject the cursor at the scene depth and subtract the tip's view position
before normalizing. Previously the direction was camera-to-cursor, launching
parallel to the desired line and ignoring the displaced outlet. The existing
depth readback now includes the cursor during active POV emission, including
the start delay. Depth samples carry the matching cursor coordinates and time;
stale samples and background depth are rejected. Without usable depth (including
collision disabled), use a one-metre convergence plane beyond the tip. Keep
gravity, launch speed and pulse settings unchanged; distant targets may remain
beyond the physical range of the configured stream.

Tests check visible-mesh ownership separately from the effect mesh, tip geometry,
tip-to-cursor intersection at two depths, padded depth-buffer rows, sky rejection,
startup persistence, shared flow and compositor output. Diagnostics distinguish
`visible-tool-tip`, `cursor-scene-point`, and `cursor-convergence-plane`.

## POV outlet and crosshair aim (0.8.21, pending gameplay verification)

POV now transforms the native stream mesh's first vertex-ring centre through
the live spermray group ModelViewMatrix. This includes rotation and scale;
the old group rotation pivot placed particles inside the tool, where depth
collision stopped them immediately. The mesh animation is excluded so pulses
do not move the outlet down the stream.

Aim uses TK17's NativeStainUpdate input and its MouseToDevice/MouseToChannel
conversion, matching the native POV PickView crosshair. The current projection
unprojects this to a view ray, then the camera inverse rotates it into world
space. The retained tool's local stream axis supplies aim if crosshair data is
unavailable or stale. Gravity, spread and speed still affect the trajectory;
this does not force particles to hit a screen pixel. Person emitters and their
settings are unchanged. Diagnostics report outlet mode and aim source per burst.

POV tests cover the actual dispatch property/ABI, native outlet offset,
rotation/scale, crosshair conversion, asymmetric projection, stale data,
startup persistence, disabled isolation, shared physics and ribbon rendering.

## POV setting survives startup (0.8.20, pending gameplay verification)

The 0.8.19 gameplay log confirms ConfigEditor replayed a saved OFF value twice
at startup, writing `pov_enabled=false` before X was pressed. The installed DLL
was correct, but the requested custom effect had been disabled by these writes.

Ignore POV parameter notifications until its widget is built and synchronized
from the INI. Also ignore them during PrepareControls/BuildControls on later
menu rebuilds. Native notification chaining still runs, and genuine menu edits
after construction continue to persist normally. Other settings retain their
existing behavior. Diagnostics identify ignored initialization notifications
and report the preserved INI value when the POV control becomes ready.

`pov_settings_test` reproduces the overwrite in 0.8.19 and passes with this fix.
It checks startup/rebuild notifications, true/false/missing INI values, live
ON/OFF edits, synchronization feedback, and original callback forwarding.
The existing POV lifecycle, pulse, contact and compositor tests also pass.

## POV tool lifecycle fix (0.8.19, pending gameplay verification)

The 0.8.18 gameplay report showed the native effect still visible after enabling
POV. Tool ray nodes were tracked only if POV was already enabled during scene
construction, so a live enable missed the existing ray. X also discarded the
retained transforms even though TK17 reuses the loaded tool across commands.
The plugin now remembers native tool meshes while POV is off, hides them on X
when enabled, retains ownership through off/on toggles, and keeps the live
transform. Person ray visibility is unaffected by these tool-only operations.

The old fallback projected below the normal 45-degree view. Its replacement
uses the current projection (or Hook5 FOV/default fallback), at 90% screen
height and 0.25 units in front of the camera. The real tool origin takes
priority. Diagnostics report the selected origin once per emission, plus
the number of native tool nodes hidden on X.

The lifecycle regression fails with 0.8.18 and passes with this version.
Tests cover off/on/off/on without renaming or recreating the tool, retained
live transform resolution, model-ray isolation, fallback projection at
20/45/90/120 degrees, and the existing shared-flow/compositor checks.
Actual tool alignment still requires gameplay verification.

## POV tool liquids (0.8.18, lifecycle corrected above)

Set `[liquids] pov_enabled = true` or enable **POV Tool Liquids** in the
Liquids settings menu to replace the X-key tool ray with physics liquids.
The default is false, preserving native POV behavior. The master switch must
also be enabled. The tool shares model pulses, decay, cohesion, satellites,
physics, visuals, collision/contact settings, and pulse sounds. Existing
`tool_start_delay` controls the delay; the full pulse sequence controls duration.
Emission uses the native outlet transform and crosshair aim described above;
the resolver uses its bottom-centre viewport fallback if no live tool is found.
Penis-bone offsets and testicular retraction remain specific to persons.

The option reloads live. Disabling it restores tool ray nodes and clears only
POV particles/emission; enabling takes effect on the next tool command.
Check X, repeated shots, moving/turning the camera, body/room contacts, and
simultaneous person emission in game before distribution.

`pov_emitter_test` verifies default/off behavior, the menu binding, shared
particle properties, pulse timing, world-space emission, native contact queues,
and disabling POV without clearing person liquids. Its real D3D11 compositor
fixture compares model and POV ribbon output pixel-for-pixel and checks native
depth snapshots include POV particles. Flow, flight, body ownership, and native
compositor regressions also pass. The DLL builds with the existing MinGW script
flags; actual tool alignment and timing still require gameplay verification.

