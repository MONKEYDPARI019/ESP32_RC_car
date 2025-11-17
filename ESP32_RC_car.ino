#include <WiFi.h>
#include <WebServer.h>

// WiFi credentials for Access Point mode
const char* ssid = "ESP32-RC";
const char* password = "12345678";

WebServer server(80);

// Motor pin configuration
#define DRIVE_EN 27
#define DRIVE_IN1 26
#define DRIVE_IN2 25
#define STEER_EN 14
#define STEER_IN1 13   // swapped to fix left/right issue
#define STEER_IN2 12   // swapped to fix left/right issue

// ---------- Motor Control Functions ----------
void stopAll() {
  digitalWrite(DRIVE_IN1, LOW);
  digitalWrite(DRIVE_IN2, LOW);
  digitalWrite(STEER_IN1, LOW);
  digitalWrite(STEER_IN2, LOW);
  analogWrite(DRIVE_EN, 0);
  analogWrite(STEER_EN, 0);
  Serial.println("STOP");
}

void forward() {
  digitalWrite(DRIVE_IN1, HIGH);
  digitalWrite(DRIVE_IN2, LOW);
  analogWrite(DRIVE_EN, 255);
  Serial.println("FORWARD");
}

void backward() {
  digitalWrite(DRIVE_IN1, LOW);
  digitalWrite(DRIVE_IN2, HIGH);
  analogWrite(DRIVE_EN, 255);
  Serial.println("BACKWARD");
}

void left() {
  digitalWrite(STEER_IN1, HIGH);
  digitalWrite(STEER_IN2, LOW);
  analogWrite(STEER_EN, 255);
  Serial.println("LEFT");
}

void right() {
  digitalWrite(STEER_IN1, LOW);
  digitalWrite(STEER_IN2, HIGH);
  analogWrite(STEER_EN, 255);
  Serial.println("RIGHT");
}

// ---------- HTML Joystick Page ----------
String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32 RC Car</title>
<style>
body{background:#111;color:white;text-align:center;font-family:sans-serif;}
button{width:100px;height:100px;margin:10px;font-size:22px;border:none;
border-radius:50%;background:#444;color:white;}
button:active{background:#0f0;color:black;}
.grid{display:grid;grid-template-columns:repeat(3,1fr);justify-items:center;align-items:center;}
</style>
<script>
function go(cmd){fetch('/'+cmd);}
function stopAll(){fetch('/stop');}
</script>
</head>
<body>
<h2>🚗 ESP32 RC Car Control</h2>
<div class="grid">
  <div></div><button ontouchstart="go('forward')" ontouchend="stopAll()">↑</button><div></div>
  <button ontouchstart="go('left')" ontouchend="stopAll()">←</button>
  <button ontouchstart="go('backward')" ontouchend="stopAll()">↓</button>
  <button ontouchstart="go('right')" ontouchend="stopAll()">→</button>
</div>
</body>
</html>
)rawliteral";

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);

  pinMode(DRIVE_IN1, OUTPUT);
  pinMode(DRIVE_IN2, OUTPUT);
  pinMode(STEER_IN1, OUTPUT);
  pinMode(STEER_IN2, OUTPUT);
  pinMode(DRIVE_EN, OUTPUT);
  pinMode(STEER_EN, OUTPUT);

  stopAll();

  // Start Wi-Fi Access Point
  WiFi.softAP(ssid, password);
  Serial.println("\n=== Access Point Started ===");
  Serial.print("SSID: "); Serial.println(ssid);
  Serial.print("Password: "); Serial.println(password);
  Serial.print("IP address: "); Serial.println(WiFi.softAPIP());

  // Set up web routes
  server.on("/", [](){ server.send(200, "text/html", htmlPage); });
  server.on("/forward", [](){ forward(); server.send(200, "text/plain", "FWD"); });
  server.on("/backward", [](){ backward(); server.send(200, "text/plain", "BWD"); });
  server.on("/left", [](){ left(); server.send(200, "text/plain", "LEFT"); });
  server.on("/right", [](){ right(); server.send(200, "text/plain", "RIGHT"); });
  server.on("/stop", [](){ stopAll(); server.send(200, "text/plain", "STOP"); });

  server.begin();
  Serial.println("Server Ready! ✅");
}

// ---------- Loop ----------
void loop() {
  server.handleClient();
}
