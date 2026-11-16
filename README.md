Copyright (C) 2020 The LineageOS Project

Device configuration for realme X2 Pro
=========================================

The realme X2 Pro (codenamed _"samurai"_) is a high-end smartphone from realme.

It was announced & released in October 2019.

## Device specifications

Basic   | Spec Sheet
-------:|:-------------------------
SoC     | Qualcomm SDM855+ Snapdragon 855+
CPU     | Octa-Core (1x2.96 GHz Kryo 485 & 3x2.42 GHz Kryo 485 & 4x1.8 GHz Kryo 485)
GPU     | Adreno 640
Memory  | 6/8/12 GB RAM
Shipped Android Version | 9.0 with ColorOS 6.1
Storage | 64/128/256GB
Battery | Non-removable Li-Po 4000 mAh battery
Display | 1080 x 2400 pixels, 20:9 ratio, 6.5 inches, Super AMOLED (~402 ppi density)
Rear Camera  | 64MP (wide) / 13MP (telephoto) / 8MP (ultrawide) / 2MP (depth)
Front Camera | 16MP (wide)

## Device picture
![realme X2 Pro](https://image01.realme.net/general/20191112/1573530955589.jpg "realme X2 Pro in lunar white")

## Build Dependencies & Repositories

| Component | Target Path | Recommended Branch | Notes |
|:---|:---|:---|:---|
| Device tree | `device/realme/samurai` | `lineage-24.0` | Main tree |
| Vendor blobs | `vendor/realme/samurai` | `lineage-24.0` | Base proprietary blobs |
| Kernel source | `kernel/realme/sm8150` | `lineage-24.0` | 4.14 unified kernel |
| Hardware Oplus | `hardware/oplus` | `lineage-24.0` | Oplus HAL interfaces |
| RealmeParts | `packages/apps/RealmeParts` | `lineage-24.0` | Device settings & features |
| Lunaris Dolby | `vendor/lunaris/dolby` | `main` | Dolby Atmos sound system |

> **Note on Security Patch Level**: Vendor blobs are based on stock Android 11 (`RMX1931EX_11_F.12`) with Qualcomm QSSI 13 stack. Vendor SPL reflects the official stock release `2021-11-05`.

