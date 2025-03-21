// soil_moisture.cpp
#include "sensor/soil_moisture.h"

SoilMoistureSensor::SoilMoistureSensor(adc1_channel_t channel, uint16_t airValue, uint16_t waterValue)
    : adc_channel(channel), air_value(airValue), water_value(waterValue), lastReading(0.0f) {
}

esp_err_t SoilMoistureSensor::init() {
    // Configure ADC
    esp_err_t ret = adc1_config_width(ADC_WIDTH);
    if (ret != ESP_OK) {
        return ret;
    }
    
    ret = adc1_config_channel_atten(adc_channel, ADC_ATTEN);
    if (ret != ESP_OK) {
        return ret;
    }
    
    // Characterize ADC for more accurate voltage readings
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN, ADC_WIDTH, 1100, &adc_chars);
    
    return ESP_OK;
}

uint16_t SoilMoistureSensor::readRaw() {
    // Read ADC value
    uint32_t adc_reading = 0;
    
    // Take multiple samples to reduce noise
    constexpr int NO_OF_SAMPLES = 10;
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        adc_reading += adc1_get_raw(adc_channel);
    }
    
    // Calculate average
    adc_reading /= NO_OF_SAMPLES;
    
    return static_cast<uint16_t>(adc_reading);
}

float SoilMoistureSensor::readPercentage() {
    uint16_t rawValue = readRaw();
    
    // Ensure values are within expected range
    if (rawValue >= air_value) {
        lastReading = 0.0f;
    } else if (rawValue <= water_value) {
        lastReading = 100.0f;
    } else {
        // Map the value from 0-100%
        // Note: Lower ADC values mean higher moisture, which is why we invert the calculation
        lastReading = 100.0f * (air_value - rawValue) / (air_value - water_value);
    }
    
    return lastReading;
}

void SoilMoistureSensor::calibrate(uint16_t airValue, uint16_t waterValue) {
    air_value = airValue;
    water_value = waterValue;
}