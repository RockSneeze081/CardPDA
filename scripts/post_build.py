# CardOS Post-Build Script for PlatformIO
# Creates M5Burner-compatible single firmware file

Import("env")
Import("projenv")

import os

def create_single_firmware(source, destination):
    """Create a single merged firmware file for M5Burner"""
    os.makedirs(destination, exist_ok=True)
    
    # Check if firmware files exist
    bootloader_path = os.path.join(source, 'bootloader.bin')
    partitions_path = os.path.join(source, 'partitions.bin')
    firmware_path = os.path.join(source, 'firmware.bin')
    
    if not os.path.exists(bootloader_path):
        print(f"Warning: {bootloader_path} not found, skipping single firmware creation")
        return
    
    # Read all firmware components
    with open(bootloader_path, 'rb') as f:
        bootloader = f.read()
    with open(partitions_path, 'rb') as f:
        partitions = f.read()
    with open(firmware_path, 'rb') as f:
        firmware = f.read()
    
    # Create merged firmware with proper layout
    merged = bytearray()
    
    # Pad to 0x1000 (bootloader offset)
    merged.extend(bytes(0x1000))
    
    # Add bootloader at 0x1000
    merged.extend(bootloader)
    
    # Pad to 0x8000 (partition offset)
    current_len = len(merged)
    if current_len < 0x8000:
        merged.extend(bytes(0x8000 - current_len))
    
    # Add partitions at 0x8000
    merged.extend(partitions)
    
    # Pad to 0x10000 (app offset)
    current_len = len(merged)
    if current_len < 0x10000:
        merged.extend(bytes(0x10000 - current_len))
    
    # Add firmware at 0x10000
    merged.extend(firmware)
    
    # Write single firmware file
    single_path = os.path.join(destination, 'CardOS_single.bin')
    with open(single_path, 'wb') as f:
        f.write(bytes(merged))
    
    print(f"Created single firmware: {len(merged)} bytes ({len(merged)//1024}KB)")
    
    # Write config.json
    config = """{
  "name": "CardOS",
  "version": "0.1.0",
  "chip": "esp32s3",
  "description": "CardOS - Modular Personal OS for M5Stack Cardputer",
  "binaries": [
    {
      "filename": "CardOS_single.bin",
      "offset": "0x0"
    }
  ],
  "config": {
    "baudrate": 921600,
    "flash_size": "8MB",
    "flash_mode": "dio",
    "flash_freq": "80m"
  }
}"""
    config_path = os.path.join(destination, 'config.json')
    with open(config_path, 'w') as f:
        f.write(config)
    
    print(f"M5Burner package ready: {destination}")

# Get paths - use projenv to get correct build directory
source_dir = projenv['BUILD_DIR']
dest_dir = os.path.join(projenv['PROJECT_DIR'], 'burn', 'CardOS')

# Create single firmware
create_single_firmware(source_dir, dest_dir)
