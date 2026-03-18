# CardOS Firmware Package

This folder contains the CardOS firmware ready for flashing with M5Burner.

## Files

| File | Size | Description |
|------|------|-------------|
| `bootloader.bin` | 15 KB | ESP32 bootloader |
| `partitions.bin` | 3 KB | Partition table |
| `firmware.bin` | 824 KB | Main application |
| `CardOS_combined.bin` | 842 KB | All-in-one firmware |
| `config.json` | 688 B | M5Burner configuration |

## Usage with M5Burner

### Method 1: Custom App (Recommended)
1. Open M5Burner
2. Select your device (StampS3 or generic ESP32-S3)
3. Click the "Custom" tab
4. Point to this folder (e.g., `burn/CardOS/`)
5. Click "Burn"

### Method 2: Direct Flash with esptool
```bash
# Flash using config offsets
esptool.py --chip esp32s3 write_flash @config.json

# Or manually:
esptool.py --chip esp32s3 \
  write_flash 0x1000 bootloader.bin \
  0x8000 partitions.bin \
  0x10000 firmware.bin
```

### Method 3: PlatformIO Upload
```bash
pio run --target upload
```

## Flashing with Combined Bin
```bash
esptool.py --chip esp32s3 \
  --baud 921600 \
  write_flash \
  0x1000 CardOS_combined.bin
```

## Device Requirements
- **Chip**: ESP32-S3
- **Flash**: 8MB minimum
- **RAM**: 320KB+
- **Baudrate**: 921600 (default)

## Build from Source
```bash
pio run        # Build firmware
pio run --target upload  # Build + upload
```

Or use the convenience script:
```bash
./scripts/build_burner.sh
```
