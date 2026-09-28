#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#c); exit(1); } } while (0)
static int picks, native_views;
static float hit_z = -2;
static struct { int count; BYTE data[0x58]; } hit;
static void *pick_camera, *pick_parent;
static unsigned int expected_matrix_version = 37;

static void *THISCALL camera_parent(void *object, unsigned int property)
{ CHECK(object==pick_camera && property==0x03fff043u); return pick_parent; }
static unsigned int THISCALL matrix_version(void *object, unsigned int property)
{ CHECK(object==pick_parent && property==0x05fff049u); return expected_matrix_version; }
static void setup_camera(void)
{
    static BYTE camera[128] __attribute__((aligned(16))), parent[128] __attribute__((aligned(16)));
    static BYTE cm[512], pm[512], cd[512], pd[512];
    pick_camera=camera+64; pick_parent=parent+64;
    *(void**)((BYTE*)pick_camera-0x18)=cm; *(void**)((BYTE*)pick_parent-0x18)=pm;
    *(void**)(cm+0x10c)=cd+32; *(unsigned int*)(cd+4)=1;
    *(void**)(cd+32+0xc0)=(void*)camera_parent;
    *(void**)(pm+0x124)=pd+32; *(unsigned int*)(pd+4)=1;
    *(void**)(pd+32+0x140)=(void*)matrix_version;
}
static int THISCALL pick(void *self, const void *geometry, const float *origin,
                        const float *direction, void *results, int hidden, unsigned int flags)
{
    (void)self;(void)geometry;(void)hidden;
    CHECK(flags==expected_matrix_version); /* Native picker skips mismatched versions. */
    CHECK(origin[2] < -1.8f && origin[2] > -2.1f); CHECK(direction[2] < 0);
    ++picks; hit.count = 1; memset(hit.data,0,sizeof(hit.data));
    *(float*)(hit.data+0x1c) = hit_z; *(float*)(hit.data+0x28) = 1;
    *(void**)results = hit.data;
    return 0;
}
static int THISCALL view(void *self,const void *g,void *c,const float *xy,void *r,int h)
{ (void)self;(void)g;(void)c;(void)xy;(void)r;(void)h; ++native_views; return 7; }

static void setup(void)
{
    memset(&cfg,0,sizeof(cfg)); memset(&tool_emitter,0,sizeof(tool_emitter));
    memset(liquid_particles,0,sizeof(liquid_particles));
    memset(liquid_native_contacts,0,sizeof(liquid_native_contacts));
    memset((void*)liquid_native_last_contact_queue_tick,0,sizeof(liquid_native_last_contact_queue_tick));
    cfg.liquids_enabled = cfg.pov_enabled = cfg.collision_spawn_model_stains = 1;
    cfg.collision_model_stain_rate = 20; cfg.particle_limit = 8; cfg.particle_size = 0.005f;
    cfg.pov_reach = 2; cfg.particle_lifetime = 3; cfg.speed = 2.06127f; cfg.gravity_strength = 1; cfg.drag = 0.08f;
    cfg.model_pulse_count = 10; cfg.model_pulse_duration = 0.5f; cfg.model_pulse_interval = 1;
    memset(captured_camera_inverse,0,sizeof(captured_camera_inverse));
    captured_camera_inverse[0]=captured_camera_inverse[5]=captured_camera_inverse[10]=captured_camera_inverse[15]=1;
    captured_camera_inverse_valid = 1;
    liquid_begin_emitter(&tool_emitter,2,0,GetTickCount(),liquid_tool_pulse_duration());
    picks=0; hit_z=-2; tramp_AppPick_PickRay=pick;
    setup_camera();
}

static void test_launch(void)
{
    const float targets[][3] = {{0.6f,1.3f,-2}, {-2,2,-5}, {1,0.5f,-1}};
    const float drags[] = {0,0.08f,2};
    setup();
    for (int d=0; d<3; ++d) for (int i=0;i<3;++i) for (int p=0;p<2;++p) {
        liquid_particle_t particle={0};
        particle.position[0]=0.2f; particle.position[1]=1; particle.position[2]=-0.2f;
        cfg.drag=drags[d];
        float t=liquid_tool_launch_velocity(particle.position,targets[i],p ? 0.65f : 1.0f,particle.velocity);
        CHECK(t>0 && t<=0.3001f);
        CHECK(sqrtf(liquid_vec3_dot(particle.velocity,particle.velocity))>cfg.speed);
        for(int j=0;j<60;++j) liquid_integrate_flight(&particle,t/60);
        for(int axis=0;axis<3;++axis) CHECK(fabsf(particle.position[axis]-targets[i][axis])<0.0001f);
    }
    puts("PASS: stronger POV launch reaches cursor target under actual gravity/drag integrator across pulse strengths");
}

