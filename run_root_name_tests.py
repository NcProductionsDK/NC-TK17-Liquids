"""Check emitter identity matching against the previous formatting implementation."""
import os
from pathlib import Path
import re
import subprocess

root = Path(__file__).resolve().parent
source = (root / 'NC-TK17-Liquids.c').read_text(encoding='utf-8')
match = re.search(r'^static int liquid_person_spermray_root_name\([^;{}]*\)\s*\{',
                  source, re.M)
assert match
end, depth = match.end(), 1
while depth:
    depth += (source[end] == '{') - (source[end] == '}')
    end += 1
fixture = r'''
#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
'''
fixture += source[match.start():end]
fixture += r'''
static int reference(const char *name) {
    char expected[32];
    if (!name) return 0;
    for (int person=1; person<=4; person++) {
        _snprintf(expected,sizeof(expected),"Person%02dSpermray",person);
        expected[sizeof(expected)-1]=0;
        if (!_stricmp(name,expected)) return person;
    }
    return 0;
}
static void compare(const char *name) {
    assert(liquid_person_spermray_root_name(name)==reference(name));
}
static double timer(void) {
    LARGE_INTEGER t,f; QueryPerformanceCounter(&t); QueryPerformanceFrequency(&f);
    return (double)t.QuadPart/f.QuadPart;
}
int main(void) {
    char name[256], original[256];
    compare(NULL); compare("");
    for (int person=0; person<=99; person++) {
        _snprintf(name,sizeof(name),"Person%02dSpermray",person);
        compare(name);
        assert(liquid_person_spermray_root_name(name)==
               (person>=1 && person<=4 ? person : 0));
    }
    for (int person=1; person<=4; person++) {
        _snprintf(original,sizeof(original),"Person%02dSpermray",person);
        /* Every ASCII case combination, including mixed-case names. */
        for (unsigned mask=0; mask<(1u<<13); mask++) {
            unsigned bit=0;
            strcpy(name,original);
            for (unsigned i=0; name[i]; i++) {
                unsigned char c=(unsigned char)name[i];
                if ((c>='a' && c<='z') || (c>='A' && c<='Z')) {
                    name[i]=(char)((c&~32u) | ((mask>>bit++ & 1u) ? 32u : 0u));
                }
            }
            compare(name); assert(liquid_person_spermray_root_name(name)==person);
        }
        /* Truncations, non-ASCII bytes, altered digits, terminators and suffixes. */
        for (unsigned pos=0; pos<=strlen(original); pos++) {
            for (unsigned byte=0; byte<256; byte++) {
                strcpy(name,original); name[strlen(original)+1]=0;
                name[pos]=(char)byte; compare(name);
            }
        }
        _snprintf(name,sizeof(name),"S%s",original); compare(name);
        assert(!liquid_person_spermray_root_name(name));
        assert(liquid_person_spermray_root_name(name+1)==person);
        _snprintf(name,sizeof(name),"/Primary01/%s",original); compare(name);
        _snprintf(name,sizeof(name),"%s:spermray01_group",original); compare(name);
    }
    compare("Tool01Spermray"); compare("PoseEdit_KeyframeP01_Image");
    memset(name,'A',255); name[255]=0; compare(name);
    puts("PASS: person identity, all case combinations, byte mutations, truncations and aliases");
    strcpy(name,"PoseEdit_KeyframeP01_Image");
    volatile unsigned sink=0;
    for (int mode=0; mode<2; mode++) {
        double start=timer();
        for (unsigned i=0; i<100000; i++)
            sink+=mode?liquid_person_spermray_root_name(name):reference(name);
        printf("BENCH %s: %.3f ms / 100000 unrelated names\n",
               mode?"fixed":"original",(timer()-start)*1000);
    }
    assert(!sink);
    return 0;
}
'''
build = root / 'build/root-name-tests'
build.mkdir(parents=True, exist_ok=True)
cfile, exe = build / 'root_name.c', build / 'root_name.exe'
cfile.write_text(fixture, encoding='utf-8')
env = dict(os.environ)
env['PATH'] = r'C:\msys64\mingw32\bin;' + env.get('PATH', '')
subprocess.run([r'C:\msys64\mingw32\bin\gcc.exe', '-m32', '-O2', '-Wall',
                '-Wextra', '-Werror', str(cfile), '-o', str(exe)], env=env, check=True)
subprocess.run([str(exe)], env=env, check=True)
