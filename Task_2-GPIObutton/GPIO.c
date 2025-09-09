#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>

// --- prototypes ---
static void btn1_handler(void *arg1, void *arg2);
static void btn2_handler(void *arg1, void *arg2);

/*
 * useBTN1: each press cycles through blink rates for LED1 -> 2 Hz, 1 Hz, 0.5 Hz 
 *         (so toggle every 250 ms, 500 ms, 1000 ms).
 *
 * BTN2: while held, turn LED2 on; when released, LED2 off.
 */

// use aliases instead of hardcode pin numbers so code becomes compatible with other boards
// devicetree also provides information about pul-up/down config, active high/low states, etc
// issue caught at compile time if pin isn't properly configured instead of runtime

// Use the devicetree alias "led0" (on nrf52840dk this maps to LED1, P0.13, active-low)
#define LED0_NODE DT_ALIAS(led0) // find LED called led0 in devicetree and map it to LED0_NODE
#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "No alias 'led0' found in devicetree"
#endif

#define LED1_NODE DT_ALIAS(led1)
#if !DT_NODE_HAS_STATUS(LED1_NODE, okay)
#error "No alias 'led1' found in devicetree"
#endif

#define BTN1_NODE DT_ALIAS(sw0) // sw0 is the alias for the button1 in the devicetree
#if !DT_NODE_HAS_STATUS(BTN1_NODE, okay)
#error "No alias 'btn1' found in devicetree"
#endif

#define BTN2_NODE DT_ALIAS(sw1) // sw1 is the alias for the button2 in the devicetree
#if !DT_NODE_HAS_STATUS(BTN2_NODE, okay)
#error "No alias 'btn2' found in devicetree"
#endif

// keep track of button state
uint8_t btn1_value;
uint8_t btn2_value;

// keep track of which round of button state we are on (0, 1, 2)
uint8_t blink_idx = 0;


static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec btn1 = GPIO_DT_SPEC_GET(BTN1_NODE, gpios);
static const struct gpio_dt_spec btn2 = GPIO_DT_SPEC_GET(BTN2_NODE, gpios);

int main (void) {
    if (!device_is_ready(led1.port)) {
        return 0;
    }
    if (!device_is_ready(led2.port)) {
        return 0;
    }
    if (!device_is_ready(btn1.port)) {
        return 0;
    }
    if (!device_is_ready(btn2.port)) {
        return 0;
    }
    if (gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE) != 0) {
        return 0;
    }

    if (gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE) != 0) {
        return 0;
    }
    if (gpio_pin_configure_dt(&btn1, GPIO_INPUT | GPIO_PULL_UP) != 0) {
        return 0;
    }
    if (gpio_pin_configure_dt(&btn2, GPIO_INPUT | GPIO_PULL_UP) != 0) {
        return 0;
    }

    while (1) {

        // can use polling instead of butotn interrupts because simple application checks both buttons 100 times per second
        btn1_handler(NULL, NULL);
        btn2_handler(NULL, NULL);
        k_msleep(10);
    }
}

static void btn1_handler(void *arg1, void *arg2) {
    static int prev_value = 1; // static means keeps its value between function calls, initialized to 1 because button is pulled up (reads 1 when not pressed)
    btn1_value = gpio_pin_get_dt(&btn1);

    // Detect button press (transition from 1 to 0)
    if (prev_value == 1 && btn1_value == 0) {
        blink_idx++;
    }
    prev_value = btn1_value;

    gpio_pin_toggle_dt(&led1);
    switch (blink_idx % 3) {
        case 0: k_msleep(250); break; // 2 Hz
        case 1: k_msleep(500); break; // 1 Hz
        case 2: k_msleep(1000); break; // 0.5 Hz
    }

    
}
static void btn2_handler(void *arg1, void *arg2) {
    btn2_value = gpio_pin_get_dt(&btn2);
    gpio_pin_set_dt(&led2, !btn2_value); // use ! because btns are active low
}