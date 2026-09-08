/* Production flight regression checks, without starting TK17. */
#include "NC-TK17-Liquids.c"
#include <assert.h>

static liquid_particle_t flight(int steps, float drag)
{
    liquid_particle_t p = {0};
    int i;
    cfg.drag = drag;
    cfg.gravity_strength = 1.0f;
    p.velocity[0] = 2.0f;
    p.velocity[1] = 3.0f;
    p.velocity[2] = -1.0f;
    for (i = 0; i < steps; i++)
        liquid_integrate_flight(&p, 1.0f / steps);
    return p;
}

int main(void)
{
    float drags[] = {0.0f, 0.00001f, 0.8f, 8.0f};
    int counts[] = {30, 60, 144};
    unsigned int d, n;
    int axis;
    for (d = 0; d < sizeof(drags) / sizeof(drags[0]); d++) {
        liquid_particle_t reference = flight(1, drags[d]);
        for (n = 0; n < sizeof(counts) / sizeof(counts[0]); n++) {
            liquid_particle_t p = flight(counts[n], drags[d]);
            for (axis = 0; axis < 3; axis++) {
                assert(fabsf(p.position[axis] - reference.position[axis]) < 0.0001f);
                assert(fabsf(p.velocity[axis] - reference.velocity[axis]) < 0.0001f);
            }
        }
        if (drags[d] == 0.0f) {
            assert(fabsf(reference.position[1] - (3.0f - 4.905f)) < 0.00001f);
            assert(fabsf(reference.velocity[1] - (3.0f - 9.81f)) < 0.00001f);
        }
    }
    {
        liquid_emitter_t emitter = {0};
        float origin[3] = {0}, direction[3] = {1, 0, 0};
        int i;
        cfg.particle_limit = 3;
        cfg.speed = 2.0f;
        cfg.particle_lifetime = 2.0f;
        cfg.drag = cfg.gravity_strength = cfg.spread = 0.0f;
        for (i = 0; i < 3; i++) {
            liquid_particle_t *p = &liquid_particles[i];
            liquid_spawn_particle(&emitter, origin, direction, 0, 1.0f,
                                  (i + 1) * 0.01f, (i + 1) * 0.01f);
            liquid_integrate_flight(p, 0.035f - p->birth_delay);
            assert(fabsf(p->age - (0.025f - i * 0.01f)) < 0.000001f);
            assert(p->previous[0] == 0.0f);
            if (i) assert(p->position[0] < liquid_particles[i - 1].position[0]);
        }
    }
    puts("PASS: flight agrees at 30/60/144 Hz; ballistic limit; staggered births retain collision sweep origins");
    return 0;
}
