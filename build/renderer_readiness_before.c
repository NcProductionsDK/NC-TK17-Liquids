#include "before-renderer-readiness-20260912-172646/NC-TK17-Liquids.c"
int main(void) { hook5_present_registered=hook5_d3d11_scene_registered=1; if (!liquid_native_allowed()) { puts("REPRODUCED: extension registration blocks native rendering without Hook5"); return 0; } return 1; }
