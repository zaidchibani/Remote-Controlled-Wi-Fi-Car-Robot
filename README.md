# Secure MQTT Robot Controller

A secure robot control system using MQTT protocol with ASCON-128 encryption. The system consists of a C++ controller that encrypts commands and an ESP32-based robot that receives, decrypts, and executes these commands.

## System Architecture

```
[C Controller] ---(encrypted MQTT)---> [MQTT Broker] ---> [ESP32 Robot]
     |                                                           |
  Encrypts with                                            Decrypts with
  ASCON-128                                                ASCON-128
```

## Components

### 1. C Controller (`c_controller.cpp`)
- Captures keyboard input (W/A/S/D/X)
- Encrypts commands using ASCON-128 cipher
- Publishes encrypted messages to MQTT broker
- Appends authentication tag for integrity verification

### 2. ESP32 Robot 
- Subscribes to MQTT topic
- Receives encrypted commands
- Decrypts messages using ASCON-128
- Verifies authentication tag
- Executes robot movements

### 3. MQTT Broker
- Mosquitto MQTT broker running on `localhost:1883`
- Handles message routing between controller and robot

## Security Features

- **ASCON-128 Authenticated Encryption**: Provides both confidentiality and integrity
- **Shared Secret Key**: 128-bit encryption key
- **Initialization Vector (IV)**: Prevents pattern analysis
- **Authentication Tag**: 16-byte tag ensures message integrity

## Hardware Requirements

### ESP32 Robot
- ESP32 Development Board
- 2-Wheel Robot Chassis
- Motor Driver (L298N or similar)
- DC Motors (2x)
- Power Supply

### Controller
- Any computer with C++ compiler
- Network connection to MQTT broker

## Software Requirements

### Controller
- C++ Compiler (g++)
- Paho MQTT C Library
- ASCON-128 Cryptographic Library
- Linux/Unix environment (for `termios.h`)

### ESP32
- Arduino IDE or PlatformIO
- Libraries:
  - WiFi (built-in)
  - PubSubClient
  - Ascon128

## Installation

### 1. Setup MQTT Broker

```bash
# Install Mosquitto
sudo apt-get update
sudo apt-get install mosquitto mosquitto-clients

# Start the broker
sudo systemctl start mosquitto
sudo systemctl enable mosquitto
```

### 2. Configure the Controller

Edit `c_controller.cpp` if needed:
```cpp
#define ADDRESS     "tcp://localhost:1883"  // MQTT broker address
#define TOPIC       "robot/cmd"              // MQTT topic
```

Compile:
```bash
g++ c_controller.cpp -o controller -lpaho-mqtt3c -lascon
```

### 3. Configure the ESP32

Update `esp.cpp` with your network settings:
```cpp
const char* ssid     = "YourWiFiSSID";
const char* password = "YourWiFiPassword";
const char* mqtt_server = "YOUR_PC_IP";  // IP address of MQTT broker
```

Upload to ESP32 using Arduino IDE or PlatformIO.

## Cryptographic Configuration

Both controller and ESP32 use the same encryption parameters:

```cpp
// Shared 128-bit key
uint8_t key[16] = {
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
  0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F
};

// Initialization Vector (Nonce)
uint8_t nonce[16] = {
  0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
  0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF
};
```

**⚠️ Security Warning**: Change these default values in production!

## Usage

### 1. Start the MQTT Broker
```bash
mosquitto -v
```

### 2. Upload and Run ESP32 Code
- Upload `esp.cpp` to your ESP32
- Open Serial Monitor (115200 baud) to see decrypted commands

### 3. Run the Controller
```bash
./controller
```

### Control Commands

| Key | Command | Action |
|-----|---------|--------|
| W   | FORWARD | Move forward |
| S   | BACK    | Move backward |
| A   | LEFT    | Turn left |
| D   | RIGHT   | Turn right |
| X   | STOP    | Stop motors |
| Q   | -       | Quit controller |

## Message Format

### Encrypted Payload Structure
```
[Ciphertext (variable length)] + [Authentication Tag (16 bytes)]
```

### Encryption Process
1. Controller reads keyboard input
2. Maps key to command string
3. Encrypts command with ASCON-128
4. Computes 16-byte authentication tag
5. Concatenates ciphertext + tag
6. Publishes to MQTT topic

### Decryption Process
1. ESP32 receives MQTT message
2. Separates ciphertext and tag
3. Resets cipher state (important!)
4. Decrypts ciphertext
5. Verifies authentication tag
6. Executes command if valid

## Troubleshooting

### ESP32 Not Receiving Messages
- Verify MQTT broker IP address in `esp.cpp`
- Check WiFi connection status
- Confirm MQTT topic matches on both sides
- Check firewall settings

### Tag Verification Failed
- Ensure key and nonce match on both devices
- Verify cipher state is reset before each operation
- Check payload length is at least 16 bytes

### Controller Won't Compile
- Install Paho MQTT library: `sudo apt-get install libpaho-mqtt-dev`
- Install ASCON library or add source files
- Verify all dependencies are installed

### WiFi Connection Issues
- Double-check SSID and password
- Ensure ESP32 is in range of WiFi router
- Check serial monitor for connection status

## Security Considerations

1. **Change Default Keys**: The provided key and nonce are for demonstration only
2. **Key Distribution**: Use secure methods to share keys between devices
3. **IV/Nonce Management**: Consider using unique nonces for each session
4. **Network Security**: Use TLS/SSL for MQTT in production environments
5. **Access Control**: Implement MQTT authentication and authorization

## Future Enhancements

- [ ] Add speed control (PWM)
- [ ] Implement obstacle detection
- [ ] Add autonomous navigation mode
- [ ] Support multiple robots
- [ ] Web-based control interface
- [ ] Dynamic key exchange protocol
- [ ] Implement counter-based nonce generation

## License

This project is provided as-is for educational purposes.

## Contributors
-Zaid Chibani
-ChihabEddin Zadem
## Acknowledgments

- ASCON cipher implementation
- Eclipse Paho MQTT library
- ESP32 Arduino framework