static D3DMATRIX scene_projection;
static int __cdecl scene_matrix(unsigned int channel, float *matrix)
{ CHECK(channel==1); memcpy(matrix,&scene_projection,sizeof(scene_projection)); return 1; }
static void THISCALL tip_frame(void *object, unsigned int property, float *frame)
{
    CHECK(object==pick_parent && property==0x02fff049u);
    memset(frame,0,64); frame[0]=frame[5]=frame[10]=frame[15]=1;
    frame[13]=-0.05f; frame[14]=-0.2f;
}
static void test_aim_alignment(void)
{
    const RECT clients[]={{0,0,2560,1368},{0,0,1280,720},{0,0,1920,1080}};
    D3DVIEWPORT8 viewport={0,0,2560,1368,0,1};
    setup();
    scene_projection=(D3DMATRIX){0};
    scene_projection._11=1.2f; scene_projection._22=2.1f; scene_projection._34=-1;
    scene_projection._33=-1.001f; scene_projection._43=-0.1001f;
    captured_d3d_projection=scene_projection; captured_d3d_projection._11=4;
    captured_d3d_projection_valid=1; hook5_get_projection_matrix=scene_matrix;
    for(int camera=0;camera<2;++camera) for(int size=0;size<3;++size) for(int i=0;i<3;++i) {
        float frame[16], origin[3], dir[3], world[3], target[3];
        float sx,sy,depth,screen_depth;
        liquid_particle_t p={0}; DWORD now=GetTickCount();
        POINT cursor={clients[size].right*(i+1)/4,clients[size].bottom*2/5};
        if(camera) { /* Real world-to-view renderer must undo a turned camera. */
            captured_camera_inverse[0]=captured_camera_inverse[10]=0;
            captured_camera_inverse[2]=-1; captured_camera_inverse[8]=1;
            captured_camera_inverse[12]=3; captured_camera_inverse[13]=1;
        }
        CHECK(liquid_client_cursor_ndc(cursor,&clients[size],&liquid_tool_crosshair[0],&liquid_tool_crosshair[1]));
        liquid_tool_crosshair_tick=now;
        tip_frame(pick_parent,0x02fff049u,frame);
        CHECK(liquid_tool_outlet_view(frame,origin,dir));
        CHECK(liquid_tool_crosshair_direction(origin,dir,now)==1);
        CHECK(fabsf(liquid_tool_aim_target_view[0]*1.2f/2-liquid_tool_crosshair[0])<0.00001f);
        CHECK(liquid_view_to_world_point(origin,world));
        CHECK(liquid_view_to_world_point(liquid_tool_aim_target_view,target));
        float time=liquid_tool_launch_velocity(world,target,1,p.velocity);
        memcpy(p.position,world,sizeof(world));
        if(i==1 && !camera) CHECK(fabsf(p.velocity[0])<0.000001f);
        for(int j=0;j<60;++j) liquid_integrate_flight(&p,time/60);
        CHECK(liquid_project_world_d3d11(p.position,2560,1368,2560.0f/1368,0.414f,
            1,&scene_projection,&viewport,&sx,&sy,&depth,&screen_depth,NULL));
        CHECK(fabsf(sx/2560*clients[size].right-cursor.x)<0.01f);
        CHECK(fabsf(sy/1368*clients[size].bottom-cursor.y)<0.01f);
    }
    float x,y;
    CHECK(!liquid_client_cursor_ndc((POINT){-1,20},&clients[0],&x,&y));
    CHECK(!liquid_client_cursor_ndc((POINT){2560,20},&clients[0],&x,&y));
    CHECK(!liquid_client_cursor_ndc((POINT){0,0},&(RECT){0,0,0,0},&x,&y));
    hook5_get_projection_matrix=NULL; tool_mesh_object=NULL;
    puts("PASS: client pixel -> scene aim -> world flight -> actual renderer reaches cursor for left/center/right, rotated camera and three window sizes; outside/minimized rejected");
}

static void __attribute__((naked)) run_cleanup(void *app, void *site, int *path, float *out)
{
    __asm__ __volatile__(
        "pushl %ebx\n\tmovl 8(%esp), %ebx\n\tmovl 16(%esp), %eax\n\t"
        "movl 20(%esp), %edx\n\tfld1\n\tcall *12(%esp)\n\tpopl %ebx\n\tret\n\t");
}

