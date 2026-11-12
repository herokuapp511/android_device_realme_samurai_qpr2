/*
 * Copyright (C) 2024 LibreMobileOS Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "CameraProviderExtension.h"

#include <cstdint>
#include <fstream>
#include <string>

#define TORCH_BRIGHTNESS "brightness"
#define TORCH_MAX_BRIGHTNESS "max_brightness"
#define TOGGLE_SWITCH "/sys/devices/platform/soc/c440000.qcom,spmi/spmi-0/spmi0-05/c440000.qcom,spmi:qcom,pm8150l@5:qcom,leds@d300/leds/led:switch_2/brightness"
#define TOGGLE_SWITCH_ALT "/sys/devices/platform/soc/c440000.qcom,spmi/spmi-0/spmi0-05/c440000.qcom,spmi:qcom,pm8150l@5:qcom,leds@d300/leds/led:switch_3/brightness"

static std::string kTorchLedPaths[] = {
        "/sys/devices/platform/soc/c440000.qcom,spmi/spmi-0/spmi0-05/c440000.qcom,spmi:qcom,pm8150l@5:qcom,leds@d300/leds/led:torch_0",
        "/sys/devices/platform/soc/c440000.qcom,spmi/spmi-0/spmi0-05/c440000.qcom,spmi:qcom,pm8150l@5:qcom,leds@d300/leds/led:torch_1",
};

/**
 * Write value to path and close file.
 */
template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
}

/**
 * Read value from the path and close file.
 */
template <typename T>
static T get(const std::string& path, const T& def) {
    std::ifstream file(path);
    T result;

    file >> result;
    return file.fail() ? def : result;
}

bool supportsTorchStrengthControlExt() {
    return true;
}

bool supportsSetTorchModeExt() {
    return false;
}

int32_t getTorchDefaultStrengthLevelExt() {
    return 100;
}

int32_t getTorchMaxStrengthLevelExt() {
    // 200 (out of 500) is a safe and bright max continuous level for dual LEDs
    return 200;
}

int32_t getTorchStrengthLevelExt() {
    // Both torch LEDs receive the same level; read from channel 0.
    auto node = kTorchLedPaths[0] + "/" + TORCH_BRIGHTNESS;
    return get(node, 0);
}

void setTorchStrengthLevelExt(int32_t torchStrength, bool enabled) {
    set(TOGGLE_SWITCH, 0);
    set(TOGGLE_SWITCH_ALT, 0);
    for (auto& path : kTorchLedPaths) {
        auto node = path + "/" + TORCH_BRIGHTNESS;
        set(node, enabled ? torchStrength : 0);
    }
    if (enabled) {
        set(TOGGLE_SWITCH, 255);
        set(TOGGLE_SWITCH_ALT, 255);
    }
}

void setTorchModeExt(bool enabled) {
    int32_t strength = getTorchDefaultStrengthLevelExt();
    setTorchStrengthLevelExt(enabled ? strength : 0, enabled);
}
