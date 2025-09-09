#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/devicetree.h>

/* Use the PWM-backed LED alias from the overlay */
// can't use default because not pwm-backed
#define LED3_PWM_NODE DT_ALIAS(pwmled3)
#if !DT_NODE_HAS_STATUS(LED3_PWM_NODE, okay)
#error "pwmled3 alias not found (check app.overlay)"
#endif

static const struct pwm_dt_spec led3_pwm = PWM_DT_SPEC_GET(LED3_PWM_NODE);

int main(void)
{
    if (!device_is_ready(led3_pwm.dev)) {
        return 0;
    }

    /* Start from LED off (duty 0) */
    if (pwm_set_dt(&led3_pwm, led3_pwm.period, 0) != 0) {
        return 0;
    }

    /* Breathe: triangle ramp up and down */
    const int steps = 64;                  // smoothness
    const int cycle_ms = 2000;             // one full breath in ~2 s
    const int step_delay = cycle_ms / (2 * steps); // per half-cycle
    uint32_t max_pulse = led3_pwm.period;  // from DT (1 kHz → period = 1 ms)

    uint32_t pulse = 0;
    bool up = true;

    while (1) {
        /* Apply new duty */
        (void)pwm_set_dt(&led3_pwm, led3_pwm.period, pulse);

        /* Next step */
        if (up) {
            pulse += max_pulse / steps;
            if (pulse >= max_pulse) { 
                pulse = max_pulse; 
                up = false; 
            }
        } else {
            if (pulse <= max_pulse / steps) { 
                pulse = 0; 
                up = true; 
            } else { 
                pulse -= max_pulse / steps; 
            }
        }

        k_msleep(step_delay);
    }
}