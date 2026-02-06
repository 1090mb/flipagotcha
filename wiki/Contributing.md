# Contributing

Thank you for your interest in contributing to Flipagotcha! This guide will help you get started.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [How to Contribute](#how-to-contribute)
- [Coding Standards](#coding-standards)
- [Testing](#testing)
- [Submitting Changes](#submitting-changes)
- [Project Structure](#project-structure)

## Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inclusive environment for all contributors.

### Expected Behavior

- Be respectful and inclusive
- Accept constructive criticism gracefully
- Focus on what's best for the project
- Show empathy towards others

### Unacceptable Behavior

- Harassment or discrimination
- Trolling or insulting comments
- Publishing others' private information
- Unprofessional conduct

## Getting Started

### Prerequisites

Before contributing, ensure you have:

- **Flipper Zero** device (for testing)
- **Development environment** set up
- **Git** installed and configured
- **GitHub account**
- **ESP32 board** with Marauder (optional for hardware testing)

### Find Something to Work On

1. **Browse Issues**
   - Check [open issues](https://github.com/1090mb/flipagotcha/issues)
   - Look for `good first issue` or `help wanted` labels

2. **Read Documentation**
   - Familiarize yourself with the codebase
   - Read [Architecture](Architecture.md) and [API Reference](API-Reference.md)

3. **Ask Questions**
   - Open an issue for discussion
   - Propose new features or improvements

## Development Setup

### 1. Fork and Clone

```bash
# Fork the repository on GitHub, then:
git clone https://github.com/YOUR_USERNAME/flipagotcha.git
cd flipagotcha
```

### 2. Set Up Flipper Firmware

```bash
# Clone Flipper Zero firmware
git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git
cd flipperzero-firmware

# Create symlink to your flipagotcha fork
cd applications_user/
ln -s /path/to/your/flipagotcha flipagotcha
cd ..

# Set up environment
./fbt
```

### 3. Build and Test

```bash
# Build the app
./fbt COMPACT=1 APPSRC=applications_user/flipagotcha

# Launch on connected Flipper
./fbt launch_app APPSRC=applications_user/flipagotcha
```

## How to Contribute

### Reporting Bugs

When reporting bugs, include:

**Title**: Clear, concise description

**Description**:
- Flipper Zero firmware version
- ESP32 board type (if applicable)
- Marauder version (if applicable)
- Steps to reproduce
- Expected behavior
- Actual behavior
- Screenshots or logs
- Any relevant configuration

**Example**:
```markdown
**Title**: App crashes when starting scan

**Environment**:
- Flipper firmware: 0.95.0
- ESP32: WROOM-32
- Marauder: v0.13.3

**Steps**:
1. Launch Flipagotcha
2. Press OK to start scan
3. App immediately crashes

**Expected**: Scan should start
**Actual**: App returns to menu

**Logs**: [attach logs]
```

### Suggesting Enhancements

For feature requests:

1. **Check existing issues** - may already be proposed
2. **Describe the feature** - what it should do
3. **Explain rationale** - why it's useful
4. **Consider alternatives** - other ways to achieve goal
5. **Implementation ideas** - if you have thoughts

**Example**:
```markdown
**Title**: Add PCAP file export

**Description**: 
Export captured packets in PCAP format for Wireshark analysis.

**Rationale**:
Current text format is hard to analyze. PCAP is standard format.

**Implementation**:
- Convert packet buffer to PCAP format
- Write PCAP header
- Export each packet with metadata
- Save as .pcap file
```

### Submitting Code

1. **Create branch**
   ```bash
   git checkout -b feature/my-new-feature
   ```

2. **Make changes**
   - Follow coding standards
   - Write clear commit messages
   - Test thoroughly

3. **Commit**
   ```bash
   git add .
   git commit -m "Add feature: description"
   ```

4. **Push**
   ```bash
   git push origin feature/my-new-feature
   ```

5. **Open Pull Request**
   - Describe changes clearly
   - Reference related issues
   - Add screenshots if UI changes

## Coding Standards

### C Code Style

Follow these conventions:

#### Naming Conventions

```c
// Functions: snake_case
void wifi_scanner_start_scan(WifiScanner* scanner);

// Types: PascalCase
typedef struct WifiNetwork WifiNetwork;

// Constants: UPPER_SNAKE_CASE
#define MAX_NETWORKS 16

// Variables: snake_case
uint8_t network_count = 0;

// Private functions: static with _impl suffix
static void parse_response_impl(const char* data);
```

#### Indentation and Formatting

```c
// 4 spaces for indentation (no tabs)
void example_function(int param) {
    if(condition) {
        do_something();
    } else {
        do_something_else();
    }
}

// Braces on same line for control structures
if(x > 0) {
    process(x);
}

// Function braces on new line
void my_function(void)
{
    // code
}
```

#### Comments

```c
// Single-line comments for brief explanations
int count = 0;  // Initialize counter

/**
 * Multi-line comment for functions
 * 
 * @param scanner Pointer to scanner instance
 * @return true on success, false on failure
 */
bool wifi_scanner_start_scan(WifiScanner* scanner);

// TODO: Add error handling
// FIXME: Memory leak here
// HACK: Workaround for Marauder bug
```

### Code Organization

#### Header Files

```c
#pragma once  // Use instead of include guards

#include <stdint.h>
#include <stdbool.h>

// Forward declarations
typedef struct WifiScanner WifiScanner;

// Public API
WifiScanner* wifi_scanner_alloc(Uart* uart);
void wifi_scanner_free(WifiScanner* scanner);
```

#### Source Files

```c
#include "wifi_scanner.h"  // Own header first

#include <stdlib.h>         // System headers
#include <string.h>

#include <furi.h>           // Flipper headers

// Constants
#define BUFFER_SIZE 1024

// Private structure definition
struct WifiScanner {
    // fields
};

// Private function declarations
static void parse_line(const char* line);

// Public function implementations
// Private function implementations
```

### Error Handling

Always check return values:

```c
// Good
WifiScanner* scanner = wifi_scanner_alloc(uart);
if(!scanner) {
    FURI_LOG_E(TAG, "Failed to allocate scanner");
    return false;
}

// Bad
WifiScanner* scanner = wifi_scanner_alloc(uart);
scanner->field = value;  // Crash if allocation failed!
```

### Memory Management

```c
// Always free what you allocate
void cleanup(void) {
    if(scanner) {
        wifi_scanner_free(scanner);
        scanner = NULL;  // Good practice
    }
}

// Check before dereferencing
if(scanner && scanner->uart) {
    uart_send(scanner->uart, data, len);
}
```

### Thread Safety

```c
// Always protect shared data
furi_mutex_acquire(scanner->mutex, FuriWaitForever);
scanner->network_count++;
furi_mutex_release(scanner->mutex);

// Don't return pointers to mutex-protected data
// Unless caller will hold mutex or copy data
```

## Testing

### Manual Testing

Before submitting:

1. **Build without errors**
   ```bash
   ./fbt COMPACT=1 APPSRC=applications_user/flipagotcha
   ```

2. **Test basic functionality**
   - App launches
   - Face animates
   - Buttons respond
   - Demo mode works

3. **Test with ESP32 (if available)**
   - Connection detected
   - Scanning works
   - Handshake capture works
   - File saving works

4. **Test edge cases**
   - No SD card
   - SD card full
   - ESP32 disconnected mid-scan
   - Multiple rapid button presses

### Testing Checklist

- [ ] Code compiles without warnings
- [ ] App launches successfully
- [ ] Demo mode functional
- [ ] ESP32 mode functional (if testable)
- [ ] No memory leaks
- [ ] No crashes during normal use
- [ ] File I/O works correctly
- [ ] UI responsive
- [ ] Documentation updated

## Submitting Changes

### Pull Request Process

1. **Update documentation**
   - Add/update comments
   - Update wiki if needed
   - Update README if features changed

2. **Test thoroughly**
   - Manual testing on device
   - Edge case testing
   - Multiple firmware versions if possible

3. **Commit message format**
   ```
   Short summary (50 chars or less)
   
   More detailed explanation if needed. Wrap at 72 characters.
   
   - Bullet points for multiple changes
   - Reference issues: Fixes #123
   ```

4. **Open Pull Request**
   - Use clear title
   - Fill out PR template
   - Describe what changed and why
   - Add screenshots for UI changes
   - Reference related issues

5. **Respond to review**
   - Address feedback promptly
   - Make requested changes
   - Discuss if you disagree
   - Be professional and courteous

### PR Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Code refactoring
- [ ] Performance improvement

## Testing
- [ ] Tested on device
- [ ] Demo mode works
- [ ] ESP32 mode works
- [ ] No new warnings/errors

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Comments added for complex code
- [ ] Documentation updated
- [ ] No new warnings
- [ ] Changes are minimal and focused

## Related Issues
Fixes #(issue number)

## Screenshots
(if applicable)
```

## Project Structure

### Key Files

```
flipagotcha/
├── application.fam       # Build configuration
├── README.md            # Main documentation
├── LICENSE              # MIT License
├── wiki/                # Wiki documentation
└── src/
    ├── main.c           # Entry point
    ├── common/
    │   └── protocol.h   # Protocol definitions
    ├── hw/
    │   ├── uart.c       # UART implementation
    │   ├── uart.h
    │   ├── wifi_scanner.c  # Scanner implementation
    │   └── wifi_scanner.h
    └── ui/
        ├── view.c       # View management
        ├── view.h
        ├── draw.c       # Drawing functions
        └── draw.h
```

### Module Responsibilities

- **main.c**: Application lifecycle, event handling
- **uart.c**: UART communication with ESP32
- **wifi_scanner.c**: Network scanning, packet capture
- **view.c**: UI state, rendering coordination
- **draw.c**: Face drawing, animations

## Recognition

Contributors will be:
- Listed in project contributors
- Credited in release notes
- Mentioned in documentation (if significant contribution)

## Questions?

- Open a [GitHub Issue](https://github.com/1090mb/flipagotcha/issues)
- Check existing [documentation](Home.md)
- Ask in discussions

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

**Thank you for contributing to Flipagotcha!**
