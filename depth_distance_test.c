/* The world-space impact stays fixed; only the camera moves. */
#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while (0)
static liquid_depth_snapshot_t snapshot;
static float pixels[64*64];
static D3D11_MAPPED_SUBRESOURCE mapped;

static float projected_depth(float z)
{
    float point[3]={0,0,z}, x,y,depth;
    CHECK(liquid_project_world_depth_snapshot(&snapshot,point,&x,&y,&depth));
    /* Match a D24 surface readback rather than ideal double precision. */
    return roundf(depth*16777215.0f)/16777215.0f;
}

static void setup(float distance, float tolerance, int reversed, int source)
{
    const float near_plane=0.03f, far_plane=100.0f;
    memset(&cfg,0,sizeof(cfg)); memset(&snapshot,0,sizeof(snapshot));
    memset(liquid_particles,0,sizeof(liquid_particles));
    memset(liquid_native_contacts,0,sizeof(liquid_native_contacts));
    cfg.liquids_enabled=cfg.collision_enabled=1;
    cfg.particle_limit=1; cfg.particle_size=0.005f;
    cfg.collision_surface_hold=1; cfg.collision_depth_tolerance=tolerance;
    liquid_physx_query_body_collider=NULL;
    snapshot.valid=1; snapshot.scene_width=snapshot.scene_height=64;
    snapshot.width=snapshot.height=64;
    snapshot.viewport=(D3DVIEWPORT8){0,0,64,64,0,1};
    snapshot.camera_inverse[0]=snapshot.camera_inverse[5]=
        snapshot.camera_inverse[10]=snapshot.camera_inverse[15]=1;
    snapshot.camera_inverse[14]=distance-1;
    snapshot.projection._11=snapshot.projection._22=1.5f;
    snapshot.projection._34=-1;
    snapshot.projection._33=reversed ? near_plane/(far_plane-near_plane) : far_plane/(near_plane-far_plane);
    snapshot.projection._43=reversed ? near_plane*far_plane/(far_plane-near_plane) : near_plane*far_plane/(near_plane-far_plane);
    liquid_d3d11_depth_test_func=reversed ? D3D11_COMPARISON_GREATER_EQUAL : D3D11_COMPARISON_LESS_EQUAL;
    float depth=projected_depth(-1);
    for(int i=0;i<64*64;i++) pixels[i]=depth;
    mapped=(D3D11_MAPPED_SUBRESOURCE){pixels,64*sizeof(float),sizeof(pixels)};
    liquid_particle_t *p=&liquid_particles[0];
    p->active=1; p->source_kind=source; p->emission_id=p->spawn_order=1;
    p->age=0.2f; p->lifetime=2; p->size_scale=1;
    p->previous[2]=-0.98f; p->position[2]=-1.02f; p->velocity[2]=-2;
    liquid_depth_particle_sample_t *s=&snapshot.particles[0];
    s->valid=1; s->emission_id=s->spawn_order=1; s->age=p->age;
    memcpy(s->previous,p->previous,sizeof(s->previous));
    memcpy(s->position,p->position,sizeof(s->position));
    memcpy(s->velocity,p->velocity,sizeof(s->velocity));
}

int main(void)
{
    const float distances[]={1,2,5,10,25,50};
    const float tolerances[]={0.00015f,0.0015f,0.05f};
    int failed=0;
    for(int r=0;r<2;r++) for(int t=0;t<3;t++) for(int k=1;k<=2;k++) {
        for(int i=0;i<6;i++) {
            setup(distances[i],tolerances[t],r,k);
            liquid_apply_depth_collisions(&snapshot,&mapped);
            if(!liquid_particles[0].collided || fabsf(liquid_particles[0].position[2]+1)>0.012f) {
                printf("MISS distance=%.1f tolerance=%.5f reversed=%d source=%d z=%.5f\n",
                    distances[i],tolerances[t],r,k,liquid_particles[0].position[2]);
                failed++;
            }
        }
    }
    for(int r=0;r<2;r++) for(int i=0;i<6;i++) {
        setup(distances[i],0.0015f,r,1);
        snapshot.particles[0].previous[2]=-0.96f;
        snapshot.particles[0].position[2]=-0.98f;
        liquid_apply_depth_collisions(&snapshot,&mapped);
        CHECK(!liquid_particles[0].collided);
        setup(distances[i],0.0015f,r,1);
        /* A foreground silhouette crosses the screen path, but its world
           surface is 0.8 units away from the particle. It must not stick. */
        float foreground=projected_depth(-0.2f);
        for(int y=0;y<64;y++) for(int x=0;x<64;x++) pixels[y*64+x]=x<32 ? (r?0:1) : foreground;
        snapshot.particles[0].previous[0]=-0.04f;
        snapshot.particles[0].position[0]=0.04f;
        snapshot.particles[0].previous[2]=snapshot.particles[0].position[2]=-1;
        liquid_apply_depth_collisions(&snapshot,&mapped);
        CHECK(!liquid_particles[0].collided);
        setup(distances[i],0.0015f,r,1);
        for(int j=0;j<64*64;j++) pixels[j]=r?0:1;
        liquid_apply_depth_collisions(&snapshot,&mapped);
        CHECK(!liquid_particles[0].collided);
    }
    CHECK(!failed);
    puts("PASS: same impact at camera distances 1/2/5/10/25/50, both sources, normal/reversed D24, user/default/max tolerance; pre-impact, foreground silhouettes and clear sky rejected");
    return 0;
}
