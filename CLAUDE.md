# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is **Marlin 2.1.1** firmware customized for the **PandaPlacer A1** using the Pandaplacer MotionController V2.x board family (PPMC V2.x). The target MCU is an STM32F103RCT6 (ARM Cortex-M3, 256KB Flash, 64KB SRAM). The primary PlatformIO environment is `PANDAPLACER_MC_V2_0`; the board info string in the pins file reports `Pandaplacer MotionController V2.1`.

## Build Commands

```bash
# Build firmware for PandaPlacer
pio run -e PANDAPLACER_MC_V2_0

# Upload firmware (HID protocol by default)
pio run -e PANDAPLACER_MC_V2_0 -t upload

# Clean build artifacts
pio run -e PANDAPLACER_MC_V2_0 -t clean

# Run build tests (compile verification only, no unit tests)
make tests-single-local TEST_TARGET=PANDAPLACER_MC_V2_0
make tests-all-local
make tests-single-local-docker TEST_TARGET=PANDAPLACER_MC_V2_0
```

Build output goes to `.pio/build/PANDAPLACER_MC_V2_0/`. Compiler is configured with `-fmax-errors=5`, so only the first 5 errors are shown.

## Key Configuration Files

- **`Marlin/Configuration.h`** — Primary hardware/feature configuration (board, baud rates, machine name, axes, sensors)
- **`Marlin/Configuration_adv.h`** — Advanced features, driver tuning, thermocouple settings
- **`Marlin/src/pins/stm32f1/pins_PANDAPLACER_MC_V2_0.h`** — All pin assignments for the custom board
- **`platformio.ini`** — Build system root; sets default env and includes platform-specific ini files
- **`ini/stm32f1.ini`** — STM32F1 platform configs including the `PANDAPLACER_MC_V2_0` environment

## Architecture

### HAL Abstraction
All hardware-specific code lives under `Marlin/src/HAL/`. For this board, the relevant HAL is `STM32F1/`. The HAL provides platform-independent interfaces for timers, EEPROM (flash-emulated), serial, and ADC.

### Feature Compilation
Features are conditionally compiled via `#define` flags in `Configuration.h` and `Configuration_adv.h`. The PlatformIO source filter in `platformio.ini` explicitly excludes unused HAL backends and LCD/feature directories to reduce build size. When adding a new feature, you may need to update the source filter.

### G-code Command Structure
G-code commands are implemented as individual files under `Marlin/src/gcode/`, organized by category (motion, temp, config, calibrate, etc.). Each command is a function registered via the `GcodeSuite` class.

### PandaPlacer-Specific Hardware
- **6 Marlin motion channels configured:** X, Y, Z, I (A nozzle rotation), J (B nozzle rotation), plus E0/E1 extruder channels used for Marlin's multi-extruder/tool model rather than real extruder step pins
- **5 physical stepper axes:** X, Y, Z, I/A, J/B. I and J are rotational axes named `A` and `B` in G-code.
- **3 serial ports:** USB CDC at 500,000 bps (`SERIAL_PORT -1`), Serial2 and Serial3 at 115,200 bps. `M575` baud-rate changes are enabled.
- **Valve/pump outputs** mapped as Marlin fans: PC9, PC8, PC7, PC6
- **Vacuum sensors** mapped through Marlin temperature channels: PC3, PC2, PC1 use repurposed thermistor table 20
- **Timers:** TIM1 (serial), TIM6 (temp), TIM7 (stepper), TIM2 (servo), TIM8 (tone)
- **EEPROM:** Flash-emulated (2KB pages at end of flash)
- **Upload:** HID bootloader by default; st-link and serial are commented alternatives in `ini/stm32f1.ini`

### Changed Files from Stock Marlin 2.1.1
Compared with the stock `2.1.1` tag, this fork changes:

