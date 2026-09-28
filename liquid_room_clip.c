/* Clip newly extracted static room geometry to the primary projected UV tile.
   Room triangles may span metres; sampler clamp is not a geometry boundary,
   especially when Hook5 replaces the material/sampler. Never touch the source
   mesh, and preserve every supported float vertex stream by interpolation. */
#define LIQUID_ROOM_CLIP_STREAMS 8
#define LIQUID_ROOM_CLIP_VERTICES 32768
typedef void *( __cdecl *liquid_room_array_new_t)(int,int,const void*);
typedef void (THISCALL *liquid_room_array_get_t)(void*,unsigned int,void**);
typedef void (THISCALL *liquid_room_array_set_t)(void*,unsigned int,const void*);
typedef void (THISCALL *liquid_room_int_set_t)(void*,unsigned int,int);
static liquid_room_array_new_t liquid_room_array_new;
static void *liquid_room_empty_array;
static void *liquid_room_clip_trampoline __attribute__((used));

typedef struct liquid_room_clip_vertex_t { float uv[2], weight[3]; } liquid_room_clip_vertex_t;
typedef struct liquid_room_clip_stream_t {
    void *object, *input, *output;
    unsigned int member;
    int width;
    liquid_room_array_set_t set;
} liquid_room_clip_stream_t;

static void *liquid_room_member(void *object,unsigned int member,unsigned int operation)
{
    BYTE *table=liquid_node_interface(object,(member&0xfff)*4);
    size_t offset=(member>>24)*64+operation;
    if (!table || !ptr_readable(table+offset,sizeof(void*))) return NULL;
    void *fn=*(void**)(table+offset);
    return ptr_executable(fn) ? fn : NULL;
}

static void liquid_room_array_release(void *data)
{
    if (!data) return;
    if (InterlockedDecrement((LONG*)((BYTE*)data-16))<=0) {
        BYTE *type=*(BYTE**)((BYTE*)data-12);
        void (__cdecl *destroy)(void*)=*(void (__cdecl **)(void*))(type+8);
        destroy(data);
    }
}

static int liquid_room_read_stream(liquid_room_clip_stream_t *s,void *obj,unsigned int member,int width)
{
    liquid_room_array_get_t get=(liquid_room_array_get_t)liquid_room_member(obj,member,0);
    s->set=(liquid_room_array_set_t)liquid_room_member(obj,member,4);
    if (!get || !s->set) return 0;
    s->object=obj; s->member=member; s->width=width; s->input=liquid_room_empty_array;
    get(obj,member,&s->input);
    return 1;
}

static void *liquid_room_new_array_like(const void *source,int count,int width)
{
    if (!source || count<1 || count>LIQUID_ROOM_CLIP_VERTICES ||
        !ptr_readable((const BYTE*)source-16,16)) return NULL;
    const BYTE *type=*(const BYTE*const*)((const BYTE*)source-12);
    if (!type || !ptr_readable(type,20) || *(const int*)(type+16)!=width) return NULL;
    return liquid_room_array_new(count,count,type);
}

/* Sutherland-Hodgman, carrying barycentric weights for every vertex stream. */
static int liquid_room_clip_triangle(const float uv[6],liquid_room_clip_vertex_t out[12])
{
    liquid_room_clip_vertex_t buffers[2][12];
    int count=3, current=0;
    for(int i=0;i<3;i++) {
        memset(&buffers[0][i],0,sizeof(buffers[0][i]));
        for(int j=0;j<2;j++) { if (!_finite(uv[i*2+j])) return 0; buffers[0][i].uv[j]=uv[i*2+j]; }
        buffers[0][i].weight[i]=1;
    }
    for(int plane=0;plane<4 && count;plane++) {
        int axis=plane/2, high=plane&1, next=1-current, produced=0;
        for(int i=0;i<count;i++) {
            const liquid_room_clip_vertex_t *a=&buffers[current][i],*b=&buffers[current][(i+1)%count];
            double da=high ? 1.0-a->uv[axis] : a->uv[axis];
            double db=high ? 1.0-b->uv[axis] : b->uv[axis];
            if (da>=0) buffers[next][produced++]=*a;
            if ((da>=0)!=(db>=0)) {
                double t=da/(da-db); liquid_room_clip_vertex_t *v=&buffers[next][produced++];
                for(int k=0;k<2;k++) v->uv[k]=(float)(a->uv[k]+t*(b->uv[k]-a->uv[k]));
                v->uv[axis]=(float)high;
                for(int k=0;k<3;k++) v->weight[k]=(float)(a->weight[k]+t*(b->weight[k]-a->weight[k]));
            }
        }
        count=produced; current=next;
    }
    if(count<3) return 0;
    double area=0;
    for(int i=0;i<count;i++) {
        const float *a=buffers[current][i].uv,*b=buffers[current][(i+1)%count].uv;
        area+=(double)a[0]*b[1]-(double)b[0]*a[1];
    }
    if(fabs(area)<1e-12) return 0;
    memcpy(out,buffers[current],count*sizeof(*out)); return count;
}

