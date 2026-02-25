#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <HTTPClient.h>

// const char* WIFI_SSID     = "vivo Y75";
// const char* WIFI_PASSWORD = "Livi2003.";
const char*  WIFI_SSID  = "MCA Department";
const char*   WIFI_PASSWORD = "mcadept@ac.in";

#define USE_STA_MODE

const char* BACKEND_URL = "http://192.168.0.115:5000/api/data";

constexpr uint8_t PIN_SOIL   = 34;  
constexpr uint8_t PIN_LDR    = 35;  
constexpr uint8_t PIN_RAIN   = 33; 
constexpr uint8_t PIN_WATER  = 32;  
constexpr uint8_t PIN_DHT    = 27;  
constexpr uint8_t PIN_FIRE   = 25;  
constexpr uint8_t PIN_ALERT  = 26; 


constexpr int SOIL_DRY_TH   = 2000;
constexpr int RAIN_WET_TH   = 2000;
constexpr int WATER_LOW_TH  = 1800;

DHT dht(PIN_DHT, DHT11);
WebServer server(80);

struct SensorPacket {
  int   soil;
  int   light;
  int   rain;
  int   water;
  float temp;
  float hum;
  bool  isDry;
  bool  isRaining;
  bool  isWaterLow;
  bool  fire;
} data;

unsigned long lastRead = 0;

void readSensors() {
  data.soil   = analogRead(PIN_SOIL);
  data.light  = analogRead(PIN_LDR);
  data.rain   = analogRead(PIN_RAIN);
  data.water  = analogRead(PIN_WATER);
  data.temp   = dht.readTemperature();
  data.hum    = dht.readHumidity();

  if (isnan(data.temp)) data.temp = 0.0;
  if (isnan(data.hum))  data.hum  = 0.0;

  data.fire   = (digitalRead(PIN_FIRE) == LOW);
  data.isDry      = data.soil  < SOIL_DRY_TH;
  data.isRaining  = data.rain  < RAIN_WET_TH;
  data.isWaterLow = data.water < WATER_LOW_TH;

  bool needAlert = data.fire || (data.isDry && !data.isRaining);
  digitalWrite(PIN_ALERT, needAlert ? HIGH : LOW);

  Serial.printf("Soil:%d Light:%d Rain:%d Water:%d Fire:%d  T:%.1fC  H:%.1f%%\n",
                data.soil, data.light, data.rain, data.water, data.fire,
                data.temp, data.hum);
}

void postToBackend() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(BACKEND_URL);
    http.addHeader("Content-Type", "application/json");

    String payload = "{";
    payload += "\"soil\":" + String(data.soil) + ",";
    payload += "\"light\":" + String(data.light) + ",";
    payload += "\"rain\":" + String(data.rain) + ",";
    payload += "\"water\":" + String(data.water) + ",";
    payload += "\"temp\":" + String(data.temp, 1) + ",";
    payload += "\"hum\":" + String(data.hum, 1) + ",";
    payload += "\"dry\":" + String(data.isDry ? "true" : "false") + ",";
    payload += "\"raining\":" + String(data.isRaining ? "true" : "false") + ",";
    payload += "\"waterLow\":" + String(data.isWaterLow ? "true" : "false") + ",";
    payload += "\"fire\":" + String(data.fire ? "true" : "false");
    payload += "}";

    Serial.print("Sending data: ");
    Serial.println(payload);

    int res = http.POST(payload);
    Serial.print("HTTP Response Code: ");
    Serial.println(res);

    if (res > 0) {
      String response = http.getString();
      Serial.print("Server Response: ");
      Serial.println(response);
    } else {
      Serial.print("POST failed, error: ");
      Serial.println(http.errorToString(res).c_str());
    }

    http.end();
  } else {
    Serial.println(" No Wi-Fi. Skipping POST.");
  }
}


void handleData() {
  String json = "{";
  json += "\"soil\":"     + String(data.soil)             + ",";
  json += "\"light\":"    + String(data.light)            + ",";
  json += "\"rain\":"     + String(data.rain)             + ",";
  json += "\"water\":"    + String(data.water)            + ",";
  json += "\"temp\":"     + String(data.temp,1)           + ",";
  json += "\"hum\":"      + String(data.hum,1)            + ",";
  json += "\"dry\":"      + String(data.isDry ? "true":"false")      + ",";
  json += "\"raining\":"  + String(data.isRaining ? "true":"false")  + ",";
  json += "\"waterLow\":" + String(data.isWaterLow ? "true":"false") + ",";
  json += "\"fire\":"     + String(data.fire ? "true":"false");
  json += "}";
  server.send(200, "application/json", json);
}

