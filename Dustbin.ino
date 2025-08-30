#define BLYNK_TEMPLATE_ID "TMPL3LZLM-4OL"
#define BLYNK_TEMPLATE_NAME "dustbin"
#define BLYNK_AUTH_TOKEN "gNDcDOdb_DTH-xNOS21DYI26lBO_Wsi5"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>

// WiFi credentials
char ssid[] = "network";
char pass[] = "1122334455";

// Ultrasonic sensor pins
#define trigPin D1
#define echoPin D2

// LED and buzzer pins
#define led20 D5
#define led15 D6
#define led5 D7
#define buzzer D8

// SIM800L on SoftwareSerial (D3 = RX, D4 = TX)
SoftwareSerial sim800(D3, D4); // RX, TX

bool smsSent = false;  // Flag to prevent multiple SMS

void setup() {
  Serial.begin(9600);           // Serial monitor
  sim800.begin(9600);           // SIM800L communication

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Pin modes
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(led20, OUTPUT);
  pinMode(led15, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

void sendSMS(const char* message) {
  sim800.println("AT+CMGF=1");    // Text mode
  delay(100);
  sim800.println("AT+CMGS=\"+919699084613\"");
  delay(100);
  sim800.print(message);
  delay(100);
  sim800.write(26);
  delay(1000);
  Serial.println("SMS Sent");
}

void loop() {
  Blynk.run();

  long distance = getDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  // Send distance to Blynk (Virtual Pin V0)
  Blynk.virtualWrite(V0, distance);

  // LED and buzzer indicators
  digitalWrite(led20, distance <= 20);
  digitalWrite(led15, distance <= 15);
  digitalWrite(led5,  distance <= 5);
  digitalWrite(buzzer, distance <= 5);

  // Send SMS alert when bin is full
  if (distance <= 5 && !smsSent) {
    sendSMS("Alert: Dustbin is full!");
    smsSent = true;  // Prevent repeated messages
  } else if (distance > 5) {
    smsSent = false; // Reset SMS flag
  }

  delay(500);
}
