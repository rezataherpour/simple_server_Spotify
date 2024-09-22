#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define GPIO_OUTPUT_PIN 14  // Use GPIO 13 for toggling
#define GPIO_OUTPUT_PIN_SEL (1ULL << GPIO_OUTPUT_PIN)

#define GPIO_INPUT_PIN 45  // Use GPIO 12 for input
#define GPIO_INPUT_PIN_SEL (1ULL << GPIO_INPUT_PIN)

void app_main(void) {
    // Configure GPIO 13 as an output
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;   // Disable interrupts
    io_conf.mode = GPIO_MODE_OUTPUT;         // Set as output mode
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL; // Pin selection
    io_conf.pull_down_en = 0;                // Disable pull-down mode
    io_conf.pull_up_en = 0;                  // Disable pull-up mode
    gpio_config(&io_conf);                   // Apply the configuration


    gpio_config_t io_conf1;
    io_conf1.intr_type = GPIO_INTR_DISABLE;   // Disable interrupts
    io_conf1.mode = GPIO_MODE_INPUT;          // Set as input mode
    io_conf1.pin_bit_mask = GPIO_INPUT_PIN_SEL; // Pin selection
    io_conf1.pull_down_en = 0;                // Disable pull-down
    io_conf1.pull_up_en = 0;                  // Enable pull-up (optional)
    gpio_config(&io_conf1);                   // Apply the configuration

    int level = 0;  // Initial state

    while (1) {
        // Toggle the output pin
        level = !level;                      // Toggle level (0 -> 1, 1 -> 0)
        gpio_set_level(GPIO_OUTPUT_PIN, level); // Write the new level
        int level1 = gpio_get_level(GPIO_INPUT_PIN);
        
        // If the pin reads HIGH, print "hello"
        if (level1 == 1) {
            printf("hello\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Add a 500ms delay
    }
}
