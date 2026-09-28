/* Native decal extraction accepts a triangle only if a vertex is inside its
   selection sphere (EXE+1D3F00, +1D40C6, +1D4360). Large room polygons may
   cross that sphere with all vertices outside. Correct only those three first
   distance calls, only for the verified room target in this native update.
   The remaining native mesh extraction, UV projection and size stay intact. */
#include <float.h>
typedef float (THISCALL *liquid_room_distance_t)(const float *, const float *);
static liquid_room_distance_t liquid_room_original_distance;
static const void *liquid_room_distance_returns[3];
static int liquid_room_triangle_hooks_installed;

static void liquid_room_reset_triangle_validation(liquid_native_freeze_capture_t *capture)
{
    capture->room_checked_frame = capture->room_checked_sphere = capture->room_checked_positions = NULL;
    capture->room_checked_position_count = 0;
}

/* A conservative box rejection avoids a full closest-point calculation for
   triangles nowhere near the impact. Expand slightly to preserve float
   boundary decisions in the original exact distance test. */
static int liquid_room_triangle_distant(const float *sphere, const float *a,
                                        const float *b, const float *c)
{
    double radius = (double)sphere[3] * 1.000001 + 1e-7;
    for (int i=0;i<3;i++) {
        double lo=a[i], hi=a[i], center=sphere[i];
        if (b[i]<lo) lo=b[i];
        if (c[i]<lo) lo=c[i];
        if (b[i]>hi) hi=b[i];
        if (c[i]>hi) hi=c[i];
        if (center < lo-radius || center > hi+radius) return 1;
    }
    return 0;
}

static double liquid_room_segment_distance2(const double p[3], const double a[3], const double b[3])
{
    double d[3], q[3], length2=0, t=0, result=0;
    for (int i=0;i<3;i++) { d[i]=b[i]-a[i]; q[i]=p[i]-a[i]; length2+=d[i]*d[i]; t+=q[i]*d[i]; }
    t=length2>0 ? t/length2 : 0;
    if (t<0) t=0; else if (t>1) t=1;
    for (int i=0;i<3;i++) { double x=q[i]-t*d[i]; result+=x*x; }
    return result;
}

static float liquid_room_triangle_distance2(const float *p, const float *a, const float *b, const float *c)
{
    double dp[3], da[3], db[3], dc[3], ab[3], ac[3], ap[3], n[3];
    double aa=0, bb=0, cc=0, d=0, e=0, n2, det, u, v, distance;
    for (int i=0;i<3;i++) {
        if (!_finite(p[i]) || !_finite(a[i]) || !_finite(b[i]) || !_finite(c[i])) return FLT_MAX;
        dp[i]=p[i]; da[i]=a[i]; db[i]=b[i]; dc[i]=c[i];
        ab[i]=db[i]-da[i]; ac[i]=dc[i]-da[i]; ap[i]=dp[i]-da[i];
        aa+=ab[i]*ab[i]; bb+=ab[i]*ac[i]; cc+=ac[i]*ac[i];
        d+=ap[i]*ab[i]; e+=ap[i]*ac[i];
    }
    n[0]=ab[1]*ac[2]-ab[2]*ac[1]; n[1]=ab[2]*ac[0]-ab[0]*ac[2]; n[2]=ab[0]*ac[1]-ab[1]*ac[0];
    n2=n[0]*n[0]+n[1]*n[1]+n[2]*n[2]; det=aa*cc-bb*bb;
    if (det>1e-12*aa*cc && n2>0) {
        u=(d*cc-e*bb)/det; v=(e*aa-d*bb)/det;
        if (u>=0 && v>=0 && u+v<=1) {
            distance=ap[0]*n[0]+ap[1]*n[1]+ap[2]*n[2];
            return (float)(distance*distance/n2);
        }
    }
    distance=liquid_room_segment_distance2(dp,da,db);
    double other=liquid_room_segment_distance2(dp,db,dc); if (other<distance) distance=other;
    other=liquid_room_segment_distance2(dp,dc,da); if (other<distance) distance=other;
    return (float)distance;
}