static int liquid_room_clip_mesh(void *mesh,int *vertices_before,int *vertices_after)
{
    liquid_room_clip_stream_t streams[LIQUID_ROOM_CLIP_STREAMS]={0}, indices={0};
    int stream_count=2, uv_slot=-1, count=0, index_count=0, output_count=0, ok=0;
    liquid_node_get_t node=(liquid_node_get_t)liquid_room_member(mesh,0x01fff043,0);
    if (!node || !liquid_room_array_new || !liquid_room_empty_array) return 0;
    void *geometry=node(mesh,0x01fff043);
    liquid_room_int_set_t primitive_set=(liquid_room_int_set_t)liquid_room_member(geometry,0x02fff076,4);
    if (!primitive_set || liquid_node_interface(geometry,0x1e0)) return 0; /* static only */
    if (!liquid_room_read_stream(&streams[0],geometry,0x06fff076,3) ||
        !liquid_room_read_stream(&streams[1],geometry,0x07fff076,3) ||
        !liquid_room_read_stream(&indices,geometry,0x04fff076,1)) goto done;
    count=liquid_native_diag_array_count(streams[0].input,12);
    index_count=liquid_native_diag_array_count(indices.input,4);
    if (count<3 || index_count<3 || index_count%3 ||
        liquid_native_diag_array_count(streams[1].input,12)!=count) goto done;
    typedef int (THISCALL *get_count_t)(void*,unsigned int);
    typedef void *(THISCALL *get_item_t)(void*,unsigned int,int);
    get_count_t get_count=(get_count_t)liquid_room_member(geometry,0x05fff076,8);
    get_item_t get_item=(get_item_t)liquid_room_member(geometry,0x05fff076,0);
    if (!get_count || !get_item) goto done;
    int extras=get_count(geometry,0x05fff076);
    if (extras<1 || extras>LIQUID_ROOM_CLIP_STREAMS-2) goto done;
    for(int i=0;i<extras;i++) {
        void *data=get_item(geometry,0x05fff076,i); int width=0; unsigned int member=0;
        if (liquid_node_interface(data,0x288)) { width=2; member=0x01fff0a2; }
        else if (liquid_node_interface(data,0x28c)) { width=3; member=0x01fff0a3; }
        else if (liquid_node_interface(data,0x290)) { width=4; member=0x01fff0a4; }
        if (!width) goto done;
        get_count_t usage=(get_count_t)liquid_room_member(data,0x01fff0a0,0);
        if (!usage || !liquid_room_read_stream(&streams[stream_count],data,member,width)) goto done;
        int slot=stream_count++;
        if (liquid_native_diag_array_count(streams[slot].input,width*4)!=count) goto done;
        if (usage(data,0x01fff0a0)==0x10000 && width==2) uv_slot=slot;
    }
    if(uv_slot<0) goto done;
    /* First pass validates every index and calculates bounded allocation size. */
    for(int pass=0;pass<2;pass++) {
        int at=0;
        for(int i=0;i<index_count;i+=3) {
            int *ix=(int*)indices.input+i; float uv[6]; liquid_room_clip_vertex_t polygon[12];
            for(int j=0;j<3;j++) {
                if(ix[j]<0 || ix[j]>=count) goto done;
                memcpy(uv+2*j,(float*)streams[uv_slot].input+ix[j]*2,8);
            }
            int n=liquid_room_clip_triangle(uv,polygon);
            if(!n) continue;
            for(int j=1;j<n-1;j++) for(int corner=0;corner<3;corner++) {
                int k=corner==0 ? 0 : j+corner-1;
                if(at>=LIQUID_ROOM_CLIP_VERTICES) goto done;
                if(pass) {
                    ((int*)indices.output)[at]=at;
                    for(int s=0;s<stream_count;s++) for(int d=0;d<streams[s].width;d++) {
                        double value=0;
                        for(int v=0;v<3;v++) value+=polygon[k].weight[v]*((float*)streams[s].input)[ix[v]*streams[s].width+d];
                        if(!_finite(value)) goto done;
                        ((float*)streams[s].output)[at*streams[s].width+d]=(float)value;
                    }
                    memcpy((float*)streams[uv_slot].output+at*2,polygon[k].uv,8);
                }
                at++;
            }
        }
        if(!pass) {
            output_count=at;
            if(!at) goto done;
            indices.output=liquid_room_new_array_like(indices.input,at,4);
            if(!indices.output) goto done;
            for(int s=0;s<stream_count;s++) {
                streams[s].output=liquid_room_new_array_like(streams[s].input,at,streams[s].width*4);
                if(!streams[s].output) goto done;
            }
        }
    }
    /* All data is ready before any setter. Original room arrays stay intact. */
    for(int s=0;s<stream_count;s++) streams[s].set(streams[s].object,streams[s].member,streams[s].output);
    indices.set(indices.object,indices.member,indices.output);
    primitive_set(geometry,0x02fff076,output_count/3);
    *vertices_before=count; *vertices_after=output_count; ok=1;
done:
    if(!ok) {
        liquid_room_array_set_t clear=(liquid_room_array_set_t)liquid_room_member(geometry,0x04fff076,4);
        if(clear) clear(geometry,0x04fff076,liquid_room_empty_array);
        primitive_set(geometry,0x02fff076,0); /* never expose an unbounded decal */
    }
    for(int s=0;s<LIQUID_ROOM_CLIP_STREAMS;s++) { liquid_room_array_release(streams[s].input); liquid_room_array_release(streams[s].output); }
    liquid_room_array_release(indices.input); liquid_room_array_release(indices.output);
    return ok;
}