- **`platformio.ini`** — makes `PANDAPLACER_MC_V2_0` the default environment.
- **`ini/stm32f1.ini`** — adds the `PANDAPLACER_MC_V2_0` environment for `genericSTM32F103RC`, variant `MARLIN_F103Rx`, USB CDC (`USBCON`, `USBD_USE_CDC`), `ENABLE_HWSERIAL2`, ST-Link debugging, and HID upload by default.
- **`ini/features.ini`** — updates Adafruit NeoPixel dependency from `~1.8.0` to `~1.10.6`.
- **`Marlin/src/core/boards.h`** and **`Marlin/src/pins/pins.h`** — register `BOARD_PANDAPLACER_MC_V2_0` (`4099`) and include the PandaPlacer STM32F1 pins file.
- **`Marlin/src/pins/stm32f1/pins_PANDAPLACER_MC_V2_0.h`** — defines the complete PPMC V2.x pin map, timers, disabled JTAG, and flash EEPROM emulation.
- **`Marlin/Configuration.h`** — selects the PandaPlacer board, machine name, serial ports, TMC standalone drivers, two extruders for tool semantics, vacuum sensor channels, A/B rotational axes, travel limits, homing setup, EEPROM, NeoPixels, speaker, and one editable servo.
- **`Marlin/Configuration_adv.h`** — tunes fan PWM, serial buffers/flow control, emergency parser, homing bump/backoff, step timing, saved positions, I2C bus support, direct pin control/debugging, startup commands, and soft reset behavior.
- **`Marlin/src/module/thermistor/thermistor_20.h`** and **`thermistors.h`** — repurpose sensor table 20 for vacuum pressure readings and enable filtered ADC handling.
- **`Marlin/src/MarlinCore.cpp`** — changes kill behavior from waiting forever to delaying briefly and calling `HAL_reboot()`.
- **`Marlin/Version.h`** — sets PandaPlacer distribution date and website URL.
- **`Marlin/src/gcode/host/M115.cpp`** — contains a commented-out `F_CPU` report line; it does not change runtime output unless uncommented.

### Pinout Notes

The pins file disables JTAG so JTAG pins can be used as GPIO, while leaving SWD enabled for debug access. `NC_PIN` is assigned to `PD2` as a reserved no-connect placeholder for unused Marlin-required pins.

Stepper pins:

| Axis | Step | Dir | Enable | Notes |
| --- | --- | --- | --- | --- |
| X | PC10 | PB15 | PA8 | Linear X |
| Y | PB4 | PB5 | PC11 | Linear Y |
| Z | PB13 | PB14 | PB12 | Linear Z |
| I / A | PC5 | PC4 | PB0 | Rotational A nozzle axis |
| J / B | PB1 | PB2 | PB0 | Rotational B nozzle axis; shares enable with A |
| E0/E1 | PD2 placeholder | PD2 placeholder | PD2 placeholder | Configured for Marlin tool/extruder logic only; no real step/dir/enable pins |

Endstop pins:

| Signal | Pin | Use |
| --- | --- | --- |
| X_MIN | PC15 | X homes to min |
| Y_MAX | PC13 | Y homes to max and is also aliased as I_MIN/J_MIN |
| Z_MAX | PC14 | Z homes to max |
| I_MIN | PC13 | Shares Y_MAX input |
| J_MIN | PC13 | Shares Y_MAX input |

Output and peripheral pins:

| Function | Pin | Marlin mapping |
| --- | --- | --- |
| A valve | PC9 | `FAN_PIN` / fan 0 |
| B valve | PC8 | `FAN1_PIN` / fan 1 |
| A pump | PC7 | `FAN2_PIN` / fan 2 |
| B pump | PC6 | `FAN3_PIN` / fan 3 |
| Servo 0 | PC12 | `SERVO0_PIN`, usable with M280/M281/M282 |
| NeoPixel strip 1 | PA0 | 4 RGB LEDs |
| NeoPixel strip 2 | PA1 | 8 RGB LEDs |
| Beeper | PC0 | Speaker/tone output |
| I2C SCL/SDA | PB10/PB3 | Experimental I2C bus enabled |
| SPI | SPI1 | Default SPI bus |
| RS485 DE | PA15 | Direction-enable pin; requires JTAG disabled |

Vacuum sensing:

| Sensor | Pin | Marlin channel | Notes |
| --- | --- | --- | --- |
| A vacuum sense | PC3 | `TEMP_SENSOR_0 20` | Table 20 maps ADC to vacuum magnitude |
| B vacuum sense | PC2 | `TEMP_SENSOR_1 20` | Same table |
| Tank vacuum sense | PC1 | `TEMP_SENSOR_BED 20` | Uses the bed temperature channel |

