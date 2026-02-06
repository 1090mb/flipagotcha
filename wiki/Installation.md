# Installation Guide

This guide provides detailed instructions for installing Flipagotcha on your Flipper Zero.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Installation Methods](#installation-methods)
  - [Method 1: Pre-built Binary](#method-1-pre-built-binary-easiest)
  - [Method 2: Build from Source](#method-2-build-from-source)
  - [Method 3: Using Flipper Build Tool](#method-3-using-flipper-build-tool)
- [Verifying Installation](#verifying-installation)
- [Updating](#updating)
- [Uninstalling](#uninstalling)

## Prerequisites

### Required
- **Flipper Zero** with firmware version 0.80.0 or later
- **SD card** installed in Flipper Zero (16GB+ recommended)
- **USB cable** (USB-C or compatible with your Flipper Zero model)
- **Computer** (Windows, macOS, or Linux)

### For Building from Source
- **Flipper Zero Firmware SDK** installed
- **Git** for cloning the repository
- **Python 3.8+** (usually required by Flipper build tools)
- **Build tools** (gcc, make, etc.)

### Optional
- **qFlipper** application for easier file management
- **ESP32 board** with Marauder firmware for real WiFi scanning

## Installation Methods

### Method 1: Pre-built Binary (Easiest)

This is the recommended method for most users.

#### Step 1: Download the Application

1. Go to the [Releases page](https://github.com/1090mb/flipagotcha/releases)
2. Find the latest release
3. Download the `flipagotcha.fap` file

#### Step 2: Transfer to Flipper Zero

**Option A: Using qFlipper (Recommended)**

1. Connect Flipper Zero to your computer via USB
2. Launch qFlipper application
3. Go to **File Manager**
4. Navigate to `/ext/apps/GPIO/`
5. Click **Upload** and select the downloaded `flipagotcha.fap`
6. Wait for the upload to complete

**Option B: Manual SD Card Copy**

1. Remove the SD card from Flipper Zero
2. Insert it into your computer's SD card reader
3. Navigate to `/ext/apps/GPIO/` folder
4. Copy the `flipagotcha.fap` file to this location
5. Safely eject the SD card
6. Reinsert the SD card into Flipper Zero

#### Step 3: Launch the Application

1. On Flipper Zero, navigate to: **Applications** → **GPIO**
2. Find **Flipagotcha** in the list
3. Press **OK** to launch

### Method 2: Build from Source

For developers who want to build from source code.

#### Step 1: Set Up Flipper Zero Development Environment

If you haven't already set up the Flipper Zero development environment:

```bash
# Clone the official Flipper Zero firmware
git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git
cd flipperzero-firmware

# Set up the environment
./fbt
```

#### Step 2: Clone Flipagotcha Repository

```bash
# Navigate to the applications_user directory
cd applications_user/

# Clone the Flipagotcha repository
git clone https://github.com/1090mb/flipagotcha.git

# Return to firmware root
cd ..
```

Your directory structure should look like:
```
flipperzero-firmware/
├── applications_user/
│   └── flipagotcha/
│       ├── src/
│       ├── assets/
│       ├── application.fam
│       └── ...
└── ...
```

#### Step 3: Build the Application

```bash
# Build the application
./fbt COMPACT=1 APPSRC=applications_user/flipagotcha

# The compiled .fap file will be in:
# dist/flipagotcha.fap
```

#### Step 4: Install to Flipper Zero

**Option A: Using fbt (if Flipper is connected)**

```bash
# Flash to connected Flipper Zero
./fbt launch_app APPSRC=applications_user/flipagotcha
```

**Option B: Manual installation**

1. Copy `dist/flipagotcha.fap` to Flipper Zero's `/ext/apps/GPIO/` directory
2. Use qFlipper or SD card method as described in Method 1

### Method 3: Using Flipper Build Tool

This method uses the Flipper Build Tool (fbt) for a streamlined build and install process.

#### Step 1: Prepare Build Environment

```bash
# Clone Flipper firmware if not already done
git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git
cd flipperzero-firmware

# Clone Flipagotcha into applications_user
cd applications_user/
git clone https://github.com/1090mb/flipagotcha.git
cd ..
```

#### Step 2: Build and Install in One Command

```bash
# Build and launch on connected Flipper Zero
./fbt launch_app APPSRC=applications_user/flipagotcha
```

This command will:
1. Compile the application
2. Connect to your Flipper Zero
3. Upload and launch the app automatically

## Verifying Installation

### Check Installation Success

1. **Navigate to Applications**
   - On Flipper Zero: **Applications** → **GPIO**
   
2. **Find Flipagotcha**
   - You should see **Flipagotcha** in the list
   - Icon: Custom Flipagotcha icon

3. **Launch and Test**
   - Press **OK** to launch
   - You should see:
     - Animated face in the center
     - "DEMO" or "ESP32" indicator in top-right
     - Responsive button controls

### Verify File Structure

The app creates this directory structure:

```
/ext/
├── apps/
│   └── GPIO/
│       └── flipagotcha.fap          (application binary)
└── apps_data/
    └── flipagotcha/
        └── capture.txt              (created after first capture)
```

## Updating

### Update from Pre-built Binary

1. Download the latest `flipagotcha.fap` from [Releases](https://github.com/1090mb/flipagotcha/releases)
2. Replace the existing file in `/ext/apps/GPIO/`
3. Restart the app

### Update from Source

```bash
# Navigate to the repository
cd /path/to/flipperzero-firmware/applications_user/flipagotcha

# Pull latest changes
git pull origin main

# Rebuild
cd ../..
./fbt COMPACT=1 APPSRC=applications_user/flipagotcha

# Reinstall the updated binary
```

## Uninstalling

### Complete Removal

1. **Delete the application binary**
   - Remove `/ext/apps/GPIO/flipagotcha.fap`

2. **Delete application data** (optional)
   - Remove `/ext/apps_data/flipagotcha/` directory
   - This contains captured packets and saved data

### Using qFlipper

1. Connect Flipper Zero to computer
2. Open qFlipper → File Manager
3. Navigate to `/ext/apps/GPIO/`
4. Right-click `flipagotcha.fap` → **Delete**
5. Navigate to `/ext/apps_data/`
6. Right-click `flipagotcha/` folder → **Delete** (if desired)

## Troubleshooting Installation

### Common Issues

#### "Application not found" after installation

**Cause**: File placed in wrong directory  
**Solution**: Ensure `flipagotcha.fap` is in `/ext/apps/GPIO/` not `/ext/apps/`

#### "Cannot launch application" error

**Cause**: Incompatible firmware version  
**Solution**: Update Flipper Zero firmware to latest version

#### Build fails with compilation errors

**Cause**: Outdated Flipper firmware SDK  
**Solution**: 
```bash
cd /path/to/flipperzero-firmware
git pull
./fbt update
```

#### App crashes on launch

**Cause**: Corrupted binary or SD card issues  
**Solution**:
1. Re-download or rebuild the `.fap` file
2. Check SD card for errors
3. Try reformatting SD card (backup data first)

### Getting Help

If you encounter issues:

1. Check the [Troubleshooting Guide](Troubleshooting.md)
2. Review [FAQ](FAQ.md)
3. Search [GitHub Issues](https://github.com/1090mb/flipagotcha/issues)
4. Open a new issue with:
   - Flipper Zero firmware version
   - Installation method used
   - Error messages or symptoms
   - Steps to reproduce

## Platform-Specific Notes

### Windows
- Use PowerShell or Git Bash for build commands
- Ensure Python is in PATH
- May need to install build tools via Visual Studio or MinGW

### macOS
- Install Xcode Command Line Tools: `xcode-select --install`
- May need to use `python3` instead of `python`

### Linux
- Install build dependencies:
  ```bash
  sudo apt-get install git python3 gcc-arm-none-eabi
  ```

## Next Steps

After successful installation:

1. **[Hardware Setup](Hardware-Setup.md)** - Connect ESP32 for real WiFi scanning
2. **[Getting Started](Getting-Started.md)** - Quick start guide
3. **[User Guide](User-Guide.md)** - Learn how to use all features

---

**Need Help?** Visit [Troubleshooting](Troubleshooting.md) or [FAQ](FAQ.md)
