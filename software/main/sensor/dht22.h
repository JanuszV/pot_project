// dht22.h
#ifndef DHT22_H
#define DHT22_H

#include <driver/gpio.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cstdint>

class DHT22 {
public:
    enum error{
        OK,
        ERR_TIMING_80,
        ERR_TIMING_50,
        ERR_TIMING_BITS,
        ERR_CRC
    };
    // Constructor
    DHT22(gpio_num_t pin);
    
    // Read temperature in Celsius
    float readTemperature();
    
    // Read humidity percentage
    float readHumidity();
    
    // Read both temperature and humidity
    bool readSensor();
    
    // Get last temperature reading
    float getTemperature() const { return temperature; }
    
    // Get last humidity reading
    float getHumidity() const { return humidity; }

private:
    gpio_num_t dht_pin;
    float temperature;
    float humidity;
    
    // Timing constants
    static constexpr uint8_t MAX_TIMINGS = 85;
    static constexpr uint16_t TIMEOUT_US = 100;
    
    // Internal methods for sensor communication
    void startSignal();
    bool waitForResponse();
    bool readData(uint8_t data[5]);
    uint64_t getTimeUs();
    void setGpioOutput();
    void setGpioInput();
};

#endif // DHT22_H