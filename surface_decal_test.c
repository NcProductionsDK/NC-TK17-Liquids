/* Production geometry and executable mid-function hook regression. */
#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL %d: %s\n", __LINE__, #c); exit(1); } } while (0)
static void check_close(float a, float b) { CHECK(fabsf(a - b) < 0.00002f); }
static void transform_vector(const float *m, const float *v, float *out)
{
    int i;
    for (i = 0; i < 3; i++) out[i] = v[0]*m[i] + v[1]*m[4+i] + v[2]*m[8+i];
}
static void check_basis(const float *m, const float *normal)
{
    float projected[3], x[3] = {m[0],m[4],m[8]}, y[3] = {m[1],m[5],m[9]};
    check_close(liquid_vec3_dot(x,x), 1); check_close(liquid_vec3_dot(y,y), 1);
    check_close(liquid_vec3_dot(x,y), 0);
    transform_vector(m, normal, projected);
    check_close(projected[0], 0); check_close(projected[1], 0); check_close(projected[2], 1);
    check_close(m[12], 0); check_close(m[13], 0); check_close(m[14], 0); check_close(m[15], 1);
}

/* Run the actual hook/trampoline with a synthetic native EBP and a live
   x87 value. The replayed FLD must load hit.x and preserve the prior value. */
static void __attribute__((naked)) run_site(void *frame, void *site, float *out)
{
    __asm__ __volatile__(
        "pushl %ebp\n\tmovl 8(%esp), %ebp\n\tmovl 16(%esp), %eax\n\t"
        "fld1\n\tcall *12(%esp)\n\tpopl %ebp\n\tret\n\t");
}

static void read_native_instruction(BYTE bytes[6])
{
    FILE *file = fopen("The Klub 17/Binaries/TK17-158.001.exe", "rb");
    IMAGE_DOS_HEADER dos; IMAGE_NT_HEADERS nt; IMAGE_SECTION_HEADER section;
    int i, found = 0;
    CHECK(file && fread(&dos,sizeof(dos),1,file) == 1);
    CHECK(fseek(file,dos.e_lfanew,SEEK_SET) == 0);
    CHECK(fread(&nt,sizeof(nt),1,file) == 1);
    CHECK(nt.FileHeader.TimeDateStamp == TK17_EXE_TIMESTAMP);
    for (i=0; i<nt.FileHeader.NumberOfSections; i++) {
        CHECK(fread(&section,sizeof(section),1,file) == 1);
        if (LIQUID_NATIVE_STAIN_PROJECTOR_RVA >= section.VirtualAddress &&
            LIQUID_NATIVE_STAIN_PROJECTOR_RVA + 6 <= section.VirtualAddress + section.SizeOfRawData) {
            CHECK(fseek(file, section.PointerToRawData + LIQUID_NATIVE_STAIN_PROJECTOR_RVA - section.VirtualAddress, SEEK_SET) == 0);
            CHECK(fread(bytes,6,1,file) == 1); found = 1; break;
        }
    }
    fclose(file); CHECK(found);
    CHECK(!memcmp(bytes, "\xd9\x85\x7c\xff\xff\xff", 6));
}

