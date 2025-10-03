#ifndef PRESSURESENSOR_H
#define PRESSURESENSOR_H

#include <ADS1X15.h>
#include <Arduino.h>

constexpr int PRESSURE_READ_INTERVAL_MS = 30;

#ifdef USE_ADS_1015
// ADS1015 is 12-bit and has different data rates but otherwise compatible with the ADS1115 code here
// TODO: potential auto-detect method using the sample rate here:
// https://github.com/pimoroni/ads1015-python/blob/64bc296b506ca7144414cd0ba6d0b6adeabdb007/ads1015/__init__.py#L243
constexpr float ADC_MAX_VALUE = 2047.0f;
constexpr uint8_t DATA_RATE_128_SPS = ADS1X15_DATARATE_0;
#define ADS1X15Impl ADS1015
#else
constexpr float ADC_MAX_VALUE = 32767.0f;
constexpr uint8_t DATA_RATE_128_SPS = ADS1X15_DATARATE_4;
#define ADS1X15Impl ADS1115
#endif

constexpr float ADC_STEP = ADS1x15_GAIN_6144MV_FSRANGE_V / ADC_MAX_VALUE;

using pressure_callback_t = std::function<void(float)>;

class PressureSensor {
  public:
    PressureSensor(uint8_t sda_pin, uint8_t scl_pin, const pressure_callback_t &callback, float pressure_scale = 16.0f,
                   float voltage_floor = 0.5, float voltage_ceil = 4.5);
    ~PressureSensor() = default;

    void setup();
    void loop();
    inline float getPressure() const { return _pressure; };
    inline float getRawPressure() const { return _raw_pressure; };
    void setScale(float pressure_scale);

  private:
    uint8_t _sda_pin;
    uint8_t _scl_pin;
    float _pressure = 0.0f;
    float _raw_pressure = 0.0f;
    float _pressure_adc_range;
    float _pressure_scale;
    float _pressure_step;
    int16_t _adc_floor;
    ADS1X15Impl *ads = nullptr;
    pressure_callback_t _callback;
    xTaskHandle taskHandle;

    const char *LOG_TAG = "PressureSensor";
    static void loopTask(void *arg);
};

#endif // PRESSURESENSOR_H
