# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Purpose

This is the **Infinity-X** (LineageOS-based) device tree for the **Realme X2 Pro** (codename `samurai`, model RMX1931). It sits between the ROM platform and the device's hardware, providing:

- Hardware configuration for Qualcomm Snapdragon 855+ (msmnile/sm8150)
- Device-local HALs for fingerprint (UDFPS) and sensors subhal; shared oplus HALs for touch/vibrator/livedisplay
- Device-specific overlays, init scripts, power profiles, and SELinux policies
- Build system integration for Infinity-X (`infinity_samurai`)

## Build Commands

This repo is part of a larger AOSP/Infinity-X tree. All build commands are run from the root of that tree, not from this directory.

```bash
# Set up the build environment (also runs vendorsetup.sh clone helpers)
source build/envsetup.sh

# Lunch targets (COMMON_LUNCH_CHOICES)
lunch infinity_samurai-user
# or: infinity_samurai-userdebug / infinity_samurai-eng
# Legacy roomservice may still use: breakfast samurai

# Full build
mka bacon

# Build a specific module (e.g. fingerprint HAL)
mka android.hardware.biometrics.fingerprint@2.3-service.samurai

# Build the kernel
mka dtboimage bootimage

# Vanilla (no GApps): override at lunch
WITH_GAPPS=false lunch infinity_samurai-userdebug

# Extract proprietary blobs from a connected device (run from this directory)
./extract-files.sh

# Extract from a local ROM zip/directory
./extract-files.sh /path/to/rom/dump
```

There are no unit tests in this repository. Validation is done by flashing and booting.

### GApps flags

`infinity_samurai.mk` defaults `WITH_GAPPS := true`. Telephony/full-GApps targets
(`TARGET_BUILD_GOOGLE_TELEPHONY`, `TARGET_SHIPS_FULL_GAPPS`) are set only when
`WITH_GAPPS` is true. `INFINITY_MAINTAINER` uses the make/property-safe form
`NAYEM_&_TITAN` (underscores, not spaces).

## Repo Dependencies

This device tree depends on several other repos that must be present in the Android tree:

| Path | Purpose |
|---|---|
| `kernel/realme/sm8150` | Device kernel source (`TARGET_KERNEL_SOURCE`) |
| `vendor/realme/samurai` | Proprietary blobs extracted via `extract-files.sh` |
| `hardware/oplus` | Shared oplus HALs (touch, vibrator, livedisplay, UDFPS extension, sepolicy) — LineageOS `lineage-23.2` |
| `vendor/lunaris/dolby` | Dolby Atmos packaging (`dolby.mk` / BoardConfigDolby) |
| `hardware/qcom-caf/common` | Common QCOM CAF hardware definitions |
| `device/qcom/sepolicy_vndr` | QCOM vendor SELinux policy base |
| `device/lineage/sepolicy` | LineageOS SELinux policy extensions |
| `packages/apps/RealmeParts` | Device-specific settings app |
| `vendor/infinity` / `vendor/lineage` | ROM common build config |
| `vendor/oplus/camera` | A16 OPlus Camera app stack (12R base); hybrid with F.14 sensor assets in `vendor/realme/samurai` |

`lineage.dependencies` lists `hardware/oplus`, `vendor/lunaris/dolby`, and `vendor/oplus/camera`.
`vendorsetup.sh` auto-clones those when missing (roomservice / envsetup helper).
Prefer the manifest or `vendorsetup.sh` over hand-cloning so paths stay consistent.

### OPlus Camera hybrid (Android 16.2)

- **Toggle:** `TARGET_USES_OPLUS_CAMERA` (default `true`) in `device.mk`.
- **App/props/sepolicy:** `vendor/oplus/camera` → package `com.oplus.camera`.
- **F.14 sensor configs + models + ODM algo libs:** `vendor/realme/samurai` (`camera-f14-hybrid.mk` + full `odm/etc/camera`).
- **Never** overwrite F.14 sensor configs with OnePlus ones.
- **ODM is 256MB** — megvii models optional via `TARGET_ODM_CAMERA_MEGVII := true`.
- Docs: `vendor/oplus/camera/docs/BRINGUP.md`, `DIFFERENCES.md`, `RMX1931_README.md`.

## Key Directories

