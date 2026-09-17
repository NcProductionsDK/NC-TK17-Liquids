"""Exercise production string validation against Windows memory boundaries."""
import os
from pathlib import Path
import re
import subprocess

root = Path(__file__).resolve().parent
source = (root / 'NC-TK17-Liquids.c').read_text(encoding='utf-8')


def function(name):
    match = re.search(r'^static [^;{}]*\b' + name + r'\([^;{}]*\)\s*\{', source, re.M)
    if not match:
        raise ValueError(name)
    pos, depth = match.end(), 1
    while depth:
        depth += (source[pos] == '{') - (source[pos] == '}')
        pos += 1
    return source[match.start():pos] + '\n'


fixture = r'''
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
'''
fixture += function('ptr_readable')
fixture += function('safe_cstr_a')
fixture += function('stringref_cstr_a')
fixture += r'''
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
'''
build = root / 'build/string-scan-tests'
build.mkdir(parents=True, exist_ok=True)
cfile, exe = build / 'string_scan.c', build / 'string_scan.exe'
cfile.write_text(fixture)
env = dict(os.environ)
env['PATH'] = r'C:\msys64\mingw32\bin;' + env.get('PATH', '')
subprocess.run([r'C:\msys64\mingw32\bin\gcc.exe', '-m32', '-O2', '-Wall',
                '-Wextra', '-Werror', str(cfile), '-o', str(exe)], env=env, check=True)
subprocess.run([str(exe)], env=env, check=True)
