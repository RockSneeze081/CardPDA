#!/bin/bash
# CardOS Burner Build Script
# This script builds the firmware and creates M5Burner-compatible files

set -e

echo "=== CardOS Firmware Build Script ==="
echo ""

# Build firmware with PlatformIO
echo "[1/4] Building firmware with PlatformIO..."
~/.local/bin/pio run

# Create burn directory
echo "[2/4] Creating M5Burner package..."
mkdir -p burn/CardOS

# Copy firmware files
echo "[3/4] Copying firmware files..."
cp .pio/build/m5stack-stamps3/firmware.bin burn/CardOS/
cp .pio/build/m5stack-stamps3/bootloader.bin burn/CardOS/
cp .pio/build/m5stack-stamps3/partitions.bin burn/CardOS/

# Create combined firmware
echo "[4/4] Creating combined firmware..."
cd burn/CardOS

# Combine all binaries
cat bootloader.bin partitions.bin firmware.bin > CardOS_combined.bin

# Create hex dump for verification
xxd -l 16 CardOS_combined.bin > /dev/null 2>&1 && echo "Combined firmware verified!"

# Go back to project root
cd ../..

echo ""
echo "=== Build Complete ==="
echo ""
echo "M5Burner package created in: burn/CardOS/"
echo ""
echo "Files:"
echo "  - bootloader.bin     ($(stat -f%z burn/CardOS/bootloader.bin 2>/dev/null || stat -c%s burn/CardOS/bootloader.bin) bytes)"
echo "  - partitions.bin      ($(stat -f%z burn/CardOS/partitions.bin 2>/dev/null || stat -c%s burn/CardOS/partitions.bin) bytes)"
echo "  - firmware.bin        ($(stat -f%z burn/CardOS/firmware.bin 2>/dev/null || stat -c%s burn/CardOS/firmware.bin) bytes)"
echo "  - config.json        (M5Burner config)"
echo "  - CardOS_combined.bin ($(stat -f%z burn/CardOS/CardOS_combined.bin 2>/dev/null || stat -c%s burn/CardOS/CardOS_combined.bin) bytes - all-in-one)"
echo ""
echo "To flash with M5Burner:"
echo "  1. Open M5Burner"
echo "  2. Select 'Custom' tab"
echo "  3. Point to burn/CardOS/ folder"
echo ""
echo "Or flash directly with esptool:"
echo "  esptool.py --chip esp32s3 write_flash @burn/CardOS/config.json"
