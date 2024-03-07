# Digital Control System

## Train Location Tracker Project Documentation

### Summary:
This code is designed for an IoT project that involves connecting an ESP8266 module to WiFi, obtaining GPS coordinates, and transmitting this information along with some user input data (like Train ID and Route Number) to a server. It includes handling of different system states like connecting to WiFi, obtaining GPS data, and user input through a keypad. The code also uses an LCD for displaying information and error messages.

### Hardware Components:
- ESP8266 module
- GPS module
- Keypad
- LiquidCrystal I2C for the LCD display

### Libraries Used:
- ESP8266WiFi
- WiFiClient
- ESP8266HTTPClient
- ArduinoJson for JSON handling
- TinyGPSPlus for GPS parsing
- SoftwareSerial for serial communication
- LiquidCrystal_I2C for LCD display

### Functionalities:
- WiFi connectivity
- GPS data retrieval
- Data transmission to a server via HTTP POST request
- User input through a keypad
- Displaying information and errors on an LCD

### Detailed Documentation:

#### Constants:
- `NO_WIFI`, `NO_GPS`, `INET_ERR`: Error codes for different failure states.
- `WIFI_SSID`, `WIFI_PASSWORD`: Credentials for WiFi connection.
- `WIFI_ATTEMPT_COUNT`: Maximum number of attempts to connect to WiFi.
- `RXPin`, `TXPin`, `GPSBaud`: Configuration for GPS module communication.

#### Global Variables:
- `wifiClient`, `lcd`, `gps`, `ss`: Instances for WiFi, LCD, GPS, and software serial communication respectively.
- `doc`: JSON document for constructing the data payload.
- `currentState`: Tracks the current state of the system (e.g., connecting to WiFi, transmitting data).
- Various flags and counters for managing connections, setup completion, and data transmission.

#### Enums:
- `SystemState`: Enumerates possible states of the system such as `STATE_CONNECTING_WIFI`, `STATE_NO_GPS`, `STATE_TRANSMITTING`, etc.

#### Setup Function:
- Initializes serial communication, GPS module, and LCD.
- Tries to connect to WiFi and updates the system state accordingly.

#### Loop Function:
- Main loop that handles different states of the system and transitions between them based on conditions like WiFi connectivity, GPS data availability, and user actions.

#### Helper Functions:
- `showError`: Displays error messages on the LCD.
- `sendRequest`: Constructs a JSON payload with GPS data and user inputs, then sends it to a server using HTTP POST.
- `connectToWIFI`: Tries to connect to the predefined WiFi network and updates the system state.
- `getLocation`: Retrieves GPS coordinates and updates global variables.
- `encordGPS`: Processes incoming GPS data for a specified duration.
- `initLCD`: Initializes the LCD display.
- `captureInput`: Handles user input for setup data (like Train ID and Route Number).
- `keypadManager` and related functions: Manages user input through the keypad, allowing users to enter data, clear input, or submit the data.
- `updateLCD`: Updates the LCD display with current information or messages.

#### Keypad Configuration:
- Defines the layout and behavior of the keypad, including mappings for character input and special function keys.

### Key Points:
- The code is structured around a state machine approach, with different system states dictating the current functionality and behavior.
- User input is collected through a keypad, and feedback is provided via an LCD screen.
- GPS data and user inputs are sent to a server in JSON format over HTTP.
- The code includes error handling for common issues like WiFi connectivity problems and GPS signal unavailability.
