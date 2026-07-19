# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is **Marlin 2.1.1** firmware customized for the **Pandaplacer MotionController V2.0** (PPMC V2.0), an STM32F103RCT6-based (ARM Cortex-M3, 256KB Flash, 64KB SRAM) pick-and-place motion controller. The primary PlatformIO environment is `PANDAPLACER_MC_V2_0`.

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
- **5 axes:** X, Y, Z, I (A-axis), J (B-axis)
- **3 serial ports:** USB CDC at 500,000 bps (`SERIAL_PORT -1`), Serial2 and Serial3 at 115,200 bps
- **Valve outputs** mapped as "fans": PC9, PC8, PC7, PC6
- **Temperature sensors:** PC3, PC2, PC1 (ADC inputs for thermocouples/thermistors)
- **Timers:** TIM1 (serial), TIM6 (temp), TIM7 (stepper), TIM2 (servo), TIM8 (tone)
- **EEPROM:** Flash-emulated (2KB pages at end of flash)
- **Upload:** HID bootloader by default; st-link and serial are commented alternatives in `ini/stm32f1.ini`

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
