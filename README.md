# Device Tree for realme X2 Pro (samurai)

**Project Infinity-X** device configuration for the realme X2 Pro (RMX1931).

The realme X2 Pro (codenamed _samurai_) is a high-end smartphone from realme.
It was announced & released in October 2019.

## Device specifications

| Basic | Spec Sheet |
|------:|:-----------|
| SoC | Qualcomm SDM855+ Snapdragon 855+ |
| CPU | Octa-Core (1×2.96 GHz Kryo 485 & 3×2.42 GHz Kryo 485 & 4×1.8 GHz Kryo 485) |
| GPU | Adreno 640 |
| Memory | 6/8/12 GB RAM |
| Shipped Android Version | 9.0 with ColorOS 6.1 |
| Storage | 64/128/256 GB |
| Battery | Non-removable Li-Po 4000 mAh |
| Display | 1080 × 2400, 20:9, 6.5" Super AMOLED (~402 ppi), 90 Hz |
| Rear Camera | 64 MP (wide) / 13 MP (telephoto) / 8 MP (ultrawide) / 2 MP (depth) |
| Front Camera | 16 MP (wide) |

## Build

```bash
# From the root of your Infinity-X tree
source build/envsetup.sh
# vendorsetup.sh clones hardware/oplus and vendor/lunaris/dolby if missing
lunch infinity_samurai-userdebug
mka bacon
```

### Dependencies

| Path | Source |
|------|--------|
| `hardware/oplus` | [LineageOS/android_hardware_oplus](https://github.com/LineageOS/android_hardware_oplus) (`lineage-23.2`) |
| `vendor/lunaris/dolby` | [tranQuila-Project/vendor_lunaris_dolby](https://github.com/tranQuila-Project/vendor_lunaris_dolby) (LunarisAOSP Dolby Atmos) |
| `kernel/realme/sm8150` | Device kernel (`TARGET_KERNEL_SOURCE`) |
| `vendor/realme/samurai` | Proprietary blobs (`./extract-files.sh`) |

Also listed in `lineage.dependencies` for roomservice-style sync.

### Maintainers

NAYEM & TITAN

## Device picture

![realme X2 Pro](https://image01.realme.net/general/20191112/1573530955589.jpg "realme X2 Pro in lunar white")
