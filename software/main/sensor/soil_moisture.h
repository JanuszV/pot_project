// soil_moisture.h
#ifndef SOIL_MOISTURE_H
#define SOIL_MOISTURE_H

#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <cstdint>

class SoilMoistureSensor {
public:
    // Constructor
    SoilMoistureSensor(adc1_channel_t channel, uint16_t airValue = 3200, uint16_t waterValue = 1400);
    
    // Initialize the ADC
    esp_err_t init();
    
    // Read raw ADC value
    uint16_t readRaw();
    
    // Read soil moisture percentage (0-100%)
    float readPercentage();
    
    // Set calibration values
    void calibrate(uint16_t airValue, uint16_t waterValue);
    
    // Get the last reading
    float getLastReading() const { return lastReading; }

private:
    adc1_channel_t adc_channel;
    uint16_t air_value;    // ADC value in dry air (0% moisture)
    uint16_t water_value;  // ADC value in water (100% moisture)
    float lastReading;     // Last moisture percentage reading
    
    // ADC characteristics for voltage conversion
    esp_adc_cal_characteristics_t adc_chars;
    
    // ADC width and attenuation constants
    static constexpr adc_bits_width_t ADC_WIDTH = ADC_WIDTH_BIT_12;
    static constexpr adc_atten_t ADC_ATTEN = ADC_ATTEN_DB_12;
};

#endif // SOIL_MOISTURE_H