- **`fingerprint/`** — HIDL 2.3 fingerprint HAL with UDFPS support. Wraps the vendor goodix blob and drives `dimlayer_hbm` via `/sys/kernel/oppo_display` (syshelper AIDL + UDFPS extension for SurfaceFlinger).
- **`sensors/`** — OPLUS sensor subhal (`sensors.oplus.samurai`) loaded by AOSP multihal via `configs/sensors/hals.conf`.
- **`syshelper/`** — OPLUS system helper AIDL service; coordinates UDFPS-related display operations.
- **`als/`** — LineageOS `vendor.lineage.oplus_als` service; feeds corrected ALS data to framework auto-brightness.
- **`touch/include/`** — Gesture config for `vendor.lineage.touch-service.oplus` (soong_config INCLUDE_DIR).
- **`init/`** — `libinit_samurai` shared library loaded by init; detects device variant (Global `RMX1931L1` vs China `RMX1931CN`) and overrides build properties accordingly.
- **`sepolicy/`** — Device-specific SELinux rules on top of qcom + `hardware/oplus` policy (prefer oplus type labels; keep only samurai-unique paths such as `sysfs_oppo_display`).
- **`shims/`** — Tiny libbase/libcrypto shims for legacy ODM blobs (dspservice, ATFWD-daemon).
- **`overlay*/`** — RRO (Runtime Resource Overlay) packages that patch framework resources at runtime (SystemUI, Settings, Telephony, WiFi, etc.). `overlay-lineage/` is for LineageOS-specific overrides.
- **`rootdir/`** — Files copied verbatim to the root filesystem: `bin/` (init shell scripts), `etc/` (init RC files, `fstab.qcom`).
  - `init.oppo.rc` — OTG / oppo_display / touchpanel permissions (gesture nodes for oplus touch HAL, including `double_tap_enable_indep`).
  - `init.qcom.power.rc` — `perf_profile` CPU/GPU caps (gold+ max 2956800; Adreno `max_pwrlevel` / `min_pwrlevel`).
  - `init.target.power.rc` / `init.target.rc` — suspend-to-RAM (`mem_sleep=deep`), `console_suspend=Y`, clear `lpm_levels.sleep_disabled` on `boot_completed` (cmdline boots with `sleep_disabled=1`).
- **`audio/`** — ALSA mixer paths, audio platform info, sound trigger config, and ACDB calibration data for the WCD9340 codec.
- **`configs/`** — GPS, media codec/profile XML, **powerhint.json** (PowerHAL nodes/hints), QMI config.
- **`vendorsetup.sh`** — Clones missing `hardware/oplus` and `vendor/lunaris/dolby` at envsetup time.
- **`infinity_samurai.mk`** / **`AndroidProducts.mk`** — Product entry point and lunch choices.
- **`manifest.xml`** — Device VINTF fragment (e.g. `tetheroffload.control` must be **1.1** to match IPACM).

## Architecture Patterns

### HAL Service Pattern
All HAL implementations follow the same structure: a `service.cpp` that registers the HIDL/AIDL service, a main implementation class inheriting from the generated interface base (e.g., `IFingerprint`), and a `Android.bp` declaring a `cc_binary` with `init_rc` and `vintf_fragments` entries pointing to the service's RC and manifest fragment.

### UDFPS Integration
The fingerprint HAL (`fingerprint/`) calls into `syshelper/` via AIDL to raise/lower a dim layer on the display during fingerprint authentication. The relevant chain is:
1. `BiometricsFingerprint` receives an authenticate/cancel request
2. It calls `UdfpsHelper` (in `syshelper/`) to toggle `dimlayer_hbm` via the display driver sysfs node
3. `getUdfpsDimZOrder()` controls the z-order of the dim layer

**Null-safety:** `BiometricsFingerprint` must tolerate a missing oplus FP or syshelper service (return `SYS_EINVAL` / no-op callbacks). `isEnrolling` is initialized `false`.

**Icon asset:** White fingerprint glyph in static SystemUI RRO only:
`overlay/SystemUIRes/res/drawable-nodpi/udfps_icon_pressed.webp`  
Do **not** duplicate under `overlay-lineage/.../SystemUI/`.

### Product ownership (do not dual-stack)

| Feature | Owner | Notes |
|---------|--------|--------|
| Ambient / pickup | **OplusDoze** + `OplusDozeResCommon` | RealmeParts doze activity/service disabled |
| Charge limit | **Lineage Health** on `mmi_charging_enable` | RealmeParts SmartCharging disabled |
| Display extras | RealmeParts (DC/HBM/sRGB/OTG/game) | Sysfs under `/sys/kernel/oppo_display` (0660 system) |

### Vibrator / DRM
- Vibrator HAL: `vendor.qti.hardware.vibrator.service.oplus` (hardware/oplus QTI path; aw8697 via `/sys/class/leds/vibrator`).
- Copy CAF `excluded-input-devices.xml` so touchscreen input does not double-trigger haptics.
- Ship `wvmkiller` with DRM/clearkey packages.

