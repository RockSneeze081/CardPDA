# CardOS Firmware Package

Single-file firmware for M5Burner.

## Files

| File | Size | Description |
|------|------|-------------|
| `CardOS_single.bin` | 888 KB | **Single firmware file** (bootloader + partitions + app) |
| `config.json` | M5Burner config |

## Flash with M5Burner

1. Open M5Burner
2. Select ESP32-S3 or StampS3 device
3. Click "Custom" tab
4. Point to this folder
5. Click "Burn"

## Flash with esptool

```bash
esptool.py --chip esp32s3 write_flash @config.json
```

Or manually:
```bash
esptool.py --chip esp32s3 write_flash 0x0 CardOS_single.bin
```

## Build from Source

```bash
pio run        # Build
pio run --target upload  # Build + flash
```
