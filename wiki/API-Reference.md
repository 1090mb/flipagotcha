# API Reference

Complete API documentation for Flipagotcha modules and functions.

## Table of Contents

- [WiFi Scanner API](#wifi-scanner-api)
- [UART API](#uart-api)
- [Protocol Definitions](#protocol-definitions)
- [UI Drawing API](#ui-drawing-api)
- [Data Structures](#data-structures)
- [Constants](#constants)

## WiFi Scanner API

**Module**: `src/hw/wifi_scanner.c` / `wifi_scanner.h`

### wifi_scanner_alloc()

Allocate and initialize a WiFi scanner instance.

```c
WifiScanner* wifi_scanner_alloc(Uart* uart);
```

**Parameters:**
- `uart` - Pointer to initialized UART instance

**Returns:**
- Pointer to allocated `WifiScanner` structure
- `NULL` on allocation failure

**Description:**
- Allocates scanner structure
- Initializes mutex
- Sets up packet buffer
- Registers UART callback
- Tests ESP32 connection

**Example:**
```c
Uart* uart = uart_alloc();
WifiScanner* scanner = wifi_scanner_alloc(uart);
if(!scanner) {
    // Handle allocation failure
}
```

---

### wifi_scanner_free()

Free WiFi scanner resources.

```c
void wifi_scanner_free(WifiScanner* scanner);
```

**Parameters:**
- `scanner` - Pointer to scanner instance

**Description:**
- Stops any active scanning
- Frees packet buffer
- Destroys mutex
- Frees scanner structure

**Example:**
```c
wifi_scanner_free(scanner);
scanner = NULL;  // Good practice
```

---

### wifi_scanner_start_scan()

Start WiFi network scanning.

```c
bool wifi_scanner_start_scan(WifiScanner* scanner);
```

**Parameters:**
- `scanner` - Pointer to scanner instance

**Returns:**
- `true` on success
- `false` on failure (UART error, already scanning)

**Description:**
- Sends `scanap` command to ESP32
- Starts packet capture
- Clears previous network list
- Sets scanning state

**Example:**
```c
if(wifi_scanner_start_scan(scanner)) {
    // Scan started successfully
} else {
    // Failed to start scan
}
```

---

### wifi_scanner_stop_scan()

Stop WiFi network scanning.

```c
void wifi_scanner_stop_scan(WifiScanner* scanner);
```

**Parameters:**
- `scanner` - Pointer to scanner instance

**Description:**
- Sends `stopscan` command to ESP32
- Stops packet capture
- Clears scanning state
- Saves captured packets to file

**Example:**
```c
wifi_scanner_stop_scan(scanner);
```

---

### wifi_scanner_start_handshake()

Initiate WPA handshake capture.

```c
bool wifi_scanner_start_handshake(WifiScanner* scanner);
```

**Parameters:**
- `scanner` - Pointer to scanner instance

**Returns:**
- `true` on success
- `false` if no networks detected

**Description:**
- Targets first detected network
- Sends `sniffpmkid -c <channel> -d` command
- Enables deauthentication
- Captures EAPOL frames

**Thread Safety:**
- Acquires mutex before checking network count
- Prevents TOCTOU race condition

**Example:**
```c
if(wifi_scanner_start_handshake(scanner)) {
    // Handshake capture started
} else {
    // No networks to target
}
```

---

### wifi_scanner_get_network_count()

Get number of detected networks.

```c
uint8_t wifi_scanner_get_network_count(const WifiScanner* scanner);
```

**Parameters:**
- `scanner` - Pointer to scanner instance

**Returns:**
- Number of detected networks (0-16)

**Thread Safety:**
- Acquires mutex for safe access

**Example:**
```c
uint8_t count = wifi_scanner_get_network_count(scanner);
printf("Found %d networks\n", count);
```

---

### wifi_scanner_get_network()

Get network information by index.

```c
const WifiNetwork* wifi_scanner_get_network(const WifiScanner* scanner, uint8_t index);
```

**Parameters:**
- `scanner` - Pointer to scanner instance
- `index` - Network index (0 to count-1)

**Returns:**
- Pointer to `WifiNetwork` structure
- `NULL` if index out of range

**Thread Safety:**
- Acquires mutex for safe access

**Example:**
```c
const WifiNetwork* network = wifi_scanner_get_network(scanner, 0);
if(network) {
    printf("SSID: %s, RSSI: %d dBm\n", network->ssid, network->rssi);
}
```

---

### wifi_scanner_get_packet_count()

Get number of captured packets.

```c
uint16_t wifi_scanner_get_packet_count(const WifiScanner* scanner);
```

**Parameters:**
- `scanner` - Pointer to scanner instance

**Returns:**
- Number of captured packets

**Thread Safety:**
- Acquires mutex for safe access

**Example:**
```c
uint16_t count = wifi_scanner_get_packet_count(scanner);
printf("Captured %d packets\n", count);
```

---

### wifi_scanner_save_capture()

Save captured packets to file.

```c
bool wifi_scanner_save_capture(WifiScanner* scanner);
```

**Parameters:**
- `scanner` - Pointer to scanner instance

**Returns:**
- `true` on success
- `false` on file I/O error

**Description:**
- Creates `/ext/apps_data/flipagotcha/` directory
- Opens `capture.txt` file
- Writes packet header and data
- Formats packets as hex dump

**Example:**
```c
if(wifi_scanner_save_capture(scanner)) {
    printf("Packets saved\n");
} else {
    printf("Failed to save\n");
}
```

---

### wifi_scanner_is_connected()

Check if ESP32 is connected.

```c
bool wifi_scanner_is_connected(const WifiScanner* scanner);
```

**Parameters:**
- `scanner` - Pointer to scanner instance

**Returns:**
- `true` if ESP32 detected
- `false` if demo mode

**Example:**
```c
if(wifi_scanner_is_connected(scanner)) {
    printf("ESP32 connected\n");
} else {
    printf("Demo mode\n");
}
```

---

## UART API

**Module**: `src/hw/uart.c` / `uart.h`

### uart_alloc()

Allocate and initialize UART instance.

```c
Uart* uart_alloc(void);
```

**Returns:**
- Pointer to allocated `Uart` structure
- `NULL` on allocation failure

**Description:**
- Allocates UART structure
- Initializes serial port (115200, 8N1)
- Creates RX worker thread
- Allocates RX buffer (2KB)

**Example:**
```c
Uart* uart = uart_alloc();
if(!uart) {
    // Handle allocation failure
}
```

---

### uart_free()

Free UART resources.

```c
void uart_free(Uart* uart);
```

**Parameters:**
- `uart` - Pointer to UART instance

**Description:**
- Stops RX worker thread
- Releases serial port
- Frees buffers
- Frees structure

**Example:**
```c
uart_free(uart);
uart = NULL;
```

---

### uart_send()

Send data via UART.

```c
void uart_send(Uart* uart, const uint8_t* data, size_t length);
```

**Parameters:**
- `uart` - Pointer to UART instance
- `data` - Data to send
- `length` - Number of bytes

**Description:**
- Transmits data to ESP32
- Blocking operation
- No error checking (fire and forget)

**Example:**
```c
const char* cmd = "scanap\n";
uart_send(uart, (const uint8_t*)cmd, strlen(cmd));
```

---

### uart_set_callback()

Register callback for received data.

```c
void uart_set_callback(Uart* uart, UartCallback callback, void* context);
```

**Parameters:**
- `uart` - Pointer to UART instance
- `callback` - Function to call on data reception
- `context` - User context pointer

**Callback Signature:**
```c
typedef void (*UartCallback)(const uint8_t* data, size_t len, void* context);
```

**Description:**
- Registers callback invoked by RX worker
- Called when data received
- Runs in worker thread context

**Example:**
```c
void my_callback(const uint8_t* data, size_t len, void* context) {
    // Process received data
}

uart_set_callback(uart, my_callback, my_context);
```

---

### uart_is_connected()

Test if ESP32 is responding.

```c
bool uart_is_connected(Uart* uart);
```

**Parameters:**
- `uart` - Pointer to UART instance

**Returns:**
- `true` if ESP32 responds
- `false` if no response or timeout

**Description:**
- Sends `help` command
- Waits for response
- Parses for Marauder signature

**Example:**
```c
if(uart_is_connected(uart)) {
    printf("ESP32 detected\n");
} else {
    printf("ESP32 not found\n");
}
```

---

## Protocol Definitions

**Module**: `src/common/protocol.h`

### Commands

```c
// Marauder CLI commands
#define CMD_SCAN_AP       "scanap"
#define CMD_STOP_SCAN     "stopscan"
#define CMD_SNIFF_PMKID   "sniffpmkid -c %d -d"
#define CMD_SNIFF_RAW     "sniffraw"
#define CMD_HELP          "help"
```

### Constants

```c
#define MAX_NETWORKS          16    // Maximum networks to store
#define MAX_PACKET_SIZE       512   // Maximum packet size (bytes)
#define MAX_SSID_LEN          32    // Maximum SSID length
#define UART_RX_BUFFER_SIZE   2048  // UART receive buffer size
#define MARAUDER_CMD_BUFFER_SIZE 256 // Command buffer size
```

### Network Structure

```c
typedef struct {
    char ssid[MAX_SSID_LEN + 1];    // Network name (null-terminated)
    uint8_t bssid[6];                // MAC address
    int8_t rssi;                     // Signal strength (-100 to 0 dBm)
    uint8_t channel;                 // WiFi channel (1-14)
    uint8_t encryption;              // Encryption type
} WifiNetwork;
```

### Encryption Types

```c
typedef enum {
    WIFI_ENCRYPTION_OPEN = 0,
    WIFI_ENCRYPTION_WEP,
    WIFI_ENCRYPTION_WPA,
    WIFI_ENCRYPTION_WPA2,
    WIFI_ENCRYPTION_WPA3
} WifiEncryption;
```

### Packet Structure

```c
typedef struct {
    uint8_t data[MAX_PACKET_SIZE];   // Raw packet data
    uint16_t length;                 // Actual packet length
    uint32_t timestamp;              // Capture timestamp (ms)
    uint8_t channel;                 // WiFi channel
} CapturedPacket;
```

---

## UI Drawing API

**Module**: `src/ui/draw.c` / `draw.h`

### draw_face()

Draw animated face on canvas.

```c
void draw_face(Canvas* canvas, bool eyes_open, bool smiling);
```

**Parameters:**
- `canvas` - Flipper Zero canvas
- `eyes_open` - `true` for open eyes, `false` for closed
- `smiling` - `true` for smile, `false` for frown

**Description:**
- Draws eyes (filled or closed)
- Draws mouth (smile or frown curve)
- Centered on 128x64 screen

**Example:**
```c
// Draw happy face with open eyes
draw_face(canvas, true, true);

// Draw sleeping face
draw_face(canvas, false, true);

// Draw sad face
draw_face(canvas, true, false);
```

---

## Data Structures

### WifiScanner (Opaque)

```c
typedef struct WifiScanner WifiScanner;
```

**Internal Structure** (implementation detail):
```c
struct WifiScanner {
    Uart* uart;                          // UART instance
    FuriMutex* mutex;                   // Thread safety
    WifiNetwork networks[MAX_NETWORKS];  // Network list
    uint8_t network_count;              // Number of networks
    CapturedPacket* packets;            // Packet buffer
    uint16_t packet_count;              // Number of packets
    uint16_t packet_capacity;           // Buffer capacity
    bool scanning;                      // Scanning state
    bool esp32_connected;               // Connection status
};
```

### Uart (Opaque)

```c
typedef struct Uart Uart;
```

**Internal Structure** (implementation detail):
```c
struct Uart {
    FuriHalSerialHandle* serial_handle;  // Serial port
    FuriThread* rx_worker;              // RX thread
    uint8_t* rx_buffer;                 // Receive buffer
    size_t rx_buffer_size;              // Buffer size
    UartCallback callback;              // User callback
    void* callback_context;             // User context
    FuriMutex* mutex;                   // Thread safety
};
```

---

## Constants

### File Paths

```c
#define APP_DATA_DIR    "/ext/apps_data/flipagotcha"
#define CAPTURE_FILE    "/ext/apps_data/flipagotcha/capture.txt"
```

### Timing

```c
#define UART_CONNECTION_TEST_TIMEOUT_MS  5000  // Connection test timeout
#define BLINK_INTERVAL_MS                2000  // Eye blink interval
```

### UART Configuration

```c
#define UART_BAUD_RATE   115200
#define UART_GPIO_TX     13  // Flipper TX pin
#define UART_GPIO_RX     14  // Flipper RX pin
```

### Validation Ranges

```c
#define WIFI_CHANNEL_MIN  1     // Minimum WiFi channel
#define WIFI_CHANNEL_MAX  14    // Maximum WiFi channel
#define RSSI_MIN          -100  // Minimum RSSI (dBm)
#define RSSI_MAX          0     // Maximum RSSI (dBm)
```

---

## Error Codes

Currently, most functions return boolean status:
- `true` = Success
- `false` = Failure

Future enhancement: Detailed error codes.

---

## Thread Safety Notes

### Mutex-Protected Functions

These functions acquire internal mutex:
- `wifi_scanner_get_network_count()`
- `wifi_scanner_get_network()`
- `wifi_scanner_get_packet_count()`
- `wifi_scanner_start_handshake()`

### Non-Blocking Functions

All API functions are non-blocking except:
- `uart_send()` - Blocks until transmission complete
- File I/O in `wifi_scanner_save_capture()`

### Callback Context

UART callbacks run in worker thread context:
- Must not block
- Must not call long-running functions
- Should defer work to main thread if needed

---

## Usage Examples

### Complete Initialization

```c
// Allocate UART
Uart* uart = uart_alloc();

// Allocate scanner
WifiScanner* scanner = wifi_scanner_alloc(uart);

// Check connection
if(wifi_scanner_is_connected(scanner)) {
    printf("ESP32 ready\n");
} else {
    printf("Demo mode\n");
}

// Use scanner...

// Cleanup
wifi_scanner_free(scanner);
uart_free(uart);
```

### Scan Workflow

```c
// Start scan
wifi_scanner_start_scan(scanner);

// Wait for networks...
furi_delay_ms(30000);  // 30 seconds

// Check results
uint8_t count = wifi_scanner_get_network_count(scanner);
printf("Found %d networks\n", count);

// Print networks
for(uint8_t i = 0; i < count; i++) {
    const WifiNetwork* net = wifi_scanner_get_network(scanner, i);
    if(net) {
        printf("%s: %d dBm on channel %d\n", 
               net->ssid, net->rssi, net->channel);
    }
}

// Stop and save
wifi_scanner_stop_scan(scanner);
```

### Handshake Capture

```c
// Must have networks first
if(wifi_scanner_get_network_count(scanner) > 0) {
    // Start handshake capture
    wifi_scanner_start_handshake(scanner);
    
    // Wait for capture
    furi_delay_ms(20000);  // 20 seconds
    
    // Check packets
    uint16_t packets = wifi_scanner_get_packet_count(scanner);
    printf("Captured %d packets\n", packets);
    
    // Stop and save
    wifi_scanner_stop_scan(scanner);
}
```

---

## Version History

### v1.0.0
- Initial API release
- WiFi scanning
- Handshake capture
- Packet capture
- Demo mode

---

**Next**: [Contributing](Contributing.md) | [Architecture](Architecture.md)