static float __attribute__((used, noinline)) __cdecl liquid_room_triangle_distance_call(
    const float *vertex, const float *sphere, const BYTE *frame, unsigned int ebx, const void *caller)
{
    liquid_native_freeze_capture_t *capture=liquid_native_freeze_capture;
    const float *positions;
    unsigned int indices[3];
    int mode=-1, count;
    float original=liquid_room_original_distance(vertex,sphere);
    if (!cfg.liquids_enabled || !cfg.collision_spawn_room_stains || !capture ||
        !capture->room_projection_target || !capture->room_projection_frame) return original;
    for (int i=0;i<3;i++) if (caller==liquid_room_distance_returns[i]) mode=i;
    if (mode<0 || !frame) return original;
    if (capture->room_checked_frame != frame) {
        if (!ptr_readable(frame-0x60,0x70)) return original;
        capture->room_checked_frame = frame;
    }
    if (*(void*const*)frame != capture->room_projection_frame ||
        *(void*const*)(frame-0x54) != capture->room_projection_target ||
        !sphere) return original;
    if (capture->room_checked_sphere != sphere) {
        if (!ptr_readable(sphere,16)) return original;
        capture->room_checked_sphere = sphere;
    }
    if (!_finite(sphere[3]) || sphere[3]<=0) return original;
    if (original<=sphere[3]*sphere[3]) return original;
    positions=*(const float*const*)(frame-0x60);
    if (!positions) return original;
    if (capture->room_checked_positions != positions &&
        !ptr_readable((const BYTE*)positions-4,4)) return original;
    count=*((const int*)positions-1);
    if (count<3 || count>2000000) return original;
    if (capture->room_checked_positions != positions || capture->room_checked_position_count != count) {
        if (!ptr_readable(positions, (size_t)count*3*sizeof(float))) return original;
        capture->room_checked_positions = positions;
        capture->room_checked_position_count = count;
    }
    if (mode==2) {
        indices[0]=*(const unsigned int*)(frame-0x14);
        indices[1]=*(const unsigned int*)(frame-0x0c);
        indices[2]=*(const unsigned int*)(frame-0x10);
    } else {
        indices[0]=*(const unsigned int*)(frame-0x10);
        indices[1]=*(const unsigned int*)(frame-(mode==0 ? 0x0c : 0x18));
        indices[2]=ebx;
    }
    for (int i=0;i<3;i++) if (indices[i]>=(unsigned int)count) return original;
    if (vertex!=positions+3*indices[0]) return original;
    const float *a=positions+3*indices[0], *b=positions+3*indices[1], *c=positions+3*indices[2];
    capture->room_triangles_tested++;
    if (liquid_room_triangle_distant(sphere,a,b,c)) {
        capture->room_triangles_distant++;
        return original;
    }
    float distance=liquid_room_triangle_distance2(sphere,a,b,c);
    if (distance<=sphere[3]*sphere[3]) {
        capture->room_triangles_recovered++;
        return distance;
    }
    return original;
}

/* Keep the native THISCALL argument/return ABI, including ret 4 and ST(0).
   EBX and EBP here belong to the original mesh extraction loop. */
static void __attribute__((naked)) liquid_room_triangle_distance_bridge(void)
{
    __asm__ __volatile__(
        "pushfl\n\tpushal\n\tmovl %esp, %ebx\n\t"
        "subl $528, %esp\n\tandl $-16, %esp\n\tfxsave (%esp)\n\tfninit\n\t"
        "subl $32, %esp\n\t"
        "movl 24(%ebx), %eax\n\tmovl %eax, (%esp)\n\t"
        "movl 40(%ebx), %eax\n\tmovl %eax, 4(%esp)\n\t"
        "movl 8(%ebx), %eax\n\tmovl %eax, 8(%esp)\n\t"
        "movl 16(%ebx), %eax\n\tmovl %eax, 12(%esp)\n\t"
        "movl 36(%ebx), %eax\n\tmovl %eax, 16(%esp)\n\t"
        "call _liquid_room_triangle_distance_call\n\t"
        "fstps 20(%esp)\n\tmovl 20(%esp), %eax\n\tmovl %eax, 28(%ebx)\n\t"
        "addl $32, %esp\n\tfxrstor (%esp)\n\tmovl %ebx, %esp\n\t"
        "popal\n\tpopfl\n\tpushl %eax\n\tflds (%esp)\n\taddl $4, %esp\n\tret $4\n\t");
}
static void (*liquid_room_distance_bridge_pointer)(void)=liquid_room_triangle_distance_bridge;

static int liquid_room_install_triangle_hooks(BYTE *exe)
{
    static const unsigned int sites[3]={0x1d3f00,0x1d40c6,0x1d4360};
    DWORD old, restored;
    uintptr_t iat=(uintptr_t)(exe+0x24a794), replacement=(uintptr_t)&liquid_room_distance_bridge_pointer;
    if (liquid_room_triangle_hooks_installed) return 1;
    if (!ptr_readable((void*)iat,sizeof(void*))) return 0;
    memcpy(&liquid_room_original_distance,(void*)iat,sizeof(void*));
    if (!ptr_executable((void*)liquid_room_original_distance)) return 0;
    for (int i=0;i<3;i++) {
        BYTE *site=exe+sites[i];
        if (!ptr_readable(site,6) || site[0]!=0xff || site[1]!=0x15 ||
            memcmp(site+2,&iat,4)) return 0;
    }
    BYTE *start=exe+sites[0]; size_t length=sites[2]+6-sites[0];
    if (!VirtualProtect(start,length,PAGE_EXECUTE_READWRITE,&old)) return 0;
    for (int i=0;i<3;i++) {
        liquid_room_distance_returns[i]=exe+sites[i]+6;
        memcpy(exe+sites[i]+2,&replacement,4);
    }
    FlushInstructionCache(GetCurrentProcess(),start,length);
    VirtualProtect(start,length,old,&restored);
    liquid_room_triangle_hooks_installed=1;
    log_line("liquid room triangle selection installed sites=3 mode=surface-distance");
    return 1;
}