static void __attribute__((used,noinline)) __cdecl liquid_room_clip_created(void *frame,void *source)
{
    liquid_native_freeze_capture_t *capture=liquid_native_freeze_capture;
    int before=0,after=0;
    if(!capture || capture->room_projection_frame!=frame || !source || capture->room_projection_target!=source ||
        !ptr_readable((BYTE*)frame-0x10,sizeof(void*))) return;
    liquid_room_reset_triangle_validation(capture);
    void *mesh=*(void**)((BYTE*)frame-0x10);
    if(liquid_node_is_nil(mesh)) return;
    LARGE_INTEGER start,end;
    int measure=cfg.enabled;
    if(measure) {
        QueryPerformanceCounter(&start);
        if(capture->room_extract_start)
            capture->room_extract_ticks+=start.QuadPart-capture->room_extract_start;
    }
    capture->room_extract_start=0;
    int ok=liquid_room_clip_mesh(mesh,&before,&after);
    if(measure) {
        QueryPerformanceCounter(&end);
        capture->room_clip_ticks+=end.QuadPart-start.QuadPart;
    }
    static unsigned int logs;
    if(logs++<24) log_line("liquid room decal footprint clipped=%d vertices_before=%d vertices_after=%d",ok,before,after);
}

static void __attribute__((naked)) liquid_room_clip_bridge(void)
{
    __asm__ __volatile__(
        "pushfl\n\tpushal\n\tmovl %esp, %ebx\n\tsubl $528, %esp\n\tandl $-16, %esp\n\t"
        "fxsave (%esp)\n\tfninit\n\tsubl $16, %esp\n\tmovl %ebp, (%esp)\n\t"
        "movl 16(%ebx), %eax\n\tmovl %eax, 4(%esp)\n\t"
        "call _liquid_room_clip_created\n\taddl $16, %esp\n\tfxrstor (%esp)\n\t"
        "movl %ebx, %esp\n\tpopal\n\tpopfl\n\tjmp *_liquid_room_clip_trampoline\n\t");
}

static void liquid_room_install_clip_hook(BYTE *exe,HMODULE sys)
{
    static const BYTE expected[5]={0x8b,0x75,0xf0,0x8b,0xc6};
    if(liquid_room_clip_trampoline) return;
    FARPROC allocate=GetProcAddress(sys,"?UninitializedNewArrayRef1@Memory@Bionic@@SAPAXHHABVTypeInfo@2@@Z");
    memcpy(&liquid_room_array_new,&allocate,sizeof(liquid_room_array_new));
    void **empty=(void**)GetProcAddress(sys,"?G_NullArray@Bionic@@3QBXB");
    liquid_room_empty_array=empty ? *empty : NULL;
    BYTE *site=exe+0x1f27c4;
    if(liquid_room_array_new && liquid_room_empty_array && ptr_readable(site,5) &&
        !memcmp(site,expected,5) && install_inline_hook(site,liquid_room_clip_bridge,5,&liquid_room_clip_trampoline))
        log_line("liquid room footprint clip hook installed uv_bounds=0..1");
    else {
        static int logged;
        if(!logged++) log_line("liquid room footprint clip unavailable; room creation disabled");
    }
}