Repurposed table 20 is no longer the stock Pt100/INA826 table. It maps roughly `OV(40) -> 0`, `OV(622) -> 67`, and `OV(932) -> 100`, with comments indicating `-67 kPa` vacuum at `2.0V`.

`HEATER_0_PIN`, `HEATER_1_PIN`, and `HEATER_BED_PIN` are disabled by default. The commented `VAC_TANK` option would assign `HEATER_BED_PIN` to `PC10` (`EXMOS1`), but `PC10` is already `X_STEP_PIN`; do not enable `VAC_TANK` without first resolving that pin conflict.

### Motion and G-code Behavior

- G-code uses `A` and `B` for the fourth and fifth axes (`AXIS4_NAME 'A'`, `AXIS5_NAME 'B'`) while Marlin internally refers to them as `I` and `J`.
- Default steps are `{ X=160, Y=160, Z=200, A=17.7778, B=17.7778, E=10 }`.
- Travel limits are `X 0..335`, `Y 0..352`, `Z -33..33`, and `A/B -3600..3600` degrees. A/B software endstops are disabled.
- Homing uses X min, Y max, Z max, and A/B min semantics. A/B manual home positions are `0`.
- `NO_MOTION_BEFORE_HOMING` is enabled, so movement is blocked until homing.
- Extrusion safeguards and thermal protection are disabled because the temperature/heater/extruder channels are repurposed for PandaPlacer control rather than hotend printing.
- `STARTUP_COMMANDS` plays a short tone and runs `M42 I P15 S0`; this appears intended to initialize a raw pin state at startup.

## Y Position-Loss Investigation — 2026-07-19

Machine symptom: after a job error (e.g. No Part / No Vacuum) and resume, Y can be "lost" and repeatedly drives into one Y hard stop (which end depends on the offset direction) until killed. Mechanical slip at the carriage has been ruled out by the operator; machine runs at 25% speed.

Audit results (this source tree):

- **The homed-axis motion path is watertight.** Every `G0/G1` is clamped in *native* space by `apply_motion_limits` (`src/module/motion.cpp:1391`, per-axis clamps ~line 940). Workspace offsets are enabled (`NO_WORKSPACE_OFFSETS` commented out), so `G92` only changes the reporting shift (`position_shift`), never the clamp window — a bogus `G92` cannot defeat soft endstops. `M211`/`M114` report in the *shifted* frame (e.g. window `0.845..352.845` after OpenPnP visual-homing `G92`); `Count` values are native steps.
- **Soft endstops do not exist on unhomed axes.** Each clamp is guarded by `axis_was_homed()`. After any reset/reboot the only protection is `NO_MOTION_BEFORE_HOMING` refusing moves.
- **Refused moves look like success to a host.** `homing_needed_error` emits `echo:Home XY first` + `ok` (`motion.cpp:1474`); OpenPnP's GcodeDriver treats that as a completed move. A silently-rebooted controller therefore desyncs from OpenPnP without any error.
- **Prime suspect: the fork's kill auto-reboot.** The only functional code change vs stock 2.1.1 is `minkill()` in `MarlinCore.cpp`: instead of halting forever, it delays ~5 s then calls `HAL_reboot()`. Any `kill()` (homing failure, watchdog, M112, thermal fault) silently reboots: position zeroed, axes unhomed, **no soft endstops**. If the *flashed* binary lacks `NO_MOTION_BEFORE_HOMING`, subsequent OpenPnP moves execute unclamped in a zeroed frame → hard-stop crashes. **Pending test:** reset the board, do NOT home, send `G1 Y10 F600`. If it moves, the flashed build has no homing gate (smoking gun). If it answers `echo:Home XYZ first`, the gate is present.
- **`HOME_AFTER_DEACTIVATE` is now enabled** (`Configuration.h:1661`, previously off). With it off, `M84`/stepper-disable marked axes *untrusted* but `axes_should_home()` keyed off `axis_was_homed` (`motion.cpp:1456`), so motion resumed against possibly-stale position. Verified safe to enable: all `DISABLE_*` are `false` (SanityCheck passes), and the OpenPnP `HOME_COMMAND`'s `G1 X95 Y334` before `G28 Z` still works because the `G0/G1` homing check only masks axes present in the command (`src/gcode/motion/G0_G1.cpp:51`). Consequences: re-home required after any `M84` (incl. OpenPnP DISABLE_COMMAND, the `M84 X Y Z A B` actuator) and after the 1800 s stepper inactivity timeout. **Needs rebuild + reflash.**
- Ruled out: spurious endstop triggers truncating moves (`ENDSTOPS_ALWAYS_ON_DEFAULT` is off, so endstops are ignored outside homing); `G92`/workspace-offset frame attacks (reporting-only); OpenPnP sending out-of-range targets (OpenPnP throws before sending — see openpnp repo notes).
- Upstream refs: Marlin #23095 (axis-trust-on-disable only patched for one vendor; `HOME_AFTER_DEACTIVATE` is the general fix), #25117 (2.1.2 stepper-ISR regression — do not blind-upgrade from 2.1.1).

