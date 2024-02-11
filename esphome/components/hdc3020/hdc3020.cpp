#include "hdc3020.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace hdc3020 {

static const char *const TAG = "hdc3020";

void HDC3020Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up HDC3020...");

  const uint8_t data[2] = { 0x24, 0x0 };

  if (this->write(data, 2) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "HDC3020 initialization failed");
    this->status_set_warning();
  }

  delay(20);

}
void HDC3020Component::dump_config() {
  ESP_LOGCONFIG(TAG, "HDC3020:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with HDC3020 failed!");
  }
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Temperature", this->temperature_);
  LOG_SENSOR("  ", "Humidity", this->humidity_);
}
void HDC3020Component::update() {

  const uint8_t data[2] = { 0x24, 0x0 };
  if (this->write(data, 2) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "HDC3020 initialization failed");
    this->status_set_warning();
  }
  delay(20);

  uint8_t buffer[6];
  if (this->read((uint8_t *)&buffer, 6) != i2c::ERROR_OK) {
    this->status_set_warning();
    return;
  }

  uint16_t raw_temp = i2c::i2ctohs(*(uint16_t *)(buffer));
  float temp = ((float)raw_temp / 65535) * 175 - 45;
  this->temperature_->publish_state(temp);

  uint16_t raw_humidity = i2c::i2ctohs(*(uint16_t *)(buffer + 3));
  float humidity = ((float)raw_humidity / 65535) * 100;
  this->humidity_->publish_state(humidity);

  ESP_LOGD(TAG, "Got temperature=%.1f°C (raw=%d) humidity=%.1f%% (raw=%d)", temp, raw_temp, humidity, raw_humidity);
  this->status_clear_warning();
}
float HDC3020Component::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace hdc3020
}  // namespace esphome
