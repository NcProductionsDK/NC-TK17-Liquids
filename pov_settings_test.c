#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #c); exit(1); } } while (0)

static int forwarded;
static void THISCALL original_change(void *self, const char *parameter,
    const char *value, DWORD arg3, DWORD arg4)
{
    (void)self; (void)parameter; (void)value; (void)arg3; (void)arg4;
    forwarded++;
}

static void THISCALL prepare_saved_values(void *self)
{
    (void)self;
    hook_ConfigEditor_ParamChange(NULL, "NcLiquidsPovEnabled", "OFF", 0, 0);
}

static int THISCALL rebuild_saved_values(void *self, void *a, void *b, void *c)
{
    (void)self; (void)a; (void)b; (void)c;
    hook_Customizer_PrepareControls(NULL);
    hook_ConfigEditor_ParamChange(NULL, "NcLiquidsPovEnabled", "OFF", 0, 0);
    return 73;
}

int main(void)
{
    char directory[MAX_PATH], temporary[MAX_PATH];
    CHECK(GetCurrentDirectoryA(MAX_PATH, directory));
    CHECK(GetTempFileNameA(directory, "pvs", 0, temporary));
    lstrcpynA(config_path, temporary, sizeof(config_path));
    CHECK(WritePrivateProfileStringA("liquids", "pov_enabled", "true", config_path));
    load_config(); cfg.enabled = 0;
    CHECK(cfg.pov_reach==2.5f);
    CHECK(WritePrivateProfileStringA("liquids","pov_reach","3.25",config_path));
    load_config(); CHECK(cfg.pov_reach==3.25f);
    CHECK(WritePrivateProfileStringA("liquids","pov_reach","100",config_path));
    load_config(); CHECK(cfg.pov_reach==10);
    CHECK(WritePrivateProfileStringA("liquids","pov_reach","-1",config_path));
    load_config(); CHECK(cfg.pov_reach==0.5f);
    CHECK(WritePrivateProfileStringA("liquids","pov_reach",NULL,config_path));
    load_config(); CHECK(cfg.pov_reach==2.5f); cfg.enabled=0;
    tramp_ConfigEditor_ParamChange = original_change;
    /* The gameplay log shows these saved/default notifications before
       BuildControls has synchronized the widget from the INI. */
    hook_ConfigEditor_ParamChange(NULL, "NcLiquidsPovEnabled", "OFF", 0, 0);
    CHECK(read_bool("liquids", "pov_enabled", 0));
    CHECK(forwarded == 1);
    /* Widget registration enables genuine edits, but menu reconstruction
       must still ignore saved values even after a prior widget existed. */
    liquid_pov_control_ready = 1;
    tramp_Customizer_PrepareControls = prepare_saved_values;
    tramp_Customizer_BuildControls = rebuild_saved_values;
    CHECK(hook_Customizer_BuildControls(NULL, NULL, NULL, NULL) == 73);
    CHECK(forwarded == 3 && !liquid_pov_settings_build_depth);
    CHECK(read_bool("liquids", "pov_enabled", 0));
    hook_ConfigEditor_ParamChange(NULL, "NcLiquidsPovEnabled", "OFF", 0, 0);
    CHECK(!read_bool("liquids", "pov_enabled", 1));
    hook_ConfigEditor_ParamChange(NULL, "NcLiquidsPovEnabled", "ON", 0, 0);
    CHECK(read_bool("liquids", "pov_enabled", 0));
    liquid_setting_sync_depth++;
    hook_ConfigEditor_ParamChange(NULL, "NcLiquidsPovEnabled", "OFF", 0, 0);
    liquid_setting_sync_depth--;
    CHECK(read_bool("liquids", "pov_enabled", 0));
    CHECK(forwarded == 6);
    liquid_pov_control_ready = 0;
    CHECK(WritePrivateProfileStringA("liquids", "pov_enabled", "false", config_path));
    hook_ConfigEditor_ParamChange(NULL, "NcLiquidsPovEnabled", "ON", 0, 0);
    CHECK(!read_bool("liquids", "pov_enabled", 1));
    CHECK(WritePrivateProfileStringA("liquids", "pov_enabled", NULL, config_path));
    hook_ConfigEditor_ParamChange(NULL, "NcLiquidsPovEnabled", "ON", 0, 0);
    load_config();
    CHECK(!cfg.pov_enabled);
    CHECK(DeleteFileA(temporary));
    puts("PASS: startup/rebuild preserves true, false and missing INI; live menu edits work; sync cannot write back; native callback chaining preserved");
    return 0;
}
