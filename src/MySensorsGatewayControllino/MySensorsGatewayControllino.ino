// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable gateway ethernet module type
#define MY_GATEWAY_W5100

// W5100 Ethernet module SPI enable (optional if using a shield/module that manages SPI_EN signal)
#define MY_W5100_SPI_EN 66

//#define MY_IP_ADDRESS 192,168,137,67   // If this is disabled, DHCP is used to retrieve address
// Renewal period if using DHCP
//#define MY_IP_RENEWAL_INTERVAL 60000
// The port to keep open on node server mode / or port to contact in client mode
#define MY_PORT 5003

// Controller ip address. Enables client mode (default is "server" mode).
// Also enable this if MY_USE_UDP is used and you want sensor data sent somewhere.
//#define MY_CONTROLLER_IP_ADDRESS 192, 168, 178, 254

// The MAC address can be anything you want but should be unique on your network.
// Newer boards have a MAC address printed on the underside of the PCB, which you can (optionally) use.
// Note that most of the Ardunio examples use  "DEAD BEEF FEED" for the MAC address.
#define MY_MAC_ADDRESS 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED

// Set blinking period
#define MY_DEFAULT_LED_BLINK_PERIOD 300

// Flash leds on rx/tx/err
// Uncomment to override default HW configurations
//#define MY_DEFAULT_ERR_LED_PIN 7  // Error led pin
//#define MY_DEFAULT_RX_LED_PIN  8  // Receive led pin
//#define MY_DEFAULT_TX_LED_PIN  9  // Transmit led pin

#include <Ethernet.h>
#include <MySensors.h>
#include <Controllino.h>
#include <ArduinoJson.h>
#include "ControllinoConstants.h"

uint8_t inputToOutputMap[CONTROLLINO_INPUTS_COUNT] = {
  CONTROLLINO_D5,
  CONTROLLINO_D1,
  CONTROLLINO_D2,
  CONTROLLINO_D3,
  CONTROLLINO_D4,
  CONTROLLINO_D5,
  CONTROLLINO_D6,
  CONTROLLINO_D7,
  CONTROLLINO_D8,
  CONTROLLINO_D9,
  CONTROLLINO_D10,
  CONTROLLINO_D11,
  CONTROLLINO_D12,
  CONTROLLINO_D13,
  CONTROLLINO_D14,
  CONTROLLINO_D15,
  CONTROLLINO_D16,
  CONTROLLINO_D17,
  CONTROLLINO_D18
};

uint8_t inputsOldValues[CONTROLLINO_INPUTS_COUNT];

void setup()
{
  for(int i = 0; i < CONTROLLINO_RELAYS_COUNT; i++) {
    pinMode(CONTROLLINO_RELAYS[i], OUTPUT);
  }

  for(int i = 0; i < CONTROLLINO_OUTPUTS_COUNT; i++) {
    pinMode(CONTROLLINO_OUTPUTS[i], OUTPUT);
  }
  
  for(int i = 0; i < CONTROLLINO_INPUTS_COUNT; i++) {
    pinMode(CONTROLLINO_INPUTS[i], INPUT);
  }
}

void onClick(uint8_t input)
{
  /*if (client.connect("192.168.137.1", 5000)) {
    Serial.println("connected");

    StaticJsonBuffer<200> jsonBuffer;

    JsonObject& root = jsonBuffer.createObject();
    root["token"] = "xxx";
    root["eventType"] = "click";
    root["args"] = input;
    
    // Make a HTTP request:
    client.println("POST /api/events/report HTTP/1.1");
    client.println("Host: 192.168.137.1:5000");
    client.println("Content-Type: application/json;charset=utf-8");
    client.println("Connection: close");
    client.println();
    root.printTo(client);
    client.println();
  } else {
    client.stop();

    // if you didn't get a connection to the server:
    Serial.println("connection failed");
    
    uint8_t output = inputToOutputMap[input];
  
    digitalWrite(output, !digitalRead(output));
  }*/

  if (client.connected()) {
    
    MyMessage message(input, V_STATUS);

    message.set("1");
    
    send(message);
  }
  else {
    
    uint8_t output = inputToOutputMap[input];
  
    digitalWrite(output, !digitalRead(output));
  }
}

void detectClicks()
{
  for(int i = 0; i < CONTROLLINO_INPUTS_COUNT; i++) {
    
    uint8_t value = digitalRead(CONTROLLINO_INPUTS[i]);
    
    if (value != inputsOldValues[i] && value == 1)
    {
      onClick(i);
    }
    
    inputsOldValues[i] = value;
  }
}

void loop()
{
  detectClicks();
  sleep(5); /* Sleep for button debounce */
}

void receive(const MyMessage &message)
{
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type == V_STATUS) {
    // Change relay state
    digitalWrite(CONTROLLINO_RELAYS[message.sensor], message.getBool() ? HIGH : LOW);
    // Store state in eeprom
    //saveState(message.sensor, message.getBool());
    // Write some debug info
    Serial.print("Incoming change for sensor: ");
    Serial.print(message.sensor);
    Serial.print(", Pin: ");
    Serial.print(CONTROLLINO_RELAYS[message.sensor]);
    Serial.print(", New status: ");
    Serial.println(message.getBool() ? HIGH : LOW);
  }
}
