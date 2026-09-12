#define WIN32_LEAN_AND_MEAN
#define CINTERFACE
#define COBJMACROS
#include <windows.h>
#include <objbase.h>
#include <tlhelp32.h>
#include <dshow.h>
#include <d3d8.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <GL/gl.h>
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(__GNUC__)
#define THISCALL __attribute__((thiscall))
#else
#define THISCALL __thiscall
#endif

#define LIQUIDS_VERSION "0.8.11-decal-vertical-flip"
#define LIQUID_EMITTER_OFFSET_METRES 0.01f
#define TK17_EXE_TIMESTAMP 0x56EF69A0u
#define TK17_EXE_IMAGE_SIZE 0x00312000u
#define TK17_EXE_CHECKSUM 0x00304A16u
#define APP_MAIN_COMMAND_NAME_STRINGREF_OFFSET 0x0003321Cu

typedef DWORD (__cdecl *app_main_command_t)(void *);
typedef unsigned int (THISCALL *stringref_hash32_t)(const void *);
typedef void *(THISCALL *namehash_find_t)(void *, unsigned int, const void *);
typedef void (THISCALL *object_i_name_set_t)(void *, const void *, const void *);
typedef void *(__cdecl *app_find_objc_t)(const char *);
typedef void *(THISCALL *get_weak_obj_target_t)(void *);
typedef const void *(THISCALL *object_get_type_info_t)(const void *);
typedef const void *(__cdecl *static_get_type_info_t)(void);
typedef void (__cdecl *model_pivot_t)(void *, float *);
typedef void (THISCALL *apptracker_set_world_matrix_inverse_t)(void *, const float *);
typedef int (THISCALL *app_pick_ray_t)(void *, const void *, const float *,
                                      const float *, void *, int,
                                      unsigned int);
typedef void *(__cdecl *clone_object_t)(const void *);
typedef void *(THISCALL *native_stain_cache_t)(void *, void *, void *);
typedef void (THISCALL *native_stain_update_t)(void *, void *, void *,
                                               void *);
typedef void (THISCALL *timer_set_seconds_t)(void *, double);
typedef void (THISCALL *script_u32_set_t)(void *, DWORD, unsigned int);
typedef void (THISCALL *configeditor_param_change_t)(
    void *, const char *, const char *, DWORD, DWORD);
typedef int (THISCALL *customizer_create_slider_t)(
    void *, void *, void *, void *, float, int, int);
typedef int (THISCALL *customizer_build_controls_t)(
    void *, void *, void *, void *);
typedef void (THISCALL *customizer_prepare_controls_t)(void *);
typedef void (THISCALL *script_get_string_t)(void *, DWORD, char **);
typedef void (THISCALL *engine_string_release_t)(char **);
typedef void (THISCALL *engine_string_construct_cstr_t)(char **,
                                                        const char *);
typedef int (THISCALL *script_array_count_t)(void *, DWORD);
typedef void (THISCALL *script_array_resize_t)(void *, DWORD, int, int);
typedef void (THISCALL *script_array_set_string_t)(void *, DWORD, int,
                                                   const char *);
typedef float (THISCALL *widget_get_float_t)(void *, DWORD);
typedef void (THISCALL *widget_set_float_t)(void *, DWORD, float);
typedef void (THISCALL *widget_set_string_t)(void *, DWORD, const char *);
typedef IDirect3D8 *(WINAPI *direct3d_create8_t)(UINT);
typedef HRESULT (WINAPI *d3d8_create_device_t)(IDirect3D8 *, UINT, D3DDEVTYPE,
                                               HWND, DWORD,
                                               D3DPRESENT_PARAMETERS *,
                                               IDirect3DDevice8 **);
typedef HRESULT (WINAPI *d3d8_present_t)(IDirect3DDevice8 *, const RECT *,
                                         const RECT *, HWND, const RGNDATA *);
typedef HRESULT (WINAPI *d3d8_end_scene_t)(IDirect3DDevice8 *);
typedef BOOL (WINAPI *swap_buffers_t)(HDC);
typedef FARPROC (WINAPI *get_proc_address_t)(HMODULE, LPCSTR);
typedef int (__cdecl *hook5_register_present_callback_t)(void *);
typedef int (__cdecl *hook5_get_fov_multiplier_t)(unsigned int, float *);
typedef int (__cdecl *hook5_get_projection_matrix_t)(unsigned int, float *);
typedef void (__cdecl *hook5_scene_callback_t)(
    ID3D11DeviceContext *, ID3D11RenderTargetView *,
    ID3D11DepthStencilView *, unsigned int, unsigned int);
typedef void (APIENTRY *liquid_gl_begin_t)(GLenum);
typedef void (APIENTRY *liquid_gl_end_t)(void);
typedef void (APIENTRY *liquid_gl_vertex2f_t)(GLfloat, GLfloat);
typedef void (APIENTRY *liquid_gl_color4ub_t)(GLubyte, GLubyte, GLubyte,
                                              GLubyte);
typedef void (APIENTRY *liquid_gl_get_floatv_t)(GLenum, GLfloat *);
typedef void (APIENTRY *liquid_gl_get_integerv_t)(GLenum, GLint *);
typedef void (APIENTRY *liquid_gl_push_attrib_t)(GLbitfield);
typedef void (APIENTRY *liquid_gl_pop_attrib_t)(void);
typedef void (APIENTRY *liquid_gl_disable_t)(GLenum);
typedef void (APIENTRY *liquid_gl_enable_t)(GLenum);
typedef void (APIENTRY *liquid_gl_blend_func_t)(GLenum, GLenum);
typedef void (APIENTRY *liquid_gl_matrix_mode_t)(GLenum);
typedef void (APIENTRY *liquid_gl_push_matrix_t)(void);
typedef void (APIENTRY *liquid_gl_pop_matrix_t)(void);
typedef void (APIENTRY *liquid_gl_load_identity_t)(void);
typedef void (APIENTRY *liquid_gl_ortho_t)(GLdouble, GLdouble, GLdouble,
                                           GLdouble, GLdouble, GLdouble);
typedef struct nc_tk17_physx_body_hit_v1_t {
    unsigned int size;
    unsigned int version;
    int person_index;
    int primitive_kind;
    int node_start;
    int node_end;
    float segment_t;
    float signed_distance;
} nc_tk17_physx_body_hit_v1_t;
typedef unsigned int (__cdecl *nc_physx_body_api_version_t)(void);
typedef int (__cdecl *nc_physx_request_body_colliders_t)(void);
typedef int (__cdecl *nc_physx_query_body_collider_t)(
    const float *, float, nc_tk17_physx_body_hit_v1_t *, unsigned int);
typedef void (__cdecl *nc_physx_post_animation_callback_t)(void);
typedef int (__cdecl *nc_physx_register_post_animation_callback_t)(
    nc_physx_post_animation_callback_t);
typedef int (__cdecl *nc_physx_set_blend_weight_t)(void *, float);
typedef int (__cdecl *nc_physx_set_blend_overlay_t)(void *, float, int);

typedef struct liquids_config_t {
    int enabled;
    int log_all_commands;
    int log_object_names;
    int dump_command_arguments;
    int probe_known_objects;
    int clear_log_on_startup;
    int max_events;
    int deep_event_capture;
    int deep_event_limit;
    int deep_event_bytes;
    int liquids_enabled;
    int particle_limit;
    float spawn_rate;
    float particle_lifetime;
    float model_duration;
    int model_pulse_count;
    float model_pulse_duration;
    float model_pulse_interval;
    float model_pulse_decay;
    float model_stream_cohesion;
    float model_satellite_chance;
    float tool_duration;
    float tool_start_delay;
    float emitter_position[3];
    int testicular_retraction_enabled;
    float testicular_retraction_target_weight;
    float testicular_retraction_time;
    float testicular_retraction_hold_time;
    float testicular_retraction_release_time;
    int testicular_retraction_easing;
    float testicular_retraction_bezier[4];
    int testicular_release_easing;
    float testicular_release_bezier[4];
    float speed;
    float spread;
    float gravity_strength;
    float drag;
    float particle_size;
    float stream_thickness;
    float stream_opacity;
    float contact_droplet_size;
    float contact_droplet_opacity;
    float stream_base_opacity;
    float stream_base_fade_length;
    float stream_wobble_start;
    float stream_wobble_fade_length;
    float stream_bulge;
    int stream_curve_smoothness;
    float stream_curve_tension;
    float core_opacity;
    int draw_layer;
    int collision_enabled;
    float collision_surface_hold;
    float collision_depth_tolerance;
    int collision_follow_bodies;
    float collision_surface_spread;
    int collision_connect_droplets;
    float collision_connection_distance;
    float collision_connection_thickness;
    int collision_spawn_model_stains;
    float collision_model_stain_rate;
    int collision_native_decal_drip;
    char ejaculation_sound_preset[128];
    float ejaculation_sound_volume;
    int ejaculation_sound_delay_ms;
    float ejaculation_sound_min_distance;
    float ejaculation_sound_max_distance;
} liquids_config_t;

typedef enum liquid_setting_type_t {
    LIQUID_SETTING_BOOL,
    LIQUID_SETTING_INT,
    LIQUID_SETTING_FLOAT,
    LIQUID_SETTING_TEXT
} liquid_setting_type_t;

typedef struct liquid_setting_binding_t {
    const char *param_name;
    const char *section;
    const char *key;
    liquid_setting_type_t type;
    void *slider_widget;
} liquid_setting_binding_t;

static liquid_setting_binding_t liquid_setting_bindings[] = {
    { "NcLiquidsEmitterPositionX", "liquid_emitter_position", "position_x", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsEmitterPositionY", "liquid_emitter_position", "position_y", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsEmitterPositionZ", "liquid_emitter_position", "position_z", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsMasterSwitch", "liquids", "enabled", LIQUID_SETTING_BOOL, NULL },
    { "NcLiquidsPhysicsSpeed", "liquid_physics", "speed", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsPhysicsSpread", "liquid_physics", "spread", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsPhysicsDrag", "liquid_physics", "drag", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsPhysicsGravity", "liquid_physics", "gravity_strength", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsAmount", "liquids", "spawn_rate", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsStreamCohesion", "liquids", "model_stream_cohesion", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsDropletAmount", "liquids", "model_satellite_chance", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsVisualSize", "liquid_visuals", "size", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsStreamThickness", "liquid_visuals", "stream_thickness", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsVisualOpacity", "liquid_visuals", "stream_opacity", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsContactDropletSize", "liquid_contact_droplets", "size", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsContactDropletOpacity", "liquid_contact_droplets", "opacity", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsContactConnectDroplets", "liquid_contact_droplets", "connect_droplets", LIQUID_SETTING_BOOL, NULL },
    { "NcLiquidsContactDropletConnectionDistance", "liquid_contact_droplets", "connection_distance", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsContactDropletConnectionThickness", "liquid_contact_droplets", "connection_thickness", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsParticleLimit", "liquids", "particle_limit", LIQUID_SETTING_INT, NULL },
    { "NcLiquidsStreamQuality", "liquid_visuals", "stream_curve_smoothness", LIQUID_SETTING_INT, NULL },
    { "NcLiquidsPulseCount", "liquids", "model_pulse_count", LIQUID_SETTING_INT, NULL },
    { "NcLiquidsPulseDuration", "liquids", "model_pulse_duration", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsPulseInterval", "liquids", "model_pulse_interval", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsPulseDecay", "liquids", "model_pulse_decay", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsCollisionEnabled", "liquid_collision", "enabled", LIQUID_SETTING_BOOL, NULL },
    { "NcLiquidsBodyStains", "liquid_collision", "spawn_model_stains", LIQUID_SETTING_BOOL, NULL },
    { "NcLiquidsAnimatedStains", "liquid_collision", "native_decal_drip", LIQUID_SETTING_BOOL, NULL },
    { "NcLiquidsStainAmount", "liquid_collision", "model_stain_rate", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsTesticularRetractionEnabled", "testicular_retraction", "enabled", LIQUID_SETTING_BOOL, NULL },
    { "NcLiquidsTesticularRetractionEasingRetraction", "testicular_retraction", "retraction_easing", LIQUID_SETTING_TEXT, NULL },
    { "NcLiquidsTesticularRetractionEasingRelease", "testicular_retraction", "release_easing", LIQUID_SETTING_TEXT, NULL },
    { "NcLiquidsTesticularRetractionStrength", "testicular_retraction", "target_weight", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsTesticularRetractionTimeRetraction", "testicular_retraction", "retraction_time", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsTesticularRetractionTimeHold", "testicular_retraction", "hold_time", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsTesticularRetractionTimeRelease", "testicular_retraction", "release_time", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsEjaculationSoundTarget", "ejaculation_sound", "preset", LIQUID_SETTING_TEXT, NULL },
    { "NcLiquidsEjaculationSoundVolume", "ejaculation_sound", "volume", LIQUID_SETTING_FLOAT, NULL },
    { "NcLiquidsEjaculationSoundDelay", "ejaculation_sound", "delay", LIQUID_SETTING_INT, NULL }
};

#define LIQUID_EASING_LINEAR 0
#define LIQUID_EASING_SMOOTH 1
#define LIQUID_EASING_BEZIER 2

#define LIQUID_PARTICLE_CAP 512
#define LIQUID_D3D11_VERTEX_CAP (LIQUID_PARTICLE_CAP * 36)
#define LIQUID_CURVE_SUBDIVISIONS_MAX 12
#define LIQUID_CURVE_SAMPLE_CAP \
    (LIQUID_PARTICLE_CAP * LIQUID_CURVE_SUBDIVISIONS_MAX + 1)
#define LIQUID_RUNTIME_SPLASH_RVA 0x00025527u
#define LIQUID_NATIVE_STAIN_CACHE_RVA 0x001F0E80u
#define LIQUID_NATIVE_STAIN_UPDATE_RVA 0x001F1F70u
#define LIQUID_NATIVE_STAIN_PICK_CALLER_RVA 0x001F24ACu
#define LIQUID_NATIVE_STAIN_MATRIX_EBP_OFFSET 0x124u
#define LIQUID_NATIVE_STAIN_PROJECTOR_RVA 0x001f2537u
#define LIQUID_NATIVE_CONTACT_SLOTS 256
#define LIQUID_NATIVE_CONTACT_MAX_AGE_MS 5000u
#define LIQUID_NATIVE_CONTACT_RETRY_WINDOW_MS 150u
#define LIQUID_NATIVE_CONTACT_RETRY_INTERVAL_MS 35u
#define LIQUID_NATIVE_CONTACT_MAX_RETRIES 3u
#define LIQUID_NATIVE_FROZEN_CONTROL_SLOTS 512
#define LIQUID_SPERMRAY_SOURCE_SLOTS 16
#define LIQUID_AUDIO_FILE_CAP 128
#define LIQUID_AUDIO_PENDING_CAP 32
#define LIQUID_AUDIO_VOICE_CAP 24
#define LIQUID_AUDIO_RENDER_TO_GAME_DISTANCE 100.0f
#define SCRIPT_OBJECT_META_BACK_OFFSET 0x018u
#define SCRIPT_OBJECT_DISPATCH_TABLE_OFFSET 0x10Cu
#define SCRIPT_OBJECT_BOOL_SET_OFFSET 0x184u
#define SCRIPT_PROPERTY_VISIBILITY 0x06FFF043u
#define CUSTOM_PARAMETER_NAME_MEMBER_RVA 0x002B0438u
#define CUSTOM_PARAMETER_DESCRIPTION_MEMBER_RVA 0x002B0448u
#define CUSTOM_PARAMETER_ENABLE_MEMBER_RVA 0x002B0440u
#define CUSTOM_PARAMETER_BOX_VALUE_ARRAY_MEMBER_RVA 0x002B0444u
#define CUSTOM_PARAMETER_BOX_DESCRIPTION_ARRAY_MEMBER_RVA 0x002B044Cu
#define ENGINE_EMPTY_STRING_RVA 0x002B0D80u
#define ENGINE_STRING_CSTR_CONSTRUCT_RVA 0x0024A9E4u
#define ENGINE_STRING_RELEASE_RVA 0x0024A9E8u
#define WIDGET_TEXT_MEMBER_ID 0x04FFF0EBu

typedef struct liquid_native_contact_t {
    volatile LONG state;
    DWORD tick;
    DWORD next_retry_tick;
    unsigned int emission_id;
    unsigned int particle_id;
    unsigned int retry_count;
    float impact_world[3];
    float origin_world[3];
    float direction_world[3];
} liquid_native_contact_t;

typedef struct liquid_native_frozen_control_t {
    void *update;
    void *wrapper;
    void *object;
    void *group;
    DWORD added_tick;
} liquid_native_frozen_control_t;

typedef struct liquid_native_freeze_capture_t {
    void *update;
    void *descriptor;
    void *group;
    int first_control;
    void *projector_frame;
    float projector_inverse[16];
} liquid_native_freeze_capture_t;

typedef struct liquid_spermray_source_person_t {
    void *source;
    int person_index;
} liquid_spermray_source_person_t;

typedef struct liquid_particle_t {
    int active;
    float position[3];
    float previous[3];
    float velocity[3];
    float age;
    float birth_delay;
    float emission_time;
    unsigned int stream_id;
    float launch_speed;
    int stream_neighbor;
    unsigned int stream_neighbor_order;
    float stream_rest_length;
    float stream_stretch;
    float stretch_breakup;
    float separation_age;
    int secondary;
    float lifetime;
    float size_scale;
    float opacity;
    unsigned int emission_id;
    unsigned int spawn_order;
    int source_kind;
    int pulse_index;
    int satellite;
    int collided;
    float collision_age;
    float impact_velocity[3];
    float impact_tangent[3];
    float impact_strength;
    float impact_grazing;
    float contact_normal[3];
    int contact_normal_valid;
    int contact_link_index;
    unsigned int contact_link_order;
    unsigned int contact_successor_order;
    int contact_person;
    /* 0=PhysX unavailable/not ready, -1=confirmed world/room contact,
       1..4=confirmed live PhysX body owner. */
    int contact_physx_person;
    int contact_anchor;
    int contact_anchor_end;
    float contact_segment_t;
    DWORD contact_attached_tick;
    volatile LONG model_contact_confirmed;
    volatile LONG model_contact_verified;
    DWORD model_contact_probe_tick;
    unsigned int model_contact_probe_count;
    float contact_local_offset[3];
    float contact_anchor_world[3];
    float last_visible[3];
    int has_last_visible;
} liquid_particle_t;

enum {
    LIQUID_CONTACT_ROOT,
    LIQUID_CONTACT_SPINE,
    LIQUID_CONTACT_HEAD,
    LIQUID_CONTACT_BREAST_L,
    LIQUID_CONTACT_BREAST_R,
    LIQUID_CONTACT_BUTT_L,
    LIQUID_CONTACT_BUTT_R,
    LIQUID_CONTACT_SHOULDER_L,
    LIQUID_CONTACT_SHOULDER_R,
    LIQUID_CONTACT_ELBOW_L,
    LIQUID_CONTACT_ELBOW_R,
    LIQUID_CONTACT_WRIST_L,
    LIQUID_CONTACT_WRIST_R,
    LIQUID_CONTACT_KNEE_L,
    LIQUID_CONTACT_KNEE_R,
    LIQUID_CONTACT_ANKLE_L,
    LIQUID_CONTACT_ANKLE_R,
    LIQUID_CONTACT_HIP_L,
    LIQUID_CONTACT_HIP_R,
    LIQUID_CONTACT_PENIS_01,
    LIQUID_CONTACT_PENIS_02,
    LIQUID_CONTACT_PENIS_03,
    LIQUID_CONTACT_TESTICLES_01,
    LIQUID_CONTACT_TESTICLES_02,
    LIQUID_CONTACT_ANCHOR_COUNT
};

typedef struct liquid_contact_anchor_t {
    int valid;
    float world[3];
} liquid_contact_anchor_t;

typedef struct liquid_contact_person_t {
    int valid;
    float horizontal[3];
    float vertical[3];
    float depth[3];
    liquid_contact_anchor_t anchors[LIQUID_CONTACT_ANCHOR_COUNT];
} liquid_contact_person_t;

typedef struct liquid_contact_segment_t {
    int start_anchor;
    int end_anchor;
} liquid_contact_segment_t;

#define LIQUID_COLLISION_READBACK_COUNT 3
#define LIQUID_COLLISION_TILE_SIZE 1024

typedef struct liquid_depth_particle_sample_t {
    int valid;
    unsigned int spawn_order;
    unsigned int emission_id;
    float age;
    float position[3];
    float previous[3];
    float velocity[3];
    float last_visible[3];
    int has_last_visible;
} liquid_depth_particle_sample_t;

typedef struct liquid_depth_snapshot_t {
    int valid;
    UINT left;
    UINT top;
    UINT width;
    UINT height;
    UINT scene_width;
    UINT scene_height;
    D3DMATRIX projection;
    D3DVIEWPORT8 viewport;
    float camera_inverse[16];
    liquid_depth_particle_sample_t particles[LIQUID_PARTICLE_CAP];
} liquid_depth_snapshot_t;

typedef struct liquid_emitter_t {
    int active;
    int source_kind; /* 1=model, 2=tool */
    int person_index;
    DWORD start_tick;
    DWORD end_tick;
    DWORD last_native_splash_tick;
    float spawn_accumulator;
    unsigned int emission_id;
    float current_position[3];
    float current_direction[3];
    DWORD transform_tick;
    unsigned int stream_id;
    int last_stream_particle;
    unsigned int last_stream_order;
    DWORD last_impact_spray_tick;
    int transform_valid;
    int pulse_emitting;
    int current_pulse_index;
    int sound_pulse_index;
    void *native_stain_descriptor;
} liquid_emitter_t;

typedef struct liquid_audio_file_t {
    char path[MAX_PATH * 4];
} liquid_audio_file_t;

typedef struct liquid_audio_pending_t {
    int active;
    DWORD due_tick;
    int person_index;
    int pulse_index;
    unsigned int emission_id;
    float volume;
    char path[MAX_PATH * 4];
} liquid_audio_pending_t;

typedef struct liquid_audio_voice_t {
    int active;
    DWORD started_tick;
    DWORD last_spatial_tick;
    int person_index;
    int pulse_index;
    unsigned int emission_id;
    float volume;
    long applied_volume;
    long applied_balance;
    IGraphBuilder *graph;
    IMediaControl *control;
    IMediaEventEx *events;
    void *basic_audio;
    char path[MAX_PATH * 4];
} liquid_audio_voice_t;

typedef struct liquid_testicular_retraction_state_t {
    void *control;
    unsigned int emission_id;
    int pulse_index;
    int active;
    DWORD trigger_tick;
    DWORD resolve_retry_tick;
    float start_contribution;
    float contribution;
    float baseline;
    float last_written;
    int last_written_valid;
    unsigned int applied_emission_id;
    int applied_pulse_index;
} liquid_testicular_retraction_state_t;

typedef struct liquid_vertex_t {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
} liquid_vertex_t;

typedef struct liquid_d3d11_vertex_t {
    float position[3];
    float uv[2];
    float color[4];
} liquid_d3d11_vertex_t;

typedef struct liquid_curve_point_t {
    float x;
    float y;
    float screen_depth;
    float half_width;
    float alpha;
    float base_alpha;
    float distance_from_nozzle;
    float emission_time;
    int particle_index;
} liquid_curve_point_t;

#define NAMED_OBJECT_SLOTS 8192
typedef struct named_object_t {
    volatile void *object;
    char name[96];
} named_object_t;

#define LIQUID_TRANSFORM_NODE_SLOTS 32
typedef struct liquid_transform_node_t {
    volatile void *object;
    char name[128];
} liquid_transform_node_t;

static HMODULE self_module;
static HMODULE liquid_physx_module;
static CRITICAL_SECTION log_lock;
static CRITICAL_SECTION contact_lock;
static int log_ready;
static char config_path[MAX_PATH * 4];
static char log_path[MAX_PATH * 4];
static char ejaculation_sound_root[MAX_PATH * 4];
#define LIQUID_AUDIO_PRESET_CAP 128
#define LIQUID_AUDIO_PRESET_NAME_CAP 128
static char liquid_audio_preset_names[LIQUID_AUDIO_PRESET_CAP]
                                     [LIQUID_AUDIO_PRESET_NAME_CAP];
static int liquid_audio_preset_count;
static liquids_config_t cfg;
static volatile LONG event_count;
static volatile LONG startup_started;
static volatile LONG shutdown_requested;
static volatile LONG customizer_prepare_controls_hook_installed;
static volatile LONG customizer_build_controls_hook_installed;
static volatile LONG app_command_hook_installed;
static volatile LONG configeditor_param_change_hook_installed;
static volatile LONG customizer_create_slider_hook_installed;
static volatile LONG object_name_hook_installed;
static volatile LONG address_scan_done;
static volatile LONG deep_event_count;
static volatile LONG graphics_hook_ready;
static volatile LONG camera_hook_ready;
static volatile LONG app_pick_ray_hook_installed;
static volatile LONG clone_object_hook_installed;
static volatile LONG native_stain_cache_hook_installed;
static volatile LONG native_stain_update_hook_installed;
static volatile LONG hook5_present_registered;
static volatile LONG hook5_d3d8_present_registered;
static volatile LONG hook5_d3d8_scene_registered;
static volatile LONG hook5_d3d11_scene_registered;
static volatile LONG hook5_d3d11_composite_registered;
static volatile LONG hook5_scene_drawn_since_present;
static volatile LONG hook5_composite_drawn_since_present;
static __thread int hook_recursion;
static __thread int liquid_setting_sync_depth;
static named_object_t named_objects[NAMED_OBJECT_SLOTS];
static liquid_transform_node_t
    liquid_transform_nodes[LIQUID_TRANSFORM_NODE_SLOTS];
static liquid_particle_t liquid_particles[LIQUID_PARTICLE_CAP];
#define LIQUID_MODEL_EMITTER_COUNT 4
static liquid_emitter_t model_emitters[LIQUID_MODEL_EMITTER_COUNT];
static liquid_emitter_t tool_emitter;
static liquid_testicular_retraction_state_t
    liquid_testicular_retraction_states[LIQUID_MODEL_EMITTER_COUNT];
static liquid_audio_file_t liquid_audio_files[LIQUID_AUDIO_FILE_CAP];
static liquid_audio_pending_t liquid_audio_pending[LIQUID_AUDIO_PENDING_CAP];
static liquid_audio_voice_t liquid_audio_voices[LIQUID_AUDIO_VOICE_CAP];
static int liquid_audio_file_count;
static int liquid_audio_last_file_index = -1;
static unsigned int liquid_audio_random_state = 0x4e434155u;
#define LIQUID_RECENT_NATIVE_RAY_NODES 8
static void *liquid_recent_native_ray_nodes[
    LIQUID_RECENT_NATIVE_RAY_NODES];
static unsigned int liquid_recent_native_ray_node_count;
#define LIQUID_HIDDEN_NATIVE_RAY_NODES 16
static void *liquid_hidden_native_ray_nodes[
    LIQUID_HIDDEN_NATIVE_RAY_NODES];
static unsigned int liquid_hidden_native_ray_node_count;
static DWORD liquid_last_tick;
static DWORD last_tool_command_tick;
static DWORD liquid_config_check_tick;
static FILETIME liquid_config_write_time;
static int liquid_config_write_time_valid;
static volatile LONG liquid_config_reload_lock;
static volatile LONG liquid_native_pick_probe_count;
static volatile LONG liquid_native_object_probe_count;
static volatile LONG liquid_native_stain_trace_until_tick;
static volatile LONG liquid_native_contact_log_count;
static volatile LONG liquid_native_rearm_log_count;
static volatile LONG liquid_native_animation_diag_generation;
static volatile LONG liquid_native_animation_diag_emission;
static volatile LONG liquid_native_animation_diag_particle;
static volatile LONG liquid_native_animation_diag_until_tick;
static volatile LONG liquid_native_animation_diag_last_tick;
static __thread liquid_native_freeze_capture_t *liquid_native_freeze_capture;
static void *tramp_NativeStainProjector __attribute__((used));
static int native_stain_projector_hook_installed;
static LONG liquid_native_animation_diag_snapshot_generation;
static void *liquid_native_animation_diag_snapshot_node;
static void *liquid_native_animation_diag_snapshot_control;
static BYTE liquid_native_animation_diag_node_previous[96];
static BYTE liquid_native_animation_diag_control_previous[96];
static liquid_native_contact_t
    liquid_native_contacts[LIQUID_NATIVE_CONTACT_SLOTS];
static liquid_native_frozen_control_t
    liquid_native_frozen_controls[LIQUID_NATIVE_FROZEN_CONTROL_SLOTS];
static unsigned int liquid_native_frozen_control_cursor;
static unsigned int liquid_native_frozen_control_limit;
static volatile void *liquid_native_stain_descriptor;
static volatile LONG liquid_native_stain_descriptor_emission;
static volatile LONG liquid_native_last_contact_queue_tick[
    LIQUID_MODEL_EMITTER_COUNT];
static volatile LONG liquid_latest_model_emission;
static unsigned int liquid_emission_serial;
static unsigned int liquid_particle_serial;
static volatile void *latest_spermray_group_object;
static volatile void *latest_spermray_splash_source_object;
static volatile void *tool_spermray_group_object;
static volatile void *tool_spermray_root_object;
static liquid_spermray_source_person_t
    liquid_spermray_source_people[LIQUID_SPERMRAY_SOURCE_SLOTS];
static unsigned int liquid_random_state = 0x4e43584cu;
static float captured_camera_inverse[16];
static volatile LONG captured_camera_inverse_valid;
static D3DMATRIX captured_d3d_projection;
static D3DVIEWPORT8 captured_d3d_viewport;
static volatile LONG captured_d3d_projection_valid;
static liquid_contact_person_t liquid_contact_people[4];
static liquid_contact_person_t liquid_contact_people_previous[4];
static liquid_contact_person_t liquid_contact_people_older[4];
static volatile LONG liquid_contact_cache_tick;
static volatile LONG liquid_contact_previous_cache_tick;
static volatile LONG liquid_contact_older_cache_tick;
static nc_physx_request_body_colliders_t
    liquid_physx_request_body_colliders;
static nc_physx_query_body_collider_t liquid_physx_query_body_collider;
static nc_physx_register_post_animation_callback_t
    liquid_physx_register_post_animation_callback;
static nc_physx_set_blend_weight_t liquid_physx_set_blend_weight;
static nc_physx_set_blend_overlay_t liquid_physx_set_blend_overlay;
static volatile LONG liquid_physx_retraction_callback_registered;
static DWORD liquid_physx_api_retry_tick;
static volatile LONG liquid_physx_api_logged;

static void liquid_apply_testicular_retraction_weights(void);

static void __cdecl liquid_physx_post_animation_callback(void)
{
    liquid_apply_testicular_retraction_weights();
}

static app_main_command_t real_AppMain_Command;
static configeditor_param_change_t tramp_ConfigEditor_ParamChange;
static customizer_create_slider_t tramp_Customizer_CreateSlider;
static customizer_build_controls_t tramp_Customizer_BuildControls;
static customizer_prepare_controls_t tramp_Customizer_PrepareControls;
static object_i_name_set_t tramp_Object_iNameSet;
static stringref_hash32_t engine_StringRefHash32;
static namehash_find_t engine_NameHashFind;
static app_find_objc_t engine_FindObjC;
static get_weak_obj_target_t engine_GetWeakObjTarget;
static object_get_type_info_t engine_ObjectGetTypeInfo;
static const void *engine_BlendControlTypeInfo;
static void **engine_G_NilWeakObjTarget_ptr;
static void **engine_G_NilObject_ptr;
static model_pivot_t engine_GetModelViewRotationPivot;
static apptracker_set_world_matrix_inverse_t tramp_AppTracker_SetWorldMatrixInverse;
static app_pick_ray_t tramp_AppPick_PickRay;
static clone_object_t tramp_CloneObject;
static native_stain_cache_t tramp_NativeStainCache;
static native_stain_update_t tramp_NativeStainUpdate;
static timer_set_seconds_t engine_TimerSetSeconds;
static direct3d_create8_t real_Direct3DCreate8;
static d3d8_create_device_t real_d3d8_CreateDevice;
static d3d8_present_t real_d3d8_Present;
static d3d8_end_scene_t real_d3d8_EndScene;
static swap_buffers_t real_SwapBuffers;
static get_proc_address_t real_GetProcAddress;
static hook5_register_present_callback_t hook5_register_present_callback;
static hook5_register_present_callback_t
    hook5_register_d3d8_present_callback;
static hook5_register_present_callback_t
    hook5_register_d3d8_scene_callback;
static hook5_register_present_callback_t
    hook5_register_scene_callback;
static hook5_register_present_callback_t
    hook5_register_composite_callback;
static hook5_get_fov_multiplier_t hook5_get_fov_multiplier;
static hook5_get_projection_matrix_t hook5_get_projection_matrix;
static IDirect3DDevice8 *liquid_d3d8_texture_device;
static IDirect3DTexture8 *liquid_d3d8_stream_texture;
static ID3D11Device *liquid_d3d11_device;
static ID3D11VertexShader *liquid_d3d11_vertex_shader;
static ID3D11PixelShader *liquid_d3d11_pixel_shader;
static ID3D11InputLayout *liquid_d3d11_input_layout;
static ID3D11Buffer *liquid_d3d11_vertex_buffer;
static ID3D11BlendState *liquid_d3d11_blend_state;
static ID3D11RasterizerState *liquid_d3d11_rasterizer_state;
static ID3D11DepthStencilState *liquid_d3d11_depth_state;
static ID3D11DepthStencilState *liquid_d3d11_depth_test_state;
static D3D11_COMPARISON_FUNC liquid_d3d11_depth_test_func =
    D3D11_COMPARISON_LESS_EQUAL;
static int liquid_d3d11_depth_test_draw_layer = -999;
static ID3D11Device *liquid_collision_depth_device;
static ID3D11Resource *liquid_collision_depth_source_resource;
static ID3D11ShaderResourceView *liquid_collision_depth_source_view;
static ID3D11Texture2D *liquid_collision_depth_float_texture;
static ID3D11UnorderedAccessView *liquid_collision_depth_float_view;
static ID3D11ComputeShader *liquid_collision_depth_compute_shader;
static ID3D11Buffer *liquid_collision_depth_constant_buffer;
static ID3D11Texture2D *liquid_collision_depth_staging
    [LIQUID_COLLISION_READBACK_COUNT];
static liquid_depth_snapshot_t liquid_collision_depth_snapshots
    [LIQUID_COLLISION_READBACK_COUNT];
static UINT liquid_collision_depth_format;
static UINT liquid_collision_depth_source_width;
static UINT liquid_collision_depth_source_height;
static int liquid_collision_depth_write_index;
static int liquid_collision_depth_ready_count;
static liquid_gl_begin_t liquid_gl_begin;
static liquid_gl_end_t liquid_gl_end;
static liquid_gl_vertex2f_t liquid_gl_vertex2f;
static liquid_gl_color4ub_t liquid_gl_color4ub;
static liquid_gl_get_floatv_t liquid_gl_get_floatv;
static liquid_gl_get_integerv_t liquid_gl_get_integerv;
static liquid_gl_push_attrib_t liquid_gl_push_attrib;
static liquid_gl_pop_attrib_t liquid_gl_pop_attrib;
static liquid_gl_disable_t liquid_gl_disable;
static liquid_gl_enable_t liquid_gl_enable;
static liquid_gl_blend_func_t liquid_gl_blend_func;
static liquid_gl_matrix_mode_t liquid_gl_matrix_mode;
static liquid_gl_push_matrix_t liquid_gl_push_matrix;
static liquid_gl_pop_matrix_t liquid_gl_pop_matrix;
static liquid_gl_load_identity_t liquid_gl_load_identity;
static liquid_gl_ortho_t liquid_gl_ortho;
static int liquid_gl_api_attempted;

static int liquid_patch_vtable_slot(void *object, int index, void *hook,
                                    void **previous);

static void copy_proc_address(void *destination, FARPROC source,
                              size_t destination_size)
{
    size_t copy_size = destination_size < sizeof(source) ?
                       destination_size : sizeof(source);
    memset(destination, 0, destination_size);
    memcpy(destination, &source, copy_size);
}

static int contains_i(const char *text, const char *needle)
{
    size_t n;
    if (!text || !needle) return 0;
    n = strlen(needle);
    if (!n) return 1;
    for (; *text; text++) {
        if (_strnicmp(text, needle, n) == 0) return 1;
    }
    return 0;
}

static int ptr_readable(const void *pointer, size_t bytes)
{
    MEMORY_BASIC_INFORMATION mbi;
    const BYTE *cur = (const BYTE*)pointer;
    const BYTE *end;
    if (!pointer) return 0;
    end = cur + bytes;
    if (end < cur) return 0;
    while (cur < end) {
        const BYTE *region_end;
        if (!VirtualQuery(cur, &mbi, sizeof(mbi))) return 0;
        if (mbi.State != MEM_COMMIT ||
            (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return 0;
        region_end = (const BYTE*)mbi.BaseAddress + mbi.RegionSize;
        if (region_end <= cur) return 0;
        cur = region_end;
    }
    return 1;
}

static int ptr_executable(const void *pointer)
{
    MEMORY_BASIC_INFORMATION mbi;
    DWORD protection;
    if (!pointer || !VirtualQuery(pointer, &mbi, sizeof(mbi)) ||
        mbi.State != MEM_COMMIT || (mbi.Protect & PAGE_GUARD)) return 0;
    protection = mbi.Protect & 0xffu;
    return protection == PAGE_EXECUTE ||
           protection == PAGE_EXECUTE_READ ||
           protection == PAGE_EXECUTE_READWRITE ||
           protection == PAGE_EXECUTE_WRITECOPY;
}

static int ptr_writable(const void *pointer, size_t bytes)
{
    MEMORY_BASIC_INFORMATION mbi;
    const BYTE *cur = (const BYTE*)pointer;
    const BYTE *end;
    if (!pointer) return 0;
    end = cur + bytes;
    if (end < cur) return 0;
    while (cur < end) {
        const BYTE *region_end;
        DWORD protection;
        if (!VirtualQuery(cur, &mbi, sizeof(mbi)) ||
            mbi.State != MEM_COMMIT ||
            (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return 0;
        protection = mbi.Protect & 0xffu;
        if (protection != PAGE_READWRITE &&
            protection != PAGE_WRITECOPY &&
            protection != PAGE_EXECUTE_READWRITE &&
            protection != PAGE_EXECUTE_WRITECOPY) return 0;
        region_end = (const BYTE*)mbi.BaseAddress + mbi.RegionSize;
        if (region_end <= cur) return 0;
        cur = region_end;
    }
    return 1;
}

static int safe_cstr_a(const char *text, size_t max_len)
{
    size_t i;
    if (!text) return 0;
    for (i = 0; i < max_len; i++) {
        unsigned char ch;
        if (!ptr_readable(text + i, 1)) return 0;
        ch = (unsigned char)text[i];
        if (!ch) return i > 0;
        if (ch < 32 || ch > 126) return 0;
    }
    return 0;
}

static const char *stringref_cstr_a(const void *ref)
{
    const char *direct;
    const char *indirect;
    if (!ref) return NULL;
    direct = (const char*)ref;
    if (safe_cstr_a(direct, 256)) return direct;
    if (!ptr_readable(ref, sizeof(char*))) return NULL;
    indirect = *(const char* const*)ref;
    return safe_cstr_a(indirect, 256) ? indirect : NULL;
}

static const void *stringref_from_cstr_a(const char *text)
{
    static __thread unsigned char storage[260];
    int len;
    if (!text) return NULL;
    len = (int)strlen(text);
    if (len <= 0 || len > 255) return NULL;
    *(int*)storage = len;
    memcpy(storage + sizeof(int), text, (size_t)len + 1);
    return storage + sizeof(int);
}

/* The native sperm ray is a renderable script object, separate from the
   native stain descriptor that we intentionally keep alive for model
   decals. Hide only that renderable node. Using the object's own script
   dispatch keeps this independent of Person01/02/03/04 scene paths, which
   are not available yet when the temporary ray hierarchy is constructed. */
static int liquid_set_script_visibility(void *object, unsigned int visible)
{
    BYTE *meta;
    BYTE *dispatch;
    script_u32_set_t setter;
    if (!object ||
        !ptr_readable((BYTE*)object - SCRIPT_OBJECT_META_BACK_OFFSET,
                      sizeof(void*))) return 0;
    meta = *(BYTE**)((BYTE*)object - SCRIPT_OBJECT_META_BACK_OFFSET);
    if (!meta ||
        !ptr_readable(meta + SCRIPT_OBJECT_DISPATCH_TABLE_OFFSET,
                      sizeof(void*))) return 0;
    dispatch = *(BYTE**)(meta + SCRIPT_OBJECT_DISPATCH_TABLE_OFFSET);
    if (!dispatch ||
        !ptr_readable(dispatch + SCRIPT_OBJECT_BOOL_SET_OFFSET,
                      sizeof(void*))) return 0;
    setter = *(script_u32_set_t*)(dispatch + SCRIPT_OBJECT_BOOL_SET_OFFSET);
    if (!ptr_executable((const void*)setter)) return 0;
    setter(object, SCRIPT_PROPERTY_VISIBILITY, visible ? 1u : 0u);
    return 1;
}

static void liquid_remember_recent_native_ray_node(void *object)
{
    unsigned int i;
    if (!object) return;
    for (i = 0; i < liquid_recent_native_ray_node_count; ++i) {
        if (liquid_recent_native_ray_nodes[i] == object) return;
    }
    if (liquid_recent_native_ray_node_count <
        LIQUID_RECENT_NATIVE_RAY_NODES) {
        liquid_recent_native_ray_nodes[
            liquid_recent_native_ray_node_count++] = object;
        return;
    }
    memmove(&liquid_recent_native_ray_nodes[0],
            &liquid_recent_native_ray_nodes[1],
            sizeof(liquid_recent_native_ray_nodes[0]) *
                (LIQUID_RECENT_NATIVE_RAY_NODES - 1));
    liquid_recent_native_ray_nodes[
        LIQUID_RECENT_NATIVE_RAY_NODES - 1] = object;
}

static void liquid_remember_hidden_native_ray_node(void *object)
{
    unsigned int i;
    if (!object) return;
    for (i = 0; i < liquid_hidden_native_ray_node_count; ++i) {
        if (liquid_hidden_native_ray_nodes[i] == object) return;
    }
    if (liquid_hidden_native_ray_node_count <
        LIQUID_HIDDEN_NATIVE_RAY_NODES) {
        liquid_hidden_native_ray_nodes[
            liquid_hidden_native_ray_node_count++] = object;
        return;
    }
    memmove(&liquid_hidden_native_ray_nodes[0],
            &liquid_hidden_native_ray_nodes[1],
            sizeof(liquid_hidden_native_ray_nodes[0]) *
                (LIQUID_HIDDEN_NATIVE_RAY_NODES - 1));
    liquid_hidden_native_ray_nodes[
        LIQUID_HIDDEN_NATIVE_RAY_NODES - 1] = object;
}

static int liquid_classify_recent_native_ray_nodes(int tool_owned)
{
    unsigned int i;
    int changed = 0;
    for (i = 0; i < liquid_recent_native_ray_node_count; ++i) {
        void *object = liquid_recent_native_ray_nodes[i];
        if (object) {
            changed += liquid_set_script_visibility(
                object, tool_owned ? 1u : 0u);
            if (!tool_owned)
                liquid_remember_hidden_native_ray_node(object);
        }
        liquid_recent_native_ray_nodes[i] = NULL;
    }
    liquid_recent_native_ray_node_count = 0;
    return changed;
}

/* A live INI reload can disable custom liquids after model spermray nodes
   have already been hidden. Restore only nodes hidden by this plugin, plus
   any provisional nodes still waiting for owner classification. Subsequent
   visibility changes are left entirely to TK17's native scheduler. */
static int liquid_restore_native_ray_nodes(void)
{
    unsigned int i;
    int changed = 0;
    for (i = 0; i < liquid_hidden_native_ray_node_count; ++i) {
        void *object = liquid_hidden_native_ray_nodes[i];
        if (object) changed += liquid_set_script_visibility(object, 1u);
        liquid_hidden_native_ray_nodes[i] = NULL;
    }
    liquid_hidden_native_ray_node_count = 0;
    for (i = 0; i < liquid_recent_native_ray_node_count; ++i) {
        void *object = liquid_recent_native_ray_nodes[i];
        if (object) changed += liquid_set_script_visibility(object, 1u);
        liquid_recent_native_ray_nodes[i] = NULL;
    }
    liquid_recent_native_ray_node_count = 0;
    return changed;
}

static void build_paths(void)
{
    char binary_dir[MAX_PATH * 4];
    char game_dir[MAX_PATH * 4];
    char extensions_dir[MAX_PATH * 4];
    char liquids_dir[MAX_PATH * 4];
    char *slash;
    config_path[0] = 0;
    log_path[0] = 0;
    ejaculation_sound_root[0] = 0;
    binary_dir[0] = 0;
    GetModuleFileNameA(self_module, binary_dir, sizeof(binary_dir));
    slash = strrchr(binary_dir, '\\');
    if (slash) *slash = 0;

    lstrcpynA(game_dir, binary_dir, sizeof(game_dir));
    slash = strrchr(game_dir, '\\');
    if (slash && _stricmp(slash + 1, "Binaries") == 0)
        *slash = 0;

    _snprintf(extensions_dir, sizeof(extensions_dir),
              "%s\\Extensions", game_dir);
    extensions_dir[sizeof(extensions_dir) - 1] = 0;
    CreateDirectoryA(extensions_dir, NULL);

    _snprintf(liquids_dir, sizeof(liquids_dir),
              "%s\\Liquids", extensions_dir);
    liquids_dir[sizeof(liquids_dir) - 1] = 0;
    CreateDirectoryA(liquids_dir, NULL);

    _snprintf(config_path, sizeof(config_path),
              "%s\\Config.ini", liquids_dir);
    config_path[sizeof(config_path) - 1] = 0;
    _snprintf(ejaculation_sound_root, sizeof(ejaculation_sound_root),
              "%s\\Sounds\\Ejaculation", liquids_dir);
    ejaculation_sound_root[sizeof(ejaculation_sound_root) - 1] = 0;
    _snprintf(log_path, sizeof(log_path),
              "%s\\Logs\\NC-TK17-Liquids.log", game_dir);
    log_path[sizeof(log_path) - 1] = 0;
}

static int liquid_audio_preset_name_compare(const void *left,
                                            const void *right)
{
    return _stricmp((const char *)left, (const char *)right);
}

/* Cache the preset directory names in memory. The settings script cannot scan
   raw directories, so these names are inserted through CustomParameter's
   native string-array API immediately before TK17 builds its controls. */
static void liquid_audio_scan_preset_names(void)
{
    char pattern[MAX_PATH * 4];
    WIN32_FIND_DATAA entry;
    HANDLE search;
    liquid_audio_preset_count = 0;
    if (!ejaculation_sound_root[0]) return;

    _snprintf(pattern, sizeof(pattern), "%s\\*", ejaculation_sound_root);
    pattern[sizeof(pattern) - 1] = 0;
    search = FindFirstFileA(pattern, &entry);
    if (search != INVALID_HANDLE_VALUE) {
        do {
            if (!(entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
                entry.cFileName[0] == '.' ||
                strchr(entry.cFileName, '\\') ||
                strchr(entry.cFileName, '/')) continue;
            if (liquid_audio_preset_count >= LIQUID_AUDIO_PRESET_CAP) break;
            lstrcpynA(liquid_audio_preset_names[liquid_audio_preset_count],
                      entry.cFileName,
                      LIQUID_AUDIO_PRESET_NAME_CAP);
            ++liquid_audio_preset_count;
        } while (FindNextFileA(search, &entry));
        FindClose(search);
    }
    if (liquid_audio_preset_count > 1)
        qsort(liquid_audio_preset_names,
              (size_t)liquid_audio_preset_count,
              sizeof(liquid_audio_preset_names[0]),
              liquid_audio_preset_name_compare);
}

static void create_default_config_if_missing(void)
{
    static const char default_config[] =
        "; NC-TK17-Liquids configuration\r\n"
        "; This file was generated from the plugin's built-in defaults.\r\n"
        "; Existing configuration files are never overwritten at startup.\r\n"
        "\r\n"
        "; enabled - Master switch for custom liquid simulation and rendering.\r\n"
        "; particle_limit - Maximum active particles (1 to 512).\r\n"
        "; spawn_rate - Particles emitted per second (1.0 to 240.0).\r\n"
        "; lifetime - Maximum airborne lifetime in seconds (0.1 to 30.0).\r\n"
        "; model_duration - Maximum model emission duration (0.1 to 30.0 seconds).\r\n"
        "; model_pulse_count - Pulses per model emission (1 to 24).\r\n"
        "; model_pulse_duration - Duration of each pulse (0.05 to 2.0 seconds).\r\n"
        "; model_pulse_interval - Delay between pulses (0.0 to 3.0 seconds).\r\n"
        "; model_pulse_decay - Strength lost by each successive pulse (0.0 to 0.8).\r\n"
        "; model_stream_cohesion - Time a pulse stays connected (0.02 to 2.0 seconds).\r\n"
        "; model_satellite_chance - Fraction emitted as separate droplets (0.0 to 0.75).\r\n"
        "; tool_duration - X-key tool emission duration (0.1 to 30.0 seconds).\r\n"
        "; tool_start_delay - Delay before the X-key tool ray (0.0 to 5.0 seconds).\r\n"
        "[liquids]\r\n"
        "enabled = true\r\n"
        "particle_limit = 150\r\n"
        "spawn_rate = 42.0\r\n"
        "lifetime = 5.0\r\n"
        "model_duration = 6.0\r\n"
        "model_pulse_count = 6\r\n"
        "model_pulse_duration = 0.28\r\n"
        "model_pulse_interval = 0.55\r\n"
        "model_pulse_decay = 0.10\r\n"
        "model_stream_cohesion = 0.72\r\n"
        "model_satellite_chance = 0.16\r\n"
        "tool_duration = 10.0\r\n"
        "tool_start_delay = 2.0\r\n"
        "\r\n"
        "; enabled - Retract the emitting model's testicles once per pulse.\r\n"
        "; target_weight - Additive Scrotum hard (266) pose weight (0.0 to 1.0).\r\n"
        "; retraction_time - Time to reach the target (0.0 to 5.0 seconds).\r\n"
        "; hold_time - Time held at full retraction (0.0 to 5.0 seconds).\r\n"
        "; release_time - Time to return to zero (0.0 to 5.0 seconds).\r\n"
        "; easing values: linear, smooth, or cubic-Bezier x1,y1,x2,y2.\r\n"
        "[testicular_retraction]\r\n"
        "enabled = true\r\n"
        "target_weight = 0.50\r\n"
        "retraction_time = 0.10\r\n"
        "hold_time = 0.50\r\n"
        "release_time = 0.20\r\n"
        "retraction_easing = smooth\r\n"
        "release_easing = smooth\r\n"
        "\r\n"
        "; speed - Initial launch speed (0.01 to 100.0).\r\n"
        "; spread - Random directional variation (0.0 to 2.0).\r\n"
        "; gravity_strength - Gravity multiplier (-10.0 to 10.0).\r\n"
        "; drag - Air resistance (0.0 to 20.0).\r\n"
        "[liquid_physics]\r\n"
        "speed = 4.0\r\n"
        "spread = 0.15\r\n"
        "gravity_strength = 1.0\r\n"
        "drag = 0.08\r\n"
        "\r\n"
        "; size - Base particle and ribbon size (0.001 to 0.2).\r\n"
        "; stream_thickness - Connected-stream thickness multiplier (0.4 to 3.0).\r\n"
        "; opacity values use 0.0 (transparent) to 1.0 (opaque).\r\n"
        "; fade lengths and wobble start use world-space distance (0.0 to 2.0).\r\n"
        "; stream_bulge - Natural thickness variation (0.0 to 0.5).\r\n"
        "; stream_curve_smoothness - Samples between particles (1 to 12).\r\n"
        "; stream_curve_tension - Curve strength (0.0 to 1.0).\r\n"
        "; draw_layer - Transparent priority (-1 to 100); -1 disables bias.\r\n"
        "[liquid_visuals]\r\n"
        "size = 0.018\r\n"
        "stream_thickness = 1.30\r\n"
        "stream_opacity = 0.86\r\n"
        "stream_base_opacity = 0.15\r\n"
        "stream_base_fade_length = 0.08\r\n"
        "stream_wobble_start = 0.03\r\n"
        "stream_wobble_fade_length = 0.10\r\n"
        "stream_bulge = 0.14\r\n"
        "stream_curve_smoothness = 5\r\n"
        "stream_curve_tension = 0.5\r\n"
        "core_opacity = 0.72\r\n"
        "draw_layer = 39\r\n"
        "\r\n"
        "; size - Base size for droplets after contact with a scene surface (0.001 to 0.2).\r\n"
        "; opacity - Transparency for droplets after contact with a scene surface (0.0 to 1.0).\r\n"
        "[liquid_contact_droplets]\r\n"
        "size = 0.0025\r\n"
        "opacity = 0.56\r\n"
        "; Join nearby landed droplets; distance is in world units, thickness is relative.\r\n"
        "connect_droplets = false\r\n"
        "connection_distance = 0.03\r\n"
        "connection_thickness = 0.55\r\n"
        "\r\n"
        "; enabled - Stop model liquid at the first visible scene surface.\r\n"
        "; surface_hold - Contact visibility duration (0.05 to 10.0 seconds).\r\n"
        "; depth_tolerance - Contact precision allowance (0.00005 to 0.05).\r\n"
        "; follow_bodies - Attach landed droplets to a moving person.\r\n"
        "; surface_spread - Contact widening amount (0.0 to 3.0).\r\n"
        "; spawn_model_stains - Create native sperm00-15 stains on contact.\r\n"
        "; model_stain_rate - Maximum stain requests per second (1.0 to 60.0).\r\n"
        "; native_decal_drip - Allow native stains to animate and fade.\r\n"
        "[liquid_collision]\r\n"
        "enabled = true\r\n"
        "surface_hold = 0.65\r\n"
        "depth_tolerance = 0.0015\r\n"
        "follow_bodies = false\r\n"
        "surface_spread = 0.45\r\n"
        "spawn_model_stains = true\r\n"
        "model_stain_rate = 12.0\r\n"
        "native_decal_drip = true\r\n"
        "\r\n"
        "; Offset from penis_jointEnd; -1..1 means -1..1 cm per axis. Zero preserves the original position.\r\n"
        "; X: left/right, Y: down/up, Z: back/forward along emission; axes rotate with the bone.\r\n"
        "[liquid_emitter_position]\r\n"
        "position_x = 0.0\r\n"
        "position_y = 0.0\r\n"
        "position_z = 0.0\r\n"
        "\r\n"
        "; preset - Folder under Sounds\\Ejaculation, or false to disable.\r\n"
        "; volume - Playback level from -10 (near silent) to 10 (loud).\r\n"
        "; delay - Delay before each pulse sound in milliseconds (0 to 500).\r\n"
        "; min_distance - Distance in metres where fading begins.\r\n"
        "; max_distance - Distance in metres where the sound becomes silent.\r\n"
        "[ejaculation_sound]\r\n"
        "preset = false\r\n"
        "volume = 0\r\n"
        "delay = 0\r\n"
        "min_distance = 1.5\r\n"
        "max_distance = 12.0\r\n"
        "\r\n"
        "; enabled - Master switch for diagnostic logging.\r\n"
        "; Keep this false for normal gameplay.\r\n"
        "; max_events - Session event limit (100 to 50000).\r\n"
        "; deep_event_limit - Deep event limit (10 to 2000).\r\n"
        "; deep_event_bytes - Bytes per deep event (64 to 1024).\r\n"
        "[diagnostics]\r\n"
        "enabled = false\r\n"
        "log_all_commands = false\r\n"
        "log_object_names = true\r\n"
        "dump_command_arguments = true\r\n"
        "probe_known_objects = true\r\n"
        "clear_log_on_startup = true\r\n"
        "max_events = 3000\r\n"
        "deep_event_capture = true\r\n"
        "deep_event_limit = 200\r\n"
        "deep_event_bytes = 256\r\n";
    HANDLE file;
    DWORD written;
    BOOL write_ok;

    file = CreateFileA(config_path, GENERIC_WRITE, FILE_SHARE_READ,
                       NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return;
    write_ok = WriteFile(file, default_config,
                         (DWORD)(sizeof(default_config) - 1),
                         &written, NULL);
    CloseHandle(file);
    if (!write_ok || written != (DWORD)(sizeof(default_config) - 1))
        DeleteFileA(config_path);
}

static void log_line(const char *fmt, ...);

static int read_bool(const char *section, const char *key, int fallback)
{
    char value[32];
    GetPrivateProfileStringA(section, key, fallback ? "true" : "false",
                             value, sizeof(value), config_path);
    return _stricmp(value, "true") == 0 ||
           _stricmp(value, "yes") == 0 ||
           strcmp(value, "1") == 0 ||
           _stricmp(value, "on") == 0;
}

static float read_float(const char *section, const char *key, float fallback)
{
    char fallback_text[64];
    char value[64];
    char *end = NULL;
    double parsed;
    _snprintf(fallback_text, sizeof(fallback_text), "%.6g", fallback);
    fallback_text[sizeof(fallback_text) - 1] = 0;
    GetPrivateProfileStringA(section, key, fallback_text,
                             value, sizeof(value), config_path);
    parsed = strtod(value, &end);
    if (end == value || !_finite(parsed)) return fallback;
    return (float)parsed;
}

static void read_trimmed_string(const char *section, const char *key,
                                const char *fallback, char *out,
                                size_t out_size)
{
    char *start;
    char *end;
    if (!out || !out_size) return;
    out[0] = 0;
    GetPrivateProfileStringA(section, key, fallback ? fallback : "",
                             out, (DWORD)out_size, config_path);
    out[out_size - 1] = 0;
    start = out;
    while (*start == ' ' || *start == '\t') ++start;
    if (start != out) memmove(out, start, strlen(start) + 1);
    end = out + strlen(out);
    while (end > out &&
           (end[-1] == ' ' || end[-1] == '\t' ||
            end[-1] == '\r' || end[-1] == '\n'))
        *--end = 0;
}

static void read_easing(const char *key, int *mode, float bezier[4])
{
    char value[128];
    char *start;
    char *end;
    char trailing;
    float x1, y1, x2, y2;
    int valid = 0;
    GetPrivateProfileStringA("testicular_retraction", key, "smooth",
                             value, sizeof(value), config_path);
    start = value;
    while (*start == ' ' || *start == '\t') start++;
    end = start + strlen(start);
    while (end > start &&
           (end[-1] == ' ' || end[-1] == '\t' ||
            end[-1] == '\r' || end[-1] == '\n'))
        *--end = 0;
    if (_stricmp(start, "linear") == 0) {
        *mode = LIQUID_EASING_LINEAR;
        return;
    }
    if (_stricmp(start, "smooth") == 0) {
        *mode = LIQUID_EASING_SMOOTH;
        return;
    }
    if (sscanf(start, " %f , %f , %f , %f %c",
               &x1, &y1, &x2, &y2, &trailing) == 4 &&
        _finite(x1) && _finite(y1) && _finite(x2) && _finite(y2) &&
        x1 >= 0.0f && x1 <= 1.0f && x2 >= 0.0f && x2 <= 1.0f &&
        y1 >= -2.0f && y1 <= 2.0f && y2 >= -2.0f && y2 <= 2.0f) {
        valid = 1;
    }
    if (valid) {
        *mode = LIQUID_EASING_BEZIER;
        bezier[0] = x1;
        bezier[1] = y1;
        bezier[2] = x2;
        bezier[3] = y2;
        return;
    }
    *mode = LIQUID_EASING_SMOOTH;
    if (cfg.enabled)
        log_line("invalid [testicular_retraction] %s=\"%s\"; using smooth",
                 key, start);
}

static void load_config(void)
{
    cfg.enabled = read_bool("diagnostics", "enabled", 0);
    cfg.log_all_commands = read_bool("diagnostics", "log_all_commands", 0);
    cfg.log_object_names = read_bool("diagnostics", "log_object_names", 1);
    cfg.dump_command_arguments =
        read_bool("diagnostics", "dump_command_arguments", 1);
    cfg.probe_known_objects =
        read_bool("diagnostics", "probe_known_objects", 1);
    cfg.clear_log_on_startup =
        read_bool("diagnostics", "clear_log_on_startup", 1);
    cfg.max_events = GetPrivateProfileIntA("diagnostics", "max_events",
                                           3000, config_path);
    if (cfg.max_events < 100) cfg.max_events = 100;
    if (cfg.max_events > 50000) cfg.max_events = 50000;
    cfg.deep_event_capture =
        read_bool("diagnostics", "deep_event_capture", 1);
    cfg.deep_event_limit = GetPrivateProfileIntA(
        "diagnostics", "deep_event_limit", 200, config_path);
    if (cfg.deep_event_limit < 10) cfg.deep_event_limit = 10;
    if (cfg.deep_event_limit > 2000) cfg.deep_event_limit = 2000;
    cfg.deep_event_bytes = GetPrivateProfileIntA(
        "diagnostics", "deep_event_bytes", 256, config_path);
    if (cfg.deep_event_bytes < 64) cfg.deep_event_bytes = 64;
    if (cfg.deep_event_bytes > 1024) cfg.deep_event_bytes = 1024;
    cfg.deep_event_bytes &= ~3;

    cfg.liquids_enabled = read_bool("liquids", "enabled", 1);
    cfg.particle_limit = GetPrivateProfileIntA(
        "liquids", "particle_limit", 150, config_path);
    if (cfg.particle_limit < 1) cfg.particle_limit = 1;
    if (cfg.particle_limit > LIQUID_PARTICLE_CAP)
        cfg.particle_limit = LIQUID_PARTICLE_CAP;
    cfg.spawn_rate = read_float("liquids", "spawn_rate", 42.0f);
    cfg.particle_lifetime = read_float("liquids", "lifetime", 5.0f);
    cfg.model_duration = read_float("liquids", "model_duration", 6.0f);
    cfg.model_pulse_count = GetPrivateProfileIntA(
        "liquids", "model_pulse_count", 6, config_path);
    cfg.model_pulse_duration = read_float(
        "liquids", "model_pulse_duration", 0.28f);
    cfg.model_pulse_interval = read_float(
        "liquids", "model_pulse_interval", 0.55f);
    cfg.model_pulse_decay = read_float(
        "liquids", "model_pulse_decay", 0.10f);
    cfg.model_stream_cohesion = read_float(
        "liquids", "model_stream_cohesion", 0.72f);
    cfg.model_satellite_chance = read_float(
        "liquids", "model_satellite_chance", 0.16f);
    cfg.tool_duration = read_float("liquids", "tool_duration", 10.0f);
    cfg.tool_start_delay = read_float("liquids", "tool_start_delay", 2.0f);
    {
        static const char *keys[] = { "position_x", "position_y", "position_z" };
        int axis;
        for (axis = 0; axis < 3; axis++) {
            float value = read_float("liquid_emitter_position", keys[axis], 0.0f);
            if (!_finite(value)) value = 0.0f;
            cfg.emitter_position[axis] = fminf(1.0f, fmaxf(-1.0f, value));
        }
    }
    cfg.testicular_retraction_enabled = read_bool(
        "testicular_retraction", "enabled", 1);
    cfg.testicular_retraction_target_weight = read_float(
        "testicular_retraction", "target_weight", 0.50f);
    cfg.testicular_retraction_time = read_float(
        "testicular_retraction", "retraction_time", 0.10f);
    cfg.testicular_retraction_hold_time = read_float(
        "testicular_retraction", "hold_time", 0.50f);
    cfg.testicular_retraction_release_time = read_float(
        "testicular_retraction", "release_time", 0.20f);
    read_easing("retraction_easing",
                &cfg.testicular_retraction_easing,
                cfg.testicular_retraction_bezier);
    read_easing("release_easing",
                &cfg.testicular_release_easing,
                cfg.testicular_release_bezier);
    cfg.speed = read_float("liquid_physics", "speed", 4.0f);
    cfg.spread = read_float("liquid_physics", "spread", 0.15f);
    cfg.gravity_strength =
        read_float("liquid_physics", "gravity_strength", 1.0f);
    cfg.drag = read_float("liquid_physics", "drag", 0.08f);
    cfg.particle_size = read_float("liquid_visuals", "size", 0.018f);
    cfg.stream_thickness = read_float(
        "liquid_visuals", "stream_thickness", 1.30f);
    cfg.stream_opacity = read_float(
        "liquid_visuals", "stream_opacity", 0.86f);
    cfg.contact_droplet_size = read_float(
        "liquid_contact_droplets", "size", cfg.particle_size);
    cfg.contact_droplet_opacity = read_float(
        "liquid_contact_droplets", "opacity", cfg.stream_opacity);
    cfg.stream_base_opacity = read_float(
        "liquid_visuals", "stream_base_opacity", 0.15f);
    cfg.stream_base_fade_length = read_float(
        "liquid_visuals", "stream_base_fade_length", 0.08f);
    cfg.stream_wobble_start = read_float(
        "liquid_visuals", "stream_wobble_start", 0.03f);
    cfg.stream_wobble_fade_length = read_float(
        "liquid_visuals", "stream_wobble_fade_length", 0.10f);
    cfg.stream_bulge = read_float(
        "liquid_visuals", "stream_bulge", 0.14f);
    cfg.stream_curve_smoothness = GetPrivateProfileIntA(
        "liquid_visuals", "stream_curve_smoothness", 5, config_path);
    cfg.stream_curve_tension = read_float(
        "liquid_visuals", "stream_curve_tension", 0.5f);
    cfg.core_opacity = read_float(
        "liquid_visuals", "core_opacity", 0.72f);
    cfg.draw_layer = GetPrivateProfileIntA(
        "liquid_visuals", "draw_layer", 39, config_path);
    cfg.collision_enabled = read_bool(
        "liquid_collision", "enabled", 1);
    cfg.collision_surface_hold = read_float(
        "liquid_collision", "surface_hold", 0.65f);
    cfg.collision_depth_tolerance = read_float(
        "liquid_collision", "depth_tolerance", 0.0015f);
    cfg.collision_follow_bodies = read_bool(
        "liquid_collision", "follow_bodies", 0);
    cfg.collision_surface_spread = read_float(
        "liquid_collision", "surface_spread", 0.45f);
    cfg.collision_connect_droplets = read_bool(
        "liquid_contact_droplets", "connect_droplets",
        read_bool("liquid_collision", "connect_droplets", 0));
    cfg.collision_connection_distance = read_float(
        "liquid_contact_droplets", "connection_distance",
        read_float("liquid_collision", "connection_distance", 0.03f));
    cfg.collision_connection_thickness = read_float(
        "liquid_contact_droplets", "connection_thickness",
        read_float("liquid_collision", "connection_thickness", 0.55f));
    cfg.collision_spawn_model_stains = read_bool(
        "liquid_collision", "spawn_model_stains", 1);
    cfg.collision_model_stain_rate = read_float(
        "liquid_collision", "model_stain_rate", 12.0f);
    cfg.collision_native_decal_drip = read_bool(
        "liquid_collision", "native_decal_drip", 1);
    read_trimmed_string("ejaculation_sound", "preset", "false",
                        cfg.ejaculation_sound_preset,
                        sizeof(cfg.ejaculation_sound_preset));
    cfg.ejaculation_sound_volume = read_float(
        "ejaculation_sound", "volume", 0.0f);
    cfg.ejaculation_sound_delay_ms = GetPrivateProfileIntA(
        "ejaculation_sound", "delay", 0, config_path);
    cfg.ejaculation_sound_min_distance = read_float(
        "ejaculation_sound", "min_distance", 1.5f);
    cfg.ejaculation_sound_max_distance = read_float(
        "ejaculation_sound", "max_distance", 12.0f);
    if (cfg.spawn_rate < 1.0f) cfg.spawn_rate = 1.0f;
    if (cfg.spawn_rate > 240.0f) cfg.spawn_rate = 240.0f;
    if (cfg.particle_lifetime < 0.1f) cfg.particle_lifetime = 0.1f;
    if (cfg.particle_lifetime > 30.0f) cfg.particle_lifetime = 30.0f;
    if (cfg.model_duration < 0.1f) cfg.model_duration = 0.1f;
    if (cfg.model_duration > 30.0f) cfg.model_duration = 30.0f;
    if (cfg.model_pulse_count < 1) cfg.model_pulse_count = 1;
    if (cfg.model_pulse_count > 24) cfg.model_pulse_count = 24;
    if (cfg.model_pulse_duration < 0.05f)
        cfg.model_pulse_duration = 0.05f;
    if (cfg.model_pulse_duration > 2.0f)
        cfg.model_pulse_duration = 2.0f;
    if (cfg.model_pulse_interval < 0.0f)
        cfg.model_pulse_interval = 0.0f;
    if (cfg.model_pulse_interval > 3.0f)
        cfg.model_pulse_interval = 3.0f;
    if (cfg.model_pulse_decay < 0.0f) cfg.model_pulse_decay = 0.0f;
    if (cfg.model_pulse_decay > 0.8f) cfg.model_pulse_decay = 0.8f;
    if (cfg.model_stream_cohesion < 0.02f)
        cfg.model_stream_cohesion = 0.02f;
    if (cfg.model_stream_cohesion > 2.0f)
        cfg.model_stream_cohesion = 2.0f;
    if (cfg.model_satellite_chance < 0.0f)
        cfg.model_satellite_chance = 0.0f;
    if (cfg.model_satellite_chance > 0.75f)
        cfg.model_satellite_chance = 0.75f;
    if (cfg.tool_duration < 0.1f) cfg.tool_duration = 0.1f;
    if (cfg.tool_duration > 30.0f) cfg.tool_duration = 30.0f;
    if (cfg.tool_start_delay < 0.0f) cfg.tool_start_delay = 0.0f;
    if (cfg.tool_start_delay > 5.0f) cfg.tool_start_delay = 5.0f;
    if (cfg.testicular_retraction_target_weight < 0.0f)
        cfg.testicular_retraction_target_weight = 0.0f;
    if (cfg.testicular_retraction_target_weight > 1.0f)
        cfg.testicular_retraction_target_weight = 1.0f;
    if (cfg.testicular_retraction_time < 0.0f)
        cfg.testicular_retraction_time = 0.0f;
    if (cfg.testicular_retraction_time > 5.0f)
        cfg.testicular_retraction_time = 5.0f;
    if (cfg.testicular_retraction_hold_time < 0.0f)
        cfg.testicular_retraction_hold_time = 0.0f;
    if (cfg.testicular_retraction_hold_time > 5.0f)
        cfg.testicular_retraction_hold_time = 5.0f;
    if (cfg.testicular_retraction_release_time < 0.0f)
        cfg.testicular_retraction_release_time = 0.0f;
    if (cfg.testicular_retraction_release_time > 5.0f)
        cfg.testicular_retraction_release_time = 5.0f;
    if (cfg.speed < 0.01f) cfg.speed = 0.01f;
    if (cfg.speed > 100.0f) cfg.speed = 100.0f;
    if (cfg.spread < 0.0f) cfg.spread = 0.0f;
    if (cfg.spread > 2.0f) cfg.spread = 2.0f;
    if (cfg.gravity_strength < -10.0f) cfg.gravity_strength = -10.0f;
    if (cfg.gravity_strength > 10.0f) cfg.gravity_strength = 10.0f;
    if (cfg.drag < 0.0f) cfg.drag = 0.0f;
    if (cfg.drag > 20.0f) cfg.drag = 20.0f;
    if (cfg.particle_size < 0.001f) cfg.particle_size = 0.001f;
    if (cfg.particle_size > 0.2f) cfg.particle_size = 0.2f;
    if (cfg.stream_thickness < 0.4f) cfg.stream_thickness = 0.4f;
    if (cfg.stream_thickness > 3.0f) cfg.stream_thickness = 3.0f;
    if (cfg.stream_opacity < 0.0f) cfg.stream_opacity = 0.0f;
    if (cfg.stream_opacity > 1.0f) cfg.stream_opacity = 1.0f;
    if (cfg.contact_droplet_size < 0.001f)
        cfg.contact_droplet_size = 0.001f;
    if (cfg.contact_droplet_size > 0.2f)
        cfg.contact_droplet_size = 0.2f;
    if (cfg.contact_droplet_opacity < 0.0f)
        cfg.contact_droplet_opacity = 0.0f;
    if (cfg.contact_droplet_opacity > 1.0f)
        cfg.contact_droplet_opacity = 1.0f;
    if (cfg.stream_base_opacity < 0.0f)
        cfg.stream_base_opacity = 0.0f;
    if (cfg.stream_base_opacity > 1.0f)
        cfg.stream_base_opacity = 1.0f;
    if (cfg.stream_base_fade_length < 0.0f)
        cfg.stream_base_fade_length = 0.0f;
    if (cfg.stream_base_fade_length > 2.0f)
        cfg.stream_base_fade_length = 2.0f;
    if (cfg.stream_wobble_start < 0.0f)
        cfg.stream_wobble_start = 0.0f;
    if (cfg.stream_wobble_start > 2.0f)
        cfg.stream_wobble_start = 2.0f;
    if (cfg.stream_wobble_fade_length < 0.0f)
        cfg.stream_wobble_fade_length = 0.0f;
    if (cfg.stream_wobble_fade_length > 2.0f)
        cfg.stream_wobble_fade_length = 2.0f;
    if (cfg.stream_bulge < 0.0f) cfg.stream_bulge = 0.0f;
    if (cfg.stream_bulge > 0.5f) cfg.stream_bulge = 0.5f;
    if (cfg.stream_curve_smoothness < 1)
        cfg.stream_curve_smoothness = 1;
    if (cfg.stream_curve_smoothness > LIQUID_CURVE_SUBDIVISIONS_MAX)
        cfg.stream_curve_smoothness = LIQUID_CURVE_SUBDIVISIONS_MAX;
    if (cfg.stream_curve_tension < 0.0f)
        cfg.stream_curve_tension = 0.0f;
    if (cfg.stream_curve_tension > 1.0f)
        cfg.stream_curve_tension = 1.0f;
    if (cfg.core_opacity < 0.0f) cfg.core_opacity = 0.0f;
    if (cfg.core_opacity > 1.0f) cfg.core_opacity = 1.0f;
    if (cfg.draw_layer < -1) cfg.draw_layer = -1;
    if (cfg.draw_layer > 100) cfg.draw_layer = 100;
    if (cfg.collision_surface_hold < 0.05f)
        cfg.collision_surface_hold = 0.05f;
    if (cfg.collision_surface_hold > 10.0f)
        cfg.collision_surface_hold = 10.0f;
    if (cfg.collision_depth_tolerance < 0.00005f)
        cfg.collision_depth_tolerance = 0.00005f;
    if (cfg.collision_depth_tolerance > 0.05f)
        cfg.collision_depth_tolerance = 0.05f;
    if (cfg.collision_surface_spread < 0.0f)
        cfg.collision_surface_spread = 0.0f;
    if (cfg.collision_surface_spread > 3.0f)
        cfg.collision_surface_spread = 3.0f;
    if (cfg.collision_connection_distance < 0.002f) cfg.collision_connection_distance = 0.002f;
    if (cfg.collision_connection_distance > 0.10f) cfg.collision_connection_distance = 0.10f;
    if (cfg.collision_connection_thickness < 0.1f) cfg.collision_connection_thickness = 0.1f;
    if (cfg.collision_connection_thickness > 1.0f) cfg.collision_connection_thickness = 1.0f;
    if (cfg.collision_model_stain_rate < 1.0f)
        cfg.collision_model_stain_rate = 1.0f;
    if (cfg.collision_model_stain_rate > 60.0f)
        cfg.collision_model_stain_rate = 60.0f;
    if (cfg.ejaculation_sound_volume < -10.0f)
        cfg.ejaculation_sound_volume = -10.0f;
    if (cfg.ejaculation_sound_volume > 10.0f)
        cfg.ejaculation_sound_volume = 10.0f;
    if (cfg.ejaculation_sound_delay_ms < 0)
        cfg.ejaculation_sound_delay_ms = 0;
    if (cfg.ejaculation_sound_delay_ms > 500)
        cfg.ejaculation_sound_delay_ms = 500;
    if (cfg.ejaculation_sound_min_distance < 0.0f)
        cfg.ejaculation_sound_min_distance = 0.0f;
    if (cfg.ejaculation_sound_min_distance > 100.0f)
        cfg.ejaculation_sound_min_distance = 100.0f;
    if (cfg.ejaculation_sound_max_distance <
        cfg.ejaculation_sound_min_distance + 0.1f)
        cfg.ejaculation_sound_max_distance =
            cfg.ejaculation_sound_min_distance + 0.1f;
    if (cfg.ejaculation_sound_max_distance > 200.0f)
        cfg.ejaculation_sound_max_distance = 200.0f;
    if (!cfg.ejaculation_sound_preset[0] ||
        strchr(cfg.ejaculation_sound_preset, '\\') ||
        strchr(cfg.ejaculation_sound_preset, '/') ||
        strstr(cfg.ejaculation_sound_preset, ".."))
        lstrcpynA(cfg.ejaculation_sound_preset, "false",
                  sizeof(cfg.ejaculation_sound_preset));
}

static void ensure_log_directory(void)
{
    char directory[MAX_PATH * 4];
    char *slash;
    lstrcpynA(directory, log_path, sizeof(directory));
    slash = strrchr(directory, '\\');
    if (slash) {
        *slash = 0;
        CreateDirectoryA(directory, NULL);
    }
}

static void log_line(const char *fmt, ...)
{
    FILE *file;
    SYSTEMTIME st;
    va_list args;
    if (!cfg.enabled || !log_ready || !fmt) return;
    EnterCriticalSection(&log_lock);
    file = fopen(log_path, "ab");
    if (file) {
        GetLocalTime(&st);
        fprintf(file, "%04u-%02u-%02u %02u:%02u:%02u.%03u ",
                st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,
                st.wSecond, st.wMilliseconds);
        va_start(args, fmt);
        vfprintf(file, fmt, args);
        va_end(args);
        fputs("\r\n", file);
        fclose(file);
    }
    LeaveCriticalSection(&log_lock);
}

static void liquid_check_config_reload(DWORD now)
{
    WIN32_FILE_ATTRIBUTE_DATA attributes;
    int previous_limit;
    int previous_liquids_enabled;
    float previous_emitter_position[3];
    if (now - liquid_config_check_tick < 500) return;
    if (InterlockedCompareExchange(&liquid_config_reload_lock, 1, 0) != 0)
        return;
    liquid_config_check_tick = now;
    if (!GetFileAttributesExA(config_path, GetFileExInfoStandard,
                              &attributes)) {
        InterlockedExchange(&liquid_config_reload_lock, 0);
        return;
    }
    if (!liquid_config_write_time_valid) {
        liquid_config_write_time = attributes.ftLastWriteTime;
        liquid_config_write_time_valid = 1;
        InterlockedExchange(&liquid_config_reload_lock, 0);
        return;
    }
    if (CompareFileTime(&attributes.ftLastWriteTime,
                        &liquid_config_write_time) != 0) {
        previous_limit = cfg.particle_limit;
        previous_liquids_enabled = cfg.liquids_enabled;
        memcpy(previous_emitter_position, cfg.emitter_position,
               sizeof(previous_emitter_position));
        liquid_config_write_time = attributes.ftLastWriteTime;
        load_config();
        if (memcmp(previous_emitter_position, cfg.emitter_position,
                   sizeof(previous_emitter_position)) != 0) {
            int index;
            /* Start the next births at the adjusted source instead of
               interpolating from the old slider position. Existing liquid
               particles retain their position, velocity and contacts. */
            for (index = 0; index < LIQUID_MODEL_EMITTER_COUNT; index++)
                model_emitters[index].transform_valid = 0;
            log_line("liquid emitter position live-reloaded offset_cm=(%.4f,%.4f,%.4f)",
                     cfg.emitter_position[0], cfg.emitter_position[1],
                     cfg.emitter_position[2]);
        }
        if (previous_liquids_enabled && !cfg.liquids_enabled) {
            int restored_nodes = liquid_restore_native_ray_nodes();
            memset(model_emitters, 0, sizeof(model_emitters));
            memset(&tool_emitter, 0, sizeof(tool_emitter));
            memset(liquid_particles, 0, sizeof(liquid_particles));
            last_tool_command_tick = 0;
            log_line("custom liquids disabled; native TK17 ejaculation restored spermray_nodes=%d",
                     restored_nodes);
        }
        if (cfg.particle_limit < previous_limit)
            memset(&liquid_particles[cfg.particle_limit], 0,
                   sizeof(liquid_particles[0]) *
                   (size_t)(LIQUID_PARTICLE_CAP - cfg.particle_limit));
        log_line("liquid configuration live-reloaded spawn_rate=%.2f size=%.4f stream_thickness=%.3f stream_opacity=%.3f contact_droplet_size=%.4f contact_droplet_opacity=%.3f stream_base_opacity=%.3f stream_base_fade_length=%.3f stream_wobble_start=%.3f stream_wobble_fade_length=%.3f stream_bulge=%.3f curve_smoothness=%d curve_tension=%.3f speed=%.3f spread=%.3f gravity=%.3f drag=%.3f pulses=%d cohesion=%.3f satellite_chance=%.3f",
                 cfg.spawn_rate, cfg.particle_size,
                 cfg.stream_thickness, cfg.stream_opacity,
                 cfg.contact_droplet_size, cfg.contact_droplet_opacity,
                 cfg.stream_base_opacity,
                 cfg.stream_base_fade_length,
                 cfg.stream_wobble_start,
                 cfg.stream_wobble_fade_length,
                 cfg.stream_bulge, cfg.stream_curve_smoothness,
                 cfg.stream_curve_tension, cfg.speed, cfg.spread,
                 cfg.gravity_strength, cfg.drag,
                 cfg.model_pulse_count,
                 cfg.model_stream_cohesion,
                 cfg.model_satellite_chance);
    }
    InterlockedExchange(&liquid_config_reload_lock, 0);
}

static int reserve_event(void)
{
    if (!cfg.enabled) return 0;
    LONG value = InterlockedIncrement(&event_count);
    if (value <= cfg.max_events) return 1;
    if (value == cfg.max_events + 1) {
        log_line("diagnostic event limit reached max_events=%d further runtime events suppressed",
                 cfg.max_events);
    }
    return 0;
}

static void log_words(const char *label, const void *pointer, size_t bytes)
{
    const DWORD *words = (const DWORD*)pointer;
    size_t i;
    size_t count = bytes / sizeof(DWORD);
    char line[1024];
    size_t used;
    if (!pointer || !ptr_readable(pointer, bytes)) {
        log_line("memory snapshot label=\"%s\" address=%p readable=0 bytes=%lu",
                 label, pointer, (unsigned long)bytes);
        return;
    }
    used = (size_t)_snprintf(line, sizeof(line),
                             "memory snapshot label=\"%s\" address=%p words=",
                             label, pointer);
    if (used >= sizeof(line)) return;
    for (i = 0; i < count && used + 12 < sizeof(line); i++) {
        int wrote = _snprintf(line + used, sizeof(line) - used,
                              "%s%08lx", i ? "," : "",
                              (unsigned long)words[i]);
        if (wrote <= 0) break;
        used += (size_t)wrote;
    }
    line[sizeof(line) - 1] = 0;
    log_line("%s", line);
}

static unsigned int named_object_hash(const void *object)
{
    uintptr_t value = (uintptr_t)object;
    value >>= 4;
    value ^= value >> 11;
    value ^= value >> 21;
    return (unsigned int)value & (NAMED_OBJECT_SLOTS - 1);
}

static void remember_named_object(void *object, const char *name)
{
    unsigned int base;
    unsigned int i;
    if (!object || !name || !name[0]) return;
    base = named_object_hash(object);
    for (i = 0; i < 12; i++) {
        named_object_t *entry =
            &named_objects[(base + i) & (NAMED_OBJECT_SLOTS - 1)];
        void *current = (void*)entry->object;
        if (!current || current == object) {
            lstrcpynA(entry->name, name, sizeof(entry->name));
            MemoryBarrier();
            entry->object = object;
            return;
        }
    }
    {
        named_object_t *entry = &named_objects[base];
        entry->object = NULL;
        lstrcpynA(entry->name, name, sizeof(entry->name));
        MemoryBarrier();
        entry->object = object;
    }
}

static const char *known_object_name(const void *object)
{
    unsigned int base;
    unsigned int i;
    if (!object) return NULL;
    base = named_object_hash(object);
    for (i = 0; i < 12; i++) {
        named_object_t *entry =
            &named_objects[(base + i) & (NAMED_OBJECT_SLOTS - 1)];
        void *current = (void*)entry->object;
        if (current == object) return entry->name[0] ? entry->name : NULL;
        if (!current) return NULL;
    }
    return NULL;
}

static int liquid_person_spermray_root_name(const char *name)
{
    int person;
    char expected[32];
    if (!name) return 0;
    for (person = 1; person <= 4; person++) {
        _snprintf(expected, sizeof(expected), "Person%02dSpermray", person);
        expected[sizeof(expected) - 1] = 0;
        if (_stricmp(name, expected) == 0) return person;
    }
    return 0;
}

static void liquid_remember_spermray_source_person(void *source,
                                                    int person_index)
{
    static unsigned int cursor;
    unsigned int index;
    liquid_spermray_source_person_t *slot;
    if (!source || person_index < 1 || person_index > 4) return;
    for (index = 0; index < LIQUID_SPERMRAY_SOURCE_SLOTS; index++) {
        slot = &liquid_spermray_source_people[index];
        if (slot->source == source || !slot->source) {
            slot->person_index = person_index;
            MemoryBarrier();
            slot->source = source;
            return;
        }
    }
    slot = &liquid_spermray_source_people[
        cursor++ % LIQUID_SPERMRAY_SOURCE_SLOTS];
    slot->source = NULL;
    slot->person_index = person_index;
    MemoryBarrier();
    slot->source = source;
}

static int liquid_spermray_source_person(const void *source)
{
    unsigned int index;
    if (!source) return 0;
    for (index = 0; index < LIQUID_SPERMRAY_SOURCE_SLOTS; index++) {
        if (liquid_spermray_source_people[index].source == source)
            return liquid_spermray_source_people[index].person_index;
    }
    return 0;
}

static int liquid_transform_node_name(const char *name)
{
    if (!name) return 0;
    return contains_i(name, "spermray_group") ||
           contains_i(name, "spermray01_group") ||
           contains_i(name, "spermray01_splashSource") ||
           liquid_person_spermray_root_name(name) != 0 ||
           contains_i(name, "Tool01Spermray") ||
           contains_i(name, "penis_jointEnd") ||
           contains_i(name, "penis_joint03") ||
           contains_i(name, "penis_joint02");
}

static void remember_liquid_transform_node(void *object, const char *name)
{
    int i;
    int empty = -1;
    int person_index;
    if (!object || !liquid_transform_node_name(name)) return;
    /* The X-key tool builds a temporary spermray hierarchy, then removes its
       public name before the configured emission delay expires.  Retain the
       actual group while its root is being named so the emitter can keep
       following the live transform instead of performing a late name lookup. */
    if (_stricmp(name, "spermray01_group") == 0) {
        InterlockedExchangePointer(
            (PVOID volatile *)&latest_spermray_group_object, object);
    } else if (contains_i(name, "spermray01_splashSource")) {
        InterlockedExchangePointer(
            (PVOID volatile *)&latest_spermray_splash_source_object,
            object);
    } else if (_stricmp(name, "Tool01Spermray") == 0) {
        void *group = (void*)InterlockedCompareExchangePointer(
            (PVOID volatile *)&latest_spermray_group_object, NULL, NULL);
        InterlockedExchangePointer(
            (PVOID volatile *)&tool_spermray_root_object, object);
        if (group)
            InterlockedExchangePointer(
                (PVOID volatile *)&tool_spermray_group_object, group);
    }
    person_index = liquid_person_spermray_root_name(name);
    if (person_index) {
        void *source = (void*)InterlockedCompareExchangePointer(
            (PVOID volatile *)&latest_spermray_splash_source_object,
            NULL, NULL);
        if (source)
            liquid_remember_spermray_source_person(source, person_index);
    }
    for (i = 0; i < LIQUID_TRANSFORM_NODE_SLOTS; i++) {
        liquid_transform_node_t *entry = &liquid_transform_nodes[i];
        if (entry->name[0] && _stricmp(entry->name, name) == 0) {
            InterlockedExchangePointer((PVOID volatile *)&entry->object,
                                       object);
            return;
        }
        if (empty < 0 && !entry->name[0]) empty = i;
    }
    if (empty < 0) empty = 0;
    liquid_transform_nodes[empty].object = NULL;
    lstrcpynA(liquid_transform_nodes[empty].name, name,
              sizeof(liquid_transform_nodes[empty].name));
    MemoryBarrier();
    InterlockedExchangePointer(
        (PVOID volatile *)&liquid_transform_nodes[empty].object, object);
}

static void *find_remembered_liquid_transform_node(const char *name)
{
    int i;
    if (!name || !name[0]) return NULL;
    for (i = 0; i < LIQUID_TRANSFORM_NODE_SLOTS; i++) {
        liquid_transform_node_t *entry = &liquid_transform_nodes[i];
        if (entry->name[0] && _stricmp(entry->name, name) == 0) {
            void *object = (void*)InterlockedCompareExchangePointer(
                (PVOID volatile *)&entry->object, NULL, NULL);
            return object && ptr_readable(object, sizeof(void*)) ? object
                                                                 : NULL;
        }
    }
    return NULL;
}

static void log_module_address(const void *address, char *out, size_t outsz)
{
    MEMORY_BASIC_INFORMATION mbi;
    char path[MAX_PATH * 2];
    const char *name;
    if (!out || !outsz) return;
    out[0] = 0;
    if (!address || !VirtualQuery(address, &mbi, sizeof(mbi))) {
        _snprintf(out, outsz, "%p", address);
        out[outsz - 1] = 0;
        return;
    }
    path[0] = 0;
    GetModuleFileNameA((HMODULE)mbi.AllocationBase, path, sizeof(path));
    name = strrchr(path, '\\');
    name = name ? name + 1 : path;
    if (!name[0]) name = "anonymous";
    _snprintf(out, outsz, "%s+0x%lx(%p)", name,
              (unsigned long)((const BYTE*)address -
                              (const BYTE*)mbi.AllocationBase),
              address);
    out[outsz - 1] = 0;
}

static void log_deep_callstack(const char *event_name, void *object)
{
    void *frames[16];
    USHORT count;
    USHORT i;
    char line[4096];
    size_t used;
    count = CaptureStackBackTrace(1, 16, frames, NULL);
    used = (size_t)_snprintf(
        line, sizeof(line),
        "liquid deep stack event=\"%s\" object=%p frames=",
        event_name ? event_name : "", object);
    if (used >= sizeof(line)) return;
    for (i = 0; i < count && used + 128 < sizeof(line); i++) {
        char address[160];
        int wrote;
        log_module_address(frames[i], address, sizeof(address));
        wrote = _snprintf(line + used, sizeof(line) - used,
                          "%s%s", i ? " <- " : "", address);
        if (wrote <= 0) break;
        used += (size_t)wrote;
    }
    line[sizeof(line) - 1] = 0;
    log_line("%s", line);
}

static void log_deep_pointer_links(const char *event_name, void *object,
                                   size_t bytes)
{
    const DWORD *words = (const DWORD*)object;
    size_t count = bytes / sizeof(DWORD);
    size_t i;
    int links = 0;
    if (!object || !ptr_readable(object, bytes)) return;
    for (i = 0; i < count && links < 32; i++) {
        void *candidate = (void*)(uintptr_t)words[i];
        const char *known = known_object_name(candidate);
        if (known) {
            log_line("liquid deep link event=\"%s\" object=%p offset=0x%03lx pointer=%p target_name=\"%s\"",
                     event_name ? event_name : "", object,
                     (unsigned long)(i * sizeof(DWORD)), candidate, known);
            links++;
        } else if (candidate && ptr_readable(candidate, 1) &&
                   safe_cstr_a((const char*)candidate, 160)) {
            log_line("liquid deep string event=\"%s\" object=%p offset=0x%03lx pointer=%p text=\"%s\"",
                     event_name ? event_name : "", object,
                     (unsigned long)(i * sizeof(DWORD)), candidate,
                     (const char*)candidate);
            links++;
        }
    }
    if (!links) {
        log_line("liquid deep links event=\"%s\" object=%p named_or_string_links=0 scan_bytes=%lu",
                 event_name ? event_name : "", object,
                 (unsigned long)bytes);
    }
}

static int deep_event_name(const char *name)
{
    if (!name || contains_i(name, "Source")) return 0;
    return _stricmp(name, "spermray01_init") == 0 ||
           _stricmp(name, "spermray01_splash") == 0 ||
           contains_i(name, ":spermray01_init") ||
           contains_i(name, ":spermray01_splash") ||
           _stricmp(name, "spermray01ActiveScheduler") == 0;
}

static void capture_deep_event(const char *name, void *object,
                               const void *direct_caller)
{
    LONG sequence;
    char label[320];
    char caller_text[320];
    size_t bytes;
    if (!cfg.deep_event_capture || !deep_event_name(name) || !object) return;
    sequence = InterlockedIncrement(&deep_event_count);
    if (sequence > cfg.deep_event_limit) {
        if (sequence == cfg.deep_event_limit + 1) {
            log_line("liquid deep event limit reached deep_event_limit=%d",
                     cfg.deep_event_limit);
        }
        return;
    }
    bytes = (size_t)cfg.deep_event_bytes;
    _snprintf(label, sizeof(label), "deep:%s:%ld", name, sequence);
    label[sizeof(label) - 1] = 0;
    log_line("LIQUID DEEP EVENT sequence=%ld name=\"%s\" object=%p bytes=%lu thread=0x%08lx",
             sequence, name, object, (unsigned long)bytes,
             (unsigned long)GetCurrentThreadId());
    log_module_address(direct_caller, caller_text, sizeof(caller_text));
    log_line("liquid deep direct-caller event=\"%s\" object=%p caller=%s",
             name, object, caller_text);
    log_deep_callstack(name, object);
    log_words(label, object, bytes);
    log_deep_pointer_links(name, object, bytes);
}

static int install_inline_hook(void *target, void *hook, size_t stolen_len,
                               void **trampoline)
{
    BYTE *tramp;
    DWORD old;
    DWORD rel;
    size_t i;
    if (!target || !hook || !trampoline || *trampoline || stolen_len < 5)
        return 0;

    /* Chain existing plugins instead of overwriting them. */
    if (*(BYTE*)target == 0xe9) {
        DWORD old_rel;
        memcpy(&old_rel, (BYTE*)target + 1, sizeof(old_rel));
        *trampoline = (void*)((BYTE*)target + 5 + (LONG)old_rel);
        if (!VirtualProtect(target, 5, PAGE_EXECUTE_READWRITE, &old)) {
            *trampoline = NULL;
            return 0;
        }
        rel = (DWORD)((BYTE*)hook - ((BYTE*)target + 5));
        memcpy((BYTE*)target + 1, &rel, sizeof(rel));
        VirtualProtect(target, 5, old, &old);
        FlushInstructionCache(GetCurrentProcess(), target, 5);
        return 1;
    }

    tramp = (BYTE*)VirtualAlloc(NULL, stolen_len + 5,
                               MEM_COMMIT | MEM_RESERVE,
                               PAGE_EXECUTE_READWRITE);
    if (!tramp) return 0;
    memcpy(tramp, target, stolen_len);
    tramp[stolen_len] = 0xe9;
    rel = (DWORD)(((BYTE*)target + stolen_len) -
                  (tramp + stolen_len + 5));
    memcpy(tramp + stolen_len + 1, &rel, sizeof(rel));
    if (!VirtualProtect(target, stolen_len, PAGE_EXECUTE_READWRITE, &old)) {
        VirtualFree(tramp, 0, MEM_RELEASE);
        return 0;
    }
    ((BYTE*)target)[0] = 0xe9;
    rel = (DWORD)((BYTE*)hook - ((BYTE*)target + 5));
    memcpy((BYTE*)target + 1, &rel, sizeof(rel));
    for (i = 5; i < stolen_len; i++) ((BYTE*)target)[i] = 0x90;
    VirtualProtect(target, stolen_len, old, &old);
    FlushInstructionCache(GetCurrentProcess(), target, stolen_len);
    *trampoline = tramp;
    return 1;
}

static liquid_setting_binding_t *liquid_setting_binding_by_name(
    const char *name)
{
    size_t i;
    if (!name) return NULL;
    for (i = 0; i < sizeof(liquid_setting_bindings) /
                        sizeof(liquid_setting_bindings[0]); ++i) {
        if (strcmp(liquid_setting_bindings[i].param_name, name) == 0)
            return &liquid_setting_bindings[i];
    }
    return NULL;
}

static int liquid_setting_bool_value(const char *value, int *enabled)
{
    if (!value || !enabled) return 0;
    if (_stricmp(value, "ON") == 0 || _stricmp(value, "YES") == 0 ||
        _stricmp(value, "TRUE") == 0 || strcmp(value, "1") == 0) {
        *enabled = 1;
        return 1;
    }
    if (_stricmp(value, "OFF") == 0 || _stricmp(value, "NO") == 0 ||
        _stricmp(value, "FALSE") == 0 || strcmp(value, "0") == 0) {
        *enabled = 0;
        return 1;
    }
    return 0;
}

/* Set CustomParameter.Enable through the same native member dispatch used by
   the game. The member id and dispatch slot belong to this supported EXE. */
static int liquid_set_custom_parameter_enable(void *parameter, int enabled)
{
    HMODULE executable;
    BYTE *base;
    DWORD member_id;
    BYTE dispatch_slot;
    BYTE *metadata;
    BYTE *dispatch_table;
    BYTE *setter_slot;
    script_u32_set_t setter;
    if (!parameter ||
        !ptr_readable((BYTE*)parameter - SCRIPT_OBJECT_META_BACK_OFFSET,
                      sizeof(metadata))) return 0;
    executable = GetModuleHandleA(NULL);
    if (!executable) return 0;
    base = (BYTE*)executable;
    if (!ptr_readable(base + CUSTOM_PARAMETER_ENABLE_MEMBER_RVA,
                      sizeof(member_id))) return 0;
    memcpy(&member_id, base + CUSTOM_PARAMETER_ENABLE_MEMBER_RVA,
           sizeof(member_id));
    dispatch_slot = *(base + CUSTOM_PARAMETER_ENABLE_MEMBER_RVA + 3u);
    memcpy(&metadata,
           (BYTE*)parameter - SCRIPT_OBJECT_META_BACK_OFFSET,
           sizeof(metadata));
    if (!metadata ||
        !ptr_readable(metadata + (member_id & 0x0fffu) * sizeof(void*),
                      sizeof(dispatch_table))) return 0;
    memcpy(&dispatch_table,
           metadata + (member_id & 0x0fffu) * sizeof(void*),
           sizeof(dispatch_table));
    if (!dispatch_table) return 0;
    setter_slot = dispatch_table + ((size_t)dispatch_slot << 6) +
                  sizeof(void*);
    if (!ptr_readable(setter_slot, sizeof(setter))) return 0;
    memcpy(&setter, setter_slot, sizeof(setter));
    if (!ptr_executable((const void*)setter)) return 0;
    setter(parameter, member_id, enabled ? 1u : 0u);
    return 1;
}

static int liquid_copy_engine_string(const char *value, char *out,
                                     size_t out_size)
{
    int length;
    size_t index;
    if (!out || !out_size) return 0;
    out[0] = 0;
    if (!value || !ptr_readable(value, 1)) return 0;
    if (ptr_readable(value - sizeof(int), sizeof(int))) {
        memcpy(&length, value - sizeof(int), sizeof(length));
        if (length >= 0 && length < 32768 &&
            ptr_readable(value, (size_t)length + 1)) {
            size_t copy_length = (size_t)length;
            if (copy_length >= out_size) copy_length = out_size - 1;
            memcpy(out, value, copy_length);
            out[copy_length] = 0;
            return 1;
        }
    }
    for (index = 0; index + 1 < out_size; ++index) {
        if (!ptr_readable(value + index, 1)) return 0;
        out[index] = value[index];
        if (!out[index]) return 1;
    }
    out[out_size - 1] = 0;
    return 1;
}

/* Script arrays can contain tagged Nil values as well as native object
   references. Mirror the engine's own tag test before dispatching a member;
   readable-address checks alone do not identify these script values. */
static int liquid_is_live_script_object(const void *object)
{
    uintptr_t value = (uintptr_t)object;
    return object && ((~value & 0x08u) != 0u) &&
           ptr_readable((const BYTE*)object -
                            SCRIPT_OBJECT_META_BACK_OFFSET,
                        sizeof(void*));
}

static int liquid_custom_parameter_string(void *parameter,
                                          DWORD member_rva, char *out,
                                          size_t out_size)
{
    HMODULE executable;
    BYTE *base;
    DWORD member_id;
    BYTE dispatch_slot;
    BYTE *metadata;
    BYTE *dispatch_table;
    script_get_string_t getter;
    engine_string_release_t release_string = NULL;
    char *engine_value;
    int copied;
    if (!liquid_is_live_script_object(parameter) || !out || !out_size)
        return 0;
    out[0] = 0;
    executable = GetModuleHandleA(NULL);
    if (!executable) return 0;
    base = (BYTE*)executable;
    if (!ptr_readable(base + member_rva, sizeof(member_id)) ||
        !ptr_readable(base + ENGINE_EMPTY_STRING_RVA,
                      sizeof(engine_value))) return 0;
    memcpy(&member_id, base + member_rva, sizeof(member_id));
    dispatch_slot = *(base + member_rva + 3u);
    memcpy(&metadata,
           (BYTE*)parameter - SCRIPT_OBJECT_META_BACK_OFFSET,
           sizeof(metadata));
    if (!metadata ||
        !ptr_readable(metadata + (member_id & 0x0fffu) * sizeof(void*),
                      sizeof(dispatch_table))) return 0;
    memcpy(&dispatch_table,
           metadata + (member_id & 0x0fffu) * sizeof(void*),
           sizeof(dispatch_table));
    if (!dispatch_table ||
        !ptr_readable(dispatch_table + ((size_t)dispatch_slot << 6),
                      sizeof(getter))) return 0;
    memcpy(&getter, dispatch_table + ((size_t)dispatch_slot << 6),
           sizeof(getter));
    if (!ptr_executable((const void*)getter)) return 0;
    memcpy(&engine_value, base + ENGINE_EMPTY_STRING_RVA,
           sizeof(engine_value));
    getter(parameter, member_id, &engine_value);
    copied = liquid_copy_engine_string(engine_value, out, out_size);
    if (ptr_readable(base + ENGINE_STRING_RELEASE_RVA,
                     sizeof(release_string))) {
        memcpy(&release_string, base + ENGINE_STRING_RELEASE_RVA,
               sizeof(release_string));
        if (release_string) release_string(&engine_value);
    }
    return copied;
}

static int liquid_custom_parameter_name(void *parameter, char *out,
                                        size_t out_size)
{
    return liquid_custom_parameter_string(
        parameter, CUSTOM_PARAMETER_NAME_MEMBER_RVA, out, out_size);
}

/* ParamDescription is the stable identity. ParamID remains ordering-only and
   is intentionally not read anywhere in this conditional visibility path. */
static int liquid_custom_parameter_description(void *parameter, char *out,
                                                size_t out_size)
{
    return liquid_custom_parameter_string(
        parameter, CUSTOM_PARAMETER_DESCRIPTION_MEMBER_RVA, out, out_size);
}

/* Resize and populate one CustomParameter String array through its native
   dispatch table. Array slot +0x30 changes the element count, slot +0x04
   assigns one managed Bionic::String, and slot +0x08 reports the count. */
static int liquid_set_custom_parameter_string_array(
    void *parameter, DWORD member_rva, const char *first_value)
{
    HMODULE executable;
    BYTE *base;
    DWORD member_id;
    BYTE dispatch_slot;
    BYTE *metadata;
    BYTE *dispatch_table;
    BYTE *member_dispatch;
    script_array_count_t get_count;
    script_array_resize_t resize;
    script_array_set_string_t set_item;
    engine_string_construct_cstr_t construct_string;
    engine_string_release_t release_string;
    int target_count = liquid_audio_preset_count + 1;
    int index;
    if (!liquid_is_live_script_object(parameter) || !first_value ||
        target_count < 1 || target_count > LIQUID_AUDIO_PRESET_CAP + 1)
        return 0;
    executable = GetModuleHandleA(NULL);
    if (!executable) return 0;
    base = (BYTE*)executable;
    if (!ptr_readable(base + member_rva, sizeof(member_id)) ||
        !ptr_readable(base + ENGINE_STRING_CSTR_CONSTRUCT_RVA,
                      sizeof(construct_string)) ||
        !ptr_readable(base + ENGINE_STRING_RELEASE_RVA,
                      sizeof(release_string))) return 0;
    memcpy(&member_id, base + member_rva, sizeof(member_id));
    dispatch_slot = *(base + member_rva + 3u);
    memcpy(&metadata,
           (BYTE*)parameter - SCRIPT_OBJECT_META_BACK_OFFSET,
           sizeof(metadata));
    if (!metadata ||
        !ptr_readable(metadata + (member_id & 0x0fffu) * sizeof(void*),
                      sizeof(dispatch_table))) return 0;
    memcpy(&dispatch_table,
           metadata + (member_id & 0x0fffu) * sizeof(void*),
           sizeof(dispatch_table));
    if (!dispatch_table) return 0;
    member_dispatch = dispatch_table + ((size_t)dispatch_slot << 6);
    if (!ptr_readable(member_dispatch + 0x30, sizeof(resize))) return 0;
    memcpy(&get_count, member_dispatch + 0x08, sizeof(get_count));
    memcpy(&set_item, member_dispatch + 0x04, sizeof(set_item));
    memcpy(&resize, member_dispatch + 0x30, sizeof(resize));
    memcpy(&construct_string, base + ENGINE_STRING_CSTR_CONSTRUCT_RVA,
           sizeof(construct_string));
    memcpy(&release_string, base + ENGINE_STRING_RELEASE_RVA,
           sizeof(release_string));
    if (!ptr_executable((const void*)get_count) ||
        !ptr_executable((const void*)set_item) ||
        !ptr_executable((const void*)resize) ||
        !ptr_executable((const void*)construct_string) ||
        !ptr_executable((const void*)release_string)) return 0;

    /* Indexed assignment grows this script array. The +0x30 operation only
       truncates existing excess entries; it does not reserve/grow storage. */
    for (index = 0; index < target_count; ++index) {
        const char *value = index == 0 ? first_value :
            liquid_audio_preset_names[index - 1];
        char *engine_value = NULL;
        construct_string(&engine_value, value);
        if (!engine_value) return 0;
        set_item(parameter, member_id, index, engine_value);
        release_string(&engine_value);
    }
    if (get_count(parameter, member_id) > target_count)
        resize(parameter, member_id, target_count, -1);
    return get_count(parameter, member_id) == target_count;
}

static int liquid_inject_audio_preset_parameter(void *self)
{
    void **parameters = NULL;
    int parameter_count = 0;
    int index;
    if (!self || !ptr_readable((BYTE*)self + 0x14,
                               sizeof(parameters))) return 0;
    memcpy(&parameters, (BYTE*)self + 0x14, sizeof(parameters));
    if (!parameters ||
        !ptr_readable((BYTE*)parameters - sizeof(parameter_count),
                      sizeof(parameter_count))) return 0;
    memcpy(&parameter_count,
           (BYTE*)parameters - sizeof(parameter_count),
           sizeof(parameter_count));
    if (parameter_count <= 0 || parameter_count > 4096 ||
        !ptr_readable(parameters,
                      (size_t)parameter_count * sizeof(*parameters)))
        return 0;
    for (index = 0; index < parameter_count; ++index) {
        char name[128];
        if (!liquid_custom_parameter_name(parameters[index], name,
                                          sizeof(name)) ||
            strcmp(name, "NcLiquidsEjaculationSoundTarget") != 0)
            continue;
        if (!liquid_set_custom_parameter_string_array(
                parameters[index],
                CUSTOM_PARAMETER_BOX_DESCRIPTION_ARRAY_MEMBER_RVA,
                "False")) return 0;
        if (!liquid_set_custom_parameter_string_array(
                parameters[index],
                CUSTOM_PARAMETER_BOX_VALUE_ARRAY_MEMBER_RVA,
                "false")) return 0;
        return 1;
    }
    return 0;
}

static int liquid_is_testicular_retraction_description(
    const char *description)
{
    static const char *descriptions[] = {
        "Testicular Retraction:",
        "Retraction Easing:",
        "Release Easing:",
        "Retraction Strength:",
        "Retraction Time:",
        "Retraction Hold Time:",
        "Retraction Release Time:"
    };
    size_t index;
    if (!description) return 0;
    for (index = 0; index < sizeof(descriptions) / sizeof(descriptions[0]);
         ++index) {
        if (strcmp(description, descriptions[index]) == 0) return 1;
    }
    return 0;
}

static int liquid_enable_physx_parameters_by_description(void *self)
{
    HMODULE physx = GetModuleHandleA("NC-TK17-PhysX.dll");
    void **parameters = NULL;
    int parameter_count = 0;
    int enabled_count = 0;
    int index;
    if (physx && GetProcAddress(
            physx, "NCTK17PhysX_SetBlendControlOverlay") && self &&
        ptr_readable((BYTE*)self + 0x14, sizeof(parameters))) {
        memcpy(&parameters, (BYTE*)self + 0x14, sizeof(parameters));
        if (parameters &&
            ptr_readable((BYTE*)parameters - sizeof(parameter_count),
                         sizeof(parameter_count))) {
            memcpy(&parameter_count,
                   (BYTE*)parameters - sizeof(parameter_count),
                   sizeof(parameter_count));
            if (parameter_count > 0 && parameter_count <= 4096 &&
                ptr_readable(parameters,
                             (size_t)parameter_count * sizeof(*parameters))) {
                for (index = 0; index < parameter_count; ++index) {
                    char description[160];
                    if (liquid_custom_parameter_description(
                            parameters[index], description,
                            sizeof(description)) &&
                        liquid_is_testicular_retraction_description(
                            description) &&
                        liquid_set_custom_parameter_enable(
                            parameters[index], 1)) {
                        enabled_count++;
                    }
                }
            }
        }
    }
    return enabled_count;
}

static void THISCALL hook_Customizer_PrepareControls(void *self)
{
    int enabled_count;
    int presets_injected;
    if (tramp_Customizer_PrepareControls)
        tramp_Customizer_PrepareControls(self);
    /* BuildControls calls this method immediately before its Enable loop;
       therefore its final parameter array is available and no rebuild is
       required after changing the seven matching objects. */
    enabled_count = liquid_enable_physx_parameters_by_description(self);
    if (enabled_count)
        log_line("PhysX GUI enabled before enumeration by ParamDescription count=%d",
                 enabled_count);
    presets_injected = liquid_inject_audio_preset_parameter(self);
    if (presets_injected)
        log_line("settings ejaculation-audio presets injected in memory count=%d",
                 liquid_audio_preset_count);
}

static int liquid_slider_widget_value(void *slider, float *out_value)
{
    BYTE *metadata;
    BYTE *dispatch_table;
    widget_get_float_t getter;
    float value;
    if (!slider || !out_value ||
        !ptr_readable((BYTE*)slider - 0x18, sizeof(metadata))) return 0;
    memcpy(&metadata, (BYTE*)slider - 0x18, sizeof(metadata));
    if (!metadata ||
        !ptr_readable(metadata + 0x3b4, sizeof(dispatch_table))) return 0;
    memcpy(&dispatch_table, metadata + 0x3b4, sizeof(dispatch_table));
    if (!dispatch_table ||
        !ptr_readable(dispatch_table + 0x80, sizeof(getter))) return 0;
    memcpy(&getter, dispatch_table + 0x80, sizeof(getter));
    if (!getter) return 0;
    value = getter(slider, 0x02fff0ed);
    if (!_finite(value)) return 0;
    *out_value = value;
    return 1;
}

/* Slider.Value is member 0x02fff0ed. Its member index (0xed) selects the
   metadata table at 0x3b4 and its dispatch index (2) selects getter/setter
   slots 0x80/0x84. This is the same native F32 property-dispatch convention
   used by the PhysX plugin; using it updates the live widget after TK17 has
   restored its own previously saved ConfigEditor value. */
static int liquid_slider_widget_set_value(void *slider, float value)
{
    BYTE *metadata;
    BYTE *dispatch_table;
    widget_set_float_t setter;
    if (!slider || !_finite(value) ||
        !ptr_readable((BYTE*)slider - 0x18, sizeof(metadata))) return 0;
    memcpy(&metadata, (BYTE*)slider - 0x18, sizeof(metadata));
    if (!metadata ||
        !ptr_readable(metadata + 0x3b4, sizeof(dispatch_table))) return 0;
    memcpy(&dispatch_table, metadata + 0x3b4, sizeof(dispatch_table));
    if (!dispatch_table ||
        !ptr_readable(dispatch_table + 0x84, sizeof(setter))) return 0;
    memcpy(&setter, dispatch_table + 0x84, sizeof(setter));
    if (!ptr_executable((const void*)setter)) return 0;
    setter(slider, 0x02fff0ed, value);
    return 1;
}

static int liquid_setting_ini_numeric_value(
    const liquid_setting_binding_t *binding, float *out_value)
{
    char text[64];
    char *end;
    double parsed;
    if (!binding || !out_value ||
        (binding->type != LIQUID_SETTING_FLOAT &&
         binding->type != LIQUID_SETTING_INT)) return 0;
    text[0] = 0;
    if (!GetPrivateProfileStringA(binding->section, binding->key, "",
                                  text, sizeof(text), config_path) ||
        !text[0]) return 0;
    end = NULL;
    parsed = strtod(text, &end);
    if (end == text || !_finite(parsed)) return 0;
    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        ++end;
    if (*end) return 0;
    if (binding->type == LIQUID_SETTING_INT)
        parsed = floor(parsed + 0.5);
    *out_value = (float)parsed;
    return _finite(*out_value);
}

static void liquid_sync_slider_from_ini(liquid_setting_binding_t *binding,
                                        void *slider)
{
    float requested;
    float before;
    float after;
    int have_before;
    if (!binding || !slider ||
        !liquid_setting_ini_numeric_value(binding, &requested)) return;
    have_before = liquid_slider_widget_value(slider, &before);
    if (have_before && fabsf(before - requested) <= 0.00001f) return;
    liquid_setting_sync_depth++;
    if (!liquid_slider_widget_set_value(slider, requested)) {
        liquid_setting_sync_depth--;
        log_line("settings slider sync failed param=\"%s\" requested=%.6g widget=%p",
                 binding->param_name, requested, slider);
        return;
    }
    liquid_setting_sync_depth--;
    if (liquid_slider_widget_value(slider, &after)) {
        log_line("settings slider synced param=\"%s\" ini=%.6g previous=%s%.6g live=%.6g",
                 binding->param_name, requested,
                 have_before ? "" : "unavailable/",
                 have_before ? before : 0.0f, after);
    } else {
        log_line("settings slider synced param=\"%s\" ini=%.6g live=unavailable",
                 binding->param_name, requested);
    }
}

static int liquid_setting_ini_spinbox_value(
    const liquid_setting_binding_t *binding, char *out, size_t out_size)
{
    int enabled;
    if (!binding || !out || !out_size ||
        (binding->type != LIQUID_SETTING_BOOL &&
         binding->type != LIQUID_SETTING_TEXT)) return 0;
    out[0] = 0;
    if (!GetPrivateProfileStringA(binding->section, binding->key, "",
                                  out, (DWORD)out_size, config_path) ||
        !out[0]) return 0;
    if (binding->type == LIQUID_SETTING_BOOL) {
        if (!liquid_setting_bool_value(out, &enabled)) return 0;
        lstrcpynA(out, enabled ? "ON" : "OFF", (int)out_size);
    }
    return 1;
}

static int liquid_spinbox_widget_value(void *widget, char *out,
                                       size_t out_size)
{
    HMODULE executable;
    BYTE *base;
    BYTE *metadata;
    BYTE *dispatch_table;
    script_get_string_t getter;
    engine_string_release_t release_string = NULL;
    char *engine_value;
    int copied;
    if (!widget || !out || !out_size ||
        !ptr_readable((BYTE*)widget - SCRIPT_OBJECT_META_BACK_OFFSET,
                      sizeof(metadata))) return 0;
    out[0] = 0;
    executable = GetModuleHandleA(NULL);
    if (!executable) return 0;
    base = (BYTE*)executable;
    if (!ptr_readable(base + ENGINE_EMPTY_STRING_RVA,
                      sizeof(engine_value))) return 0;
    memcpy(&metadata,
           (BYTE*)widget - SCRIPT_OBJECT_META_BACK_OFFSET,
           sizeof(metadata));
    if (!metadata ||
        !ptr_readable(metadata +
                          (WIDGET_TEXT_MEMBER_ID & 0x0fffu) * sizeof(void*),
                      sizeof(dispatch_table))) return 0;
    memcpy(&dispatch_table,
           metadata + (WIDGET_TEXT_MEMBER_ID & 0x0fffu) * sizeof(void*),
           sizeof(dispatch_table));
    if (!dispatch_table ||
        !ptr_readable(dispatch_table + 0x100, sizeof(getter))) return 0;
    memcpy(&getter, dispatch_table + 0x100, sizeof(getter));
    if (!ptr_executable((const void*)getter)) return 0;
    memcpy(&engine_value, base + ENGINE_EMPTY_STRING_RVA,
           sizeof(engine_value));
    getter(widget, WIDGET_TEXT_MEMBER_ID, &engine_value);
    copied = liquid_copy_engine_string(engine_value, out, out_size);
    if (ptr_readable(base + ENGINE_STRING_RELEASE_RVA,
                     sizeof(release_string))) {
        memcpy(&release_string, base + ENGINE_STRING_RELEASE_RVA,
               sizeof(release_string));
        if (release_string) release_string(&engine_value);
    }
    return copied;
}

/* SpinBox.Text uses the native Bionic::String constructor and the same
   member dispatch used by TK17's BuildControls implementation. This avoids
   passing a plain C string to a property that expects a managed engine
   string. */
static int liquid_spinbox_widget_set_value(void *widget, const char *value)
{
    HMODULE executable;
    BYTE *base;
    BYTE *metadata;
    BYTE *dispatch_table;
    engine_string_construct_cstr_t construct_string;
    engine_string_release_t release_string;
    widget_set_string_t setter;
    char *engine_value = NULL;
    if (!widget || !value ||
        !ptr_readable((BYTE*)widget - SCRIPT_OBJECT_META_BACK_OFFSET,
                      sizeof(metadata))) return 0;
    executable = GetModuleHandleA(NULL);
    if (!executable) return 0;
    base = (BYTE*)executable;
    if (!ptr_readable(base + ENGINE_STRING_CSTR_CONSTRUCT_RVA,
                      sizeof(construct_string)) ||
        !ptr_readable(base + ENGINE_STRING_RELEASE_RVA,
                      sizeof(release_string))) return 0;
    memcpy(&metadata,
           (BYTE*)widget - SCRIPT_OBJECT_META_BACK_OFFSET,
           sizeof(metadata));
    if (!metadata ||
        !ptr_readable(metadata +
                          (WIDGET_TEXT_MEMBER_ID & 0x0fffu) * sizeof(void*),
                      sizeof(dispatch_table))) return 0;
    memcpy(&dispatch_table,
           metadata + (WIDGET_TEXT_MEMBER_ID & 0x0fffu) * sizeof(void*),
           sizeof(dispatch_table));
    if (!dispatch_table ||
        !ptr_readable(dispatch_table + 0x104, sizeof(setter))) return 0;
    memcpy(&setter, dispatch_table + 0x104, sizeof(setter));
    memcpy(&construct_string, base + ENGINE_STRING_CSTR_CONSTRUCT_RVA,
           sizeof(construct_string));
    memcpy(&release_string, base + ENGINE_STRING_RELEASE_RVA,
           sizeof(release_string));
    if (!ptr_executable((const void*)setter) ||
        !ptr_executable((const void*)construct_string) ||
        !ptr_executable((const void*)release_string)) return 0;
    construct_string(&engine_value, value);
    if (!engine_value) return 0;
    setter(widget, WIDGET_TEXT_MEMBER_ID, engine_value);
    release_string(&engine_value);
    return 1;
}

static void liquid_sync_spinbox_from_ini(liquid_setting_binding_t *binding,
                                         void *widget)
{
    char requested[128];
    char before[128];
    char after[128];
    int have_before;
    if (!binding || !widget ||
        !liquid_setting_ini_spinbox_value(binding, requested,
                                          sizeof(requested))) return;
    have_before = liquid_spinbox_widget_value(widget, before,
                                              sizeof(before));
    if (have_before && strcmp(before, requested) == 0) return;
    liquid_setting_sync_depth++;
    if (!liquid_spinbox_widget_set_value(widget, requested)) {
        liquid_setting_sync_depth--;
        log_line("settings spinbox sync failed param=\"%s\" requested=\"%s\" widget=%p",
                 binding->param_name, requested, widget);
        return;
    }
    liquid_setting_sync_depth--;
    if (liquid_spinbox_widget_value(widget, after, sizeof(after))) {
        log_line("settings spinbox synced param=\"%s\" ini=\"%s\" previous=\"%s\" live=\"%s\"",
                 binding->param_name, requested,
                 have_before ? before : "unavailable", after);
    } else {
        log_line("settings spinbox synced param=\"%s\" ini=\"%s\" live=unavailable",
                 binding->param_name, requested);
    }
}

static int THISCALL hook_Customizer_BuildControls(void *self, void *arg1,
                                                   void *arg2, void *arg3)
{
    int result;
    void **parameters = NULL;
    void **records = NULL;
    int parameter_count = 0;
    int record_count = 0;
    int count;
    int index;
    result = tramp_Customizer_BuildControls ?
        tramp_Customizer_BuildControls(self, arg1, arg2, arg3) : 0;
    if (!self ||
        !ptr_readable((BYTE*)self + 0x14, sizeof(parameters)) ||
        !ptr_readable((BYTE*)self + 0x18, sizeof(records))) return result;
    memcpy(&parameters, (BYTE*)self + 0x14, sizeof(parameters));
    memcpy(&records, (BYTE*)self + 0x18, sizeof(records));
    if (!parameters || !records ||
        !ptr_readable((BYTE*)parameters - sizeof(parameter_count),
                      sizeof(parameter_count)) ||
        !ptr_readable((BYTE*)records - sizeof(record_count),
                      sizeof(record_count))) return result;
    memcpy(&parameter_count,
           (BYTE*)parameters - sizeof(parameter_count),
           sizeof(parameter_count));
    memcpy(&record_count, (BYTE*)records - sizeof(record_count),
           sizeof(record_count));
    if (parameter_count <= 0 || record_count <= 0 ||
        parameter_count > 4096 || record_count > 4096) return result;
    count = parameter_count < record_count ? parameter_count : record_count;
    if (!ptr_readable(parameters,
                      (size_t)count * sizeof(*parameters)) ||
        !ptr_readable(records, (size_t)count * sizeof(*records))) return result;
    for (index = 0; index < count; ++index) {
        char parameter_name[128];
        void *record = records[index];
        void *widget = NULL;
        liquid_setting_binding_t *binding;
        if (!parameters[index] || !record ||
            !liquid_custom_parameter_name(parameters[index], parameter_name,
                                          sizeof(parameter_name))) continue;
        binding = liquid_setting_binding_by_name(parameter_name);
        if (!binding ||
            (binding->type != LIQUID_SETTING_BOOL &&
             binding->type != LIQUID_SETTING_TEXT) ||
            !ptr_readable((BYTE*)record + 0x24, sizeof(widget))) continue;
        memcpy(&widget, (BYTE*)record + 0x24, sizeof(widget));
        if (!widget) continue;
        liquid_sync_spinbox_from_ini(binding, widget);
    }
    return result;
}

static void liquid_handle_setting_change(const char *param_name,
                                         const char *string_value)
{
    liquid_setting_binding_t *binding;
    char normalized[128];
    int enabled;
    float slider_value;
    char *end = NULL;
    double parsed;
    binding = liquid_setting_binding_by_name(param_name);
    if (!binding) return;
    normalized[0] = 0;
    if (binding->type == LIQUID_SETTING_BOOL) {
        if (!liquid_setting_bool_value(string_value, &enabled)) return;
        lstrcpynA(normalized, enabled ? "true" : "false",
                  sizeof(normalized));
    } else if (binding->type == LIQUID_SETTING_TEXT) {
        if (!string_value || !string_value[0]) return;
        lstrcpynA(normalized, string_value, sizeof(normalized));
    } else if (liquid_slider_widget_value(binding->slider_widget,
                                          &slider_value)) {
        if (binding->type == LIQUID_SETTING_INT) {
            _snprintf(normalized, sizeof(normalized), "%d",
                      (int)floorf(slider_value + 0.5f));
        } else {
            _snprintf(normalized, sizeof(normalized), "%.6g", slider_value);
        }
        normalized[sizeof(normalized) - 1] = 0;
    } else if (string_value && string_value[0]) {
        parsed = strtod(string_value, &end);
        if (end == string_value || !_finite(parsed)) return;
        if (binding->type == LIQUID_SETTING_INT) {
            _snprintf(normalized, sizeof(normalized), "%d",
                      (int)floor(parsed + 0.5));
        } else {
            _snprintf(normalized, sizeof(normalized), "%.6g", parsed);
        }
        normalized[sizeof(normalized) - 1] = 0;
    } else {
        return;
    }
    if (WritePrivateProfileStringA(binding->section, binding->key,
                                   normalized, config_path)) {
        log_line("settings changed param=\"%s\" value=\"%s\" ini=[%s] %s note=\"normal INI hot-reload scheduled\"",
                 binding->param_name, normalized,
                 binding->section, binding->key);
    } else {
        log_line("settings write failed param=\"%s\" ini=[%s] %s path=\"%s\"",
                 binding->param_name, binding->section,
                 binding->key, config_path);
    }
}

static void THISCALL hook_ConfigEditor_ParamChange(
    void *self, const char *parameter, const char *value,
    DWORD arg3, DWORD arg4)
{
    char parameter_text[128];
    char value_text[128];
    const char *parameter_cstr = stringref_cstr_a(parameter);
    const char *value_cstr = stringref_cstr_a(value);
    parameter_text[0] = 0;
    value_text[0] = 0;
    if (parameter_cstr &&
        _strnicmp(parameter_cstr, "NcLiquids", 9) == 0) {
        lstrcpynA(parameter_text, parameter_cstr, sizeof(parameter_text));
        if (value_cstr)
            lstrcpynA(value_text, value_cstr, sizeof(value_text));
    }
    if (tramp_ConfigEditor_ParamChange) {
        tramp_ConfigEditor_ParamChange(self, parameter, value, arg3, arg4);
    }
    if (parameter_text[0] && !liquid_setting_sync_depth) {
        liquid_handle_setting_change(parameter_text,
                                     value_text[0] ? value_text : NULL);
    }
}

static int THISCALL hook_Customizer_CreateSlider(
    void *self, void *parameter, void *record, void *parent, float y,
    int preset_index, int has_labels)
{
    int result;
    char parameter_name[128];
    void *slider = NULL;
    liquid_setting_binding_t *binding;
    result = tramp_Customizer_CreateSlider ?
        tramp_Customizer_CreateSlider(self, parameter, record, parent, y,
                                      preset_index, has_labels) : 0;
    if (!liquid_custom_parameter_name(parameter, parameter_name,
                                      sizeof(parameter_name))) return result;
    binding = liquid_setting_binding_by_name(parameter_name);
    if (!binding || binding->type == LIQUID_SETTING_BOOL ||
        binding->type == LIQUID_SETTING_TEXT || !record) return result;
    if (preset_index < 0) {
        if (ptr_readable((BYTE*)record + 0x04, sizeof(slider)))
            memcpy(&slider, (BYTE*)record + 0x04, sizeof(slider));
    } else if (preset_index < 1024 &&
               ptr_readable((BYTE*)record + 0x08 +
                            (size_t)preset_index * sizeof(void*),
                            sizeof(slider))) {
        memcpy(&slider,
               (BYTE*)record + 0x08 +
               (size_t)preset_index * sizeof(void*),
               sizeof(slider));
    }
    if (slider) {
        binding->slider_widget = slider;
        /* Preset slots are stored separately and must retain their preset
           values. Only the main live control mirrors Config.ini. */
        if (preset_index < 0)
            liquid_sync_slider_from_ini(binding, slider);
    }
    return result;
}

static void patch_liquid_config_editor_hooks(void)
{
    HMODULE executable = GetModuleHandleA(NULL);
    BYTE *target;
    static const BYTE param_expected[6] = {
        0x55, 0x8b, 0xec, 0x83, 0xe4, 0xc0
    };
    static const BYTE slider_expected[6] = {
        0x55, 0x8b, 0xec, 0x83, 0xe4, 0xc0
    };
    static const BYTE prepare_controls_expected[6] = {
        0x56, 0x8b, 0xf1, 0x8b, 0x4e, 0x70
    };
    static const BYTE build_controls_expected[9] = {
        0x55, 0x8b, 0xec, 0x81, 0xec, 0xe0, 0x01, 0x00, 0x00
    };
    if (!executable) return;
    if (!customizer_prepare_controls_hook_installed) {
        target = (BYTE*)executable + 0x001cca00;
        if (ptr_executable(target) &&
            (target[0] == 0xe9 ||
             memcmp(target, prepare_controls_expected,
                    sizeof(prepare_controls_expected)) == 0) &&
            install_inline_hook(target,
                                (void*)hook_Customizer_PrepareControls,
                                sizeof(prepare_controls_expected),
                                (void**)&tramp_Customizer_PrepareControls)) {
            InterlockedExchange(
                &customizer_prepare_controls_hook_installed, 1);
            log_line("settings ConfigEditor pre-enumeration hook installed target=%p previous=%p identity=ParamDescription",
                     target, (void*)tramp_Customizer_PrepareControls);
        }
    }
    if (!configeditor_param_change_hook_installed) {
        target = (BYTE*)executable + 0x000fe280;
        if (ptr_executable(target) &&
            (target[0] == 0xe9 ||
             memcmp(target, param_expected, sizeof(param_expected)) == 0) &&
            install_inline_hook(target,
                                (void*)hook_ConfigEditor_ParamChange,
                                sizeof(param_expected),
                                (void**)&tramp_ConfigEditor_ParamChange)) {
            InterlockedExchange(&configeditor_param_change_hook_installed, 1);
            log_line("settings ConfigEditor change hook installed target=%p previous=%p bindings=%u",
                     target, (void*)tramp_ConfigEditor_ParamChange,
                     (unsigned int)(sizeof(liquid_setting_bindings) /
                                    sizeof(liquid_setting_bindings[0])));
        }
    }
    if (!customizer_create_slider_hook_installed) {
        target = (BYTE*)executable + 0x001c34f0;
        if (ptr_executable(target) &&
            (target[0] == 0xe9 ||
             memcmp(target, slider_expected, sizeof(slider_expected)) == 0) &&
            install_inline_hook(target,
                                (void*)hook_Customizer_CreateSlider,
                                sizeof(slider_expected),
                                (void**)&tramp_Customizer_CreateSlider)) {
            InterlockedExchange(&customizer_create_slider_hook_installed, 1);
            log_line("settings ConfigEditor slider hook installed target=%p previous=%p",
                     target, (void*)tramp_Customizer_CreateSlider);
        }
    }
    if (!customizer_build_controls_hook_installed) {
        target = (BYTE*)executable + 0x001c5110;
        if (ptr_executable(target) &&
            (target[0] == 0xe9 ||
             memcmp(target, build_controls_expected,
                    sizeof(build_controls_expected)) == 0) &&
            install_inline_hook(target,
                                (void*)hook_Customizer_BuildControls,
                                sizeof(build_controls_expected),
                                (void**)&tramp_Customizer_BuildControls)) {
            InterlockedExchange(&customizer_build_controls_hook_installed,
                                1);
            log_line("settings ConfigEditor spinbox hook installed target=%p previous=%p",
                     target, (void*)tramp_Customizer_BuildControls);
        }
    }
}

static int supported_executable(HMODULE exe)
{
    IMAGE_DOS_HEADER *dos;
    IMAGE_NT_HEADERS *nt;
    if (!exe) return 0;
    dos = (IMAGE_DOS_HEADER*)exe;
    if (!ptr_readable(dos, sizeof(*dos)) ||
        dos->e_magic != IMAGE_DOS_SIGNATURE) return 0;
    nt = (IMAGE_NT_HEADERS*)((BYTE*)exe + dos->e_lfanew);
    if (!ptr_readable(nt, sizeof(*nt)) ||
        nt->Signature != IMAGE_NT_SIGNATURE) return 0;
    log_line("executable fingerprint base=%p timestamp=0x%08lx image_size=0x%08lx checksum=0x%08lx expected_timestamp=0x%08lx expected_image_size=0x%08lx expected_checksum=0x%08lx",
             exe,
             (unsigned long)nt->FileHeader.TimeDateStamp,
             (unsigned long)nt->OptionalHeader.SizeOfImage,
             (unsigned long)nt->OptionalHeader.CheckSum,
             (unsigned long)TK17_EXE_TIMESTAMP,
             (unsigned long)TK17_EXE_IMAGE_SIZE,
             (unsigned long)TK17_EXE_CHECKSUM);
    return nt->FileHeader.TimeDateStamp == TK17_EXE_TIMESTAMP &&
           nt->OptionalHeader.SizeOfImage == TK17_EXE_IMAGE_SIZE &&
           nt->OptionalHeader.CheckSum == TK17_EXE_CHECKSUM;
}

static void resolve_engine_symbols(void)
{
    HMODULE sys = GetModuleHandleA("ThriXXX010278-SYS.dll");
    HMODULE app = GetModuleHandleA("ThriXXX010278-APP.dll");
    if (sys) {
        if (!engine_StringRefHash32) {
            FARPROC proc = GetProcAddress(
                sys, "?Hash32@StringRef@Bionic@@QBEIXZ");
            if (!proc) proc = GetProcAddress(sys, (LPCSTR)1640);
            copy_proc_address(&engine_StringRefHash32, proc,
                              sizeof(engine_StringRefHash32));
        }
        if (!engine_NameHashFind) {
            FARPROC proc = GetProcAddress(
                sys, "?Find@NameHash@Bionic@@QBEPAXIABVStringRef@2@@Z");
            if (!proc) proc = GetProcAddress(sys, (LPCSTR)1123);
            copy_proc_address(&engine_NameHashFind, proc,
                              sizeof(engine_NameHashFind));
        }
        if (!engine_GetWeakObjTarget) {
            FARPROC proc = GetProcAddress(
                sys,
                "?GetWeakObjTarget@Abstract@Bionic@@QBEPBVWeakObjTarget@2@XZ");
            if (!proc) proc = GetProcAddress(sys, (LPCSTR)1618);
            copy_proc_address(&engine_GetWeakObjTarget, proc,
                              sizeof(engine_GetWeakObjTarget));
        }
        if (!engine_ObjectGetTypeInfo) {
            FARPROC proc = GetProcAddress(
                sys, "?GetTypeInfo@Object@Bionic@@QBEPBVTypeInfo@2@XZ");
            copy_proc_address(&engine_ObjectGetTypeInfo, proc,
                              sizeof(engine_ObjectGetTypeInfo));
        }
        if (!engine_BlendControlTypeInfo) {
            static_get_type_info_t get_type_info = NULL;
            FARPROC proc = GetProcAddress(
                sys,
                "?GetBlendControlTypeInfo@BlendControl@Bionic@@SAPBVTypeInfo@2@XZ");
            copy_proc_address(&get_type_info, proc, sizeof(get_type_info));
            if (get_type_info)
                engine_BlendControlTypeInfo = get_type_info();
        }
        if (!engine_G_NilWeakObjTarget_ptr) {
            FARPROC proc = GetProcAddress(
                sys, "?G_NilWeakObjTarget@Bionic@@3QAVWeakObjTarget@1@A");
            if (!proc) proc = GetProcAddress(sys, (LPCSTR)1216);
            copy_proc_address(&engine_G_NilWeakObjTarget_ptr, proc,
                              sizeof(engine_G_NilWeakObjTarget_ptr));
        }
        if (!engine_G_NilObject_ptr) {
            FARPROC proc = GetProcAddress(
                sys, "?G_NilObject@Bionic@@3QAVScriptObject@1@A");
            if (!proc) proc = GetProcAddress(sys, (LPCSTR)1214);
            copy_proc_address(&engine_G_NilObject_ptr, proc,
                              sizeof(engine_G_NilObject_ptr));
        }
        if (!engine_TimerSetSeconds) {
            FARPROC proc = GetProcAddress(
                sys, "?SetSeconds@Timer@Bionic@@QAEXN@Z");
            copy_proc_address(&engine_TimerSetSeconds, proc,
                              sizeof(engine_TimerSetSeconds));
        }
    }
    if (app && !engine_FindObjC) {
        FARPROC proc = GetProcAddress(
            app, "?FindObjC@AppMain@@YAPAVScriptObject@Bionic@@PBD@Z");
        copy_proc_address(&engine_FindObjC, proc, sizeof(engine_FindObjC));
    }
    if (sys && !engine_GetModelViewRotationPivot) {
        FARPROC proc = GetProcAddress(
            sys,
            "?GetModelViewRotationPivot@Bionic@@YAXPAVScriptObject@1@AAVVector3f@1@@Z");
        copy_proc_address(&engine_GetModelViewRotationPivot, proc,
                          sizeof(engine_GetModelViewRotationPivot));
    }
}

static int liquid_text(const char *text)
{
    return text && (contains_i(text, "cumshot") ||
                    contains_i(text, "internalcum") ||
                    contains_i(text, "person_cum") ||
                    contains_i(text, "finish_cum") ||
                    contains_i(text, "spermray") ||
                    contains_i(text, "liquidjet") ||
                    contains_i(text, "ejac"));
}

static float liquid_absf(float value)
{
    return value < 0.0f ? -value : value;
}

static float liquid_vec3_length(const float value[3])
{
    return (float)sqrt((double)(value[0] * value[0] +
                               value[1] * value[1] +
                               value[2] * value[2]));
}

static int liquid_vec3_normalize(float value[3])
{
    float length = liquid_vec3_length(value);
    if (length < 0.00001f || !_finite(length)) return 0;
    value[0] /= length;
    value[1] /= length;
    value[2] /= length;
    return 1;
}

static float liquid_vec3_dot(const float a[3], const float b[3])
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

static void liquid_vec3_cross(const float a[3], const float b[3],
                              float result[3])
{
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
}

static float liquid_random_signed(void)
{
    liquid_random_state = liquid_random_state * 1664525u + 1013904223u;
    return ((float)((liquid_random_state >> 8) & 0xffffu) / 32767.5f) - 1.0f;
}

static int liquid_module_rva(const void *address, const char *module_name,
                             DWORD expected_rva)
{
    MEMORY_BASIC_INFORMATION mbi;
    char path[MAX_PATH * 2];
    const char *base_name;
    DWORD rva;
    if (!address || !VirtualQuery(address, &mbi, sizeof(mbi))) return 0;
    path[0] = 0;
    GetModuleFileNameA((HMODULE)mbi.AllocationBase, path, sizeof(path));
    base_name = strrchr(path, '\\');
    base_name = base_name ? base_name + 1 : path;
    if (_stricmp(base_name, module_name) != 0) return 0;
    rva = (DWORD)((const BYTE*)address - (const BYTE*)mbi.AllocationBase);
    return rva == expected_rva;
}

static void *liquid_find_object(const char *name)
{
    void *raw;
    void *nil_weak = NULL;
    void *nil_object = NULL;
    if (!engine_FindObjC || !name || !name[0]) return NULL;
    raw = engine_FindObjC(name);
    if (engine_G_NilWeakObjTarget_ptr &&
        ptr_readable(engine_G_NilWeakObjTarget_ptr, sizeof(void*)))
        nil_weak = *engine_G_NilWeakObjTarget_ptr;
    if (engine_G_NilObject_ptr &&
        ptr_readable(engine_G_NilObject_ptr, sizeof(void*)))
        nil_object = *engine_G_NilObject_ptr;
    if (!raw || raw == nil_weak || raw == nil_object) return NULL;
    /* GetModelViewRotationPivot consumes the scene object returned by
       FindObjC directly.  Unwrapping it selects a different engine object
       which does not expose the queried node pivot. */
    return raw;
}

static int liquid_object_view_pivot(const char *name, float view[3])
{
    static LONG raw_mode_logged;
    static LONG weak_mode_logged;
    static LONG named_mode_logged;
    static LONG failure_log_count;
    void *raw = liquid_find_object(name);
    void *weak = NULL;
    void *named = NULL;
    int valid;
    if (!engine_GetModelViewRotationPivot || !view) return 0;
    if (raw) {
        view[0] = view[1] = view[2] = NAN;
        engine_GetModelViewRotationPivot(raw, view);
        valid = _finite(view[0]) && _finite(view[1]) && _finite(view[2]) &&
                liquid_absf(view[0]) < 1000000.0f &&
                liquid_absf(view[1]) < 1000000.0f &&
                liquid_absf(view[2]) < 1000000.0f &&
                view[0] * view[0] + view[1] * view[1] +
                    view[2] * view[2] > 0.000001f;
        if (valid) {
            if (!InterlockedExchange(&raw_mode_logged, 1))
                log_line("liquid pivot resolver active mode=FindObjC-direct query=\"%s\" raw=%p view=(%.4f,%.4f,%.4f)",
                         name, raw, view[0], view[1], view[2]);
            return 1;
        }
    }
    if (raw && engine_GetWeakObjTarget) {
        weak = engine_GetWeakObjTarget(raw);
        if (weak && weak != raw) {
            view[0] = view[1] = view[2] = NAN;
            engine_GetModelViewRotationPivot(weak, view);
            valid = _finite(view[0]) && _finite(view[1]) &&
                    _finite(view[2]) &&
                    liquid_absf(view[0]) < 1000000.0f &&
                    liquid_absf(view[1]) < 1000000.0f &&
                    liquid_absf(view[2]) < 1000000.0f &&
                    view[0] * view[0] + view[1] * view[1] +
                        view[2] * view[2] > 0.000001f;
            if (valid) {
                if (!InterlockedExchange(&weak_mode_logged, 1))
                    log_line("liquid pivot resolver active mode=weak-target query=\"%s\" raw=%p weak=%p view=(%.4f,%.4f,%.4f)",
                             name, raw, weak,
                             view[0], view[1], view[2]);
                return 1;
            }
        }
    }
    named = find_remembered_liquid_transform_node(name);
    if (!named || named == raw || named == weak) {
        LONG failure = InterlockedIncrement(&failure_log_count);
        if (failure <= 20)
            log_line("liquid pivot resolver failed query=\"%s\" raw=%p weak=%p named=%p reason=%s",
                     name, raw, weak, named,
                     !named ? "live-name-not-captured" :
                              "captured-object-already-rejected");
        return 0;
    }
    view[0] = view[1] = view[2] = NAN;
    engine_GetModelViewRotationPivot(named, view);
    valid = _finite(view[0]) && _finite(view[1]) && _finite(view[2]) &&
            liquid_absf(view[0]) < 1000000.0f &&
            liquid_absf(view[1]) < 1000000.0f &&
            liquid_absf(view[2]) < 1000000.0f &&
            view[0] * view[0] + view[1] * view[1] +
                view[2] * view[2] > 0.000001f;
    if (valid && !InterlockedExchange(&named_mode_logged, 1))
        log_line("liquid pivot resolver active mode=name-hook-live-object query=\"%s\" object=%p view=(%.4f,%.4f,%.4f)",
                 name, named, view[0], view[1], view[2]);
    if (!valid) {
        LONG failure = InterlockedIncrement(&failure_log_count);
        if (failure <= 20)
            log_line("liquid pivot resolver failed query=\"%s\" raw=%p weak=%p named=%p reason=live-object-invalid-view view=(%.4f,%.4f,%.4f)",
                     name, raw, weak, named,
                     view[0], view[1], view[2]);
    }
    return valid;
}

static int liquid_live_object_view_pivot(void *object, const char *label,
                                         float view[3])
{
    static LONG live_mode_logged;
    int valid;
    if (!object || !view || !engine_GetModelViewRotationPivot ||
        !ptr_readable(object, sizeof(void*))) return 0;
    view[0] = view[1] = view[2] = NAN;
    engine_GetModelViewRotationPivot(object, view);
    valid = _finite(view[0]) && _finite(view[1]) && _finite(view[2]) &&
            liquid_absf(view[0]) < 1000000.0f &&
            liquid_absf(view[1]) < 1000000.0f &&
            liquid_absf(view[2]) < 1000000.0f &&
            view[0] * view[0] + view[1] * view[1] +
                view[2] * view[2] > 0.000001f;
    if (valid && !InterlockedExchange(&live_mode_logged, 1))
        log_line("liquid pivot resolver active mode=retained-live-object label=\"%s\" object=%p view=(%.4f,%.4f,%.4f)",
                 label ? label : "", object,
                 view[0], view[1], view[2]);
    return valid;
}

/* FindObjC can return a readable AppAction wrapper for a path which is not
   present.  Calling GetModelViewRotationPivot on that wrapper crashes.  The
   wrapper's weak target, however, resolves to TK17's Nil sentinel.  Validate
   that target before querying the wrapper.  Do not require the iNameSet hook
   to have observed the node: most body joints already exist before this DLL
   installs its name hook, and that older check rejected every valid joint. */
static int liquid_exact_node_view_pivot(const char *path,
                                        const char *expected_node_name,
                                        float view[3])
{
    static LONG rejected_log_count;
    void *object;
    void *target = NULL;
    void *nil_weak = NULL;
    void *nil_object = NULL;
    const char *actual_name;
    const char *target_name = NULL;
    int exact_name = 0;
    int target_valid = 0;
    LONG rejected;
    if (!path || !expected_node_name || !view) return 0;
    object = liquid_find_object(path);
    actual_name = object ? known_object_name(object) : NULL;
    if (actual_name &&
        _stricmp(actual_name, expected_node_name) == 0)
        exact_name = 1;
    if (engine_G_NilWeakObjTarget_ptr &&
        ptr_readable(engine_G_NilWeakObjTarget_ptr, sizeof(void*)))
        nil_weak = *engine_G_NilWeakObjTarget_ptr;
    if (engine_G_NilObject_ptr &&
        ptr_readable(engine_G_NilObject_ptr, sizeof(void*)))
        nil_object = *engine_G_NilObject_ptr;
    if (object && engine_GetWeakObjTarget) {
        target = engine_GetWeakObjTarget(object);
        if (target && ptr_readable(target, sizeof(void*))) {
            target_valid = 1;
            target_name = known_object_name(target);
        }
        if (target_name &&
            _stricmp(target_name, expected_node_name) == 0)
            exact_name = 1;
    }
    if (!object || object == nil_weak || object == nil_object ||
        target == nil_weak || target == nil_object ||
        (!exact_name && !target_valid)) {
        rejected = InterlockedIncrement(&rejected_log_count);
        if (rejected <= 24)
            log_line("liquid body-follow node skipped path=\"%s\" object=%p captured_name=\"%s\" target=%p target_name=\"%s\" expected=\"%s\"",
                     path, object, actual_name ? actual_name : "", target,
                     target_name ? target_name : "",
                     expected_node_name);
        return 0;
    }
    /* GetModelViewRotationPivot expects the FindObjC wrapper, even when the
       exact node identity was proven through its weak target. */
    return liquid_live_object_view_pivot(object, path, view);
}

static int liquid_view_to_world_point(const float view[3], float world[3])
{
    const float *m = captured_camera_inverse;
    if (!view || !world ||
        !InterlockedCompareExchange(&captured_camera_inverse_valid, 0, 0))
        return 0;
    world[0] = m[12] + view[0] * m[0] + view[1] * m[4] + view[2] * m[8];
    world[1] = m[13] + view[0] * m[1] + view[1] * m[5] + view[2] * m[9];
    world[2] = m[14] + view[0] * m[2] + view[1] * m[6] + view[2] * m[10];
    return _finite(world[0]) && _finite(world[1]) && _finite(world[2]);
}

static const char *liquid_contact_anchor_names[LIQUID_CONTACT_ANCHOR_COUNT] = {
    "root",
    "spine_joint04",
    "head_joint02",
    "breast_scale_L_joint",
    "breast_scale_R_joint",
    "butt_L_joint01",
    "butt_R_joint01",
    "shoulder_L_joint",
    "shoulder_R_joint",
    "elbow_L_joint",
    "elbow_R_joint",
    "wrist_L_joint",
    "wrist_R_joint",
    "knee_L_joint",
    "knee_R_joint",
    "ankle_L_joint",
    "ankle_R_joint",
    "hip_L_joint",
    "hip_R_joint",
    "penis_joint01",
    "penis_joint02",
    "penis_joint03",
    "testicles_joint01",
    "testicles_joint02"
};

static const liquid_contact_segment_t liquid_contact_segments[] = {
    { LIQUID_CONTACT_ROOT, LIQUID_CONTACT_SPINE },
    { LIQUID_CONTACT_SPINE, LIQUID_CONTACT_HEAD },
    { LIQUID_CONTACT_SPINE, LIQUID_CONTACT_SHOULDER_L },
    { LIQUID_CONTACT_SHOULDER_L, LIQUID_CONTACT_ELBOW_L },
    { LIQUID_CONTACT_ELBOW_L, LIQUID_CONTACT_WRIST_L },
    { LIQUID_CONTACT_SPINE, LIQUID_CONTACT_SHOULDER_R },
    { LIQUID_CONTACT_SHOULDER_R, LIQUID_CONTACT_ELBOW_R },
    { LIQUID_CONTACT_ELBOW_R, LIQUID_CONTACT_WRIST_R },
    { LIQUID_CONTACT_HIP_L, LIQUID_CONTACT_KNEE_L },
    { LIQUID_CONTACT_KNEE_L, LIQUID_CONTACT_ANKLE_L },
    { LIQUID_CONTACT_HIP_R, LIQUID_CONTACT_KNEE_R },
    { LIQUID_CONTACT_KNEE_R, LIQUID_CONTACT_ANKLE_R },
    { LIQUID_CONTACT_PENIS_01, LIQUID_CONTACT_PENIS_02 },
    { LIQUID_CONTACT_PENIS_02, LIQUID_CONTACT_PENIS_03 },
    { LIQUID_CONTACT_TESTICLES_01, LIQUID_CONTACT_TESTICLES_02 }
};

/* Build a stable moving frame for a skeletal segment. The segment axis
   follows the limb itself; the second axis uses the person's depth axis as
   a roll reference. This lets a contact inherit shoulder/elbow/knee motion
   instead of being reconstructed in the pelvis basis. */
static int liquid_contact_segment_frame(
    const liquid_contact_person_t *person, int start_anchor,
    int end_anchor, float t, float origin[3], float along[3],
    float side[3], float normal[3])
{
    const float *start;
    const float *end;
    float projection;
    int axis;
    if (!person || !person->valid || start_anchor < 0 ||
        start_anchor >= LIQUID_CONTACT_ANCHOR_COUNT || end_anchor < 0 ||
        end_anchor >= LIQUID_CONTACT_ANCHOR_COUNT ||
        !person->anchors[start_anchor].valid ||
        !person->anchors[end_anchor].valid)
        return 0;
    start = person->anchors[start_anchor].world;
    end = person->anchors[end_anchor].world;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    for (axis = 0; axis < 3; axis++) {
        origin[axis] = start[axis] + (end[axis] - start[axis]) * t;
        along[axis] = end[axis] - start[axis];
    }
    if (!liquid_vec3_normalize(along)) return 0;
    projection = liquid_vec3_dot(person->depth, along);
    for (axis = 0; axis < 3; axis++)
        side[axis] = person->depth[axis] - along[axis] * projection;
    if (!liquid_vec3_normalize(side)) {
        projection = liquid_vec3_dot(person->horizontal, along);
        for (axis = 0; axis < 3; axis++)
            side[axis] = person->horizontal[axis] -
                         along[axis] * projection;
        if (!liquid_vec3_normalize(side)) return 0;
    }
    liquid_vec3_cross(along, side, normal);
    return liquid_vec3_normalize(normal);
}

static int liquid_contact_target_position(
    const liquid_contact_person_t *person,
    const liquid_particle_t *particle, float target[3])
{
    int start_anchor;
    int end_anchor;
    int axis;
    if (!person || !particle || !target ||
        particle->contact_anchor < 1 ||
        particle->contact_anchor > LIQUID_CONTACT_ANCHOR_COUNT)
        return 0;
    start_anchor = particle->contact_anchor - 1;
    end_anchor = particle->contact_anchor_end - 1;
    if (particle->contact_anchor_end >= 1 &&
        particle->contact_anchor_end <= LIQUID_CONTACT_ANCHOR_COUNT) {
        float origin[3], along[3], side[3], normal[3];
        if (!liquid_contact_segment_frame(
                person, start_anchor, end_anchor,
                particle->contact_segment_t, origin, along, side, normal))
            return 0;
        for (axis = 0; axis < 3; axis++)
            target[axis] = origin[axis] +
                side[axis] * particle->contact_local_offset[0] +
                normal[axis] * particle->contact_local_offset[1];
        return 1;
    }
    if (!person->valid || !person->anchors[start_anchor].valid) return 0;
    for (axis = 0; axis < 3; axis++)
        target[axis] = person->anchors[start_anchor].world[axis] +
            person->horizontal[axis] * particle->contact_local_offset[0] +
            person->vertical[axis] * particle->contact_local_offset[1] +
            person->depth[axis] * particle->contact_local_offset[2];
    return 1;
}

static int liquid_has_active_particles(void)
{
    int i;
    for (i = 0; i < cfg.particle_limit; i++)
        if (liquid_particles[i].active) return 1;
    return 0;
}

static int liquid_has_airborne_model_particles(void)
{
    int i;
    for (i = 0; i < cfg.particle_limit; i++)
        if (liquid_particles[i].active &&
            !liquid_particles[i].collided &&
            liquid_particles[i].source_kind == 1) return 1;
    return 0;
}

static unsigned int liquid_body_follow_person_mask(void)
{
    int i;
    unsigned int person_mask = 0;
    if (!cfg.collision_follow_bodies) return 0;
    for (i = 0; i < cfg.particle_limit; i++) {
        const liquid_particle_t *particle = &liquid_particles[i];
        if (!particle->active || !particle->collided ||
            particle->source_kind != 1) continue;
        if (particle->contact_person >= 1 &&
            particle->contact_person <= 4) {
            person_mask |= 1u << (particle->contact_person - 1);
        } else {
            /* Until attachment is complete, preserve the original sampling
               of every live person. Besides supporting emission-cluster
               discovery, this keeps the prior samples used by first-frame
               attachment interpolation available. */
            return 0x0Fu;
        }
    }
    return person_mask;
}

static void liquid_update_contact_anchor_cache(void)
{
    static LONG active_person_log_count;
    liquid_contact_person_t next_people[4];
    DWORD now = GetTickCount();
    DWORD previous_tick = (DWORD)InterlockedCompareExchange(
        &liquid_contact_cache_tick, 0, 0);
    unsigned int person_mask;
    int person_index;
    if (!cfg.collision_enabled || !cfg.collision_follow_bodies) return;
    person_mask = liquid_body_follow_person_mask();
    if (!person_mask) return;
    /* Keep the body samples compact, but refresh at display cadence.  The
       follow path interpolates the two latest complete samples, so droplets
       no longer jump between sparsely sampled skeleton positions. */
    if (previous_tick && now - previous_tick < 16u) return;
    memset(next_people, 0, sizeof(next_people));
    for (person_index = 0; person_index < 4; person_index++) {
        liquid_contact_person_t *person =
            &next_people[person_index];
        int anchor_index;
        if (!(person_mask & (1u << person_index))) continue;
        for (anchor_index = 0;
             anchor_index < LIQUID_CONTACT_ANCHOR_COUNT;
             anchor_index++) {
            char name[128];
            float view[3];
            _snprintf(name, sizeof(name), "Person%02dAnim:Model01:%s",
                      person_index + 1,
                      liquid_contact_anchor_names[anchor_index]);
            name[sizeof(name) - 1] = 0;
            if (liquid_exact_node_view_pivot(
                    name, liquid_contact_anchor_names[anchor_index], view) &&
                liquid_view_to_world_point(
                    view, person->anchors[anchor_index].world))
                person->anchors[anchor_index].valid = 1;
            /* root is universal. If it is absent, this person slot is not
               live; avoid probing the remaining optional limb paths. */
            if (anchor_index == LIQUID_CONTACT_ROOT &&
                !person->anchors[anchor_index].valid)
                break;
        }
        if (person->anchors[LIQUID_CONTACT_HIP_L].valid &&
            person->anchors[LIQUID_CONTACT_HIP_R].valid &&
            person->anchors[LIQUID_CONTACT_SPINE].valid) {
            const float *left =
                person->anchors[LIQUID_CONTACT_HIP_L].world;
            const float *right =
                person->anchors[LIQUID_CONTACT_HIP_R].world;
            const float *spine =
                person->anchors[LIQUID_CONTACT_SPINE].world;
            float hip_mid[3];
            float vertical_dot;
            int axis;
            for (axis = 0; axis < 3; axis++) {
                hip_mid[axis] = (left[axis] + right[axis]) * 0.5f;
                person->horizontal[axis] = right[axis] - left[axis];
                person->vertical[axis] = spine[axis] - hip_mid[axis];
            }
            if (!liquid_vec3_normalize(person->horizontal)) continue;
            vertical_dot = liquid_vec3_dot(
                person->vertical, person->horizontal);
            for (axis = 0; axis < 3; axis++)
                person->vertical[axis] -=
                    person->horizontal[axis] * vertical_dot;
            if (!liquid_vec3_normalize(person->vertical)) continue;
            liquid_vec3_cross(person->horizontal, person->vertical,
                              person->depth);
            if (!liquid_vec3_normalize(person->depth)) continue;
            person->valid = 1;
            if (InterlockedIncrement(&active_person_log_count) <= 8)
                log_line("liquid body-follow basis active person=%d root=(%.4f,%.4f,%.4f) spine=(%.4f,%.4f,%.4f)",
                         person_index + 1,
                         person->anchors[LIQUID_CONTACT_ROOT].world[0],
                         person->anchors[LIQUID_CONTACT_ROOT].world[1],
                         person->anchors[LIQUID_CONTACT_ROOT].world[2],
                         spine[0], spine[1], spine[2]);
        }
    }
    now = GetTickCount();
    EnterCriticalSection(&contact_lock);
    memcpy(liquid_contact_people_older,
           liquid_contact_people_previous,
           sizeof(liquid_contact_people_older));
    InterlockedExchange(&liquid_contact_older_cache_tick,
        InterlockedCompareExchange(
            &liquid_contact_previous_cache_tick, 0, 0));
    memcpy(liquid_contact_people_previous, liquid_contact_people,
           sizeof(liquid_contact_people_previous));
    InterlockedExchange(&liquid_contact_previous_cache_tick,
                        (LONG)previous_tick);
    memcpy(liquid_contact_people, next_people,
           sizeof(liquid_contact_people));
    InterlockedExchange(&liquid_contact_cache_tick, (LONG)now);
    LeaveCriticalSection(&contact_lock);
}

static int liquid_attach_contact_to_body(
    liquid_particle_t *particle, int requested_person,
    float maximum_distance)
{
    static int attachment_log_count;
    liquid_contact_person_t people_snapshot[4];
    /* root/spine/hip anchors form a body-wide rigid basis.  Two metres
       covers head, torso, limbs, and clothing without selecting a distant
       model in ordinary scenes. */
    float best_distance_squared;
    int best_person = -1;
    int best_anchor = -1;
    int best_anchor_end = -1;
    float best_segment_t = 0.0f;
    int person_index;
    if (!particle || !cfg.collision_follow_bodies ||
        particle->contact_physx_person < 0 ||
        (particle->contact_physx_person == 0 &&
         !InterlockedCompareExchange(&particle->model_contact_verified, 0, 0))) return 0;
    if (maximum_distance <= 0.0f) maximum_distance = 2.0f;
    best_distance_squared = maximum_distance * maximum_distance;
    EnterCriticalSection(&contact_lock);
    memcpy(people_snapshot, liquid_contact_people,
           sizeof(people_snapshot));
    LeaveCriticalSection(&contact_lock);
    for (person_index = 0; person_index < 4; person_index++) {
        const liquid_contact_person_t *person =
            &people_snapshot[person_index];
        int anchor_index;
        if (requested_person >= 1 && requested_person <= 4 &&
            person_index != requested_person - 1) continue;
        if (!person->valid) continue;
        for (anchor_index = 0;
             anchor_index < LIQUID_CONTACT_ANCHOR_COUNT;
             anchor_index++) {
            const liquid_contact_anchor_t *anchor =
                &person->anchors[anchor_index];
            float dx, dy, dz, distance_squared;
            if (!anchor->valid) continue;
            dx = particle->position[0] - anchor->world[0];
            dy = particle->position[1] - anchor->world[1];
            dz = particle->position[2] - anchor->world[2];
            distance_squared = dx * dx + dy * dy + dz * dz;
            if (distance_squared < best_distance_squared) {
                best_distance_squared = distance_squared;
                best_person = person_index;
                best_anchor = anchor_index;
                best_anchor_end = -1;
                best_segment_t = 0.0f;
            }
        }
        {
            unsigned int segment_index;
            for (segment_index = 0;
                 segment_index < sizeof(liquid_contact_segments) /
                                     sizeof(liquid_contact_segments[0]);
                 segment_index++) {
                const liquid_contact_segment_t *segment =
                    &liquid_contact_segments[segment_index];
                const liquid_contact_anchor_t *start =
                    &person->anchors[segment->start_anchor];
                const liquid_contact_anchor_t *end =
                    &person->anchors[segment->end_anchor];
                float direction[3];
                float relative[3];
                float length_squared;
                float t;
                float closest[3];
                float dx, dy, dz, distance_squared;
                int axis;
                if (!start->valid || !end->valid) continue;
                for (axis = 0; axis < 3; axis++) {
                    direction[axis] = end->world[axis] - start->world[axis];
                    relative[axis] = particle->position[axis] -
                                     start->world[axis];
                }
                length_squared = liquid_vec3_dot(direction, direction);
                if (length_squared < 0.000001f) continue;
                t = liquid_vec3_dot(relative, direction) / length_squared;
                if (t < 0.0f) t = 0.0f;
                if (t > 1.0f) t = 1.0f;
                for (axis = 0; axis < 3; axis++)
                    closest[axis] = start->world[axis] +
                                    direction[axis] * t;
                dx = particle->position[0] - closest[0];
                dy = particle->position[1] - closest[1];
                dz = particle->position[2] - closest[2];
                distance_squared = dx * dx + dy * dy + dz * dz;
                if (distance_squared < best_distance_squared) {
                    best_distance_squared = distance_squared;
                    best_person = person_index;
                    best_anchor = segment->start_anchor;
                    best_anchor_end = segment->end_anchor;
                    best_segment_t = t;
                }
            }
        }
    }
    if (best_person >= 0 && best_anchor >= 0) {
        const liquid_contact_person_t *person =
            &people_snapshot[best_person];
        const float *anchor = person->anchors[best_anchor].world;
        float delta[3];
        int axis;
        particle->contact_person = best_person + 1;
        particle->contact_anchor = best_anchor + 1;
        particle->contact_anchor_end = best_anchor_end + 1;
        particle->contact_segment_t = best_segment_t;
        particle->contact_attached_tick = GetTickCount();
        if (best_anchor_end >= 0) {
            float origin[3], along[3], side[3], normal[3];
            if (!liquid_contact_segment_frame(
                    person, best_anchor, best_anchor_end, best_segment_t,
                    origin, along, side, normal))
                return 0;
            for (axis = 0; axis < 3; axis++)
                delta[axis] = particle->position[axis] - origin[axis];
            particle->contact_local_offset[0] =
                liquid_vec3_dot(delta, side);
            particle->contact_local_offset[1] =
                liquid_vec3_dot(delta, normal);
            particle->contact_local_offset[2] = 0.0f;
        } else {
            for (axis = 0; axis < 3; axis++)
                delta[axis] = particle->position[axis] - anchor[axis];
            particle->contact_local_offset[0] =
                liquid_vec3_dot(delta, person->horizontal);
            particle->contact_local_offset[1] =
                liquid_vec3_dot(delta, person->vertical);
            particle->contact_local_offset[2] =
                liquid_vec3_dot(delta, person->depth);
        }
        memcpy(particle->contact_anchor_world, anchor,
               sizeof(particle->contact_anchor_world));
        if (attachment_log_count < 12) {
            attachment_log_count++;
            log_line("liquid surface contact attached particle=%u person=%d anchor=%s end=%s segment_t=%.3f distance=%.4f",
                     particle->spawn_order, best_person + 1,
                     liquid_contact_anchor_names[best_anchor],
                     best_anchor_end >= 0 ?
                         liquid_contact_anchor_names[best_anchor_end] : "",
                     best_segment_t,
                     (float)sqrt((double)best_distance_squared));
        }
        return 1;
    }
    return 0;
}

static void liquid_follow_attached_contact(liquid_particle_t *particle)
{
    static LONG movement_log_count;
    int person_index;
    int anchor_index;
    liquid_contact_person_t person_snapshot;
    liquid_contact_person_t previous_person_snapshot;
    liquid_contact_person_t older_person_snapshot;
    const liquid_contact_person_t *person;
    const liquid_contact_person_t *previous_person;
    const liquid_contact_person_t *older_person;
    const liquid_contact_anchor_t *anchor;
    const liquid_contact_anchor_t *previous_anchor;
    const liquid_contact_anchor_t *older_anchor;
    DWORD current_sample_tick;
    DWORD previous_sample_tick;
    DWORD older_sample_tick;
    DWORD now;
    int axis;
    float old_position[3];
    float current_target[3];
    float previous_target[3];
    float older_target[3];
    int current_target_valid = 0;
    int previous_target_valid = 0;
    int older_target_valid = 0;
    if (!particle || !cfg.collision_follow_bodies ||
        particle->contact_person < 1 || particle->contact_person > 4 ||
        particle->contact_anchor < 1 ||
        particle->contact_anchor > LIQUID_CONTACT_ANCHOR_COUNT) return;
    person_index = particle->contact_person - 1;
    anchor_index = particle->contact_anchor - 1;
    EnterCriticalSection(&contact_lock);
    memcpy(&person_snapshot, &liquid_contact_people[person_index],
           sizeof(person_snapshot));
    memcpy(&previous_person_snapshot,
           &liquid_contact_people_previous[person_index],
           sizeof(previous_person_snapshot));
    memcpy(&older_person_snapshot,
           &liquid_contact_people_older[person_index],
           sizeof(older_person_snapshot));
    current_sample_tick = (DWORD)InterlockedCompareExchange(
        &liquid_contact_cache_tick, 0, 0);
    previous_sample_tick = (DWORD)InterlockedCompareExchange(
        &liquid_contact_previous_cache_tick, 0, 0);
    older_sample_tick = (DWORD)InterlockedCompareExchange(
        &liquid_contact_older_cache_tick, 0, 0);
    LeaveCriticalSection(&contact_lock);
    person = &person_snapshot;
    previous_person = &previous_person_snapshot;
    older_person = &older_person_snapshot;
    anchor = &person->anchors[anchor_index];
    previous_anchor = &previous_person->anchors[anchor_index];
    older_anchor = &older_person->anchors[anchor_index];
    if (!anchor->valid) return;
    memcpy(old_position, particle->position, sizeof(old_position));
    if (person->valid) {
        current_target_valid = liquid_contact_target_position(
            person, particle, current_target);
        if (previous_person->valid && previous_anchor->valid)
            previous_target_valid = liquid_contact_target_position(
                previous_person, particle, previous_target);
        if (older_person->valid && older_anchor->valid)
            older_target_valid = liquid_contact_target_position(
                older_person, particle, older_target);
        now = GetTickCount();
        if (previous_target_valid && current_sample_tick &&
            previous_sample_tick &&
            current_sample_tick != previous_sample_tick &&
            now - particle->contact_attached_tick >=
                current_sample_tick - previous_sample_tick) {
            DWORD sample_span = current_sample_tick - previous_sample_tick;
            float dx = current_target[0] - previous_target[0];
            float dy = current_target[1] - previous_target[1];
            float dz = current_target[2] - previous_target[2];
            float alpha = (float)(now - current_sample_tick) /
                          (float)sample_span;
            /* A room/pose teleport must snap immediately; interpolating a
               large discontinuity would drag the liquid through the scene. */
            if (dx * dx + dy * dy + dz * dz > 0.25f)
                alpha = 1.0f;
            if (alpha < 0.0f) alpha = 0.0f;
            if (alpha > 1.0f) alpha = 1.0f;
            if (older_target_valid && older_sample_tick &&
                previous_sample_tick != older_sample_tick &&
                alpha < 1.0f) {
                /* Cubic Hermite interpolation uses the older sample to
                   estimate the incoming body velocity.  The current
                   backward velocity supplies the outgoing tangent.  This
                   removes the stop/start look of linear sample segments
                   while keeping both sampled positions exact. */
                float alpha2 = alpha * alpha;
                float alpha3 = alpha2 * alpha;
                float h00 = 2.0f * alpha3 - 3.0f * alpha2 + 1.0f;
                float h10 = alpha3 - 2.0f * alpha2 + alpha;
                float h01 = -2.0f * alpha3 + 3.0f * alpha2;
                float h11 = alpha3 - alpha2;
                float older_span = (float)(previous_sample_tick -
                                           older_sample_tick);
                float current_span = (float)sample_span;
                float tangent_scale = older_span > 0.0f ?
                    current_span / older_span : 1.0f;
                if (tangent_scale < 0.5f) tangent_scale = 0.5f;
                if (tangent_scale > 2.0f) tangent_scale = 2.0f;
                for (axis = 0; axis < 3; axis++) {
                    float incoming = 0.5f *
                        (current_target[axis] - older_target[axis]) *
                        tangent_scale;
                    float outgoing = current_target[axis] -
                                     previous_target[axis];
                    particle->position[axis] =
                        h00 * previous_target[axis] + h10 * incoming +
                        h01 * current_target[axis] + h11 * outgoing;
                }
            } else {
                for (axis = 0; axis < 3; axis++)
                    particle->position[axis] = previous_target[axis] +
                        (current_target[axis] - previous_target[axis]) *
                        alpha;
            }
        } else if (current_target_valid) {
            memcpy(particle->position, current_target,
                   sizeof(particle->position));
        }
    } else {
        for (axis = 0; axis < 3; axis++)
            particle->position[axis] += anchor->world[axis] -
                particle->contact_anchor_world[axis];
    }
    memcpy(particle->previous, particle->position,
           sizeof(particle->previous));
    memcpy(particle->last_visible, particle->position,
           sizeof(particle->last_visible));
    memcpy(particle->contact_anchor_world, anchor->world,
           sizeof(particle->contact_anchor_world));
    if (cfg.enabled &&
        (fabsf(particle->position[0] - old_position[0]) > 0.0005f ||
         fabsf(particle->position[1] - old_position[1]) > 0.0005f ||
         fabsf(particle->position[2] - old_position[2]) > 0.0005f) &&
        InterlockedIncrement(&movement_log_count) <= 16) {
        log_line("liquid body-follow moved particle=%u person=%d anchor=%s delta=(%.5f,%.5f,%.5f)",
                 particle->spawn_order, particle->contact_person,
                 liquid_contact_anchor_names[anchor_index],
                 particle->position[0] - old_position[0],
                 particle->position[1] - old_position[1],
                 particle->position[2] - old_position[2]);
    }
}

static int liquid_world_to_view_point(const float world[3], float view[3])
{
    const float *m = captured_camera_inverse;
    float relative[3];
    if (!world || !view ||
        !InterlockedCompareExchange(&captured_camera_inverse_valid, 0, 0))
        return 0;
    relative[0] = world[0] - m[12];
    relative[1] = world[1] - m[13];
    relative[2] = world[2] - m[14];
    view[0] = relative[0] * m[0] + relative[1] * m[1] + relative[2] * m[2];
    view[1] = relative[0] * m[4] + relative[1] * m[5] + relative[2] * m[6];
    view[2] = relative[0] * m[8] + relative[1] * m[9] + relative[2] * m[10];
    return _finite(view[0]) && _finite(view[1]) && _finite(view[2]);
}

static liquid_emitter_t *liquid_model_emitter_for_person(int person_index)
{
    if (person_index < 1 ||
        person_index > LIQUID_MODEL_EMITTER_COUNT) return NULL;
    return &model_emitters[person_index - 1];
}

static liquid_emitter_t *liquid_model_emitter_for_emission(
    unsigned int emission_id)
{
    int index;
    if (!emission_id) return NULL;
    for (index = 0; index < LIQUID_MODEL_EMITTER_COUNT; index++) {
        if (model_emitters[index].emission_id == emission_id)
            return &model_emitters[index];
    }
    return NULL;
}

static liquid_emitter_t *liquid_latest_model_emitter(void)
{
    unsigned int emission_id = (unsigned int)InterlockedCompareExchange(
        &liquid_latest_model_emission, 0, 0);
    liquid_emitter_t *emitter =
        liquid_model_emitter_for_emission(emission_id);
    DWORD newest_tick = 0;
    int index;
    if (emitter) return emitter;
    for (index = 0; index < LIQUID_MODEL_EMITTER_COUNT; index++) {
        liquid_emitter_t *candidate = &model_emitters[index];
        DWORD candidate_tick = candidate->last_native_splash_tick ?
            candidate->last_native_splash_tick : candidate->start_tick;
        if (!candidate->emission_id || candidate_tick < newest_tick)
            continue;
        emitter = candidate;
        newest_tick = candidate_tick;
    }
    return emitter;
}

static int liquid_any_model_emitter_active(DWORD now)
{
    int index;
    for (index = 0; index < LIQUID_MODEL_EMITTER_COUNT; index++) {
        liquid_emitter_t *emitter = &model_emitters[index];
        if (emitter->active && now <= emitter->end_tick) return 1;
    }
    return 0;
}

static int liquid_resolve_physx_body_api(DWORD now)
{
    nc_physx_body_api_version_t version_fn;
    unsigned int version;
    if (liquid_physx_request_body_colliders &&
        liquid_physx_query_body_collider) {
        if (!InterlockedCompareExchange(
                &liquid_physx_retraction_callback_registered, 0, 0) &&
            liquid_physx_register_post_animation_callback &&
            liquid_physx_register_post_animation_callback(
                liquid_physx_post_animation_callback)) {
            InterlockedExchange(
                &liquid_physx_retraction_callback_registered, 1);
            log_line("PhysX post-animation retraction callback registered mode=\"PoseEditor playback and FreeMode; BlendControl only\"");
        }
        return 1;
    }
    if (liquid_physx_api_retry_tick &&
        now - liquid_physx_api_retry_tick < 1000u) return 0;
    liquid_physx_api_retry_tick = now;
    liquid_physx_module = GetModuleHandleA("NC-TK17-PhysX.dll");
    if (!liquid_physx_module) return 0;
    version_fn = (nc_physx_body_api_version_t)GetProcAddress(
        liquid_physx_module, "NCTK17PhysX_BodyColliderApiVersion");
    liquid_physx_request_body_colliders =
        (nc_physx_request_body_colliders_t)GetProcAddress(
            liquid_physx_module, "NCTK17PhysX_RequestBodyCollidersV1");
    liquid_physx_query_body_collider =
        (nc_physx_query_body_collider_t)GetProcAddress(
            liquid_physx_module, "NCTK17PhysX_QueryBodyColliderV1");
    liquid_physx_register_post_animation_callback =
        (nc_physx_register_post_animation_callback_t)GetProcAddress(
            liquid_physx_module,
            "NCTK17PhysX_RegisterPostAnimationCallback");
    liquid_physx_set_blend_weight =
        (nc_physx_set_blend_weight_t)GetProcAddress(
            liquid_physx_module,
            "NCTK17PhysX_SetBlendControlWeight");
    liquid_physx_set_blend_overlay =
        (nc_physx_set_blend_overlay_t)GetProcAddress(
            liquid_physx_module,
            "NCTK17PhysX_SetBlendControlOverlay");
    if (!version_fn || !liquid_physx_request_body_colliders ||
        !liquid_physx_query_body_collider) {
        liquid_physx_request_body_colliders = NULL;
        liquid_physx_query_body_collider = NULL;
        return 0;
    }
    version = version_fn();
    if ((version >> 16) != 1u) {
        liquid_physx_request_body_colliders = NULL;
        liquid_physx_query_body_collider = NULL;
        return 0;
    }
    if (InterlockedCompareExchange(&liquid_physx_api_logged, 1, 0) == 0) {
        log_line("PhysX body-collider API connected module=%p version=0x%08x mode=\"exact depth impact remains unchanged; PhysX classifies body ownership only\"",
                 liquid_physx_module, version);
    }
    if (liquid_physx_register_post_animation_callback &&
        liquid_physx_register_post_animation_callback(
            liquid_physx_post_animation_callback)) {
        InterlockedExchange(
            &liquid_physx_retraction_callback_registered, 1);
        log_line("PhysX post-animation retraction callback registered mode=\"PoseEditor playback and FreeMode; BlendControl only\"");
    } else {
        log_line("PhysX post-animation retraction callback unavailable; retaining NativeStainUpdate fallback");
    }
    return 1;
}

static void liquid_request_physx_body_colliders(DWORD now)
{
    if (liquid_resolve_physx_body_api(now)) {
        static DWORD last_status_tick;
        static unsigned int previous_status = ~0u;
        liquid_physx_request_body_colliders();
        if (!last_status_tick || now - last_status_tick >= 1000u) {
            nc_physx_body_api_version_t status_fn = NULL;
            last_status_tick = now;
            copy_proc_address(&status_fn, GetProcAddress(liquid_physx_module,
                "NCTK17PhysX_BodyColliderStatusV1"), sizeof(status_fn));
            if (status_fn) {
                unsigned int status = status_fn();
                if (status != previous_status) {
                    previous_status = status;
                    log_line("PhysX live collider status tick_active=%u camera_valid=%u enabled=%u ready_person_mask=0x%x",
                        status & 1u, (status >> 1) & 1u, (status >> 2) & 1u, (status >> 8) & 15u);
                }
            }
        }
    }
}

/* Return values mirror the optional PhysX API: 1 body hit, 0 confirmed no
   body collider, -1 unavailable/not ready. The tolerance is intentionally
   small: the depth buffer remains the exact visible impact and the collider
   is used only to decide ownership, never to move or snap that impact. */
static int liquid_query_physx_body_contact(
    const float world[3], nc_tk17_physx_body_hit_v1_t *hit)
{
    float tolerance;
    DWORD now = GetTickCount();
    if (!world || !hit || !liquid_resolve_physx_body_api(now)) return -1;
    liquid_physx_request_body_colliders();
    tolerance = cfg.particle_size * 2.5f;
    if (tolerance < 0.006f) tolerance = 0.006f;
    if (tolerance > 0.030f) tolerance = 0.030f;
    memset(hit, 0, sizeof(*hit));
    hit->size = sizeof(*hit);
    return liquid_physx_query_body_collider(
        world, tolerance, hit, sizeof(*hit));
}

static int liquid_runtime_work_active(DWORD now)
{
    if (liquid_any_model_emitter_active(now)) return 1;
    if (tool_emitter.active && now <= tool_emitter.end_tick) return 1;
    return liquid_has_active_particles();
}

/* AppPick confirms that a depth contact belongs to a TK17 model before the
   render-thread particle is allowed to follow a skeleton.  This keeps room
   and prop contacts fixed in world space instead of attaching them to a
   nearby person by accident. */
static int liquid_confirm_particle_model_contact(
    const liquid_native_contact_t *contact, const float hit_view[3])
{
    int index;
    float impact_view[3], delta[3];
    float tolerance = fminf(0.006f, fmaxf(0.002f, cfg.particle_size * 0.5f));
    if (!contact || !hit_view || !contact->emission_id || !contact->particle_id ||
        !liquid_world_to_view_point(contact->impact_world, impact_view)) return 0;
    /* PickRay is unbounded, including its camera fallback. A model somewhere
       along that ray does not identify the surface where this droplet landed.
       Compare in view space against the queued world impact using the same
       camera, without adding retry offsets or a skeleton proximity radius. */
    for (index = 0; index < 3; index++) {
        delta[index] = hit_view[index] - impact_view[index];
        if (!_finite(delta[index])) return 0;
    }
    if (liquid_vec3_dot(delta, delta) > tolerance * tolerance) return 0;
    for (index = 0; index < cfg.particle_limit; index++) {
        liquid_particle_t *particle = &liquid_particles[index];
        if (!particle->active || !particle->collided ||
            particle->emission_id != contact->emission_id ||
            particle->spawn_order != contact->particle_id)
            continue;
        if (particle->contact_physx_person < 0) return 0;
        InterlockedExchange(&particle->model_contact_verified, 1);
        InterlockedExchange(&particle->model_contact_confirmed, 1);
        return 1;
    }
    return 0;
}

/* Queue confirmed custom-liquid contacts for TK17's own stain attachment
   routine.  The depth collision pass runs on the render thread, while
   AppPick runs on the game thread, so each slot uses a tiny publish/consume
   state instead of sharing partially written vectors. */
static void liquid_queue_native_model_contact(
    const liquid_particle_t *particle)
{
    DWORD now;
    float direction[3];
    int slot_index;
    int emitter_index;
    if (!particle || particle->secondary || particle->contact_physx_person < 0 ||
        !cfg.collision_spawn_model_stains ||
        particle->source_kind != 1 || !particle->emission_id)
        return;
    memcpy(direction, particle->impact_velocity, sizeof(direction));
    if (!liquid_vec3_normalize(direction)) return;
    now = GetTickCount();
    {
        liquid_emitter_t *emitter =
            liquid_model_emitter_for_emission(particle->emission_id);
        if (!emitter) return;
        emitter_index = emitter->person_index - 1;
        if (emitter_index < 0 ||
            emitter_index >= LIQUID_MODEL_EMITTER_COUNT) return;
    }
    /* Sample current impacts at the configured stain rate. Excess contacts
       are discarded here instead of being queued, so a completed liquid
       stream can never produce delayed decals afterward. */
    {
        DWORD previous_tick = (DWORD)InterlockedCompareExchange(
            &liquid_native_last_contact_queue_tick[emitter_index], 0, 0);
        DWORD interval_ms = (DWORD)(1000.0f /
            cfg.collision_model_stain_rate);
        if (interval_ms < 1u) interval_ms = 1u;
        if (previous_tick && now - previous_tick < interval_ms) return;
    }
    for (slot_index = 0; slot_index < LIQUID_NATIVE_CONTACT_SLOTS;
         slot_index++) {
        liquid_native_contact_t *slot =
            &liquid_native_contacts[slot_index];
        LONG state = InterlockedCompareExchange(&slot->state, 0, 0);
        if (state == 1 &&
            now - slot->tick > LIQUID_NATIVE_CONTACT_MAX_AGE_MS) {
            InterlockedCompareExchange(&slot->state, 0, 1);
            state = InterlockedCompareExchange(&slot->state, 0, 0);
        }
        if (state != 0 ||
            InterlockedCompareExchange(&slot->state, -1, 0) != 0)
            continue;
        slot->tick = now;
        slot->next_retry_tick = now;
        slot->emission_id = particle->emission_id;
        slot->particle_id = particle->spawn_order;
        slot->retry_count = 0;
        slot->impact_world[0] = particle->position[0];
        slot->impact_world[1] = particle->position[1];
        slot->impact_world[2] = particle->position[2];
        slot->direction_world[0] = direction[0];
        slot->direction_world[1] = direction[1];
        slot->direction_world[2] = direction[2];
        /* Start just in front of the depth-confirmed contact.  AppPick then
           resolves the precise skinned triangle and lets TK17 attach its
           native sperm material to that moving model or garment. */
        slot->origin_world[0] = particle->position[0] - direction[0] * 0.08f;
        slot->origin_world[1] = particle->position[1] - direction[1] * 0.08f;
        slot->origin_world[2] = particle->position[2] - direction[2] * 0.08f;
        InterlockedExchange(
            &liquid_native_last_contact_queue_tick[emitter_index],
            (LONG)now);
        InterlockedExchange(&slot->state, 1);
        if (InterlockedIncrement(&liquid_native_contact_log_count) <= 20)
            log_line("liquid native stain contact queued particle=%u emission=%u origin=(%.4f,%.4f,%.4f) direction=(%.4f,%.4f,%.4f)",
                     slot->particle_id, slot->emission_id,
                     slot->origin_world[0], slot->origin_world[1],
                     slot->origin_world[2], slot->direction_world[0],
                     slot->direction_world[1], slot->direction_world[2]);
        return;
    }
}

static int liquid_pop_native_model_contact(
    unsigned int emission_id, DWORD now, liquid_native_contact_t *output)
{
    int slot_index;
    if (!output || !emission_id) return 0;
    for (slot_index = 0; slot_index < LIQUID_NATIVE_CONTACT_SLOTS;
         slot_index++) {
        liquid_native_contact_t *slot =
            &liquid_native_contacts[slot_index];
        if (InterlockedCompareExchange(&slot->state, 0, 0) != 1)
            continue;
        if (now - slot->tick > LIQUID_NATIVE_CONTACT_MAX_AGE_MS) {
            InterlockedCompareExchange(&slot->state, 0, 1);
            continue;
        }
        if (slot->emission_id != emission_id) continue;
        if ((LONG)(now - slot->next_retry_tick) < 0) continue;
        if (InterlockedCompareExchange(&slot->state, -1, 1) != 1)
            continue;
        memcpy(output, slot, sizeof(*output));
        output->state = 0;
        InterlockedExchange(&slot->state, 0);
        return 1;
    }
    return 0;
}

static int liquid_native_model_contact_pending(
    unsigned int emission_id, DWORD now)
{
    int slot_index;
    if (!emission_id) return 0;
    for (slot_index = 0; slot_index < LIQUID_NATIVE_CONTACT_SLOTS;
         slot_index++) {
        liquid_native_contact_t *slot =
            &liquid_native_contacts[slot_index];
        if (InterlockedCompareExchange(&slot->state, 0, 0) != 1)
            continue;
        if (now - slot->tick > LIQUID_NATIVE_CONTACT_MAX_AGE_MS) {
            InterlockedCompareExchange(&slot->state, 0, 1);
            continue;
        }
        if (slot->emission_id != emission_id) continue;
        if ((LONG)(now - slot->next_retry_tick) < 0) continue;
        return 1;
    }
    return 0;
}

static int liquid_native_contact_particle_alive(
    const liquid_native_contact_t *contact)
{
    int index;
    if (!contact) return 0;
    for (index = 0; index < cfg.particle_limit; index++) {
        liquid_particle_t *particle = &liquid_particles[index];
        if (particle->active && particle->collided &&
            particle->emission_id == contact->emission_id &&
            particle->spawn_order == contact->particle_id)
            return 1;
    }
    return 0;
}

static int liquid_retry_native_model_contact(
    const liquid_native_contact_t *contact, DWORD now)
{
    liquid_emitter_t *emitter;
    int slot_index;
    if (!contact ||
        contact->retry_count >= LIQUID_NATIVE_CONTACT_MAX_RETRIES ||
        now - contact->tick > LIQUID_NATIVE_CONTACT_RETRY_WINDOW_MS ||
        !liquid_native_contact_particle_alive(contact)) return 0;
    emitter = liquid_model_emitter_for_emission(contact->emission_id);
    if (!emitter || now > emitter->end_tick) return 0;
    for (slot_index = 0; slot_index < LIQUID_NATIVE_CONTACT_SLOTS;
         slot_index++) {
        liquid_native_contact_t *slot =
            &liquid_native_contacts[slot_index];
        if (InterlockedCompareExchange(&slot->state, -1, 0) != 0)
            continue;
        slot->tick = contact->tick;
        slot->next_retry_tick =
            now + LIQUID_NATIVE_CONTACT_RETRY_INTERVAL_MS;
        slot->emission_id = contact->emission_id;
        slot->particle_id = contact->particle_id;
        slot->retry_count = contact->retry_count + 1;
        memcpy(slot->impact_world, contact->impact_world,
               sizeof(slot->impact_world));
        memcpy(slot->origin_world, contact->origin_world,
               sizeof(slot->origin_world));
        memcpy(slot->direction_world, contact->direction_world,
               sizeof(slot->direction_world));
        InterlockedExchange(&slot->state, 1);
        return 1;
    }
    return 0;
}

static void liquid_native_contact_retry_offset(
    const liquid_native_contact_t *contact, float offset_world[3])
{
    const float *basis = captured_camera_inverse;
    float scale = 0.010f;
    int axis;
    memset(offset_world, 0, sizeof(float) * 3);
    if (!contact || !contact->retry_count ||
        !InterlockedCompareExchange(&captured_camera_inverse_valid, 0, 0))
        return;
    if (contact->retry_count == 1) {
        for (axis = 0; axis < 3; axis++)
            offset_world[axis] = basis[axis] * scale;
    } else if (contact->retry_count == 2) {
        for (axis = 0; axis < 3; axis++)
            offset_world[axis] = -basis[axis] * scale;
    } else {
        for (axis = 0; axis < 3; axis++)
            offset_world[axis] = basis[4 + axis] * scale;
    }
}

static int liquid_native_contact_view_ray(
    const liquid_native_contact_t *contact, float origin_view[3],
    float direction_view[3])
{
    float origin_world[3];
    float target_world[3];
    float target_view[3];
    float retry_offset[3];
    int axis;
    if (!contact || !origin_view || !direction_view) return 0;
    liquid_native_contact_retry_offset(contact, retry_offset);
    for (axis = 0; axis < 3; axis++) {
        origin_world[axis] = contact->origin_world[axis] +
            retry_offset[axis] - contact->direction_world[axis] *
            (0.02f * (float)contact->retry_count);
        target_world[axis] = contact->impact_world[axis] +
            retry_offset[axis] + contact->direction_world[axis] * 0.02f;
    }
    if (!liquid_world_to_view_point(origin_world, origin_view) ||
        !liquid_world_to_view_point(target_world, target_view))
        return 0;
    for (axis = 0; axis < 3; axis++)
        direction_view[axis] = target_view[axis] - origin_view[axis];
    return liquid_vec3_normalize(direction_view);
}

static int liquid_native_contact_camera_ray(
    const liquid_native_contact_t *contact, float origin_view[3],
    float direction_view[3])
{
    float impact_world[3];
    float impact_view[3];
    float retry_offset[3];
    int axis;
    if (!contact || !origin_view || !direction_view) return 0;
    liquid_native_contact_retry_offset(contact, retry_offset);
    for (axis = 0; axis < 3; axis++)
        impact_world[axis] = contact->impact_world[axis] +
            retry_offset[axis];
    if (!liquid_world_to_view_point(impact_world, impact_view))
        return 0;
    origin_view[0] = 0.0f;
    origin_view[1] = 0.0f;
    origin_view[2] = 0.0f;
    direction_view[0] = impact_view[0];
    direction_view[1] = impact_view[1];
    direction_view[2] = impact_view[2];
    return liquid_vec3_normalize(direction_view);
}

static int liquid_native_pick_result_data(
    void *results, void **data_out, int *count_out)
{
    void *data = NULL;
    int count = 0;
    if (results && ptr_readable(results, sizeof(void*))) {
        data = *(void**)results;
        if (data && ptr_readable((BYTE*)data - sizeof(int), sizeof(int)))
            count = *((const int*)data - 1);
    }
    if (data_out) *data_out = data;
    if (count_out) *count_out = count;
    return data && count > 0;
}

/* TK17 keeps the PickRay origin and direction in its caller's stack frame
   and reuses those same vectors after PickRay returns to construct the
   stain's skinned local plane. Passing temporary replacement vectors only
   to PickRay therefore combines a custom hit with TK17's old ray and can
   shear a normal 0..1 decal quad into long strips. Keep the caller-owned
   vectors synchronized with the substituted contact ray. */
static int liquid_native_replace_caller_ray(
    const float *native_origin, const float *native_direction,
    const float replacement_origin[3],
    const float replacement_direction[3])
{
    if (!native_origin || !native_direction || !replacement_origin ||
        !replacement_direction ||
        !ptr_writable((void*)native_origin, sizeof(float) * 3) ||
        !ptr_writable((void*)native_direction, sizeof(float) * 3))
        return 0;
    memcpy((void*)native_origin, replacement_origin, sizeof(float) * 3);
    memcpy((void*)native_direction, replacement_direction,
           sizeof(float) * 3);
    return 1;
}

/* Convert TK17 world-up (Y+) into the current camera/view coordinate system.
   The native stain routine works entirely in view space after obtaining the
   source ModelViewMatrix, so its replacement in-plane basis must use the
   same coordinate system. */
static int liquid_native_world_up_view(float up_view[3])
{
    const float *m = captured_camera_inverse;
    if (!up_view ||
        !InterlockedCompareExchange(&captured_camera_inverse_valid, 0, 0))
        return 0;
    up_view[0] = m[1];
    up_view[1] = m[5];
    up_view[2] = m[9];
    return liquid_vec3_normalize(up_view);
}

/* Synchronize TK17's source ModelViewMatrix with the custom impact ray.
   The engine derives its ray from matrix row Z and later reuses all three
   basis rows to clip the native square onto skinned geometry. Build a rigid,
   gravity-upright basis: ray direction controls projector depth while TK17's
   world Y axis controls texture roll. This avoids the emitter-dependent roll
   and shearing produced by borrowing the original sperm-ray X axis. */
static int liquid_native_build_projector_transform(
    float projector_matrix[16], const float replacement_origin[3],
    const float replacement_direction[3])
{
    float forward[3];
    float right[3];
    float up[3];
    float projection;
    int axis;
    if (!projector_matrix || !replacement_origin || !replacement_direction)
        return 0;
    forward[0] = -replacement_direction[0];
    forward[1] = -replacement_direction[1];
    forward[2] = -replacement_direction[2];
    if (!liquid_vec3_normalize(forward)) return 0;
    if (!liquid_native_world_up_view(up)) {
        up[0] = 0.0f;
        up[1] = 1.0f;
        up[2] = 0.0f;
    }
    projection = liquid_vec3_dot(up, forward);
    for (axis = 0; axis < 3; axis++)
        up[axis] -= forward[axis] * projection;
    if (!liquid_vec3_normalize(up)) {
        /* A vertical impact has no unique gravity-up direction on its
           projector plane. Keep the native source's X direction only for
           this degenerate case, then make it orthogonal to the new ray. */
        right[0] = projector_matrix[0];
        right[1] = projector_matrix[1];
        right[2] = projector_matrix[2];
        projection = liquid_vec3_dot(right, forward);
        for (axis = 0; axis < 3; axis++)
            right[axis] -= forward[axis] * projection;
        if (!liquid_vec3_normalize(right)) return 0;
        liquid_vec3_cross(forward, right, up);
        if (!liquid_vec3_normalize(up)) return 0;
    } else {
        liquid_vec3_cross(up, forward, right);
        if (!liquid_vec3_normalize(right)) return 0;
        liquid_vec3_cross(forward, right, up);
        if (!liquid_vec3_normalize(up)) return 0;
    }
    /* TK17's sperm texture convention is rotated 180 degrees relative to
       the gravity-upright projector basis. Rotate the complete projector
       plane without mirroring it. The basis remains rigid and orthogonal,
       so this corrects drip direction without restoring the old shear. */
    for (axis = 0; axis < 3; axis++) {
        right[axis] = -right[axis];
        up[axis] = -up[axis];
    }
    projector_matrix[0] = right[0];
    projector_matrix[1] = right[1];
    projector_matrix[2] = right[2];
    projector_matrix[4] = up[0];
    projector_matrix[5] = up[1];
    projector_matrix[6] = up[2];
    projector_matrix[8] = forward[0];
    projector_matrix[9] = forward[1];
    projector_matrix[10] = forward[2];
    projector_matrix[12] = replacement_origin[0];
    projector_matrix[13] = replacement_origin[1];
    projector_matrix[14] = replacement_origin[2];
    projector_matrix[3] = projector_matrix[7] = projector_matrix[11] = 0.0f;
    projector_matrix[15] = 1.0f;
    return 1;
}

static int liquid_native_replace_projector_transform(
    void *native_frame, const float replacement_origin[3],
    const float replacement_direction[3])
{
    float *matrix;
    if (!native_frame) return 0;
    matrix = (float*)((BYTE*)native_frame - LIQUID_NATIVE_STAIN_MATRIX_EBP_OFFSET);
    if (!ptr_writable(matrix, sizeof(float) * 16)) return 0;
    return liquid_native_build_projector_transform(
        matrix, replacement_origin, replacement_direction);
}

/* AppPick's 0x58-byte PickResult stores view-space hit at +0x14 and the
   normalized mesh normal at +0x20 (APP+0x21e18 / 0x2207c / 0x221b7).
   This is the actual picked mesh, not an approximate PhysX body capsule.
   Build a unit-scale view-to-projector matrix. TK17 separately subtracts
   the exact hit and applies descriptor size in EXE+0x1f2537..0x1f2613. */
static int liquid_native_surface_projector(const void *data, float inverse[16])
{
    float normal[3], inward[3], rotation[16] = {0};
    const float zero[3] = {0, 0, 0};
    float length2;
    int axis, row, column;
    if (!data || !ptr_readable(data, 0x2c)) return 0;
    memcpy(normal, (const BYTE*)data + 0x20, sizeof(normal));
    length2 = liquid_vec3_dot(normal, normal);
    if (!_finite(length2) || length2 < 0.25f || length2 > 2.25f ||
        !liquid_vec3_normalize(normal)) return 0;
    for (axis = 0; axis < 3; axis++) inward[axis] = -normal[axis];
    /* Stable world X fallback at horizontal surfaces. Do not inherit roll
       from the incoming ray, which may change from one droplet to the next. */
    if (InterlockedCompareExchange(&captured_camera_inverse_valid, 0, 0)) {
        rotation[0] = captured_camera_inverse[0];
        rotation[1] = captured_camera_inverse[4];
        rotation[2] = captured_camera_inverse[8];
    } else rotation[0] = 1.0f;
    if (!liquid_native_build_projector_transform(rotation, zero, inward)) return 0;
    memset(inverse, 0, sizeof(float) * 16);
    for (row = 0; row < 3; row++)
        for (column = 0; column < 3; column++)
            inverse[row * 4 + column] = rotation[column * 4 + row];
    /* Gameplay confirmed the surface-aligned texture was upside down.
       Reflect only projector Y about the hit center; native UV centering
       then makes this V -> 1-V. Keep horizontal direction, depth and scale.
       Apply here so the contact ray and ordinary native picks are untouched. */
    for (row = 0; row < 3; row++)
        inverse[row * 4 + 1] = -inverse[row * 4 + 1];
    inverse[15] = 1.0f;
    return 1;
}

static void liquid_native_stage_surface_projector(void *frame, const void *data)
{
    liquid_native_freeze_capture_t *capture = liquid_native_freeze_capture;
    if (!capture || !frame || !native_stain_projector_hook_installed) return;
    capture->projector_frame = NULL;
    if (liquid_native_surface_projector(data, capture->projector_inverse))
        capture->projector_frame = frame;
    else {
        static int logged;
        if (logged++ < 12)
            log_line("liquid surface decal alignment skipped reason=invalid-mesh-normal");
    }
}

/* PickRay's source matrix is overwritten by the native inverse-matrix
   getter at EXE+0x1f2513 (or identity at +0x1f2531). Apply AFTER both paths,
   immediately before TK17 builds the centered, sized projector. */
static void __attribute__((used, noinline)) __cdecl
liquid_native_apply_surface_projector(void *frame)
{
    liquid_native_freeze_capture_t *capture = liquid_native_freeze_capture;
    float *matrix;
    if (!capture || capture->projector_frame != frame) return;
    capture->projector_frame = NULL;
    matrix = (float*)((BYTE*)frame - LIQUID_NATIVE_STAIN_MATRIX_EBP_OFFSET);
    if (!ptr_writable(matrix, sizeof(capture->projector_inverse))) return;
    memcpy(matrix, capture->projector_inverse, sizeof(capture->projector_inverse));
    {
        static int logged;
        if (logged++ < 24)
            log_line("liquid surface decal aligned frame=%p mode=mesh-normal final-projector=1", frame);
    }
}

/* Mid-function hook: preserve integer, flags, x87 and SSE state, including
   any live x87 stack values. The trampoline replays the stolen FLD exactly. */
static void __attribute__((naked)) hook_NativeStainProjector(void)
{
    __asm__ __volatile__(
        "pushfl\n\tpushal\n\t"
        "movl %esp, %ebx\n\tsubl $528, %esp\n\tandl $-16, %esp\n\t"
        "fxsave (%esp)\n\tfninit\n\t"
        "subl $16, %esp\n\tmovl %ebp, (%esp)\n\t"
        "call _liquid_native_apply_surface_projector\n\t"
        "addl $16, %esp\n\tfxrstor (%esp)\n\tmovl %ebx, %esp\n\t"
        "popal\n\tpopfl\n\tjmp *_tramp_NativeStainProjector\n\t");
}

/* Verified against SYS 010278: GetModelViewRotationPivot (RVA 8AEB0)
   reads ParentTransform.ModelViewMatrix. That is render traversal state,
   not a world pose; PoseEditor can leave it in a different pass's space.
   Evaluate the same parent chain from identity using SBaseTransform's
   native local-matrix evaluator instead. No render matrices are written,
   no animation/controller pass is repeated, and visibility is irrelevant. */
typedef void *(THISCALL *liquid_node_get_t)(void *, unsigned int);
typedef void (THISCALL *liquid_vector_get_t)(void *, unsigned int, float *);
typedef const float *(THISCALL *liquid_matrix_eval_t)(
    void *, const float *, float *);

static int liquid_node_is_nil(void *object)
{
    /* TK17 uses tagged nil objects; ParentTransform's own getter checks
       this bit before dereferencing a parent (SYS+ED9B0). */
    return !object || ((uintptr_t)object & 8u) != 0;
}

static BYTE *liquid_node_interface(void *object, unsigned int offset)
{
    BYTE *meta, *table;
    if (liquid_node_is_nil(object) ||
        !ptr_readable((BYTE*)object - 0x18, sizeof(void*))) return NULL;
    meta = *(BYTE**)((BYTE*)object - 0x18);
    if (!ptr_readable(meta, offset + sizeof(void*))) return NULL;
    table = *(BYTE**)(meta + offset);
    if (!table || !ptr_readable(table - 0x1c, sizeof(unsigned int)) ||
        !*(unsigned int*)(table - 0x1c)) return NULL;
    return table;
}

static void *liquid_node_method(void *object, unsigned int interface_offset,
                                unsigned int method_offset)
{
    BYTE *table = liquid_node_interface(object, interface_offset);
    void *method;
    if (!table || !ptr_readable(table + method_offset, sizeof(void*)))
        return NULL;
    method = *(void**)(table + method_offset);
    return ptr_executable(method) ? method : NULL;
}

static int liquid_node_parent(void *object, void **parent)
{
    liquid_node_get_t getter = (liquid_node_get_t)liquid_node_method(
        object, 0x10c, 0xc0);
    if (!getter) return 0;
    *parent = getter(object, 0x03fff043u); /* TSNode.ParentTransform */
    return 1;
}

static void *liquid_node_source(void *object)
{
    liquid_node_get_t getter = (liquid_node_get_t)liquid_node_method(
        object, 0x10c, 0x40);
    return getter ? getter(object, 0x01fff043u) : NULL; /* TSNode.SNode */
}

static int liquid_node_vector(void *source, unsigned int iface,
                              unsigned int slot, unsigned int property,
                              float value[3])
{
    liquid_vector_get_t getter = (liquid_vector_get_t)liquid_node_method(
        source, iface, slot);
    if (!getter) return 0;
    getter(source, property, value);
    return _finite(value[0]) && _finite(value[1]) && _finite(value[2]);
}

static int liquid_matrix_sane(const float matrix[16])
{
    int i;
    for (i = 0; i < 16; ++i)
        if (!_finite(matrix[i]) || liquid_absf(matrix[i]) > 1000000.0f)
            return 0;
    return liquid_absf(matrix[3]) < 0.0001f &&
           liquid_absf(matrix[7]) < 0.0001f &&
           liquid_absf(matrix[11]) < 0.0001f &&
           liquid_absf(matrix[15] - 1.0f) < 0.0001f;
}

/* PhysX writes SJoint.Rotation at +6C directly. Unlike TK17's property
   setter, that does not invalidate SBaseTransform's local matrix at +18
   (cache state +5C). The generic evaluator at SYS+DAC70 will therefore
   return identity or a previous pose even when the rotation has changed.
   For the verified SJoint implementation, rebuild a private value copy.
   SYS+DCB40 uses only fields in that copy and pure matrix routines; its
   writes are limited to the copy's local matrix and cache state. Never
   invalidate/overwrite the game's matrices from a renderer callback. */
static int liquid_evaluate_source_matrix(void *source,
                                         liquid_matrix_eval_t evaluate,
                                         const float parent[16],
                                         float output[16])
{
    BYTE joint_copy[0x110] __attribute__((aligned(16)));
    void **local_vtable;
    const float *result;
    float scratch[16];
    void *evaluation_source = source;
    if (ptr_readable((BYTE*)source + 8, sizeof(void*))) {
        local_vtable = *(void***)((BYTE*)source + 8);
        if (ptr_readable(local_vtable, 2 * sizeof(void*)) &&
            liquid_module_rva(local_vtable[1],
                              "ThriXXX010278-SYS.dll", 0x000dcb40u)) {
            if (!ptr_readable(source, sizeof(joint_copy))) return 0;
            memcpy(joint_copy, source, sizeof(joint_copy));
            *(unsigned int*)(joint_copy + 0x5c) = 0;
            evaluation_source = joint_copy;
        }
    }
    result = evaluate(evaluation_source, parent, scratch);
    if ((result != parent && result != scratch) ||
        !liquid_matrix_sane(result)) return 0;
    if (cfg.enabled && evaluation_source == joint_copy) {
        static DWORD last_cache_log;
        DWORD now = GetTickCount();
        float max_delta = 0.0f;
        const float *cached = (const float*)((BYTE*)source + 0x18);
        const float *rebuilt = (const float*)(joint_copy + 0x18);
        int i;
        for (i = 0; i < 16; ++i) {
            float delta = liquid_absf(cached[i] - rebuilt[i]);
            if (delta > max_delta) max_delta = delta;
        }
        if (max_delta > 0.0001f &&
            (!last_cache_log || now - last_cache_log >= 1000u)) {
            const float *rotation = (const float*)(joint_copy + 0x6c);
            last_cache_log = now;
            log_line("liquid source local cache bypassed source=%p state=%u matrix_delta=%.6f rotation=(%.4f,%.4f,%.4f) mode=private-SJoint-rebuild",
                     source, *(unsigned int*)((BYTE*)source + 0x5c),
                     max_delta, rotation[0], rotation[1], rotation[2]);
        }
    }
    memcpy(output, result, sizeof(float) * 16);
    return 1;
}

static int liquid_object_world_pivot_frame(
    const char *path, float world[3], float frame[16])
{
    void *object = liquid_find_object(path), *node, *source;
    void *chain[128];
    int count = 0, i, axis;
    float matrix[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    float pivot[3] = { 0,0,0 };
    if (!object || !liquid_node_parent(object, &node)) return 0;
    while (!liquid_node_is_nil(node)) {
        if (count == 128) return 0;
        for (i = 0; i < count; ++i)
            if (chain[i] == node) return 0;
        chain[count++] = node;
        if (!liquid_node_parent(node, &node)) return 0;
    }
    /* Use the exact evaluator called by DrawTree at SYS+9C0AD. Joint local
       caches are rebuilt privately even after raw PhysX writes; the result may be
       the input itself for identity nodes, so always honor the return. */
    for (i = count - 1; i >= 0; --i) {
        void **vtable;
        liquid_matrix_eval_t evaluate;
        source = liquid_node_source(chain[i]);
        if (!liquid_node_interface(source, 0x120) ||
            !ptr_readable(source, sizeof(void*))) return 0;
        vtable = *(void***)source;
        if (!ptr_readable(vtable, 2 * sizeof(void*))) return 0;
        evaluate = (liquid_matrix_eval_t)vtable[1];
        if (!liquid_module_rva((void*)evaluate,
                              "ThriXXX010278-SYS.dll", 0x000dac70u))
            return 0;
        if (!liquid_evaluate_source_matrix(source, evaluate, matrix, matrix))
            return 0;
    }
    /* Match GetModelViewRotationPivot's local pivot convention, including
       transform pivot offsets rather than assuming a joint's origin. */
    if (liquid_node_interface(object, 0x12c)) {
        source = liquid_node_source(object);
        if (!liquid_node_vector(source, 0x128, 0x40, 0x01fff04au, pivot))
            return 0;
        if (liquid_node_interface(object, 0x134)) {
            float rotation_pivot[3], pivot_offset[3];
            if (!liquid_node_vector(source, 0x130, 0x180,
                                    0x06fff04cu, rotation_pivot) ||
                !liquid_node_vector(source, 0x130, 0x140,
                                    0x05fff04cu, pivot_offset)) return 0;
            for (axis = 0; axis < 3; ++axis)
                pivot[axis] += rotation_pivot[axis] + pivot_offset[axis];
        }
    }
    for (axis = 0; axis < 3; ++axis) {
        world[axis] = pivot[0] * matrix[axis] +
                      pivot[1] * matrix[4 + axis] +
                      pivot[2] * matrix[8 + axis] + matrix[12 + axis];
        if (!_finite(world[axis]) || liquid_absf(world[axis]) > 1000000.0f)
            return 0;
    }
    if (frame) {
        void **vtable;
        liquid_matrix_eval_t evaluate;
        source = liquid_node_source(object);
        if (!liquid_node_interface(source, 0x120) ||
            !ptr_readable(source, sizeof(void*))) return 0;
        vtable = *(void***)source;
        if (!ptr_readable(vtable, 2 * sizeof(void*))) return 0;
        evaluate = (liquid_matrix_eval_t)vtable[1];
        if (!liquid_module_rva((void*)evaluate,
                              "ThriXXX010278-SYS.dll", 0x000dac70u) ||
            !liquid_evaluate_source_matrix(source, evaluate, matrix, frame))
            return 0;
    }
    return 1;
}

static int liquid_object_world_pivot(const char *path, float world[3])
{
    return liquid_object_world_pivot_frame(path, world, NULL);
}

/* UI axes: Z follows the original emission aim, Y follows tip-local up,
   and X is right. Orthonormalize so bodymod scale/shear cannot multiply the
   one-centimetre range. Never change aim or write any bone transforms. */
static int liquid_offset_emitter_position(
    const float frame[16], const float forward[3], float position[3])
{
    float up[3] = {frame[4], frame[5], frame[6]}, right[3];
    int axis;
    liquid_vec3_cross(up, forward, right);
    if (!liquid_vec3_normalize(right)) {
        /* Alternate tip-local reference if its Y axis is parallel to aim. */
        up[0] = frame[8]; up[1] = frame[9]; up[2] = frame[10];
        liquid_vec3_cross(up, forward, right);
        if (!liquid_vec3_normalize(right)) return 0;
    }
    liquid_vec3_cross(forward, right, up);
    if (!liquid_vec3_normalize(up)) return 0;
    for (axis = 0; axis < 3; axis++)
        position[axis] += LIQUID_EMITTER_OFFSET_METRES *
            (cfg.emitter_position[0] * right[axis] +
             cfg.emitter_position[1] * up[axis] +
             cfg.emitter_position[2] * forward[axis]);
    return 1;
}

static int liquid_emitter_transform(const liquid_emitter_t *emitter,
                                    float position[3], float direction[3])
{
    char source_name[128];
    char target_name[128];
    float source_view[3];
    float source_world[3];
    float target_world[3];
    float source_frame[16];
    int has_offset = cfg.emitter_position[0] != 0.0f ||
                     cfg.emitter_position[1] != 0.0f ||
                     cfg.emitter_position[2] != 0.0f;
    int person;
    if (!emitter || !position || !direction) return 0;
    if (emitter->source_kind == 2) {
        static LONG tool_view_fallback_logged;
        void *tool_group = (void*)InterlockedCompareExchangePointer(
            (PVOID volatile *)&tool_spermray_group_object, NULL, NULL);
        void *tool_root = (void*)InterlockedCompareExchangePointer(
            (PVOID volatile *)&tool_spermray_root_object, NULL, NULL);
        lstrcpynA(source_name, "Tool01Spermray:spermray01_group",
                  sizeof(source_name));
        source_name[sizeof(source_name) - 1] = 0;
        if (!liquid_object_view_pivot(source_name, source_view) &&
            !liquid_live_object_view_pivot(
                tool_group, "Tool01Spermray:spermray01_group", source_view) &&
            !liquid_live_object_view_pivot(
                tool_root, "Tool01Spermray", source_view)) {
            /* The POV tool is a temporary viewport object rather than a
               persistent scene node.  If TK17 has already released its live
               transform, emit from the tool's stable bottom-centre viewport
               position instead of dropping the entire effect. */
            source_view[0] = 0.0f;
            source_view[1] = -0.72f;
            source_view[2] = -1.25f;
            if (!InterlockedExchange(&tool_view_fallback_logged, 1))
                log_line("liquid tool transform fallback active view=(%.3f,%.3f,%.3f)",
                         source_view[0], source_view[1], source_view[2]);
        }
        if (!liquid_view_to_world_point(source_view, source_world)) return 0;
        position[0] = source_world[0];
        position[1] = source_world[1];
        position[2] = source_world[2];
        /* Visible camera space points down -Z. */
        direction[0] = -captured_camera_inverse[8];
        direction[1] = -captured_camera_inverse[9];
        direction[2] = -captured_camera_inverse[10];
        return liquid_vec3_normalize(direction);
    }

    person = emitter->person_index;
    if (person < 1 || person > 4) person = 1;
    _snprintf(source_name, sizeof(source_name),
              "Person%02dAnim:Model01:penis_jointEnd", person);
    _snprintf(target_name, sizeof(target_name),
              "Person%02dAnim:Model01:penis_joint03", person);
    source_name[sizeof(source_name) - 1] = 0;
    target_name[sizeof(target_name) - 1] = 0;
    if (!liquid_object_world_pivot_frame(source_name, source_world,
                                        has_offset ? source_frame : NULL) ||
        !liquid_object_world_pivot(target_name, target_world)) return 0;
    position[0] = source_world[0];
    position[1] = source_world[1];
    position[2] = source_world[2];
    /* Aim outward from penis_joint03 through the exact tip joint. */
    direction[0] = source_world[0] - target_world[0];
    direction[1] = source_world[1] - target_world[1];
    direction[2] = source_world[2] - target_world[2];
    /* A missing/collapsed model chain must never become a camera emitter. */
    if (!liquid_vec3_normalize(direction)) return 0;
    return !has_offset ||
        liquid_offset_emitter_position(source_frame, direction, position);
}

static const GUID liquid_clsid_filter_graph = {
    0xe436ebb3, 0x524f, 0x11ce,
    {0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}
};
static const GUID liquid_iid_graph_builder = {
    0x56a868a9, 0x0ad4, 0x11ce,
    {0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}
};
static const GUID liquid_iid_media_control = {
    0x56a868b1, 0x0ad4, 0x11ce,
    {0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}
};
static const GUID liquid_iid_media_event_ex = {
    0x56a868c0, 0x0ad4, 0x11ce,
    {0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}
};
static const GUID liquid_iid_basic_audio = {
    0x56a868b3, 0x0ad4, 0x11ce,
    {0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}
};

static HRESULT liquid_basic_audio_put(void *basic_audio, int slot,
                                      long value)
{
    void **vtable;
    typedef HRESULT (WINAPI *put_long_t)(void *, long);
    if (!basic_audio || slot < 0) return E_POINTER;
    vtable = *(void ***)basic_audio;
    if (!vtable || !vtable[slot]) return E_POINTER;
    return ((put_long_t)vtable[slot])(basic_audio, value);
}

static void liquid_basic_audio_release(void *basic_audio)
{
    void **vtable;
    typedef ULONG (WINAPI *release_t)(void *);
    if (!basic_audio) return;
    vtable = *(void ***)basic_audio;
    if (vtable && vtable[2]) ((release_t)vtable[2])(basic_audio);
}

static int liquid_audio_supported_file(const char *name)
{
    const char *extension;
    if (!name) return 0;
    extension = strrchr(name, '.');
    return extension &&
           (_stricmp(extension, ".mp3") == 0 ||
            _stricmp(extension, ".wav") == 0);
}

static int liquid_audio_file_compare(const void *left, const void *right)
{
    const liquid_audio_file_t *a = (const liquid_audio_file_t *)left;
    const liquid_audio_file_t *b = (const liquid_audio_file_t *)right;
    return _stricmp(a->path, b->path);
}

static int liquid_audio_disabled(void)
{
    return !cfg.ejaculation_sound_preset[0] ||
           _stricmp(cfg.ejaculation_sound_preset, "false") == 0 ||
           _stricmp(cfg.ejaculation_sound_preset, "off") == 0 ||
           _stricmp(cfg.ejaculation_sound_preset, "disabled") == 0;
}

static void liquid_audio_refresh_files(void)
{
    static char loaded_preset[128];
    char preset_directory[MAX_PATH * 4];
    char pattern[MAX_PATH * 4];
    WIN32_FIND_DATAA entry;
    HANDLE search;
    if (_stricmp(loaded_preset, cfg.ejaculation_sound_preset) == 0)
        return;
    liquid_audio_file_count = 0;
    liquid_audio_last_file_index = -1;
    lstrcpynA(loaded_preset, cfg.ejaculation_sound_preset,
              sizeof(loaded_preset));
    if (liquid_audio_disabled()) return;
    _snprintf(preset_directory, sizeof(preset_directory), "%s\\%s",
              ejaculation_sound_root, cfg.ejaculation_sound_preset);
    preset_directory[sizeof(preset_directory) - 1] = 0;
    _snprintf(pattern, sizeof(pattern), "%s\\*", preset_directory);
    pattern[sizeof(pattern) - 1] = 0;
    search = FindFirstFileA(pattern, &entry);
    if (search == INVALID_HANDLE_VALUE) {
        log_line("ejaculation audio preset unavailable preset=\"%s\" directory=\"%s\"",
                 cfg.ejaculation_sound_preset, preset_directory);
        return;
    }
    do {
        liquid_audio_file_t *file;
        if ((entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
            !liquid_audio_supported_file(entry.cFileName)) continue;
        if (liquid_audio_file_count >= LIQUID_AUDIO_FILE_CAP) break;
        file = &liquid_audio_files[liquid_audio_file_count++];
        _snprintf(file->path, sizeof(file->path), "%s\\%s",
                  preset_directory, entry.cFileName);
        file->path[sizeof(file->path) - 1] = 0;
    } while (FindNextFileA(search, &entry));
    FindClose(search);
    if (liquid_audio_file_count > 1)
        qsort(liquid_audio_files, (size_t)liquid_audio_file_count,
              sizeof(liquid_audio_files[0]), liquid_audio_file_compare);
    log_line("ejaculation audio preset loaded preset=\"%s\" files=%d directory=\"%s\"",
             cfg.ejaculation_sound_preset, liquid_audio_file_count,
             preset_directory);
}

static unsigned int liquid_audio_random(void)
{
    unsigned int value = liquid_audio_random_state;
    if (!value) value = 0x4e434155u ^ (unsigned int)GetTickCount();
    value ^= value << 13;
    value ^= value >> 17;
    value ^= value << 5;
    liquid_audio_random_state = value;
    return value;
}

static int liquid_audio_choose_file(void)
{
    int index;
    if (liquid_audio_file_count <= 0) return -1;
    index = (int)(liquid_audio_random() %
                  (unsigned int)liquid_audio_file_count);
    if (liquid_audio_file_count > 1 &&
        index == liquid_audio_last_file_index) {
        index = (index + 1 +
                 (int)(liquid_audio_random() %
                       (unsigned int)(liquid_audio_file_count - 1))) %
                liquid_audio_file_count;
    }
    liquid_audio_last_file_index = index;
    return index;
}

static void liquid_audio_voice_release(liquid_audio_voice_t *voice)
{
    if (!voice) return;
    if (voice->control) {
        IMediaControl_Stop(voice->control);
        IMediaControl_Release(voice->control);
    }
    if (voice->events) IMediaEventEx_Release(voice->events);
    if (voice->basic_audio)
        liquid_basic_audio_release(voice->basic_audio);
    if (voice->graph) IGraphBuilder_Release(voice->graph);
    memset(voice, 0, sizeof(*voice));
}

static int liquid_audio_person_position(int person_index, float position[3])
{
    liquid_emitter_t probe;
    float direction[3];
    int index = person_index - 1;
    if (!position || person_index < 1 || person_index > 4) return 0;
    if (index >= 0 && index < LIQUID_MODEL_EMITTER_COUNT &&
        model_emitters[index].transform_valid) {
        memcpy(position, model_emitters[index].current_position,
               sizeof(model_emitters[index].current_position));
        return 1;
    }
    memset(&probe, 0, sizeof(probe));
    probe.source_kind = 1;
    probe.person_index = person_index;
    return liquid_emitter_transform(&probe, position, direction);
}

static float liquid_audio_base_gain(float setting)
{
    if (setting <= -10.0f) return 0.01f;
    if (setting >= 10.0f) return 1.0f;
    if (setting < 0.0f)
        return 0.01f + (setting + 10.0f) * 0.049f;
    return 0.50f + setting * 0.05f;
}

static long liquid_audio_gain_to_directshow(float gain)
{
    long value;
    if (gain <= 0.00001f) return -10000;
    if (gain >= 1.0f) return 0;
    value = (long)(2000.0 * log10((double)gain));
    if (value < -10000) value = -10000;
    if (value > 0) value = 0;
    return value;
}

static void liquid_audio_voice_spatialize(liquid_audio_voice_t *voice,
                                          DWORD now, int force)
{
    float source[3];
    float dx, dy, dz;
    float distance;
    float min_distance;
    float max_distance;
    float distance_gain = 1.0f;
    float view_x, view_z, horizontal;
    float gain;
    long volume;
    long balance;
    if (!voice || !voice->active || !voice->basic_audio) return;
    if (!force && voice->last_spatial_tick &&
        now - voice->last_spatial_tick < 50) return;
    voice->last_spatial_tick = now;
    if (!InterlockedCompareExchange(&captured_camera_inverse_valid, 0, 0) ||
        !liquid_audio_person_position(voice->person_index, source)) return;
    dx = source[0] - captured_camera_inverse[12];
    dy = source[1] - captured_camera_inverse[13];
    dz = source[2] - captured_camera_inverse[14];
    /* Render-space bone/camera coordinates are metre-like, while TK17's
       established positional-audio distances use centimetre-style units. */
    distance = (float)sqrt((double)(dx * dx + dy * dy + dz * dz)) *
               LIQUID_AUDIO_RENDER_TO_GAME_DISTANCE;
    min_distance = cfg.ejaculation_sound_min_distance *
                   LIQUID_AUDIO_RENDER_TO_GAME_DISTANCE;
    max_distance = cfg.ejaculation_sound_max_distance *
                   LIQUID_AUDIO_RENDER_TO_GAME_DISTANCE;
    if (distance >= max_distance) {
        distance_gain = 0.0f;
    } else if (distance > min_distance) {
        distance_gain = 1.0f -
            (distance - min_distance) / (max_distance - min_distance);
        distance_gain *= distance_gain;
    }
    view_x = dx * captured_camera_inverse[0] +
             dy * captured_camera_inverse[1] +
             dz * captured_camera_inverse[2];
    view_z = dx * captured_camera_inverse[8] +
             dy * captured_camera_inverse[9] +
             dz * captured_camera_inverse[10];
    horizontal = (float)sqrt((double)(view_x * view_x + view_z * view_z));
    balance = horizontal > 0.0001f ?
              (long)(8000.0f * view_x / horizontal) : 0;
    if (balance < -10000) balance = -10000;
    if (balance > 10000) balance = 10000;
    gain = liquid_audio_base_gain(voice->volume) * distance_gain;
    volume = liquid_audio_gain_to_directshow(gain);
    if (force || volume != voice->applied_volume) {
        liquid_basic_audio_put(voice->basic_audio, 7, volume);
        voice->applied_volume = volume;
    }
    if (force || balance != voice->applied_balance) {
        liquid_basic_audio_put(voice->basic_audio, 9, balance);
        voice->applied_balance = balance;
    }
}

static liquid_audio_voice_t *liquid_audio_claim_voice(void)
{
    liquid_audio_voice_t *oldest = NULL;
    int index;
    for (index = 0; index < LIQUID_AUDIO_VOICE_CAP; ++index) {
        liquid_audio_voice_t *voice = &liquid_audio_voices[index];
        if (!voice->active) return voice;
        if (!oldest ||
            (LONG)(voice->started_tick - oldest->started_tick) < 0)
            oldest = voice;
    }
    if (oldest) liquid_audio_voice_release(oldest);
    return oldest;
}

static int liquid_audio_voice_start(const liquid_audio_pending_t *pending,
                                    DWORD now)
{
    liquid_audio_voice_t *voice;
    wchar_t path[MAX_PATH * 4];
    HRESULT result;
    if (!pending || !pending->path[0]) return 0;
    if (!MultiByteToWideChar(CP_ACP, 0, pending->path, -1, path,
                             (int)(sizeof(path) / sizeof(path[0]))))
        return 0;
    result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(result) && result != RPC_E_CHANGED_MODE) {
        log_line("ejaculation audio COM initialization failed hr=0x%08lx path=\"%s\"",
                 (unsigned long)result, pending->path);
        return 0;
    }
    voice = liquid_audio_claim_voice();
    if (!voice) return 0;
    memset(voice, 0, sizeof(*voice));
    voice->person_index = pending->person_index;
    voice->pulse_index = pending->pulse_index;
    voice->emission_id = pending->emission_id;
    voice->volume = pending->volume;
    voice->applied_volume = LONG_MAX;
    voice->applied_balance = LONG_MAX;
    voice->started_tick = now;
    lstrcpynA(voice->path, pending->path, sizeof(voice->path));
    result = CoCreateInstance(&liquid_clsid_filter_graph, NULL,
                              CLSCTX_INPROC_SERVER,
                              &liquid_iid_graph_builder,
                              (void **)&voice->graph);
    if (FAILED(result) || !voice->graph) goto failed;
    result = IGraphBuilder_RenderFile(voice->graph, path, NULL);
    if (FAILED(result)) goto failed;
    IGraphBuilder_QueryInterface(voice->graph, &liquid_iid_media_control,
                                 (void **)&voice->control);
    IGraphBuilder_QueryInterface(voice->graph, &liquid_iid_media_event_ex,
                                 (void **)&voice->events);
    IGraphBuilder_QueryInterface(voice->graph, &liquid_iid_basic_audio,
                                 (void **)&voice->basic_audio);
    if (!voice->control) goto failed;
    voice->active = 1;
    liquid_audio_voice_spatialize(voice, now, 1);
    result = IMediaControl_Run(voice->control);
    if (FAILED(result)) goto failed;
    log_line("ejaculation audio started person=%d emission=%u pulse=%d volume=%.3f path=\"%s\"",
             voice->person_index, voice->emission_id,
             voice->pulse_index + 1, voice->volume, voice->path);
    return 1;

failed:
    log_line("ejaculation audio playback failed person=%d emission=%u pulse=%d hr=0x%08lx path=\"%s\"",
             pending->person_index, pending->emission_id,
             pending->pulse_index + 1, (unsigned long)result,
             pending->path);
    liquid_audio_voice_release(voice);
    return 0;
}

static void liquid_audio_schedule_pulse(const liquid_emitter_t *emitter,
                                        DWORD now)
{
    liquid_audio_pending_t *pending = NULL;
    int file_index;
    int index;
    if (!emitter || emitter->source_kind != 1 || liquid_audio_disabled())
        return;
    liquid_audio_refresh_files();
    file_index = liquid_audio_choose_file();
    if (file_index < 0) return;
    for (index = 0; index < LIQUID_AUDIO_PENDING_CAP; ++index) {
        if (!liquid_audio_pending[index].active) {
            pending = &liquid_audio_pending[index];
            break;
        }
    }
    if (!pending) {
        log_line("ejaculation audio queue full person=%d emission=%u pulse=%d",
                 emitter->person_index, emitter->emission_id,
                 emitter->current_pulse_index + 1);
        return;
    }
    memset(pending, 0, sizeof(*pending));
    pending->active = 1;
    pending->due_tick = now + (DWORD)cfg.ejaculation_sound_delay_ms;
    pending->person_index = emitter->person_index;
    pending->pulse_index = emitter->current_pulse_index;
    pending->emission_id = emitter->emission_id;
    pending->volume = cfg.ejaculation_sound_volume;
    lstrcpynA(pending->path, liquid_audio_files[file_index].path,
              sizeof(pending->path));
}

static void liquid_audio_update(DWORD now)
{
    int index;
    liquid_audio_refresh_files();
    for (index = 0; index < LIQUID_AUDIO_PENDING_CAP; ++index) {
        liquid_audio_pending_t pending;
        if (!liquid_audio_pending[index].active ||
            (LONG)(now - liquid_audio_pending[index].due_tick) < 0)
            continue;
        pending = liquid_audio_pending[index];
        memset(&liquid_audio_pending[index], 0,
               sizeof(liquid_audio_pending[index]));
        liquid_audio_voice_start(&pending, now);
    }
    for (index = 0; index < LIQUID_AUDIO_VOICE_CAP; ++index) {
        liquid_audio_voice_t *voice = &liquid_audio_voices[index];
        long event_code;
        LONG_PTR param1;
        LONG_PTR param2;
        int finished = 0;
        if (!voice->active) continue;
        liquid_audio_voice_spatialize(voice, now, 0);
        if (voice->events) {
            while (IMediaEventEx_GetEvent(voice->events, &event_code,
                                          &param1, &param2, 0) == S_OK) {
                IMediaEventEx_FreeEventParams(voice->events, event_code,
                                              param1, param2);
                if (event_code == EC_COMPLETE || event_code == EC_ERRORABORT ||
                    event_code == EC_USERABORT)
                    finished = 1;
            }
        }
        /* Event delivery is normally immediate. This ceiling prevents a bad
           decoder or malformed file from occupying a voice forever. */
        if (now - voice->started_tick > 120000u) finished = 1;
        if (finished) {
            log_line("ejaculation audio finished person=%d emission=%u pulse=%d path=\"%s\"",
                     voice->person_index, voice->emission_id,
                     voice->pulse_index + 1, voice->path);
            liquid_audio_voice_release(voice);
        }
    }
}

static void liquid_begin_emitter(liquid_emitter_t *emitter, int source_kind,
                                 int person_index, DWORD start_tick,
                                 float duration)
{
    DWORD duration_ms = (DWORD)(duration * 1000.0f + 0.5f);
    if (!emitter) return;
    memset(emitter, 0, sizeof(*emitter));
    emitter->active = 1;
    emitter->source_kind = source_kind;
    emitter->person_index = person_index;
    emitter->start_tick = start_tick;
    emitter->end_tick = start_tick + duration_ms;
    emitter->sound_pulse_index = -1;
    liquid_emission_serial++;
    if (!liquid_emission_serial) liquid_emission_serial++;
    emitter->emission_id = liquid_emission_serial;
    if (source_kind == 1) {
        int emitter_index = person_index - 1;
        if (emitter_index >= 0 &&
            emitter_index < LIQUID_MODEL_EMITTER_COUNT)
            InterlockedExchange(
                &liquid_native_last_contact_queue_tick[emitter_index], 0);
        InterlockedExchange(&liquid_latest_model_emission,
                            (LONG)emitter->emission_id);
    }
    log_line("liquid prototype emitter started source=%s person=%d start=%lu duration=%.3f end=%lu",
             source_kind == 2 ? "tool" : "model", person_index,
             (unsigned long)start_tick, duration,
             (unsigned long)emitter->end_tick);
}

static void liquid_note_tool_command(DWORD now)
{
    if (!cfg.liquids_enabled) return;
    /* CumShot is TK17's POV tool. Keep its native spermray untouched and do
       not create custom liquid particles. The timestamp is retained only so
       its native splash callbacks cannot be mistaken for person emission. */
    last_tool_command_tick = now;
    memset(&tool_emitter, 0, sizeof(tool_emitter));
    log_line("liquid POV tool command observed; custom emission disabled and native spermray retained");
}

static int liquid_runtime_splash_person(const void *splash_object)
{
    void *source;
    if (!splash_object ||
        !ptr_readable((const BYTE*)splash_object + 0x10, sizeof(source)))
        return 0;
    source = *(void**)((const BYTE*)splash_object + 0x10);
    return liquid_spermray_source_person(source);
}

static void liquid_note_runtime_splash(const void *splash_object,
                                       const void *caller)
{
    DWORD now = GetTickCount();
    DWORD tool_window = (DWORD)((cfg.tool_start_delay + cfg.tool_duration +
                                 1.0f) * 1000.0f);
    int person_index;
    liquid_emitter_t *emitter;
    if (!cfg.liquids_enabled ||
        !liquid_module_rva(caller, "ThriXXX010278-SYS.dll",
                           LIQUID_RUNTIME_SPLASH_RVA)) return;
    person_index = liquid_runtime_splash_person(splash_object);
    if (!person_index && last_tool_command_tick &&
        now - last_tool_command_tick <= tool_window) {
        tool_emitter.last_native_splash_tick = now;
        return;
    }
    if (person_index < 1 || person_index > 4) person_index = 1;
    emitter = liquid_model_emitter_for_person(person_index);
    if (!emitter) return;
    if (!emitter->active || now > emitter->end_tick ||
        (emitter->last_native_splash_tick &&
         now - emitter->last_native_splash_tick > 1500)) {
        liquid_begin_emitter(emitter, 1, person_index, now,
                             cfg.model_duration);
    }
    emitter->last_native_splash_tick = now;
    InterlockedExchange(&liquid_latest_model_emission,
                        (LONG)emitter->emission_id);
}

/* Interpolate the sampled source only across short, continuous motion.
   Teleports, missing transforms and near-opposite directions restart at the
   current source instead of drawing liquid across an unrelated pose. */
static int liquid_emitter_history_continuous(
    const float old_position[3], const float old_direction[3],
    const float position[3], const float direction[3], int history_valid)
{
    float delta[3];
    int axis;
    for (axis = 0; axis < 3; axis++) delta[axis] = position[axis] - old_position[axis];
    return history_valid && liquid_vec3_dot(delta, delta) <= 0.1225f &&
           liquid_vec3_dot(old_direction, direction) >= 0.0f;
}

static void liquid_sample_emitter_birth(
    const float old_position[3], const float old_direction[3],
    const float position[3], const float direction[3], int history_valid,
    float fraction, float birth_position[3], float birth_direction[3])
{
    float delta[3];
    int axis;
    for (axis = 0; axis < 3; axis++)
        delta[axis] = position[axis] - old_position[axis];
    if (!liquid_emitter_history_continuous(old_position, old_direction,
                                          position, direction, history_valid))
        fraction = 1.0f;
    if (fraction < 0.0f) fraction = 0.0f;
    if (fraction > 1.0f) fraction = 1.0f;
    for (axis = 0; axis < 3; axis++) {
        birth_position[axis] = old_position[axis] + delta[axis] * fraction;
        birth_direction[axis] = old_direction[axis] +
            (direction[axis] - old_direction[axis]) * fraction;
    }
    if (!liquid_vec3_normalize(birth_direction))
        memcpy(birth_direction, direction, sizeof(float) * 3);
}

static void liquid_spawn_particle(liquid_emitter_t *emitter,
                                  const float position[3],
                                  const float direction[3],
                                  int pulse_index,
                                  float pulse_strength, float birth_delay,
                                  float emission_time)
{
    int i;
    int limit = cfg.particle_limit;
    float velocity[3];
    float random_size;
    float speed_scale;
    float spread_scale = 1.0f;
    if (pulse_strength < 0.15f) pulse_strength = 0.15f;
    if (pulse_strength > 1.5f) pulse_strength = 1.5f;
    for (i = 0; i < limit; i++) {
        liquid_particle_t *particle = &liquid_particles[i];
        if (particle->active) continue;
        memset(particle, 0, sizeof(*particle));
        particle->active = 1;
        particle->birth_delay = birth_delay;
        particle->emission_time = emission_time;
        particle->stream_id = emitter ? emitter->stream_id : 0;
        particle->stream_stretch = 1.0f;
        particle->separation_age = -1.0f;
        particle->emission_id = emitter ? emitter->emission_id : 0;
        particle->source_kind = emitter ? emitter->source_kind : 0;
        liquid_particle_serial++;
        if (!liquid_particle_serial) liquid_particle_serial++;
        particle->spawn_order = liquid_particle_serial;
        particle->pulse_index = pulse_index;
        if (emitter && emitter->source_kind == 1) {
            float satellite_roll =
                (liquid_random_signed() + 1.0f) * 0.5f;
            particle->satellite =
                satellite_roll < cfg.model_satellite_chance;
            spread_scale = particle->satellite ? 1.0f : 0.28f;
        }
        memcpy(particle->position, position, sizeof(particle->position));
        memcpy(particle->previous, position, sizeof(particle->previous));
        velocity[0] = direction[0] +
                      liquid_random_signed() * cfg.spread * spread_scale;
        velocity[1] = direction[1] +
                      liquid_random_signed() * cfg.spread * spread_scale;
        velocity[2] = direction[2] +
                      liquid_random_signed() * cfg.spread * spread_scale;
        liquid_vec3_normalize(velocity);
        speed_scale = (0.82f +
                       (liquid_random_signed() + 1.0f) * 0.12f) *
                      (0.72f + pulse_strength * 0.30f);
        /* Keep the main flow coherent; independent spray retains variation. */
        if (emitter && emitter->source_kind == 1 && !particle->satellite)
            speed_scale = (0.94f + liquid_random_signed() * 0.015f) *
                          (0.72f + pulse_strength * 0.30f);
        particle->velocity[0] = velocity[0] * cfg.speed * speed_scale;
        particle->velocity[1] = velocity[1] * cfg.speed * speed_scale;
        particle->velocity[2] = velocity[2] * cfg.speed * speed_scale;
        particle->launch_speed = cfg.speed * speed_scale;
        if (emitter && !particle->satellite) {
            int neighbor_index = emitter->last_stream_particle;
            if (neighbor_index >= 0 && neighbor_index < limit) {
                liquid_particle_t *neighbor = &liquid_particles[neighbor_index];
                float interval = emission_time - neighbor->emission_time;
                if (neighbor != particle && neighbor->active &&
                    !neighbor->collided && !neighbor->satellite &&
                    neighbor->spawn_order == emitter->last_stream_order &&
                    neighbor->emission_id == particle->emission_id &&
                    neighbor->pulse_index == pulse_index && interval > 0.0f) {
                    particle->stream_neighbor = neighbor_index;
                    particle->stream_neighbor_order = neighbor->spawn_order;
                    particle->stream_rest_length = interval *
                        (particle->launch_speed + neighbor->launch_speed) * 0.5f;
                    if (particle->stream_rest_length < cfg.particle_size * 0.5f)
                        particle->stream_rest_length = cfg.particle_size * 0.5f;
                }
            }
            emitter->last_stream_particle = i;
            emitter->last_stream_order = particle->spawn_order;
        }
        particle->lifetime = cfg.particle_lifetime *
            (0.8f + (liquid_random_signed() + 1.0f) * 0.1f);
        random_size = 0.78f +
            (liquid_random_signed() + 1.0f) * 0.22f;
        particle->size_scale = random_size *
            (0.82f + pulse_strength * 0.18f);
        if (particle->satellite) particle->size_scale *= 0.68f;
        particle->opacity = 0.52f +
            (liquid_random_signed() + 1.0f) * 0.10f;
        if (particle->satellite) particle->opacity *= 0.82f;
        return;
    }
}

static void liquid_update_emitter(liquid_emitter_t *emitter, DWORD now,
                                  float dt)
{
    static int transform_failure_logs[3];
    static int transform_success_logs[3];
    float position[3];
    float direction[3];
    float old_position[3], old_direction[3];
    float sample_dt;
    float emission_dt = dt;
    int history_valid;
    float emission_strength = 1.0f;
    float emission_rate_scale = 1.0f;
    int pulse_index = 0;
    int spawn_count;
    if (!emitter) return;
    emitter->pulse_emitting = 0;
    if (!emitter->active) {
        emitter->transform_valid = 0;
        return;
    }
    if (now < emitter->start_tick) return;
    if (now >= emitter->end_tick) {
        emitter->active = 0;
        emitter->transform_valid = 0;
        return;
    }
    if (!liquid_emitter_transform(emitter, position, direction)) {
        int kind = emitter->source_kind >= 1 && emitter->source_kind <= 2 ?
                   emitter->source_kind : 0;
        if (transform_failure_logs[kind] < 5) {
            transform_failure_logs[kind]++;
            log_line("liquid prototype emitter transform unavailable source=%s person=%d camera_valid=%ld failure=%d",
                     kind == 2 ? "tool" : "model", emitter->person_index,
                     InterlockedCompareExchange(
                         &captured_camera_inverse_valid, 0, 0),
                     transform_failure_logs[kind]);
        }
        emitter->transform_valid = 0;
        return;
    }
    memcpy(old_position, emitter->current_position, sizeof(old_position));
    memcpy(old_direction, emitter->current_direction, sizeof(old_direction));
    sample_dt = (float)(now - emitter->transform_tick) * 0.001f;
    history_valid = emitter->transform_valid && now != emitter->transform_tick &&
                    now - emitter->transform_tick <= 100u;
    history_valid = liquid_emitter_history_continuous(old_position, old_direction,
                                                      position, direction, history_valid);
    if (!history_valid) {
        emitter->stream_id++;
        if (!emitter->stream_id) emitter->stream_id++;
        emitter->last_stream_order = 0;
    }
    memcpy(emitter->current_direction, direction, sizeof(direction));
    emitter->transform_tick = now;
    memcpy(emitter->current_position, position,
           sizeof(emitter->current_position));
    emitter->transform_valid = 1;
    if (cfg.enabled && emitter->source_kind == 1 &&
        emitter->person_index >= 1 && emitter->person_index <= 4) {
        static DWORD sample_tick[4];
        int index = emitter->person_index - 1;
        if (!sample_tick[index] || now - sample_tick[index] >= 250u) {
            sample_tick[index] = now;
            log_line("liquid world emitter sample person=%d emission=%u position=(%.5f,%.5f,%.5f) direction=(%.5f,%.5f,%.5f) source=rebuilt-joint-local-hierarchy",
                     emitter->person_index, emitter->emission_id,
                     position[0], position[1], position[2],
                     direction[0], direction[1], direction[2]);
        }
    }
    {
        int kind = emitter->source_kind >= 1 && emitter->source_kind <= 2 ?
                   emitter->source_kind : 0;
        if (!transform_success_logs[kind]) {
            transform_success_logs[kind] = 1;
            log_line("liquid prototype emitter transform active source=%s person=%d position=(%.4f,%.4f,%.4f) direction=(%.4f,%.4f,%.4f)",
                     kind == 2 ? "tool" : "model", emitter->person_index,
                     position[0], position[1], position[2],
                     direction[0], direction[1], direction[2]);
        }
    }
    if (emitter->source_kind == 1) {
        float elapsed = (float)(now - emitter->start_tick) * 0.001f;
        float cycle = cfg.model_pulse_duration + cfg.model_pulse_interval;
        pulse_index = cycle > 0.0001f ?
                      (int)(elapsed / cycle) : 0;
        float pulse_time = elapsed - (float)pulse_index * cycle;
        float normalized;
        float envelope;
        float decay;
        if (pulse_index >= cfg.model_pulse_count ||
            pulse_time >= cfg.model_pulse_duration) {
            emitter->spawn_accumulator = 0.0f;
            return;
        }
        if (emitter->current_pulse_index != pulse_index) {
            emitter->spawn_accumulator = 0.0f;
            emitter->last_stream_order = 0;
        }
        if (emission_dt > pulse_time) emission_dt = pulse_time;
        emitter->current_pulse_index = pulse_index;
        emitter->pulse_emitting = 1;
        if (emitter->sound_pulse_index != pulse_index) {
            emitter->sound_pulse_index = pulse_index;
            liquid_audio_schedule_pulse(emitter, now);
        }
        normalized = pulse_time / cfg.model_pulse_duration;
        envelope = (float)sin((double)(normalized * 3.14159265359f));
        if (envelope < 0.0f) envelope = 0.0f;
        decay = 1.0f - cfg.model_pulse_decay * (float)pulse_index;
        if (decay < 0.35f) decay = 0.35f;
        emission_strength = (0.35f + envelope * 0.95f) * decay;
        emission_rate_scale = (0.30f + envelope * 1.25f) * decay;
    }
    emitter->spawn_accumulator +=
        cfg.spawn_rate * emission_rate_scale * emission_dt;
    spawn_count = (int)emitter->spawn_accumulator;
    emitter->spawn_accumulator -= (float)spawn_count;
    /* Drop excess work after a stall; never accumulate a delayed burst. */
    if (spawn_count > 12) spawn_count = 12;
    while (spawn_count-- > 0) {
        /* Accumulator crossings are births within this frame. Advancing all
           newborns for a full frame collapses them into visible clumps. */
        float remaining = (emitter->spawn_accumulator + spawn_count) /
                          (cfg.spawn_rate * emission_rate_scale);
        float birth_position[3], birth_direction[3];
        if (remaining > emission_dt) remaining = emission_dt;
        liquid_sample_emitter_birth(old_position, old_direction,
            position, direction, history_valid,
            history_valid ? 1.0f - remaining / sample_dt : 1.0f,
            birth_position, birth_direction);
        liquid_spawn_particle(emitter, birth_position, birth_direction, pulse_index,
            emission_strength, dt - remaining,
            (float)(now - emitter->start_tick) * 0.001f - remaining);
    }
}

static float liquid_clamp_unit(float value)
{
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

static float liquid_smoothstep_unit(float value)
{
    value = liquid_clamp_unit(value);
    return value * value * (3.0f - 2.0f * value);
}

static float liquid_cubic_bezier_component(float t, float p1, float p2)
{
    float inverse = 1.0f - t;
    return 3.0f * inverse * inverse * t * p1 +
           3.0f * inverse * t * t * p2 + t * t * t;
}

/* Match CSS cubic-bezier timing: the input progress is the curve's X
   coordinate, so solve X(t) first and then evaluate Y(t). */
static float liquid_cubic_bezier_easing(float progress,
                                        const float bezier[4])
{
    float low = 0.0f;
    float high = 1.0f;
    float t = liquid_clamp_unit(progress);
    int iteration;
    if (progress <= 0.0f) return 0.0f;
    if (progress >= 1.0f) return 1.0f;
    for (iteration = 0; iteration < 16; iteration++) {
        float x;
        t = (low + high) * 0.5f;
        x = liquid_cubic_bezier_component(t, bezier[0], bezier[2]);
        if (x < progress)
            low = t;
        else
            high = t;
    }
    return liquid_cubic_bezier_component(t, bezier[1], bezier[3]);
}

static float liquid_apply_easing(float progress, int mode,
                                 const float bezier[4])
{
    if (mode == LIQUID_EASING_LINEAR)
        return liquid_clamp_unit(progress);
    if (mode == LIQUID_EASING_BEZIER)
        return liquid_cubic_bezier_easing(progress, bezier);
    return liquid_smoothstep_unit(progress);
}

static void *liquid_resolve_testicular_retraction_control(int person_index)
{
    char path[128];
    void *wrapper;
    void *control = NULL;
    void *nil_weak = NULL;
    void *nil_object = NULL;
    const void *wrapper_type = NULL;
    const void *control_type = NULL;
    const char *resolution_mode = NULL;
    static LONG resolution_log_count;
    float weight;
    if (person_index < 1 || person_index > LIQUID_MODEL_EMITTER_COUNT)
        return NULL;
    _snprintf(path, sizeof(path),
              "Person%02dAnim:Model01:penis_poseBlend_scrotum_hard",
              person_index);
    path[sizeof(path) - 1] = 0;
    wrapper = liquid_find_object(path);
    if (!wrapper) return NULL;
    if (engine_G_NilWeakObjTarget_ptr &&
        ptr_readable(engine_G_NilWeakObjTarget_ptr, sizeof(void*)))
        nil_weak = *engine_G_NilWeakObjTarget_ptr;
    if (engine_G_NilObject_ptr &&
        ptr_readable(engine_G_NilObject_ptr, sizeof(void*)))
        nil_object = *engine_G_NilObject_ptr;
    /* FindObjC is mode-dependent here. FreeMode commonly returns a weak
       reference whose target is the BlendControl, while PoseEditor can
       return the BlendControl itself. Identify the engine type before
       selecting either form; rejecting target==wrapper was why every pulse
       was ignored in PoseEditor. */
    if (engine_ObjectGetTypeInfo && engine_BlendControlTypeInfo &&
        ptr_readable(wrapper, sizeof(void*)))
        wrapper_type = engine_ObjectGetTypeInfo(wrapper);
    if (wrapper_type == engine_BlendControlTypeInfo) {
        control = wrapper;
        resolution_mode = "FindObjC-direct";
    } else if (engine_GetWeakObjTarget) {
        control = engine_GetWeakObjTarget(wrapper);
        if (control && control != nil_weak && control != nil_object &&
            ptr_readable(control, sizeof(void*)) &&
            engine_ObjectGetTypeInfo && engine_BlendControlTypeInfo)
            control_type = engine_ObjectGetTypeInfo(control);
        if (control_type == engine_BlendControlTypeInfo)
            resolution_mode = "weak-target";
        else
            control = NULL;
    }
    if (!control || control == nil_weak || control == nil_object ||
        !ptr_readable((const BYTE*)control + 0x10, sizeof(weight)) ||
        !ptr_writable((BYTE*)control + 0x10, sizeof(weight)))
        return NULL;
    weight = *(const float*)((const BYTE*)control + 0x10);
    if (!_finite(weight) || weight < -1.0f || weight > 2.0f) return NULL;
    if (InterlockedIncrement(&resolution_log_count) <= 8)
        log_line("testicular retraction control resolved person=%d mode=%s wrapper=%p control=%p weight=%.3f",
                 person_index, resolution_mode ? resolution_mode : "",
                 wrapper, control, weight);
    return control;
}

static void liquid_testicular_retraction_restore(
    liquid_testicular_retraction_state_t *state)
{
    float current;
    if (!state) return;
    if (state->control && liquid_physx_set_blend_overlay)
        liquid_physx_set_blend_overlay(state->control, 0.0f, 0);
    if (state->control && state->last_written_valid &&
        ptr_readable((const BYTE*)state->control + 0x10, sizeof(current)) &&
        ptr_writable((BYTE*)state->control + 0x10, sizeof(current))) {
        current = *(const float*)((const BYTE*)state->control + 0x10);
        if (_finite(current) &&
            liquid_absf(current - state->last_written) <= 0.002f)
            *(float*)((BYTE*)state->control + 0x10) =
                liquid_clamp_unit(state->baseline);
    }
    state->active = 0;
    state->contribution = 0.0f;
    state->start_contribution = 0.0f;
    state->last_written_valid = 0;
}

static void liquid_update_testicular_retraction(DWORD now)
{
    int index;
    int feature_enabled = cfg.liquids_enabled &&
                          cfg.testicular_retraction_enabled;
    for (index = 0; index < LIQUID_MODEL_EMITTER_COUNT; index++) {
        liquid_emitter_t *emitter = &model_emitters[index];
        liquid_testicular_retraction_state_t *state =
            &liquid_testicular_retraction_states[index];
        int new_pulse = feature_enabled && emitter->pulse_emitting &&
            (state->emission_id != emitter->emission_id ||
             state->pulse_index != emitter->current_pulse_index);
        float current;
        float elapsed;
        float attack_end;
        float hold_end;
        float release_end;

        if (!feature_enabled) {
            liquid_testicular_retraction_restore(state);
            state->control = NULL;
            state->emission_id = 0;
            state->pulse_index = -1;
            continue;
        }

        if (new_pulse) {
            void *control = liquid_resolve_testicular_retraction_control(
                emitter->person_index);
            if (!control) {
                if (!state->resolve_retry_tick ||
                    now - state->resolve_retry_tick >= 250u) {
                    state->resolve_retry_tick = now;
                    log_line("testicular retraction control unavailable person=%d emission=%u pulse=%d",
                             emitter->person_index, emitter->emission_id,
                             emitter->current_pulse_index + 1);
                }
            } else {
                if (state->control != control) {
                    liquid_testicular_retraction_restore(state);
                    memset(state, 0, sizeof(*state));
                    state->control = control;
                    state->pulse_index = -1;
                }
                current = *(const float*)((const BYTE*)control + 0x10);
                if (!state->last_written_valid ||
                    liquid_absf(current - state->last_written) > 0.002f)
                    state->baseline = liquid_clamp_unit(current);
                state->emission_id = emitter->emission_id;
                state->pulse_index = emitter->current_pulse_index;
                state->trigger_tick = now;
                state->start_contribution = state->contribution;
                state->active = 1;
                state->resolve_retry_tick = 0;
                log_line("testicular retraction pulse person=%d emission=%u pulse=%d baseline=%.3f start=%.3f target=%.3f",
                         emitter->person_index, emitter->emission_id,
                         emitter->current_pulse_index + 1,
                         state->baseline, state->start_contribution,
                         cfg.testicular_retraction_target_weight);
            }
        }

        if (!state->active || !state->control) continue;
        if (!ptr_readable((const BYTE*)state->control + 0x10,
                          sizeof(current)) ||
            !ptr_writable((BYTE*)state->control + 0x10,
                          sizeof(current))) {
            memset(state, 0, sizeof(*state));
            state->pulse_index = -1;
            continue;
        }
        current = *(const float*)((const BYTE*)state->control + 0x10);
        if (!_finite(current) || current < -1.0f || current > 2.0f) {
            memset(state, 0, sizeof(*state));
            state->pulse_index = -1;
            continue;
        }
        elapsed = (float)(now - state->trigger_tick) * 0.001f;
        attack_end = cfg.testicular_retraction_time;
        hold_end = attack_end + cfg.testicular_retraction_hold_time;
        release_end = hold_end + cfg.testicular_retraction_release_time;
        if (attack_end > 0.0001f && elapsed < attack_end) {
            float blend = liquid_apply_easing(
                elapsed / attack_end,
                cfg.testicular_retraction_easing,
                cfg.testicular_retraction_bezier);
            state->contribution = state->start_contribution +
                (cfg.testicular_retraction_target_weight -
                 state->start_contribution) * blend;
        } else if (elapsed < hold_end) {
            state->contribution =
                cfg.testicular_retraction_target_weight;
        } else if (cfg.testicular_retraction_release_time > 0.0001f &&
                   elapsed < release_end) {
            float blend = liquid_apply_easing(
                (elapsed - hold_end) /
                cfg.testicular_retraction_release_time,
                cfg.testicular_release_easing,
                cfg.testicular_release_bezier);
            state->contribution =
                cfg.testicular_retraction_target_weight * (1.0f - blend);
        } else {
            liquid_testicular_retraction_restore(state);
            continue;
        }
    }
}

/* The render callbacks advance pulse timing after TK17 has already drawn the
   body. Apply the temporary morph from the native animation/update path
   instead, after TK17 has written its ordinary PoseEditor/animation value
   and before the model is rendered. This touches only BlendControl.Weight;
   no testicle PhysX switch, joint transform, or solver state is modified. */
static void liquid_apply_testicular_retraction_weights(void)
{
    static DWORD trace_tick[LIQUID_MODEL_EMITTER_COUNT];
    int index;
    if (!cfg.liquids_enabled || !cfg.testicular_retraction_enabled) return;
    for (index = 0; index < LIQUID_MODEL_EMITTER_COUNT; index++) {
        liquid_testicular_retraction_state_t *state =
            &liquid_testicular_retraction_states[index];
        float current;
        float output;
        if (!state->active || !state->control) continue;
        if (!ptr_readable((const BYTE*)state->control + 0x10,
                          sizeof(current)) ||
            !ptr_writable((BYTE*)state->control + 0x10,
                          sizeof(current)))
            continue;
        current = *(const float*)((const BYTE*)state->control + 0x10);
        if (!_finite(current) || current < -1.0f || current > 2.0f)
            continue;
        if (!state->last_written_valid ||
            liquid_absf(current - state->last_written) > 0.002f)
            state->baseline = liquid_clamp_unit(current);
        output = liquid_clamp_unit(state->baseline + state->contribution);
        if (liquid_physx_set_blend_overlay)
            liquid_physx_set_blend_overlay(state->control, output, 1);
        if (!liquid_physx_set_blend_weight ||
            !liquid_physx_set_blend_weight(state->control, output))
            *(float*)((BYTE*)state->control + 0x10) = output;
        if (cfg.enabled) {
            DWORD now = GetTickCount();
            if (now - trace_tick[index] >= 250u) {
                trace_tick[index] = now;
                log_line("testicular retraction live write person=%d control=%p input=%.3f baseline=%.3f contribution=%.3f output=%.3f",
                         index + 1, state->control, current,
                         state->baseline, state->contribution, output);
            }
        }
        state->last_written = output;
        state->last_written_valid = 1;
        if (state->applied_emission_id != state->emission_id ||
            state->applied_pulse_index != state->pulse_index) {
            state->applied_emission_id = state->emission_id;
            state->applied_pulse_index = state->pulse_index;
            log_line("testicular retraction applied person=%d emission=%u pulse=%d baseline=%.3f contribution=%.3f output=%.3f",
                     index + 1, state->emission_id,
                     state->pulse_index + 1, state->baseline,
                     state->contribution, output);
        }
    }
}

/* Exact constant-gravity, linear-drag flight. The small-x expansion avoids
   cancellation at tiny timesteps and approaches ballistic motion smoothly. */
static void liquid_integrate_flight(liquid_particle_t *particle, float dt)
{
    double k = cfg.drag;
    double x = k * dt;
    double damping = exp(-x);
    double travel, gravity_travel;
    int axis;
    if (fabs(x) < 0.001) {
        travel = dt * (1.0 - x * 0.5 + x * x / 6.0);
        gravity_travel = dt * dt * (0.5 - x / 6.0 + x * x / 24.0);
    } else {
        travel = (1.0 - damping) / k;
        gravity_travel = (dt - travel) / k;
    }
    memcpy(particle->previous, particle->position, sizeof(particle->previous));
    for (axis = 0; axis < 3; axis++) {
        double acceleration = axis == 1 ? -9.81 * cfg.gravity_strength : 0.0;
        particle->position[axis] += (float)(particle->velocity[axis] * travel +
                                            acceleration * gravity_travel);
        particle->velocity[axis] = (float)(particle->velocity[axis] * damping +
                                           acceleration * travel);
    }
    particle->age += dt;
}

static float liquid_model_breakup_blend(const liquid_particle_t *particle);

/* Links are captured at birth, identified by slot AND serial, and evaluated
   after every particle has advanced. Reusing a pool slot cannot reconnect an
   old strand. Stretch and detachment are independent of camera/render order. */
static void liquid_update_stream_stretch(float dt)
{
    int i;
    float response = 1.0f - expf(-25.0f * dt);
    for (i = 0; i < cfg.particle_limit; i++) {
        liquid_particle_t *p = &liquid_particles[i];
        if (!p->active || p->collided || p->satellite || p->source_kind != 1)
            continue;
        if (p->stream_neighbor_order && p->stream_rest_length > 0.0f &&
            p->stream_neighbor >= 0 && p->stream_neighbor < cfg.particle_limit) {
            liquid_particle_t *neighbor = &liquid_particles[p->stream_neighbor];
            if (neighbor->active && !neighbor->collided &&
                neighbor->spawn_order == p->stream_neighbor_order &&
                neighbor->emission_id == p->emission_id &&
                neighbor->pulse_index == p->pulse_index) {
                float delta[3], stretch, breakup;
                int axis;
                for (axis = 0; axis < 3; axis++)
                    delta[axis] = p->position[axis] - neighbor->position[axis];
                stretch = sqrtf(liquid_vec3_dot(delta, delta)) / p->stream_rest_length;
                if (stretch < 1.0f) stretch = 1.0f;
                if (stretch > 6.0f) stretch = 6.0f;
                p->stream_stretch += (stretch - p->stream_stretch) * response;
                breakup = liquid_smoothstep_unit((p->stream_stretch - 1.6f) / 1.6f) *
                          liquid_smoothstep_unit(p->age / 0.08f);
                if (breakup > p->stretch_breakup) p->stretch_breakup = breakup;
            }
        }
        if (p->separation_age < 0.0f && liquid_model_breakup_blend(p) > 0.02f)
            p->separation_age = p->age;
    }
}

static void liquid_simulation_tick(void)
{
    DWORD now = GetTickCount();
    float dt;
    int i;
    liquid_check_config_reload(now);
    /* Register the late blend overlay independently of particle collision.
       Retraction must still work when collision is disabled or before the
       first emitted particle reaches a collider. */
    if (cfg.testicular_retraction_enabled &&
        !InterlockedCompareExchange(
            &liquid_physx_retraction_callback_registered, 0, 0))
        liquid_resolve_physx_body_api(now);
    if (!cfg.liquids_enabled) {
        liquid_audio_update(now);
        liquid_update_testicular_retraction(now);
        liquid_last_tick = now;
        return;
    }
    if (!liquid_last_tick) {
        liquid_last_tick = now;
        return;
    }
    dt = (float)(now - liquid_last_tick) * 0.001f;
    liquid_last_tick = now;
    if (dt <= 0.0f) return;
    if (dt > 0.05f) dt = 0.05f;
    for (i = 0; i < LIQUID_MODEL_EMITTER_COUNT; i++)
        liquid_update_emitter(&model_emitters[i], now, dt);
    liquid_update_emitter(&tool_emitter, now, dt);
    liquid_audio_update(now);
    liquid_update_testicular_retraction(now);
    /* The plugin spends nearly all of its lifetime idle. Keep live reload
       and trigger handling above, but avoid all particle and body-anchor
       work when there is no emitter and no visible liquid. */
    if (!liquid_runtime_work_active(now)) return;
    if (cfg.collision_enabled)
        liquid_request_physx_body_colliders(now);
    /* Contacts can outlive TK17's brief native-stain update window. Refresh
       the compact root/spine/hip body basis from the same render/simulation
       thread that already resolves emitter pivots, so attached droplets keep
       following after ejaculation has ended and while the person moves. */
    liquid_update_contact_anchor_cache();
    for (i = 0; i < cfg.particle_limit; i++) {
        liquid_particle_t *particle = &liquid_particles[i];
        float flight_dt;
        if (!particle->active) continue;
        if (particle->collided) {
            if (cfg.collision_follow_bodies &&
                particle->contact_physx_person >= 1 &&
                particle->contact_physx_person <= 4 &&
                particle->contact_anchor == 0) {
                liquid_attach_contact_to_body(
                    particle, particle->contact_physx_person, 2.0f);
            }
            if (InterlockedExchange(
                    &particle->model_contact_confirmed, 0) &&
                particle->contact_physx_person >= 0 &&
                InterlockedCompareExchange(&particle->model_contact_verified, 0, 0)) {
                if (cfg.collision_follow_bodies) {
                    if (!liquid_attach_contact_to_body(
                            particle,
                            particle->contact_physx_person > 0 ?
                                particle->contact_physx_person : 0,
                            2.0f))
                        InterlockedExchange(
                            &particle->model_contact_confirmed, 1);
                } else if (cfg.collision_spawn_model_stains &&
                           particle->lifetime > particle->age + 0.18f) {
                    /* TK17's native decal is now attached to the skinned
                       target. Retire the temporary world-space impact blob
                       quickly so it cannot float behind when that target
                       moves. */
                    particle->lifetime = particle->age + 0.18f;
                }
            }
            /* Unknown depth contacts remain in world space until this exact
               impact is confirmed. Neighboring droplets cannot confer body
               ownership, even when they share a pulse or visual connection. */
            liquid_follow_attached_contact(particle);
            particle->collision_age += dt;
            particle->age += dt;
            if (particle->collision_age >= cfg.collision_surface_hold ||
                particle->age >= particle->lifetime)
                particle->active = 0;
            continue;
        }
        flight_dt = dt - particle->birth_delay;
        particle->birth_delay = 0.0f;
        if (flight_dt < 0.0f) flight_dt = 0.0f;
        liquid_integrate_flight(particle, flight_dt);
        if (particle->age >= particle->lifetime) particle->active = 0;
    }
    liquid_update_stream_stretch(dt);
}

static void liquid_identity_matrix(D3DMATRIX *matrix)
{
    memset(matrix, 0, sizeof(*matrix));
    matrix->_11 = 1.0f;
    matrix->_22 = 1.0f;
    matrix->_33 = 1.0f;
    matrix->_44 = 1.0f;
}

static int liquid_project_d3d_view_point(const D3DMATRIX *projection,
                                         const D3DVIEWPORT8 *viewport,
                                         const float point[3],
                                         float *screen_x, float *screen_y,
                                         float *screen_depth)
{
    float clip_x, clip_y, clip_z, clip_w;
    float ndc_x, ndc_y, ndc_z;
    if (!projection || !viewport || !point || !screen_x || !screen_y ||
        !screen_depth || !viewport->Width || !viewport->Height) return 0;
    clip_x = point[0] * projection->_11 + point[1] * projection->_21 +
             point[2] * projection->_31 + projection->_41;
    clip_y = point[0] * projection->_12 + point[1] * projection->_22 +
             point[2] * projection->_32 + projection->_42;
    clip_z = point[0] * projection->_13 + point[1] * projection->_23 +
             point[2] * projection->_33 + projection->_43;
    clip_w = point[0] * projection->_14 + point[1] * projection->_24 +
             point[2] * projection->_34 + projection->_44;
    if (!_finite(clip_w) || clip_w <= 0.00001f) return 0;
    ndc_x = clip_x / clip_w;
    ndc_y = clip_y / clip_w;
    ndc_z = clip_z / clip_w;
    if (!_finite(ndc_x) || !_finite(ndc_y) || !_finite(ndc_z) ||
        ndc_x < -2.0f || ndc_x > 2.0f ||
        ndc_y < -2.0f || ndc_y > 2.0f ||
        ndc_z < -0.02f || ndc_z > 1.02f) return 0;
    *screen_x = (float)viewport->X +
        (1.0f + ndc_x) * (float)viewport->Width * 0.5f;
    *screen_y = (float)viewport->Y +
        (1.0f - ndc_y) * (float)viewport->Height * 0.5f;
    *screen_depth = ndc_z;
    if (*screen_depth < 0.0f) *screen_depth = 0.0f;
    if (*screen_depth > 1.0f) *screen_depth = 1.0f;
    return _finite(*screen_x) && _finite(*screen_y);
}

/* TK17's pivot helper returns OpenGL-style view coordinates (visible points
   have negative Z), even while Hook5 presents through D3D8.  Some Hook5
   projection states therefore cannot consume those coordinates directly.
   This fallback performs the same 45-degree projection used by the prototype
   D3D11 path and produces explicit screen coordinates. */
static int liquid_project_d3d_view_point_fallback(
    const D3DVIEWPORT8 *viewport, const float point[3],
    float *screen_x, float *screen_y, float *screen_depth)
{
    const float tan_half_fov = 0.41421356f;
    float depth;
    float aspect;
    float ndc_x, ndc_y;
    if (!viewport || !point || !screen_x || !screen_y || !screen_depth ||
        !viewport->Width || !viewport->Height) return 0;
    depth = -point[2];
    if (!_finite(depth) || depth <= 0.01f) return 0;
    aspect = (float)viewport->Width / (float)viewport->Height;
    ndc_x = point[0] / (depth * tan_half_fov * aspect);
    ndc_y = point[1] / (depth * tan_half_fov);
    if (!_finite(ndc_x) || !_finite(ndc_y) ||
        ndc_x < -2.0f || ndc_x > 2.0f ||
        ndc_y < -2.0f || ndc_y > 2.0f) return 0;
    *screen_x = (float)viewport->X +
        (1.0f + ndc_x) * (float)viewport->Width * 0.5f;
    *screen_y = (float)viewport->Y +
        (1.0f - ndc_y) * (float)viewport->Height * 0.5f;
    /* The fallback is used only when Hook5's retained projection cannot be
       consumed. Keep it near the far plane so foreground scene geometry
       still wins instead of turning the liquid back into an overlay. */
    *screen_depth = 0.9995f;
    return 1;
}

static int liquid_ensure_d3d8_stream_texture(IDirect3DDevice8 *device)
{
    D3DLOCKED_RECT locked;
    HRESULT result;
    int y;
    if (!device) return 0;
    if (liquid_d3d8_stream_texture &&
        liquid_d3d8_texture_device == device) return 1;
    if (liquid_d3d8_stream_texture) {
        IDirect3DTexture8_Release(liquid_d3d8_stream_texture);
        liquid_d3d8_stream_texture = NULL;
    }
    liquid_d3d8_texture_device = device;
    result = IDirect3DDevice8_CreateTexture(
        device, 64, 64, 1, 0, D3DFMT_A8R8G8B8,
        D3DPOOL_MANAGED, &liquid_d3d8_stream_texture);
    if (FAILED(result) || !liquid_d3d8_stream_texture) {
        log_line("liquid Hook5 scene texture creation failed result=0x%08lX",
                 (unsigned long)result);
        liquid_d3d8_stream_texture = NULL;
        return 0;
    }
    memset(&locked, 0, sizeof(locked));
    result = IDirect3DTexture8_LockRect(
        liquid_d3d8_stream_texture, 0, &locked, NULL, 0);
    if (FAILED(result) || !locked.pBits) {
        log_line("liquid Hook5 scene texture lock failed result=0x%08lX",
                 (unsigned long)result);
        IDirect3DTexture8_Release(liquid_d3d8_stream_texture);
        liquid_d3d8_stream_texture = NULL;
        return 0;
    }
    for (y = 0; y < 64; ++y) {
        DWORD *row = (DWORD *)((BYTE *)locked.pBits + y * locked.Pitch);
        int x;
        float lifetime_alpha = 1.0f - (float)y / 63.0f;
        for (x = 0; x < 64; ++x) {
            float lateral = ((float)x / 63.0f) * 2.0f - 1.0f;
            float cross_alpha = 1.0f - lateral * lateral;
            float opacity;
            int alpha;
            if (cross_alpha < 0.0f) cross_alpha = 0.0f;
            cross_alpha = cross_alpha * cross_alpha;
            opacity = cross_alpha * lifetime_alpha;
            alpha = (int)(opacity * 255.0f + 0.5f);
            if (alpha < 0) alpha = 0;
            if (alpha > 255) alpha = 255;
            row[x] = D3DCOLOR_ARGB(alpha, 250, 252, 240);
        }
    }
    IDirect3DTexture8_UnlockRect(liquid_d3d8_stream_texture, 0);
    log_line("liquid Hook5 textured D3D8 scene geometry initialized device=%p texture=%p",
             device, liquid_d3d8_stream_texture);
    return 1;
}

static void liquid_append_d3d8_scene_band(
    liquid_vertex_t *vertices, int *vertex_count,
    const float start[3], const float end[3],
    float start_offset_x, float start_offset_y,
    float end_offset_x, float end_offset_y,
    float side_a, float side_b, float alpha)
{
    liquid_vertex_t *v = &vertices[*vertex_count];
#define SET_SCENE_VERTEX(V, P, OX, OY, SIDE, ALPHA) \
    do { \
        (V).x = (P)[0] + (OX) * (SIDE); \
        (V).y = (P)[1] + (OY) * (SIDE); \
        (V).z = (P)[2]; \
        (V).nx = 0.0f; (V).ny = 0.0f; (V).nz = -1.0f; \
        (V).u = ((SIDE) + 1.0f) * 0.5f; \
        (V).v = 1.0f - (ALPHA); \
    } while (0)
    SET_SCENE_VERTEX(v[0], start, start_offset_x, start_offset_y,
                     side_a, alpha);
    SET_SCENE_VERTEX(v[1], end, end_offset_x, end_offset_y,
                     side_a, alpha);
    SET_SCENE_VERTEX(v[2], end, end_offset_x, end_offset_y,
                     side_b, alpha);
    v[3] = v[0];
    v[4] = v[2];
    SET_SCENE_VERTEX(v[5], start, start_offset_x, start_offset_y,
                     side_b, alpha);
#undef SET_SCENE_VERTEX
    *vertex_count += 6;
}

static void liquid_draw_particles(IDirect3DDevice8 *device)
{
    static liquid_vertex_t vertices[LIQUID_PARTICLE_CAP * 18];
    D3DMATRIX world;
    D3DMATRIX view_matrix;
    D3DMATRIX identity;
    D3DMATRIX projection;
    D3DVIEWPORT8 viewport;
    DWORD old_vertex_shader = 0;
    DWORD old_z_enable = TRUE;
    DWORD old_z_write = TRUE;
    DWORD old_z_func = D3DCMP_LESSEQUAL;
    DWORD old_alpha_blend = FALSE;
    DWORD old_alpha_test = FALSE;
    DWORD old_fog = FALSE;
    DWORD old_src_blend = D3DBLEND_ONE;
    DWORD old_dst_blend = D3DBLEND_ZERO;
    DWORD old_lighting = TRUE;
    DWORD old_cull = D3DCULL_CCW;
    DWORD old_color_op = D3DTOP_DISABLE;
    DWORD old_color_arg1 = D3DTA_TEXTURE;
    DWORD old_alpha_op = D3DTOP_DISABLE;
    DWORD old_alpha_arg1 = D3DTA_TEXTURE;
    DWORD old_address_u = D3DTADDRESS_WRAP;
    DWORD old_address_v = D3DTADDRESS_WRAP;
    DWORD old_mag_filter = D3DTEXF_POINT;
    DWORD old_min_filter = D3DTEXF_POINT;
    D3DMATERIAL8 old_material;
    D3DMATERIAL8 material;
    IDirect3DBaseTexture8 *old_texture = NULL;
    int vertex_count = 0;
    int i;
    if (!device || !cfg.liquids_enabled ||
        !liquid_has_active_particles()) return;
    memset(&projection, 0, sizeof(projection));
    memset(&viewport, 0, sizeof(viewport));
    if (FAILED(IDirect3DDevice8_GetTransform(
            device, D3DTS_PROJECTION, &projection)) ||
        FAILED(IDirect3DDevice8_GetViewport(device, &viewport)) ||
        !viewport.Width || !viewport.Height) return;
    for (i = 0; i < cfg.particle_limit; i++) {
        liquid_particle_t *particle = &liquid_particles[i];
        float view[3];
        float previous_view[3];
        float x0, y0, z0, x1, y1, z1;
        float dx, dy, length, nx, ny, half_width;
        float depth0, depth1;
        float pixel_to_view_x0, pixel_to_view_y0;
        float pixel_to_view_x1, pixel_to_view_y1;
        float offset_x0, offset_y0, offset_x1, offset_y1;
        float alpha_fraction;
        if (!particle->active ||
            !liquid_world_to_view_point(particle->position, view) ||
            !liquid_world_to_view_point(particle->previous, previous_view))
            continue;
        if ((!liquid_project_d3d_view_point(
                 &projection, &viewport, view, &x1, &y1, &z1) ||
             !liquid_project_d3d_view_point(
                 &projection, &viewport, previous_view, &x0, &y0, &z0)) &&
            (!liquid_project_d3d_view_point_fallback(
                 &viewport, view, &x1, &y1, &z1) ||
             !liquid_project_d3d_view_point_fallback(
                 &viewport, previous_view, &x0, &y0, &z0))) continue;
        dx = x1 - x0;
        dy = y1 - y0;
        length = (float)sqrt((double)(dx * dx + dy * dy));
        if (length < 3.0f) {
            y0 -= 3.0f;
            dx = x1 - x0;
            dy = y1 - y0;
            length = (float)sqrt((double)(dx * dx + dy * dy));
        }
        if (length < 0.000001f) continue;
        nx = -dy / length;
        ny = dx / length;
        half_width = (particle->collided ? cfg.contact_droplet_size :
                      cfg.particle_size) * 220.0f;
        if (half_width < 1.0f) half_width = 1.0f;
        if (half_width > 12.0f) half_width = 12.0f;
        depth0 = -previous_view[2];
        depth1 = -view[2];
        if (depth0 <= 0.01f || depth1 <= 0.01f ||
            fabs(projection._11) < 0.001f ||
            fabs(projection._22) < 0.001f) continue;
        pixel_to_view_x0 = depth0 * 2.0f /
            ((float)viewport.Width * (float)fabs(projection._11));
        pixel_to_view_y0 = depth0 * 2.0f /
            ((float)viewport.Height * (float)fabs(projection._22));
        pixel_to_view_x1 = depth1 * 2.0f /
            ((float)viewport.Width * (float)fabs(projection._11));
        pixel_to_view_y1 = depth1 * 2.0f /
            ((float)viewport.Height * (float)fabs(projection._22));
        offset_x0 = nx * half_width * pixel_to_view_x0;
        offset_y0 = -ny * half_width * pixel_to_view_y0;
        offset_x1 = nx * half_width * pixel_to_view_x1;
        offset_y1 = -ny * half_width * pixel_to_view_y1;
        alpha_fraction = 1.0f - particle->age / particle->lifetime;
        if (alpha_fraction < 0.0f) alpha_fraction = 0.0f;
        if (alpha_fraction > 1.0f) alpha_fraction = 1.0f;
        alpha_fraction *= particle->opacity *
            (particle->collided ? cfg.contact_droplet_opacity :
             cfg.stream_opacity);
        liquid_append_d3d8_scene_band(
            vertices, &vertex_count, previous_view, view,
            offset_x0, offset_y0, offset_x1, offset_y1,
            1.0f, 0.32f, alpha_fraction);
        liquid_append_d3d8_scene_band(
            vertices, &vertex_count, previous_view, view,
            offset_x0, offset_y0, offset_x1, offset_y1,
            0.32f, -0.32f, alpha_fraction);
        liquid_append_d3d8_scene_band(
            vertices, &vertex_count, previous_view, view,
            offset_x0, offset_y0, offset_x1, offset_y1,
            -0.32f, -1.0f, alpha_fraction);
    }
    if (!vertex_count || !liquid_ensure_d3d8_stream_texture(device)) return;

    memset(&world, 0, sizeof(world));
    memset(&view_matrix, 0, sizeof(view_matrix));
    IDirect3DDevice8_GetTransform(device, D3DTS_WORLD, &world);
    IDirect3DDevice8_GetTransform(device, D3DTS_VIEW, &view_matrix);
    IDirect3DDevice8_GetVertexShader(device, &old_vertex_shader);
    IDirect3DDevice8_GetRenderState(device, D3DRS_ZENABLE, &old_z_enable);
    IDirect3DDevice8_GetRenderState(device, D3DRS_ZWRITEENABLE, &old_z_write);
    IDirect3DDevice8_GetRenderState(device, D3DRS_ZFUNC, &old_z_func);
    IDirect3DDevice8_GetRenderState(device, D3DRS_ALPHABLENDENABLE,
                                    &old_alpha_blend);
    IDirect3DDevice8_GetRenderState(device, D3DRS_ALPHATESTENABLE,
                                    &old_alpha_test);
    IDirect3DDevice8_GetRenderState(device, D3DRS_FOGENABLE, &old_fog);
    IDirect3DDevice8_GetRenderState(device, D3DRS_SRCBLEND, &old_src_blend);
    IDirect3DDevice8_GetRenderState(device, D3DRS_DESTBLEND, &old_dst_blend);
    IDirect3DDevice8_GetRenderState(device, D3DRS_LIGHTING, &old_lighting);
    IDirect3DDevice8_GetRenderState(device, D3DRS_CULLMODE, &old_cull);
    IDirect3DDevice8_GetTextureStageState(
        device, 0, D3DTSS_COLOROP, &old_color_op);
    IDirect3DDevice8_GetTextureStageState(
        device, 0, D3DTSS_COLORARG1, &old_color_arg1);
    IDirect3DDevice8_GetTextureStageState(
        device, 0, D3DTSS_ALPHAOP, &old_alpha_op);
    IDirect3DDevice8_GetTextureStageState(
        device, 0, D3DTSS_ALPHAARG1, &old_alpha_arg1);
    IDirect3DDevice8_GetTextureStageState(
        device, 0, D3DTSS_ADDRESSU, &old_address_u);
    IDirect3DDevice8_GetTextureStageState(
        device, 0, D3DTSS_ADDRESSV, &old_address_v);
    IDirect3DDevice8_GetTextureStageState(
        device, 0, D3DTSS_MAGFILTER, &old_mag_filter);
    IDirect3DDevice8_GetTextureStageState(
        device, 0, D3DTSS_MINFILTER, &old_min_filter);
    memset(&old_material, 0, sizeof(old_material));
    IDirect3DDevice8_GetMaterial(device, &old_material);
    IDirect3DDevice8_GetTexture(device, 0, &old_texture);

    liquid_identity_matrix(&identity);
    IDirect3DDevice8_SetTransform(device, D3DTS_WORLD, &identity);
    IDirect3DDevice8_SetTransform(device, D3DTS_VIEW, &identity);
    IDirect3DDevice8_SetVertexShader(
        device, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
    IDirect3DDevice8_SetTexture(
        device, 0, (IDirect3DBaseTexture8 *)liquid_d3d8_stream_texture);
    IDirect3DDevice8_SetTextureStageState(
        device, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    IDirect3DDevice8_SetTextureStageState(
        device, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    IDirect3DDevice8_SetTextureStageState(
        device, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    IDirect3DDevice8_SetTextureStageState(
        device, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    IDirect3DDevice8_SetTextureStageState(
        device, 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    IDirect3DDevice8_SetTextureStageState(
        device, 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    IDirect3DDevice8_SetTextureStageState(
        device, 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    IDirect3DDevice8_SetTextureStageState(
        device, 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    memset(&material, 0, sizeof(material));
    material.Diffuse.r = material.Diffuse.g = material.Diffuse.b = 1.0f;
    material.Diffuse.a = 1.0f;
    material.Ambient = material.Diffuse;
    IDirect3DDevice8_SetMaterial(device, &material);
    IDirect3DDevice8_SetRenderState(device, D3DRS_ZENABLE, D3DZB_TRUE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_ZWRITEENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    IDirect3DDevice8_SetRenderState(device, D3DRS_ALPHABLENDENABLE, TRUE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_ALPHATESTENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_FOGENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    IDirect3DDevice8_SetRenderState(device, D3DRS_DESTBLEND,
                                    D3DBLEND_INVSRCALPHA);
    IDirect3DDevice8_SetRenderState(device, D3DRS_LIGHTING, FALSE);
    IDirect3DDevice8_SetRenderState(device, D3DRS_CULLMODE, D3DCULL_NONE);
    {
        HRESULT draw_result = IDirect3DDevice8_DrawPrimitiveUP(
            device, D3DPT_TRIANGLELIST, (UINT)(vertex_count / 3),
            vertices, sizeof(vertices[0]));
        static int draw_logged;
        if (!draw_logged) {
            draw_logged = 1;
            log_line("liquid Hook5 textured scene draw result=0x%08lX vertices=%d viewport=%ux%u projection34=%.4f projection44=%.4f",
                     (unsigned long)draw_result, vertex_count,
                     (unsigned)viewport.Width, (unsigned)viewport.Height,
                     projection._34, projection._44);
        }
    }

    IDirect3DDevice8_SetTexture(device, 0, old_texture);
    if (old_texture) IDirect3DBaseTexture8_Release(old_texture);
    IDirect3DDevice8_SetTextureStageState(
        device, 0, D3DTSS_COLOROP, old_color_op);
    IDirect3DDevice8_SetTextureStageState(
        device, 0, D3DTSS_COLORARG1, old_color_arg1);
    IDirect3DDevice8_SetTextureStageState(
        device, 0, D3DTSS_ALPHAOP, old_alpha_op);
    IDirect3DDevice8_SetTextureStageState(
        device, 0, D3DTSS_ALPHAARG1, old_alpha_arg1);
    IDirect3DDevice8_SetTextureStageState(
        device, 0, D3DTSS_ADDRESSU, old_address_u);
    IDirect3DDevice8_SetTextureStageState(
        device, 0, D3DTSS_ADDRESSV, old_address_v);
    IDirect3DDevice8_SetTextureStageState(
        device, 0, D3DTSS_MAGFILTER, old_mag_filter);
    IDirect3DDevice8_SetTextureStageState(
        device, 0, D3DTSS_MINFILTER, old_min_filter);
    IDirect3DDevice8_SetMaterial(device, &old_material);
    IDirect3DDevice8_SetVertexShader(device, old_vertex_shader);
    IDirect3DDevice8_SetTransform(device, D3DTS_VIEW, &view_matrix);
    IDirect3DDevice8_SetTransform(device, D3DTS_WORLD, &world);
    IDirect3DDevice8_SetRenderState(device, D3DRS_ZENABLE, old_z_enable);
    IDirect3DDevice8_SetRenderState(device, D3DRS_ZWRITEENABLE, old_z_write);
    IDirect3DDevice8_SetRenderState(device, D3DRS_ZFUNC, old_z_func);
    IDirect3DDevice8_SetRenderState(device, D3DRS_ALPHABLENDENABLE,
                                    old_alpha_blend);
    IDirect3DDevice8_SetRenderState(device, D3DRS_ALPHATESTENABLE,
                                    old_alpha_test);
    IDirect3DDevice8_SetRenderState(device, D3DRS_FOGENABLE, old_fog);
    IDirect3DDevice8_SetRenderState(device, D3DRS_SRCBLEND, old_src_blend);
    IDirect3DDevice8_SetRenderState(device, D3DRS_DESTBLEND, old_dst_blend);
    IDirect3DDevice8_SetRenderState(device, D3DRS_LIGHTING, old_lighting);
    IDirect3DDevice8_SetRenderState(device, D3DRS_CULLMODE, old_cull);
}

static int liquid_resolve_gl_api(void)
{
    HMODULE gl;
    if (liquid_gl_api_attempted) return liquid_gl_begin != NULL;
    liquid_gl_api_attempted = 1;
    gl = GetModuleHandleA("opengl32.dll");
    if (!gl) return 0;
#define LOAD_LIQUID_GL(NAME, TYPE, SYMBOL) \
    liquid_gl_##NAME = (TYPE)GetProcAddress(gl, SYMBOL)
    LOAD_LIQUID_GL(begin, liquid_gl_begin_t, "glBegin");
    LOAD_LIQUID_GL(end, liquid_gl_end_t, "glEnd");
    LOAD_LIQUID_GL(vertex2f, liquid_gl_vertex2f_t, "glVertex2f");
    LOAD_LIQUID_GL(color4ub, liquid_gl_color4ub_t, "glColor4ub");
    LOAD_LIQUID_GL(get_floatv, liquid_gl_get_floatv_t, "glGetFloatv");
    LOAD_LIQUID_GL(get_integerv, liquid_gl_get_integerv_t, "glGetIntegerv");
    LOAD_LIQUID_GL(push_attrib, liquid_gl_push_attrib_t, "glPushAttrib");
    LOAD_LIQUID_GL(pop_attrib, liquid_gl_pop_attrib_t, "glPopAttrib");
    LOAD_LIQUID_GL(disable, liquid_gl_disable_t, "glDisable");
    LOAD_LIQUID_GL(enable, liquid_gl_enable_t, "glEnable");
    LOAD_LIQUID_GL(blend_func, liquid_gl_blend_func_t, "glBlendFunc");
    LOAD_LIQUID_GL(matrix_mode, liquid_gl_matrix_mode_t, "glMatrixMode");
    LOAD_LIQUID_GL(push_matrix, liquid_gl_push_matrix_t, "glPushMatrix");
    LOAD_LIQUID_GL(pop_matrix, liquid_gl_pop_matrix_t, "glPopMatrix");
    LOAD_LIQUID_GL(load_identity, liquid_gl_load_identity_t, "glLoadIdentity");
    LOAD_LIQUID_GL(ortho, liquid_gl_ortho_t, "glOrtho");
#undef LOAD_LIQUID_GL
    return liquid_gl_begin && liquid_gl_end && liquid_gl_vertex2f &&
           liquid_gl_color4ub && liquid_gl_get_floatv &&
           liquid_gl_get_integerv && liquid_gl_push_attrib &&
           liquid_gl_pop_attrib && liquid_gl_disable &&
           liquid_gl_enable && liquid_gl_blend_func &&
           liquid_gl_matrix_mode && liquid_gl_push_matrix &&
           liquid_gl_pop_matrix && liquid_gl_load_identity &&
           liquid_gl_ortho;
}

static int liquid_project_gl_view_point(const GLfloat projection[16],
                                        const GLint viewport[4],
                                        const float point[3],
                                        float *screen_x, float *screen_y)
{
    float clip_x, clip_y, clip_w;
    if (!projection || !viewport || !point || !screen_x || !screen_y)
        return 0;
    clip_x = projection[0] * point[0] + projection[4] * point[1] +
             projection[8] * point[2] + projection[12];
    clip_y = projection[1] * point[0] + projection[5] * point[1] +
             projection[9] * point[2] + projection[13];
    clip_w = projection[3] * point[0] + projection[7] * point[1] +
             projection[11] * point[2] + projection[15];
    if (!_finite(clip_w) || clip_w <= 0.00001f) return 0;
    *screen_x = viewport[0] +
        (1.0f + clip_x / clip_w) * viewport[2] * 0.5f;
    *screen_y = viewport[1] +
        (1.0f - clip_y / clip_w) * viewport[3] * 0.5f;
    return _finite(*screen_x) && _finite(*screen_y);
}

static void liquid_draw_particles_opengl(void)
{
    GLfloat projection[16];
    GLint viewport[4];
    GLint old_matrix_mode = GL_MODELVIEW;
    int i;
    int drawn = 0;
    if (!cfg.liquids_enabled || !liquid_has_active_particles() ||
        !liquid_resolve_gl_api()) return;
    liquid_gl_get_floatv(GL_PROJECTION_MATRIX, projection);
    liquid_gl_get_integerv(GL_VIEWPORT, viewport);
    liquid_gl_get_integerv(GL_MATRIX_MODE, &old_matrix_mode);
    if (viewport[2] <= 0 || viewport[3] <= 0) return;

    liquid_gl_push_attrib(GL_ALL_ATTRIB_BITS);
    liquid_gl_disable(GL_DEPTH_TEST);
    liquid_gl_disable(GL_TEXTURE_2D);
    liquid_gl_disable(GL_LIGHTING);
    liquid_gl_enable(GL_BLEND);
    liquid_gl_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    liquid_gl_matrix_mode(GL_PROJECTION);
    liquid_gl_push_matrix();
    liquid_gl_load_identity();
    liquid_gl_ortho(0.0, viewport[2], viewport[3], 0.0, -1.0, 1.0);
    liquid_gl_matrix_mode(GL_MODELVIEW);
    liquid_gl_push_matrix();
    liquid_gl_load_identity();
    liquid_gl_begin(GL_TRIANGLES);
    for (i = 0; i < cfg.particle_limit; i++) {
        liquid_particle_t *particle = &liquid_particles[i];
        float view[3];
        float previous_view[3];
        float x0, y0, x1, y1;
        float dx, dy, length, nx, ny;
        float half_width;
        float alpha_fraction;
        GLubyte alpha;
        if (!particle->active ||
            !liquid_world_to_view_point(particle->position, view) ||
            !liquid_world_to_view_point(particle->previous, previous_view) ||
            !liquid_project_gl_view_point(projection, viewport,
                                          view, &x1, &y1) ||
            !liquid_project_gl_view_point(projection, viewport,
                                          previous_view, &x0, &y0)) continue;
        dx = x1 - x0;
        dy = y1 - y0;
        length = (float)sqrt((double)(dx * dx + dy * dy));
        if (length < 3.0f) {
            y0 -= 3.0f;
            dx = x1 - x0;
            dy = y1 - y0;
            length = (float)sqrt((double)(dx * dx + dy * dy));
        }
        if (length < 0.0001f) continue;
        half_width = (particle->collided ? cfg.contact_droplet_size :
                      cfg.particle_size) * 220.0f;
        if (half_width < 1.0f) half_width = 1.0f;
        if (half_width > 12.0f) half_width = 12.0f;
        nx = -dy / length * half_width;
        ny = dx / length * half_width;
        alpha_fraction = 1.0f - particle->age / particle->lifetime;
        if (alpha_fraction < 0.0f) alpha_fraction = 0.0f;
        if (alpha_fraction > 1.0f) alpha_fraction = 1.0f;
        if (particle->collided)
            alpha = (GLubyte)((80.0f + alpha_fraction * 170.0f) *
                              particle->opacity *
                              cfg.contact_droplet_opacity);
        else
            alpha = (GLubyte)(80.0f + alpha_fraction * 170.0f);
        liquid_gl_color4ub(246, 246, 232, alpha);
        liquid_gl_vertex2f(x0 + nx, y0 + ny);
        liquid_gl_vertex2f(x1 + nx, y1 + ny);
        liquid_gl_vertex2f(x1 - nx, y1 - ny);
        liquid_gl_vertex2f(x0 + nx, y0 + ny);
        liquid_gl_vertex2f(x1 - nx, y1 - ny);
        liquid_gl_vertex2f(x0 - nx, y0 - ny);
        drawn++;
    }
    liquid_gl_end();
    liquid_gl_pop_matrix();
    liquid_gl_matrix_mode(GL_PROJECTION);
    liquid_gl_pop_matrix();
    liquid_gl_matrix_mode((GLenum)old_matrix_mode);
    liquid_gl_pop_attrib();
    if (drawn) {
        static int draw_logged;
        if (!draw_logged) {
            draw_logged = 1;
            log_line("liquid prototype OpenGL draw active particles=%d viewport=%dx%d",
                     drawn, viewport[2], viewport[3]);
        }
    }
}

static void liquid_native_gl_frame(void);
static FARPROC liquid_native_gl_proc(const char *name, FARPROC result);
static IDirect3D8 *WINAPI hook_liquid_Direct3DCreate8(UINT sdk_version);
static FARPROC liquid_wrap_d3d8_factory(FARPROC factory);

/* Different engine modules may already have different plugin wrappers in
   their IAT. A single global 'previous' silently bypasses those wrappers. */
typedef struct liquid_import_chain_t {
    HMODULE module;
    void *hook;
    void *previous;
} liquid_import_chain_t;
static liquid_import_chain_t liquid_import_chains[128];
static volatile LONG liquid_import_chain_count;

static void *liquid_import_previous_for_module(HMODULE module, void *hook, void *fallback)
{
    LONG i, count = InterlockedCompareExchange(&liquid_import_chain_count, 0, 0);
    for (i = 0; i < count; i++)
        if (liquid_import_chains[i].module == module && liquid_import_chains[i].hook == hook)
            return liquid_import_chains[i].previous;
    return fallback;
}

static void *liquid_import_previous_for_caller(const void *caller, void *hook, void *fallback)
{
    MEMORY_BASIC_INFORMATION info;
    if (!VirtualQuery(caller, &info, sizeof(info))) return fallback;
    return liquid_import_previous_for_module((HMODULE)info.AllocationBase, hook, fallback);
}

static BOOL WINAPI hook_liquid_SwapBuffers(HDC hdc)
{
    swap_buffers_t previous = (swap_buffers_t)liquid_import_previous_for_caller(
        __builtin_return_address(0), hook_liquid_SwapBuffers, (void*)real_SwapBuffers);
    static int callback_logged;
    if (!callback_logged) {
        callback_logged = 1;
        log_line("liquid OpenGL SwapBuffers callback active hdc=%p", hdc);
    }
    liquid_native_gl_frame();
    return previous ? previous(hdc) : FALSE;
}

static FARPROC WINAPI hook_liquid_GetProcAddress(HMODULE module,
                                                  LPCSTR name)
{
    FARPROC result;
    get_proc_address_t previous = (get_proc_address_t)liquid_import_previous_for_caller(
        __builtin_return_address(0), hook_liquid_GetProcAddress, (void*)real_GetProcAddress);
    char module_name[MAX_PATH * 2];
    result = previous ? previous(module, name) : NULL;
    if (!module || !name || (ULONG_PTR)name <= 0xffffu) return result;
    module_name[0] = 0;
    GetModuleFileNameA(module, module_name, sizeof(module_name));
    if (contains_i(module_name, "opengl32.dll"))
        return liquid_native_gl_proc(name, result);
    if (contains_i(module_name, "d3d8.dll") && strcmp(name, "Direct3DCreate8") == 0) {
        return liquid_wrap_d3d8_factory(result);
    }
    if (contains_i(module_name, "gdi32.dll") &&
        strcmp(name, "SwapBuffers") == 0) {
        if (!real_SwapBuffers && result &&
            result != (FARPROC)hook_liquid_SwapBuffers) {
            real_SwapBuffers = (swap_buffers_t)result;
        }
        return (FARPROC)hook_liquid_SwapBuffers;
    }
    return result;
}

static void liquid_release_collision_depth_resources(void)
{
    int i;
    for (i = 0; i < LIQUID_COLLISION_READBACK_COUNT; i++) {
        if (liquid_collision_depth_staging[i])
            ID3D11Texture2D_Release(liquid_collision_depth_staging[i]);
        liquid_collision_depth_staging[i] = NULL;
        memset(&liquid_collision_depth_snapshots[i], 0,
               sizeof(liquid_collision_depth_snapshots[i]));
    }
    if (liquid_collision_depth_constant_buffer)
        ID3D11Buffer_Release(liquid_collision_depth_constant_buffer);
    if (liquid_collision_depth_compute_shader)
        ID3D11ComputeShader_Release(liquid_collision_depth_compute_shader);
    if (liquid_collision_depth_float_view)
        ID3D11UnorderedAccessView_Release(liquid_collision_depth_float_view);
    if (liquid_collision_depth_float_texture)
        ID3D11Texture2D_Release(liquid_collision_depth_float_texture);
    if (liquid_collision_depth_source_view)
        ID3D11ShaderResourceView_Release(liquid_collision_depth_source_view);
    if (liquid_collision_depth_source_resource)
        ID3D11Resource_Release(liquid_collision_depth_source_resource);
    if (liquid_collision_depth_device)
        ID3D11Device_Release(liquid_collision_depth_device);
    liquid_collision_depth_constant_buffer = NULL;
    liquid_collision_depth_compute_shader = NULL;
    liquid_collision_depth_float_view = NULL;
    liquid_collision_depth_float_texture = NULL;
    liquid_collision_depth_source_view = NULL;
    liquid_collision_depth_source_resource = NULL;
    liquid_collision_depth_device = NULL;
    liquid_collision_depth_format = DXGI_FORMAT_UNKNOWN;
    liquid_collision_depth_source_width = 0;
    liquid_collision_depth_source_height = 0;
    liquid_collision_depth_write_index = 0;
    liquid_collision_depth_ready_count = 0;
}

static void liquid_release_d3d11_resources(void)
{
    liquid_release_collision_depth_resources();
    if (liquid_d3d11_depth_test_state)
        ID3D11DepthStencilState_Release(liquid_d3d11_depth_test_state);
    if (liquid_d3d11_depth_state)
        ID3D11DepthStencilState_Release(liquid_d3d11_depth_state);
    if (liquid_d3d11_rasterizer_state)
        ID3D11RasterizerState_Release(liquid_d3d11_rasterizer_state);
    if (liquid_d3d11_blend_state)
        ID3D11BlendState_Release(liquid_d3d11_blend_state);
    if (liquid_d3d11_vertex_buffer)
        ID3D11Buffer_Release(liquid_d3d11_vertex_buffer);
    if (liquid_d3d11_input_layout)
        ID3D11InputLayout_Release(liquid_d3d11_input_layout);
    if (liquid_d3d11_pixel_shader)
        ID3D11PixelShader_Release(liquid_d3d11_pixel_shader);
    if (liquid_d3d11_vertex_shader)
        ID3D11VertexShader_Release(liquid_d3d11_vertex_shader);
    if (liquid_d3d11_device)
        ID3D11Device_Release(liquid_d3d11_device);
    liquid_d3d11_depth_test_state = NULL;
    liquid_d3d11_depth_state = NULL;
    liquid_d3d11_rasterizer_state = NULL;
    liquid_d3d11_blend_state = NULL;
    liquid_d3d11_vertex_buffer = NULL;
    liquid_d3d11_input_layout = NULL;
    liquid_d3d11_pixel_shader = NULL;
    liquid_d3d11_vertex_shader = NULL;
    liquid_d3d11_device = NULL;
    liquid_d3d11_depth_test_draw_layer = -999;
}

static int liquid_ensure_d3d11_resources(ID3D11Device *device)
{
    static const char vertex_shader_source[] =
        "struct VIn{float3 p:POSITION;float2 uv:TEXCOORD0;float4 c:COLOR0;};"
        "struct VOut{float4 p:SV_POSITION;float2 uv:TEXCOORD0;float4 c:COLOR0;};"
        "VOut main(VIn i){VOut o;o.p=float4(i.p,1);o.uv=i.uv;o.c=i.c;return o;}";
    static const char pixel_shader_source[] =
        "struct PIn{float4 p:SV_POSITION;float2 uv:TEXCOORD0;float4 c:COLOR0;};"
        "float4 main(PIn i):SV_Target{"
        "float a=abs(i.uv.x);float b=abs(i.uv.y);"
        "float w=sqrt(saturate(1-a*a));"
        "float shape=1-smoothstep(max(0,w-0.18),w,b);"
        "float roundness=pow(saturate(1-b*b),0.38);"
        "float rim=smoothstep(0.62,0.98,b);"
        "float material=shape*(0.48+0.52*roundness);"
        "float coverage=smoothstep(0.025,0.18,material);"
        "float coreMask=smoothstep(0.20,0.52,material);"
        /* i.c.a is the complete per-particle alpha including
           stream_opacity. i.c.r is core_opacity. Unlike the old workaround,
           neither value is converted into near-opaque coverage. */
        "float visualAlpha=saturate(i.c.a);"
        "float coreDensity=0.35+0.65*saturate(i.c.r);"
        /* Green carries absolute base opacity independently of random drop
           alpha and core density. Other primitives leave this channel zero. */
        "float baseAlpha=saturate(i.c.g);"
        "float shellAlpha=coverage*saturate(visualAlpha+baseAlpha)*0.35;"
        "float coreAlpha=coreMask*saturate(visualAlpha*coreDensity+baseAlpha);"
        "float alpha=max(shellAlpha,coreAlpha);"
        "clip(alpha-0.0005);"
        "float sheen=(1-smoothstep(0.015,0.24,abs(i.uv.y+0.31)))*0.30;"
        "float soft=(1-smoothstep(0.05,0.55,abs(i.uv.y-0.12)))*0.07;"
        "float grain=sin(i.p.x*0.071+i.p.y*0.047)*0.012;"
        "float3 liquidColor=float3(0.92,0.94,0.89);"
        "float density=saturate(0.72+0.28*i.c.r);"
        "float3 base=liquidColor*density*(0.82+0.18*roundness-0.18*rim)+sheen+soft+grain;"
        /* Premultiplied output keeps the soft shell stable at low alpha and
           avoids color fringes when Hook5 composites it over unlike scene
           materials. */
        "return float4(saturate(base)*alpha,alpha);}";
    static const D3D11_INPUT_ELEMENT_DESC input_elements[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20,
         D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    HMODULE compiler = NULL;
    pD3DCompile compile_shader = NULL;
    ID3DBlob *vertex_blob = NULL;
    ID3DBlob *pixel_blob = NULL;
    ID3DBlob *errors = NULL;
    D3D11_BUFFER_DESC buffer_description;
    D3D11_BLEND_DESC blend_description;
    D3D11_RASTERIZER_DESC rasterizer_description;
    D3D11_DEPTH_STENCIL_DESC depth_description;
    HRESULT result;
    if (!device) return 0;
    if (liquid_d3d11_device == device && liquid_d3d11_vertex_shader &&
        liquid_d3d11_pixel_shader && liquid_d3d11_input_layout &&
        liquid_d3d11_vertex_buffer && liquid_d3d11_blend_state &&
        liquid_d3d11_rasterizer_state && liquid_d3d11_depth_state &&
        liquid_d3d11_depth_test_state) return 1;
    liquid_release_d3d11_resources();
    compiler = LoadLibraryA("d3dcompiler_47.dll");
    if (!compiler) compiler = LoadLibraryA("d3dcompiler_43.dll");
    if (compiler) copy_proc_address(&compile_shader,
                                    GetProcAddress(compiler, "D3DCompile"),
                                    sizeof(compile_shader));
    if (!compile_shader) {
        log_line("liquid D3D11 shader setup failed reason=D3DCompile-unavailable");
        if (compiler) FreeLibrary(compiler);
        return 0;
    }
    result = compile_shader(vertex_shader_source,
                            sizeof(vertex_shader_source) - 1,
                            "NC-TK17-Liquids-VS", NULL, NULL,
                            "main", "vs_4_0", D3DCOMPILE_ENABLE_STRICTNESS,
                            0, &vertex_blob, &errors);
    if (FAILED(result) || !vertex_blob) goto shader_failure;
    if (errors) { ID3D10Blob_Release(errors); errors = NULL; }
    result = compile_shader(pixel_shader_source,
                            sizeof(pixel_shader_source) - 1,
                            "NC-TK17-Liquids-PS", NULL, NULL,
                            "main", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS,
                            0, &pixel_blob, &errors);
    if (FAILED(result) || !pixel_blob) goto shader_failure;
    if (errors) { ID3D10Blob_Release(errors); errors = NULL; }
    result = ID3D11Device_CreateVertexShader(
        device, ID3D10Blob_GetBufferPointer(vertex_blob),
        ID3D10Blob_GetBufferSize(vertex_blob), NULL,
        &liquid_d3d11_vertex_shader);
    if (FAILED(result)) goto setup_failure;
    result = ID3D11Device_CreatePixelShader(
        device, ID3D10Blob_GetBufferPointer(pixel_blob),
        ID3D10Blob_GetBufferSize(pixel_blob), NULL,
        &liquid_d3d11_pixel_shader);
    if (FAILED(result)) goto setup_failure;
    result = ID3D11Device_CreateInputLayout(
        device, input_elements, ARRAYSIZE(input_elements),
        ID3D10Blob_GetBufferPointer(vertex_blob),
        ID3D10Blob_GetBufferSize(vertex_blob),
        &liquid_d3d11_input_layout);
    if (FAILED(result)) goto setup_failure;
    memset(&buffer_description, 0, sizeof(buffer_description));
    buffer_description.ByteWidth =
        sizeof(liquid_d3d11_vertex_t) * LIQUID_D3D11_VERTEX_CAP;
    buffer_description.Usage = D3D11_USAGE_DYNAMIC;
    buffer_description.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    result = ID3D11Device_CreateBuffer(device, &buffer_description, NULL,
                                       &liquid_d3d11_vertex_buffer);
    if (FAILED(result)) goto setup_failure;
    memset(&blend_description, 0, sizeof(blend_description));
    blend_description.RenderTarget[0].BlendEnable = TRUE;
    blend_description.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blend_description.RenderTarget[0].DestBlend =
        D3D11_BLEND_INV_SRC_ALPHA;
    blend_description.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend_description.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blend_description.RenderTarget[0].DestBlendAlpha =
        D3D11_BLEND_INV_SRC_ALPHA;
    blend_description.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    /* Hook5 carries private post-processing data in the scene target's alpha
       channel. The liquid contributes visible RGB only; preserving the
       destination alpha prevents later Hook5 passes from recoloring or
       segmenting the ribbon according to whatever material is behind it. */
    blend_description.RenderTarget[0].RenderTargetWriteMask =
        D3D11_COLOR_WRITE_ENABLE_RED |
        D3D11_COLOR_WRITE_ENABLE_GREEN |
        D3D11_COLOR_WRITE_ENABLE_BLUE;
    result = ID3D11Device_CreateBlendState(device, &blend_description,
                                           &liquid_d3d11_blend_state);
    if (FAILED(result)) goto setup_failure;
    memset(&rasterizer_description, 0, sizeof(rasterizer_description));
    rasterizer_description.FillMode = D3D11_FILL_SOLID;
    rasterizer_description.CullMode = D3D11_CULL_NONE;
    rasterizer_description.DepthClipEnable = TRUE;
    result = ID3D11Device_CreateRasterizerState(
        device, &rasterizer_description,
        &liquid_d3d11_rasterizer_state);
    if (FAILED(result)) goto setup_failure;
    memset(&depth_description, 0, sizeof(depth_description));
    depth_description.DepthEnable = FALSE;
    depth_description.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depth_description.DepthFunc = D3D11_COMPARISON_ALWAYS;
    result = ID3D11Device_CreateDepthStencilState(
        device, &depth_description, &liquid_d3d11_depth_state);
    if (FAILED(result)) goto setup_failure;
    depth_description.DepthEnable = TRUE;
    depth_description.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depth_description.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    result = ID3D11Device_CreateDepthStencilState(
        device, &depth_description, &liquid_d3d11_depth_test_state);
    if (FAILED(result)) goto setup_failure;
    liquid_d3d11_device = device;
    ID3D11Device_AddRef(liquid_d3d11_device);
    ID3D10Blob_Release(vertex_blob);
    ID3D10Blob_Release(pixel_blob);
    FreeLibrary(compiler);
    log_line("liquid D3D11 translucent ribbon renderer initialized device=%p",
             device);
    return 1;

shader_failure:
    if (errors && ID3D10Blob_GetBufferPointer(errors))
        log_line("liquid D3D11 shader compilation failed hr=0x%08lx message=\"%.500s\"",
                 (unsigned long)result,
                 (const char*)ID3D10Blob_GetBufferPointer(errors));
    else
        log_line("liquid D3D11 shader compilation failed hr=0x%08lx",
                 (unsigned long)result);
setup_failure:
    if (errors) ID3D10Blob_Release(errors);
    if (vertex_blob) ID3D10Blob_Release(vertex_blob);
    if (pixel_blob) ID3D10Blob_Release(pixel_blob);
    if (compiler) FreeLibrary(compiler);
    liquid_release_d3d11_resources();
    return 0;
}

static int liquid_match_d3d11_scene_depth_state(
    ID3D11Device *device, ID3D11DepthStencilState *scene_state)
{
    D3D11_DEPTH_STENCIL_DESC description;
    ID3D11DepthStencilState *replacement = NULL;
    ID3D11RasterizerState *rasterizer_replacement = NULL;
    D3D11_RASTERIZER_DESC rasterizer_description;
    D3D11_COMPARISON_FUNC comparison = D3D11_COMPARISON_LESS_EQUAL;
    int layer_delta;
    HRESULT result;
    if (!device) return 0;
    memset(&description, 0, sizeof(description));
    if (scene_state) {
        ID3D11DepthStencilState_GetDesc(scene_state, &description);
        if (description.DepthEnable &&
            description.DepthFunc != D3D11_COMPARISON_NEVER &&
            description.DepthFunc != D3D11_COMPARISON_ALWAYS)
            comparison = description.DepthFunc;
    }
    if (liquid_d3d11_depth_test_state &&
        liquid_d3d11_rasterizer_state &&
        liquid_d3d11_depth_test_func == comparison &&
        liquid_d3d11_depth_test_draw_layer == cfg.draw_layer) return 1;
    memset(&description, 0, sizeof(description));
    description.DepthEnable = TRUE;
    /* The liquid is transparent geometry. It must test against the scene's
       depth, but it must never modify that depth buffer: doing so lets the
       first ribbon triangle reject later overlapping liquid triangles and
       produces hard, background-shaped gaps in an otherwise continuous
       stream. */
    description.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    description.DepthFunc = comparison;
    result = ID3D11Device_CreateDepthStencilState(
        device, &description, &replacement);
    if (FAILED(result) || !replacement) {
        log_line("liquid D3D11 scene depth-state creation failed func=%u hr=0x%08lX",
                 (unsigned)comparison, (unsigned long)result);
        return 0;
    }
    memset(&rasterizer_description, 0, sizeof(rasterizer_description));
    rasterizer_description.FillMode = D3D11_FILL_SOLID;
    rasterizer_description.CullMode = D3D11_CULL_NONE;
    rasterizer_description.DepthClipEnable = TRUE;
    layer_delta = cfg.draw_layer >= 0 ? cfg.draw_layer : 0;
    /* draw_layer is only an equal/near-surface ordering preference. Keep a
       conservative bias so Hook5 materials whose stored depth differs by a
       few quantization steps cannot punch stripes through liquid which is
       physically in front. Truly closer geometry still wins the depth test. */
    rasterizer_description.DepthBias =
        (comparison == D3D11_COMPARISON_GREATER ||
         comparison == D3D11_COMPARISON_GREATER_EQUAL) ?
            layer_delta * 1024 : -layer_delta * 1024;
    result = ID3D11Device_CreateRasterizerState(
        device, &rasterizer_description, &rasterizer_replacement);
    if (FAILED(result) || !rasterizer_replacement) {
        log_line("liquid D3D11 draw-layer rasterizer creation failed layer=%d hr=0x%08lX",
                 cfg.draw_layer, (unsigned long)result);
        ID3D11DepthStencilState_Release(replacement);
        return 0;
    }
    if (liquid_d3d11_depth_test_state)
        ID3D11DepthStencilState_Release(liquid_d3d11_depth_test_state);
    if (liquid_d3d11_rasterizer_state)
        ID3D11RasterizerState_Release(liquid_d3d11_rasterizer_state);
    liquid_d3d11_depth_test_state = replacement;
    liquid_d3d11_rasterizer_state = rasterizer_replacement;
    liquid_d3d11_depth_test_func = comparison;
    liquid_d3d11_depth_test_draw_layer = cfg.draw_layer;
    log_line("liquid D3D11 scene depth matched func=%u draw_layer=%d depth_write=0 depth_bias=%d",
             (unsigned)comparison, cfg.draw_layer,
             rasterizer_description.DepthBias);
    return 1;
}

static void liquid_probe_d3d11_collision_depth(
    ID3D11DepthStencilView *depth_view)
{
    static int probe_logged;
    ID3D11Resource *resource = NULL;
    D3D11_RESOURCE_DIMENSION dimension = D3D11_RESOURCE_DIMENSION_UNKNOWN;
    D3D11_DEPTH_STENCIL_VIEW_DESC view_desc;
    D3D11_TEXTURE2D_DESC texture_desc;
    if (probe_logged || !depth_view) return;
    probe_logged = 1;
    memset(&view_desc, 0, sizeof(view_desc));
    memset(&texture_desc, 0, sizeof(texture_desc));
    ID3D11DepthStencilView_GetDesc(depth_view, &view_desc);
    ID3D11DepthStencilView_GetResource(depth_view, &resource);
    if (!resource) {
        log_line("liquid collision depth probe failed: DSV has no resource view_format=%u",
                 (unsigned)view_desc.Format);
        return;
    }
    ID3D11Resource_GetType(resource, &dimension);
    if (dimension == D3D11_RESOURCE_DIMENSION_TEXTURE2D) {
        ID3D11Texture2D_GetDesc((ID3D11Texture2D*)resource, &texture_desc);
        log_line("liquid collision depth probe ready view_format=%u texture_format=%u size=%ux%u samples=%u quality=%u usage=%u bind=0x%X cpu=0x%X misc=0x%X view_dimension=%u",
                 (unsigned)view_desc.Format,
                 (unsigned)texture_desc.Format,
                 texture_desc.Width, texture_desc.Height,
                 texture_desc.SampleDesc.Count,
                 texture_desc.SampleDesc.Quality,
                 (unsigned)texture_desc.Usage,
                 texture_desc.BindFlags,
                 texture_desc.CPUAccessFlags,
                 texture_desc.MiscFlags,
                 (unsigned)view_desc.ViewDimension);
    } else {
        log_line("liquid collision depth probe unsupported resource_dimension=%u view_format=%u",
                 (unsigned)dimension, (unsigned)view_desc.Format);
    }
    ID3D11Resource_Release(resource);
}

static void liquid_set_d3d11_vertex(liquid_d3d11_vertex_t *vertex,
                                    float x, float y, float screen_depth,
                                    float u, float v,
                                    float width, float height, float alpha)
{
    vertex->position[0] = x / width * 2.0f - 1.0f;
    vertex->position[1] = 1.0f - y / height * 2.0f;
    vertex->position[2] = screen_depth;
    vertex->uv[0] = u;
    vertex->uv[1] = v;
    vertex->color[0] = cfg.core_opacity;
    vertex->color[1] = 0.0f;
    vertex->color[2] = 0.0f;
    vertex->color[3] = alpha;
}

static int liquid_project_world_d3d11(
    const float world[3], UINT width, UINT height, float aspect,
    float tan_half_fov, int exact_projection_available,
    const D3DMATRIX *frame_projection, const D3DVIEWPORT8 *frame_viewport,
    float *screen_x, float *screen_y, float *depth,
    float *screen_depth, int *used_exact)
{
    float view[3];
    float ndc_x, ndc_y;
    if (!world || !screen_x || !screen_y || !depth || !screen_depth ||
        !liquid_world_to_view_point(world, view)) return 0;
    *depth = -view[2];
    if (*depth <= 0.01f || !_finite(*depth)) return 0;
    if (used_exact) *used_exact = 0;
    if (exact_projection_available) {
        /* Clipping is a result of this camera, not evidence that its matrix
           is unavailable. Falling through here projected clipped particles
           with the default FOV and zero depth, splitting a wide-FOV stream
           between two different screen-space trajectories. */
        if (!frame_projection || !frame_viewport ||
            !liquid_project_d3d_view_point(
                frame_projection, frame_viewport, view,
                screen_x, screen_y, screen_depth)) return 0;
        *screen_x = (*screen_x - (float)frame_viewport->X) *
                    (float)width / (float)frame_viewport->Width;
        *screen_y = (*screen_y - (float)frame_viewport->Y) *
                    (float)height / (float)frame_viewport->Height;
        if (used_exact) *used_exact = 1;
        return 1;
    }
    ndc_x = view[0] / (*depth * tan_half_fov * aspect);
    ndc_y = view[1] / (*depth * tan_half_fov);
    if (!_finite(ndc_x) || !_finite(ndc_y) ||
        ndc_x < -1.35f || ndc_x > 1.35f ||
        ndc_y < -1.35f || ndc_y > 1.35f) return 0;
    *screen_x = (1.0f + ndc_x) * (float)width * 0.5f;
    *screen_y = (1.0f - ndc_y) * (float)height * 0.5f;
    *screen_depth = 0.0f;
    if (used_exact) *used_exact = 0;
    return 1;
}

static int liquid_project_world_depth_snapshot(
    const liquid_depth_snapshot_t *snapshot, const float world[3],
    float *screen_x, float *screen_y, float *screen_depth)
{
    float relative[3];
    float view[3];
    if (!snapshot || !snapshot->valid || !world || !screen_x || !screen_y ||
        !screen_depth || !snapshot->viewport.Width ||
        !snapshot->viewport.Height || !snapshot->scene_width ||
        !snapshot->scene_height) return 0;
    relative[0] = world[0] - snapshot->camera_inverse[12];
    relative[1] = world[1] - snapshot->camera_inverse[13];
    relative[2] = world[2] - snapshot->camera_inverse[14];
    view[0] = relative[0] * snapshot->camera_inverse[0] +
              relative[1] * snapshot->camera_inverse[1] +
              relative[2] * snapshot->camera_inverse[2];
    view[1] = relative[0] * snapshot->camera_inverse[4] +
              relative[1] * snapshot->camera_inverse[5] +
              relative[2] * snapshot->camera_inverse[6];
    view[2] = relative[0] * snapshot->camera_inverse[8] +
              relative[1] * snapshot->camera_inverse[9] +
              relative[2] * snapshot->camera_inverse[10];
    if (!liquid_project_d3d_view_point(
            &snapshot->projection, &snapshot->viewport, view,
            screen_x, screen_y, screen_depth)) return 0;
    *screen_x = (*screen_x - (float)snapshot->viewport.X) *
                (float)snapshot->scene_width /
                (float)snapshot->viewport.Width;
    *screen_y = (*screen_y - (float)snapshot->viewport.Y) *
                (float)snapshot->scene_height /
                (float)snapshot->viewport.Height;
    return _finite(*screen_x) && _finite(*screen_y) &&
           _finite(*screen_depth);
}

static int liquid_sample_depth_float(
    const liquid_depth_snapshot_t *snapshot,
    const D3D11_MAPPED_SUBRESOURCE *mapped,
    float screen_x, float screen_y, float *depth)
{
    UINT x, y;
    const float *row;
    if (!snapshot || !mapped || !mapped->pData || !depth ||
        screen_x < (float)snapshot->left ||
        screen_y < (float)snapshot->top ||
        screen_x >= (float)(snapshot->left + snapshot->width) ||
        screen_y >= (float)(snapshot->top + snapshot->height)) return 0;
    x = (UINT)screen_x - snapshot->left;
    y = (UINT)screen_y - snapshot->top;
    row = (const float *)((const BYTE *)mapped->pData +
                         (size_t)y * mapped->RowPitch);
    *depth = row[x];
    return _finite(*depth) && *depth >= 0.0f && *depth <= 1.0f;
}

static int liquid_depth_is_behind(float particle_depth, float scene_depth)
{
    float tolerance = cfg.collision_depth_tolerance;
    switch (liquid_d3d11_depth_test_func) {
    case D3D11_COMPARISON_GREATER:
    case D3D11_COMPARISON_GREATER_EQUAL:
        if (scene_depth <= 0.000001f) return 0;
        return particle_depth < scene_depth - tolerance;
    case D3D11_COMPARISON_LESS:
    case D3D11_COMPARISON_LESS_EQUAL:
    default:
        if (scene_depth >= 0.999999f) return 0;
        return particle_depth > scene_depth + tolerance;
    }
}

static int liquid_d3d11_target_size(
    ID3D11RenderTargetView *target, UINT *width, UINT *height)
{
    ID3D11Resource *resource = NULL;
    D3D11_RESOURCE_DIMENSION dimension = D3D11_RESOURCE_DIMENSION_UNKNOWN;
    D3D11_TEXTURE2D_DESC desc;
    int valid = 0;
    if (!target || !width || !height) return 0;
    memset(&desc, 0, sizeof(desc));
    ID3D11RenderTargetView_GetResource(target, &resource);
    if (resource) {
        ID3D11Resource_GetType(resource, &dimension);
        if (dimension == D3D11_RESOURCE_DIMENSION_TEXTURE2D) {
            ID3D11Texture2D_GetDesc((ID3D11Texture2D*)resource, &desc);
            if (desc.Width && desc.Height) {
                *width = desc.Width;
                *height = desc.Height;
                valid = 1;
            }
        }
        ID3D11Resource_Release(resource);
    }
    return valid;
}

static int liquid_d3d11_color_depth_compatible(
    ID3D11RenderTargetView *target, ID3D11DepthStencilView *depth)
{
    ID3D11Resource *color_resource = NULL;
    ID3D11Resource *depth_resource = NULL;
    D3D11_RESOURCE_DIMENSION color_dimension =
        D3D11_RESOURCE_DIMENSION_UNKNOWN;
    D3D11_RESOURCE_DIMENSION depth_dimension =
        D3D11_RESOURCE_DIMENSION_UNKNOWN;
    D3D11_TEXTURE2D_DESC color_desc;
    D3D11_TEXTURE2D_DESC depth_desc;
    int compatible = 0;
    if (!target || !depth) return 0;
    memset(&color_desc, 0, sizeof(color_desc));
    memset(&depth_desc, 0, sizeof(depth_desc));
    ID3D11RenderTargetView_GetResource(target, &color_resource);
    ID3D11DepthStencilView_GetResource(depth, &depth_resource);
    if (color_resource && depth_resource) {
        ID3D11Resource_GetType(color_resource, &color_dimension);
        ID3D11Resource_GetType(depth_resource, &depth_dimension);
        if (color_dimension == D3D11_RESOURCE_DIMENSION_TEXTURE2D &&
            depth_dimension == D3D11_RESOURCE_DIMENSION_TEXTURE2D) {
            ID3D11Texture2D_GetDesc(
                (ID3D11Texture2D*)color_resource, &color_desc);
            ID3D11Texture2D_GetDesc(
                (ID3D11Texture2D*)depth_resource, &depth_desc);
            compatible = color_desc.Width == depth_desc.Width &&
                color_desc.Height == depth_desc.Height &&
                color_desc.SampleDesc.Count == depth_desc.SampleDesc.Count &&
                color_desc.SampleDesc.Quality == depth_desc.SampleDesc.Quality;
        }
    }
    if (depth_resource) ID3D11Resource_Release(depth_resource);
    if (color_resource) ID3D11Resource_Release(color_resource);
    return compatible;
}

/* Find a visible point immediately before a depth crossing using only the
   camera and depth buffer stored in this snapshot. Looking several particle
   steps backwards recovers thin room surfaces missed between render frames.
   Camera contamination is rejected later in world space; requiring the
   visible point to sample the same depth here incorrectly rejects an opaque
   room surface whose visible side naturally samples the background. */
static int liquid_find_depth_sweep_front(
    const liquid_depth_snapshot_t *snapshot,
    const D3D11_MAPPED_SUBRESOURCE *mapped,
    const liquid_depth_particle_sample_t *sample,
    float front[3], int *steps_out)
{
    float delta[3];
    float step_distance;
    int sweep_step;
    int axis;
    if (!snapshot || !mapped || !sample || !front) return 0;
    for (axis = 0; axis < 3; axis++)
        delta[axis] = sample->position[axis] - sample->previous[axis];
    step_distance = (float)sqrt((double)(
        delta[0] * delta[0] + delta[1] * delta[1] +
        delta[2] * delta[2]));
    if (step_distance < 0.00001f) return 0;
    for (sweep_step = 1; sweep_step <= 6; sweep_step++) {
        float candidate[3];
        float screen_x, screen_y;
        float particle_depth, scene_depth;
        for (axis = 0; axis < 3; axis++)
            candidate[axis] = sample->position[axis] -
                              delta[axis] * (float)sweep_step;
        if (!liquid_project_world_depth_snapshot(
                snapshot, candidate, &screen_x, &screen_y,
                &particle_depth) ||
            !liquid_sample_depth_float(
                snapshot, mapped, screen_x, screen_y, &scene_depth) ||
            liquid_depth_is_behind(particle_depth, scene_depth))
            continue;
        memcpy(front, candidate, sizeof(float) * 3);
        if (steps_out) *steps_out = sweep_step;
        return 1;
    }
    return 0;
}

/* Reconstruct one depth-buffer sample into world space. This converts a
   screen occluder into its actual 3D location, allowing collision validation
   against the droplet path rather than against camera visibility. */
static int liquid_unproject_depth_snapshot(
    const liquid_depth_snapshot_t *snapshot, float screen_x,
    float screen_y, float screen_depth, float world[3])
{
    const D3DMATRIX *m;
    const float *camera;
    float viewport_x, viewport_y;
    float ndc_x, ndc_y;
    float denominator;
    float view_z;
    float a11, a12, a21, a22, b1, b2, determinant;
    float view_x, view_y;
    if (!snapshot || !world || !snapshot->scene_width ||
        !snapshot->scene_height || !snapshot->viewport.Width ||
        !snapshot->viewport.Height || !_finite(screen_depth)) return 0;
    m = &snapshot->projection;
    camera = snapshot->camera_inverse;
    viewport_x = (float)snapshot->viewport.X + screen_x *
        (float)snapshot->viewport.Width / (float)snapshot->scene_width;
    viewport_y = (float)snapshot->viewport.Y + screen_y *
        (float)snapshot->viewport.Height / (float)snapshot->scene_height;
    ndc_x = ((viewport_x - (float)snapshot->viewport.X) /
             (float)snapshot->viewport.Width) * 2.0f - 1.0f;
    ndc_y = 1.0f - ((viewport_y - (float)snapshot->viewport.Y) /
                    (float)snapshot->viewport.Height) * 2.0f;
    denominator = screen_depth * m->_34 - m->_33;
    if (!_finite(denominator) || fabs(denominator) < 0.0000001f)
        return 0;
    view_z = (m->_43 - screen_depth * m->_44) / denominator;
    if (!_finite(view_z)) return 0;
    a11 = m->_11 - ndc_x * m->_14;
    a12 = m->_21 - ndc_x * m->_24;
    a21 = m->_12 - ndc_y * m->_14;
    a22 = m->_22 - ndc_y * m->_24;
    b1 = ndc_x * (view_z * m->_34 + m->_44) -
         view_z * m->_31 - m->_41;
    b2 = ndc_y * (view_z * m->_34 + m->_44) -
         view_z * m->_32 - m->_42;
    determinant = a11 * a22 - a12 * a21;
    if (!_finite(determinant) || fabs(determinant) < 0.0000001f)
        return 0;
    view_x = (b1 * a22 - a12 * b2) / determinant;
    view_y = (a11 * b2 - b1 * a21) / determinant;
    world[0] = camera[12] + view_x * camera[0] +
               view_y * camera[4] + view_z * camera[8];
    world[1] = camera[13] + view_x * camera[1] +
               view_y * camera[5] + view_z * camera[9];
    world[2] = camera[14] + view_x * camera[2] +
               view_y * camera[6] + view_z * camera[10];
    return _finite(world[0]) && _finite(world[1]) && _finite(world[2]);
}

/* A depth transition is a physical contact only when the reconstructed scene
   surface occupies the same world-space point as the droplet path. Moving a
   foreground object across the camera produces a depth transition too, but
   its reconstructed point is far from the airborne droplet and is rejected. */
static int liquid_validate_world_surface_contact(
    const liquid_depth_snapshot_t *snapshot,
    const D3D11_MAPPED_SUBRESOURCE *mapped,
    const liquid_depth_particle_sample_t *sample,
    const float front[3], const float back[3],
    float surface_world[3], float *gap_out, float *limit_out)
{
    float contact[3];
    float screen_x, screen_y, particle_depth, scene_depth;
    float dx, dy, dz;
    float step_dx, step_dy, step_dz;
    float gap, step_distance, limit;
    int axis;
    if (!snapshot || !mapped || !sample || !front || !back ||
        !surface_world) return 0;
    for (axis = 0; axis < 3; axis++)
        contact[axis] = (front[axis] + back[axis]) * 0.5f;
    /* back is the side of the binary-search interval proven to be behind
       scene depth, so sample there instead of at the unclassified midpoint. */
    if (!liquid_project_world_depth_snapshot(
            snapshot, back, &screen_x, &screen_y, &particle_depth) ||
        !liquid_sample_depth_float(
            snapshot, mapped, screen_x, screen_y, &scene_depth) ||
        !liquid_unproject_depth_snapshot(
            snapshot, screen_x, screen_y, scene_depth, surface_world))
        return 0;
    dx = surface_world[0] - contact[0];
    dy = surface_world[1] - contact[1];
    dz = surface_world[2] - contact[2];
    gap = (float)sqrt((double)(dx * dx + dy * dy + dz * dz));
    step_dx = sample->position[0] - sample->previous[0];
    step_dy = sample->position[1] - sample->previous[1];
    step_dz = sample->position[2] - sample->previous[2];
    step_distance = (float)sqrt((double)(
        step_dx * step_dx + step_dy * step_dy + step_dz * step_dz));
    limit = cfg.particle_size * 1.75f + step_distance * 0.35f;
    if (limit < 0.012f) limit = 0.012f;
    if (limit > 0.075f) limit = 0.075f;
    if (gap_out) *gap_out = gap;
    if (limit_out) *limit_out = limit;
    return gap <= limit;
}

/* Estimate the signed-distance gradient only while all six samples belong
   to the same collider primitive. Seams and unavailable queries fail closed. */
static int liquid_body_surface_normal(const float position[3],
    const nc_tk17_physx_body_hit_v1_t *center, float normal[3])
{
    const float epsilon = 0.002f;
    int axis, side;
    if (!liquid_physx_query_body_collider || !center) return 0;
    for (axis = 0; axis < 3; axis++) {
        float distances[2];
        for (side = 0; side < 2; side++) {
            float point[3];
            nc_tk17_physx_body_hit_v1_t hit = {0};
            memcpy(point, position, sizeof(point));
            point[axis] += side ? epsilon : -epsilon;
            hit.size = sizeof(hit);
            if (liquid_physx_query_body_collider(point, 0.04f, &hit, sizeof(hit)) <= 0 ||
                hit.person_index != center->person_index ||
                hit.primitive_kind != center->primitive_kind ||
                hit.node_start != center->node_start || hit.node_end != center->node_end ||
                !_finite(hit.signed_distance)) return 0;
            distances[side] = hit.signed_distance;
        }
        normal[axis] = (distances[1] - distances[0]) / (2.0f * epsilon);
    }
    {
        float length2 = liquid_vec3_dot(normal, normal);
        if (!_finite(length2) || length2 < 0.25f || length2 > 2.25f) return 0;
    }
    return liquid_vec3_normalize(normal);
}

/* Reject silhouette/depth discontinuities: both sides must be nearby and
   independently agree on the tangent plane. No normal is invented on failure. */
static int liquid_depth_surface_normal(const liquid_depth_snapshot_t *snapshot,
    const D3D11_MAPPED_SUBRESOURCE *mapped, const float surface[3], float normal[3])
{
    const float offsets[4][2] = {{1,0}, {0,1}, {-1,0}, {0,-1}};
    float x, y, depth, delta[4][3], other_normal[3], toward_camera[3];
    int i, axis;
    if (!liquid_project_world_depth_snapshot(snapshot, surface, &x, &y, &depth)) return 0;
    x = floorf(x) + 0.5f;
    y = floorf(y) + 0.5f;
    /* Use pixel centers for both the center and its neighbors. */
    {
        float center[3];
        if (!liquid_sample_depth_float(snapshot, mapped, x, y, &depth) ||
            !liquid_unproject_depth_snapshot(snapshot, x, y, depth, center)) return 0;
        for (i = 0; i < 4; i++) {
            float point[3], length2;
            if (!liquid_sample_depth_float(snapshot, mapped,
                    x + offsets[i][0], y + offsets[i][1], &depth) ||
                !liquid_unproject_depth_snapshot(snapshot,
                    x + offsets[i][0], y + offsets[i][1], depth, point)) return 0;
            for (axis = 0; axis < 3; axis++) delta[i][axis] = point[axis] - center[axis];
            length2 = liquid_vec3_dot(delta[i], delta[i]);
            if (length2 < 0.000000001f || length2 > 0.0009f) return 0;
        }
    }
    liquid_vec3_cross(delta[0], delta[1], normal);
    liquid_vec3_cross(delta[2], delta[3], other_normal);
    /* Normalize explicitly: per-pixel cross products can be much smaller than
       the generic world-vector normalizer's minimum length. */
    {
        float a = sqrtf(liquid_vec3_dot(normal, normal));
        float b = sqrtf(liquid_vec3_dot(other_normal, other_normal));
        if (a < 1e-10f || b < 1e-10f) return 0;
        for (axis = 0; axis < 3; axis++) { normal[axis] /= a; other_normal[axis] /= b; }
    }
    if (liquid_vec3_dot(normal, other_normal) < 0.90f) return 0;
    for (axis = 0; axis < 3; axis++)
        toward_camera[axis] = snapshot->camera_inverse[12 + axis] - surface[axis];
    if (liquid_vec3_dot(normal, toward_camera) < 0.0f)
        for (axis = 0; axis < 3; axis++) normal[axis] = -normal[axis];
    return 1;
}

static void liquid_spawn_impact_spray(liquid_particle_t *parent,
    const float surface[3], const float normal[3], DWORD now)
{
    liquid_emitter_t *emitter;
    float tangent[3], bitangent[3], reference[3] = {0, 1, 0};
    int i, axis, active = 0, free_slots = 0, spawned = 0;
    if (parent->secondary || parent->impact_strength < 0.20f) return;
    emitter = liquid_model_emitter_for_emission(parent->emission_id);
    if (!emitter || (emitter->last_impact_spray_tick &&
                    now - emitter->last_impact_spray_tick < 100u)) return;
    for (i = 0; i < cfg.particle_limit; i++) {
        if (!liquid_particles[i].active) free_slots++;
        else if (liquid_particles[i].secondary) active++;
    }
    /* Leave room for the main flow; splashes never evict live particles. */
    if (active >= 24 || free_slots < 4) return;
    if (fabsf(normal[1]) > 0.9f) { reference[0] = 1; reference[1] = 0; }
    liquid_vec3_cross(normal, reference, tangent);
    if (!liquid_vec3_normalize(tangent)) return;
    liquid_vec3_cross(normal, tangent, bitangent);
    for (i = 0; i < cfg.particle_limit && spawned < 2 && active < 24; i++) {
        liquid_particle_t *p = &liquid_particles[i];
        float angle, radial_speed, outward_speed;
        if (p->active) continue;
        memset(p, 0, sizeof(*p));
        p->active = p->satellite = p->secondary = 1;
        p->source_kind = 1;
        p->emission_id = parent->emission_id;
        p->pulse_index = parent->pulse_index;
        liquid_particle_serial++;
        if (!liquid_particle_serial) liquid_particle_serial++;
        p->spawn_order = liquid_particle_serial;
        p->emission_time = parent->emission_time;
        p->separation_age = 0.0f;
        p->stream_stretch = 1.0f;
        p->size_scale = parent->size_scale * 0.38f;
        p->opacity = parent->opacity * 0.65f;
        p->lifetime = fminf(cfg.particle_lifetime, 0.25f + parent->impact_strength * 0.20f);
        angle = (float)(parent->spawn_order % 97u) * 2.39996323f + spawned * 3.14159265f;
        radial_speed = 0.12f + parent->impact_strength * 0.35f;
        outward_speed = 0.10f + parent->impact_strength * 0.25f;
        for (axis = 0; axis < 3; axis++) {
            p->position[axis] = surface[axis] + normal[axis] * (cfg.particle_size + 0.004f);
            p->velocity[axis] = normal[axis] * outward_speed + radial_speed *
                (cosf(angle) * tangent[axis] + sinf(angle) * bitangent[axis]) +
                parent->impact_tangent[axis] * parent->impact_grazing * 0.12f;
        }
        memcpy(p->previous, p->position, sizeof(p->previous));
        active++;
        spawned++;
    }
    if (spawned) emitter->last_impact_spray_tick = now;
}

static int liquid_contact_pair_compatible(const liquid_particle_t *a,
                                         const liquid_particle_t *b)
{
    float delta[3], distance2;
    int axis, owner_a, owner_b;
    if (a == b || !a->active || !b->active || !a->collided || !b->collided ||
        a->secondary || b->secondary || !a->emission_id ||
        a->emission_id != b->emission_id || a->pulse_index != b->pulse_index ||
        a->stream_id != b->stream_id || !a->contact_normal_valid || !b->contact_normal_valid ||
        liquid_vec3_dot(a->contact_normal, b->contact_normal) < 0.80f) return 0;
    owner_a = a->contact_person > 0 ? a->contact_person : a->contact_physx_person;
    owner_b = b->contact_person > 0 ? b->contact_person : b->contact_physx_person;
    if (!owner_a || owner_a != owner_b) return 0;
    if (owner_a > 0 && a->contact_anchor && b->contact_anchor &&
        (a->contact_anchor != b->contact_anchor || a->contact_anchor_end != b->contact_anchor_end))
        return 0;
    for (axis = 0; axis < 3; axis++) delta[axis] = a->position[axis] - b->position[axis];
    distance2 = liquid_vec3_dot(delta, delta);
    if (distance2 < 0.00000001f ||
        distance2 > cfg.collision_connection_distance * cfg.collision_connection_distance) return 0;
    if (owner_a < 0) {
        float plane_limit = fmaxf(0.0015f, cfg.contact_droplet_size * 0.75f);
        if (fabsf(liquid_vec3_dot(delta, a->contact_normal)) > plane_limit ||
            fabsf(liquid_vec3_dot(delta, b->contact_normal)) > plane_limit) return 0;
    }
    return 1;
}

static int liquid_contact_path_supported(const liquid_particle_t *a,
    const liquid_particle_t *b, const liquid_depth_snapshot_t *snapshot,
    const D3D11_MAPPED_SUBRESOURCE *mapped)
{
    int i, axis;
    /* Body ownership and matching anchors constrain moving contacts. For room
       surfaces also verify the space between deposits in the same depth tile. */
    if (a->contact_physx_person > 0 || a->contact_person > 0) return 1;
    for (i = 1; i < 4; i++) {
        float point[3], surface[3], delta[3], x, y, depth;
        float tolerance = fmaxf(0.002f, cfg.contact_droplet_size);
        for (axis = 0; axis < 3; axis++)
            point[axis] = a->position[axis] + (b->position[axis] - a->position[axis]) * (i * 0.25f);
        if (!liquid_project_world_depth_snapshot(snapshot, point, &x, &y, &depth) ||
            !liquid_sample_depth_float(snapshot, mapped, x, y, &depth) ||
            !liquid_unproject_depth_snapshot(snapshot, x, y, depth, surface)) return 0;
        for (axis = 0; axis < 3; axis++) delta[axis] = point[axis] - surface[axis];
        if (liquid_vec3_dot(delta, delta) > tolerance * tolerance) return 0;
    }
    return 1;
}

/* Pin each connection once at impact. A deposit has at most one predecessor
   and one successor, so neighboring contacts form a beaded line, not a web. */
static void liquid_link_contact(liquid_particle_t *p,
    const liquid_depth_snapshot_t *snapshot, const D3D11_MAPPED_SUBRESOURCE *mapped)
{
    float best = cfg.collision_connection_distance * cfg.collision_connection_distance;
    int i, selected = -1;
    if (!cfg.collision_connect_droplets || p->secondary) return;
    for (i = 0; i < cfg.particle_limit; i++) {
        liquid_particle_t *candidate = &liquid_particles[i];
        float delta[3], distance2;
        int axis;
        if (candidate->contact_successor_order || !liquid_contact_pair_compatible(p, candidate)) continue;
        for (axis = 0; axis < 3; axis++) delta[axis] = p->position[axis] - candidate->position[axis];
        distance2 = liquid_vec3_dot(delta, delta);
        if (distance2 >= best || !liquid_contact_path_supported(p, candidate, snapshot, mapped)) continue;
        selected = i;
        best = distance2;
    }
    if (selected >= 0) {
        p->contact_link_index = selected;
        p->contact_link_order = liquid_particles[selected].spawn_order;
        liquid_particles[selected].contact_successor_order = p->spawn_order;
    }
}

static void liquid_set_impact_response(liquid_particle_t *p,
    const float spray_surface[3], const float normal[3], int normal_valid, DWORD now)
{
    float speed = sqrtf(liquid_vec3_dot(p->impact_velocity, p->impact_velocity));
    float inward = normal_valid ? -liquid_vec3_dot(p->impact_velocity, normal) : 0.0f;
    int axis;
    p->contact_normal_valid = normal_valid;
    if (normal_valid) memcpy(p->contact_normal, normal, sizeof(p->contact_normal));
    p->impact_strength = liquid_clamp_unit(speed / 5.0f);
    p->impact_grazing = 0.0f;
    memcpy(p->impact_tangent, p->impact_velocity, sizeof(p->impact_tangent));
    if (normal_valid && inward > 0.0f && speed > 0.0001f) {
        p->impact_strength = liquid_clamp_unit(inward / 4.0f);
        p->impact_grazing = liquid_clamp_unit(1.0f - inward / speed);
        for (axis = 0; axis < 3; axis++)
            p->impact_tangent[axis] += normal[axis] * inward;
        liquid_vec3_normalize(p->impact_tangent);
        liquid_spawn_impact_spray(p, spray_surface, normal, now);
    }
}

static void liquid_apply_depth_collisions(
    const liquid_depth_snapshot_t *snapshot,
    const D3D11_MAPPED_SUBRESOURCE *mapped)
{
    static int collision_log_count;
    static int sample_log_count;
    int i;
    if (!snapshot || !mapped) return;
    for (i = 0; i < cfg.particle_limit; i++) {
        liquid_particle_t *particle = &liquid_particles[i];
        const liquid_depth_particle_sample_t *sample =
            &snapshot->particles[i];
        float current_x, current_y, current_depth, current_scene_depth;
        int current_behind;
        int iteration;
        int sweep_steps = 0;
        float front[3], back[3], surface_world[3];
        float world_gap = 0.0f;
        float world_limit = 0.0f;
        if (!sample->valid || !particle->active || particle->collided ||
            particle->source_kind != 1 ||
            particle->spawn_order != sample->spawn_order ||
            particle->emission_id != sample->emission_id ||
            sample->age < (particle->secondary ? 0.015f : 0.06f))
            continue;
        /* Character collision must not depend on the visible depth buffer.
           Keep the query in this established asynchronous render-collision
           callback, but test the stored world-space particle step before
           projecting it. Four samples cover the capped simulation step
           without moving PhysX access into an earlier startup callback. */
        {
            nc_tk17_physx_body_hit_v1_t body_hit;
            float body_impact[3];
            int body_result = 0;
            int body_step;
            int axis;
            for (body_step = 1; body_step <= 4; body_step++) {
                float t = (float)body_step * 0.25f;
                for (axis = 0; axis < 3; axis++)
                    body_impact[axis] = sample->previous[axis] +
                        (sample->position[axis] - sample->previous[axis]) * t;
                memset(&body_hit, 0, sizeof(body_hit));
                body_result = liquid_query_physx_body_contact(
                    body_impact, &body_hit);
                if (body_result != 0) break;
            }
            if (body_result > 0 && body_hit.person_index >= 1 &&
                body_hit.person_index <= 4) {
                static LONG body_collision_log_count;
                float inward_direction[3];
                float normal[3], spray_surface[3];
                int normal_valid;
                float inward_distance;
                int inward_direction_valid;
                memcpy(spray_surface, body_impact, sizeof(spray_surface));
                normal_valid = liquid_body_surface_normal(body_impact, &body_hit, normal);
                memcpy(inward_direction, sample->velocity,
                       sizeof(inward_direction));
                inward_distance = body_hit.signed_distance > 0.0f
                    ? body_hit.signed_distance : 0.0f;
                /* Native impact-ray diagnostics place the skinned surface
                   about 0.018 world units beyond the PhysX response shell
                   (0.0176 median across confirmed hits). Consume both that
                   measured shell gap and the query's positive tolerance. */
                inward_distance += 0.018f + cfg.particle_size * 0.15f;
                if (inward_distance < 0.018f)
                    inward_distance = 0.018f;
                if (inward_distance > 0.055f)
                    inward_distance = 0.055f;
                inward_direction_valid =
                    liquid_vec3_normalize(inward_direction);
                if (inward_direction_valid) {
                    for (axis = 0; axis < 3; axis++)
                        body_impact[axis] +=
                            inward_direction[axis] * inward_distance;
                }
                memcpy(particle->position, body_impact,
                       sizeof(particle->position));
                memcpy(particle->previous, body_impact,
                       sizeof(particle->previous));
                memcpy(particle->last_visible, body_impact,
                       sizeof(particle->last_visible));
                particle->has_last_visible = 1;
                memcpy(particle->impact_velocity, sample->velocity,
                       sizeof(particle->impact_velocity));
                particle->contact_physx_person = body_hit.person_index;
                memset(particle->velocity, 0, sizeof(particle->velocity));
                particle->collided = 1;
                particle->collision_age = 0.0f;
                particle->satellite = 1;
                liquid_set_impact_response(particle, spray_surface, normal,
                                           normal_valid, GetTickCount());
                if (particle->lifetime <
                    particle->age + cfg.collision_surface_hold)
                    particle->lifetime =
                        particle->age + cfg.collision_surface_hold;
                if (particle->secondary) particle->lifetime = particle->age + 0.18f;
                /* The asynchronous collision sample can arrive after the
                   coherent ribbon has already advanced through the body.
                   Remove only same-pulse ribbon particles beyond the impact
                   plane; keep independent droplets and every landed contact. */
                if (inward_direction_valid && !particle->secondary) {
                    int clip_index;
                    for (clip_index = 0;
                         clip_index < cfg.particle_limit; clip_index++) {
                        liquid_particle_t *clip =
                            &liquid_particles[clip_index];
                        float relative[3];
                        float past_contact;
                        if (clip == particle || !clip->active ||
                            clip->collided || clip->satellite ||
                            clip->source_kind != 1 ||
                            clip->emission_id != particle->emission_id ||
                            clip->stream_id != particle->stream_id ||
                            clip->pulse_index != particle->pulse_index ||
                            clip->age > cfg.model_stream_cohesion)
                            continue;
                        for (axis = 0; axis < 3; axis++)
                            relative[axis] = clip->position[axis] -
                                             particle->position[axis];
                        past_contact =
                            liquid_vec3_dot(relative, inward_direction);
                        if (past_contact > cfg.particle_size * 0.5f)
                            clip->active = 0;
                    }
                }
                liquid_link_contact(particle, snapshot, mapped);
                liquid_queue_native_model_contact(particle);
                if (InterlockedIncrement(&body_collision_log_count) <= 16)
                    log_line("liquid camera-independent body collision particle=%u person=%d primitive=%d nodes=(%d,%d) segment_t=%.3f signed_distance=%.5f inward=%.5f world=(%.4f,%.4f,%.4f)",
                             particle->spawn_order,
                             body_hit.person_index,
                             body_hit.primitive_kind,
                             body_hit.node_start, body_hit.node_end,
                             body_hit.segment_t,
                             body_hit.signed_distance,
                             inward_distance,
                             particle->position[0],
                             particle->position[1],
                             particle->position[2]);
                continue;
            }
        }
        if (!liquid_project_world_depth_snapshot(
                snapshot, sample->position, &current_x, &current_y,
                &current_depth) ||
            !liquid_sample_depth_float(snapshot, mapped, current_x, current_y,
                                       &current_scene_depth))
            continue;
        current_behind = liquid_depth_is_behind(
            current_depth, current_scene_depth);
        if (sample_log_count < 24 &&
            ((particle->spawn_order & 7u) == 0u) &&
            sample->age >= 0.10f) {
            sample_log_count++;
            log_line("liquid collision sample particle=%u age=%.3f screen=(%.1f,%.1f) particle_depth=%.7f scene_depth=%.7f delta=%.7f behind=%d tile=(%u,%u %ux%u)",
                     particle->spawn_order, sample->age,
                     current_x, current_y, current_depth,
                     current_scene_depth,
                     current_depth - current_scene_depth,
                     current_behind, snapshot->left, snapshot->top,
                     snapshot->width, snapshot->height);
        }
        if (!current_behind) {
            memcpy(particle->last_visible, sample->position,
                   sizeof(particle->last_visible));
            particle->has_last_visible = 1;
            continue;
        }
        if (!liquid_find_depth_sweep_front(
                snapshot, mapped, sample, front, &sweep_steps)) {
            /* Never fall back to last_visible here: it may have been
               captured with an older camera orientation. */
            continue;
        }
        memcpy(back, sample->position, sizeof(back));
        for (iteration = 0; iteration < 6; iteration++) {
            float midpoint[3];
            float midpoint_x, midpoint_y, midpoint_depth, scene_depth;
            int axis;
            for (axis = 0; axis < 3; axis++)
                midpoint[axis] = (front[axis] + back[axis]) * 0.5f;
            if (liquid_project_world_depth_snapshot(
                    snapshot, midpoint, &midpoint_x, &midpoint_y,
                    &midpoint_depth) &&
                liquid_sample_depth_float(snapshot, mapped, midpoint_x,
                                          midpoint_y, &scene_depth) &&
                liquid_depth_is_behind(midpoint_depth, scene_depth))
                memcpy(back, midpoint, sizeof(back));
            else
                memcpy(front, midpoint, sizeof(front));
        }
        if (!liquid_validate_world_surface_contact(
                snapshot, mapped, sample, front, back, surface_world,
                &world_gap, &world_limit)) {
            static int occlusion_log_count;
            if (occlusion_log_count < 24) {
                occlusion_log_count++;
                log_line("liquid camera occlusion rejected particle=%u world_gap=%.5f contact_limit=%.5f sweep_steps=%d",
                         particle->spawn_order, world_gap, world_limit,
                         sweep_steps);
            }
            continue;
        }
        {
            static LONG physx_contact_log_count;
            nc_tk17_physx_body_hit_v1_t physx_hit;
            int physx_result;
            memset(&physx_hit, 0, sizeof(physx_hit));
            physx_result = liquid_query_physx_body_contact(
                surface_world, &physx_hit);
            if (physx_result <= 0)
                physx_result = liquid_query_physx_body_contact(
                    front, &physx_hit);
            if (physx_result > 0 && physx_hit.person_index >= 1 &&
                physx_hit.person_index <= 4) {
                particle->contact_physx_person = physx_hit.person_index;
            } else if (physx_result == 0) {
                particle->contact_physx_person = -1;
            } else {
                particle->contact_physx_person = 0;
            }
            if (InterlockedIncrement(&physx_contact_log_count) <= 16) {
                log_line("liquid contact classification particle=%u body_result=%d person=%d primitive=%d nodes=(%d,%d) segment_t=%.3f signed_distance=%.5f world_gap=%.5f",
                         particle->spawn_order, physx_result,
                         physx_result > 0 ? physx_hit.person_index : 0,
                         physx_result > 0 ? physx_hit.primitive_kind : 0,
                         physx_result > 0 ? physx_hit.node_start : -1,
                         physx_result > 0 ? physx_hit.node_end : -1,
                         physx_result > 0 ? physx_hit.segment_t : 0.0f,
                         physx_hit.signed_distance, world_gap);
            }
            memcpy(particle->position, surface_world,
                   sizeof(particle->position));
            memcpy(particle->previous, surface_world,
                   sizeof(particle->previous));
            memcpy(particle->last_visible, surface_world,
                   sizeof(particle->last_visible));
            particle->has_last_visible = 1;
            memcpy(particle->impact_velocity, sample->velocity,
                   sizeof(particle->impact_velocity));
            liquid_queue_native_model_contact(particle);
        }
        memset(particle->velocity, 0, sizeof(particle->velocity));
        particle->collided = 1;
        particle->collision_age = 0.0f;
        particle->satellite = 1;
        {
            float normal[3];
            int normal_valid = liquid_depth_surface_normal(snapshot, mapped,
                                                            surface_world, normal);
            liquid_set_impact_response(particle, surface_world, normal,
                                       normal_valid, GetTickCount());
        }
        if (particle->lifetime < particle->age + cfg.collision_surface_hold)
            particle->lifetime = particle->age + cfg.collision_surface_hold;
        if (particle->secondary) particle->lifetime = particle->age + 0.18f;
        liquid_link_contact(particle, snapshot, mapped);
        if (collision_log_count < 12) {
            collision_log_count++;
            log_line("liquid depth collision hit particle=%u age=%.3f world=(%.4f,%.4f,%.4f) particle_depth=%.6f scene_depth=%.6f sweep_steps=%d world_gap=%.5f",
                     particle->spawn_order, sample->age,
                     particle->position[0], particle->position[1],
                     particle->position[2], current_depth,
                     current_scene_depth, sweep_steps, world_gap);
        }
    }
}

static int liquid_ensure_collision_depth_resources(
    ID3D11Device *device, ID3D11Resource *source_resource,
    const D3D11_TEXTURE2D_DESC *source_desc)
{
    static const char compute_shader_source[] =
        "Texture2D<float> SourceDepth:register(t0);"
        "RWTexture2D<float> DestDepth:register(u0);"
        "cbuffer CopyInfo:register(b0){uint2 sourceOrigin;uint2 copySize;}"
        "[numthreads(8,8,1)]"
        "void main(uint3 id:SV_DispatchThreadID){"
        "if(id.x<copySize.x&&id.y<copySize.y)"
        "DestDepth[id.xy]=SourceDepth.Load(int3(sourceOrigin+id.xy,0));}";
    D3D11_TEXTURE2D_DESC staging_desc;
    D3D11_TEXTURE2D_DESC float_desc;
    D3D11_SHADER_RESOURCE_VIEW_DESC source_view_desc;
    D3D11_UNORDERED_ACCESS_VIEW_DESC float_view_desc;
    D3D11_BUFFER_DESC constant_desc;
    HMODULE compiler = NULL;
    pD3DCompile compile_shader = NULL;
    ID3DBlob *compute_blob = NULL;
    ID3DBlob *errors = NULL;
    HRESULT result;
    int i;
    if (!device || !source_resource || !source_desc ||
        source_desc->SampleDesc.Count != 1 ||
        source_desc->Format != DXGI_FORMAT_R24G8_TYPELESS ||
        !(source_desc->BindFlags & D3D11_BIND_SHADER_RESOURCE)) return 0;
    if (liquid_collision_depth_device == device &&
        liquid_collision_depth_source_resource == source_resource &&
        liquid_collision_depth_source_view &&
        liquid_collision_depth_float_texture &&
        liquid_collision_depth_float_view &&
        liquid_collision_depth_compute_shader &&
        liquid_collision_depth_constant_buffer &&
        liquid_collision_depth_staging[0] &&
        liquid_collision_depth_format == (UINT)source_desc->Format &&
        liquid_collision_depth_source_width == source_desc->Width &&
        liquid_collision_depth_source_height == source_desc->Height)
        return 1;
    liquid_release_collision_depth_resources();
    compiler = LoadLibraryA("d3dcompiler_47.dll");
    if (!compiler) compiler = LoadLibraryA("d3dcompiler_43.dll");
    if (compiler) copy_proc_address(&compile_shader,
                                    GetProcAddress(compiler, "D3DCompile"),
                                    sizeof(compile_shader));
    if (!compile_shader) {
        log_line("liquid depth compute setup failed reason=D3DCompile-unavailable");
        if (compiler) FreeLibrary(compiler);
        return 0;
    }
    result = compile_shader(compute_shader_source,
                            sizeof(compute_shader_source) - 1,
                            "NC-TK17-Liquids-Depth-CS", NULL, NULL,
                            "main", "cs_5_0", D3DCOMPILE_ENABLE_STRICTNESS,
                            0, &compute_blob, &errors);
    if (FAILED(result) || !compute_blob) {
        log_line("liquid depth compute compilation failed hr=0x%08lX error=%s",
                 (unsigned long)result,
                 errors ? (const char*)ID3D10Blob_GetBufferPointer(errors) :
                          "unavailable");
        if (errors) ID3D10Blob_Release(errors);
        if (compute_blob) ID3D10Blob_Release(compute_blob);
        FreeLibrary(compiler);
        return 0;
    }
    if (errors) { ID3D10Blob_Release(errors); errors = NULL; }
    result = ID3D11Device_CreateComputeShader(
        device, ID3D10Blob_GetBufferPointer(compute_blob),
        ID3D10Blob_GetBufferSize(compute_blob), NULL,
        &liquid_collision_depth_compute_shader);
    ID3D10Blob_Release(compute_blob);
    FreeLibrary(compiler);
    if (FAILED(result) || !liquid_collision_depth_compute_shader) {
        log_line("liquid depth compute shader creation failed hr=0x%08lX",
                 (unsigned long)result);
        liquid_release_collision_depth_resources();
        return 0;
    }
    memset(&source_view_desc, 0, sizeof(source_view_desc));
    source_view_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    source_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    source_view_desc.Texture2D.MostDetailedMip = 0;
    source_view_desc.Texture2D.MipLevels = 1;
    result = ID3D11Device_CreateShaderResourceView(
        device, source_resource, &source_view_desc,
        &liquid_collision_depth_source_view);
    if (FAILED(result) || !liquid_collision_depth_source_view) {
        log_line("liquid depth shader view creation failed hr=0x%08lX",
                 (unsigned long)result);
        liquid_release_collision_depth_resources();
        return 0;
    }
    memset(&float_desc, 0, sizeof(float_desc));
    float_desc.Width = source_desc->Width < LIQUID_COLLISION_TILE_SIZE ?
                       source_desc->Width : LIQUID_COLLISION_TILE_SIZE;
    float_desc.Height = source_desc->Height < LIQUID_COLLISION_TILE_SIZE ?
                        source_desc->Height : LIQUID_COLLISION_TILE_SIZE;
    float_desc.MipLevels = 1;
    float_desc.ArraySize = 1;
    float_desc.Format = DXGI_FORMAT_R32_FLOAT;
    float_desc.SampleDesc.Count = 1;
    float_desc.Usage = D3D11_USAGE_DEFAULT;
    float_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    result = ID3D11Device_CreateTexture2D(
        device, &float_desc, NULL, &liquid_collision_depth_float_texture);
    if (FAILED(result) || !liquid_collision_depth_float_texture) {
        log_line("liquid depth float texture creation failed hr=0x%08lX",
                 (unsigned long)result);
        liquid_release_collision_depth_resources();
        return 0;
    }
    memset(&float_view_desc, 0, sizeof(float_view_desc));
    float_view_desc.Format = DXGI_FORMAT_R32_FLOAT;
    float_view_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    float_view_desc.Texture2D.MipSlice = 0;
    result = ID3D11Device_CreateUnorderedAccessView(
        device, (ID3D11Resource*)liquid_collision_depth_float_texture,
        &float_view_desc, &liquid_collision_depth_float_view);
    if (FAILED(result) || !liquid_collision_depth_float_view) {
        log_line("liquid depth float view creation failed hr=0x%08lX",
                 (unsigned long)result);
        liquid_release_collision_depth_resources();
        return 0;
    }
    memset(&constant_desc, 0, sizeof(constant_desc));
    constant_desc.ByteWidth = 4 * sizeof(UINT);
    constant_desc.Usage = D3D11_USAGE_DEFAULT;
    constant_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    result = ID3D11Device_CreateBuffer(
        device, &constant_desc, NULL,
        &liquid_collision_depth_constant_buffer);
    if (FAILED(result) || !liquid_collision_depth_constant_buffer) {
        log_line("liquid depth constant buffer creation failed hr=0x%08lX",
                 (unsigned long)result);
        liquid_release_collision_depth_resources();
        return 0;
    }
    memset(&staging_desc, 0, sizeof(staging_desc));
    staging_desc.Width = float_desc.Width;
    staging_desc.Height = float_desc.Height;
    staging_desc.MipLevels = 1;
    staging_desc.ArraySize = 1;
    staging_desc.Format = DXGI_FORMAT_R32_FLOAT;
    staging_desc.SampleDesc.Count = 1;
    staging_desc.Usage = D3D11_USAGE_STAGING;
    staging_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    for (i = 0; i < LIQUID_COLLISION_READBACK_COUNT; i++) {
        HRESULT result = ID3D11Device_CreateTexture2D(
            device, &staging_desc, NULL,
            &liquid_collision_depth_staging[i]);
        if (FAILED(result) || !liquid_collision_depth_staging[i]) {
            log_line("liquid collision staging creation failed slot=%d hr=0x%08lX",
                     i, (unsigned long)result);
            liquid_release_collision_depth_resources();
            return 0;
        }
    }
    liquid_collision_depth_source_resource = source_resource;
    ID3D11Resource_AddRef(liquid_collision_depth_source_resource);
    liquid_collision_depth_device = device;
    ID3D11Device_AddRef(liquid_collision_depth_device);
    liquid_collision_depth_format = (UINT)source_desc->Format;
    liquid_collision_depth_source_width = source_desc->Width;
    liquid_collision_depth_source_height = source_desc->Height;
    log_line("liquid GPU-decoded depth collision initialized source=%ux%u tile=%ux%u source_format=%u output_format=%u buffers=%d",
             source_desc->Width, source_desc->Height,
             staging_desc.Width, staging_desc.Height,
             (unsigned)source_desc->Format,
             (unsigned)staging_desc.Format,
             LIQUID_COLLISION_READBACK_COUNT);
    return 1;
}

static void liquid_depth_collision_tick(
    ID3D11DeviceContext *context, ID3D11DepthStencilView *depth_view)
{
    ID3D11Device *device = NULL;
    ID3D11Resource *resource = NULL;
    ID3D11RenderTargetView *bound_target = NULL;
    ID3D11DepthStencilView *bound_depth = NULL;
    ID3D11ComputeShader *old_compute_shader = NULL;
    ID3D11ShaderResourceView *old_compute_source_view = NULL;
    ID3D11UnorderedAccessView *old_compute_float_view = NULL;
    ID3D11Buffer *old_compute_constant_buffer = NULL;
    ID3D11ShaderResourceView *null_source_view = NULL;
    ID3D11UnorderedAccessView *null_float_view = NULL;
    D3D11_RESOURCE_DIMENSION dimension = D3D11_RESOURCE_DIMENSION_UNKNOWN;
    D3D11_TEXTURE2D_DESC source_desc;
    D3D11_MAPPED_SUBRESOURCE mapped;
    liquid_depth_snapshot_t current_snapshot;
    int read_index;
    int i;
    int active_count = 0;
    float min_x, min_y, max_x, max_y;
    UINT tile_width, tile_height;
    UINT copy_info[4];
    if (!cfg.collision_enabled || !context || !depth_view ||
        !liquid_has_airborne_model_particles() ||
        !InterlockedCompareExchange(&captured_d3d_projection_valid, 0, 0) ||
        !InterlockedCompareExchange(&captured_camera_inverse_valid, 0, 0))
        return;
    memset(&source_desc, 0, sizeof(source_desc));
    ID3D11DepthStencilView_GetResource(depth_view, &resource);
    if (!resource) return;
    ID3D11Resource_GetType(resource, &dimension);
    if (dimension != D3D11_RESOURCE_DIMENSION_TEXTURE2D) goto cleanup;
    ID3D11Texture2D_GetDesc((ID3D11Texture2D*)resource, &source_desc);
    ID3D11DeviceContext_GetDevice(context, &device);
    if (!device || !liquid_ensure_collision_depth_resources(
            device, resource, &source_desc)) goto cleanup;

    /* Read the oldest ring slot—the one about to be reused. Waiting for the
       full ring avoids mapping the immediately previous frame while the GPU
       is commonly still writing it. */
    read_index = liquid_collision_depth_write_index;
    if (liquid_collision_depth_ready_count >=
            LIQUID_COLLISION_READBACK_COUNT &&
        liquid_collision_depth_snapshots[read_index].valid) {
        HRESULT map_result;
        static int readback_logged;
        memset(&mapped, 0, sizeof(mapped));
        map_result = ID3D11DeviceContext_Map(
                context,
                (ID3D11Resource*)liquid_collision_depth_staging[read_index],
                0, D3D11_MAP_READ, D3D11_MAP_FLAG_DO_NOT_WAIT, &mapped);
        if (SUCCEEDED(map_result)) {
            liquid_apply_depth_collisions(
                &liquid_collision_depth_snapshots[read_index], &mapped);
            ID3D11DeviceContext_Unmap(
                context,
                (ID3D11Resource*)liquid_collision_depth_staging[read_index],
                0);
            liquid_collision_depth_snapshots[read_index].valid = 0;
            liquid_collision_depth_ready_count--;
            if (!readback_logged) {
                readback_logged = 1;
                log_line("liquid asynchronous depth readback active latency_frames=%d",
                         LIQUID_COLLISION_READBACK_COUNT);
            }
        } else {
            /* Preserve this slot until it becomes readable instead of
               overwriting the only completed collision snapshot. */
            goto cleanup;
        }
    }

    memset(&current_snapshot, 0, sizeof(current_snapshot));
    current_snapshot.valid = 1;
    current_snapshot.scene_width = source_desc.Width;
    current_snapshot.scene_height = source_desc.Height;
    memcpy(&current_snapshot.projection, &captured_d3d_projection,
           sizeof(current_snapshot.projection));
    memcpy(&current_snapshot.viewport, &captured_d3d_viewport,
           sizeof(current_snapshot.viewport));
    memcpy(current_snapshot.camera_inverse, captured_camera_inverse,
           sizeof(current_snapshot.camera_inverse));
    min_x = (float)source_desc.Width;
    min_y = (float)source_desc.Height;
    max_x = 0.0f;
    max_y = 0.0f;
    for (i = 0; i < cfg.particle_limit; i++) {
        liquid_particle_t *particle = &liquid_particles[i];
        liquid_depth_particle_sample_t *sample =
            &current_snapshot.particles[i];
        float x, y, depth;
        float sweep_back[3];
        float sweep_x, sweep_y, sweep_depth;
        int axis;
        if (!particle->active || particle->collided ||
            particle->source_kind != 1 ||
            !liquid_project_world_depth_snapshot(
                &current_snapshot, particle->position, &x, &y, &depth))
            continue;
        sample->valid = 1;
        sample->spawn_order = particle->spawn_order;
        sample->emission_id = particle->emission_id;
        sample->age = particle->age;
        memcpy(sample->position, particle->position,
               sizeof(sample->position));
        memcpy(sample->previous, particle->previous,
               sizeof(sample->previous));
        memcpy(sample->velocity, particle->velocity,
               sizeof(sample->velocity));
        memcpy(sample->last_visible, particle->last_visible,
               sizeof(sample->last_visible));
        sample->has_last_visible = particle->has_last_visible;
        if (x < min_x) min_x = x;
        if (y < min_y) min_y = y;
        if (x > max_x) max_x = x;
        if (y > max_y) max_y = y;
        /* Include the entire backward collision sweep in the copied depth
           tile.  Previously the tile covered only current particle pixels,
           so a valid pre-impact sample could lie outside the readback. */
        for (axis = 0; axis < 3; axis++)
            sweep_back[axis] = particle->position[axis] -
                (particle->position[axis] - particle->previous[axis]) * 6.0f;
        if (liquid_project_world_depth_snapshot(
                &current_snapshot, sweep_back, &sweep_x, &sweep_y,
                &sweep_depth)) {
            if (sweep_x < min_x) min_x = sweep_x;
            if (sweep_y < min_y) min_y = sweep_y;
            if (sweep_x > max_x) max_x = sweep_x;
            if (sweep_y > max_y) max_y = sweep_y;
        }
        active_count++;
    }
    if (!active_count) goto cleanup;
    min_x -= 8.0f;
    min_y -= 8.0f;
    max_x += 8.0f;
    max_y += 8.0f;
    if (min_x < 0.0f) min_x = 0.0f;
    if (min_y < 0.0f) min_y = 0.0f;
    if (max_x > (float)source_desc.Width) max_x = (float)source_desc.Width;
    if (max_y > (float)source_desc.Height) max_y = (float)source_desc.Height;
    if (max_x <= min_x || max_y <= min_y) goto cleanup;
    tile_width = (UINT)ceil((double)(max_x - min_x));
    tile_height = (UINT)ceil((double)(max_y - min_y));
    if (tile_width > LIQUID_COLLISION_TILE_SIZE) {
        float center = (min_x + max_x) * 0.5f;
        min_x = center - LIQUID_COLLISION_TILE_SIZE * 0.5f;
        if (min_x < 0.0f) min_x = 0.0f;
        if (min_x + LIQUID_COLLISION_TILE_SIZE > source_desc.Width)
            min_x = (float)(source_desc.Width - LIQUID_COLLISION_TILE_SIZE);
        tile_width = LIQUID_COLLISION_TILE_SIZE;
    }
    if (tile_height > LIQUID_COLLISION_TILE_SIZE) {
        float center = (min_y + max_y) * 0.5f;
        min_y = center - LIQUID_COLLISION_TILE_SIZE * 0.5f;
        if (min_y < 0.0f) min_y = 0.0f;
        if (min_y + LIQUID_COLLISION_TILE_SIZE > source_desc.Height)
            min_y = (float)(source_desc.Height - LIQUID_COLLISION_TILE_SIZE);
        tile_height = LIQUID_COLLISION_TILE_SIZE;
    }
    current_snapshot.left = (UINT)floor((double)min_x);
    current_snapshot.top = (UINT)floor((double)min_y);
    current_snapshot.width = tile_width;
    current_snapshot.height = tile_height;
    if (current_snapshot.left + current_snapshot.width > source_desc.Width)
        current_snapshot.width = source_desc.Width - current_snapshot.left;
    if (current_snapshot.top + current_snapshot.height > source_desc.Height)
        current_snapshot.height = source_desc.Height - current_snapshot.top;
    if (!current_snapshot.width || !current_snapshot.height) goto cleanup;
    liquid_collision_depth_snapshots
        [liquid_collision_depth_write_index] = current_snapshot;
    /* Typeless D24 depth cannot be read reliably through a raw CPU staging
       copy on Hook5. Decode it on the GPU through its legal R24 SRV, then
       asynchronously read back the R32_FLOAT result. */
    ID3D11DeviceContext_OMGetRenderTargets(
        context, 1, &bound_target, &bound_depth);
    if (bound_depth)
        ID3D11DeviceContext_OMSetRenderTargets(
            context, bound_target ? 1 : 0,
            bound_target ? &bound_target : NULL, NULL);
    ID3D11DeviceContext_CSGetShader(
        context, &old_compute_shader, NULL, NULL);
    ID3D11DeviceContext_CSGetShaderResources(
        context, 0, 1, &old_compute_source_view);
    ID3D11DeviceContext_CSGetUnorderedAccessViews(
        context, 0, 1, &old_compute_float_view);
    ID3D11DeviceContext_CSGetConstantBuffers(
        context, 0, 1, &old_compute_constant_buffer);
    copy_info[0] = current_snapshot.left;
    copy_info[1] = current_snapshot.top;
    copy_info[2] = current_snapshot.width;
    copy_info[3] = current_snapshot.height;
    ID3D11DeviceContext_UpdateSubresource(
        context,
        (ID3D11Resource*)liquid_collision_depth_constant_buffer,
        0, NULL, copy_info, 0, 0);
    ID3D11DeviceContext_CSSetShader(
        context, liquid_collision_depth_compute_shader, NULL, 0);
    ID3D11DeviceContext_CSSetShaderResources(
        context, 0, 1, &liquid_collision_depth_source_view);
    ID3D11DeviceContext_CSSetUnorderedAccessViews(
        context, 0, 1, &liquid_collision_depth_float_view, NULL);
    ID3D11DeviceContext_CSSetConstantBuffers(
        context, 0, 1, &liquid_collision_depth_constant_buffer);
    ID3D11DeviceContext_Dispatch(
        context, (current_snapshot.width + 7) / 8,
        (current_snapshot.height + 7) / 8, 1);
    ID3D11DeviceContext_CSSetShaderResources(
        context, 0, 1, &null_source_view);
    ID3D11DeviceContext_CSSetUnorderedAccessViews(
        context, 0, 1, &null_float_view, NULL);
    {
        D3D11_BOX copy_box;
        memset(&copy_box, 0, sizeof(copy_box));
        copy_box.right = current_snapshot.width;
        copy_box.bottom = current_snapshot.height;
        copy_box.back = 1;
        /* The compute shader writes the selected scene tile into the
           top-left of the fixed-size float texture. Copy only that written
           region; the collision reader never consumes pixels outside the
           snapshot dimensions. */
        ID3D11DeviceContext_CopySubresourceRegion(
            context,
            (ID3D11Resource*)liquid_collision_depth_staging
            [liquid_collision_depth_write_index],
            0, 0, 0, 0,
            (ID3D11Resource*)liquid_collision_depth_float_texture,
            0, &copy_box);
    }
    ID3D11DeviceContext_CSSetShader(
        context, old_compute_shader, NULL, 0);
    ID3D11DeviceContext_CSSetShaderResources(
        context, 0, 1, &old_compute_source_view);
    ID3D11DeviceContext_CSSetUnorderedAccessViews(
        context, 0, 1, &old_compute_float_view, NULL);
    ID3D11DeviceContext_CSSetConstantBuffers(
        context, 0, 1, &old_compute_constant_buffer);
    if (bound_depth)
        ID3D11DeviceContext_OMSetRenderTargets(
            context, bound_target ? 1 : 0,
            bound_target ? &bound_target : NULL, bound_depth);
    liquid_collision_depth_write_index =
        (liquid_collision_depth_write_index + 1) %
        LIQUID_COLLISION_READBACK_COUNT;
    if (liquid_collision_depth_ready_count <
        LIQUID_COLLISION_READBACK_COUNT)
        liquid_collision_depth_ready_count++;
cleanup:
    if (old_compute_constant_buffer)
        ID3D11Buffer_Release(old_compute_constant_buffer);
    if (old_compute_float_view)
        ID3D11UnorderedAccessView_Release(old_compute_float_view);
    if (old_compute_source_view)
        ID3D11ShaderResourceView_Release(old_compute_source_view);
    if (old_compute_shader)
        ID3D11ComputeShader_Release(old_compute_shader);
    if (bound_depth) ID3D11DepthStencilView_Release(bound_depth);
    if (bound_target) ID3D11RenderTargetView_Release(bound_target);
    if (device) ID3D11Device_Release(device);
    if (resource) ID3D11Resource_Release(resource);
}

static float liquid_particle_visual_alpha(const liquid_particle_t *particle)
{
    float life_fraction;
    float fade_in;
    float fade_out;
    float alpha;
    if (!particle || particle->lifetime <= 0.0f) return 0.0f;
    life_fraction = 1.0f - particle->age / particle->lifetime;
    if (life_fraction < 0.0f) life_fraction = 0.0f;
    if (life_fraction > 1.0f) life_fraction = 1.0f;
    fade_in = particle->age / 0.035f;
    if (fade_in > 1.0f) fade_in = 1.0f;
    fade_out = life_fraction / 0.22f;
    if (fade_out > 1.0f) fade_out = 1.0f;
    alpha = particle->opacity * fade_in * fade_out;
    if (particle->collided && cfg.collision_surface_hold > 0.0f) {
        float contact_fade =
            1.0f - particle->collision_age / cfg.collision_surface_hold;
        if (contact_fade < 0.0f) contact_fade = 0.0f;
        if (contact_fade > 1.0f) contact_fade = 1.0f;
        alpha *= contact_fade;
    }
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 0.82f) alpha = 0.82f;
    return alpha;
}

/* Blend an absolute base-center opacity into the established downstream
   appearance. Newborn fade/random opacity must not hollow out the live base. */
static void liquid_stream_alpha(const liquid_particle_t *particle,
    float distance, float breakup, float *alpha, float *base_alpha)
{
    float t = cfg.stream_base_fade_length > 0.000001f ?
        liquid_smoothstep_unit(distance / cfg.stream_base_fade_length) : 1.0f;
    float life_fade = particle->lifetime > 0.0f ? liquid_clamp_unit(
        (1.0f - particle->age / particle->lifetime) / 0.22f) : 0.0f;
    float intact = 1.0f - liquid_clamp_unit(breakup);
    *alpha = fminf(0.78f, liquid_particle_visual_alpha(particle) *
                            cfg.stream_opacity * 1.08f * intact) * t;
    *base_alpha = cfg.stream_base_opacity * (1.0f - t) * intact * life_fade;
}

/* Keep the liquid straight immediately after it leaves penis_jointEnd, then
   smoothly introduce the existing ribbon curve and thickness variation. */
static float liquid_stream_wobble_envelope(float distance)
{
    float t;
    if (distance <= cfg.stream_wobble_start) return 0.0f;
    if (cfg.stream_wobble_fade_length <= 0.000001f) return 1.0f;
    t = (distance - cfg.stream_wobble_start) /
        cfg.stream_wobble_fade_length;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return t * t * (3.0f - 2.0f * t);
}

/* Age retains the configured maximum cohesion time. Physical stretching can
   detach a strand earlier, and cannot be undone by moving the camera. */
static float liquid_model_breakup_blend(
    const liquid_particle_t *particle)
{
    float window;
    float start;
    float blend;
    if (!particle || particle->source_kind != 1 || particle->satellite)
        return 1.0f;
    window = cfg.model_stream_cohesion * 0.28f;
    if (window < 0.10f) window = 0.10f;
    if (window > 0.24f) window = 0.24f;
    if (window > cfg.model_stream_cohesion * 0.75f)
        window = cfg.model_stream_cohesion * 0.75f;
    if (window < 0.001f) return 1.0f;
    start = cfg.model_stream_cohesion - window;
    blend = (particle->age - start) / window;
    if (blend < 0.0f) blend = 0.0f;
    if (blend > 1.0f) blend = 1.0f;
    blend = blend * blend * (3.0f - 2.0f * blend);
    return blend > particle->stretch_breakup ? blend : particle->stretch_breakup;
}

/* Material coordinates: a particle carries the same phase for its lifetime,
   regardless of curve indices, clipped points, or the number of samples. */
static float liquid_stream_bulge_at(float emission_time, int pulse_index)
{
    float phase = emission_time * 48.0f + pulse_index * 1.31f;
    return 0.75f * sinf(phase) + 0.25f * sinf(phase * 2.2162162f);
}

static float liquid_droplet_rounding(const liquid_particle_t *particle)
{
    float detached_time;
    if (particle->satellite) detached_time = particle->age;
    else if (particle->separation_age >= 0.0f)
        detached_time = particle->age - particle->separation_age;
    else
        detached_time = particle->age - cfg.model_stream_cohesion;
    return liquid_smoothstep_unit(detached_time / 0.18f);
}

/* Keep a bulbous drop profile even at high speed. A fixed time trail alone
   can stretch a 5 mm drop into a 100 mm needle. Bound the trail by the drop's
   own diameter in world space, while retaining the existing rounding time. */
static void liquid_droplet_shape(const liquid_particle_t *particle,
    float rounding, float *width_scale, float *trail_seconds, float *tail_scale)
{
    float speed = sqrtf(liquid_vec3_dot(particle->velocity, particle->velocity));
    float diameter, max_trail;
    *width_scale = particle->satellite ? 0.95f : 1.10f;
    /* Match the world radius used by liquid_projected_half_width_for_size. */
    diameter = 2.0f * cfg.particle_size * particle->size_scale * *width_scale;
    max_trail = diameter * (particle->secondary ? 1.6f : 2.4f);
    *trail_seconds = particle->satellite ? 0.024f : 0.034f;
    if (speed > 0.00001f && *trail_seconds * speed > max_trail)
        *trail_seconds = max_trail / speed;
    *trail_seconds *= 1.0f - rounding;
    if (*trail_seconds > particle->age) *trail_seconds = particle->age;
    *tail_scale = 0.76f + 0.24f * rounding;
}

static void liquid_impact_footprint(const liquid_particle_t *p,
    float *radius_scale, float *aspect)
{
    float settle = liquid_smoothstep_unit(p->collision_age / 0.08f);
    *radius_scale = 0.90f + p->impact_strength * (0.25f + 0.15f * settle);
    *aspect = 1.0f + 0.45f * p->impact_grazing * (1.0f - settle);
}

/* A relaxed droplet needs a diameter-long quad, not a zero-length trail.
   Expand short trails around the particle center with equal end radii. */
static void liquid_round_droplet_extent(float radius,
    float *tail_x, float *tail_y, float *tail_depth,
    float *head_x, float *head_y, float head_depth)
{
    float dx = *head_x - *tail_x, dy = *head_y - *tail_y;
    float length = sqrtf(dx * dx + dy * dy);
    float extension;
    if (length >= radius * 2.0f) return;
    if (length > 0.0001f) { dx /= length; dy /= length; }
    else { dx = 0.0f; dy = -1.0f; }
    extension = radius - length * 0.5f;
    *tail_x -= dx * extension;
    *tail_y -= dy * extension;
    *head_x += dx * extension;
    *head_y += dy * extension;
    *tail_depth += (head_depth - *tail_depth) * (extension / radius);
}

/* The rendered root is attached to the exact sampled source. Particle birth
   interpolation smooths the moving flow; filtering this anchor creates a gap. */
static int liquid_model_nozzle(const liquid_emitter_t *emitter, float position[3])
{
    if (!position || !emitter || !emitter->transform_valid ||
        !emitter->emission_id || emitter->person_index < 1 ||
        emitter->person_index > LIQUID_MODEL_EMITTER_COUNT) return 0;
    memcpy(position, emitter->current_position, sizeof(float) * 3);
    return 1;
}
static float liquid_projected_half_width_for_size(
    float base_size, float depth, UINT height, float projection_y,
    float size_scale, float stream_scale)
{
    float half_width;
    if (depth <= 0.01f) return 0.0f;
    half_width = base_size * 0.5f * projection_y *
                 (float)height / depth * size_scale * stream_scale;
    if (half_width < 1.15f) half_width = 1.15f;
    if (half_width > 18.0f) half_width = 18.0f;
    return half_width;
}

static float liquid_projected_half_width(float depth, UINT height,
                                         float projection_y,
                                         float size_scale,
                                         float stream_scale)
{
    return liquid_projected_half_width_for_size(
        cfg.particle_size, depth, height, projection_y,
        size_scale, stream_scale);
}

static float liquid_catmull_rom(float p0, float p1, float p2, float p3,
                                float t, float tangent_scale)
{
    float t2 = t * t;
    float t3 = t2 * t;
    float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
    float h10 = t3 - 2.0f * t2 + t;
    float h01 = -2.0f * t3 + 3.0f * t2;
    float h11 = t3 - t2;
    float m1 = (p2 - p0) * tangent_scale;
    float m2 = (p3 - p1) * tangent_scale;
    return h00 * p1 + h10 * m1 + h01 * p2 + h11 * m2;
}

static int liquid_append_smooth_ribbon(
    liquid_d3d11_vertex_t *vertices, UINT *vertex_count,
    const liquid_curve_point_t *points, int point_count,
    float viewport_width, float viewport_height)
{
    float left_x[LIQUID_CURVE_SAMPLE_CAP];
    float left_y[LIQUID_CURVE_SAMPLE_CAP];
    float right_x[LIQUID_CURVE_SAMPLE_CAP];
    float right_y[LIQUID_CURVE_SAMPLE_CAP];
    int i;
    int appended = 0;
    if (!vertices || !vertex_count || !points || point_count < 2 ||
        point_count > LIQUID_CURVE_SAMPLE_CAP) return 0;
    for (i = 0; i < point_count; i++) {
        int previous = i > 0 ? i - 1 : i;
        int next = i + 1 < point_count ? i + 1 : i;
        float dx = points[next].x - points[previous].x;
        float dy = points[next].y - points[previous].y;
        float length = (float)sqrt((double)(dx * dx + dy * dy));
        float nx, ny;
        if (length < 0.15f) {
            if (i > 0) {
                dx = points[i].x - points[i - 1].x;
                dy = points[i].y - points[i - 1].y;
                length = (float)sqrt((double)(dx * dx + dy * dy));
            }
            if (length < 0.15f) { dx = 0.0f; dy = -1.0f; length = 1.0f; }
        }
        nx = -dy / length;
        ny = dx / length;
        left_x[i] = points[i].x + nx * points[i].half_width;
        left_y[i] = points[i].y + ny * points[i].half_width;
        right_x[i] = points[i].x - nx * points[i].half_width;
        right_y[i] = points[i].y - ny * points[i].half_width;
    }
    for (i = 0; i < point_count - 1; i++) {
        liquid_d3d11_vertex_t *v;
        float dx = points[i + 1].x - points[i].x;
        float dy = points[i + 1].y - points[i].y;
        float length = (float)sqrt((double)(dx * dx + dy * dy));
        if (length < 0.15f || length > 90.0f ||
            *vertex_count + 6 > LIQUID_D3D11_VERTEX_CAP) continue;
        v = &vertices[*vertex_count];
        liquid_set_d3d11_vertex(
            &v[0], left_x[i], left_y[i], points[i].screen_depth,
            0.0f, -1.0f, viewport_width, viewport_height, points[i].alpha);
        liquid_set_d3d11_vertex(
            &v[1], left_x[i + 1], left_y[i + 1],
            points[i + 1].screen_depth, 0.0f, -1.0f,
            viewport_width, viewport_height, points[i + 1].alpha);
        liquid_set_d3d11_vertex(
            &v[2], right_x[i + 1], right_y[i + 1],
            points[i + 1].screen_depth, 0.0f, 1.0f,
            viewport_width, viewport_height, points[i + 1].alpha);
        v[3] = v[0];
        v[4] = v[2];
        liquid_set_d3d11_vertex(
            &v[5], right_x[i], right_y[i], points[i].screen_depth,
            0.0f, 1.0f, viewport_width, viewport_height, points[i].alpha);
        v[0].color[1] = v[3].color[1] = v[5].color[1] = points[i].base_alpha;
        v[1].color[1] = v[2].color[1] = v[4].color[1] = points[i + 1].base_alpha;
        *vertex_count += 6;
        appended++;
    }
    return appended;
}

static int liquid_append_capsule(
    liquid_d3d11_vertex_t *vertices, UINT *vertex_count,
    float x0, float y0, float depth0, float width0,
    float x1, float y1, float depth1, float width1,
    float alpha, float viewport_width, float viewport_height, float base_alpha)
{
    liquid_d3d11_vertex_t *v;
    float dx, dy, length, nx, ny;
    if (!vertices || !vertex_count ||
        *vertex_count + 6 > LIQUID_D3D11_VERTEX_CAP) return 0;
    dx = x1 - x0;
    dy = y1 - y0;
    length = (float)sqrt((double)(dx * dx + dy * dy));
    if (length < 0.15f) return 0;
    if (length > 68.0f) {
        x0 = x1 - dx / length * 68.0f;
        y0 = y1 - dy / length * 68.0f;
        dx = x1 - x0;
        dy = y1 - y0;
        length = 68.0f;
    }
    nx = -dy / length;
    ny = dx / length;
    v = &vertices[*vertex_count];
    liquid_set_d3d11_vertex(&v[0], x0 + nx * width0, y0 + ny * width0,
                            depth0,
                            -1.0f, -1.0f, viewport_width, viewport_height,
                            alpha);
    liquid_set_d3d11_vertex(&v[1], x1 + nx * width1, y1 + ny * width1,
                            depth1,
                            1.0f, -1.0f, viewport_width, viewport_height,
                            alpha);
    liquid_set_d3d11_vertex(&v[2], x1 - nx * width1, y1 - ny * width1,
                            depth1,
                            1.0f, 1.0f, viewport_width, viewport_height,
                            alpha);
    v[3] = v[0];
    v[4] = v[2];
    liquid_set_d3d11_vertex(&v[5], x0 - nx * width0, y0 - ny * width0,
                            depth0,
                            -1.0f, 1.0f, viewport_width, viewport_height,
                            alpha);
    {
        int i;
        for (i = 0; i < 6; i++) v[i].color[1] = base_alpha;
    }
    *vertex_count += 6;
    return 1;
}

static void liquid_append_contact_connections(liquid_d3d11_vertex_t *vertices,
    UINT *vertex_count, UINT width, UINT height, float aspect, float tan_half_fov,
    int exact_projection, const D3DMATRIX *projection, const D3DVIEWPORT8 *viewport,
    float projection_y)
{
    int i;
    if (!cfg.collision_connect_droplets) return;
    for (i = 0; i < cfg.particle_limit; i++) {
        const liquid_particle_t *a = &liquid_particles[i], *b;
        liquid_curve_point_t points[5] = {{0}};
        float alpha, radius_a, radius_b, unused_aspect;
        int j, axis;
        if (!a->contact_link_order || a->contact_link_index < 0 ||
            a->contact_link_index >= cfg.particle_limit) continue;
        b = &liquid_particles[a->contact_link_index];
        if (b->spawn_order != a->contact_link_order ||
            b->contact_successor_order != a->spawn_order ||
            !liquid_contact_pair_compatible(a, b)) continue;
        alpha = fminf(liquid_particle_visual_alpha(a), liquid_particle_visual_alpha(b)) *
                cfg.contact_droplet_opacity;
        if (alpha < 0.0005f) continue;
        /* Optional necks must leave room for the actual droplet footprints. */
        if (*vertex_count + 24u + (UINT)cfg.particle_limit * 6u > LIQUID_D3D11_VERTEX_CAP) break;
        liquid_impact_footprint(a, &radius_a, &unused_aspect);
        liquid_impact_footprint(b, &radius_b, &unused_aspect);
        radius_a *= a->size_scale * (1.0f + cfg.collision_surface_spread *
            sqrtf(liquid_clamp_unit(a->collision_age / cfg.collision_surface_hold)));
        radius_b *= b->size_scale * (1.0f + cfg.collision_surface_spread *
            sqrtf(liquid_clamp_unit(b->collision_age / cfg.collision_surface_hold)));
        for (j = 0; j < 5; j++) {
            float t = j * 0.25f, world[3], depth;
            float waist = 2.0f * t - 1.0f;
            float neck = cfg.collision_connection_thickness +
                (1.0f - cfg.collision_connection_thickness) * waist * waist;
            for (axis = 0; axis < 3; axis++)
                world[axis] = a->position[axis] + (b->position[axis] - a->position[axis]) * t;
            if (!liquid_project_world_d3d11(world, width, height, aspect, tan_half_fov,
                exact_projection, projection, viewport, &points[j].x, &points[j].y,
                &depth, &points[j].screen_depth, NULL)) break;
            points[j].half_width = liquid_projected_half_width_for_size(
                cfg.contact_droplet_size, depth, height, projection_y,
                radius_a + (radius_b - radius_a) * t, 1.18f) * neck;
            points[j].alpha = alpha;
        }
        if (j == 5) liquid_append_smooth_ribbon(vertices, vertex_count, points, 5,
                                               (float)width, (float)height);
    }
}

static void liquid_draw_particles_d3d11_target(
    IDXGISwapChain *swap_chain, ID3D11DeviceContext *provided_context,
    ID3D11RenderTargetView *provided_target,
    ID3D11DepthStencilView *provided_depth,
    UINT provided_width, UINT provided_height)
{
    static const GUID liquid_iid_device = {
        0xdb6f6ddb, 0xac77, 0x4e88,
        {0x82, 0x53, 0x81, 0x9d, 0xf9, 0xbb, 0xf1, 0x40}
    };
    static const GUID liquid_iid_texture2d = {
        0x6f15aaf2, 0xd208, 0x4e89,
        {0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c}
    };
    static liquid_d3d11_vertex_t vertices[LIQUID_D3D11_VERTEX_CAP];
    unsigned char processed[LIQUID_PARTICLE_CAP];
    unsigned char ribbon_rendered[LIQUID_PARTICLE_CAP];
    unsigned char ribbon_participating[LIQUID_PARTICLE_CAP];
    unsigned char ribbon_occluded[LIQUID_PARTICLE_CAP];
    float droplet_blend[LIQUID_PARTICLE_CAP];
    ID3D11Device *device = NULL;
    ID3D11DeviceContext *context = NULL;
    ID3D11Texture2D *back_buffer = NULL;
    ID3D11RenderTargetView *render_target = NULL;
    ID3D11RenderTargetView *old_render_target = NULL;
    ID3D11DepthStencilView *old_depth_view = NULL;
    ID3D11DepthStencilView *render_depth_view = NULL;
    ID3D11BlendState *old_blend_state = NULL;
    ID3D11DepthStencilState *old_depth_state = NULL;
    ID3D11RasterizerState *old_rasterizer_state = NULL;
    ID3D11InputLayout *old_input_layout = NULL;
    ID3D11Buffer *old_vertex_buffer = NULL;
    ID3D11VertexShader *old_vertex_shader = NULL;
    ID3D11PixelShader *old_pixel_shader = NULL;
    ID3D11GeometryShader *old_geometry_shader = NULL;
    ID3D11HullShader *old_hull_shader = NULL;
    ID3D11DomainShader *old_domain_shader = NULL;
    DXGI_SWAP_CHAIN_DESC description;
    D3D11_MAPPED_SUBRESOURCE mapped;
    D3D11_VIEWPORT render_viewport;
    D3DMATRIX frame_projection;
    D3DVIEWPORT8 frame_viewport;
    UINT width, height;
    UINT vertex_count = 0;
    UINT exact_projection_count = 0;
    UINT stride = sizeof(liquid_d3d11_vertex_t);
    UINT offset = 0;
    UINT old_stride = 0;
    UINT old_offset = 0;
    UINT old_sample_mask = 0xffffffffu;
    UINT old_stencil_ref = 0;
    UINT old_viewport_count = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    D3D11_VIEWPORT old_viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    D3D11_PRIMITIVE_TOPOLOGY old_topology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    FLOAT blend_factor[4] = {0, 0, 0, 0};
    FLOAT old_blend_factor[4] = {0, 0, 0, 0};
    float aspect;
    float tan_half_fov = 0.41421356f;
    float projection_y = 2.41421356f;
    int exact_projection_available;
    int state_captured = 0;
    int depth_test_active = 0;
    int i;
    if ((!swap_chain && (!provided_context || !provided_target)) ||
        !cfg.liquids_enabled || !liquid_has_active_particles()) return;
    if (provided_context && provided_target) {
        UINT actual_width = 0;
        UINT actual_height = 0;
        static UINT logged_reported_width;
        static UINT logged_reported_height;
        static UINT logged_actual_width;
        static UINT logged_actual_height;
        width = provided_width;
        height = provided_height;
        if (liquid_d3d11_target_size(
                provided_target, &actual_width, &actual_height)) {
            if (provided_width != actual_width ||
                provided_height != actual_height) {
                if (logged_reported_width != provided_width ||
                    logged_reported_height != provided_height ||
                    logged_actual_width != actual_width ||
                    logged_actual_height != actual_height) {
                    log_line("liquid dynamic resolution mapping scene=%ux%u color_target=%ux%u scale=(%.4f,%.4f)",
                             provided_width, provided_height,
                             actual_width, actual_height,
                             actual_width ?
                                (double)provided_width / actual_width : 1.0,
                             actual_height ?
                                (double)provided_height / actual_height : 1.0);
                    logged_reported_width = provided_width;
                    logged_reported_height = provided_height;
                    logged_actual_width = actual_width;
                    logged_actual_height = actual_height;
                }
            }
            width = actual_width;
            height = actual_height;
        }
    } else {
        memset(&description, 0, sizeof(description));
        if (FAILED(IDXGISwapChain_GetDesc(swap_chain, &description))) return;
        width = description.BufferDesc.Width;
        height = description.BufferDesc.Height;
        if ((!width || !height) && description.OutputWindow) {
            RECT client;
            if (GetClientRect(description.OutputWindow, &client)) {
                width = (UINT)(client.right - client.left);
                height = (UINT)(client.bottom - client.top);
            }
        }
    }
    if (!width || !height) return;
    aspect = (float)width / (float)height;
    exact_projection_available = InterlockedCompareExchange(
        &captured_d3d_projection_valid, 0, 0) != 0;
    if (!exact_projection_available && hook5_get_fov_multiplier) {
        float hook5_fov_multiplier = 1.0f;
        if (hook5_get_fov_multiplier(1u, &hook5_fov_multiplier) &&
            _finite(hook5_fov_multiplier) &&
            hook5_fov_multiplier > 0.01f) {
            tan_half_fov *= hook5_fov_multiplier;
            projection_y /= hook5_fov_multiplier;
        }
    }
    if (exact_projection_available) {
        memcpy(&frame_projection, &captured_d3d_projection,
               sizeof(frame_projection));
        memcpy(&frame_viewport, &captured_d3d_viewport,
               sizeof(frame_viewport));
        if (!frame_viewport.Width || !frame_viewport.Height)
            exact_projection_available = 0;
    }
    if (exact_projection_available && _finite(frame_projection._22) &&
        fabs(frame_projection._22) > 0.001f)
        projection_y = (float)fabs(frame_projection._22);
    memset(processed, 0, sizeof(processed));
    memset(ribbon_rendered, 0, sizeof(ribbon_rendered));
    memset(ribbon_participating, 0, sizeof(ribbon_participating));
    memset(ribbon_occluded, 0, sizeof(ribbon_occluded));
    memset(droplet_blend, 0, sizeof(droplet_blend));
    /* Build one smooth ribbon for every coherent model pulse.  The actual
       particles remain the physical control points, so gravity bends the
       stream naturally while Catmull-Rom interpolation removes the old
       segmented-droplet appearance. */
    for (i = 0; i < cfg.particle_limit; i++) {
        liquid_particle_t *seed = &liquid_particles[i];
        int group_indices[LIQUID_PARTICLE_CAP];
        liquid_curve_point_t points[LIQUID_PARTICLE_CAP + 1];
        int group_count = 0;
        int point_count = 0;
        liquid_curve_point_t smooth_points[LIQUID_CURVE_SAMPLE_CAP];
        float group_nozzle_position[3] = {0.0f, 0.0f, 0.0f};
        int smooth_count = 0;
        int group_nozzle_valid = 0;
        int j, k;
        if (processed[i] || !seed->active || seed->source_kind != 1 ||
            seed->satellite || seed->age > cfg.model_stream_cohesion)
            continue;
        for (j = 0; j < cfg.particle_limit; j++) {
            liquid_particle_t *candidate = &liquid_particles[j];
            if (!candidate->active || candidate->source_kind != 1 ||
                candidate->satellite ||
                (!candidate->collided &&
                 candidate->age > cfg.model_stream_cohesion) ||
                candidate->emission_id != seed->emission_id ||
                candidate->stream_id != seed->stream_id ||
                candidate->pulse_index != seed->pulse_index) continue;
            group_indices[group_count++] = j;
            processed[j] = 1;
        }
        for (j = 1; j < group_count; j++) {
            int key = group_indices[j];
            int insert = j - 1;
            while (insert >= 0 &&
                   liquid_particles[group_indices[insert]].spawn_order <
                   liquid_particles[key].spawn_order) {
                group_indices[insert + 1] = group_indices[insert];
                insert--;
            }
            group_indices[insert + 1] = key;
        }
        {
            liquid_emitter_t *emitter =
                liquid_model_emitter_for_emission(seed->emission_id);
            if (group_count > 0 && emitter && emitter->transform_valid &&
                emitter->pulse_emitting &&
                emitter->stream_id == seed->stream_id &&
                emitter->current_pulse_index == seed->pulse_index) {
                float depth;
                int used_exact = 0;
                liquid_particle_t *newest =
                    &liquid_particles[group_indices[0]];
                if (liquid_model_nozzle(
                        emitter, group_nozzle_position)) {
                    group_nozzle_valid = 1;
                    if (liquid_project_world_d3d11(
                            group_nozzle_position, width, height, aspect,
                            tan_half_fov, exact_projection_available,
                            &frame_projection, &frame_viewport,
                            &points[point_count].x,
                            &points[point_count].y,
                            &depth, &points[point_count].screen_depth,
                            &used_exact)) {
                        points[point_count].half_width =
                            liquid_projected_half_width(
                                depth, height, projection_y,
                                newest->size_scale,
                                1.22f * cfg.stream_thickness);
                        liquid_stream_alpha(newest, 0.0f, 0.0f,
                            &points[point_count].alpha, &points[point_count].base_alpha);
                        points[point_count].distance_from_nozzle = 0.0f;
                        points[point_count].emission_time =
                            (float)(GetTickCount() - emitter->start_tick) * 0.001f;
                        points[point_count].particle_index = -1;
                        point_count++;
                        if (used_exact) exact_projection_count++;
                    }
                }
            }
        }
        for (j = 0; j < group_count; j++) {
            liquid_particle_t *particle =
                &liquid_particles[group_indices[j]];
            float breakup = liquid_model_breakup_blend(particle);
            float bead;
            float taper;
            float depth;
            float distance_from_nozzle = 10000.0f;
            int used_exact = 0;
            if (!liquid_project_world_d3d11(
                    particle->position, width, height, aspect, tan_half_fov,
                    exact_projection_available, &frame_projection,
                    &frame_viewport, &points[point_count].x,
                    &points[point_count].y, &depth,
                    &points[point_count].screen_depth,
                    &used_exact)) {
                if (particle->collided) break;
                continue;
            }
            bead = 0.5f + 0.5f * liquid_stream_bulge_at(
                particle->emission_time, particle->pulse_index);
            taper = 1.0f - breakup * (0.38f +
                                     0.20f * (1.0f - bead));
            points[point_count].half_width =
                liquid_projected_half_width(
                    depth, height, projection_y,
                    particle->size_scale,
                    1.22f * cfg.stream_thickness *
                    (0.72f + 0.28f * (1.0f - liquid_clamp_unit(
                        particle->age / cfg.model_stream_cohesion))) *
                    fmaxf(0.55f, taper / sqrtf(fmaxf(1.0f, particle->stream_stretch))));
            if (group_nozzle_valid) {
                float dx = particle->position[0] - group_nozzle_position[0];
                float dy = particle->position[1] - group_nozzle_position[1];
                float dz = particle->position[2] - group_nozzle_position[2];
                distance_from_nozzle = (float)sqrt((double)(
                    dx * dx + dy * dy + dz * dz));
            }
            liquid_stream_alpha(particle, distance_from_nozzle, breakup,
                &points[point_count].alpha, &points[point_count].base_alpha);
            points[point_count].distance_from_nozzle =
                distance_from_nozzle;
            points[point_count].particle_index = group_indices[j];
            points[point_count].emission_time = particle->emission_time;
            droplet_blend[group_indices[j]] = breakup;
            point_count++;
            if (used_exact) exact_projection_count++;
            if (particle->collided) {
                int hidden;
                /* The ordered control points run from the nozzle outwards.
                   A physical contact is therefore the final visible ribbon
                   point.  Without a same-size depth view at supersampled
                   resolutions, suppress only later airborne points; landed
                   contacts remain available to the impact renderer. */
                for (hidden = j + 1; hidden < group_count; hidden++) {
                    int hidden_index = group_indices[hidden];
                    if (!liquid_particles[hidden_index].collided)
                        ribbon_occluded[hidden_index] = 1;
                }
                break;
            }
        }
        if (point_count < 2) continue;
        points[point_count - 1].half_width *= 1.10f;
        if (points[point_count - 1].half_width > 18.0f)
            points[point_count - 1].half_width = 18.0f;
        smooth_points[smooth_count++] = points[0];
        for (j = 0; j < point_count - 1; j++) {
            liquid_curve_point_t *p0 = &points[j > 0 ? j - 1 : j];
            liquid_curve_point_t *p1 = &points[j];
            liquid_curve_point_t *p2 = &points[j + 1];
            liquid_curve_point_t *p3 =
                &points[j + 2 < point_count ? j + 2 : j + 1];
            float span_x = p2->x - p1->x;
            float span_y = p2->y - p1->y;
            float span = (float)sqrt((double)(span_x * span_x +
                                              span_y * span_y));
            if (span > 260.0f) continue;
            for (k = 1; k <= cfg.stream_curve_smoothness; k++) {
                float t = (float)k /
                          (float)cfg.stream_curve_smoothness;
                float material_time = p1->emission_time +
                    (p2->emission_time - p1->emission_time) * t;
                float next_distance = p1->distance_from_nozzle +
                    (p2->distance_from_nozzle -
                     p1->distance_from_nozzle) * t;
                float wobble = liquid_stream_wobble_envelope(
                    next_distance);
                float bulge = 1.0f + wobble * cfg.stream_bulge *
                    liquid_stream_bulge_at(material_time, seed->pulse_index);
                float linear_x = p1->x + (p2->x - p1->x) * t;
                float linear_y = p1->y + (p2->y - p1->y) * t;
                float curved_x = liquid_catmull_rom(
                    p0->x, p1->x, p2->x, p3->x, t,
                    cfg.stream_curve_tension);
                float curved_y = liquid_catmull_rom(
                    p0->y, p1->y, p2->y, p3->y, t,
                    cfg.stream_curve_tension);
                float next_x = linear_x + (curved_x - linear_x) * wobble;
                float next_y = linear_y + (curved_y - linear_y) * wobble;
                float next_width = liquid_catmull_rom(
                    p0->half_width, p1->half_width,
                    p2->half_width, p3->half_width, t,
                    cfg.stream_curve_tension) * bulge;
                float next_alpha = liquid_catmull_rom(
                    p0->alpha, p1->alpha, p2->alpha, p3->alpha, t,
                    cfg.stream_curve_tension);
                /* Curve X/Y and width for a smooth stream, but never apply
                   an overshooting spline to device depth. Perspective depth
                   between the two projected control points is monotonic;
                   Catmull-Rom depth can temporarily place a ribbon section
                   behind distant room geometry even though both endpoints
                   are in front of it. */
                float next_depth = p1->screen_depth +
                    (p2->screen_depth - p1->screen_depth) * t;
                if (next_width < 1.1f) next_width = 1.1f;
                if (next_width > 18.0f) next_width = 18.0f;
                if (next_alpha < 0.0f) next_alpha = 0.0f;
                if (next_alpha > 0.80f) next_alpha = 0.80f;
                if (next_depth < 0.0f) next_depth = 0.0f;
                if (next_depth > 1.0f) next_depth = 1.0f;
                if (smooth_count >= LIQUID_CURVE_SAMPLE_CAP) break;
                smooth_points[smooth_count].x = next_x;
                smooth_points[smooth_count].y = next_y;
                smooth_points[smooth_count].screen_depth = next_depth;
                smooth_points[smooth_count].half_width = next_width;
                smooth_points[smooth_count].alpha = next_alpha;
                /* Linear interpolation cannot overshoot an absolute opacity. */
                smooth_points[smooth_count].base_alpha = p1->base_alpha +
                    (p2->base_alpha - p1->base_alpha) * t;
                smooth_points[smooth_count].distance_from_nozzle =
                    next_distance;
                smooth_points[smooth_count].emission_time = material_time;
                smooth_points[smooth_count].particle_index =
                    k == cfg.stream_curve_smoothness ?
                    p2->particle_index : -1;
                smooth_count++;
            }
        }
        if (liquid_append_smooth_ribbon(
                vertices, &vertex_count, smooth_points, smooth_count,
                (float)width, (float)height) > 0) {
            for (j = 0; j < point_count; j++) {
                int particle_index = points[j].particle_index;
                if (particle_index < 0) continue;
                ribbon_participating[particle_index] = 1;
                if (!liquid_particles[particle_index].collided &&
                    droplet_blend[particle_index] <= 0.001f)
                    ribbon_rendered[particle_index] = 1;
            }
        }
        {
            liquid_curve_point_t *before_tip =
                &smooth_points[smooth_count > 1 ? smooth_count - 2 : 0];
            liquid_curve_point_t *tip =
                &smooth_points[smooth_count > 0 ? smooth_count - 1 : 0];
            float dx = tip->x - before_tip->x;
            float dy = tip->y - before_tip->y;
            float length = (float)sqrt((double)(dx * dx + dy * dy));
            if (length > 0.1f) {
                float ux = dx / length;
                float uy = dy / length;
                float radius = tip->half_width;
                if ((tip->particle_index < 0 ||
                     !liquid_particles[tip->particle_index].collided) &&
                    liquid_append_capsule(
                        vertices, &vertex_count,
                        tip->x - ux * radius * 0.65f,
                        tip->y - uy * radius * 0.65f,
                        before_tip->screen_depth,
                        radius * 0.92f,
                        tip->x + ux * radius * 0.80f,
                        tip->y + uy * radius * 0.80f,
                        tip->screen_depth,
                        radius * 1.10f,
                        tip->alpha,
                        (float)width, (float)height, tip->base_alpha) &&
                    tip->particle_index >= 0 &&
                    droplet_blend[tip->particle_index] <= 0.001f)
                    ribbon_rendered[tip->particle_index] = 1;
            }
        }
    }

    liquid_append_contact_connections(vertices, &vertex_count, width, height,
        aspect, tan_half_fov, exact_projection_available, &frame_projection,
        &frame_viewport, projection_y);

    /* Particles outside their cohesion window, plus the lighter satellite
       spray, become independent tapered droplets. */
    for (i = 0; i < cfg.particle_limit; i++) {
        liquid_particle_t *particle = &liquid_particles[i];
        float trail_world[3];
        float screen_x, screen_y, trail_x, trail_y;
        float depth, trail_depth;
        float screen_depth, trail_screen_depth;
        float half_width, alpha;
        float trail_seconds, rounding, drop_width_scale, tail_scale;
        int used_exact = 0;
        if (!particle->active || ribbon_rendered[i] ||
            ribbon_occluded[i]) continue;
        if (particle->collided) {
            float impact_world[3];
            float impact_x, impact_y, impact_depth, impact_screen_depth;
            float direction_x, direction_y, direction_length;
            float contact_progress, impact_scale, impact_aspect;
            int axis;
            if (!liquid_project_world_d3d11(
                    particle->position, width, height, aspect,
                    tan_half_fov, exact_projection_available,
                    &frame_projection, &frame_viewport,
                    &screen_x, &screen_y, &depth, &screen_depth,
                    &used_exact))
                continue;
            if (used_exact) exact_projection_count++;
            for (axis = 0; axis < 3; axis++)
                impact_world[axis] = particle->position[axis] -
                    particle->impact_tangent[axis] * 0.018f;
            used_exact = 0;
            if (!liquid_project_world_d3d11(
                    impact_world, width, height, aspect, tan_half_fov,
                    exact_projection_available, &frame_projection,
                    &frame_viewport, &impact_x, &impact_y, &impact_depth,
                    &impact_screen_depth, &used_exact)) {
                impact_x = screen_x;
                impact_y = screen_y - 1.0f;
                impact_screen_depth = screen_depth;
            } else if (used_exact) {
                exact_projection_count++;
            }
            direction_x = screen_x - impact_x;
            direction_y = screen_y - impact_y;
            direction_length = (float)sqrt((double)(
                direction_x * direction_x + direction_y * direction_y));
            if (direction_length < 0.25f) {
                float angle = (float)(particle->spawn_order % 17u) *
                              0.3695991f;
                direction_x = (float)cos((double)angle);
                direction_y = (float)sin((double)angle);
                direction_length = 1.0f;
            }
            direction_x /= direction_length;
            direction_y /= direction_length;
            contact_progress = cfg.collision_surface_hold > 0.0f ?
                particle->collision_age / cfg.collision_surface_hold : 1.0f;
            if (contact_progress < 0.0f) contact_progress = 0.0f;
            if (contact_progress > 1.0f) contact_progress = 1.0f;
            half_width = liquid_projected_half_width_for_size(
                cfg.contact_droplet_size, depth, height, projection_y,
                particle->size_scale, 1.18f);
            half_width *= 1.0f + cfg.collision_surface_spread *
                          (float)sqrt((double)contact_progress);
            liquid_impact_footprint(particle, &impact_scale, &impact_aspect);
            half_width *= impact_scale;
            alpha = liquid_particle_visual_alpha(particle) *
                    cfg.contact_droplet_opacity;
            if (alpha < 0.0005f) continue;
            /* A short angle-dependent flattening settles into a fixed round
               footprint. Area stays stable as the impact ellipse rounds out;
               continued spreading still follows surface_spread. */
            liquid_append_capsule(
                vertices, &vertex_count,
                screen_x - direction_x * half_width * sqrtf(impact_aspect),
                screen_y - direction_y * half_width * sqrtf(impact_aspect),
                screen_depth, half_width / sqrtf(impact_aspect),
                screen_x + direction_x * half_width * sqrtf(impact_aspect),
                screen_y + direction_y * half_width * sqrtf(impact_aspect),
                screen_depth, half_width / sqrtf(impact_aspect),
                alpha, (float)width, (float)height, 0.0f);
            continue;
        }
        rounding = liquid_droplet_rounding(particle);
        liquid_droplet_shape(particle, rounding, &drop_width_scale,
                             &trail_seconds, &tail_scale);
        trail_world[0] = particle->position[0] -
                         particle->velocity[0] * trail_seconds;
        trail_world[1] = particle->position[1] -
                         particle->velocity[1] * trail_seconds;
        trail_world[2] = particle->position[2] -
                         particle->velocity[2] * trail_seconds;
        if (!liquid_project_world_d3d11(
                particle->position, width, height, aspect, tan_half_fov,
                exact_projection_available, &frame_projection,
                &frame_viewport, &screen_x, &screen_y, &depth,
                &screen_depth, &used_exact))
            continue;
        if (used_exact) exact_projection_count++;
        used_exact = 0;
        if (!liquid_project_world_d3d11(
                trail_world, width, height, aspect, tan_half_fov,
                exact_projection_available, &frame_projection,
                &frame_viewport, &trail_x, &trail_y,
                &trail_depth, &trail_screen_depth, &used_exact)) continue;
        if (used_exact) exact_projection_count++;
        half_width = liquid_projected_half_width(
            depth, height, projection_y, particle->size_scale,
            drop_width_scale);
        liquid_round_droplet_extent(half_width,
            &trail_x, &trail_y, &trail_screen_depth,
            &screen_x, &screen_y, screen_depth);
        alpha = liquid_particle_visual_alpha(particle) *
                cfg.stream_opacity;
        if (ribbon_participating[i])
            alpha *= droplet_blend[i];
        if (alpha < 0.0005f) continue;
        liquid_append_capsule(
            vertices, &vertex_count,
            trail_x, trail_y, trail_screen_depth, half_width * tail_scale,
            screen_x, screen_y, screen_depth, half_width, alpha,
            (float)width, (float)height, 0.0f);
    }
    if (!vertex_count) return;
    if (provided_context && provided_target) {
        context = provided_context;
        ID3D11DeviceContext_AddRef(context);
        ID3D11DeviceContext_GetDevice(context, &device);
        render_target = provided_target;
        ID3D11RenderTargetView_AddRef(render_target);
    } else {
        if (FAILED(IDXGISwapChain_GetDevice(swap_chain, &liquid_iid_device,
                                            (void**)&device)) || !device)
            return;
        ID3D11Device_GetImmediateContext(device, &context);
        if (!context) goto cleanup;
        if (FAILED(IDXGISwapChain_GetBuffer(swap_chain, 0,
                                            &liquid_iid_texture2d,
                                            (void**)&back_buffer)) ||
            !back_buffer) goto cleanup;
        if (FAILED(ID3D11Device_CreateRenderTargetView(
                device, (ID3D11Resource*)back_buffer, NULL,
                &render_target)) || !render_target) goto cleanup;
    }
    if (!device || !liquid_ensure_d3d11_resources(device)) goto cleanup;
    memset(&mapped, 0, sizeof(mapped));
    if (FAILED(ID3D11DeviceContext_Map(
            context, (ID3D11Resource*)liquid_d3d11_vertex_buffer, 0,
            D3D11_MAP_WRITE_DISCARD, 0, &mapped)) || !mapped.pData)
        goto cleanup;
    memcpy(mapped.pData, vertices,
           vertex_count * sizeof(liquid_d3d11_vertex_t));
    ID3D11DeviceContext_Unmap(
        context, (ID3D11Resource*)liquid_d3d11_vertex_buffer, 0);
    memset(&render_viewport, 0, sizeof(render_viewport));
    render_viewport.Width = (float)width;
    render_viewport.Height = (float)height;
    render_viewport.MinDepth = 0.0f;
    render_viewport.MaxDepth = 1.0f;

    /* Hook5 shares this immediate context with TK17 and other plugins.  Save
       every pipeline stage touched by this overlay and put it back exactly
       as it was after drawing. */
    ID3D11DeviceContext_OMGetRenderTargets(
        context, 1, &old_render_target, &old_depth_view);
    ID3D11DeviceContext_OMGetBlendState(
        context, &old_blend_state, old_blend_factor, &old_sample_mask);
    ID3D11DeviceContext_OMGetDepthStencilState(
        context, &old_depth_state, &old_stencil_ref);
    ID3D11DeviceContext_RSGetState(context, &old_rasterizer_state);
    ID3D11DeviceContext_RSGetViewports(
        context, &old_viewport_count, old_viewports);
    ID3D11DeviceContext_IAGetInputLayout(context, &old_input_layout);
    ID3D11DeviceContext_IAGetVertexBuffers(
        context, 0, 1, &old_vertex_buffer, &old_stride, &old_offset);
    ID3D11DeviceContext_IAGetPrimitiveTopology(context, &old_topology);
    ID3D11DeviceContext_VSGetShader(
        context, &old_vertex_shader, NULL, NULL);
    ID3D11DeviceContext_PSGetShader(
        context, &old_pixel_shader, NULL, NULL);
    ID3D11DeviceContext_GSGetShader(
        context, &old_geometry_shader, NULL, NULL);
    ID3D11DeviceContext_HSGetShader(
        context, &old_hull_shader, NULL, NULL);
    ID3D11DeviceContext_DSGetShader(
        context, &old_domain_shader, NULL, NULL);
    state_captured = 1;
    depth_test_active = exact_projection_available &&
        ((provided_depth && liquid_d3d11_color_depth_compatible(
              render_target, provided_depth) &&
          (render_depth_view = provided_depth) != NULL) ||
         (old_depth_view && liquid_d3d11_color_depth_compatible(
              render_target, old_depth_view) &&
          (render_depth_view = old_depth_view) != NULL)) &&
        liquid_match_d3d11_scene_depth_state(device, old_depth_state);

    ID3D11DeviceContext_OMSetRenderTargets(
        context, 1, &render_target,
        depth_test_active ? render_depth_view : NULL);
    ID3D11DeviceContext_OMSetBlendState(
        context, liquid_d3d11_blend_state, blend_factor, 0xffffffffu);
    ID3D11DeviceContext_OMSetDepthStencilState(
        context, depth_test_active ? liquid_d3d11_depth_test_state :
                                     liquid_d3d11_depth_state, 0);
    ID3D11DeviceContext_RSSetState(context, liquid_d3d11_rasterizer_state);
    ID3D11DeviceContext_RSSetViewports(context, 1, &render_viewport);
    ID3D11DeviceContext_IASetInputLayout(
        context, liquid_d3d11_input_layout);
    ID3D11DeviceContext_IASetVertexBuffers(
        context, 0, 1, &liquid_d3d11_vertex_buffer, &stride, &offset);
    ID3D11DeviceContext_IASetPrimitiveTopology(
        context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ID3D11DeviceContext_VSSetShader(
        context, liquid_d3d11_vertex_shader, NULL, 0);
    ID3D11DeviceContext_GSSetShader(context, NULL, NULL, 0);
    ID3D11DeviceContext_HSSetShader(context, NULL, NULL, 0);
    ID3D11DeviceContext_DSSetShader(context, NULL, NULL, 0);
    ID3D11DeviceContext_PSSetShader(
        context, liquid_d3d11_pixel_shader, NULL, 0);
    ID3D11DeviceContext_Draw(context, vertex_count, 0);
    {
        static int draw_logged;
        if (!draw_logged) {
            draw_logged = 1;
            log_line("liquid background-isolated ribbon draw active sections=%u viewport=%ux%u exact_projection=%u depth_test=%d premultiplied=1",
                     vertex_count / 6, width, height,
                     exact_projection_count, depth_test_active);
        }
    }
cleanup:
    if (state_captured) {
        ID3D11DeviceContext_OMSetRenderTargets(
            context, 1, &old_render_target, old_depth_view);
        ID3D11DeviceContext_OMSetBlendState(
            context, old_blend_state, old_blend_factor, old_sample_mask);
        ID3D11DeviceContext_OMSetDepthStencilState(
            context, old_depth_state, old_stencil_ref);
        ID3D11DeviceContext_RSSetState(context, old_rasterizer_state);
        if (old_viewport_count)
            ID3D11DeviceContext_RSSetViewports(
                context, old_viewport_count, old_viewports);
        ID3D11DeviceContext_IASetInputLayout(context, old_input_layout);
        ID3D11DeviceContext_IASetVertexBuffers(
            context, 0, 1, &old_vertex_buffer, &old_stride, &old_offset);
        ID3D11DeviceContext_IASetPrimitiveTopology(context, old_topology);
        ID3D11DeviceContext_VSSetShader(
            context, old_vertex_shader, NULL, 0);
        ID3D11DeviceContext_PSSetShader(
            context, old_pixel_shader, NULL, 0);
        ID3D11DeviceContext_GSSetShader(
            context, old_geometry_shader, NULL, 0);
        ID3D11DeviceContext_HSSetShader(
            context, old_hull_shader, NULL, 0);
        ID3D11DeviceContext_DSSetShader(
            context, old_domain_shader, NULL, 0);
    }
    if (old_domain_shader) ID3D11DomainShader_Release(old_domain_shader);
    if (old_hull_shader) ID3D11HullShader_Release(old_hull_shader);
    if (old_geometry_shader) ID3D11GeometryShader_Release(old_geometry_shader);
    if (old_pixel_shader) ID3D11PixelShader_Release(old_pixel_shader);
    if (old_vertex_shader) ID3D11VertexShader_Release(old_vertex_shader);
    if (old_vertex_buffer) ID3D11Buffer_Release(old_vertex_buffer);
    if (old_input_layout) ID3D11InputLayout_Release(old_input_layout);
    if (old_rasterizer_state)
        ID3D11RasterizerState_Release(old_rasterizer_state);
    if (old_depth_state) ID3D11DepthStencilState_Release(old_depth_state);
    if (old_blend_state) ID3D11BlendState_Release(old_blend_state);
    if (old_depth_view) ID3D11DepthStencilView_Release(old_depth_view);
    if (old_render_target) ID3D11RenderTargetView_Release(old_render_target);
    if (render_target) ID3D11RenderTargetView_Release(render_target);
    if (back_buffer) ID3D11Texture2D_Release(back_buffer);
    if (context) ID3D11DeviceContext_Release(context);
    if (device) ID3D11Device_Release(device);
}

#include "liquid_native_renderers.c"

static void __cdecl liquid_hook5_d3d8_present_capture_callback(
    IDirect3DDevice8 *device)
{
    D3DMATRIX projection;
    D3DVIEWPORT8 viewport;
    float hook5_fov_multiplier = 1.0f;
    int live_hook5_fov = 0;
    static int capture_logged;
    static float logged_fov_multiplier;
    static LONG projection_change_logs;
    if (!device || !cfg.liquids_enabled ||
        !liquid_runtime_work_active(GetTickCount()) ||
        FAILED(IDirect3DDevice8_GetTransform(
            device, D3DTS_PROJECTION, &projection)) ||
        FAILED(IDirect3DDevice8_GetViewport(device, &viewport)) ||
        !viewport.Width || !viewport.Height) return;
    if (hook5_get_fov_multiplier && hook5_get_fov_multiplier(
            1u, &hook5_fov_multiplier)) {
        live_hook5_fov = 1;
        projection._11 /= hook5_fov_multiplier;
        projection._22 /= hook5_fov_multiplier;
        if (fabs((double)(hook5_fov_multiplier - logged_fov_multiplier)) >
                0.002 &&
            InterlockedIncrement(&projection_change_logs) <= 16) {
            log_line("liquid Hook5 live FovMultiplier=%.5f corrected_projection=(%.5f,%.5f) d3d8_viewport=%lux%lu",
                     hook5_fov_multiplier, projection._11, projection._22,
                     (unsigned long)viewport.Width,
                     (unsigned long)viewport.Height);
            logged_fov_multiplier = hook5_fov_multiplier;
        }
    }
    memcpy(&captured_d3d_projection, &projection, sizeof(projection));
    memcpy(&captured_d3d_viewport, &viewport, sizeof(viewport));
    MemoryBarrier();
    InterlockedExchange(&captured_d3d_projection_valid, 1);
    if (!capture_logged) {
        capture_logged = 1;
        log_line("liquid Hook5 D3D8 projection capture active viewport=%lux%lu projection=(%.5f,%.5f,%.5f,%.5f) live_fov=%d",
                 (unsigned long)viewport.Width,
                 (unsigned long)viewport.Height,
                 projection._11, projection._22,
                 projection._33, projection._34,
                 live_hook5_fov);
    }
}

static void liquid_capture_hook5_projection(unsigned int width,
                                            unsigned int height)
{
    D3DMATRIX projection;
    D3DVIEWPORT8 viewport;
    static int capture_logged;
    if (!hook5_get_projection_matrix || !width || !height ||
        !hook5_get_projection_matrix(1u, (float *)&projection))
        return;
    memset(&viewport, 0, sizeof(viewport));
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;
    memcpy(&captured_d3d_projection, &projection, sizeof(projection));
    memcpy(&captured_d3d_viewport, &viewport, sizeof(viewport));
    MemoryBarrier();
    InterlockedExchange(&captured_d3d_projection_valid, 1);
    if (!capture_logged) {
        capture_logged = 1;
        log_line("liquid Hook5 native fullscreen projection capture active viewport=%ux%u projection=(%.5f,%.5f,%.5f,%.5f)",
                 width, height, projection._11, projection._22,
                 projection._33, projection._34);
    }
}

static void __cdecl liquid_hook5_present_callback(IDXGISwapChain *swap_chain)
{
    static int callback_logged;
    LONG scene_drawn;
    LONG composite_drawn;
    if (!callback_logged) {
        callback_logged = 1;
        log_line("liquid Hook5 final D3D11 Present callback active swap_chain=%p",
                 swap_chain);
    }
    /* Registration only proves that the bridge exists. It does not prove
       that the current room/pose location exposed its main scene target.
       Consume the per-frame submission flag and use Present as a safe
       fallback whenever no valid camera-sized scene pass was found. */
    scene_drawn = InterlockedExchange(
        &hook5_scene_drawn_since_present, 0);
    composite_drawn = InterlockedExchange(
        &hook5_composite_drawn_since_present, 0);
    if (InterlockedCompareExchange(
            &hook5_d3d11_composite_registered, 0, 0)) {
        /* Never degrade a depth-aware Hook5 installation into a late
           Present overlay.  If final-scene target discovery fails, hiding
           the liquid for that frame is preferable to drawing it through
           every person, wall, and interface element. */
        if (!composite_drawn) {
            static LONG missed_composite_logs;
            if (InterlockedIncrement(&missed_composite_logs) <= 8)
                log_line("liquid skipped unsafe Present overlay because post-lighting depth composite was not dispatched scene_seen=%ld",
                         scene_drawn);
        }
        return;
    }
    if (scene_drawn) return;
    if (!scene_drawn) liquid_simulation_tick();
    liquid_draw_particles_d3d11_target(
        swap_chain, NULL, NULL, NULL, 0, 0);
}

static void __cdecl liquid_hook5_composite_callback(
    ID3D11DeviceContext *context, ID3D11RenderTargetView *render_target,
    ID3D11DepthStencilView *depth_view, unsigned int width,
    unsigned int height)
{
    static int callback_logged;
    liquid_capture_hook5_projection(width, height);
    if (!callback_logged) {
        callback_logged = 1;
        log_line("liquid Hook5 post-lighting D3D11 composition active context=%p target=%p retained_depth=%p viewport=%ux%u",
                 context, render_target, depth_view, width, height);
    }
    InterlockedExchange(&hook5_composite_drawn_since_present, 1);
    if (!liquid_runtime_work_active(GetTickCount())) return;
    liquid_draw_particles_d3d11_target(
        NULL, context, render_target, depth_view, width, height);
}

static void __cdecl liquid_hook5_scene_callback(
    ID3D11DeviceContext *context, ID3D11RenderTargetView *render_target,
    ID3D11DepthStencilView *depth_view, unsigned int width,
    unsigned int height)
{
    static int callback_logged;
    static LONG rejected_target_logs;
    D3DVIEWPORT8 captured_viewport;
    int captured_valid;
    int target_matches_camera = 1;
    liquid_capture_hook5_projection(width, height);
    captured_valid = InterlockedCompareExchange(
        &captured_d3d_projection_valid, 0, 0) != 0;
    if (captured_valid) {
        double target_aspect;
        double camera_aspect;
        double relative_error;
        memcpy(&captured_viewport, &captured_d3d_viewport,
               sizeof(captured_viewport));
        if (captured_viewport.Width && captured_viewport.Height &&
            width && height) {
            target_aspect = (double)width / (double)height;
            camera_aspect = (double)captured_viewport.Width /
                            (double)captured_viewport.Height;
            relative_error = target_aspect > camera_aspect
                ? (target_aspect - camera_aspect) / camera_aspect
                : (camera_aspect - target_aspect) / camera_aspect;
            target_matches_camera = relative_error <= 0.08;
        }
    }
    if (!target_matches_camera) {
        LONG count = InterlockedIncrement(&rejected_target_logs);
        if (count <= 8)
            log_line("liquid ignored offscreen Hook5 scene target viewport=%ux%u camera=%lux%lu; Present fallback remains armed",
                     width, height,
                     (unsigned long)captured_viewport.Width,
                     (unsigned long)captured_viewport.Height);
        return;
    }
    if (!callback_logged) {
        callback_logged = 1;
        log_line("liquid Hook5 depth-tested D3D11 scene submission active context=%p target=%p depth=%p viewport=%ux%u",
                 context, render_target, depth_view, width, height);
    }
    liquid_simulation_tick();
    /* This was the valid camera-sized scene even when no liquid exists.
       Mark it consumed so Present does not repeat the idle simulation. */
    InterlockedExchange(&hook5_scene_drawn_since_present, 1);
    if (!liquid_runtime_work_active(GetTickCount())) return;
    liquid_probe_d3d11_collision_depth(depth_view);
    liquid_depth_collision_tick(context, depth_view);
    /* When Hook5 Extended exposes its verified post-lighting callback, this
       early scene stage is collision/depth capture only. Drawing colour here
       writes into Hook5's deferred material buffers, allowing later room
       lighting/material data to stripe or recolour the liquid. */
    if (!InterlockedCompareExchange(
            &hook5_d3d11_composite_registered, 0, 0))
        liquid_draw_particles_d3d11_target(
            NULL, context, render_target, depth_view, width, height);
}

static int liquid_try_register_hook5_present_callback(void)
{
    HMODULE hook5_extended;
    if (hook5_present_registered) return 1;
    hook5_extended = GetModuleHandleA("NC-TK17-Hook5-Extended.dll");
    if (!hook5_extended) return 0;
    copy_proc_address(
        &hook5_get_fov_multiplier,
        GetProcAddress(hook5_extended,
            "nc_hook5_extended_get_fov_multiplier"),
        sizeof(hook5_get_fov_multiplier));
    copy_proc_address(
        &hook5_get_projection_matrix,
        GetProcAddress(hook5_extended,
            "nc_hook5_extended_get_projection_matrix"),
        sizeof(hook5_get_projection_matrix));
    copy_proc_address(
        &hook5_register_present_callback,
        GetProcAddress(hook5_extended,
            "nc_hook5_extended_register_present_callback"),
        sizeof(hook5_register_present_callback));
    if (!hook5_register_present_callback) return 0;
    if (!hook5_register_present_callback(
            (void*)liquid_hook5_present_callback)) return 0;
    InterlockedExchange(&hook5_present_registered, 1);
    copy_proc_address(
        &hook5_register_d3d8_present_callback,
        GetProcAddress(hook5_extended,
            "nc_hook5_extended_register_d3d8_present_callback"),
        sizeof(hook5_register_d3d8_present_callback));
    if (hook5_register_d3d8_present_callback &&
        hook5_register_d3d8_present_callback(
            (void*)liquid_hook5_d3d8_present_capture_callback)) {
        InterlockedExchange(&hook5_d3d8_present_registered, 1);
        log_line("liquid Hook5 D3D8 projection capture callback registered module=%p",
                 hook5_extended);
    }
    /* Exclusive fullscreen does not reliably traverse D3D8 Present.  The
       Hook5 D3D8 scene callback runs from EndScene in both display modes, so
       use it as the authoritative per-frame camera/projection capture while
       retaining Present above as a compatibility fallback. */
    copy_proc_address(
        &hook5_register_d3d8_scene_callback,
        GetProcAddress(hook5_extended,
            "nc_hook5_extended_register_d3d8_scene_callback"),
        sizeof(hook5_register_d3d8_scene_callback));
    if (hook5_register_d3d8_scene_callback &&
        hook5_register_d3d8_scene_callback(
            (void*)liquid_hook5_d3d8_present_capture_callback)) {
        InterlockedExchange(&hook5_d3d8_scene_registered, 1);
        log_line("liquid Hook5 D3D8 EndScene projection capture callback registered module=%p",
                 hook5_extended);
    }
    /* Submit directly into Hook5's full-size D3D11 scene while its real
       depth-stencil view is still bound. This is safer than injecting custom
       retained geometry into Hook5's private deferred D3D8 packet queue and
       keeps the later TK17 interface composition above the liquid. */
    copy_proc_address(
        &hook5_register_scene_callback,
        GetProcAddress(hook5_extended,
            "nc_hook5_extended_register_scene_callback"),
        sizeof(hook5_register_scene_callback));
    if (hook5_register_scene_callback &&
        hook5_register_scene_callback(
            (void*)liquid_hook5_scene_callback)) {
        InterlockedExchange(&hook5_d3d11_scene_registered, 1);
        log_line("liquid Hook5 D3D11 scene callback registered module=%p",
                 hook5_extended);
    }
    /* Draw liquid colour only after Hook5 has completed deferred lighting and
       material composition. The early scene callback above still performs
       simulation and collision against the real scene depth. */
    copy_proc_address(
        &hook5_register_composite_callback,
        GetProcAddress(hook5_extended,
            "nc_hook5_extended_register_composite_callback"),
        sizeof(hook5_register_composite_callback));
    if (hook5_register_composite_callback &&
        hook5_register_composite_callback(
            (void*)liquid_hook5_composite_callback)) {
        InterlockedExchange(&hook5_d3d11_composite_registered, 1);
        log_line("liquid Hook5 post-lighting composite callback registered module=%p",
                 hook5_extended);
    } else {
        InterlockedExchange(&hook5_d3d11_composite_registered, 0);
        log_line("liquid Hook5 post-lighting composite callback unavailable; using early scene fallback");
    }
    InterlockedExchange(&graphics_hook_ready, 1);
    log_line("liquid Hook5 final D3D11 Present callback registered module=%p",
             hook5_extended);
    return 1;
}

static int liquid_patch_vtable_slot(void *object, int index, void *hook,
                                    void **previous)
{
    void **vtable;
    DWORD old;
    if (!object || !hook) return 0;
    vtable = *(void***)object;
    if (!vtable || !ptr_readable(vtable, sizeof(void*) * (index + 1)))
        return 0;
    if (vtable[index] == hook) return 1;
    if (previous && !*previous) *previous = vtable[index];
    if (!VirtualProtect(&vtable[index], sizeof(void*),
                        PAGE_EXECUTE_READWRITE, &old)) return 0;
    vtable[index] = hook;
    VirtualProtect(&vtable[index], sizeof(void*), old, &old);
    FlushInstructionCache(GetCurrentProcess(), &vtable[index], sizeof(void*));
    return 1;
}

static HRESULT WINAPI hook_liquid_EndScene(IDirect3DDevice8 *device)
{
    /* Hook5's D3D11 callback is the authoritative depth-tested path. Avoid
       simulating and building the same geometry in its compatibility D3D8
       device as well. Native D3D8 remains unchanged. */
    if (InterlockedCompareExchange(
            &hook5_d3d11_scene_registered, 0, 0))
        return real_d3d8_EndScene ?
            real_d3d8_EndScene(device) : D3DERR_INVALIDCALL;
    return real_d3d8_EndScene ? real_d3d8_EndScene(device) : D3DERR_INVALIDCALL;
}

static HRESULT WINAPI hook_liquid_Present(IDirect3DDevice8 *device,
                                          const RECT *source,
                                          const RECT *destination,
                                          HWND override_window,
                                          const RGNDATA *dirty)
{
    liquid_native_d3d8_frame(device);
    return real_d3d8_Present ?
        real_d3d8_Present(device, source, destination, override_window, dirty) :
        D3DERR_INVALIDCALL;
}

static void liquid_patch_device(IDirect3DDevice8 *device)
{
    HMODULE physx;
    if (!device) return;
    physx = GetModuleHandleA("NC-TK17-PhysX.dll");
    if (physx) {
        int (__cdecl *register_device)(IDirect3DDevice8*) = NULL;
        copy_proc_address(&register_device, GetProcAddress(physx,
            "NCTK17PhysX_RegisterD3D8DeviceV1"), sizeof(register_device));
        if (register_device)
            log_line("PhysX native device registration device=%p accepted=%d", device, register_device(device));
    }
    liquid_patch_vtable_slot(device, 15, hook_liquid_Present,
                             (void**)&real_d3d8_Present);
    liquid_patch_vtable_slot(device, 35, hook_liquid_EndScene,
                             (void**)&real_d3d8_EndScene);
    if (liquid_native_allowed()) liquid_native_patch_d3d8(device);
    InterlockedExchange(&graphics_hook_ready, 1);
}

static HRESULT WINAPI hook_liquid_CreateDevice(
    IDirect3D8 *d3d, UINT adapter, D3DDEVTYPE device_type, HWND focus_window,
    DWORD behavior_flags, D3DPRESENT_PARAMETERS *parameters,
    IDirect3DDevice8 **returned_device)
{
    HRESULT result = real_d3d8_CreateDevice ?
        real_d3d8_CreateDevice(d3d, adapter, device_type, focus_window,
                               behavior_flags, parameters, returned_device) :
        D3DERR_INVALIDCALL;
    if (SUCCEEDED(result) && returned_device && *returned_device) {
        liquid_patch_device(*returned_device);
        log_line("liquid graphics device hooked device=%p", *returned_device);
    }
    return result;
}

static IDirect3D8 *liquid_call_d3d8_factory(direct3d_create8_t factory, UINT sdk_version)
{
    IDirect3D8 *d3d = factory ? factory(sdk_version) : NULL;
    if (d3d) {
        liquid_patch_vtable_slot(d3d, 15, hook_liquid_CreateDevice,
                                 (void**)&real_d3d8_CreateDevice);
    }
    return d3d;
}

static IDirect3D8 *WINAPI hook_liquid_Direct3DCreate8(UINT sdk_version)
{
    direct3d_create8_t previous = (direct3d_create8_t)liquid_import_previous_for_caller(
        __builtin_return_address(0), hook_liquid_Direct3DCreate8, (void*)real_Direct3DCreate8);
    return liquid_call_d3d8_factory(previous, sdk_version);
}

/* GetProcAddress returns a callable pointer which may be cached anywhere.
   Bind that pointer to the exact previous factory, rather than choosing the
   first factory resolved by a different engine module/plugin. */
static direct3d_create8_t liquid_d3d8_factories[8];
#define LIQUID_FACTORY_SLOT(n) \
static IDirect3D8 *WINAPI liquid_d3d8_factory_##n(UINT sdk) \
{ return liquid_call_d3d8_factory(liquid_d3d8_factories[n], sdk); }
LIQUID_FACTORY_SLOT(0) LIQUID_FACTORY_SLOT(1) LIQUID_FACTORY_SLOT(2) LIQUID_FACTORY_SLOT(3)
LIQUID_FACTORY_SLOT(4) LIQUID_FACTORY_SLOT(5) LIQUID_FACTORY_SLOT(6) LIQUID_FACTORY_SLOT(7)
#undef LIQUID_FACTORY_SLOT
static FARPROC liquid_wrap_d3d8_factory(FARPROC factory)
{
    static const direct3d_create8_t wrappers[8] = {
        liquid_d3d8_factory_0, liquid_d3d8_factory_1, liquid_d3d8_factory_2, liquid_d3d8_factory_3,
        liquid_d3d8_factory_4, liquid_d3d8_factory_5, liquid_d3d8_factory_6, liquid_d3d8_factory_7 };
    int i;
    if (!factory || factory == (FARPROC)hook_liquid_Direct3DCreate8) return factory;
    for (i = 0; i < 8; i++) if (factory == (FARPROC)wrappers[i]) return factory;
    for (i = 0; i < 8; i++) {
        if ((FARPROC)liquid_d3d8_factories[i] == factory) return (FARPROC)wrappers[i];
        if (!liquid_d3d8_factories[i]) {
            copy_proc_address(&liquid_d3d8_factories[i], factory, sizeof(liquid_d3d8_factories[i]));
            return (FARPROC)wrappers[i];
        }
    }
    return factory; /* Preserve the existing chain if all slots are occupied. */
}

static void liquid_patch_iat(HMODULE module, const char *dll,
                             const char *name, void *hook, void **previous)
{
    BYTE *base = (BYTE*)module;
    IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER*)base;
    IMAGE_NT_HEADERS *nt;
    IMAGE_IMPORT_DESCRIPTOR *descriptor;
    if (!base || !dos || dos->e_magic != IMAGE_DOS_SIGNATURE) return;
    nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE ||
        !nt->OptionalHeader.DataDirectory[
            IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress) return;
    descriptor = (IMAGE_IMPORT_DESCRIPTOR*)(base +
        nt->OptionalHeader.DataDirectory[
            IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    for (; descriptor->Name; descriptor++) {
        const char *dll_name = (const char*)(base + descriptor->Name);
        IMAGE_THUNK_DATA *original;
        IMAGE_THUNK_DATA *thunk;
        if (_stricmp(dll_name, dll) != 0) continue;
        original = descriptor->OriginalFirstThunk ?
            (IMAGE_THUNK_DATA*)(base + descriptor->OriginalFirstThunk) : NULL;
        thunk = (IMAGE_THUNK_DATA*)(base + descriptor->FirstThunk);
        if (!original) original = thunk;
        for (; original->u1.AddressOfData; original++, thunk++) {
            IMAGE_IMPORT_BY_NAME *import_name;
            DWORD old;
            if (original->u1.Ordinal & IMAGE_ORDINAL_FLAG) continue;
            import_name = (IMAGE_IMPORT_BY_NAME*)(base +
                                                  original->u1.AddressOfData);
            if (strcmp((const char*)import_name->Name, name) != 0) continue;
            if ((void*)thunk->u1.Function == hook) return;
            if (hook == (void*)hook_liquid_GetProcAddress || hook == (void*)hook_liquid_SwapBuffers ||
                hook == (void*)hook_liquid_Direct3DCreate8) {
                LONG chain_count = InterlockedCompareExchange(&liquid_import_chain_count, 0, 0);
                void *saved_previous = liquid_import_previous_for_module(module, hook, NULL);
                if (saved_previous) {
                    /* PhysX repeats its import installation in on_create.
                       If the slot is our exact predecessor again, we were
                       removed, not wrapped. Restore our existing link without
                       changing the predecessor or adding another chain entry.
                       Leave unknown later wrappers alone to avoid a cycle. */
                    if ((void*)thunk->u1.Function == saved_previous &&
                        VirtualProtect(&thunk->u1.Function, sizeof(void*), PAGE_READWRITE, &old)) {
                        InterlockedCompareExchange((volatile LONG*)&thunk->u1.Function,
                            (LONG)(LONG_PTR)hook, (LONG)(LONG_PTR)saved_previous);
                        VirtualProtect(&thunk->u1.Function, sizeof(void*), old, &old);
                        log_line("liquid restored graphics import module=%p function=%s", module, name);
                    }
                    return;
                }
                if (chain_count >= (LONG)(sizeof(liquid_import_chains) / sizeof(liquid_import_chains[0]))) return;
                if (!VirtualProtect(&thunk->u1.Function, sizeof(void*), PAGE_READWRITE, &old)) return;
                liquid_import_chains[chain_count].module = module;
                liquid_import_chains[chain_count].hook = hook;
                liquid_import_chains[chain_count].previous = (void*)thunk->u1.Function;
                if (previous && !*previous) *previous = (void*)thunk->u1.Function;
                MemoryBarrier();
                InterlockedExchange(&liquid_import_chain_count, chain_count + 1);
                thunk->u1.Function = (DWORD_PTR)hook;
                VirtualProtect(&thunk->u1.Function, sizeof(void*), old, &old);
                return;
            }
            if (previous && !*previous) *previous = (void*)thunk->u1.Function;
            if (VirtualProtect(&thunk->u1.Function, sizeof(void*),
                               PAGE_READWRITE, &old)) {
                thunk->u1.Function = (DWORD_PTR)hook;
                VirtualProtect(&thunk->u1.Function, sizeof(void*), old, &old);
            }
            return;
        }
    }
}

static void liquid_install_graphics_hooks(void)
{
    HANDLE snapshot;
    MODULEENTRY32 module;
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,
                                        GetCurrentProcessId());
    if (snapshot == INVALID_HANDLE_VALUE) return;
    memset(&module, 0, sizeof(module));
    module.dwSize = sizeof(module);
    if (Module32First(snapshot, &module)) {
        do {
            int is_sys = contains_i(module.szExePath,
                                      "ThriXXX010278-SYS.dll");
            int is_dx8 = contains_i(module.szExePath,
                                      "ThriXXX010278-DX8.dll");
            int is_dx3 = contains_i(module.szExePath,
                                      "ThriXXX010278-DX3.dll");
            if (is_sys || is_dx8 || is_dx3 || contains_i(module.szModule, "ThriXXX")) {
                /* Never intercept another plugin's own imports: it needs
                   those originals to finish its renderer callback chain. */
                liquid_patch_iat(module.hModule, "D3D8.dll", "Direct3DCreate8",
                                 hook_liquid_Direct3DCreate8,
                                 (void**)&real_Direct3DCreate8);
                liquid_patch_iat(module.hModule, "KERNEL32.dll",
                                 "GetProcAddress",
                                 hook_liquid_GetProcAddress,
                                 (void**)&real_GetProcAddress);
            }
            if (is_sys || is_dx8 || is_dx3 || contains_i(module.szModule, "ThriXXX")) {
                liquid_patch_iat(module.hModule, "GDI32.dll", "SwapBuffers",
                                 hook_liquid_SwapBuffers,
                                 (void**)&real_SwapBuffers);
                liquid_patch_iat(module.hModule, "OPENGL32.dll", "glDrawElements",
                    liquid_native_glDrawElements, (void**)&liquid_native_real_glDrawElements);
                liquid_patch_iat(module.hModule, "OPENGL32.dll", "glDrawArrays",
                    liquid_native_glDrawArrays, (void**)&liquid_native_real_glDrawArrays);
                liquid_patch_iat(module.hModule, "OPENGL32.dll", "glBegin",
                    liquid_native_glBegin, (void**)&liquid_native_real_glBegin);
            }
        } while (Module32Next(snapshot, &module));
    }
    CloseHandle(snapshot);
    liquid_try_register_hook5_present_callback();
}

static void THISCALL hook_AppTracker_SetWorldMatrixInverse(
    void *self, const float *matrix)
{
    if (matrix && ptr_readable(matrix, sizeof(captured_camera_inverse))) {
        memcpy(captured_camera_inverse, matrix,
               sizeof(captured_camera_inverse));
        InterlockedExchange(&captured_camera_inverse_valid, 1);
    }
    if (tramp_AppTracker_SetWorldMatrixInverse)
        tramp_AppTracker_SetWorldMatrixInverse(self, matrix);
}

static int app_main_command_name(void *command_args, char *out, size_t outsz)
{
    HMODULE app;
    const void *cmd_ref;
    unsigned int hash;
    void *value;
    const char *text;
    if (out && outsz) out[0] = 0;
    if (!command_args || !out || outsz < 2 ||
        !engine_StringRefHash32 || !engine_NameHashFind) return 0;
    app = GetModuleHandleA("ThriXXX010278-APP.dll");
    if (!app || !ptr_readable((BYTE*)app +
                              APP_MAIN_COMMAND_NAME_STRINGREF_OFFSET,
                              sizeof(void*))) return 0;
    cmd_ref = *(const void**)((BYTE*)app +
                              APP_MAIN_COMMAND_NAME_STRINGREF_OFFSET);
    if (!cmd_ref) return 0;
    hash = engine_StringRefHash32(cmd_ref);
    value = engine_NameHashFind(command_args, hash, cmd_ref);
    if (!value || !ptr_readable(value, sizeof(char*))) return 0;
    text = *(const char* const*)value;
    if (!safe_cstr_a(text, 256)) return 0;
    lstrcpynA(out, text, (int)outsz);
    return out[0] != 0;
}

static void log_named_command_field(void *command_args,
                                    const char *field_name)
{
    const void *ref;
    unsigned int hash;
    void *value;
    const char *text = NULL;
    if (!command_args || !field_name || !engine_StringRefHash32 ||
        !engine_NameHashFind) return;
    ref = stringref_from_cstr_a(field_name);
    if (!ref) return;
    hash = engine_StringRefHash32(ref);
    value = engine_NameHashFind(command_args, hash, ref);
    if (!value) return;
    if (ptr_readable(value, sizeof(char*))) {
        const char *candidate = *(const char* const*)value;
        if (safe_cstr_a(candidate, 256)) text = candidate;
    }
    if (text) {
        log_line("liquid command field name=\"%s\" value_address=%p string=\"%s\"",
                 field_name, value, text);
    } else if (ptr_readable(value, sizeof(DWORD) * 4)) {
        const DWORD *d = (const DWORD*)value;
        log_line("liquid command field name=\"%s\" value_address=%p raw=%08lx,%08lx,%08lx,%08lx float0=%.6g",
                 field_name, value,
                 (unsigned long)d[0], (unsigned long)d[1],
                 (unsigned long)d[2], (unsigned long)d[3],
                 *(const float*)value);
    } else {
        log_line("liquid command field name=\"%s\" value_address=%p readable=0",
                 field_name, value);
    }
}

static void probe_known_objects(const char *phase)
{
    static const char *names[] = {
        "Person01Spermray", "/Primary01/Person01Spermray",
        "Person02Spermray", "/Primary01/Person02Spermray",
        "Person03Spermray", "/Primary01/Person03Spermray",
        "Person04Spermray", "/Primary01/Person04Spermray",
        "Tool01Spermray", "/Primary01/Tool01Spermray",
        "Person01Spermray:spermray01_init",
        "Person01Spermray:spermray01_splash",
        "Tool01Spermray:spermray01_init",
        "Tool01Spermray:spermray01_splash"
    };
    size_t i;
    if (!cfg.probe_known_objects || !engine_FindObjC) return;
    for (i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
        void *raw = engine_FindObjC(names[i]);
        void *object = NULL;
        void *nil_weak = NULL;
        void *nil_object = NULL;
        if (engine_G_NilWeakObjTarget_ptr &&
            ptr_readable(engine_G_NilWeakObjTarget_ptr, sizeof(void*)))
            nil_weak = *engine_G_NilWeakObjTarget_ptr;
        if (engine_G_NilObject_ptr &&
            ptr_readable(engine_G_NilObject_ptr, sizeof(void*)))
            nil_object = *engine_G_NilObject_ptr;
        if (raw && engine_GetWeakObjTarget) object = engine_GetWeakObjTarget(raw);
        if (raw == nil_weak || object == nil_weak || object == nil_object) {
            raw = NULL;
            object = NULL;
        }
        if (raw || object) {
            log_line("liquid object probe phase=%s query=\"%s\" raw=%p object=%p",
                     phase ? phase : "", names[i], raw,
                     object ? object : raw);
        }
    }
}

static DWORD __cdecl hook_AppMain_Command(void *command_args)
{
    char command[128];
    int interesting = 0;
    DWORD result;
    if (!hook_recursion) {
        hook_recursion = 1;
        resolve_engine_symbols();
        if (app_main_command_name(command_args, command, sizeof(command))) {
            interesting = liquid_text(command);
            if (_stricmp(command, "CumShot") == 0)
                liquid_note_tool_command(GetTickCount());
            if ((interesting || cfg.log_all_commands) && reserve_event()) {
                static const char *fields[] = {
                    "Cmd", "SubCmd", "Action", "Situation", "Person",
                    "Target", "Object", "NodeName", "tool", "flares",
                    "speed"
                };
                size_t i;
                log_line("%s command phase=before name=\"%s\" args=%p thread=0x%08lx",
                         interesting ? "LIQUID TRIGGER" : "command",
                         command, command_args,
                         (unsigned long)GetCurrentThreadId());
                if (interesting && cfg.dump_command_arguments) {
                    log_words("command_args", command_args, 64);
                    for (i = 0; i < sizeof(fields) / sizeof(fields[0]); i++)
                        log_named_command_field(command_args, fields[i]);
                }
                if (interesting) probe_known_objects("before-command");
            }
        }
        hook_recursion = 0;
    }
    result = real_AppMain_Command ? real_AppMain_Command(command_args)
                                  : 0x80000001u;
    if (interesting && reserve_event()) {
        log_line("LIQUID TRIGGER command phase=after name=\"%s\" args=%p result=0x%08lx",
                 command, command_args, (unsigned long)result);
        probe_known_objects("after-command");
    }
    return result;
}

static void THISCALL hook_Object_iNameSet(void *self, const void *member,
                                          const void *name_ref)
{
#if defined(__GNUC__)
    const void *direct_caller = __builtin_return_address(0);
#else
    const void *direct_caller = NULL;
#endif
    char copy[256];
    const char *name = stringref_cstr_a(name_ref);
    int interesting = liquid_text(name) ||
                      (name && contains_i(name, "penis_jointEnd"));
    copy[0] = 0;
    if (name) {
        remember_named_object(self, name);
        remember_liquid_transform_node(self, name);
    }
    if (interesting) lstrcpynA(copy, name, sizeof(copy));
    if (tramp_Object_iNameSet)
        tramp_Object_iNameSet(self, member, name_ref);
    if (cfg.liquids_enabled && copy[0] &&
        (contains_i(copy, "spermray01_mesh") ||
         contains_i(copy, "spermray02_mesh"))) {
        int changed;
        /* Mesh children are named before their PersonXXSpermray or
           Tool01Spermray owner. Hide provisionally, then restore the same
           freshly created nodes when the owner is identified as the tool. */
        liquid_remember_recent_native_ray_node(self);
        changed = liquid_set_script_visibility(self, 0u);
        if (cfg.enabled && reserve_event())
            log_line("liquid native ray pending owner object=%p name=\"%s\" visible=0 changed=%d",
                     self, copy, changed);
    }
    if (cfg.liquids_enabled && copy[0] &&
        (_stricmp(copy, "Tool01Spermray") == 0 ||
         _stricmp(copy, "STool01Spermray") == 0)) {
        int changed = liquid_classify_recent_native_ray_nodes(1);
        if (cfg.enabled && reserve_event())
            log_line("liquid native ray owner classified source=tool owner=\"%s\" restored_nodes=%d",
                     copy, changed);
    } else if (cfg.liquids_enabled && copy[0] &&
               (liquid_person_spermray_root_name(copy) ||
                (copy[0] == 'S' &&
                 liquid_person_spermray_root_name(copy + 1)))) {
        int changed = liquid_classify_recent_native_ray_nodes(0);
        if (cfg.enabled && reserve_event())
            log_line("liquid native ray owner classified source=model owner=\"%s\" hidden_nodes=%d",
                     copy, changed);
    }
    if (copy[0] && _stricmp(copy, "spermray01_splash") == 0)
        liquid_note_runtime_splash(self, direct_caller);
    if (!hook_recursion && cfg.log_object_names && copy[0] && reserve_event()) {
        hook_recursion = 1;
        log_line("LIQUID OBJECT name=\"%s\" object=%p member=%p ref=%p thread=0x%08lx",
                 copy, self, member, name_ref,
                 (unsigned long)GetCurrentThreadId());
        capture_deep_event(copy, self, direct_caller);
        hook_recursion = 0;
    }
}

static int native_stain_trace_active(void)
{
    DWORD now = GetTickCount();
    DWORD until = (DWORD)InterlockedCompareExchange(
        &liquid_native_stain_trace_until_tick, 0, 0);
    return until != 0 && (LONG)(until - now) >= 0;
}

static void copy_known_object_name(const void *object, char *copy,
                                   size_t copy_size)
{
    const char *name;
    if (!copy || copy_size == 0) return;
    copy[0] = 0;
    name = known_object_name(object);
    if (name) lstrcpynA(copy, name, (int)copy_size);
}

/* TK17 builds its persistent sperm shader object immediately after a
   successful AppPick hit.  Observe the engine's clone call only during that
   short placement window so ordinary room/model cloning does not flood the
   diagnostic log. */
static void *__cdecl hook_CloneObject(const void *source)
{
#if defined(__GNUC__)
    const void *caller = __builtin_return_address(0);
#else
    const void *caller = NULL;
#endif
    char source_name[96];
    char clone_name[96];
    char caller_text[320];
    int tracing = native_stain_trace_active();
    void *clone;
    copy_known_object_name(source, source_name, sizeof(source_name));
    clone = tramp_CloneObject ? tramp_CloneObject(source) : NULL;
    if (tracing) {
        LONG sequence = InterlockedIncrement(&liquid_native_object_probe_count);
        if (sequence <= 80 && reserve_event()) {
            copy_known_object_name(clone, clone_name, sizeof(clone_name));
            log_module_address(caller, caller_text, sizeof(caller_text));
            log_line("NATIVE STAIN CLONE sequence=%ld caller=%s source=%p source_name=\"%s\" clone=%p clone_name=\"%s\"",
                     sequence, caller_text, source,
                     source_name[0] ? source_name : "", clone,
                     clone_name[0] ? clone_name : "");
        }
    }
    return clone;
}

/* This executable-local cache call is the first ordinary thiscall after
   TK17 converts the PickResult into its skinned-surface placement object.
   Its two inputs and returned object identify the native shader/template
   route we need to reuse for custom contacts. */
static void *THISCALL hook_NativeStainCache(void *self, void *surface,
                                            void *stain_template)
{
#if defined(__GNUC__)
    const void *caller = __builtin_return_address(0);
#else
    const void *caller = NULL;
#endif
    char surface_name[96];
    char template_name[96];
    char result_name[96];
    char caller_text[320];
    int tracing = native_stain_trace_active();
    void *result;
    if (tracing) {
        copy_known_object_name(surface, surface_name, sizeof(surface_name));
        copy_known_object_name(stain_template, template_name,
                               sizeof(template_name));
    }
    result = tramp_NativeStainCache ?
        tramp_NativeStainCache(self, surface, stain_template) : NULL;
    if (tracing) {
        LONG sequence = InterlockedIncrement(&liquid_native_object_probe_count);
        if (sequence <= 80 && reserve_event()) {
            copy_known_object_name(result, result_name, sizeof(result_name));
            log_module_address(caller, caller_text, sizeof(caller_text));
            log_line("NATIVE STAIN CACHE sequence=%ld caller=%s self=%p surface=%p surface_name=\"%s\" template=%p template_name=\"%s\" result=%p result_name=\"%s\"",
                     sequence, caller_text, self, surface,
                     surface_name[0] ? surface_name : "", stain_template,
                     template_name[0] ? template_name : "", result,
                     result_name[0] ? result_name : "");
            if (result && ptr_readable(result, 64))
                log_words("native-stain-cache-result", result, 64);
        }
    }
    return result;
}

static int liquid_native_descriptor_in_update(
    const void *update, const void *descriptor)
{
    void **items;
    int count;
    int index;
    if (!update || !descriptor ||
        !ptr_readable((const BYTE*)update + 0x14, sizeof(items)))
        return 0;
    items = *(void***)((const BYTE*)update + 0x14);
    if (!items ||
        !ptr_readable((const BYTE*)items - sizeof(int), sizeof(int)))
        return 0;
    count = *((const int*)items - 1);
    if (count < 1 || count > 4096 ||
        !ptr_readable(items, (size_t)count * sizeof(*items)))
        return 0;
    for (index = 0; index < count; index++) {
        if (items[index] == descriptor) return 1;
    }
    return 0;
}

static int liquid_native_diag_array_count(const void *array_data,
                                          size_t item_size)
{
    int count;
    if (!array_data || item_size == 0 ||
        !ptr_readable((const BYTE*)array_data - sizeof(int), sizeof(int)))
        return -1;
    count = *((const int*)array_data - 1);
    if (count < 0 || count > 4096 ||
        (count > 0 &&
         !ptr_readable(array_data, (size_t)count * item_size)))
        return -1;
    return count;
}

static unsigned long liquid_native_diag_checksum(const void *pointer,
                                                  size_t bytes)
{
    const BYTE *data = (const BYTE*)pointer;
    unsigned long hash = 2166136261u;
    size_t index;
    if (!pointer || bytes == 0 || !ptr_readable(pointer, bytes)) return 0;
    for (index = 0; index < bytes; index++) {
        hash ^= data[index];
        hash *= 16777619u;
    }
    return hash;
}

/* Entries in the native group's +0x10 array are TK17 reference wrappers,
   not BlendControl objects themselves.  NativeStainUpdate first rejects a
   wrapper whose low flag byte contains 0x08, then dereferences its first
   DWORD and calls BlendControl.Weight on that object.  Mirror that exact
   read-only path for diagnostics. */
static void *liquid_native_diag_control_object(void *wrapper)
{
    void *object;
    if (!wrapper || !ptr_readable(wrapper, sizeof(void*))) return NULL;
    if ((*(const BYTE*)wrapper & 0x08) != 0) return NULL;
    object = *(void**)wrapper;
    if (!object || !ptr_readable(object, 96)) return NULL;
    return object;
}

static void *liquid_native_stain_group(void *update, void *descriptor)
{
    void **groups;
    int group_count;
    int group_index;
    if (!update || !descriptor ||
        !ptr_readable((const BYTE*)descriptor + 0x28, sizeof(group_index)) ||
        !ptr_readable((const BYTE*)update + 0x18, sizeof(groups)))
        return NULL;
    group_index = *(const int*)((const BYTE*)descriptor + 0x28);
    groups = *(void***)((const BYTE*)update + 0x18);
    group_count = liquid_native_diag_array_count(groups, sizeof(*groups));
    if (group_index < 0 || group_index >= group_count) return NULL;
    if (!groups[group_index] || !ptr_readable(groups[group_index], 0x1c))
        return NULL;
    return groups[group_index];
}

static int liquid_native_stain_control_count(void *group)
{
    void **controls;
    if (!group || !ptr_readable((const BYTE*)group + 0x10,
                                sizeof(controls)))
        return -1;
    controls = *(void***)((const BYTE*)group + 0x10);
    return liquid_native_diag_array_count(controls, sizeof(*controls));
}

/* Match NativeStainUpdate's Weight dispatch (EXE+0x1f2046). The setter at
   SYS+0xd7cb0 also invalidates dependent animation evaluators; writing +0x10
   directly leaves their cached output unchanged. Do not pre-write the value:
   the setter only notifies consumers when the weight actually changes. */
static int liquid_native_set_stain_weight(void *object)
{
    typedef void (THISCALL *weight_set_t)(void *, unsigned int, float);
    weight_set_t setter = (weight_set_t)liquid_node_method(
        object, 0x238, 0x44);
    if (!setter || !ptr_writable((BYTE*)object + 0x10, sizeof(float)))
        return 0;
    setter(object, 0x01fff08eu, 1.0f);
    return 1;
}

static void liquid_native_track_frozen_control(
    void *update, void *group, void *wrapper)
{
    liquid_native_frozen_control_t *slot;
    void *object = liquid_native_diag_control_object(wrapper);
    unsigned int slot_index;
    unsigned int index;
    if (!update || !group || !wrapper || !object ||
        !liquid_native_set_stain_weight(object))
        return;

    for (index = 0; index < LIQUID_NATIVE_FROZEN_CONTROL_SLOTS; index++) {
        if (liquid_native_frozen_controls[index].update == update &&
            liquid_native_frozen_controls[index].group == group &&
            liquid_native_frozen_controls[index].wrapper == wrapper &&
            liquid_native_frozen_controls[index].object == object)
            return;
    }

    slot_index = liquid_native_frozen_control_cursor++ %
                 LIQUID_NATIVE_FROZEN_CONTROL_SLOTS;
    slot = &liquid_native_frozen_controls[slot_index];
    slot->update = update;
    slot->wrapper = wrapper;
    slot->object = object;
    slot->group = group;
    slot->added_tick = GetTickCount();
    if (liquid_native_frozen_control_limit < slot_index + 1)
        liquid_native_frozen_control_limit = slot_index + 1;
    {
        static unsigned int logged;
        if (logged++ < 24)
            log_line("liquid native stain frozen update=%p group=%p wrapper=%p control=%p weight=1 mode=property-setter",
                     update, group, wrapper, object);
    }
}

static void liquid_native_capture_new_frozen_controls(
    void *update, void *group, int first_control)
{
    void **controls;
    int control_count;
    int index;
    if (!group || first_control < 0 ||
        !ptr_readable((const BYTE*)group + 0x10, sizeof(controls)))
        return;
    controls = *(void***)((const BYTE*)group + 0x10);
    control_count = liquid_native_diag_array_count(controls,
                                                    sizeof(*controls));
    if (first_control >= control_count) return;
    for (index = first_control; index < control_count; index++)
        liquid_native_track_frozen_control(update, group, controls[index]);
}

/* The native loop can remove expired controls before picking, and can create
   stains for several descriptors in one update. Snapshot at the successful
   custom PickRay, then finish before the next native pick (or on update return).
   This also covers the very first pick that discovers an emitter descriptor. */
static void liquid_native_finish_frozen_capture(void)
{
    liquid_native_freeze_capture_t *capture = liquid_native_freeze_capture;
    if (!capture || !capture->group) return;
    if (!cfg.collision_native_decal_drip &&
        liquid_native_stain_group(capture->update, capture->descriptor) ==
            capture->group)
        liquid_native_capture_new_frozen_controls(
            capture->update, capture->group, capture->first_control);
    capture->group = NULL;
}

static void liquid_native_begin_frozen_capture(void *descriptor)
{
    liquid_native_freeze_capture_t *capture = liquid_native_freeze_capture;
    if (!capture || cfg.collision_native_decal_drip ||
        !liquid_native_descriptor_in_update(capture->update, descriptor))
        return;
    capture->descriptor = descriptor;
    capture->group = liquid_native_stain_group(capture->update, descriptor);
    capture->first_control = liquid_native_stain_control_count(capture->group);
}

/* Called only after submitting a custom liquid ray. Keep native decal
   creation and exact particle-to-body confirmation as separate outcomes. */
static int liquid_native_track_custom_stain_pick(
    void *descriptor, const liquid_native_contact_t *contact,
    int native_result, void *results)
{
    void *data = NULL;
    int count = 0;
    int body_confirmed;
    float hit_view[3];
    if (native_result < 0 || !contact ||
        !liquid_native_pick_result_data(results, &data, &count) ||
        !ptr_readable(data, 0x20)) return 0;
    memcpy(hit_view, (const BYTE*)data + 0x14, sizeof(hit_view));
    body_confirmed = liquid_confirm_particle_model_contact(contact, hit_view);
    /* EXE+0x1f24ae skips creation only when the native result is negative.
       A nonnegative pick can still fail the stricter particle attachment
       check (surface gap, room ownership, or particle lifetime). TK17 will
       create its decal anyway, so that new control must still be frozen.
       Never promote the particle to a body contact to make freezing work. */
    liquid_native_begin_frozen_capture(descriptor);
    InterlockedIncrement(&liquid_native_animation_diag_generation);
    InterlockedExchange(&liquid_native_animation_diag_emission,
                        (LONG)contact->emission_id);
    InterlockedExchange(&liquid_native_animation_diag_particle,
                        (LONG)contact->particle_id);
    InterlockedExchange(&liquid_native_animation_diag_last_tick, 0);
    InterlockedExchange(&liquid_native_animation_diag_until_tick,
                        (LONG)(GetTickCount() + 2500u));
    return body_confirmed;
}

static int liquid_native_frozen_control_is_live(
    void *update, const liquid_native_frozen_control_t *slot)
{
    void **groups, **controls;
    int count, index;
    if (!ptr_readable((BYTE*)update + 0x18, sizeof(groups))) return 0;
    groups = *(void***)((BYTE*)update + 0x18);
    count = liquid_native_diag_array_count(groups, sizeof(*groups));
    for (index = 0; index < count; index++)
        if (groups[index] == slot->group) break;
    if (index >= count ||
        !ptr_readable((BYTE*)slot->group + 0x10, sizeof(controls))) return 0;
    controls = *(void***)((BYTE*)slot->group + 0x10);
    count = liquid_native_diag_array_count(controls, sizeof(*controls));
    for (index = 0; index < count; index++)
        if (controls[index] == slot->wrapper)
            return liquid_native_diag_control_object(slot->wrapper) ==
                slot->object;
    return 0;
}

static void liquid_native_hold_custom_stain_weights(void *update)
{
    unsigned int index;
    if (cfg.collision_native_decal_drip) return;
    for (index = 0; index < liquid_native_frozen_control_limit; index++) {
        liquid_native_frozen_control_t *slot =
            &liquid_native_frozen_controls[index];
        if (slot->update != update || !slot->wrapper || !slot->object) continue;
        if (!liquid_native_frozen_control_is_live(update, slot) ||
            !liquid_native_set_stain_weight(slot->object)) {
            memset(slot, 0, sizeof(*slot));
            continue;
        }
    }
    while (liquid_native_frozen_control_limit > 0) {
        liquid_native_frozen_control_t *tail =
            &liquid_native_frozen_controls[
                liquid_native_frozen_control_limit - 1];
        if (tail->wrapper && tail->object) break;
        liquid_native_frozen_control_limit--;
    }
}

/* Log exact DWORD mutations on the newest native stain objects.  Waiting
   until contacts have been quiet for a short period avoids repeatedly
   resetting the baseline while one liquid pulse is still adding stains.
   This remains strictly read-only: it snapshots bytes owned by TK17 and
   never writes them back. */
static void liquid_native_animation_diag_mutations(
    LONG generation, void *node, void *control)
{
    char line[4096];
    size_t used;
    size_t index;
    int node_changes = 0;
    int control_changes = 0;
    int logged_changes = 0;

    if (!node || !control ||
        !ptr_readable(node, sizeof(liquid_native_animation_diag_node_previous)) ||
        !ptr_readable(control,
                      sizeof(liquid_native_animation_diag_control_previous)))
        return;

    if (generation != liquid_native_animation_diag_snapshot_generation ||
        node != liquid_native_animation_diag_snapshot_node ||
        control != liquid_native_animation_diag_snapshot_control) {
        liquid_native_animation_diag_snapshot_generation = generation;
        liquid_native_animation_diag_snapshot_node = node;
        liquid_native_animation_diag_snapshot_control = control;
        memcpy(liquid_native_animation_diag_node_previous, node,
               sizeof(liquid_native_animation_diag_node_previous));
        memcpy(liquid_native_animation_diag_control_previous, control,
               sizeof(liquid_native_animation_diag_control_previous));
        log_line("NATIVE STAIN FIELD DIAG generation=%ld outcome=baseline node=%p control=%p",
                 generation, node, control);
        log_words("native-stain-animation-node-baseline", node,
                  sizeof(liquid_native_animation_diag_node_previous));
        log_words("native-stain-animation-control-baseline", control,
                  sizeof(liquid_native_animation_diag_control_previous));
        log_deep_pointer_links("native-stain-animation-node", node,
                               sizeof(liquid_native_animation_diag_node_previous));
        log_deep_pointer_links(
            "native-stain-animation-control", control,
            sizeof(liquid_native_animation_diag_control_previous));
        return;
    }

    used = (size_t)_snprintf(
        line, sizeof(line),
        "NATIVE STAIN FIELD DIAG generation=%ld node=%p control=%p changes=",
        generation, node, control);
    if (used >= sizeof(line)) return;

    for (index = 0;
         index + sizeof(DWORD) <=
             sizeof(liquid_native_animation_diag_node_previous);
         index += sizeof(DWORD)) {
        DWORD before;
        DWORD after;
        float before_float;
        float after_float;
        int written;
        memcpy(&before, liquid_native_animation_diag_node_previous + index,
               sizeof(before));
        memcpy(&after, (const BYTE*)node + index, sizeof(after));
        if (before == after) continue;
        node_changes++;
        if (logged_changes >= 48 || used + 90 >= sizeof(line)) continue;
        memcpy(&before_float, &before, sizeof(before_float));
        memcpy(&after_float, &after, sizeof(after_float));
        written = _snprintf(
            line + used, sizeof(line) - used,
            "%sN+0x%02lx:%08lx>%08lx(%.7g>%.7g)",
            logged_changes ? "," : "", (unsigned long)index,
            (unsigned long)before, (unsigned long)after,
            _finite(before_float) ? before_float : 0.0f,
            _finite(after_float) ? after_float : 0.0f);
        if (written <= 0) break;
        used += (size_t)written;
        logged_changes++;
    }

    for (index = 0;
         index + sizeof(DWORD) <=
             sizeof(liquid_native_animation_diag_control_previous);
         index += sizeof(DWORD)) {
        DWORD before;
        DWORD after;
        float before_float;
        float after_float;
        int written;
        memcpy(&before,
               liquid_native_animation_diag_control_previous + index,
               sizeof(before));
        memcpy(&after, (const BYTE*)control + index, sizeof(after));
        if (before == after) continue;
        control_changes++;
        if (logged_changes >= 48 || used + 90 >= sizeof(line)) continue;
        memcpy(&before_float, &before, sizeof(before_float));
        memcpy(&after_float, &after, sizeof(after_float));
        written = _snprintf(
            line + used, sizeof(line) - used,
            "%sC+0x%02lx:%08lx>%08lx(%.7g>%.7g)",
            logged_changes ? "," : "", (unsigned long)index,
            (unsigned long)before, (unsigned long)after,
            _finite(before_float) ? before_float : 0.0f,
            _finite(after_float) ? after_float : 0.0f);
        if (written <= 0) break;
        used += (size_t)written;
        logged_changes++;
    }

    memcpy(liquid_native_animation_diag_node_previous, node,
           sizeof(liquid_native_animation_diag_node_previous));
    memcpy(liquid_native_animation_diag_control_previous, control,
           sizeof(liquid_native_animation_diag_control_previous));
    if (node_changes || control_changes) {
        int written = _snprintf(
            line + used, sizeof(line) - used,
            " totals=(node:%d,control:%d)", node_changes, control_changes);
        if (written > 0) used += (size_t)written;
        line[sizeof(line) - 1] = 0;
        log_line("%s", line);
    }
}

/* Observe the native stain produced by a confirmed custom-liquid contact.
   This is deliberately read-only.  The native update owns all arrays and
   objects; diagnostics only sample their identity, weights, attachment
   vectors, and short memory checksums after the original update completes. */
static void liquid_native_animation_diag_sample(void *update)
{
    DWORD now = GetTickCount();
    DWORD until = (DWORD)InterlockedCompareExchange(
        &liquid_native_animation_diag_until_tick, 0, 0);
    DWORD last = (DWORD)InterlockedCompareExchange(
        &liquid_native_animation_diag_last_tick, 0, 0);
    void *descriptor = (void*)InterlockedCompareExchangePointer(
        (PVOID volatile *)&liquid_native_stain_descriptor, NULL, NULL);
    void **groups;
    void *group;
    void **nodes;
    void **controls;
    float *weights;
    float *vectors;
    int group_index;
    int group_count;
    int node_count;
    int control_count;
    int weight_count;
    int vector_count;
    int index;
    char node_details[1400];
    char control_details[1100];
    char weight_details[700];
    char vector_details[900];
    size_t used;
    unsigned long sample_ms;

    if (!update || until == 0 || (LONG)(until - now) < 0 ||
        (last != 0 && now - last < 100))
        return;
    InterlockedExchange(&liquid_native_animation_diag_last_tick, (LONG)now);
    if (!descriptor || !ptr_readable(descriptor, 0x40) ||
        !ptr_readable((const BYTE*)update + 0x18, sizeof(groups)))
        return;

    group_index = *(const int*)((const BYTE*)descriptor + 0x28);
    groups = *(void***)((const BYTE*)update + 0x18);
    group_count = liquid_native_diag_array_count(groups, sizeof(*groups));
    if (group_index < 0 || group_index >= group_count) {
        log_line("NATIVE STAIN ANIMATION DIAG generation=%ld emission=%ld particle=%ld sample_ms=%lu update=%p descriptor=%p group_index=%d group_count=%d outcome=invalid-group",
                 InterlockedCompareExchange(
                     &liquid_native_animation_diag_generation, 0, 0),
                 InterlockedCompareExchange(
                     &liquid_native_animation_diag_emission, 0, 0),
                 InterlockedCompareExchange(
                     &liquid_native_animation_diag_particle, 0, 0),
                 (unsigned long)(2500u - (until - now)), update,
                 descriptor, group_index, group_count);
        return;
    }

    group = groups[group_index];
    if (!group || !ptr_readable(group, 0x1c)) return;
    nodes = *(void***)((const BYTE*)group + 0x0c);
    controls = *(void***)((const BYTE*)group + 0x10);
    weights = *(float**)((const BYTE*)group + 0x14);
    vectors = *(float**)((const BYTE*)group + 0x18);
    node_count = liquid_native_diag_array_count(nodes, sizeof(*nodes));
    control_count = liquid_native_diag_array_count(
        controls, sizeof(*controls));
    weight_count = liquid_native_diag_array_count(weights, sizeof(*weights));
    vector_count = liquid_native_diag_array_count(
        vectors, sizeof(float) * 3);
    sample_ms = (unsigned long)(2500u - (until - now));

    node_details[0] = 0;
    used = 0;
    if (node_count > 0) {
        int first = node_count > 6 ? node_count - 6 : 0;
        for (index = first; index < node_count && used + 180 < sizeof(node_details);
             index++) {
            char name[96];
            int written;
            copy_known_object_name(nodes[index], name, sizeof(name));
            written = _snprintf(
                node_details + used, sizeof(node_details) - used,
                "%s%d:%p:%08lx:\"%s\"",
                used ? "," : "", index, nodes[index],
                liquid_native_diag_checksum(nodes[index], 96),
                name[0] ? name : "");
            if (written < 0) break;
            used += (size_t)written;
        }
    }

    control_details[0] = 0;
    used = 0;
    if (control_count > 0) {
        int first = control_count > 6 ? control_count - 6 : 0;
        for (index = first; index < control_count &&
             used + 170 < sizeof(control_details); index++) {
            char name[96];
            int written;
            copy_known_object_name(controls[index], name, sizeof(name));
            written = _snprintf(
                control_details + used, sizeof(control_details) - used,
                "%s%d:%p:%08lx:\"%s\"",
                used ? "," : "", index, controls[index],
                liquid_native_diag_checksum(controls[index], 64),
                name[0] ? name : "");
            if (written < 0) break;
            used += (size_t)written;
        }
    }

    weight_details[0] = 0;
    used = 0;
    if (weight_count > 0) {
        int first = weight_count > 10 ? weight_count - 10 : 0;
        for (index = first; index < weight_count &&
             used + 50 < sizeof(weight_details); index++) {
            int written = _snprintf(
                weight_details + used, sizeof(weight_details) - used,
                "%s%d:%.7g", used ? "," : "", index, weights[index]);
            if (written < 0) break;
            used += (size_t)written;
        }
    }

    vector_details[0] = 0;
    used = 0;
    if (vector_count > 0) {
        int first = vector_count > 6 ? vector_count - 6 : 0;
        for (index = first; index < vector_count &&
             used + 100 < sizeof(vector_details); index++) {
            int written = _snprintf(
                vector_details + used, sizeof(vector_details) - used,
                "%s%d:(%.7g,%.7g,%.7g)", used ? "," : "", index,
                vectors[index * 3], vectors[index * 3 + 1],
                vectors[index * 3 + 2]);
            if (written < 0) break;
            used += (size_t)written;
        }
    }

    log_line("NATIVE STAIN ANIMATION DIAG generation=%ld emission=%ld particle=%ld sample_ms=%lu update=%p descriptor=%p descriptor_checksum=%08lx group_index=%d group=%p group_checksum=%08lx counts=(nodes:%d,controls:%d,weights:%d,vectors:%d) nodes=[%s] controls=[%s] weights=[%s] vectors=[%s]",
             InterlockedCompareExchange(
                 &liquid_native_animation_diag_generation, 0, 0),
             InterlockedCompareExchange(
                 &liquid_native_animation_diag_emission, 0, 0),
             InterlockedCompareExchange(
                 &liquid_native_animation_diag_particle, 0, 0),
             sample_ms, update, descriptor,
             liquid_native_diag_checksum(descriptor, 64), group_index, group,
             liquid_native_diag_checksum(group, 0x1c), node_count,
             control_count, weight_count, vector_count, node_details,
             control_details, weight_details, vector_details);
    if (sample_ms >= 300 && node_count > 0 && control_count > 0) {
        void *control_wrapper = controls[control_count - 1];
        void *control_object =
            liquid_native_diag_control_object(control_wrapper);
        if (!control_object) {
            log_line("NATIVE STAIN FIELD DIAG generation=%ld outcome=control-unavailable wrapper=%p wrapper_flags=0x%02x",
                     InterlockedCompareExchange(
                         &liquid_native_animation_diag_generation, 0, 0),
                     control_wrapper,
                     control_wrapper && ptr_readable(control_wrapper, 1) ?
                         (unsigned int)*(const BYTE*)control_wrapper : 0u);
            return;
        }
        liquid_native_animation_diag_mutations(
            InterlockedCompareExchange(
                &liquid_native_animation_diag_generation, 0, 0),
            nodes[node_count - 1], control_object);
    }
}

/* Depth contacts arrive on Hook5's render thread after TK17's normal stain
   burst has ended.  Re-arm the still-live native splash descriptor here,
   inside TK17's own per-frame update on the game thread.  The original
   routine then performs its complete PickRay, skinned-surface attachment,
   and sperm texture selection path; our PickRay hook only substitutes the
   next confirmed contact ray. */
static liquid_emitter_t *liquid_emitter_for_native_update(
    void *update, DWORD now)
{
    static volatile LONG next_emitter_index;
    int offset;
    int start_index = (int)InterlockedCompareExchange(
        &next_emitter_index, 0, 0);
    for (offset = 0; offset < LIQUID_MODEL_EMITTER_COUNT; offset++) {
        int index = (start_index + offset) % LIQUID_MODEL_EMITTER_COUNT;
        liquid_emitter_t *emitter = &model_emitters[index];
        void *descriptor = emitter->native_stain_descriptor;
        if (!emitter->emission_id || !descriptor ||
            !liquid_native_model_contact_pending(emitter->emission_id, now))
            continue;
        if (liquid_native_descriptor_in_update(update, descriptor)) {
            InterlockedExchange(
                &next_emitter_index,
                (LONG)((index + 1) % LIQUID_MODEL_EMITTER_COUNT));
            return emitter;
        }
    }
    return NULL;
}

static void THISCALL hook_NativeStainUpdate(
    void *self, void *update_context, void *receiver, void *result_state)
{
    DWORD now = GetTickCount();
    liquid_emitter_t *model_emitter =
        liquid_emitter_for_native_update(self, now);
    unsigned int emission_id = model_emitter ?
        model_emitter->emission_id : 0;
    void *descriptor = model_emitter ?
        model_emitter->native_stain_descriptor : NULL;
    LONG descriptor_emission = model_emitter ?
        (LONG)model_emitter->emission_id : 0;
    liquid_native_freeze_capture_t capture = {0};
    liquid_native_freeze_capture_t *previous_capture =
        liquid_native_freeze_capture;
    int armed = 0;
    capture.update = self;
    liquid_native_freeze_capture = &capture;
    if (cfg.liquids_enabled && cfg.collision_spawn_model_stains &&
        emission_id && descriptor_emission == (LONG)emission_id &&
        liquid_native_model_contact_pending(emission_id, now) &&
        descriptor && liquid_native_descriptor_in_update(self, descriptor) &&
        ptr_readable((const BYTE*)descriptor + 0x20, sizeof(float)) &&
        ptr_writable((BYTE*)descriptor + 0x10, sizeof(LONG)) &&
        ptr_writable((BYTE*)descriptor + 0x18, sizeof(double))) {
        LONG state = *(volatile LONG*)((BYTE*)descriptor + 0x10);
        float rate = *(const float*)((const BYTE*)descriptor + 0x20);
        if ((state == 0 || state == 1) && _finite(rate) &&
            rate > 0.0001f && rate < 10000.0f && engine_TimerSetSeconds) {
            double trigger_seconds = 0.4 / (double)rate + 0.001;
            engine_TimerSetSeconds((BYTE*)descriptor + 0x18,
                                   trigger_seconds);
            InterlockedExchange(
                (volatile LONG*)((BYTE*)descriptor + 0x10), 1);
            armed = 1;
            if (InterlockedIncrement(&liquid_native_rearm_log_count) <= 40)
                log_line("liquid native stain descriptor rearmed emission=%u descriptor=%p state=%ld rate=%.5f trigger=%.5f",
                         emission_id, descriptor, (long)state, rate,
                         trigger_seconds);
        }
    }
    if (!armed && descriptor &&
        liquid_native_model_contact_pending(emission_id, now) &&
        InterlockedIncrement(&liquid_native_rearm_log_count) <= 40) {
        log_line("liquid native stain descriptor not-rearmed emission=%u descriptor=%p descriptor_emission=%ld member=%d timer=%p",
                 emission_id, descriptor, (long)descriptor_emission,
                 liquid_native_descriptor_in_update(self, descriptor),
                 (void*)engine_TimerSetSeconds);
    }
    if (tramp_NativeStainUpdate)
        tramp_NativeStainUpdate(self, update_context, receiver,
                                result_state);
    liquid_native_finish_frozen_capture();
    liquid_native_freeze_capture = previous_capture;
    liquid_native_hold_custom_stain_weights(self);
    if (cfg.enabled)
        liquid_native_animation_diag_sample(self);
    liquid_apply_testicular_retraction_weights();
}

/* Observe TK17's own ray-hit path without changing its result.  The native
   sperm stain system uses AppPick before it places a sperm_shader plane on a
   model.  Capturing the caller and opaque PickResult bytes lets us identify
   that placement routine without guessing object layouts or dereferencing
   unsafe skeleton objects. */
/* Use the live game-thread picker while its geometry list and result storage
   are valid. These are read-only ownership queries; the caller's actual stain
   pick is performed afterward and replaces the scratch results as usual.
   Do not throttle body ownership to the separate cosmetic decal rate. */
static void liquid_verify_recent_body_contacts(
    void *self, const void *geometry_list_ref, void *results,
    int include_hidden, unsigned int flags, unsigned int emission_id, DWORD now)
{
    static unsigned int cursor;
    unsigned int scanned;
    int budget = 16;
    if (!cfg.collision_follow_bodies || !tramp_AppPick_PickRay || !results ||
        !emission_id || cfg.particle_limit <= 0) return;
    for (scanned = 0; scanned < (unsigned int)cfg.particle_limit && budget; scanned++) {
        liquid_particle_t *p = &liquid_particles[cursor++ % (unsigned int)cfg.particle_limit];
        liquid_native_contact_t contact = {0};
        float origin[3], direction[3], hit[3];
        void *data = NULL;
        int count = 0;
        if (!p->active || !p->collided || p->emission_id != emission_id ||
            p->contact_physx_person != 0 || p->contact_person ||
            InterlockedCompareExchange(&p->model_contact_verified, 0, 0) ||
            p->collision_age > 0.25f || p->model_contact_probe_count >= 3 ||
            (p->model_contact_probe_count && now - p->model_contact_probe_tick < 16)) continue;
        p->model_contact_probe_tick = now; p->model_contact_probe_count++;
        budget--;
        contact.emission_id = p->emission_id; contact.particle_id = p->spawn_order;
        memcpy(contact.impact_world, p->position, sizeof(contact.impact_world));
        /* The camera ray samples the visible model at the depth impact's
           exact pixel. No lateral retry offsets and no neighborhood inference. */
        if (!liquid_native_contact_camera_ray(&contact, origin, direction)) continue;
        tramp_AppPick_PickRay(self, geometry_list_ref, origin, direction,
                             results, include_hidden, flags);
        if (!liquid_native_pick_result_data(results, &data, &count) ||
            !ptr_readable(data, 0x20)) continue;
        memcpy(hit, (const BYTE*)data + 0x14, sizeof(hit));
        if (!liquid_confirm_particle_model_contact(&contact, hit)) {
            static int mismatch_logged;
            float expected[3], delta[3];
            int axis;
            if (mismatch_logged < 24 && liquid_world_to_view_point(contact.impact_world, expected)) {
                for (axis = 0; axis < 3; axis++) delta[axis] = hit[axis] - expected[axis];
                mismatch_logged++;
                log_line("liquid individual body verification rejected particle=%u gap=%.6f age=%.4f", contact.particle_id,
                    sqrtf(liquid_vec3_dot(delta, delta)), p->collision_age);
            }
        }
    }
}

static int THISCALL hook_AppPick_PickRay(
    void *self, const void *geometry_list_ref, const float *origin,
    const float *direction, void *results, int include_hidden,
    unsigned int flags)
{
#if defined(__GNUC__)
    const void *caller = __builtin_return_address(0);
#else
    const void *caller = NULL;
#endif
    DWORD now = GetTickCount();
    int result = -1;
    int used_custom_contact = 0;
    int custom_contact_hit = 0;
    int custom_contact_retried = 0;
    int used_camera_fallback = 0;
    unsigned int custom_particle_id = 0;
    void *native_frame = NULL;
    void *native_descriptor = NULL;
    liquid_emitter_t *model_emitter = NULL;
    int native_stain_caller = caller &&
        caller == (const BYTE*)GetModuleHandleA(NULL) +
                      LIQUID_NATIVE_STAIN_PICK_CALLER_RVA;
    if (native_stain_caller) {
        liquid_native_finish_frozen_capture();
        if (liquid_native_freeze_capture)
            liquid_native_freeze_capture->projector_frame = NULL;
        native_frame = __builtin_frame_address(1);
        if (native_frame &&
            ptr_readable((const BYTE*)native_frame - 0x14, sizeof(void*)))
            native_descriptor =
                *(void**)((const BYTE*)native_frame - 0x14);
        if (native_descriptor) {
            int emitter_index;
            for (emitter_index = 0;
                 emitter_index < LIQUID_MODEL_EMITTER_COUNT;
                 emitter_index++) {
                if (model_emitters[emitter_index].native_stain_descriptor ==
                    native_descriptor) {
                    model_emitter = &model_emitters[emitter_index];
                    break;
                }
            }
        }
        if (!model_emitter) model_emitter = liquid_latest_model_emitter();
        if (native_descriptor &&
            model_emitter &&
            ptr_readable(native_descriptor, 0x40) &&
            ptr_writable((BYTE*)native_descriptor + 0x10, sizeof(LONG))) {
            model_emitter->native_stain_descriptor = native_descriptor;
            void *previous = InterlockedExchangePointer(
                (PVOID volatile *)&liquid_native_stain_descriptor,
                native_descriptor);
            LONG previous_emission = InterlockedExchange(
                &liquid_native_stain_descriptor_emission,
                (LONG)model_emitter->emission_id);
            if (previous != native_descriptor ||
                previous_emission != (LONG)model_emitter->emission_id) {
                const float descriptor_rate =
                    *(const float*)((const BYTE*)native_descriptor + 0x20);
                const float descriptor_size =
                    *(const float*)((const BYTE*)native_descriptor + 0x24);
                const float descriptor_field28 =
                    *(const float*)((const BYTE*)native_descriptor + 0x28);
                const float descriptor_field2c =
                    *(const float*)((const BYTE*)native_descriptor + 0x2c);
                const float descriptor_field30 =
                    *(const float*)((const BYTE*)native_descriptor + 0x30);
                const float descriptor_scale_x =
                    *(const float*)((const BYTE*)native_descriptor + 0x34);
                const float descriptor_scale_y =
                    *(const float*)((const BYTE*)native_descriptor + 0x38);
                const float descriptor_scale_z =
                    *(const float*)((const BYTE*)native_descriptor + 0x3c);
                log_line("liquid native stain descriptor captured emission=%u frame=%p descriptor=%p rate=%.7g size=%.7g field28=%.7g field2c=%.7g field30=%.7g scale=(%.7g,%.7g,%.7g)",
                         model_emitter->emission_id, native_frame,
                         native_descriptor, descriptor_rate,
                         descriptor_size, descriptor_field28,
                         descriptor_field2c, descriptor_field30,
                         descriptor_scale_x, descriptor_scale_y,
                         descriptor_scale_z);
                log_words("native-stain-descriptor", native_descriptor, 64);
            }
        }
    }
    if (!model_emitter) model_emitter = liquid_latest_model_emitter();
    if (tramp_AppPick_PickRay && cfg.liquids_enabled && native_stain_caller &&
        model_emitter && model_emitter->emission_id &&
        now <= model_emitter->end_tick + LIQUID_NATIVE_CONTACT_MAX_AGE_MS)
        liquid_verify_recent_body_contacts(self, geometry_list_ref, results,
            include_hidden, flags, model_emitter->emission_id, now);
    if (tramp_AppPick_PickRay && cfg.liquids_enabled &&
        cfg.collision_spawn_model_stains && model_emitter &&
        model_emitter->emission_id &&
        now <= model_emitter->end_tick + LIQUID_NATIVE_CONTACT_MAX_AGE_MS &&
        native_stain_caller) {
        liquid_native_contact_t contact;
        float custom_origin[3];
        float custom_direction[3];
        int have_custom_ray = 0;
        if (liquid_pop_native_model_contact(
                model_emitter->emission_id, now, &contact) &&
            liquid_native_contact_view_ray(
                &contact, custom_origin, custom_direction)) {
            custom_particle_id = contact.particle_id;
            have_custom_ray = 1;
        }
        if (have_custom_ray) {
            void *data = NULL;
            int count = 0;
            if (liquid_native_replace_projector_transform(
                    native_frame, custom_origin, custom_direction) &&
                liquid_native_replace_caller_ray(
                    origin, direction, custom_origin, custom_direction)) {
                used_custom_contact = 1;
                result = tramp_AppPick_PickRay(
                    self, geometry_list_ref, origin, direction,
                    results, include_hidden, flags);
            } else {
                log_line("liquid native stain contact skipped particle=%u emission=%u reason=caller-ray-not-writable",
                         custom_particle_id, contact.emission_id);
            }
            liquid_native_pick_result_data(results, &data, &count);
            if (used_custom_contact && count <= 0 &&
                liquid_native_contact_camera_ray(
                    &contact, custom_origin, custom_direction)) {
                data = NULL;
                count = 0;
                used_camera_fallback = 1;
                if (liquid_native_replace_projector_transform(
                        native_frame, custom_origin, custom_direction) &&
                    liquid_native_replace_caller_ray(
                        origin, direction, custom_origin,
                        custom_direction)) {
                    result = tramp_AppPick_PickRay(
                        self, geometry_list_ref, origin, direction,
                        results, include_hidden, flags);
                    liquid_native_pick_result_data(results, &data, &count);
                }
            }
            custom_contact_hit = 0;
            if (used_custom_contact) {
                if (result >= 0 && count > 0)
                    liquid_native_stage_surface_projector(native_frame, data);
                custom_contact_hit = liquid_native_track_custom_stain_pick(
                    native_descriptor, &contact, result, results);
                if (!custom_contact_hit && result >= 0 && count > 0) {
                    static int mismatch_logged;
                    if (mismatch_logged++ < 24)
                        log_line("liquid model attachment rejected particle=%u reason=hit-does-not-confirm-contact-surface", custom_particle_id);
                }
            }
            if (!custom_contact_hit) {
                custom_contact_retried =
                    liquid_retry_native_model_contact(&contact, now);
            }
            if (custom_contact_hit && data && ptr_readable(data, 0x20)) {
                /* The native routine now receives this same synchronized
                   ray both for picking and for its post-pick local-plane
                   construction. Record the hit and descriptor scale for
                   diagnostics without modifying the descriptor. */
                float hit_view[3];
                float descriptor_scale[3] = {NAN, NAN, NAN};
                hit_view[0] = *(const float*)((const BYTE*)data + 0x14);
                hit_view[1] = *(const float*)((const BYTE*)data + 0x18);
                hit_view[2] = *(const float*)((const BYTE*)data + 0x1c);
                if (native_descriptor &&
                    ptr_readable((const BYTE*)native_descriptor + 0x34,
                                 sizeof(float) * 3)) {
                    descriptor_scale[0] =
                        *(const float*)((const BYTE*)native_descriptor + 0x34);
                    descriptor_scale[1] =
                        *(const float*)((const BYTE*)native_descriptor + 0x38);
                    descriptor_scale[2] =
                        *(const float*)((const BYTE*)native_descriptor + 0x3c);
                }
                log_line("liquid native stain transform particle=%u frame=%p descriptor=%p descriptor_scale=(%.5f,%.5f,%.5f) custom_hit_view=(%.5f,%.5f,%.5f) custom_ray_view=(%.5f,%.5f,%.5f)->(%.5f,%.5f,%.5f)",
                         custom_particle_id, native_frame, native_descriptor,
                         descriptor_scale[0], descriptor_scale[1],
                         descriptor_scale[2], hit_view[0], hit_view[1],
                         hit_view[2], custom_origin[0], custom_origin[1],
                         custom_origin[2], custom_direction[0],
                         custom_direction[1], custom_direction[2]);
            }
            log_line("liquid native stain contact consumed source=confirmed particle=%u emission=%u retry=%u result=%d count=%d native_hit=%d body_confirmed=%d retried=%d outcome=%s",
                     custom_particle_id, contact.emission_id,
                     contact.retry_count, result, count,
                     used_custom_contact && result >= 0 && count > 0,
                     custom_contact_hit, custom_contact_retried,
                     custom_contact_hit ?
                         (used_camera_fallback ?
                              "native-model-hit-camera-ray" :
                              "native-model-hit-impact-ray") :
                         (custom_contact_retried ?
                              "native-model-miss-requeued" :
                              "native-model-miss-final"));
        }
    }
    if (!tramp_AppPick_PickRay) {
        result = -1;
    } else if (!used_custom_contact) {
        /* Preserve TK17's exact original stain behavior whenever there is no
           queued custom contact. A confirmed contact that misses the model
           geometry must remain a miss; retrying TK17's unrelated old ray can
           place a decal before or away from the custom droplet. */
        result = tramp_AppPick_PickRay(
            self, geometry_list_ref, origin, direction, results,
            include_hidden, flags);
    }
    if (cfg.enabled && liquid_any_model_emitter_active(now)) {
        LONG sequence = InterlockedIncrement(&liquid_native_pick_probe_count);
        if (sequence <= 80 && reserve_event()) {
            const char *geometry_list =
                stringref_cstr_a(geometry_list_ref);
            void *data = NULL;
            int count = -1;
            char caller_text[320];
            float ox = 0.0f, oy = 0.0f, oz = 0.0f;
            float dx = 0.0f, dy = 0.0f, dz = 0.0f;
            if (origin && ptr_readable(origin, sizeof(float) * 3)) {
                ox = origin[0]; oy = origin[1]; oz = origin[2];
            }
            if (direction && ptr_readable(direction, sizeof(float) * 3)) {
                dx = direction[0]; dy = direction[1]; dz = direction[2];
            }
            if (results && ptr_readable(results, sizeof(void*))) {
                data = *(void**)results;
                if (data && ptr_readable((BYTE*)data - sizeof(int),
                                         sizeof(int)))
                    count = *((const int*)data - 1);
            }
            log_module_address(caller, caller_text, sizeof(caller_text));
            log_line("NATIVE STAIN PICK sequence=%ld caller=%s self=%p list=\"%s\" origin=(%.5f,%.5f,%.5f) direction=(%.5f,%.5f,%.5f) include_hidden=%d flags=0x%08lx result=%d count=%d data=%p custom_contact=%d predicted=0 custom_particle=%u custom_hit=%d",
                     sequence, caller_text, self,
                     geometry_list ? geometry_list : "",
                     ox, oy, oz, dx, dy, dz, include_hidden,
                     (unsigned long)flags, result, count, data,
                     used_custom_contact, custom_particle_id,
                     custom_contact_hit);
            if (count > 0 && count < 4096 && data &&
                ptr_readable(data, 96)) {
                log_words("native-stain-pick-result", data, 96);
                /* The caller performs the native shader attachment directly
                   after PickRay returns. */
                InterlockedExchange(&liquid_native_stain_trace_until_tick,
                                    (LONG)(GetTickCount() + 750));
            }
        }
    }
    return result;
}

static void install_hooks(void)
{
    static const BYTE app_expected[5] = {0x55, 0x8b, 0xec, 0x53, 0x56};
    static const BYTE native_stain_cache_expected[5] = {
        0x55, 0x8b, 0xec, 0x51, 0x53
    };
    static const BYTE native_stain_update_expected[9] = {
        0x55, 0x8b, 0xec, 0x81, 0xec, 0x28, 0x03, 0x00, 0x00
    };
    HMODULE exe = GetModuleHandleA(NULL);
    HMODULE app = GetModuleHandleA("ThriXXX010278-APP.dll");
    HMODULE sys = GetModuleHandleA("ThriXXX010278-SYS.dll");
    BYTE *target;
    if (!app || !sys || !exe) return;
    /* The settings bridge must remain available even when the liquids master
       switch is off, otherwise the in-game menu could never turn it on. */
    patch_liquid_config_editor_hooks();
    if (!cfg.enabled && !cfg.liquids_enabled) return;
    resolve_engine_symbols();

    if (!camera_hook_ready) {
        target = (BYTE*)GetProcAddress(
            app,
            "?SetWorldMatrixInverse@AppTracker@@QAEXABVMatrix4f@Bionic@@@Z");
        if (target && ptr_executable(target) &&
            install_inline_hook(target,
                                (void*)hook_AppTracker_SetWorldMatrixInverse,
                                7,
                                (void**)&tramp_AppTracker_SetWorldMatrixInverse)) {
            InterlockedExchange(&camera_hook_ready, 1);
            log_line("liquid camera world-space hook installed target=%p previous=%p chaining=1",
                     target, (void*)tramp_AppTracker_SetWorldMatrixInverse);
        }
    }

    if (!app_command_hook_installed) {
        target = (BYTE*)GetProcAddress(
            app, "?Command@AppMain@@YA?AW4EResult@Bionic@@ABVNameHash@3@@Z");
        if (!target) target = (BYTE*)GetProcAddress(app, (LPCSTR)34);
        if (target && ptr_executable(target) &&
            (target[0] == 0xe9 ||
             memcmp(target, app_expected, sizeof(app_expected)) == 0) &&
            install_inline_hook(target, (void*)hook_AppMain_Command, 5,
                                (void**)&real_AppMain_Command)) {
            InterlockedExchange(&app_command_hook_installed, 1);
            log_line("diagnostic AppMain::Command hook installed target=%p previous=%p chaining=1",
                     target, (void*)real_AppMain_Command);
        }
    }

    if (!object_name_hook_installed) {
        target = (BYTE*)GetProcAddress(
            sys,
            "?iNameSet@Object@Bionic@@QAEXPBVClassMember_String@2@ABVStringRef@2@@Z");
        if (target && ptr_executable(target) &&
            install_inline_hook(target, (void*)hook_Object_iNameSet, 6,
                                (void**)&tramp_Object_iNameSet)) {
            InterlockedExchange(&object_name_hook_installed, 1);
            log_line("diagnostic Object::iNameSet hook installed target=%p previous=%p chaining=1",
                     target, (void*)tramp_Object_iNameSet);
        }
    }

    if (cfg.enabled && !clone_object_hook_installed) {
        target = (BYTE*)GetProcAddress(
            sys, "?CloneObject@Bionic@@YAPAVScriptObject@1@PBV21@@Z");
        if (target && ptr_executable(target) &&
            install_inline_hook(target, (void*)hook_CloneObject, 7,
                                (void**)&tramp_CloneObject)) {
            InterlockedExchange(&clone_object_hook_installed, 1);
            log_line("diagnostic native-stain CloneObject hook installed target=%p previous=%p chaining=1",
                     target, (void*)tramp_CloneObject);
        }
    }

    if (cfg.enabled && !native_stain_cache_hook_installed) {
        target = (BYTE*)exe + LIQUID_NATIVE_STAIN_CACHE_RVA;
        if (ptr_executable(target) &&
            (target[0] == 0xe9 ||
             memcmp(target, native_stain_cache_expected,
                    sizeof(native_stain_cache_expected)) == 0) &&
            install_inline_hook(target, (void*)hook_NativeStainCache, 5,
                                (void**)&tramp_NativeStainCache)) {
            InterlockedExchange(&native_stain_cache_hook_installed, 1);
            log_line("diagnostic native-stain cache hook installed target=%p rva=0x%08lx previous=%p chaining=1",
                     target, (unsigned long)LIQUID_NATIVE_STAIN_CACHE_RVA,
                     (void*)tramp_NativeStainCache);
        } else {
            log_line("diagnostic native-stain cache hook not-installed target=%p rva=0x%08lx reason=\"signature mismatch or inline patch failed\"",
                     target, (unsigned long)LIQUID_NATIVE_STAIN_CACHE_RVA);
        }
    }

    if (cfg.liquids_enabled && cfg.collision_spawn_model_stains &&
        !native_stain_projector_hook_installed) {
        static const BYTE expected[] = {0xd9, 0x85, 0x7c, 0xff, 0xff, 0xff};
        target = (BYTE*)exe + LIQUID_NATIVE_STAIN_PROJECTOR_RVA;
        if (ptr_executable(target) &&
            memcmp(target, expected, sizeof(expected)) == 0 &&
            install_inline_hook(target, (void*)hook_NativeStainProjector,
                                sizeof(expected), &tramp_NativeStainProjector)) {
            native_stain_projector_hook_installed = 1;
            log_line("liquid surface decal projector hook installed rva=0x%08lx",
                     (unsigned long)LIQUID_NATIVE_STAIN_PROJECTOR_RVA);
        } else {
            log_line("liquid surface decal projector hook not-installed reason=signature-or-hook-failed");
        }
    }

    if (cfg.liquids_enabled &&
        (cfg.collision_spawn_model_stains ||
         cfg.collision_follow_bodies) &&
        !native_stain_update_hook_installed) {
        target = (BYTE*)exe + LIQUID_NATIVE_STAIN_UPDATE_RVA;
        if ((!cfg.collision_spawn_model_stains || engine_TimerSetSeconds) &&
            ptr_executable(target) &&
            (target[0] == 0xe9 ||
             memcmp(target, native_stain_update_expected,
                    sizeof(native_stain_update_expected)) == 0) &&
            install_inline_hook(target, (void*)hook_NativeStainUpdate,
                                sizeof(native_stain_update_expected),
                                (void**)&tramp_NativeStainUpdate)) {
            InterlockedExchange(&native_stain_update_hook_installed, 1);
            log_line("native stain confirmed-contact update hook installed target=%p rva=0x%08lx previous=%p timer_set=%p chaining=1",
                     target, (unsigned long)LIQUID_NATIVE_STAIN_UPDATE_RVA,
                     (void*)tramp_NativeStainUpdate,
                     (void*)engine_TimerSetSeconds);
        } else {
            log_line("native stain confirmed-contact update hook not-installed target=%p rva=0x%08lx timer_set=%p reason=\"signature mismatch, missing Timer::SetSeconds, or inline patch failed\"",
                     target, (unsigned long)LIQUID_NATIVE_STAIN_UPDATE_RVA,
                     (void*)engine_TimerSetSeconds);
        }
    }

    if (cfg.liquids_enabled && cfg.collision_spawn_model_stains &&
        !app_pick_ray_hook_installed) {
        target = (BYTE*)GetProcAddress(
            app,
            "?PickRay@AppPick@@QAEHABVStringRef@Bionic@@ABVVector3f@3@1AAV?$Array@VPickResult@@@3@_NI@Z");
        if (target && ptr_executable(target) &&
            install_inline_hook(target, (void*)hook_AppPick_PickRay, 6,
                                (void**)&tramp_AppPick_PickRay)) {
            InterlockedExchange(&app_pick_ray_hook_installed, 1);
            log_line("diagnostic AppPick::PickRay hook installed target=%p previous=%p chaining=1",
                     target, (void*)tramp_AppPick_PickRay);
        }
    }
}

static BYTE *find_ascii(HMODULE module, const char *needle)
{
    IMAGE_DOS_HEADER *dos;
    IMAGE_NT_HEADERS *nt;
    BYTE *base = (BYTE*)module;
    size_t needle_len;
    DWORD i;
    if (!base || !needle) return NULL;
    dos = (IMAGE_DOS_HEADER*)base;
    if (!ptr_readable(dos, sizeof(*dos)) ||
        dos->e_magic != IMAGE_DOS_SIGNATURE) return NULL;
    nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);
    if (!ptr_readable(nt, sizeof(*nt)) ||
        nt->Signature != IMAGE_NT_SIGNATURE) return NULL;
    needle_len = strlen(needle);
    for (i = 0; i + needle_len <= nt->OptionalHeader.SizeOfImage; i++) {
        if (base[i] == (BYTE)needle[0] &&
            memcmp(base + i, needle, needle_len) == 0)
            return base + i;
    }
    return NULL;
}

static void log_string_xrefs(HMODULE module, const char *module_name,
                             const char *needle)
{
    IMAGE_DOS_HEADER *dos;
    IMAGE_NT_HEADERS *nt;
    IMAGE_SECTION_HEADER *section;
    BYTE *base = (BYTE*)module;
    BYTE *found = find_ascii(module, needle);
    int s;
    int count = 0;
    if (!found) return;
    dos = (IMAGE_DOS_HEADER*)base;
    nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);
    section = IMAGE_FIRST_SECTION(nt);
    log_line("address marker module=%s text=\"%s\" address=%p rva=0x%08lx",
             module_name, needle, found,
             (unsigned long)(found - base));
    for (s = 0; s < nt->FileHeader.NumberOfSections && count < 12; s++) {
        DWORD offset;
        DWORD size;
        BYTE *start;
        if (!(section[s].Characteristics & IMAGE_SCN_MEM_EXECUTE)) continue;
        start = base + section[s].VirtualAddress;
        size = section[s].Misc.VirtualSize;
        for (offset = 0; offset + sizeof(DWORD) <= size && count < 12;
             offset++) {
            DWORD value;
            memcpy(&value, start + offset, sizeof(value));
            if (value == (DWORD)(uintptr_t)found) {
                log_line("address xref module=%s text=\"%s\" address=%p rva=0x%08lx",
                         module_name, needle, start + offset,
                         (unsigned long)(start + offset - base));
                count++;
            }
        }
    }
}

static void scan_known_addresses(void)
{
    static const char *exe_markers[] = {
        "CumShot", "InternalCum_Start", "Finish_CumShot",
        "Person_CumShot", "spermray01_init", "spermray01_splash",
        "CumShot/Done"
    };
    static const char *type_markers[] = {
        ".?AVInternalCumControl@@",
        ".?AVInternalCumEmitterMap@@",
        ".?AVInternalCumZone@@"
    };
    HMODULE exe = GetModuleHandleA(NULL);
    HMODULE app = GetModuleHandleA("ThriXXX010278-APP.dll");
    HMODULE sys = GetModuleHandleA("ThriXXX010278-SYS.dll");
    size_t i;
    if (InterlockedCompareExchange(&address_scan_done, 1, 0) != 0) return;
    log_line("engine modules exe=%p app=%p sys=%p FindObjC=%p GetModelViewRotationPivot=%p GetWeakObjTarget=%p StringRefHash32=%p NameHashFind=%p",
             exe, app, sys, (void*)engine_FindObjC,
             (void*)engine_GetModelViewRotationPivot,
             (void*)engine_GetWeakObjTarget,
             (void*)engine_StringRefHash32, (void*)engine_NameHashFind);
    log_line("engine nil sentinels NilWeakSlot=%p NilWeak=%p NilObjectSlot=%p NilObject=%p",
             (void*)engine_G_NilWeakObjTarget_ptr,
             (engine_G_NilWeakObjTarget_ptr &&
              ptr_readable(engine_G_NilWeakObjTarget_ptr, sizeof(void*))) ?
                 *engine_G_NilWeakObjTarget_ptr : NULL,
             (void*)engine_G_NilObject_ptr,
             (engine_G_NilObject_ptr &&
              ptr_readable(engine_G_NilObject_ptr, sizeof(void*))) ?
                 *engine_G_NilObject_ptr : NULL);
    for (i = 0; i < sizeof(exe_markers) / sizeof(exe_markers[0]); i++)
        log_string_xrefs(exe, "TK17-158.001.exe", exe_markers[i]);
    for (i = 0; i < sizeof(type_markers) / sizeof(type_markers[0]); i++) {
        log_string_xrefs(exe, "TK17-158.001.exe", type_markers[i]);
        log_string_xrefs(app, "ThriXXX010278-APP.dll", type_markers[i]);
        log_string_xrefs(sys, "ThriXXX010278-SYS.dll", type_markers[i]);
    }
}

static DWORD WINAPI startup_worker(void *unused)
{
    int attempts = 0;
    (void)unused;
    while (!shutdown_requested && attempts < 80) {
        HMODULE exe = GetModuleHandleA(NULL);
        HMODULE app = GetModuleHandleA("ThriXXX010278-APP.dll");
        HMODULE sys = GetModuleHandleA("ThriXXX010278-SYS.dll");
        if (exe && app && sys) {
            if (!address_scan_done) {
                if (!supported_executable(exe)) {
                    log_line("unsupported TK17 executable; diagnostic hooks disabled to avoid corrupting an unknown build");
                    return 0;
                }
                resolve_engine_symbols();
                scan_known_addresses();
            }
            liquid_install_graphics_hooks();
            install_hooks();
            if (app_command_hook_installed && object_name_hook_installed &&
                engine_GetModelViewRotationPivot && graphics_hook_ready) {
                log_line("liquid prototype ready command_hook=1 object_name_hook=1 camera_hook=%ld graphics_device_hook=%ld enabled=%d model_duration=%.3f tool_duration=%.3f particle_limit=%d",
                         camera_hook_ready, graphics_hook_ready,
                         cfg.liquids_enabled, cfg.model_duration,
                         cfg.tool_duration, cfg.particle_limit);
                return 0;
            }
        }
        attempts++;
        Sleep(250);
    }
    log_line("diagnostic startup incomplete attempts=%d command_hook=%ld object_name_hook=%ld graphics_device_hook=%ld hook5_callback=%ld",
             attempts, app_command_hook_installed, object_name_hook_installed,
             graphics_hook_ready, hook5_present_registered);
    return 0;
}

static void start_diagnostics(void)
{
    HANDLE thread;
    if (InterlockedCompareExchange(&startup_started, 1, 0) == 0) {
        thread = CreateThread(NULL, 0, startup_worker, NULL, 0, NULL);
        if (thread) CloseHandle(thread);
        else log_line("diagnostic startup failed CreateThread error=%lu",
                      (unsigned long)GetLastError());
    }
}

__declspec(dllexport) int loadextension(void)
{
    start_diagnostics();
    log_line("loadextension");
    return 1;
}

__declspec(dllexport) int on_create(void)
{
    start_diagnostics();
    log_line("on_create");
    return 1;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    (void)reserved;
    if (reason == DLL_PROCESS_ATTACH) {
        self_module = instance;
        DisableThreadLibraryCalls(instance);
        InitializeCriticalSection(&log_lock);
        InitializeCriticalSection(&contact_lock);
        build_paths();
        liquid_audio_scan_preset_names();
        create_default_config_if_missing();
        load_config();
        {
            WIN32_FILE_ATTRIBUTE_DATA attributes;
            if (GetFileAttributesExA(config_path, GetFileExInfoStandard,
                                     &attributes)) {
                liquid_config_write_time = attributes.ftLastWriteTime;
                liquid_config_write_time_valid = 1;
            }
        }
        if (cfg.enabled) {
            ensure_log_directory();
            if (cfg.clear_log_on_startup) DeleteFileA(log_path);
        }
        log_ready = 1;
        log_line("NC-TK17-Liquids.dll attached version=%s diagnostic_enabled=%d liquids_enabled=%d config=\"%s\"",
                 LIQUIDS_VERSION, cfg.enabled, cfg.liquids_enabled,
                 config_path);
    } else if (reason == DLL_PROCESS_DETACH) {
        InterlockedExchange(&shutdown_requested, 1);
        if (log_ready) log_line("NC-TK17-Liquids.dll detached");
        log_ready = 0;
        DeleteCriticalSection(&contact_lock);
        DeleteCriticalSection(&log_lock);
    }
    return TRUE;
}
