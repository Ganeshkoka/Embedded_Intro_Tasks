#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>
#include <stdio.h>

// The devicetree node identifier for the BME680
#define BME680_NODE DT_NODELABEL(bme680)

#if !DT_NODE_EXISTS(BME680_NODE)
#error "No BME680 sensor specified in DT_NODELABEL(bme680)"
#endif

static const struct device *get_bme680_device(void)
{
    const struct device *dev = DEVICE_DT_GET(BME680_NODE);
    
    if (!device_is_ready(dev)) {
        printk("Error: Device %s is not ready\n", dev->name);
        return NULL;
    }

    return dev;
}

// Shell command to read sensor data
static int cmd_read_sensor(const struct shell *shell, size_t argc, char **argv)
{
    // need fresh handle each time becuase device state might have changed since startup. safety measure ensuring always working with valid device handle
    const struct device *dev = get_bme680_device();
    if (dev == NULL) {
        shell_error(shell, "Failed to get sensor device");
        return -1;
    }

    struct sensor_value temp, press, humidity;
    int rc;

    // Get fresh data
    rc = sensor_sample_fetch(dev); // this runs I2C under the hood
    if (rc != 0) {
        shell_error(shell, "Failed to fetch sensor data: %d", rc);
        return -1;
    }

    // Get temperature
    rc = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
    if (rc == 0) {
        shell_print(shell, "Temperature: %.2f °C", sensor_value_to_double(&temp));
    }

    // Get pressure
    rc = sensor_channel_get(dev, SENSOR_CHAN_PRESS, &press);
    if (rc == 0) {
        shell_print(shell, "Pressure: %.2f kPa", sensor_value_to_double(&press));
    }

    // Get humidity
    rc = sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &humidity);
    if (rc == 0) {
        shell_print(shell, "Humidity: %.2f %%", sensor_value_to_double(&humidity));
    }

    return 0;
}

// Register shell commands
SHELL_CMD_REGISTER(sensor, NULL, "Read sensor data", cmd_read_sensor);

void main(void)
{
    printk("BME680 sensor application started\n");
    
    // verfiy sensor availability at start providing immediate feedback to user
    const struct device *dev = get_bme680_device();
    if (dev == NULL) {
        printk("Failed to get sensor device. Check your devicetree overlay!\n");
        return;
    }

    printk("Found sensor device %s\n", dev->name);
    printk("Use 'sensor' command to read sensor data\n");
}