static void test_fixed_reach(void)
{
    const float reaches[]={1,2.5f,4};
    const float origin[]={0.04f,-0.03f,-0.23f};
    D3DVIEWPORT8 viewport={0,0,2560,1368,0,1};
    setup(); captured_d3d_projection_valid=1;
    captured_d3d_projection=(D3DMATRIX){0};
    captured_d3d_projection._11=1.2901f; captured_d3d_projection._22=2.41421f;
    captured_d3d_projection._33=-1.001f; captured_d3d_projection._34=-1;
    captured_d3d_projection._43=-0.1001f;
    for(int r=0;r<3;++r) for(int pulse=0;pulse<10;++pulse) for(int frame=0;frame<15;++frame) {
        DWORD now=10000+1500*pulse+33*frame;
        float direction[3], x,y,depth,screen_depth;
        liquid_particle_t p={0};
        cfg.pov_reach=reaches[r];
        liquid_tool_crosshair[0]=0.12f+0.017f*frame;
        liquid_tool_crosshair[1]=0.4f-0.012f*frame;
        liquid_tool_crosshair_tick=now;
        /* No depth readbacks at all, including long pulse gaps. The old
           depth-dependent path fell back to one unit in these conditions. */
        CHECK(liquid_tool_crosshair_direction(origin,direction,now));
        CHECK(liquid_tool_aim_target_view[2]==-cfg.pov_reach);
        memcpy(p.position,origin,sizeof(origin));
        float time=liquid_tool_launch_velocity(origin,liquid_tool_aim_target_view,
            0.94f*(0.72f+0.3f*fmaxf(0.35f,1-pulse*0.1f)),p.velocity);
        CHECK(time>0);
        for(int step=0;step<60;++step) liquid_integrate_flight(&p,time/60);
        CHECK(fabsf(p.position[2]+cfg.pov_reach)<0.00001f);
        CHECK(liquid_project_world_d3d11(p.position,2560,1368,2560.0f/1368,0.414f,1,
            &captured_d3d_projection,&viewport,&x,&y,&depth,&screen_depth,NULL));
        CHECK(fabsf(x-(liquid_tool_crosshair[0]+1)*1280)<0.02f);
        CHECK(fabsf(y-(1-liquid_tool_crosshair[1])*684)<0.02f);
    }
    puts("PASS: fixed 1/2.5/4 reach through ten pulses, moving cursor and absent depth readbacks; compensated flight retains reach across pulse strengths");
}

static void test_close_surface_arc(void)
{
    const float drags[]={0,0.08f,2};
    float worst_old=0, worst_new=0;
    setup();
    for(int d=0;d<3;++d) for(int pulse=0;pulse<2;++pulse) {
        const float origin[]={0,-0.03f,-0.23f}, target[]={0,0,-2.5f};
        float strength=pulse ? 0.70f : 1.0f;
        float new_velocity[3], old_velocity[3];
        float distance=sqrtf(2.27f*2.27f+0.03f*0.03f);
        float old_time=fminf(fmaxf(distance/(fmaxf(6,cfg.speed*3)*strength),0.06f),0.6f);
        cfg.drag=drags[d];
        float new_time=liquid_tool_launch_velocity(origin,target,strength,new_velocity);
        CHECK(fabsf(new_time*2-old_time)<0.00001f);
        /* Reference trajectory from the previous release's flight time. */
        double travel=cfg.drag ? (1-exp(-cfg.drag*old_time))/cfg.drag : old_time;
        double gravity_travel=cfg.drag ? (old_time-travel)/cfg.drag : old_time*old_time*0.5;
        for(int axis=0;axis<3;++axis)
            old_velocity[axis]=(target[axis]-origin[axis]+(axis==1 ? 9.81*gravity_travel : 0))/travel;
        for(int surface=0;surface<3;++surface) {
            float depth=0.75f+surface*0.5f, heights[2];
            for(int version=0;version<2;++version) {
                liquid_particle_t p={0};
                memcpy(p.position,origin,sizeof(origin));
                memcpy(p.velocity,version ? new_velocity : old_velocity,sizeof(new_velocity));
                float fraction=(-depth-origin[2])/(target[2]-origin[2]);
                float duration=version ? new_time : old_time;
                double total_travel=cfg.drag ? (1-exp(-cfg.drag*duration))/cfg.drag : duration;
                float intercept=cfg.drag ? -log(1-cfg.drag*fraction*total_travel)/cfg.drag : fraction*duration;
                liquid_integrate_flight(&p,intercept);
                CHECK(fabsf(p.position[2]+depth)<0.00001f);
                float straight_y=origin[1]+fraction*(target[1]-origin[1]);
                heights[version]=p.position[1]-straight_y;
            }
            CHECK(heights[1]>0 && heights[1]<heights[0]*0.28f);
            worst_old=fmaxf(worst_old,heights[0]); worst_new=fmaxf(worst_new,heights[1]);
        }
        liquid_particle_t arrived={0};
        memcpy(arrived.position,origin,sizeof(origin)); memcpy(arrived.velocity,new_velocity,sizeof(new_velocity));
        liquid_integrate_flight(&arrived,new_time);
        for(int axis=0;axis<3;++axis) CHECK(fabsf(arrived.position[axis]-target[axis])<0.00001f);
    }
    printf("PASS: closer surfaces at 0.75/1.25/1.75 units have >72%% less arc lift; worst lift %.4f -> %.4f; fixed target unchanged\n",worst_old,worst_new);
}

