#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define GPIO_OUTPUT_PIN 14  // Use GPIO 13 for toggling
#define GPIO_OUTPUT_PIN_SEL (1ULL << GPIO_OUTPUT_PIN)

void app_main(void) {
    // Configure GPIO 13 as an output
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;   // Disable interrupts
    io_conf.mode = GPIO_MODE_OUTPUT;         // Set as output mode
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL; // Pin selection
    io_conf.pull_down_en = 0;                // Disable pull-down mode
    io_conf.pull_up_en = 0;                  // Disable pull-up mode
    gpio_config(&io_conf);                   // Apply the configuration

    int level = 0;  // Initial state

    while (1) {
        // Toggle the output pin
        level = !level;                      // Toggle level (0 -> 1, 1 -> 0)
        gpio_set_level(GPIO_OUTPUT_PIN, level); // Write the new level

        vTaskDelay(10 / portTICK_PERIOD_MS); // Add a 500ms delay
    }
}
