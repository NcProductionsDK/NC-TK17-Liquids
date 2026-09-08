/* Read-only diagnostic for the installed PhysX DLL. No injection or writes.
   RVAs below were verified against its QueryBodyColliderV1 disassembly.
   Caller must verify the installed file hash before using this build. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>

static DWORD read_u32(HANDLE process, ULONG_PTR address)
{
    DWORD value; SIZE_T size;
    if (!ReadProcessMemory(process, (const void*)address, &value, sizeof(value), &size) || size != sizeof(value)) {
        fprintf(stderr, "ReadProcessMemory failed error=%lu\n", GetLastError()); exit(2);
    }
    return value;
}

int main(int argc, char **argv)
{
    DWORD pid; HANDLE snapshot, process; MODULEENTRY32 module = {0}; ULONG_PTR base = 0;
    int i; ULONG_PTR liquids = 0;
    if (argc != 2) { fprintf(stderr, "Usage: physx_live_probe PID\n"); return 2; }
    pid = strtoul(argv[1], NULL, 10);
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (snapshot == INVALID_HANDLE_VALUE) return 2;
    module.dwSize = sizeof(module);
    if (Module32First(snapshot, &module)) do {
        if (!_stricmp(module.szModule, "NC-TK17-PhysX.dll")) base = (ULONG_PTR)module.modBaseAddr;
        if (!_stricmp(module.szModule, "NC-TK17-Liquids.dll")) liquids = (ULONG_PTR)module.modBaseAddr;
        if (strstr(module.szModule, "NC-TK17") || !_stricmp(module.szModule, "d3d8.dll"))
            printf("module=%s base=%p\n", module.szModule, module.modBaseAddr);
    } while (Module32Next(snapshot, &module));
    CloseHandle(snapshot);
    if (!base) { fprintf(stderr, "PhysX module not loaded\n"); return 2; }
    process = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!process) { fprintf(stderr, "OpenProcess error=%lu\n", GetLastError()); return 2; }
    if (0 && liquids) {
        DWORD count = read_u32(process, liquids + 0xebbc8);
        printf("Liquids previous GetProcAddress=%08lx Present=%08lx Create8=%08lx chains=%lu\n",
            read_u32(process, liquids + 0x105cc4), read_u32(process, liquids + 0x105cd0),
            read_u32(process, liquids + 0x105cd8), count);
        if (count <= 128) for (i=0; i<(int)count; i++) printf("chain module=%08lx hook=%08lx previous=%08lx\n",
            read_u32(process, liquids + 0xebbe0 + i*12), read_u32(process, liquids + 0xebbe4 + i*12),
            read_u32(process, liquids + 0xebbe8 + i*12));
    }
    printf("PhysX enabled=%lu camera_valid=%lu heartbeat_age_ms=%lu\n",
        read_u32(process, base + 0xa6ea0), read_u32(process, base + 0x43de80),
        GetTickCount() - read_u32(process, base + 0x4b108e0));
    printf("render_tick_ever_entered=%lu\n", read_u32(process, base + 0xdaa94));
    for (i = 0; i < 4; i++) printf("person=%d enabled=%lu ready=%lu basis_valid=%lu\n", i+1,
        read_u32(process, base + 0x4b5b6e0 + i * 0xeb8),
        read_u32(process, base + 0x4b10900 + i * 0x48bc + 0x4844),
        read_u32(process, base + 0x4b10900 + i * 0x48bc + 0x39e4));
    CloseHandle(process);
    return 0;
}


