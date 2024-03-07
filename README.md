# Digital Control System

### Documentation for Train Location Data Transmitter

#### Overview
This code is designed for an ESP8266 microcontroller project that integrates GPS tracking, WiFi connectivity, a keypad for user input, and an LCD for display. It sends GPS data and additional information provided via the keypad to a server through WebSocket communication.

#### Hardware Requirements
- ESP8266 Microcontroller
- GPS Module (configured with RXPin on D8 and TXPin on D7, 9600 baud rate)
- Liquid Crystal Display (LCD) with I2C interface
- 4x4 Keypad
- Software Serial for GPS data communication

#### Software Libraries
- ESP8266WiFi: For connecting to WiFi networks
- WebSocketsClient: For WebSocket communication
- ArduinoJson: For JSON data handling
- TinyGPSPlus: For parsing GPS module data
- SoftwareSerial: For serial communication with GPS module
- LiquidCrystal_I2C: For interfacing with the LCD
- Keypad: For interfacing with the 4x4 keypad

#### Main Components
- `WiFiClient`, `LiquidCrystal_I2C`, `TinyGPSPlus`, `SoftwareSerial`, `StaticJsonDocument`: Instances for handling WiFi, LCD, GPS, serial communication, and JSON data respectively.
- `SystemState`: Enum defining various states of the system including WiFi connection states, setup, data transmission, and error handling.
- `setup()`: Initializes serial communication, GPS module, LCD, and tries to connect to WiFi. Sets up WebSocket communication.
- `loop()`: Core loop handling different system states and transitions between them, such as connecting to WiFi, handling user input through the keypad, reading GPS data, and transmitting data.
- `connectToWIFI()`: Attempts to connect to a predefined WiFi network and updates the system state based on the outcome.
- `getLocation()`: Retrieves the current location from the GPS module and updates internal state if GPS data is not available.
- `sendRequest()`: Packages the GPS data and additional information into a JSON object and sends it through the WebSocket.
- `webSocketEvent()`: Handles incoming WebSocket events such as disconnections and receiving text messages.
- `keypadManager()`: Manages keypad input, updating system state or user input based on key presses.
- `updateLCD()`: Updates the LCD display based on the current system state and information.

#### Functional Flow
1. The system initializes and attempts to connect to the WiFi network.
2. Once connected, it enters a setup state where user inputs can be entered through the keypad.
3. After setup, the system periodically reads GPS data and transmits it along with the setup information to a server via WebSocket.
4. The system can display error messages or status information on the LCD based on its current state.

#### Error Handling
- Defined error codes for no WiFi (`NO_WIFI`), no GPS signal (`NO_GPS`), and internet connection issues (`INET_ERR`).
- System state and LCD are updated to reflect error conditions.

#### Customization
- WiFi SSID, password, and server details can be customized according to user requirements.
- Additional user inputs and system states can be added by modifying the `SystemState` enum and adjusting the `loop()` and `keypadManager()` functions.

