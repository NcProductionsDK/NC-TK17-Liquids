#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static struct { int count; BYTE data[0x58]; } picked;
static struct { int count; void *data[1]; } group_array;
static struct { int count; void *data[256]; } control_array;
static struct { int count; void *data[256]; } node_array;
static BYTE update[64], group[64], frame_storage[1024];
static BYTE mesh_storage[128] __attribute__((aligned(16))), meta[512], type[64];
static void *mesh, *frame;
static liquid_native_freeze_capture_t capture;
static liquid_native_contact_t contact;
static int pick_calls, strings_created, strings_released, pick_result;
static float hit_gap, normal_sign;
static void THISCALL string_new(char **out, const char *value)
{ CHECK(!strcmp(value,"Room|PoseEditRoom")); *out=(char*)value; strings_created++; }
static void THISCALL string_free(char **out) { CHECK(*out); *out=NULL; strings_released++; }
static int THISCALL room_pick(void *self, const void *geometry, const float *origin,
    const float *direction, void *results, int hidden, unsigned int version)
{
    float hit[3], normal[3]; (void)self;
    CHECK(!strcmp((const char*)geometry,"Room|PoseEditRoom") && !hidden && version==37);
    CHECK(fabsf(liquid_vec3_dot(direction,direction)-1)<0.0001f);
    CHECK(liquid_world_to_view_point(contact.impact_world,hit));
    for(int i=0;i<3;i++) normal[i]=normal_sign*liquid_vec3_dot(contact.normal_world,captured_camera_inverse+i*4);
    CHECK(fabsf(liquid_vec3_dot(normal,direction)+normal_sign)<0.0001f);
    CHECK(fabsf(origin[0]-hit[0]-normal[0]*0.03f/normal_sign)<0.0001f);
    hit[0]+=hit_gap;
    memset(picked.data,0,sizeof(picked.data)); picked.count=pick_result>=0 ? 1 : 0;
    *(void**)(picked.data+4)=mesh;
    memcpy(picked.data+0x14,hit,12); memcpy(picked.data+0x20,normal,12);
    *(void**)results=picked.data; pick_calls++;
    return pick_result;
}
static void setup(int owner)
{
    memset(&cfg,0,sizeof(cfg)); memset(liquid_particles,0,sizeof(liquid_particles));
    memset(liquid_native_contacts,0,sizeof(liquid_native_contacts));
    memset((void*)liquid_native_last_contact_queue_tick,0,sizeof(liquid_native_last_contact_queue_tick));
    memset(&capture,0,sizeof(capture)); memset(&contact,0,sizeof(contact));
    cfg.liquids_enabled=cfg.pov_enabled=cfg.collision_spawn_room_stains=cfg.collision_spawn_model_stains=1;
    cfg.particle_limit=2; cfg.particle_size=0.0035f; cfg.collision_model_stain_rate=12;
    cfg.collision_surface_hold=1.75f; native_stain_projector_hook_installed=1;
    liquid_room_clip_trampoline=(void*)1; /* This fixture mocks native extraction. */
    captured_camera_inverse_valid=1; liquid_scene_camera_override=NULL;
    memset(captured_camera_inverse,0,sizeof(captured_camera_inverse));
    captured_camera_inverse[0]=captured_camera_inverse[5]=captured_camera_inverse[10]=captured_camera_inverse[15]=1;
    liquid_particle_t *p=&liquid_particles[0];
    p->active=p->collided=p->source_kind=1; p->emission_id=42; p->spawn_order=1;
    p->contact_physx_person=owner; p->position[2]=-2; p->contact_normal_valid=1;
    p->contact_normal[1]=1; p->impact_velocity[1]=-1; p->lifetime=10;
    memset(model_emitters,0,sizeof(model_emitters));
    model_emitters[0].emission_id=42; model_emitters[0].source_kind=model_emitters[0].person_index=1;
    model_emitters[0].end_tick=GetTickCount()+10000;
    contact.emission_id=42; contact.particle_id=1; contact.impact_world[2]=-2;
    contact.target_kind=owner<0 ? 1 : owner==0 ? 2 : 0; contact.normal_world[1]=1;
    contact.tick=GetTickCount(); contact.direction_world[1]=-1;
    mesh=mesh_storage+64; memset(meta,0,sizeof(meta)); memset(type,0,sizeof(type));
    *(void**)((BYTE*)mesh-0x18)=meta; *(void**)(meta+0x1dc)=type+32; *(int*)(type+4)=1;
    group_array.count=1; group_array.data[0]=group; control_array.count=node_array.count=0;
    *(void**)(group+0x0c)=node_array.data;
    *(void**)(update+0x18)=group_array.data; *(void**)(group+0x10)=control_array.data;
    capture.update=update; liquid_native_freeze_capture=&capture;
    frame=frame_storage+sizeof(frame_storage); liquid_room_last_stain_tick=0;
    tramp_AppPick_PickRay=room_pick; liquid_room_string_construct=string_new; liquid_room_string_release=string_free;
    pick_calls=strings_created=strings_released=0; pick_result=0; hit_gap=0; normal_sign=1;
    picked.count=0;
}
static int run(int previous)
{
    float origin[3]={0}, direction[3]={0,0,-1}; void *results=picked.data;
    int r=liquid_try_room_stain_pick(NULL,&contact,frame,origin,direction,&results,37,previous);
    CHECK(strings_created==strings_released);
    return r;
}

