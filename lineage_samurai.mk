#
# Copyright (C) 2020 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

$(call inherit-product, device/realme/samurai/device.mk)

# Inherit some common Lineage stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Device identifier. This must come after all inclusions.
PRODUCT_BRAND := realme
PRODUCT_DEVICE := samurai
PRODUCT_MANUFACTURER := realme
PRODUCT_MODEL := realme X2 Pro
PRODUCT_NAME := lineage_samurai

BUILD_FINGERPRINT := realme/RMX1931/RMX1931L1:11/RKQ1.200928.002/1625457120608:user/release-keys

PRODUCT_BUILD_PROP_OVERRIDES += \
    BuildDesc="RMX1931-user 11 RKQ1.200928.002 1625457120608 release-keys" \
    BuildFingerprint=realme/RMX1931/RMX1931L1:11/RKQ1.200928.002/1625457120608:user/release-keys \
    DeviceName=RMX1931L1 \
    DeviceProduct=RMX1931 \
    SystemDevice=RMX1931L1 \
    SystemName=RMX1931

PRODUCT_GMS_CLIENTID_BASE := android-oppo

# RisingOS stuff
# Lunch banner maintainer variable
RISING_MAINTAINER="NAYEM"

# Chipset/Maintainer properties (ro.rising.chipset/ro.rising.maintainer)
# Set RISING_MAINTAINER for version control
# (Optional if builder is setting properties via init_<device>.cpp)
PRODUCT_BUILD_PROP_OVERRIDES += \
    RisingChipset="Snapdragon™ 855+" \
    RisingMaintainer="NAYEM"

RISING_MAINTAINER := NAYEM

# Disable/enable blur support, false by default
TARGET_ENABLE_BLUR := true

# CORE build flags
WITH_GMS := true
TARGET_USES_PICO_GAPPS := true
