# Architecture

Technical architecture and design documentation for Flipagotcha.

## Table of Contents

- [System Overview](#system-overview)
- [Architecture Diagram](#architecture-diagram)
- [Module Structure](#module-structure)
- [Data Flow](#data-flow)
- [Threading Model](#threading-model)
- [Memory Management](#memory-management)
- [Design Patterns](#design-patterns)
- [Security Architecture](#security-architecture)

## System Overview

Flipagotcha is a Flipper Zero external application (FAP) that integrates with ESP32 Marauder firmware via UART for WiFi security research.

### Technology Stack

- **Platform**: Flipper Zero (ARM Cortex-M4F)
- **Language**: C (C11 standard)
- **Framework**: Flipper Zero SDK / Furi OS
- **External Hardware**: ESP32 with Marauder firmware
- **Communication**: UART (115200 baud, 8N1)
- **Storage**: SD card (FAT32)

### Key Components

1. **UI Layer**: Canvas-based interface and user input handling
2. **Hardware Layer**: UART communication and WiFi scanner integration
3. **Data Layer**: Packet storage and file management
4. **Application Layer**: Main app logic and state management

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                     Flipper Zero Device                      │
│  ┌────────────────────────────────────────────────────────┐ │
│  │                  Flipagotcha App                       │ │
│  │                                                        │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌────────────┐ │ │
│  │  │  UI Layer    │  │  App Layer   │  │  HW Layer  │ │ │
│  │  │              │  │              │  │            │ │ │
│  │  │ • view.c     │→ │ • main.c     │→ │ • uart.c   │ │ │
│  │  │ • draw.c     │  │ • State mgmt │  │ • wifi_    │ │ │
│  │  │ • Canvas API │  │ • Event loop │  │   scanner.c│ │ │
│  │  └──────────────┘  └──────────────┘  └────────────┘ │ │
│  │         ↑                 ↑                  ↓       │ │
│  │         └─────────────────┴──────────────────┘       │ │
│  │                     Furi OS / SDK                    │ │
│  └────────────────────────────────────────────────────────┘ │
│                           │                                  │
│                           ↓                                  │
│                   ┌───────────────┐                          │
│                   │  SD Card      │                          │
│                   │  Storage      │                          │
│                   └───────────────┘                          │
│                           ↑                                  │
│                           ↓                                  │
│                   ┌───────────────┐                          │
│                   │  GPIO Pins    │                          │
│                   │  (UART)       │                          │
│                   └───────┬───────┘                          │
└───────────────────────────┼──────────────────────────────────┘
                            │ TX/RX @ 115200 baud
                            ↓
                    ┌───────────────┐
                    │  ESP32 Board  │
                    │  ┌─────────┐  │
                    │  │Marauder │  │
                    │  │Firmware │  │
                    │  └─────────┘  │
                    │  WiFi Radio   │
                    └───────────────┘
                            ↓
                      WiFi Networks
```

## Module Structure

### File Organization

```
flipagotcha/
├── application.fam          # Build configuration
├── assets/
│   └── icon.png            # App icon
└── src/
    ├── main.c              # Application entry point
    ├── common/
    │   └── protocol.h      # Command definitions
    ├── hw/
    │   ├── uart.c          # UART communication
    │   ├── uart.h
    │   ├── wifi_scanner.c  # WiFi scanner module
    │   └── wifi_scanner.h
    └── ui/
        ├── view.c          # View management
        ├── view.h
        ├── draw.c          # Drawing functions
        └── draw.h
```

### Module Descriptions

#### main.c
- **Purpose**: Application entry point and main loop
- **Responsibilities**:
  - Initialize all modules
  - Create view and scanner instances
  - Handle button events
  - Manage application lifecycle
  - Clean up resources on exit

#### ui/view.c
- **Purpose**: View rendering and state management
- **Responsibilities**:
  - Render main view
  - Display network/packet counts
  - Show connection status
  - Update face expressions
  - Handle UI state

#### ui/draw.c
- **Purpose**: Drawing primitives for face animation
- **Responsibilities**:
  - Draw eyes (open/closed)
  - Draw mouth (smile/frown)
  - Animate blinking
  - Render face geometry

#### hw/uart.c
- **Purpose**: UART communication with ESP32
- **Responsibilities**:
  - Initialize UART hardware
  - Send commands to ESP32
  - Receive responses asynchronously
  - Manage RX buffer
  - Handle connection detection

#### hw/wifi_scanner.c
- **Purpose**: WiFi scanning and packet capture
- **Responsibilities**:
  - Parse Marauder responses
  - Store network information
  - Manage packet buffer
  - Save captures to file
  - Provide thread-safe access

#### common/protocol.h
- **Purpose**: Protocol definitions and constants
- **Responsibilities**:
  - Define Marauder commands
  - Network/packet data structures
  - Configuration constants
  - Buffer sizes

## Data Flow

### Startup Sequence

```
1. main() entry
   ↓
2. Allocate ViewPort
   ↓
3. Initialize UART
   ↓
4. Create WiFi Scanner
   ↓
5. Register view callback
   ↓
6. Add ViewPort to GUI
   ↓
7. Enter event loop
```

### WiFi Scan Flow

```
User presses OK
      ↓
Main event handler
      ↓
wifi_scanner_start_scan()
      ↓
uart_send("scanap\n")
      ↓
ESP32 Marauder processes
      ↓
UART RX worker receives data
      ↓
UART callback invoked
      ↓
wifi_scanner_parse_response()
      ↓
Store network data (mutex protected)
      ↓
View renders updated count
```

### Handshake Capture Flow

```
User presses Left
      ↓
wifi_scanner_start_handshake()
      ↓
Acquire mutex
      ↓
Check network_count > 0
      ↓
Get first network's channel
      ↓
Build command: "sniffpmkid -c <ch> -d\n"
      ↓
uart_send(command)
      ↓
Release mutex
      ↓
ESP32 starts capture
      ↓
Packets captured by UART worker
      ↓
Stored in packet buffer
      ↓
Packet count updated
```

### Save Capture Flow

```
User presses OK (stop scan)
      ↓
wifi_scanner_stop_scan()
      ↓
uart_send("stopscan\n")
      ↓
wifi_scanner_save_capture()
      ↓
Open file: /ext/apps_data/flipagotcha/capture.txt
      ↓
Write header
      ↓
For each packet:
  - Write metadata
  - Write hex dump
      ↓
Close file
      ↓
Return status
```

## Threading Model

### Threads

Flipagotcha uses multiple threads:

1. **Main Thread (GUI)**
   - Runs event loop
   - Handles button input
   - Renders display
   - Updates UI state

2. **UART RX Worker**
   - Background thread
   - Processes incoming UART data
   - Invokes callbacks
   - Non-blocking operation

3. **Furi System Threads**
   - Timer callbacks
   - Hardware interrupts
   - File I/O

### Synchronization

#### Mutex Protection

```c
typedef struct {
    FuriMutex* mutex;           // Protects all fields below
    WifiNetwork networks[MAX_NETWORKS];
    uint8_t network_count;
    CapturedPacket* packets;
    uint16_t packet_count;
    bool scanning;
} WifiScanner;
```

All access to shared data:
1. Acquire mutex
2. Access/modify data
3. Release mutex

#### Critical Sections

- Network list operations
- Packet buffer access
- Scanner state changes
- UART buffer operations

### Deadlock Prevention

- **Lock Ordering**: Always acquire mutexes in same order
- **Timeout**: Use `FuriWaitForever` or reasonable timeout
- **No Nested Locks**: Avoid holding multiple mutexes
- **Release Quickly**: Minimize critical section duration

## Memory Management

### Static Allocations

- Network array: 16 entries (compile-time constant)
- UART buffer: 2048 bytes (fixed size)
- Command buffer: 256 bytes

### Dynamic Allocations

- Packet buffer: Initially 100 packets, expandable
- Scanner structure: Allocated/freed with app
- UART structure: Allocated on initialization

### Memory Layout

```
Stack (4KB)
    ↓
Heap (allocated objects)
    - WifiScanner: ~1.5KB
    - Packet buffer: ~50KB (100 packets × 512 bytes)
    - UART buffer: 2KB
    ↓
Program code (read-only)
```

### Cleanup

Proper cleanup on exit:
1. Stop UART worker
2. Free scanner structure
3. Free UART structure
4. Remove viewport
5. Release Furi resources

## Design Patterns

### Callback Pattern

UART uses callbacks for async data:

```c
typedef void (*UartCallback)(const uint8_t* data, size_t len, void* context);

void uart_set_callback(Uart* uart, UartCallback callback, void* context);
```

### Opaque Pointer Pattern

Modules expose opaque handles:

```c
// In header
typedef struct WifiScanner WifiScanner;

// In implementation
struct WifiScanner {
    // Internal fields hidden
};
```

### Resource Acquisition Is Initialization (RAII-like)

```c
// Allocate
WifiScanner* scanner = wifi_scanner_alloc();

// Use
wifi_scanner_start_scan(scanner);

// Free (automatic cleanup)
wifi_scanner_free(scanner);
```

### State Machine

Main app uses simple state machine:
- Idle
- Scanning
- Capturing handshake
- Saving

Transitions controlled by button events.

## Security Architecture

### Input Validation

All untrusted input validated:

```c
// Channel validation
if(channel < 1 || channel > 14) {
    return false;  // Invalid channel
}

// RSSI validation
if(rssi < -100 || rssi > 0) {
    return false;  // Invalid RSSI
}
```

### Buffer Overflow Prevention

```c
// Safe string operations
snprintf(buffer, sizeof(buffer), "sniffpmkid -c %d -d\n", channel);

// Bounds checking
if(offset + 16 > buffer_size) {
    break;  // Prevent overflow
}
```

### Command Injection Prevention

- Channel numbers validated before use
- No user strings in commands
- Fixed command format

### Race Condition Prevention

```c
// TOCTOU fix: Check under mutex
furi_mutex_acquire(scanner->mutex, FuriWaitForever);

if(scanner->network_count > 0) {
    channel = scanner->networks[0].channel;
    // Use channel while locked
}

furi_mutex_release(scanner->mutex);
```

### Secure File Operations

- Safe file paths
- Error checking
- Proper permissions
- No path traversal

## Error Handling

### Strategy

1. **Fail Gracefully**: Demo mode on ESP32 failure
2. **User Feedback**: Status indicators on screen
3. **Log Errors**: Furi logging for debugging
4. **Resource Cleanup**: Always cleanup on error

### Error Paths

```c
// Example: Allocation failure
WifiScanner* scanner = malloc(sizeof(WifiScanner));
if(!scanner) {
    FURI_LOG_E(TAG, "Failed to allocate scanner");
    return NULL;
}
```

### Recovery Mechanisms

- **UART timeout**: Fallback to demo mode
- **File write failure**: Continue operation, log error
- **Memory allocation failure**: Graceful degradation

## Performance Considerations

### Optimization Techniques

1. **Lazy Rendering**: Only redraw when state changes
2. **Efficient Parsing**: Single-pass line parsing
3. **Bounded Buffers**: Fixed-size buffers for predictability
4. **Minimal Allocations**: Reuse buffers where possible

### Profiling Points

- UART RX throughput
- Packet parsing speed
- File write performance
- Canvas render time

### Resource Constraints

- **RAM**: ~64KB available (Flipper Zero limitation)
- **Storage**: SD card speed limits writes
- **CPU**: ARM Cortex-M4F @ 64MHz
- **Battery**: ~2-3 hours with ESP32

## Build System

### Application Manifest

`application.fam` defines:
- App metadata
- Source files
- Dependencies
- Stack size
- Entry point

### Compilation

```bash
# Build with Flipper Build Tool
./fbt COMPACT=1 APPSRC=applications_user/flipagotcha

# Output: dist/flipagotcha.fap
```

### Dependencies

- Flipper Zero SDK
- Furi OS APIs
- Standard C library (newlib-nano)

## Testing Strategy

### Unit Testing

Currently limited:
- Manual testing required
- No automated test suite

### Integration Testing

Test scenarios:
1. Without ESP32 (demo mode)
2. With ESP32 (real mode)
3. Edge cases (no networks, connection loss)

### Field Testing

Real-world testing:
- Various WiFi environments
- Different ESP32 boards
- Extended runtime
- Battery drain analysis

## Future Architecture Improvements

Potential enhancements:
1. **Plugin System**: Modular feature loading
2. **Configuration File**: User preferences
3. **Logging System**: Better debug output
4. **Test Framework**: Automated tests
5. **PCAP Export**: Standard packet format
6. **State Persistence**: Remember settings

---

**Next**: [API Reference](API-Reference.md) | [Contributing](Contributing.md)
