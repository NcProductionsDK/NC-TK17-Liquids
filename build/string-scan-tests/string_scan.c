
#include <windows.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned queries;
static SIZE_T counted_query(LPCVOID p, PMEMORY_BASIC_INFORMATION m, SIZE_T n) {
    queries++; return VirtualQuery(p,m,n);
}
#define VirtualQuery counted_query
static int ptr_readable(const void *pointer, size_t bytes)
{
    MEMORY_BASIC_INFORMATION mbi;
    const BYTE *cur = (const BYTE*)pointer;
    const BYTE *end;
    if (!pointer) return 0;
    end = cur + bytes;
    if (end < cur) return 0;
    while (cur < end) {
        const BYTE *region_end;
        if (!VirtualQuery(cur, &mbi, sizeof(mbi))) return 0;
        if (mbi.State != MEM_COMMIT ||
            (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return 0;
        region_end = (const BYTE*)mbi.BaseAddress + mbi.RegionSize;
        if (region_end <= cur) return 0;
        cur = region_end;
    }
    return 1;
}
static int safe_cstr_a(const char *text, size_t max_len)
{
    size_t i = 0;
    if (!text) return 0;
    /* Object names arrive in bursts when the Key Editor scrolls. Validate
       each region once for this scan, rather than calling VirtualQuery for
       every character. Never read into the next region without checking it,
       and do not retain permissions across calls or frames. */
    while (i < max_len) {
        MEMORY_BASIC_INFORMATION mbi;
        uintptr_t address = (uintptr_t)text + i;
        uintptr_t region_end;
        size_t available, stop;
        if (address < (uintptr_t)text ||
            !VirtualQuery((const void*)address, &mbi, sizeof(mbi)) ||
            mbi.State != MEM_COMMIT ||
            (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return 0;
        region_end = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
        if (region_end <= address) return 0;
        available = region_end - address;
        if (available > max_len - i) available = max_len - i;
        stop = i + available;
        for (; i < stop; i++) {
            unsigned char ch = (unsigned char)text[i];
            if (!ch) return i > 0;
            if (ch < 32 || ch > 126) return 0;
        }
    }
    return 0;
}
static const char *stringref_cstr_a(const void *ref)
{
    const char *direct;
    const char *indirect;
    if (!ref) return NULL;
    direct = (const char*)ref;
    if (safe_cstr_a(direct, 256)) return direct;
    if (!ptr_readable(ref, sizeof(char*))) return NULL;
    indirect = *(const char* const*)ref;
    return safe_cstr_a(indirect, 256) ? indirect : NULL;
}

static int reference_scan(const char *s,size_t n) {
    if(!s) return 0;
    for(size_t i=0;i<n;i++) {
        if(!ptr_readable(s+i,1)) return 0;
        unsigned char c=(unsigned char)s[i];
        if(!c) return i>0;
        if(c<32 || c>126) return 0;
    }
    return 0;
}
static double timer(void) {
    LARGE_INTEGER t,f; QueryPerformanceCounter(&t);QueryPerformanceFrequency(&f);
    return (double)t.QuadPart/f.QuadPart;
}
int main(void) {
    SYSTEM_INFO si; GetSystemInfo(&si);
    SIZE_T page=si.dwPageSize; DWORD old;
    char *p=VirtualAlloc(NULL,page*3,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
    assert(p); memset(p,'A',page*3);
    assert(VirtualProtect(p+page,page,PAGE_NOACCESS,&old));
    assert(VirtualFree(p+2*page,page,MEM_DECOMMIT));
    assert(!safe_cstr_a(NULL,256)); assert(!safe_cstr_a((char*)1,256));
    assert(!safe_cstr_a(p,0)); assert(!safe_cstr_a(p+page,1));
    assert(!safe_cstr_a(p+2*page,1));
    p[page-1]=0;
    queries=0; assert(safe_cstr_a(p+page-128,256)); assert(queries==1);
    p[page-1]='A';
    queries=0; assert(!safe_cstr_a(p+page-128,256)); assert(queries==2);
    assert(VirtualProtect(p+page,page,PAGE_READWRITE,&old)); p[page+3]=0;
    assert(VirtualProtect(p+page,page,PAGE_READONLY,&old));
    queries=0; assert(safe_cstr_a(p+page-2,256)); assert(queries==2);
    assert(!safe_cstr_a(p+page-2,5)); assert(safe_cstr_a(p+page-2,6));
    assert(VirtualProtect(p+page,page,PAGE_READWRITE|PAGE_GUARD,&old));
    assert(!safe_cstr_a(p+page-2,256));
    MEMORY_BASIC_INFORMATION mbi;
    assert(counted_query(p+page,&mbi,sizeof(mbi)) && (mbi.Protect&PAGE_GUARD));
    assert(VirtualProtect(p+page,page,PAGE_READONLY,&old));
    assert(safe_cstr_a(p+page-2,256));
    assert(VirtualProtect(p+page,page,PAGE_NOACCESS,&old));
    assert(!safe_cstr_a(p+page-2,256));
    puts("PASS: inaccessible, uncommitted, read-only and guard regions, cross-region terminators, limits and permission changes");
    char name[256];
    for(int c=0;c<256;c++) for(int n=0;n<256;n+=7) {
        memset(name,'A',sizeof(name)); name[n]=(char)c; name[255]=0;
        for(size_t limit=0;limit<=256;limit+=8)
            assert(safe_cstr_a(name,limit)==reference_scan(name,limit));
    }
    strcpy(name,"PoseEdit_KeyframeP01_Image");
    assert(stringref_cstr_a(name)==name);
    assert(!stringref_cstr_a(NULL));
    const char **ref=(const char**)p;
    *ref=p+512; strcpy(p+512,name);
    assert(stringref_cstr_a(ref)==*ref);
    *ref=p+page; assert(!stringref_cstr_a(ref));
    memset(name,'A',255); name[255]=0;
    queries=0; assert(safe_cstr_a(name,256)); assert(queries==1);
    queries=0; assert(reference_scan(name,256)); assert(queries==256);
    puts("PASS: differential byte validation, direct/indirect StringRef, 255-character names: 256 queries reduced to one");
    strcpy(name,"PoseEdit_KeyframeP01_Image");
    volatile unsigned sink=0;
    for(int mode=0;mode<2;mode++) {
        double start=timer(); queries=0;
        for(int i=0;i<100000;i++) sink+=mode?safe_cstr_a(name,256):reference_scan(name,256);
        printf("BENCH %s: %.3f ms, %u queries / 100000 names\n",mode?"fixed":"original",(timer()-start)*1000,queries);
    }
    assert(sink==200000); assert(VirtualFree(p,0,MEM_RELEASE));
    return 0;
}
