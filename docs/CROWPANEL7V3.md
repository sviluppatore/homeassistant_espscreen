# CrowPanel Basic 7-inch V3.0, experimental bench port

This port targets Elecrow DIS08070H with PCB marking V3.0, not CrowPanel Advanced.
It is a manual bench build. It is not registered in boards.yaml, the generated entry
files or Screen Manager's New screen list. No release or version change is intended.

## Initial scope

- ESP32-S3 with a 4 MB flash target and octal PSRAM at 80 MHz.
- 800 x 480 RGB display with explicit manufacturer pins and timings.
- GT911 polling on SDA GPIO19 and SCL GPIO20, initially at address 0x5D.
- Backlight PWM on GPIO2 at 1220 Hz.
- Tessera's standard look, 4 x 4 landscape and 2 x 7 portrait starting grids.
- Tessera's 4 MB flash-layout component and existing shared UI.

Audio, microphones, external sensors, SD storage and battery monitoring are not
configured. Camera/artwork features are not included in this first bench profile.
The 4 MB flash target can limit features even with 8 MB of PSRAM. Actual firmware
size and free heap must be measured, not inferred from this configuration.

## Hardware source and uncertainty

The baseline is Elecrow's manufacturer example:
https://github.com/Elecrow-RD/CrowPanel-7.0-HMI-ESP32-Display-800x480/blob/master/example/V3.0/ESPHome/basic-hmi-70inch.yaml

The RGB configuration is translated to ESPHome's mipi_rgb RPI model, not the
CROWPANEL-ADVANCE-7 preset. Its pixel clock is initially 15 MHz. Blue pins follow
the manufacturer sequence [15, 5, 6, 7, 4]; community examples differ, so verify
blue gradients and grayscale on the hardware before changing that order.

A separate crowpanel_v3 component runs at setup priority 700, after I2C and before
the GT911, which is explicitly set to priority 500. It follows the manufacturer's PCA9557 writes at 0x18:
configuration 0x00, output 0x00, wait 20 ms, output 0x01, wait 100 ms,
configuration 0x02. It checks each write and reports failures. It does not replace
Tessera's existing on_boot actions. Successful writes do not prove working touch.

Some community reports recommend leaving the expander untouched. The reset can be
disabled with CROWPANEL_RESET_TOUCH: "false". Address can be overridden with
CROWPANEL_TOUCH_ADDRESS: "0x14" when diagnostics justify that choice. Change one
setting at a time and compare cold power-on with reset-button starts. Do not treat
an I2C scan alone as acceptance, since it can happen before touch initialization.

## Build preparation

Use the crowpanel-7-v3 branch of this fork. Use an ESPHome version compatible with
the fork's packages/core.yaml, initially 2026.9.x. The full ESPHome schema check,
C++ compilation and hardware acceptance are still required.

Create diagnostics/secrets.yaml locally with these four keys:

```yaml
wifi_ssid: "YOUR_WIFI_SSID"
wifi_password: "YOUR_WIFI_PASSWORD"
api_encryption_key: "YOUR_OWN_BASE64_32_BYTE_KEY"
ota_password: "YOUR_OWN_OTA_PASSWORD"
```

The API-key text above is a placeholder, not a valid key. Generate a fresh key with
your ESPHome installation. Do not reuse the manufacturer's example credentials or
commit secrets. The bench entry has no fallback access point.

From the repository root, validate and compile, without uploading:

```sh
esphome config diagnostics/crowpanel7v3.yaml
esphome compile diagnostics/crowpanel7v3.yaml
```

The bench entry deliberately fetches components and fonts from this fork and
branch. Board and core YAML are included from the checkout. Ensure the checkout
matches the committed branch before interpreting build results.

## Hardware acceptance

Before any flash, back up firmware/settings that must be preserved and identify
the intended USB serial port. The first flash uses a new partition table. No
upload command is included here to avoid choosing a device without confirmation.

The bench starts on Tessera's touch-test page (CALIBRATION_ON_BOOT: "true").

1. Check serial boot logs for detected flash/PSRAM, expander errors and GT911 setup.
2. Verify touch near all corners and along a drag, including after a full power cycle.
3. Verify colors, grayscale, orientation and stability while Wi-Fi is active.
4. Test backlight levels and recovery from a dark screen.
5. Set CALIBRATION_ON_BOOT to "false" only after the touch test passes.
6. Then test layouts, page navigation, portrait orientation, heap and firmware size.

## Next integration milestone

After the bench works, use tools/new_board.py and the project's adding-board guide
to register the board and generate catalogue metadata, package/checkout entries
and override tests. Add the normal Unreleased changelog entry at integration.
Do not edit generated files by hand or claim release readiness from this prototype.
