#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace crowpanel_v3 {

class CrowPanelV3 : public Component, public i2c::I2CDevice {
 public:
  void set_reset_touch(bool enabled) { reset_touch_ = enabled; }
  // After the I2C bus, before the GT911 explicitly configured at priority 500.
  float get_setup_priority() const override { return 700.0f; }

  void setup() override {
    if (!reset_touch_) {
      ESP_LOGW("crowpanel_v3", "Manufacturer touch reset disabled for comparison");
      return;
    }
    // Elecrow V3 example: both lines low, release RST, then make INT an input.
    // These full-register values are specific to DIS08070H V3.0, not other panels.
    if (!write_step_(0x03, 0x00) || !write_step_(0x01, 0x00)) return;
    delay(20);
    if (!write_step_(0x01, 0x01)) return;
    delay(100);
    if (!write_step_(0x03, 0x02)) return;
    ESP_LOGI("crowpanel_v3", "Manufacturer V3 touch reset completed; verify GT911 next");
  }

  void dump_config() override {
    ESP_LOGCONFIG("crowpanel_v3", "Experimental CrowPanel DIS08070H V3.0 initialization");
    ESP_LOGCONFIG("crowpanel_v3", "  Touch reset: %s", reset_touch_ ? "enabled" : "disabled");
    LOG_I2C_DEVICE(this);
  }

 protected:
  bool reset_touch_{true};
  bool write_step_(uint8_t reg, uint8_t value) {
    if (this->write_byte(reg, value)) return true;
    ESP_LOGE("crowpanel_v3", "Expander write failed: register 0x%02X", reg);
    this->mark_failed();
    return false;
  }
};

}  // namespace crowpanel_v3
}  // namespace esphome
