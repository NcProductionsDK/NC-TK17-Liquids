#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL %d: %s\n", __LINE__, #c); exit(1); } } while (0)
static int webm_calls, physx_calls;
static IDirect3D8 *WINAPI webm_factory(UINT sdk)
{ CHECK(sdk == 220); webm_calls++; return NULL; }
static IDirect3D8 *WINAPI physx_factory(UINT sdk)
{ CHECK(sdk == 220); physx_calls++; return NULL; }
static FARPROC WINAPI webm_resolver(HMODULE m, LPCSTR n)
{ (void)m; (void)n; webm_calls++; return (FARPROC)(ULONG_PTR)123; }
static FARPROC WINAPI physx_resolver(HMODULE m, LPCSTR n)
{ (void)m; (void)n; physx_calls++; return (FARPROC)(ULONG_PTR)456; }

/* A tiny PE import fixture and real calling stub in that module allocation.
   This exercises caller dispatch, not just a lookup helper. */
static BYTE *fixture(void *previous)
{
    BYTE *base = VirtualAlloc(NULL, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    IMAGE_DOS_HEADER *dos;
    IMAGE_NT_HEADERS *nt;
    IMAGE_IMPORT_DESCRIPTOR *imports;
    IMAGE_THUNK_DATA *names, *iat;
    BYTE stub[] = {0xff,0x74,0x24,0x08,0xff,0x74,0x24,0x08,
                   0xff,0x15,0,0,0,0,0xc2,0x08,0};
    DWORD pointer;
    CHECK(base);
    dos = (IMAGE_DOS_HEADER*)base; dos->e_magic = IMAGE_DOS_SIGNATURE; dos->e_lfanew = 0x80;
    nt = (IMAGE_NT_HEADERS*)(base + 0x80); nt->Signature = IMAGE_NT_SIGNATURE;
    nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = 0x200;
    imports = (IMAGE_IMPORT_DESCRIPTOR*)(base + 0x200);
    imports->Name = 0x300; imports->OriginalFirstThunk = 0x340; imports->FirstThunk = 0x360;
    strcpy((char*)base + 0x300, "KERNEL32.dll");
    names = (IMAGE_THUNK_DATA*)(base + 0x340); names->u1.AddressOfData = 0x380;
    strcpy((char*)base + 0x382, "GetProcAddress");
    iat = (IMAGE_THUNK_DATA*)(base + 0x360); iat->u1.Function = (ULONG_PTR)previous;
    pointer = (DWORD)(ULONG_PTR)iat;
    memcpy(stub + 10, &pointer, sizeof(pointer)); memcpy(base + 0x400, stub, sizeof(stub));
    return base;
}

int main(void)
{
    BYTE *a = fixture(webm_resolver), *b = fixture(physx_resolver);
    get_proc_address_t call_a = (get_proc_address_t)(a + 0x400);
    get_proc_address_t call_b = (get_proc_address_t)(b + 0x400);
    liquid_patch_iat((HMODULE)a, "KERNEL32.dll", "GetProcAddress", hook_liquid_GetProcAddress, (void**)&real_GetProcAddress);
    liquid_patch_iat((HMODULE)b, "KERNEL32.dll", "GetProcAddress", hook_liquid_GetProcAddress, (void**)&real_GetProcAddress);
    CHECK(call_a(NULL, NULL) == (FARPROC)(ULONG_PTR)123);
    CHECK(call_b(NULL, NULL) == (FARPROC)(ULONG_PTR)456);
    CHECK(webm_calls == 1 && physx_calls == 1);
    CHECK(liquid_import_chain_count == 2);
    {
        direct3d_create8_t first = (direct3d_create8_t)liquid_wrap_d3d8_factory((FARPROC)webm_factory);
        direct3d_create8_t second = (direct3d_create8_t)liquid_wrap_d3d8_factory((FARPROC)physx_factory);
        CHECK(first != second);
        second(220); first(220);
        CHECK(webm_calls == 2 && physx_calls == 2);
        CHECK(liquid_wrap_d3d8_factory((FARPROC)first) == (FARPROC)first);
        CHECK(liquid_wrap_d3d8_factory((FARPROC)physx_factory) == (FARPROC)second);
    }
    /* A later wrapper must not be displaced during startup retries. */
    ((IMAGE_THUNK_DATA*)(b + 0x360))->u1.Function = (ULONG_PTR)webm_resolver;
    liquid_patch_iat((HMODULE)b, "KERNEL32.dll", "GetProcAddress", hook_liquid_GetProcAddress, (void**)&real_GetProcAddress);
    CHECK(((IMAGE_THUNK_DATA*)(b + 0x360))->u1.Function == (ULONG_PTR)webm_resolver);
    CHECK(liquid_import_chain_count == 2);
    /* PhysX on_create re-installs its original resolver after Liquids
       wrapped it during loadextension. This removes us, rather than
       wrapping us. A retry must restore the existing chain. */
    ((IMAGE_THUNK_DATA*)(b + 0x360))->u1.Function = (ULONG_PTR)physx_resolver;
    liquid_patch_iat((HMODULE)b, "KERNEL32.dll", "GetProcAddress", hook_liquid_GetProcAddress, (void**)&real_GetProcAddress);
    CHECK(((IMAGE_THUNK_DATA*)(b + 0x360))->u1.Function == (ULONG_PTR)hook_liquid_GetProcAddress);
    CHECK(call_b(NULL, NULL) == (FARPROC)(ULONG_PTR)456);
    CHECK(liquid_import_chain_count == 2);
    VirtualFree(a, 0, MEM_RELEASE); VirtualFree(b, 0, MEM_RELEASE);
    puts("PASS: real calls preserve different per-module plugin resolver chains; retries retain later wrappers");
    return 0;
}
