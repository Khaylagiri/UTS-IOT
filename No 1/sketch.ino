#include "DHTesp.h"
#include "PubSubClient.h"
#include "WiFi.h"

#define DHTPIN 18           // Pin DHT
#define DHTTYPE DHT22       // Tipe sensor DHT
#define LED_GREEN 5         // Pin LED Hijau
#define LED_YELLOW 2        // Pin LED Kuning
#define LED_RED 12          // Pin LED Merah
#define RELAY_PIN 17        // Pin Relay Pompa
#define BUZZER_PIN 19       // Pin Buzzer

DHTesp dhtSensor;

const char* ssid = "Wokwi-GUEST";
const char* pass = "";    
const char* mqtt_server = "broker.hivemq.com"; // Alamat Broker MQTT

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

void setup() {
  // Inisialisasi Serial Monitor
  Serial.begin(115200);

  // Inisialisasi DHT sensor
  dhtSensor.setup(DHTPIN, DHTesp::DHT22);

  // Konfigurasi pin untuk LED, Relay, dan Buzzer
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Koneksi ke WiFi
  setup_wifi();
  
  // Set server MQTT
  mqtt.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
}

void reconnect() {
  // Loop sampai berhasil terkoneksi ke MQTT
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqtt.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!mqtt.connected()) {
    reconnect();
  }
  mqtt.loop();

  // Membaca data suhu dan kelembapan
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  float temperature = data.temperature;
  float humidity = data.humidity;

  // Mengontrol pompa berdasarkan kelembapan
  bool pumpStatus = false;
  if (humidity < 50) {
    digitalWrite(RELAY_PIN, HIGH);  // Menyalakan Pompa
    pumpStatus = true;
  } else {
    digitalWrite(RELAY_PIN, LOW);   // Mematikan Pompa
    pumpStatus = false;
  }

  // Mengontrol LED dan Buzzer berdasarkan suhu
  if (temperature > 35) {
    digitalWrite(LED_RED, HIGH);    // Menyalakan LED Merah
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(BUZZER_PIN, HIGH); // Menyalakan Buzzer
  } else if (temperature >= 30 && temperature <= 35) {
    digitalWrite(LED_YELLOW, HIGH); // Menyalakan LED Kuning
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(BUZZER_PIN, LOW);  // Mematikan Buzzer
  } else {
    digitalWrite(LED_GREEN, HIGH);  // Menyalakan LED Hijau
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(BUZZER_PIN, LOW);  // Mematikan Buzzer
  }

  // Mengirim data ke MQTT jika data valid
  // Mengirim data ke MQTT jika data valid
if (!isnan(temperature) && !isnan(humidity)) {
  char payload[50]; // Sesuaikan ukuran buffer sesuai kebutuhan
  snprintf(payload, sizeof(payload), "{\"temperature\":%.2f,\"humidity\":%.2f,\"pump\":\"%s\"}", temperature, humidity, pumpStatus ? "ON" : "OFF");

  mqtt.publish("uts_iot_khayla", payload);

  Serial.print("Data sent to MQTT: ");
  Serial.println(payload);
}

  
  delay(2000); // Delay 2 detik sebelum loop berikutnya
}
