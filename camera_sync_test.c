#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while (0)
static float mock_view[16];
static int getter_ok = 1;
static int __cdecl get_view(unsigned int abi, float *out)
{
    CHECK(abi == 1);
    if (!getter_ok) return 0;
    memcpy(out,mock_view,sizeof(mock_view));
    return 1;
}
static void camera(float pitch, float x, float inverse[16])
{
    float a = pitch * 3.14159265f / 180;
    memset(inverse,0,16*sizeof(float));
    inverse[0]=inverse[15]=1;
    inverse[5]=inverse[10]=cosf(a);
    inverse[6]=-sinf(a); inverse[9]=sinf(a);
    inverse[12]=x; inverse[13]=2*sinf(a); inverse[14]=2*cosf(a);
}
static void make_view(const float inverse[16])
{
    memset(mock_view,0,sizeof(mock_view)); mock_view[15]=1;
    for(int i=0;i<3;i++) for(int j=0;j<3;j++) mock_view[i*4+j]=inverse[j*4+i];
    for(int j=0;j<3;j++) for(int k=0;k<3;k++) mock_view[12+j]-=inverse[12+k]*mock_view[k*4+j];
}
int main(void)
{
    const float anchor[3]={0,0,0}, normal[3]={0,1,0};
    ID3D11DepthStencilView *depth=(void*)0x1234;
    int cases=0, old_below_floor=0;
    hook5_get_view_matrix=get_view;
    captured_camera_inverse_valid=1;
    for(int angle=15;angle<=85;angle+=5) for(int reverse=0;reverse<2;reverse++)
    for(int move=-1;move<=1;move+=2) {
        liquid_depth_snapshot_t scene={0};
        float saved_native[16], sx,sy,vz,dz;
        scene.valid=1; scene.scene_width=800; scene.scene_height=600;
        scene.viewport=(D3DVIEWPORT8){0,0,800,600,0,1};
        scene.projection._11=1.5f; scene.projection._22=2;
        scene.projection._33=reverse ? 0.001001f : -1.001001f;
        scene.projection._34=-1;
        scene.projection._43=reverse ? 0.1001001f : -0.1001001f;
        camera((float)angle,0.15f,scene.camera_inverse);
        make_view(scene.camera_inverse);
        camera(angle+move*0.8f,0.15f+move*0.01f,captured_camera_inverse);
        memcpy(saved_native,captured_camera_inverse,sizeof(saved_native));
        liquid_capture_hook5_scene_camera(depth);
        CHECK(liquid_hook5_camera_for_depth(depth));
        CHECK(!liquid_hook5_camera_for_depth((void*)0x5678));
        for(int i=0;i<16;i++) CHECK(fabsf(liquid_hook5_scene_camera_inverse[i]-scene.camera_inverse[i])<0.00001f);
        /* Render the actual footprint once with the old independent camera,
           then with the depth frame camera. Reconstruct against scene depth. */
        for(int fixed=0;fixed<=1;fixed++) {
            liquid_d3d11_vertex_t vertices[6]; UINT count=0;
            liquid_scene_camera_override=fixed ? liquid_hook5_camera_for_depth(depth) : NULL;
            CHECK(liquid_project_world_d3d11(anchor,800,600,4.0f/3,0.5f,1,
                &scene.projection,&scene.viewport,&sx,&sy,&vz,&dz,NULL));
            CHECK(liquid_append_capsule(vertices,&count,sx-8,sy,dz,8,sx+8,sy,dz,8,1,800,600,0));
            CHECK(liquid_contact_surface_depth(vertices,count,anchor,normal,800,600,&scene.projection,&scene.viewport));
            for(UINT i=0;i<count;i++) {
                float world[3];
                CHECK(liquid_unproject_depth_snapshot(&scene,(vertices[i].position[0]+1)*400,
                    (1-vertices[i].position[1])*300,vertices[i].position[2],world));
                if(fixed) CHECK(fabsf(world[1]-0.0005f)<0.00004f);
                else if(world[1]<-0.0001f) old_below_floor++;
            }
        }
        liquid_scene_camera_override=NULL;
        CHECK(!memcmp(saved_native,captured_camera_inverse,sizeof(saved_native)));
        cases++;
    }
    CHECK(old_below_floor>0);
    getter_ok=0; liquid_capture_hook5_scene_camera(depth);
    CHECK(!liquid_hook5_camera_for_depth(depth));
    getter_ok=1; mock_view[0]=NAN; liquid_capture_hook5_scene_camera(depth);
    CHECK(!liquid_hook5_camera_for_depth(depth));
    camera(45,0,captured_camera_inverse); make_view(captured_camera_inverse);
    mock_view[0]=2; liquid_capture_hook5_scene_camera(depth);
    CHECK(!liquid_hook5_camera_for_depth(depth));
    make_view(captured_camera_inverse);
    liquid_capture_hook5_scene_camera(depth); CHECK(liquid_hook5_scene_camera_valid);
    hook5_d3d11_composite_registered=1;
    liquid_hook5_present_callback(NULL);
    CHECK(!liquid_hook5_camera_for_depth(depth));
    hook5_get_view_matrix=NULL; liquid_capture_hook5_scene_camera(depth);
    CHECK(!liquid_hook5_scene_camera_valid);
    printf("PASS: %d moving-camera cases, normal/reverse depth; old path buries %d vertices; synchronized path stays on floor; native aim camera unchanged; invalid/absent/stale/mismatched depth fallbacks\n",cases,old_below_floor);
    return 0;
}
