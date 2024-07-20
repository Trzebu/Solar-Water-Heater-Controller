#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D4
#define RELAY D0

// put function declarations here:
void update();
void checkTemperatures();
void debugTemperatures();
int myFunction(int, int);
void printSensorsAddresses();
void printAddressDecoded(DeviceAddress deviceAddress);
void printAddressRaw(DeviceAddress deviceAddress);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

unsigned long lastUpdate;

const uint8_t SOLAR_SENSOR_ADDR[8] = { 0x28, 0x9B, 0x95, 0x81, 0xE3, 0x5B, 0x3C, 0x0E };
const uint8_t BOILER_SENSOR_ADDR[8] = { 0x28, 0x0A, 0xB3, 0x81, 0xE3, 0x45, 0x3C, 0xDC };
const float SOLAR_HYSTERESIS = 12.0f;
const float BOILER_HYSTERESIS = 11.0f;

void setup() {
	Serial.begin(115200);
    pinMode(RELAY, OUTPUT);
	sensors.begin();
	Serial.println();
	// put your setup code here, to run once:
	//printSensorsAddresses();
	Serial.println("chuj");
}

void loop() {
  	// put your main code here, to run repeatedly:
	update();
}

void update () {
	if (millis() > (lastUpdate + 500))
		lastUpdate = millis();
	else return;

	sensors.requestTemperatures();
	checkTemperatures();
    debugTemperatures();
}

void checkTemperatures () {
	float solarTemp = sensors.getTempC(SOLAR_SENSOR_ADDR);
	float boilerTemp = sensors.getTempC(BOILER_SENSOR_ADDR);
    int relayStatus = digitalRead(RELAY);

	if (solarTemp >= (boilerTemp + SOLAR_HYSTERESIS)) {
        if (relayStatus != LOW)
            digitalWrite(RELAY, LOW);
	} else if ((boilerTemp + BOILER_HYSTERESIS) >= solarTemp) {
        if (relayStatus != HIGH)
            digitalWrite(RELAY, HIGH);
    }
}

void debugTemperatures () {
    Serial.print("Solar temp: ");
    Serial.println(sensors.getTempC(SOLAR_SENSOR_ADDR));
    Serial.print("Boiler temp: ");
    Serial.println(sensors.getTempC(BOILER_SENSOR_ADDR));
    Serial.println();
}

// put function definitions here:
int myFunction(int x, int y) {
  	return x + y;
}

void printSensorsAddresses () {
	Serial1.println("Printing addresses...");

	for (int i = 0; i < sensors.getDeviceCount(); i++) {
		Serial.print("Sensor ");
		Serial.print(i + 1);
		Serial.println(":");

		DeviceAddress address;
		sensors.getAddress(address, i);
		
		Serial.print("    decoded addr: ");
		printAddressDecoded(address);
		
		Serial.println();
		Serial.print("    raw addr: ");
		printAddressRaw(address);

		Serial.println();
	}
}

void printAddressDecoded (DeviceAddress deviceAddress) {
	for (uint8_t i = 0; i < 8; i++){
		if (deviceAddress[i] < 16) Serial.print("0");
		Serial.print(deviceAddress[i], HEX);
	}
}

void printAddressRaw (DeviceAddress deviceAddress) { 
	for (uint8_t i = 0; i < 8; i++) {
		Serial.print("0x");
		if (deviceAddress[i] < 0x10) Serial.print("0");
		Serial.print(deviceAddress[i], HEX);
		if (i < 7) Serial.print(", ");
	}
}