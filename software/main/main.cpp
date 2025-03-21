#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "sensor/dht22.h"
#include "sensor/soil_moisture.h"


static const char *TAG1 = "MAIN";

static const char *TAG2 = "DHT22_EXAMPLE";

static const char *TAG3 = "SOIL_MOISTURE_EXAMPLE";

// Define DHT22 GPIO pin
static constexpr gpio_num_t DHT_PIN = GPIO_NUM_21;  

// ADC1_CHANNEL_4 = GPIO_32
static constexpr adc1_channel_t SOIL_MOISTURE_CHANNEL = ADC1_CHANNEL_4;

// This is the critical part - add extern "C" declaration
extern "C" void app_main(void) {
    ESP_LOGI(TAG1, "Starting application...");

    DHT22 dht22(DHT_PIN);
    
    SoilMoistureSensor soilSensor(SOIL_MOISTURE_CHANNEL);

    // Your initialization code here
    esp_err_t ret = soilSensor.init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG3, "Failed to initialize soil moisture sensor: %d", ret);
        return;
    }
    
    ESP_LOGI(TAG3, "Soil moisture sensor initialized on ADC channel %d", SOIL_MOISTURE_CHANNEL);
    
    while (1) {
        // Main loop code
        uint16_t rawValue = soilSensor.readRaw();
        
        // Read moisture percentage
        float moisturePercentage = soilSensor.readPercentage();
        // Read sensor data
        if (dht22.readSensor()) {
            float temperature = dht22.getTemperature();
            float humidity = dht22.getHumidity();
            
            ESP_LOGI(TAG2, "Temperature: %.1f°C, Humidity: %.1f%%", temperature, humidity);
        } else {
            ESP_LOGE(TAG2, "Failed to read from DHT22 sensor");
        }

        ESP_LOGI(TAG3, "Soil Moisture - Raw: %d, Percentage: %.1f%%", rawValue, moisturePercentage);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}