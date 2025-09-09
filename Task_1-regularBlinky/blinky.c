#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>

// Use the devicetree alias "led0" (on nrf52840dk this maps to LED1, P0.13, active-low)
#define LED0_NODE DT_ALIAS(led0) // find LED called led0 in devicetree and map it to LED0_NODE
#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "No alias 'led0' found in devicetree"
#endif

// gpios argument refers to the devicetree whcih specific property of that led0 to get (there's another property called label = Green LED 0),
// but gpios property tells the connection of the led0 to pin 13 and that it's active-low
// gpio_dt_spec holds pointer to GPIO 0, varible for pin, and variable for flags (active-low)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void)
{
    // In our bare metal version, we didn't check - we assumed it was ready!
    // In real bare metal, we'd need to check GPIO power and clock registers
    // checks if GPIO peripheral is powered on
    // verifies clock to GPIO is enabled
    if (!device_is_ready(led.port)) {
        return 0;
    }

    // Configure using DT flags; Zephyr handles active-low correctly
    // GPIO_OUTPUT_INACTIVE means set pin as output and initialize to inactive state
    // if we passed led it would make a copy of the struct
    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE) != 0) {
        return 0;
    }

    while (1) {
        gpio_pin_toggle_dt(&led);
        k_msleep(500); // 0.5 s on/off -> 1 Hz blink
    }
}
