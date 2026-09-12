#ifndef LIQUID_LOG_SOURCE_INCLUDED
#include "NC-TK17-Liquids.c"
#endif
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)

static DWORD WINAPI writer(void *arg)
{
    int i, id = (int)(uintptr_t)arg;
    for(i=0;i<250;i++) log_line("logger-fixture thread=%d item=%d literal=100%%",id,i);
    return 0;
}

int main(void)
{
    HANDLE threads[4];
    LARGE_INTEGER start,end,frequency;
    FILE *file;
    char line[512];
    int i, count=0, seen[4][250] = {{0}};
    InitializeCriticalSection(&log_lock);
    strcpy(log_path,"Development/NC-TK17-Liquids/build/diagnostic-logging-test.log");
    /* Truncate this test's own output only. */
    file=fopen(log_path,"wb"); CHECK(file); fclose(file);
    cfg.enabled=1; log_ready=1;
    QueryPerformanceFrequency(&frequency); QueryPerformanceCounter(&start);
    for(i=0;i<4;i++) { threads[i]=CreateThread(NULL,0,writer,(void*)(uintptr_t)i,0,NULL); CHECK(threads[i]); }
    CHECK(WaitForMultipleObjects(4,threads,TRUE,10000)==WAIT_OBJECT_0);
    for(i=0;i<4;i++) CloseHandle(threads[i]);
    liquid_flush_log(0);
    QueryPerformanceCounter(&end);
    printf("Logging: 1000 complete events in %.3f ms\n",
           (end.QuadPart-start.QuadPart)*1000.0/frequency.QuadPart);
    cfg.enabled=0; log_line("must-not-appear"); liquid_flush_log(1);
    CHECK(!liquid_log_file);
    file=fopen(log_path,"rb"); CHECK(file);
    while(fgets(line,sizeof(line),file)) {
        int id,item;
        char *event=strstr(line,"logger-fixture");
        CHECK(event && sscanf(event,"logger-fixture thread=%d item=%d",&id,&item)==2);
        CHECK(id>=0 && id<4 && item>=0 && item<250 && !seen[id][item]);
        seen[id][item]=1; count++;
        CHECK(strstr(line,"literal=100%\r\n"));
    }
    fclose(file); CHECK(count==1000);
    cfg.enabled=1; log_line("logger-reopened"); liquid_flush_log(1);
    file=fopen(log_path,"rb"); CHECK(file);
    for(i=0;fgets(line,sizeof(line),file);i++) {}
    fclose(file); CHECK(i==1001);
#ifndef LIQUID_LOG_SOURCE_INCLUDED
    /* Detailed scans must not run during creation, or with deep capture off. */
    cfg.deep_event_capture=1;
    liquid_native_animation_diag_last_tick=0;
    liquid_native_animation_diag_until_tick=GetTickCount()+2500;
    liquid_native_animation_diag_sample((void*)1);
    CHECK(!liquid_native_animation_diag_last_tick);
    liquid_native_animation_diag_until_tick=GetTickCount()+2000;
    liquid_native_animation_diag_sample((void*)1);
    CHECK(liquid_native_animation_diag_last_tick);
    cfg.deep_event_capture=0; liquid_native_animation_diag_last_tick=0;
    liquid_native_animation_diag_sample((void*)1);
    CHECK(!liquid_native_animation_diag_last_tick);
#endif
    DeleteCriticalSection(&log_lock);
    puts("PASS: concurrent complete records; literal formatting; flush; disable; close/reopen preserves prior log");
    return 0;
}
