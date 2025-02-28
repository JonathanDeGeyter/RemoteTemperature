Temperature Monitoring and Heating Control System with ESP32 and Blynk
This project uses an ESP32 microcontroller to monitor temperatures from two different sensors and control a heating system based on the temperature readings. The system can be managed remotely through the Blynk app. It includes features for adjusting temperature thresholds, toggling heating on and off, and applying both manual and potentiometer-based temperature boosts. A warning system is implemented to alert when the temperature exceeds the defined threshold.

Features:
One-Wire Temperature Sensor: Measures temperature using the Dallas Temperature library and the OneWire bus protocol.
ADS1115 ADC: An external Analog-to-Digital Converter (ADC) is used for reading the temperature from a secondary sensor.
Blynk Integration: Enables remote control and monitoring of temperature, heating, and boosts via the Blynk app.
Manual and Potentiometer Boost: Provides options to increase the temperature readings either manually through the app or automatically through a potentiometer.
Temperature Threshold Alert: Sends an alert through the Blynk app and turns on an LED when both temperature readings exceed the defined threshold.
Heating Control: Allows the user to toggle the heating system on or off remotely via the Blynk app.
Components:
ESP32: Microcontroller to control the system.
Dallas Temperature Sensor: Measures the temperature on the OneWire bus.
ADS1115 ADC: Used for an additional temperature sensor reading.
Potentiometer: Used to adjust the temperature boost dynamically.
Red LED: Indicates when both temperatures exceed the threshold.
Relay/Heater Control Pin: Controls the heating system based on the Blynk app setting.
Software Libraries:
OneWire: For interfacing with the Dallas Temperature sensor.
DallasTemperature: For reading temperatures from the Dallas sensor.
Wire: For I2C communication with the ADS1115 ADC.
Adafruit_ADS1X15: For interfacing with the ADS1115 ADC.
WiFi: For connecting to Wi-Fi.
BlynkSimpleEsp32: For communication with the Blynk app.
TimeLib: For managing and syncing time.
BlynkTimer: For periodic tasks, like sensor readings.
How It Works:
The system reads temperatures from both a Dallas Temperature sensor and an ADS1115 ADC.
The temperatures can be adjusted using both a manual input through the Blynk app and an analog potentiometer.
If the temperatures exceed the defined threshold, a warning is triggered, the heating is turned off, and a red LED is activated.
The heating control can be toggled on/off using the Blynk app.
Setup Instructions:
Clone or download this repository.
Install the required libraries:
OneWire
DallasTemperature
Adafruit_ADS1X15
BlynkSimpleEsp32
WiFi
Open the Arduino IDE and upload the code to your ESP32.
Connect the hardware components:
OneWire sensor to GPIO 4
Potentiometer to ADC pin (GPIO 34)
Red LED to GPIO 5
Heating Control to GPIO 16
Configure your Wi-Fi credentials and Blynk authentication token in the code.
Open the Blynk app, create a new project, and add widgets to control the temperature, toggle heating, and monitor the potentiometer and temperature data.
Blynk App Widgets:
V1: Set the temperature threshold.
V4: Toggle heating (ON/OFF).
V6: Set a manual boost value.
V2 & V3: Display temperature readings from the OneWire sensor and ADS1115.
V7: Display the potentiometer boost value.
Notes:
The potentiometer boost adjusts the temperature readings based on its position, with a maximum boost of 20°C.
The system alerts if both temperatures exceed the set threshold, helping ensure the heating system operates safely.
