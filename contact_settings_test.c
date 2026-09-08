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
