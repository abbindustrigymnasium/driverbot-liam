#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <Servo.h>
#include "MQTTConnector.h"
#include "Credentials.h"

#define WIFI_TIMEOUT 1000
#define LOOP_TIMEOUT 0

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "MQTTConnector.h"
#include "Credentials.h"
#define motorSpeedPin 5 
#define motorDirPin 0
WiFiClient wifiClient;
Servo myServo;
PubSubClient mqttClient(wifiClient);

boolean mqttInitCompleted = false;
String clientId = "IoTPractice-" + String(ESP.getChipId());

/* Incoming data callback. */
void dataCallback(char* topic, byte* payload, unsigned int length)
{
  char payloadStr[length + 1];
  memset(payloadStr, 0, length + 1);
  strncpy(payloadStr, (char*)payload, length);
  Serial.printf("Data    : dataCallback. Topic : [%s]\n", topic);
  Serial.printf("Data    : dataCallback. Payload : %s\n", payloadStr);
  
  if (strcmp(topic, TOPIC) == 0) {
    // Motor control logic (unchanged)
    int motorSpeed;
    if (strcmp(payloadStr, "Fram") == 0) {
      motorSpeed = 512;
      digitalWrite(motorDirPin, 0);
    } else if (strcmp(payloadStr, "Bak") == 0) {
      motorSpeed = 512;
      digitalWrite(motorDirPin, 1);
    } else if (strcmp(payloadStr, "Stop") == 0) {
      motorSpeed = 0;
    }
    analogWrite(motorSpeedPin, motorSpeed);
    Serial.print("Motor speed set to ");
    Serial.print(motorSpeed);
    Serial.println(" PWM.");
  } else if (strcmp(topic, TOPIC2) == 0) {
    // Servo control logic
    if (strcmp(payloadStr, "Höger") == 0) {
      // Turn left 45 degrees
      int newAngle = myServo.read() - 90;
      if (newAngle < 0) newAngle = 0; // Ensure angle doesn't go below 0
      myServo.write(newAngle);
      Serial.println("Servo turned 45 degrees left.");
    } else if (strcmp(payloadStr, "Vänster") == 0) {
      // Turn right 45 degrees
      int newAngle = myServo.read() + 90;
      if (newAngle > 180) newAngle = 180; // Ensure angle doesn't exceed 180
      myServo.write(newAngle);
      Serial.println("Servo turned 45 degrees right.");
    }
  }
}

void performConnect()
{
  uint16_t connectionDelay = 5000;
  while (!mqttClient.connected())
  {
    Serial.printf("Trace   : Attempting MQTT connection...\n");
    if (mqttClient.connect(clientId.c_str(), MQTT_USERNAME, MQTT_KEY))
    {
      Serial.printf("Trace   : Connected to Broker.\n");

      /* Subscription to your topic after connection was succeeded.*/
      MQTTSubscribe(TOPIC);
      MQTTSubscribe(TOPIC2);
    }
    else
    {
      Serial.printf("Error!  : MQTT Connect failed, rc = %d\n", mqttClient.state());
      Serial.printf("Trace   : Trying again in %d msec.\n", connectionDelay);
      delay(connectionDelay);
    }
  }
}

boolean MQTTSubscribe(const char* topicToSubscribe)
{
  boolean retval = false;
  if (mqttClient.connected())
  {
    retval = mqttClient.subscribe(topicToSubscribe);
  }
  return retval;
}

boolean MQTTIsConnected()
{
  return mqttClient.connected();
}

void MQTTBegin()
{
  mqttClient.setServer(MQTT_BROKER, MQTT_BROKER_PORT);
  mqttClient.setCallback(dataCallback);
  mqttInitCompleted = true;
}

void MQTTLoop()
{
  if(mqttInitCompleted)
  {
    if (!MQTTIsConnected())
    {
      performConnect();
    }
    mqttClient.loop();
  }
}

void WiFiBegin(const char* ssid, const char* pass)
{
  WiFi.begin(ssid, pass);
  Serial.printf("Waiting for AP connection ...\n");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(WIFI_TIMEOUT);
    Serial.printf(".");
  }
  IPAddress ip = WiFi.localIP();
  Serial.printf("\nConnected to AP. IP : %d.%d.%d.%d\n", 
    ip[0],ip[1],ip[2],ip[3]);
}


void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (mqttClient.connect("ESP8266Client", MQTT_USERNAME, MQTT_KEY)) {
      //Om allt går rätt, subscribea till båda topics
      Serial.println("MQTT connected");
      mqttClient.subscribe(TOPIC2, 0);
      mqttClient.subscribe(TOPIC, 0);
      break;
    } else {
      Serial.print("Failed to connect to MQTT, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
      break; 
    }
  }
}


void setup() 
{
  pinMode(motorSpeedPin, OUTPUT);
  pinMode(motorDirPin, OUTPUT);
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  
  // Initialize the servo
  myServo.attach(D4); // Replace SERVO_PIN with the pin number to which the servo is connected

  WiFiBegin(STA_SSID, STA_PASS);
  MQTTBegin();
}

void loop() 
{
  MQTTLoop();
}
