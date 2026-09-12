/* Exercise generated defaults and UI writes in an isolated build-directory INI. */
#include "NC-TK17-Liquids.c"
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #c); return 1; } } while (0)
int main(void)
{
    char directory[MAX_PATH], temporary[MAX_PATH];
    CHECK(GetFullPathNameA("Development/NC-TK17-Liquids/build", MAX_PATH, directory, NULL));
    CHECK(GetTempFileNameA(directory, "lqs", 0, temporary));
    CHECK(DeleteFileA(temporary));
    lstrcpynA(config_path, temporary, sizeof(config_path));
    create_default_config_if_missing();
    load_config();
    CHECK(cfg.emitter_position[0] == 0 && cfg.emitter_position[1] == 0 &&
          cfg.emitter_position[2] == 0);
    liquid_handle_setting_change("NcLiquidsEmitterPositionX", "-0.5");
    liquid_handle_setting_change("NcLiquidsEmitterPositionY", "0.25");
    liquid_handle_setting_change("NcLiquidsEmitterPositionZ", "1");
    /* Exercise the actual hot-reload path, preserving existing liquid. */
    liquid_config_write_time_valid = 1;
    memset(&liquid_config_write_time, 0, sizeof(liquid_config_write_time));
    model_emitters[0].transform_valid = 1;
    liquid_particles[0].active = 1;
    liquid_particles[0].position[0] = 0.123f;
    liquid_check_config_reload(GetTickCount() + 1000);
    CHECK(cfg.emitter_position[0] == -0.5f && cfg.emitter_position[1] == 0.25f &&
          cfg.emitter_position[2] == 1);
    CHECK(!model_emitters[0].transform_valid);
    CHECK(liquid_particles[0].active && liquid_particles[0].position[0] == 0.123f);
    CHECK(WritePrivateProfileStringA("penis_liquid_emitter_position", "position_x", "-9", config_path));
    CHECK(WritePrivateProfileStringA("penis_liquid_emitter_position", "position_y", "9", config_path));
    CHECK(WritePrivateProfileStringA("penis_liquid_emitter_position", "position_z", "nan", config_path));
    load_config();
    CHECK(cfg.emitter_position[0] == -1 && cfg.emitter_position[1] == 1 &&
          cfg.emitter_position[2] == 0);
    CHECK(WritePrivateProfileStringA("penis_liquid_emitter_position", NULL, NULL, config_path));
    load_config();
    CHECK(cfg.emitter_position[0] == 0 && cfg.emitter_position[1] == 0 &&
          cfg.emitter_position[2] == 0);
    puts("PASS: emitter defaults, UI-to-INI bindings, live reload without moving existing particles, range clamps, invalid/missing values");
    CHECK(!read_bool("liquid_contact_droplets", "connect_droplets", 1));
    CHECK(fabsf(read_float("liquid_contact_droplets", "connection_distance", -1) - 0.03f) < 0.000001f);
    CHECK(fabsf(read_float("liquid_contact_droplets", "connection_thickness", -1) - 0.55f) < 0.000001f);
    liquid_handle_setting_change("NcLiquidsContactConnectDroplets", "ON");
    liquid_handle_setting_change("NcLiquidsContactDropletConnectionDistance", "0.035");
    liquid_handle_setting_change("NcLiquidsContactDropletConnectionThickness", "0.25");
    CHECK(read_bool("liquid_contact_droplets", "connect_droplets", 0));
    CHECK(fabsf(read_float("liquid_contact_droplets", "connection_distance", -1) - 0.035f) < 0.000001f);
    CHECK(fabsf(read_float("liquid_contact_droplets", "connection_thickness", -1) - 0.25f) < 0.000001f);
    create_default_config_if_missing();
    CHECK(fabsf(read_float("liquid_contact_droplets", "connection_thickness", -1) - 0.25f) < 0.000001f);
    CHECK(DeleteFileA(temporary));
    puts("PASS: generated contact defaults; three independent UI bindings; existing INI preserved");
    return 0;
}