const char DASH_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8"><title>Smart Farm Dashboard</title>
<meta name="viewport" content="width=device-width,initial-scale=1">
<style>
:root {
  --bg-day: linear-gradient(135deg, #74ebd5, #acb6e5);
  --bg-night: linear-gradient(135deg, #2c3e50, #4ca1af);
  --text-day: #333;
  --text-night: #fff;
}
* { box-sizing: border-box; margin: 0; padding: 0; font-family: sans-serif }
body {
  min-height: 100vh;
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: 20px;
  background: var(--bg-day);
  color: var(--text-day);
  transition: background .5s, color .5s
}
h1 { margin-bottom: 14px; text-shadow: 0 2px 4px #0004; color: #fff }
.grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(160px, 1fr));
  gap: 14px;
  width: 100%;
  max-width: 600px
}
.card {
  background: #ffffffdd;
  border-radius: 14px;
  padding: 18px;
  text-align: center;
  backdrop-filter: blur(4px);
  box-shadow: 0 4px 12px #0003
}
.value {
  font-size: 2.2rem;
  font-weight: bold;
  margin: 6px 0
}
.badge {
  padding: 3px 8px;
  border-radius: 20px;
  font-size: .8rem;
  color: #fff
}
.danger { background: #e74c3c }
.ok { background: #27ae60 }
.icon { font-size: 1.8rem }
footer { margin-top: auto; opacity: .8; font-size: .8rem }
</style>
</head>
<body>
<h1>🌾 Smart Farming IoT</h1>
<div class="grid">
<div class="card" id="soil"><div class="icon">🌱</div><div>Soil</div><div class="value">--</div><span class="badge" id="soilB"></span></div>
<div class="card" id="light"><div class="icon">☀️</div><div>Light</div><div class="value">--</div></div>
<div class="card" id="rain"><div class="icon">🌧️</div><div>Rain</div><div class="value">--</div><span class="badge" id="rainB"></span></div>
<div class="card" id="water"><div class="icon">💧</div><div>Water Tank</div><div class="value">--</div><span class="badge" id="waterB"></span></div>
<div class="card" id="temp"><div class="icon">🌡️</div><div>Temp °C</div><div class="value">--</div></div>
<div class="card" id="hum"><div class="icon">💦</div><div>Humidity %</div><div class="value">--</div></div>
<div class="card" id="fire"><div class="icon" style="font-size:2.2rem">🔥</div><div>Fire</div><div class="value">--</div><span class="badge" id="fireB"></span></div>
</div>
<footer>Auto-refresh every 3s</footer>
<script>
const $ = q => document.querySelector(q);
function updateTheme() {
  const h = new Date().getHours();
  const day = h >= 6 && h < 18;
  const root = document.documentElement;
  const isNight = !day;

  document.body.style.background = day
    ? getComputedStyle(root).getPropertyValue('--bg-day')
    : getComputedStyle(root).getPropertyValue('--bg-night');

  document.body.style.color = day
    ? getComputedStyle(root).getPropertyValue('--text-day')
    : getComputedStyle(root).getPropertyValue('--text-night');

  document.querySelectorAll('.card').forEach(card => {
    card.style.color = isNight ? '#000' : '#000';  // Optionally darker in day, lighter in night
    card.style.background = isNight ? '#ffffff99' : '#ffffffdd';
  });
}

async function tick() {
  try {
    const r = await fetch('/data'); const j = await r.json();
    $('#soil .value').textContent = j.soil;
    $('#light .value').textContent = j.light;
    $('#rain .value').textContent = j.rain;
    $('#water .value').textContent = j.water;
    $('#temp .value').textContent = j.temp;
    $('#hum .value').textContent = j.hum;
    $('#fire .value').textContent = j.fire ? '🔥' : '✅';
    $('#soilB').textContent = j.dry ? 'Dry' : 'OK';
    $('#soilB').className = 'badge ' + (j.dry ? 'danger' : 'ok');
    $('#rainB').textContent = j.raining ? 'Rain' : 'Clear';
    $('#rainB').className = 'badge ' + (j.raining ? 'ok' : 'danger');
    $('#waterB').textContent = j.waterLow ? 'Low' : 'OK';
    $('#waterB').className = 'badge ' + (j.waterLow ? 'danger' : 'ok');
    $('#fireB').textContent = j.fire ? 'Fire!' : 'Safe';
    $('#fireB').className = 'badge ' + (j.fire ? 'danger' : 'ok');
  } catch (e) { console.log(e) }
}
setInterval(tick, 3000); 
tick();
updateTheme(); setInterval(updateTheme, 60000);
</script>
</body>
</html>
)rawliteral";

void handleRoot()      { server.send_P(200, "text/html", DASH_HTML); }
void handleNotFound()  { server.send(404,"text/plain","Not found"); }

void setup() {
  Serial.begin(115200);
  pinMode(PIN_ALERT, OUTPUT);
  pinMode(PIN_FIRE, INPUT);
  dht.begin();

#ifdef USE_STA_MODE
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("\nConnecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWi-Fi connected.");
#else
  WiFi.softAP("ESP32_Farm", "smartfarm");
  Serial.println("\nStarted AP  SSID: ESP32_Farm  Pass: smartfarm");
#endif

  Serial.print("IP address: "); Serial.println(WiFi.localIP());

  server.on("/",     handleRoot);
  server.on("/data", handleData);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  if (millis() - lastRead >= 2000) {
    readSensors();
    postToBackend();
    lastRead = millis();
  }
  server.handleClient();
}

