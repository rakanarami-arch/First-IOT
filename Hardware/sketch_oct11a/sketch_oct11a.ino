#include <dummy.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h> // Tambahan library untuk melakukan HTTP POST
#include <WiFiClient.h>        // Tambahan library untuk komunikasi client
#include "DHT.h"

// --- 1. PENGATURAN WI-FI ---
const char* ssid = "NAMA WIFI MU";
const char* password = "PASSWORD WIFI MU";

// --- 2. PENGATURAN API DATABASE ---
// GANTI 192.168.x.x DENGAN IPV4 LAPTOPMU DARI CMD
const char* hostAPI = "http://IP_LAPTOP_ANDA/iot/simpan_data.php";

// Timer untuk mengirim data setiap 10 detik (10000 milidetik)
unsigned long waktuTerakhir = 0;
unsigned long jedaPengiriman = 10000; 

// --- 3. PENGATURAN SENSOR DHT11 ---
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
ESP8266WebServer server(80);

void handleSuhu() {
  float t = dht.readTemperature();
  if (isnan(t)) {
    server.send(500, "text/plain", "--");
  } else {
    server.send(200, "text/plain", String(t));
  }
}

void handleKelembaban() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    server.send(500, "text/plain", "--");
  } else {
    server.send(200, "text/plain", String(h));
  }
}

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html lang="id">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Monitor Suhu & Kelembaban</title>
    <style>
      body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #eef2f3; text-align: center; margin: 0; padding: 50px; color: #333; }
      h2 { margin-bottom: 40px; }
      .container { display: flex; justify-content: center; gap: 30px; flex-wrap: wrap; }
      .card { background-color: #ffffff; border-radius: 15px; box-shadow: 0 8px 16px rgba(0,0,0,0.1); padding: 30px; width: 220px; }
      .card h3 { margin-top: 0; color: #555; font-size: 1.2rem; }
      .value { font-size: 3rem; font-weight: bold; color: #007BFF; margin: 15px 0 0 0; }
      .humidity-color { color: #28a745; }
    </style>
  </head>
  <body>
    <h2>Dasbor Pemantauan Ruangan</h2>
    <div class="container">
      <div class="card">
        <h3>Suhu Ruangan</h3>
        <p class="value"><span id="suhuVal">--</span>&deg;C</p>
      </div>
      <div class="card">
        <h3>Kelembaban</h3>
        <p class="value humidity-color"><span id="lembabVal">--</span>%</p>
      </div>
    </div>
    <script>
      setInterval(function() {
        fetch('/suhu').then(response => response.text()).then(data => document.getElementById('suhuVal').innerText = data);
        fetch('/kelembaban').then(response => response.text()).then(data => document.getElementById('lembabVal').innerText = data);
      }, 2000);
    </script>
  </body>
  </html>
  )rawliteral";
  server.send(200, "text/html", html);
}

// --- FUNGSI BARU: Mengirim Data ke Database PHP ---
void kirimDataKeDatabase() {
  // Mengecek apakah sudah berlalu 10 detik sejak pengiriman terakhir
  if ((millis() - waktuTerakhir) > jedaPengiriman) {
    
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

      http.begin(client, hostAPI);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // Format standar pengiriman form

      float t = dht.readTemperature();
      float h = dht.readHumidity();

      // Pastikan sensor tidak error sebelum mengirim ke database
      if (!isnan(t) && !isnan(h)) {
        // Menyusun teks parameter POST (suhu=25.5&kelembaban=60)
        String dataPost = "suhu=" + String(t) + "&kelembaban=" + String(h);
        
        // Mengirimkan request
        int httpResponseCode = http.POST(dataPost);
        
        Serial.print("Respons dari Server PHP: ");
        Serial.println(httpResponseCode); // Sukses biasanya mengembalikan kode 200
      }
      
      http.end(); // Menutup koneksi
    }
    
    waktuTerakhir = millis(); // Mereset timer
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  Serial.println();
  Serial.print("Menghubungkan ke Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi Berhasil Terhubung!");
  Serial.print("Alamat IP NodeMCU: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/suhu", handleSuhu);
  server.on("/kelembaban", handleKelembaban);

  server.begin();
}

void loop() {
  server.handleClient(); // Menjaga dasbor web tetap hidup
  kirimDataKeDatabase(); // Mengecek dan mengirim data setiap 10 detik
}