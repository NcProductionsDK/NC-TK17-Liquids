#ifdef BASELINE
#include "baseline/NC-TK17-Liquids.c"
#else
#include "NC-TK17-Liquids.c"
#endif
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
int main(void)
{
    const float anchor[3]={0,0,0}, normal[3]={0,1,0};
    D3DVIEWPORT8 viewport={0,0,800,600,0,1};
    int underground=0;
    for(int angle=15;angle<=85;angle+=5) for(int reversed=0;reversed<2;reversed++) {
        float a=angle*3.14159265f/180, sx,sy,z,depth;
        liquid_depth_snapshot_t snap={0}; liquid_d3d11_vertex_t vertices[6]; UINT count=0;
        snap.valid=1; snap.scene_width=800; snap.scene_height=600; snap.viewport=viewport;
        snap.projection._11=1.5f; snap.projection._22=2;
        snap.projection._33=reversed ? 0.001001f : -1.001001f;
        snap.projection._34=-1; snap.projection._43=reversed ? 0.1001001f : -0.1001001f;
        memset(captured_camera_inverse,0,sizeof(captured_camera_inverse));
        captured_camera_inverse[0]=captured_camera_inverse[15]=1;
        captured_camera_inverse[5]=captured_camera_inverse[10]=cosf(a);
        captured_camera_inverse[6]=-sinf(a); captured_camera_inverse[9]=sinf(a);
        captured_camera_inverse[13]=2*sinf(a); captured_camera_inverse[14]=2*cosf(a);
        captured_camera_inverse_valid=1;
        memcpy(snap.camera_inverse,captured_camera_inverse,sizeof(snap.camera_inverse));
        CHECK(liquid_project_world_d3d11(anchor,800,600,4.0f/3,0.5f,1,&snap.projection,&viewport,&sx,&sy,&z,&depth,NULL));
        CHECK(liquid_append_capsule(vertices,&count,sx-8,sy,depth,8,sx+8,sy,depth,8,1,800,600,0));
#ifndef BASELINE
        CHECK(liquid_contact_surface_depth(vertices,count,anchor,normal,800,600,&snap.projection,&viewport));
#endif
        for(UINT i=0;i<count;i++) {
            float world[3];
            CHECK(liquid_unproject_depth_snapshot(&snap,(vertices[i].position[0]+1)*400,
                (1-vertices[i].position[1])*300,vertices[i].position[2],world));
            if(world[1]<-0.0001f) underground++;
#ifndef BASELINE
            CHECK(fabsf(world[1]-0.0005f)<0.00003f);
#endif
        }
    }
    printf("Vertices below floor while tilting: %d\n",underground);
    CHECK(!underground);
#ifndef BASELINE
    /* A body rotation must rotate the surface plane with its attachment. */
    for(int segment=0;segment<2;segment++) {
        liquid_contact_person_t person={0}; liquid_particle_t p={0};
        person.valid=person.anchors[0].valid=person.anchors[1].valid=1;
        person.horizontal[0]=person.vertical[1]=person.depth[2]=1;
        person.anchors[1].world[1]=1;
        p.contact_anchor=1; p.contact_anchor_end=segment ? 2 : 0;
        p.contact_normal_valid=1; p.contact_normal[2]=1;
        liquid_contact_attached_normal(&person,&p,1);
        person.horizontal[0]=person.depth[2]=0;
        person.horizontal[2]=-1; person.depth[0]=1;
        liquid_contact_attached_normal(&person,&p,0);
        CHECK(p.contact_normal_valid && fabsf(p.contact_normal[0]-1)<0.00001f && fabsf(p.contact_normal[2])<0.00001f);
    }
#endif
    puts("PASS: landed footprint remains on surface through 15-85 degree camera tilt, normal/reverse depth, without changing its screen footprint");
    return 0;
}