Suggested instrumentation: make OpenPnP's GcodeDriver treat Marlin's boot banner (`start`) as an error response to surface silent reboots; keep G-code logging on; on the next incident capture `M114` and compare `Count Y` (native belief) against physically measured carriage position before re-homing.

Audible reboot signature: every boot runs `STARTUP_COMMANDS "M300 P120 S1200..."` (`Configuration_adv.h:3794`) — a short high 1200 Hz chirp (distinct from the long low 400 Hz homing beep in the OpenPnP `HOME_COMMAND`). A kill-triggered `HAL_reboot()` therefore chirps ~5 s after the kill; watchdog resets chirp immediately.

**Endstop monitoring gap (2026-07-20):** Marlin only checks hardware endstops during homing; `ENDSTOPS_ALWAYS_ON_DEFAULT` is off (`Configuration_adv.h:721`), so normal moves grind straight through a triggered Y-max switch — matching the observed top-end banging past the B-cam. Host-side mitigation without reflashing: `M120` (enable endstop checking, until `M121`) added to the OpenPnP `CONNECT_COMMAND`. Caveats: PC13 is shared Y_MAX/I_MIN/J_MIN, so A/B rotation moves while sitting on the Y-max switch may false-abort (test after enabling); an endstop abort leaves believed position stale (Marlin thinks the move completed) until re-homed; there is no Y-min switch. `EMERGENCY_PARSER` is enabled (`Configuration_adv.h:2389`) so `M410` quickstop executes instantly mid-move and re-syncs believed position from stepper counts — the preferred software panic stop. **Never use `M112` on this fork** — kill auto-reboots in 5 s and destroys all state.

**2026-07-19 reset test result: auto-reboot theory deprioritized.** Pressing the board reset button proved that any MCU reset is *loud* on this hardware: the native USB CDC re-enumerates, the COM port dies, and OpenPnP immediately raises not-responding dialogs (recovery requires full USB replug + OpenPnP restart; cameras also need re-enumeration). Actual incidents kept a live, responsive connection through error → resume → bang, and no startup chirp was heard — so a mid-session kill-reboot almost certainly did not occur. Remaining prime suspect: **electrical step loss with a live controller** — TMC2209 output dropout while logic stays up. Note the board is partially back-powered via USB 5V (separate logic/motor rails): a motor-rail sag leaves Marlin counting while the motor freewheels silently. Incidents are bracketed by vacuum pump/valve switching (inductive load steps); check whether pumps/valves share the stepper supply rail and measure rail sag during pump switching. `HOME_AFTER_DEACTIVATE` + OpenPnP auto-home-on-enable remain in place as hygiene (confirmed working in the same test).

## Code Style

Enforced via `.editorconfig`:
- 2-space indentation for C/C++/header files
- Unix line endings (LF)
- UTF-8 encoding
- No trailing whitespace, final newline required

## Pre-build Scripts

PlatformIO runs these Python scripts before compilation (defined in `platformio.ini`):
1. `buildroot/share/PlatformIO/scripts/configuration.py` — Validates `Configuration.h`
2. `buildroot/share/PlatformIO/scripts/common-dependencies.py` — Resolves feature-conditional library dependencies
3. `buildroot/share/PlatformIO/scripts/common-cxxflags.py` — Sets C++ flags
4. `buildroot/share/PlatformIO/scripts/preflight-checks.py` — Sanity checks before build
