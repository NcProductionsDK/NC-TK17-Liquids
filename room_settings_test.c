#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); return 1; } } while(0)
static int forwarded;
static void THISCALL original_change(void *self, const char *param, const char *value, DWORD a, DWORD b)
{ (void)self; (void)param; (void)value; (void)a; (void)b; forwarded++; }
static void THISCALL prepare_defaults(void *self)
{ (void)self; hook_ConfigEditor_ParamChange(NULL,"NcLiquidsRoomStains","OFF",0,0); }
static int THISCALL rebuild_defaults(void *self, void *a, void *b, void *c)
{
    (void)a; (void)b; (void)c;
    hook_Customizer_PrepareControls(self);
    hook_ConfigEditor_ParamChange(NULL,"NcLiquidsRoomStains","OFF",0,0);
    return 73;
}
int main(void)
{
    char directory[MAX_PATH], temporary[MAX_PATH];
    CHECK(GetCurrentDirectoryA(MAX_PATH,directory));
    CHECK(GetTempFileNameA(directory,"rds",0,temporary));
    CHECK(DeleteFileA(temporary));
    lstrcpynA(config_path,temporary,sizeof(config_path));
    create_default_config_if_missing(); load_config();
    CHECK(!cfg.collision_spawn_room_stains && cfg.collision_spawn_model_stains);
    liquid_handle_setting_change("NcLiquidsRoomStains","ON"); load_config();
    CHECK(cfg.collision_spawn_room_stains && cfg.collision_spawn_model_stains);
    liquid_handle_setting_change("NcLiquidsBodyStains","OFF"); load_config();
    CHECK(cfg.collision_spawn_room_stains && !cfg.collision_spawn_model_stains);
    liquid_handle_setting_change("NcLiquidsRoomStains","OFF"); load_config();
    CHECK(!cfg.collision_spawn_room_stains && !cfg.collision_spawn_model_stains);
    /* Room settings follow native saved/menu values, including startup and
       rebuild notifications. Only our INI-to-widget writeback is suppressed. */
    liquid_handle_setting_change("NcLiquidsRoomStains","ON");
    tramp_ConfigEditor_ParamChange=original_change;
    hook_ConfigEditor_ParamChange(NULL,"NcLiquidsRoomStains","OFF",0,0);
    CHECK(!read_bool("liquid_collision","spawn_room_stains",1) && forwarded==1);
    liquid_handle_setting_change("NcLiquidsRoomStains","ON");
    tramp_Customizer_PrepareControls=prepare_defaults;
    tramp_Customizer_BuildControls=rebuild_defaults;
    CHECK(hook_Customizer_BuildControls(NULL,NULL,NULL,NULL)==73);
    CHECK(!liquid_pov_settings_build_depth && forwarded==3);
    CHECK(!read_bool("liquid_collision","spawn_room_stains",1));
    hook_ConfigEditor_ParamChange(NULL,"NcLiquidsRoomStains","OFF",0,0);
    CHECK(!read_bool("liquid_collision","spawn_room_stains",1));
    hook_ConfigEditor_ParamChange(NULL,"NcLiquidsRoomStains","ON",0,0);
    CHECK(read_bool("liquid_collision","spawn_room_stains",0));
    liquid_setting_sync_depth++;
    hook_ConfigEditor_ParamChange(NULL,"NcLiquidsRoomStains","OFF",0,0);
    liquid_setting_sync_depth--;
    CHECK(read_bool("liquid_collision","spawn_room_stains",0) && forwarded==6);
    CHECK(WritePrivateProfileStringA("liquid_collision","spawn_room_stains","false",config_path));
    hook_ConfigEditor_ParamChange(NULL,"NcLiquidsRoomStains","ON",0,0);
    CHECK(read_bool("liquid_collision","spawn_room_stains",0));
    CHECK(WritePrivateProfileStringA("liquid_collision","spawn_room_stains",NULL,config_path));
    hook_ConfigEditor_ParamChange(NULL,"NcLiquidsRoomStains","ON",0,0);
    load_config(); CHECK(cfg.collision_spawn_room_stains);
    char missing[32]; GetPrivateProfileStringA("liquid_collision","spawn_room_stains","missing",missing,sizeof(missing),config_path);
    CHECK(!strcmp(missing,"true"));
    CHECK(DeleteFileA(temporary));
    puts("PASS: room default and independent switches; startup/rebuild/menu values sync normally, INI-to-widget writeback suppressed, native callback forwarding preserved");
    return 0;
}