int main(void)
{
    BYTE pick[0x58] = {0}, original_pick[0x58];
    BYTE frame_storage[0x200] = {0};
    void *frame = frame_storage + sizeof(frame_storage);
    float *matrix = (float*)((BYTE*)frame - LIQUID_NATIVE_STAIN_MATRIX_EBP_OFFSET);
    float inverse[16], normal[3] = {0.8f,0,0.6f}, point[3] = {0.25f,0.4f,-1.2f};
    float *hit = (float*)((BYTE*)frame - 0x84);
    float outputs[2];
    liquid_native_freeze_capture_t capture = {0};
    BYTE *site = VirtualAlloc(NULL,64,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
    CHECK(site);
    memcpy(pick+0x14,point,sizeof(point)); memcpy(pick+0x20,normal,sizeof(normal));
    memcpy(original_pick,pick,sizeof(pick));
    captured_camera_inverse_valid = 1;
    captured_camera_inverse[0] = captured_camera_inverse[5] =
        captured_camera_inverse[10] = captured_camera_inverse[15] = 1;
    CHECK(liquid_native_surface_projector(pick,inverse)); check_basis(inverse,normal);
    CHECK(!memcmp(pick,original_pick,sizeof(pick)));
    /* A planar tangent is foreshortened to 60% by the old front-facing
       projector (1.67x stretching when clipped back onto that plane).
       The surface projector maps both tangents at equal unit scale. */
    {
        float tangent[3] = {0.6f,0,-0.8f}, out[3];
        transform_vector(inverse,tangent,out);
        check_close(sqrtf(out[0]*out[0]+out[1]*out[1]),1); check_close(out[2],0);
        check_close(out[0],-1); check_close(out[1],0); /* Horizontal stays unchanged. */
        check_close(fabsf(tangent[0]),0.6f);
    }
    /* Vertical-only texture flip: world up now maps to positive projector Y,
       with symmetric UVs around the exact hit. 0.8.10 mapped up to negative Y. */
    {
        float above[3] = {0,0.25f,0}, below[3] = {0,-0.25f,0}, out[3];
        transform_vector(inverse,above,out);
        check_close(out[0],0); check_close(out[1]+0.5f,0.75f); check_close(out[2],0);
        transform_vector(inverse,below,out);
        check_close(out[0],0); check_close(out[1]+0.5f,0.25f); check_close(out[2],0);
    }
    /* The projection no longer takes incoming-ray direction as an input. */
    liquid_native_freeze_capture = &capture;
    native_stain_projector_hook_installed = 1;
    liquid_native_stage_surface_projector(frame,pick);
    CHECK(capture.projector_frame == frame);
    /* Reproduce the engine's post-pick overwrite; the old early write loses. */
    memset(matrix,0,sizeof(inverse)); matrix[0]=matrix[5]=matrix[10]=matrix[15]=1;
    CHECK(memcmp(matrix,inverse,sizeof(inverse)) != 0);
    read_native_instruction(site);
    /* After the real stolen FLD, store its value and the preserved x87 one. */
    memcpy(site+6,"\xd9\x18\xd9\x58\x04\xc3",6);
    CHECK(install_inline_hook(site,hook_NativeStainProjector,6,&tramp_NativeStainProjector));
    memcpy(hit,point,sizeof(point));
    run_site(frame,site,outputs);
    check_close(outputs[0],point[0]); check_close(outputs[1],1);
    CHECK(!memcmp(matrix,inverse,sizeof(inverse)) && !capture.projector_frame);
    CHECK(!memcmp(hit,point,sizeof(point))); CHECK(!memcmp(pick,original_pick,sizeof(pick)));
    /* Ordinary native/POV placement must retain its own matrix. */
    matrix[0]=7; run_site(frame,site,outputs); check_close(matrix[0],7);
    liquid_native_stage_surface_projector(frame,pick);
    liquid_native_apply_surface_projector(frame_storage+0x180); check_close(matrix[0],7);
    CHECK(capture.projector_frame == frame);
    liquid_native_freeze_capture = NULL;
    run_site(frame,site,outputs); check_close(matrix[0],7);
    /* Horizontal normal: stable roll fallback, no dependency on ray axes. */
    normal[0]=normal[2]=0; normal[1]=1; memcpy(pick+0x20,normal,sizeof(normal));
    CHECK(liquid_native_surface_projector(pick,inverse)); check_basis(inverse,normal);
    /* Camera rolled 90 degrees around world Z; up and fallback follow it. */
    captured_camera_inverse[0]=captured_camera_inverse[5]=0;
    captured_camera_inverse[1]=1; captured_camera_inverse[4]=-1;
    normal[0]=1; normal[1]=0; memcpy(pick+0x20,normal,sizeof(normal));
    CHECK(liquid_native_surface_projector(pick,inverse)); check_basis(inverse,normal);
    /* The vertical flip follows world-up even with a rolled camera. */
    normal[0]=0; normal[1]=-0.8f; normal[2]=0.6f;
    memcpy(pick+0x20,normal,sizeof(normal));
    CHECK(liquid_native_surface_projector(pick,inverse)); check_basis(inverse,normal);
    {
        float world_up_view[3] = {1,0,0}, out[3];
        transform_vector(inverse,world_up_view,out);
        check_close(out[0],0); check_close(out[1],1); check_close(out[2],0);
    }
    memset(pick+0x20,0,sizeof(normal)); CHECK(!liquid_native_surface_projector(pick,inverse));
    *(float*)(pick+0x20)=NAN; CHECK(!liquid_native_surface_projector(pick,inverse));
    *(float*)(pick+0x20)=10; CHECK(!liquid_native_surface_projector(pick,inverse));
    puts("PASS: vertical-only flip and unchanged horizontal direction; native mesh normal; equal tangent scale; native overwrite regression; real instruction hook/trampoline and x87 preservation; unchanged hit; POV isolation; camera roll; invalid normals");
    return 0;
}