static BYTE camera_object[128] __attribute__((aligned(16))), camera_parent_object[128] __attribute__((aligned(16)));
static void *THISCALL camera_parent(void *object, unsigned int property)
{ (void)object; CHECK(property==0x03fff043u); return camera_parent_object+64; }
static unsigned int THISCALL camera_version(void *object, unsigned int property)
{ (void)object; CHECK(property==0x05fff049u); return 37; }
static void test_pov_pick_path(void)
{
    static BYTE cm[512], pm[512], cd[512], pd[512], descriptor[64];
    void *camera=camera_object+64, *parent=camera_parent_object+64, *results=picked.data;
    setup(-1);
    *(void**)((BYTE*)camera-0x18)=cm; *(void**)((BYTE*)parent-0x18)=pm;
    *(void**)(cm+0x10c)=cd+32; *(unsigned int*)(cd+4)=1;
    *(void**)(cd+32+0xc0)=(void*)camera_parent;
    *(void**)(pm+0x124)=pd+32; *(unsigned int*)(pd+4)=1;
    *(void**)(pd+32+0x140)=(void*)camera_version;
    tool_emitter=model_emitters[0]; tool_emitter.source_kind=2; tool_emitter.active=1;
    tool_emitter.native_stain_descriptor=descriptor; model_emitters[0].emission_id=0;
    liquid_particles[0].source_kind=2;
    liquid_queue_native_model_contact(&liquid_particles[0]);
    CHECK(liquid_pick_tool_contact(NULL,NULL,camera,&results,1,frame,descriptor,GetTickCount())>=0);
    CHECK(pick_calls==1 && capture.projector_frame==frame && capture.attachment_outcome==LIQUID_ATTACHMENT_ROOM);
    CHECK(liquid_particles[0].room_contact_verified && !liquid_particles[0].model_contact_verified);
    /* Clear drops pending room/unknown requests while preserving body-only requests. */
    liquid_native_contacts[0]=contact; liquid_native_contacts[0].state=1;
    liquid_native_contacts[1]=contact; liquid_native_contacts[1].target_kind=0; liquid_native_contacts[1].state=1;
    liquid_room_clear_pending();
    CHECK(!liquid_native_contacts[0].state && liquid_native_contacts[1].state==1 && !liquid_room_last_stain_tick);
}
int main(void)
{
    /* Floor, wall and camera rotation: the room ray is matched in view space,
       and it never sets the body-follow flags. */
    for(int wall=0;wall<2;wall++) for(int rotated=0;rotated<2;rotated++) {
        setup(-1);
        if(wall) { contact.normal_world[1]=0; contact.normal_world[2]=1; }
        if(rotated) {
            captured_camera_inverse[0]=captured_camera_inverse[10]=0;
            captured_camera_inverse[2]=-1; captured_camera_inverse[8]=1;
            captured_camera_inverse[12]=0.4f;
        }
        CHECK(run(-1)>=0 && pick_calls==1 && capture.room_picks==1);
        CHECK(liquid_particles[0].room_contact_verified && !liquid_particles[0].model_contact_verified && !liquid_particles[0].model_contact_confirmed);
        CHECK(run(-1)<0 && pick_calls==1); /* 10/s cap */
    }
    setup(-1); hit_gap=0.1f; CHECK(run(-1)<0 && !liquid_particles[0].room_contact_verified);
    setup(-1); normal_sign=-1; CHECK(run(-1)<0);
    setup(-1); *(int*)(type+4)=0; CHECK(run(-1)<0); /* Unsupported mesh */
    setup(-1); liquid_particles[0].active=0; CHECK(run(-1)<0);
    setup(-1); pick_result=-1; CHECK(run(-1)<0);
    setup(-1); contact.normal_world[1]=NAN; CHECK(run(-1)<0 && !pick_calls);
    setup(-1); cfg.collision_spawn_room_stains=0; CHECK(run(-1)<0 && !pick_calls);
    setup(-1); control_array.count=256; CHECK(run(-1)<0 && !pick_calls);
    control_array.count=0; CHECK(run(-1)>=0); /* Native clear restores budget */
    setup(-1); node_array.count=256; CHECK(run(-1)<0 && !pick_calls);
    node_array.count=0; CHECK(run(-1)>=0); /* Static decals have no controls. */
    setup(-1); liquid_native_freeze_capture=NULL; CHECK(run(-1)<0 && !pick_calls);
    setup(1); CHECK(run(3)==3 && !pick_calls); /* Known body untouched */
    setup(0); picked.count=1; memcpy(picked.data+0x14,contact.impact_world,12);
    CHECK(run(2)==2 && !pick_calls && !liquid_particles[0].room_contact_verified);
    setup(0); picked.count=1; memset(picked.data+0x14,0,12);
    CHECK(run(0)>=0 && pick_calls==1 && contact.target_kind==1); /* Distant body cannot steal room */
    setup(0); cfg.collision_spawn_model_stains=0; CHECK(run(-1)>=0); /* Independent switch */
    /* Both emission sources can queue confirmed room contacts. Disabled room
       behavior is unchanged, and retries retain the surface and target. */
    for(int pov=0;pov<2;pov++) {
        setup(-1);
        if(pov) { tool_emitter=model_emitters[0]; tool_emitter.source_kind=2; tool_emitter.active=1; liquid_particles[0].source_kind=2; model_emitters[0].emission_id=0; }
        cfg.collision_spawn_room_stains=0; liquid_queue_native_model_contact(&liquid_particles[0]);
        CHECK(!liquid_native_model_contact_pending(42,GetTickCount()));
        cfg.collision_spawn_room_stains=1; liquid_queue_native_model_contact(&liquid_particles[0]);
        liquid_native_contact_t out={0}; CHECK(liquid_pop_native_model_contact(42,GetTickCount(),&out));
        CHECK(out.target_kind==1 && out.normal_world[1]==1);
        CHECK(liquid_retry_native_model_contact(&out,GetTickCount()));
        CHECK(liquid_pop_native_model_contact(42,GetTickCount()+40,&out));
        CHECK(out.target_kind==1 && out.normal_world[1]==1 && out.retry_count==1);
        if(pov) {
            liquid_native_contacts[0]=out; liquid_native_contacts[0].state=1;
            liquid_pov_cancel(); CHECK(!liquid_native_contacts[0].state && !liquid_particles[0].active);
        }
    }
    test_pov_pick_path();
    puts("PASS: floor/wall/rotated camera room picks, strict hit and mesh checks, no body attachment, body fallback/isolation, disabled/independent switches, native clear budget, rate cap, person/POV queues, retries, POV native pick route, pending cleanup and POV cancel");
    return 0;
}