static void __attribute__((naked)) run_tracking(void *app, void *site, int *path, float *out, int native_count)
{
    __asm__ __volatile__(
        "pushl %ebx\n\tmovl 8(%esp), %ebx\n\tmovl 16(%esp), %ecx\n\t"
        "movl 20(%esp), %edx\n\tmovl 24(%esp), %eax\n\tfld1\n\t"
        "call *12(%esp)\n\tpopl %ebx\n\tret\n\t");
}

static void test_lifetime(void)
{
    BYTE app[0x4c8]={0}, update[0x20]={0}, descriptor[0x40]={0};
    struct { int count; void *items[1]; } array={1,{descriptor}};
    setup();
    CHECK(fabsf(liquid_tool_pulse_duration()-14)<0.00001f);
    CHECK(tool_emitter.end_tick-tool_emitter.start_tick==14000);
    CHECK(liquid_pov_busy(tool_emitter.start_tick+11000));
    *(void**)(app+0x468)=update; *(int*)(app+0x4c0)=1;
    *(void**)(update+0x14)=array.items;
    BYTE *camera_memory=_aligned_malloc(128,16), meta[512]={0}, dispatch[64]={0};
    CHECK(camera_memory); memset(camera_memory,0,128);
    void *camera=camera_memory+64;
    *(void**)((BYTE*)camera-0x18)=meta;
    *(void**)(meta+0x154)=dispatch+32; *(int*)(dispatch+4)=1;
    *(void**)descriptor=camera;
    liquid_bind_tool_descriptor(update,app+0x378);
    CHECK(tool_emitter.native_stain_descriptor==descriptor && liquid_tool_native_app==app);
    CHECK(liquid_pov_keep_tracking(app));
    *(int*)(descriptor+0x10)=0; /* Native first pulse has ended. */
    CHECK(liquid_pov_keep_tracking(app));
    BYTE *site=VirtualAlloc(NULL,64,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    const BYTE normal[]={0x8b,0x8b,0xc0,0x04,0,0, 0xc7,0x00,2,0,0,0, 0xd9,0x1a,0xc3};
    const BYTE held[]={0xc7,0x00,1,0,0,0, 0xd9,0x1a,0xc3};
    CHECK(site); memcpy(site,normal,sizeof(normal)); memcpy(site+32,held,sizeof(held));
    liquid_pov_cleanup_resume=site+32;
    CHECK(install_inline_hook(site,(void*)hook_PovCleanup,6,&tramp_PovCleanup));
    int path=0; float fp=0;
    run_cleanup(app,site,&path,&fp);
    CHECK(path==1 && fp==1); /* real assembly bridge preserves x87 and registers */
    BYTE *tracking=VirtualAlloc(NULL,128,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    const BYTE gate[]={0x85,0xc0,0x0f,0x8e,0xfe,1,0,0};
    const BYTE active[]={0xc7,0x01,1,0,0,0,0xd9,0x1a,0xc3};
    const BYTE idle[]={0xc7,0x01,2,0,0,0,0xd9,0x1a,0xc3};
    CHECK(tracking); memcpy(tracking,gate,sizeof(gate));
    memcpy(tracking+32,active,sizeof(active)); memcpy(tracking+64,idle,sizeof(idle));
    liquid_pov_tracking_active=tracking+32; liquid_pov_tracking_idle=tracking+64;
    void *unused=NULL;
    CHECK(install_inline_hook(tracking,(void*)hook_PovTracking,8,&unused));
    run_tracking(app,tracking,&path,&fp,0); CHECK(path==1 && fp==1);
    cfg.pov_enabled=0;
    run_tracking(app,tracking,&path,&fp,0); CHECK(path==2 && fp==1);
    run_tracking(app,tracking,&path,&fp,2); CHECK(path==1 && fp==1);
    cfg.pov_enabled=1;
    CHECK(liquid_pov_hold_cleanup(app));
    cfg.pov_enabled=0; CHECK(!liquid_pov_hold_cleanup(app)); cfg.pov_enabled=1;
    tool_emitter.end_tick=GetTickCount()-1;
    liquid_particles[0].active=1; liquid_particles[0].source_kind=2;
    liquid_particles[0].emission_id=tool_emitter.emission_id;
    CHECK(liquid_pov_hold_cleanup(app)); /* final pulse still in flight */
    liquid_particles[0].collided=1;
    *(int*)(descriptor+0x10)=5; descriptor[0x2c]=1; *(int*)(descriptor+0x30)=5;
    CHECK(!liquid_pov_hold_cleanup(app));
    CHECK(!liquid_pov_keep_tracking(app));
    run_tracking(app,tracking,&path,&fp,0); CHECK(path==2 && fp==1);
    run_cleanup(app,site,&path,&fp); CHECK(path==2 && fp==1);
    CHECK(!*(int*)(descriptor+0x10) && !descriptor[0x2c] && !*(int*)(descriptor+0x30));
    CHECK(!liquid_pov_hold_cleanup(update)); /* another tool/app never held */
    VirtualFree(site,0,MEM_RELEASE);
    VirtualFree(tracking,0,MEM_RELEASE);
    _aligned_free(camera_memory);
    puts("PASS: real tracking/cleanup bridges preserve x87/registers, track beyond first native pulse, release after flight, preserve disabled/native paths");
}

static void test_contacts(void)
{
    BYTE descriptor[0x40]={0}; void *results=NULL;
    float frame[128]={0};
    setup(); tool_emitter.native_stain_descriptor=descriptor;
    CHECK(liquid_pick_tool_contact(NULL,NULL,pick_camera,&results,1,frame+100,descriptor,GetTickCount())<0);
    CHECK(picks==0); /* hovering over body cannot create a decal */
    liquid_particle_t *p=&liquid_particles[0];
    p->active=p->collided=1; p->source_kind=2; p->emission_id=tool_emitter.emission_id;
    p->spawn_order=99; p->position[2]=-2; p->impact_velocity[2]=-1; p->lifetime=3;
    liquid_queue_native_model_contact(p);
    CHECK(liquid_emitter_for_native_update(NULL,GetTickCount())==NULL);
    BYTE update[0x20]={0};
    struct {int count; void *items[1];} descriptors={1,{descriptor}};
    *(void**)(update+0x14)=descriptors.items;
    CHECK(liquid_emitter_for_native_update(update,GetTickCount())==&tool_emitter);
    CHECK(liquid_pick_tool_contact(NULL,NULL,pick_camera,&results,1,frame+100,descriptor,GetTickCount())==0);
    CHECK(picks==1 && p->model_contact_verified);
    CHECK(liquid_pick_tool_contact(NULL,NULL,pick_camera,&results,1,frame+100,descriptor,GetTickCount())<0);
    CHECK(picks==1);
    /* Reject an unbounded ray hitting a body behind the actual contact. */
    p->model_contact_verified=p->model_contact_confirmed=0;
    liquid_native_last_contact_queue_tick[LIQUID_MODEL_EMITTER_COUNT]=0;
    liquid_queue_native_model_contact(p); hit_z=-3;
    CHECK(liquid_pick_tool_contact(NULL,NULL,pick_camera,&results,1,frame+100,descriptor,GetTickCount())<0);
    CHECK(!p->model_contact_verified);
    cfg.collision_spawn_model_stains=0;
    int before=picks;
    CHECK(liquid_pick_tool_contact(NULL,NULL,pick_camera,&results,1,frame+100,descriptor,GetTickCount())<0);
    CHECK(picks==before);
    tramp_PovPickView=view; cfg.pov_enabled=0;
    CHECK(hook_PovPickView(NULL,NULL,NULL,NULL,NULL,1)==7 && native_views==1);
    puts("PASS: no cursor-only decals; actual POV contact consumed once; wrong surface rejected; decals disabled/native POV preserved");
}

int main(void) { test_launch(); test_aim_alignment(); test_fixed_reach(); test_close_surface_arc(); test_lifetime(); test_contacts(); return 0; }
