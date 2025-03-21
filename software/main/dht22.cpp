// dht22.cpp
#include "sensor/dht22.h"
#include <esp_timer.h>
#include <cstring>

#define TIMEOUT_US 1000 // Adjust timeout as needed

DHT22::DHT22(gpio_num_t pin) : dht_pin(pin), temperature(0.0f), humidity(0.0f) {
    // Configure GPIO
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << dht_pin);
    io_conf.mode = GPIO_MODE_OUTPUT; 
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
    
    // Set initial state high
    gpio_set_level(dht_pin, 1);
}

float DHT22::readTemperature() {
    if (readSensor()) {
        return temperature;
    }
    return -999.0f; // Error value
}

float DHT22::readHumidity() {
    if (readSensor()) {
        return humidity;
    }
    return -999.0f; // Error value
}

bool DHT22::readSensor() {
    uint8_t data[5] = {0};
    
    // Send start signal
    startSignal();
    
    // Check for sensor response
    if (!waitForResponse()) {
        return false;
    }
    
    // Read the data
    if (!readData(data)) {
        return false;
    }
    
    // Verify checksum
    if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        return false;
    }
    
    // Convert the data
    humidity = ((data[0] << 8) | data[1]) / 10.0f;
    
    // Check if temperature is negative
    if (data[2] & 0x80) {
        temperature = -((((data[2] & 0x7F) << 8) | data[3]) / 10.0f);
    } else {
        temperature = ((data[2] << 8) | data[3]) / 10.0f;
    }
    
    return true;
}

void DHT22::startSignal() {
    // Set pin to output
    setGpioOutput();
    
    // Pull down for at least 1ms (DHT22 start signal)
    gpio_set_level(dht_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(2)); // 2ms for safety
    
    // Release the line and wait for response
    gpio_set_level(dht_pin, 1);
    esp_rom_delay_us(40); // 40us delay
    
    // Set pin to input to read response
    setGpioInput(); // Ensure this is called
}

bool DHT22::waitForResponse() {
    // Wait for pin to go low (response from DHT22)
    uint64_t startTime = getTimeUs();
    while (gpio_get_level(dht_pin) == 1) {
        if (getTimeUs() - startTime > TIMEOUT_US) {
            return false; // Timeout waiting for response
        }
    }
    
    // Wait for pin to go high (DHT22 preparing to send data)
    startTime = getTimeUs();
    while (gpio_get_level(dht_pin) == 0) {
        if (getTimeUs() - startTime > TIMEOUT_US) {
            return false; // Timeout waiting for response
        }
    }
    
    // Wait for pin to go low again (ready to start sending data)
    startTime = getTimeUs();
    while (gpio_get_level(dht_pin) == 1) {
        if (getTimeUs() - startTime > TIMEOUT_US) {
            return false; // Timeout waiting for response
        }
    }
    
    return true;
}

bool DHT22::readData(uint8_t data[5]) {
    // Read 40 bits (5 bytes) of data
    for (int i = 0; i < 5; i++) {
        data[i] = 0;
        for (int j = 7; j >= 0; j--) {
            uint64_t highTime;
            
            // Wait for pin to go high (end of data bit start)
            uint64_t startTime = getTimeUs();
            while (gpio_get_level(dht_pin) == 0) {
                if (getTimeUs() - startTime > TIMEOUT_US) {
                    return false; // Timeout waiting for response
                }
            }
            
            // Measure how long the pin stays high (determines 0 or 1)
            startTime = getTimeUs();
            while (gpio_get_level(dht_pin) == 1) {
                if (getTimeUs() - startTime > TIMEOUT_US) {
                    return false; // Timeout waiting for response
                }
            }
            
            highTime = getTimeUs() - startTime;
            
            // If high time is more than ~50us, it's a '1' bit
            if (highTime > 40) {
                data[i] |= (1 << j);
            }
        }
    }
    
    return true;
}

uint64_t DHT22::getTimeUs() {
    return esp_timer_get_time();
}

void DHT22::setGpioOutput() {
    gpio_set_direction(dht_pin, GPIO_MODE_OUTPUT);
}

void DHT22::setGpioInput() {
    gpio_set_direction(dht_pin, GPIO_MODE_INPUT);
}