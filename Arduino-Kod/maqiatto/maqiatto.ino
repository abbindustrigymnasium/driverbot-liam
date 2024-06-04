#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <Servo.h>
#include "MQTTConnector.h"
#include "Credentials.h"

#define WIFI_TIMEOUT 1000
#define LOOP_TIMEOUT 0

WiFiClient wifiClient; // Skapar en instans av WiFi-klienten
Servo myServo; // Skapar en instans av Servo-motorn
PubSubClient mqttClient(wifiClient); // Skapar en instans av MQTT-klienten med WiFi-klienten

boolean mqttInitCompleted = false; // En flagga som indikerar om MQTT-initiering är klar
String clientId = "IoTPractice-" + String(ESP.getChipId()); // Unikt klient-ID baserat på ESP-chippet

/* Callback-funktion för inkommande data från MQTT-broker. */
void dataCallback(char* topic, byte* payload, unsigned int length)
{
  char payloadStr[length + 1];
  memset(payloadStr, 0, length + 1);
  strncpy(payloadStr, (char*)payload, length);
  Serial.printf("Data    : dataCallback. Ämne: [%s]\n", topic);
  Serial.printf("Data    : dataCallback. Innehåll: %s\n", payloadStr);
  
  // Kontrollera vilket ämne meddelandet kommer ifrån och utför rätt kodblock.
  if (strcmp(topic, TOPIC) == 0) {
    // Logik för motorstyrning
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
    Serial.print("Motorhastighet inställd på ");
    Serial.print(motorSpeed);
    Serial.println(" PWM.");
  } else if (strcmp(topic, TOPIC2) == 0) {


    // Logik för servostyrning
    if (strcmp(payloadStr, "Höger") == 0) {
      // Sväng vänster 45 grader
      int newAngle = myServo.read() - 90;
      if (newAngle < 0) newAngle = 0; // Se till att vinkeln inte går under 0
      myServo.write(newAngle);
      Serial.println("Servo svängde 45 grader åt vänster.");
    } else if (strcmp(payloadStr, "Vänster") == 0) {
      // Sväng höger 45 grader
      int newAngle = myServo.read() + 90;
      if (newAngle > 180) newAngle = 180; // Se till att vinkeln inte överstiger 180
      myServo.write(newAngle);
      Serial.println("Servo svängde 45 grader åt höger.");
    }
  }
}

// Funktion för att utföra anslutning till MQTT-broker
void performConnect()
{
  uint16_t connectionDelay = 5000;
  while (!mqttClient.connected())
  {
    Serial.printf("Spår    : Försöker ansluta till MQTT...\n");
    if (mqttClient.connect(clientId.c_str(), MQTT_USERNAME, MQTT_KEY))
    {
      Serial.printf("Spår    : Ansluten till Mäklare.\n");

      // Prenumeration på ämnen efter lyckad anslutning
      MQTTSubscribe(TOPIC);
      MQTTSubscribe(TOPIC2);
    }
    else
    {
      Serial.printf("Fel!    : MQTT-anslutning misslyckades, rc = %d\n", mqttClient.state());
      Serial.printf("Spår    : Försöker igen om %d ms.\n", connectionDelay);
      delay(connectionDelay);
    }
  }
}

// Funktion för att prenumerera på ett ämne
// retval = returnvalue
boolean MQTTSubscribe(const char* topicToSubscribe)
{
  boolean retval = false;
  if (mqttClient.connected())
  {
    retval = mqttClient.subscribe(topicToSubscribe);
  }
  return retval;
}

// Funktion för att kontrollera om MQTT-klienten är ansluten
boolean MQTTIsConnected()
{
  return mqttClient.connected();
}

// Funktion för att initiera MQTT-klienten och ange server och callback-funktion
void MQTTBegin()
{
  mqttClient.setServer(MQTT_BROKER, MQTT_BROKER_PORT);
  mqttClient.setCallback(dataCallback);
  mqttInitCompleted = true;
}

// Funktion för att hantera MQTT-loopen
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

// Funktion för att initiera WiFi-anslutning
void WiFiBegin(const char* ssid, const char* pass)
{
  WiFi.begin(ssid, pass);
  Serial.printf("Väntar på AP-anslutning ...\n");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(WIFI_TIMEOUT);
    Serial.printf(".");
  }
  IPAddress ip = WiFi.localIP();
  Serial.printf("\nAnsluten till AP. IP : %d.%d.%d.%d\n", 
    ip[0],ip[1],ip[2],ip[3]);
}

// Funktion för att återansluta till MQTT-broker
void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Försöker ansluta till MQTT...");
    if (mqttClient.connect("ESP8266Client", MQTT_USERNAME, MQTT_KEY)) {
      // Om allt går rätt, prenumerera på båda ämnena
      Serial.println("MQTT ansluten");
      mqttClient.subscribe(TOPIC2, 0);
      mqttClient.subscribe(TOPIC, 0);
      break;
    } else {
      Serial.print("Misslyckades med att ansluta till MQTT, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Försöker igen om 5 sekunder...");
      delay(5000);
      break; 
    }
  }
}

// Setup-funktion för att initiera komponenter och anslutningar och ge ström
void setup() 
{
  pinMode(motorSpeedPin, OUTPUT);
  pinMode(motorDirPin, OUTPUT);
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  
  // Initiera servon
  myServo.attach(D4); // Ersätt SERVO_PIN med det pinnummer som servon är ansluten till

  WiFiBegin(STA_SSID, STA_PASS);
  MQTTBegin();
}

// Loop-funktion för att upprepa MQTT-loop
void loop() 
{
  MQTTLoop();
}
