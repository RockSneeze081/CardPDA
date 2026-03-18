# CardOS Post-Build Script for PlatformIO
# This script runs after each build to copy files to the burn folder

Import("env")

import os
import shutil

def copy_firmware(source, destination):
    """Copy firmware files to burn folder"""
    os.makedirs(destination, exist_ok=True)
    
    # Copy firmware binaries
    files_to_copy = [
        'firmware.bin',
        'bootloader.bin', 
        'partitions.bin'
    ]
    
    for filename in files_to_copy:
        src = os.path.join(source, filename)
        dst = os.path.join(destination, filename)
        if os.path.exists(src):
            shutil.copy2(src, dst)
            print(f"Copied {filename} to burn folder")
    
    # Create combined firmware
    combined_path = os.path.join(destination, 'CardOS_combined.bin')
    with open(combined_path, 'wb') as combined:
        for filename in files_to_copy:
            src = os.path.join(source, filename)
            if os.path.exists(src):
                with open(src, 'rb') as f:
                    combined.write(f.read())
    print(f"Created combined firmware at {combined_path}")

# Get paths
source_dir = env['BUILD_DIR']
dest_dir = os.path.join(env['PROJECT_DIR'], 'burn', 'CardOS')

# Run the copy function
copy_firmware(source_dir, dest_dir)
print(f"CardOS firmware ready for M5Burner in: {dest_dir}")
