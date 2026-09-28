#ifdef BASELINE
#include "baseline/NC-TK17-Liquids.c"
#else
#include "NC-TK17-Liquids.c"
#endif
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static D3DMATRIX supplied;
static float live_fov;
static int __cdecl projection_get(unsigned int abi,float *out)
{ CHECK(abi==1); memcpy(out,&supplied,sizeof(supplied)); return 1; }
static int __cdecl fov_get(unsigned int abi,float *out)
{ CHECK(abi==1); *out=live_fov; return 1; }
int main(void)
{
    const float fovs[]={0.1f,0.25f,0.5f,1,2,4,8};
    D3DVIEWPORT8 viewport={0,0,3440,1440,0,1};
    D3DMATRIX native={0}; int misses=0;
    native._11=0.8f; native._22=1.911111f; native._33=-1.001f;
    native._34=-1; native._43=-0.1001f;
    cfg.liquids_enabled=cfg.pov_enabled=1; cfg.pov_reach=1;
    cfg.speed=2.06127f; cfg.drag=0.08f; cfg.gravity_strength=1; cfg.particle_lifetime=5;
    captured_camera_inverse_valid=1;
    hook5_get_projection_matrix=projection_get; hook5_get_fov_multiplier=fov_get;
    for(int f=0;f<7;f++) for(int changed=0;changed<2;changed++) for(int k=-1;k<=1;k++) {
        float direction[3], origin[3]={-0.02f,-0.04f,-0.23f}, world[3], target[3];
        float sx,sy,depth,sd;
        memset(captured_camera_inverse,0,sizeof(captured_camera_inverse));
        captured_camera_inverse[0]=captured_camera_inverse[15]=1;
        captured_camera_inverse[5]=captured_camera_inverse[10]=cosf(0.4f);
        captured_camera_inverse[6]=-sinf(0.4f); captured_camera_inverse[9]=sinf(0.4f);
        captured_camera_inverse[12]=3; captured_camera_inverse[13]=2;
        live_fov=fovs[f]; supplied=native;
        supplied._11/=live_fov; supplied._22/=live_fov;
        /* Final draw records its true projection, before the next early
           scene pass exposes a native/unadjusted matrix. */
        liquid_hook5_composite_callback(NULL,NULL,NULL,3440,1440);
        if(changed) live_fov=fovs[(f+1)%7];
        D3DMATRIX rendered=native;
        rendered._11/=live_fov; rendered._22/=live_fov;
        supplied=native;
        DWORD now=GetTickCount();
        liquid_tool_crosshair_tick=now;
        liquid_tool_crosshair[0]=k*0.65f; liquid_tool_crosshair[1]=0.2f;
        CHECK(liquid_tool_crosshair_direction(origin,direction,now));
        CHECK(liquid_view_to_world_point(origin,world));
        CHECK(liquid_view_to_world_point(liquid_tool_aim_target_view,target));
        liquid_particle_t particle={0}; memcpy(particle.position,world,sizeof(world));
        float time=liquid_tool_launch_velocity(world,target,0.8f,particle.velocity);
        CHECK(time>0);
        for(int i=0;i<120;i++) liquid_integrate_flight(&particle,time/120);
        if(!liquid_project_world_d3d11(particle.position,3440,1440,3440.0f/1440,0.414f,1,
            &rendered,&viewport,&sx,&sy,&depth,&sd,NULL) ||
            fabsf(sx-(1+k*0.65f)*1720)>0.1f || fabsf(sy-576)>0.1f) misses++;
    }
    printf("Crosshair misses across FOV/callback phase changes: %d\n",misses);
    CHECK(!misses);
    puts("PASS: final-scene targeting at FOV 0.1/0.25/0.5/1/2/4/8, left/centre/right, pitched camera, gravity/drag and immediate live changes; no double FOV scaling");
    return 0;
}
