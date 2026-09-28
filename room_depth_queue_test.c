#define main depth_distance_regression_main
#include "depth_distance_test.c"
#undef main
static int __cdecl request_body(void) { return 1; }
static int __cdecl no_body(const float *world, float tolerance,
                          nc_tk17_physx_body_hit_v1_t *hit, unsigned int size)
{ (void)world; (void)tolerance; (void)hit; (void)size; return 0; }
int main(void)
{
    for(int source=1;source<=2;source++) for(int known_room=0;known_room<2;known_room++) {
        setup(1,0.00015f,0,source);
        cfg.collision_spawn_room_stains=1; cfg.collision_model_stain_rate=12;
        memset(model_emitters,0,sizeof(model_emitters)); memset(&tool_emitter,0,sizeof(tool_emitter));
        memset((void*)liquid_native_last_contact_queue_tick,0,sizeof(liquid_native_last_contact_queue_tick));
        liquid_emitter_t *e=source==2 ? &tool_emitter : &model_emitters[0];
        e->emission_id=1; e->person_index=1; e->source_kind=source; e->end_tick=GetTickCount()+1000;
        liquid_physx_request_body_colliders=known_room ? request_body : NULL;
        if(known_room) liquid_physx_query_body_collider=no_body;
        liquid_apply_depth_collisions(&snapshot,&mapped);
        liquid_native_contact_t contact={0};
        CHECK(liquid_particles[0].collided && liquid_particles[0].contact_normal_valid);
        CHECK(liquid_pop_native_model_contact(1,GetTickCount(),&contact));
        CHECK(contact.target_kind==(known_room ? 1 : 2));
        CHECK(fabsf(contact.normal_world[2]-1)<0.001f);
    }
    puts("PASS: actual depth collision publishes room normals before queueing, both emission sources and confirmed/unclassified room ownership");
    return 0;
}