### Power / performance
- **PowerHAL** reads `configs/powerhint.json`. Gold+ (`cpu7`) default max is **2956800** (SM8150/855+ table top — not 2841600). GPU min/max are driven via **`max_pwrlevel` / `min_pwrlevel`** (not raw `devfreq` Hz alone); `ResetOnInit` must not re-cap clocks below hardware max.
- **Init profiles** in `init.qcom.power.rc` must stay aligned with powerhint (cpu7 max + GPU pwrlevels for balanced/gaming/power-save).
- **Idle / deep sleep (device tree, not a kernel gap)**:
  - Cmdline: `lpm_levels.sleep_disabled=1` for boot, `mem_sleep_default=deep`.
  - On `sys.boot_completed=1`: clear `sleep_disabled`, set `mem_sleep=deep`, `console_suspend=Y`, UFS clkgate/hibern8 — via **both** `init.qcom.power.rc` `enable-low-power` and `init.target*.rc` (do not leave `enable-low-power` as UFS-only).
  - **Display must reach OFF** or PowerManager keeps `mHoldingDisplaySuspendBlocker` and `mWakefulness=Awake`:
    - `doze_display_state_supported=false` (SystemUIRes) — HWC/oppo panel does not complete DOZE→OFF.
    - `config_powerDecouple{AutoSuspend,Interactive}ModeFromDisplay=false` — couple autosuspend/interactive to real display off.
    - Clear `dimlayer_hbm` / `force_screenfp` / `hbm` on boot_completed if UDFPS left them set.
  - Kernel already has `CONFIG_MSM_PM` + PSCI suspend; if still broken, `dumpsys power` for app wake locks.

### Device Variant Detection
`init/init_samurai.cpp` runs at early boot and reads the `ro.boot.id.operators` property to distinguish variants, then calls `property_override()` to set the correct `ro.product.*` and `ro.build.*` values. This pattern is also used for RAM-tier-specific dalvik heap configurations.

### Property Files
- `system.prop` — ART, audio, BT, camera, non-vendor RIL/IMS, sensors (system partition)
- `vendor.prop` — Qualcomm vendor-domain tuning (audio, display, DPM, GPS, perf, **persist.vendor.radio.***)
- `product.prop` — Bluetooth profile enablement (single owner for `bluetooth.profile.*`)
- `odm.prop` / `system_ext.prop` — Partition-specific overrides

Properties are loaded by the partition they reside on; do not put vendor-domain properties in `system.prop`.

#### Property gotchas (samurai-specific)
- **`ro.qti.sensors.wu` must be `true`** — `false` blocks tilt/pickup wake while the screen is off (breaks lift-to-wake and RealmeParts / OplusDoze).
- **Do not set `ro.oplus.system.camera.flashlight=com.oppo.motor.flashlight`** — that package is for pop-up motor cameras (e.g. Realme X2). X2 Pro has a fixed front camera; the prop breaks QS torch / CameraManager torch path.

### Overlays vs Properties
Use **RRO overlays** (`overlay*/`) to override framework resource values (strings, integers, booleans in `res/values/`). Use **property files** for runtime tunables consumed by native daemons and the HAL layer. Do not duplicate the same setting in both.

### VINTF
Keep device `manifest.xml` HAL versions aligned with the actual service registration (e.g. IPACM registers `tetheroffload.control` **1.1**; declaring 1.0 fails under `kEnforceVintfManifest`).

## Conventions

- **License header**: All new source files use `SPDX-License-Identifier: Apache-2.0` with a one-line copyright.
- **Soong (`Android.bp`) is preferred** over legacy `Android.mk`. Use `Android.mk` only for cases Soong cannot handle (e.g., custom install targets via `LOCAL_MODULE_PATH`).
- **C++ standard**: C++17 is available. Use `android::base` utilities (`android::base::ReadFileToString`, `android::base::SetProperty`, etc.) rather than raw POSIX where possible.
- **SELinux**: Every new executable or service needs a corresponding type in `sepolicy/`, a file context in `file_contexts`, and a service context if it registers with hwservicemanager/servicemanager.
- **HIDL vs AIDL**: Existing HALs use HIDL (fingerprint 2.3, sensors 2.1). New services (syshelper) use AIDL with the NDK backend. Don't mix HIDL and AIDL in the same service.
- **Proprietary blobs**: Listed in `proprietary-files.txt` with SHA-1 checksums. After adding or updating a blob, run `./update-sha1sums.py` and commit the updated checksums alongside the change.
- **Init permissions**: Use valid octal modes (`0666`/`0660`, never `06666`). `chown` takes user/group; `chmod` takes mode — do not swap them (see historical `fp_state` bug).
- **Touch HAL nodes**: When adding `/proc/touchpanel/*` gesture paths, chown to `system` and chmod `0660` so `vendor.lineage.touch-service.oplus` can access them.
