/*#include <WiFi.h>
#include <PubSubClient.h>

// ================= WIFI =================
const char* WIFI_SSID = "Infinix hot 30i";
const char* WIFI_PASS = "chiheb1234";

// ================= MQTT =================
const char* MQTT_BROKER = "broker.hivemq.com";
const int   MQTT_PORT   = 1883;
const char* MQTT_TOPIC  = "robot/control";

// ================= ROBOT PINS (2 wheels) =================
const int ML_F = 14;
const int ML_B = 12;
const int MR_F = 27;
const int MR_B = 26;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// ================= ROBOT CONTROL =================
void stopMotors() {
  digitalWrite(ML_F, LOW);
  digitalWrite(ML_B, LOW);
  digitalWrite(MR_F, LOW);
  digitalWrite(MR_B, LOW);
}

void moveRobot(String cmd) {
  stopMotors();

  if (cmd == "FORWARD") {
    digitalWrite(ML_F, HIGH);
    digitalWrite(MR_F, HIGH);
  }
  else if (cmd == "BACK") {
    digitalWrite(ML_B, HIGH);
    digitalWrite(MR_B, HIGH);
  }
  else if (cmd == "LEFT") {
    digitalWrite(ML_B, HIGH);
    digitalWrite(MR_F, HIGH);
  }
  else if (cmd == "RIGHT") {
    digitalWrite(ML_F, HIGH);
    digitalWrite(MR_B, HIGH);
  }
  else if (cmd == "STOP") {
    stopMotors();
  }
}

// ================= MQTT CALLBACK =================
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String cmd = "";
  for (unsigned int i = 0; i < length; i++) {
    cmd += (char)payload[i];
  }
  cmd.trim();

  Serial.print("[MQTT] ");
  Serial.println(cmd);

  moveRobot(cmd);
}

// ================= CONNECT MQTT =================
void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect("ESP32_ROBOT")) {
      Serial.println("OK");
      mqttClient.subscribe(MQTT_TOPIC);
    } else {
      Serial.println("FAILED");
      delay(2000);
    }
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(ML_F, OUTPUT);
  pinMode(ML_B, OUTPUT);
  pinMode(MR_F, OUTPUT);
  pinMode(MR_B, OUTPUT);
  stopMotors();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
}

// ================= LOOP =================
void loop() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();
}*//*
#include <WiFi.h>
#include <PubSubClient.h>
#include <Ascon128.h>

// =========================
// ===== Wi-Fi Setup =======
const char* ssid     = "Infinix hot 30i";
const char* password = "chiheb1234";

// =========================
// ===== MQTT Setup ========
const char* mqtt_server = "10.148.198.233"; // replace with your PC IP
const int mqtt_port = 1883;
const char* mqtt_topic = "robot/cmd";

WiFiClient espClient;
PubSubClient client(espClient);

// =========================
// ===== ASCON Setup =======
Ascon128 ascon;
uint8_t key[16] = {
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
  0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F
};
uint8_t nonce[16] = {
  0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
  0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF
};

// =========================
// ===== MQTT Callback =====
void callback(char* topic, byte* payload, unsigned int length) {
  if (length < 16) {
    Serial.println("❌ Payload too short, cannot contain tag!");
    return;
  }

  uint8_t tag[16];
  uint8_t ciphertext[length - 16];
  memcpy(ciphertext, payload, length - 16);
  memcpy(tag, payload + length - 16, 16);

  uint8_t decrypted[length - 16];
  ascon.decrypt(decrypted, ciphertext, length - 16);

  // Verify tag
  if (!ascon.checkTag(tag, 16)) {
    Serial.println("❌ Tag verification failed!");
    return;
  }

  String command = "";
  for (unsigned int i = 0; i < length - 16; i++) {
    command += (char)decrypted[i];
  }

  Serial.print("✅ Decrypted command: ");
  Serial.println(command);
}

// =========================
// ===== MQTT Reconnect =====
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Robot")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5s");
      delay(5000);
    }
  }
}

// =========================
// ===== Setup =============
void setup() {
  Serial.begin(115200);
  delay(1000);

  // ASCON init
  ascon.setKey(key, sizeof(key));
  ascon.setIV(nonce, sizeof(nonce));

  // Connect Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Setup MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// =========================
// ===== Loop ==============
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
*/
#include <WiFi.h>
#include <PubSubClient.h>
#include <Ascon128.h>

// =========================
// ===== Wi-Fi Setup =======
const char* ssid     = "Infinix hot 30i";
const char* password = "chiheb1234";

// =========================
// ===== MQTT Setup ========
const char* mqtt_server = "10.148.198.233"; // replace with your PC IP
const int mqtt_port = 1883;
const char* mqtt_topic = "robot/cmd";

WiFiClient espClient;
PubSubClient client(espClient);

// =========================
// ===== ASCON Setup =======
Ascon128 ascon;
uint8_t key[16] = {
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
  0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F
};
uint8_t nonce[16] = {
  0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
  0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF
};

// =========================
// ===== MQTT Callback =====
void callback(char* topic, byte* payload, unsigned int length) {
  if (length < 16) {
    Serial.println("❌ Payload too short, cannot contain tag!");
    return;
  }

  // ===== Reset cipher state before each decryption =====
  ascon.clear();
  ascon.setKey(key, sizeof(key));
  ascon.setIV(nonce, sizeof(nonce));
  // ====================================================

  uint8_t tag[16];
  uint8_t ciphertext[length - 16];
  memcpy(ciphertext, payload, length - 16);
  memcpy(tag, payload + length - 16, 16);

  uint8_t decrypted[length - 16];
  ascon.decrypt(decrypted, ciphertext, length - 16);

  // Verify tag
  if (!ascon.checkTag(tag, 16)) {
    Serial.println("❌ Tag verification failed!");
    return;
  }

  String command = "";
  for (unsigned int i = 0; i < length - 16; i++) {
    command += (char)decrypted[i];
  }

  Serial.print("✅ Decrypted command: ");
  Serial.println(command);
}

// =========================
// ===== MQTT Reconnect =====
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Robot")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5s");
      delay(5000);
    }
  }
}

// =========================
// ===== Setup =============
void setup() {
  Serial.begin(115200);
  delay(1000);

  // ASCON init
  ascon.setKey(key, sizeof(key));
  ascon.setIV(nonce, sizeof(nonce));

  // Connect Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Setup MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// =========================
// ===== Loop ==============
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}