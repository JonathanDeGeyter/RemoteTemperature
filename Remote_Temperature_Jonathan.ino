#define BLYNK_TEMPLATE_ID "user2"
#define BLYNK_TEMPLATE_NAME "user2@server.wyns.it"
#define BLYNK_PRINT Serial

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <TimeLib.h>

char auth[] = "42_P6TtZAyaQpg6t0mdfWF7j-q0nMbiL";
char ssid[] = "WiFi-2.4-6A9C_EXT";
char pass[] = "w2p7k76kpcrfu";

#define ONE_WIRE_BUS 4
#define RED_LED_PIN 5
#define HEATING_CONTROL_PIN 16
#define POTENTIOMETER_PIN 34  // Use ADC1 pin (32, 33, 34, 35, 36, 39)

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Adafruit_ADS1115 ads;
WidgetTerminal terminal(V5);
BlynkTimer timer;

const int sensorPin = 0;
const float V_supply = 3.3;
const float R_fixed = 2200.0;
const float R_nominal = 2000.0;
const float sensitivity = 40.0;

int numberOfDevices;
DeviceAddress tempDeviceAddress;
float tempThreshold = 30.0;
bool heatingEnabled = true;
float manualBoost = 0.0;
float potBoost = 0.0;

BLYNK_WRITE(V1) {  // Set temperature threshold via Blynk (V1)
  tempThreshold = param.asFloat();
  Serial.print("Updated temperature threshold: ");
  Serial.println(tempThreshold);
}

BLYNK_WRITE(V4) {  // Toggle heating ON/OFF via Blynk (V4)
  heatingEnabled = param.asInt();
  digitalWrite(HEATING_CONTROL_PIN, heatingEnabled ? HIGH : LOW);
  Serial.print("Heating is ");
  Serial.println(heatingEnabled ? "ON" : "OFF");
}

BLYNK_WRITE(V6) {  // Set manual boost via Blynk (V6)
  manualBoost = param.asFloat();
  Serial.print("Manual boost set to: ");
  Serial.println(manualBoost);
}

float readPotentiometer() {
  int raw = analogRead(POTENTIOMETER_PIN);
  
  // ADC Fix: ESP32 reads high values when floating, ensure proper wiring
  if (raw < 50) raw = 0;

  // Smooth values using basic averaging
  static float smoothedValue = 0;
  smoothedValue = (smoothedValue * 0.8) + (raw * 0.2);

  // Increase potentiometer range to 20°C (instead of 10°C)
  float mappedBoost = map(smoothedValue, 0, 4095, 0, 20);  // Boost can go up to 20°C
  return mappedBoost;
}

void sendSensorData() {
  if (heatingEnabled) {
    sensors.requestTemperatures();
    float tempOneWire = sensors.getTempCByIndex(0);

    int16_t adcValue = ads.readADC_SingleEnded(sensorPin);
    float voltage = adcValue * (V_supply / 32768.0);
    float R_sensor = R_fixed * (V_supply / voltage - 1);
    float tempADS1115 = (R_sensor - R_nominal) / sensitivity;

    // Read potentiometer boost
    potBoost = readPotentiometer();

    // Apply both boosts independently
    tempOneWire += potBoost;
    tempADS1115 += potBoost;

    // Apply manual boost directly from the app
    tempOneWire += manualBoost;
    tempADS1115 += manualBoost;

    // Serial monitor output
    Serial.print("OneWire Temp C: ");
    Serial.print(tempOneWire);
    Serial.print(" | ADS1115 Temp C: ");
    Serial.print(tempADS1115);
    Serial.print(" | Pot Boost: ");
    Serial.print(potBoost);
    Serial.print(" | Manual Boost: ");
    Serial.print(manualBoost);
    Serial.println();

    // Send data to Blynk
    Blynk.virtualWrite(V2, tempOneWire);
    Blynk.virtualWrite(V3, tempADS1115);
    Blynk.virtualWrite(V7, potBoost);  // Show potentiometer boost value on app

    // Check if temperatures exceed threshold
    if (tempOneWire >= tempThreshold && tempADS1115 >= tempThreshold) {
      digitalWrite(RED_LED_PIN, HIGH);
      String timeStamp = String(hour()) + ":" + String(minute()) + ":" + String(second());
      Serial.println("WARNING: Both temperatures exceeded threshold! TURN OFF HEATER!");
      terminal.print(timeStamp + " - WARNING: Both temperatures exceeded threshold! TURN OFF HEATER!\n");
      terminal.flush();
    } else {
      digitalWrite(RED_LED_PIN, LOW);
    }
  } else {
    Serial.println("Heating is OFF. No temperature readings.");
    Blynk.virtualWrite(V2, "-");
    Blynk.virtualWrite(V3, "-");
  }
}

void setup() {
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);
  pinMode(HEATING_CONTROL_PIN, OUTPUT);
  digitalWrite(HEATING_CONTROL_PIN, LOW);
  pinMode(POTENTIOMETER_PIN, INPUT);

  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  Blynk.begin(auth, ssid, pass, "server.wyns.it", 8081);
  setSyncInterval(10 * 60);  // Sync time every 10 minutes

  sensors.begin();
  numberOfDevices = sensors.getDeviceCount();

  Wire.begin();
  if (!ads.begin()) {
    Serial.println("Cannot initialize ADS1115.");
    while (1);
  }
  ads.setGain(GAIN_ONE);
  ads.setDataRate(RATE_ADS1115_860SPS);

  Serial.println("Sensors Initialized.");

  timer.setInterval(5000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
