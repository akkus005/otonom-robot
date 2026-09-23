/*
 * Proje: Okul Robotu - Wi-Fi AP ve Web Server Yöneticisi (PRO SÜRÜM)
 * Dosya: src/WiFiElevatorManager.cpp
 */

#include "WiFiElevatorManager.h"
#include "MotorController.h"

extern MotorController motorCtrl;

// Gelişmiş Mobil Arayüz (Bas-Çek Joystick Mantığı + Modern UI)
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="tr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Okul Robotu Kontrol Merkezi</title>
    <style>
        :root { --bg: #121212; --panel: #1e1e1e; --primary: #00e676; --btn: #29b6f6; --stop: #ff5252; --text: #ffffff; }
        * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; user-select: none; -webkit-tap-highlight-color: transparent; }
        body { background-color: var(--bg); color: var(--text); display: flex; flex-direction: column; align-items: center; padding: 20px; }
        h2 { margin-bottom: 20px; color: var(--primary); text-transform: uppercase; letter-spacing: 1px; font-size: 22px; }
        .card { background: var(--panel); width: 100%; max-width: 350px; padding: 15px; border-radius: 12px; box-shadow: 0 4px 15px rgba(0,0,0,0.5); margin-bottom: 25px; text-align: center; border: 1px solid #333; }
        .status { font-size: 15px; color: #aaa; margin: 8px 0; }
        .highlight { color: var(--primary); font-weight: bold; }
        .dpad { display: grid; grid-template-columns: repeat(3, 80px); grid-gap: 15px; justify-content: center; margin-bottom: 25px; }
        .btn { background: var(--btn); border: none; border-radius: 15px; font-size: 24px; color: #000; height: 80px; box-shadow: 0 6px 0 #0288d1; transition: transform 0.1s, box-shadow 0.1s; cursor: pointer; display: flex; align-items: center; justify-content: center; }
        .btn:active { transform: translateY(6px); box-shadow: 0 0 0 #0288d1; }
        .btn-stop { background: var(--stop); box-shadow: 0 6px 0 #d50000; color: white; }
        .btn-stop:active { box-shadow: 0 0 0 #d50000; }
        .empty { visibility: hidden; }
        .floor-container { display: flex; justify-content: space-between; margin-top: 15px; }
        .floor-btn { background: #333; color: white; border: 1px solid #555; padding: 12px; border-radius: 8px; width: 30%; font-size: 14px; font-weight: bold; transition: 0.2s; }
        .floor-btn:active { background: #ffab40; color: black; border-color: #ffab40; }
    </style>
</head>
<body>
    <h2>🤖 Okul Robotu</h2>
    
    <div class="card">
        <p class="status">Durum: <span id="conn" class="highlight">OkulRobotu_AP Bağlı</span></p>
        <p class="status">Hedef Kat: <span id="targetFloor" class="highlight">0 (Zemin)</span></p>
    </div>
    
    <!-- Joystick Alanı (Basılı tutunca gider, çekince durur) -->
    <div class="dpad">
        <div class="empty"></div>
        <button class="btn" onpointerdown="sendCmd('/move?dir=forward')" onpointerup="sendCmd('/move?dir=stop')" onpointerleave="sendCmd('/move?dir=stop')">▲</button>
        <div class="empty"></div>
        
        <button class="btn" onpointerdown="sendCmd('/move?dir=left')" onpointerup="sendCmd('/move?dir=stop')" onpointerleave="sendCmd('/move?dir=stop')">◄</button>
        <button class="btn btn-stop" onclick="sendCmd('/move?dir=stop')">■</button>
        <button class="btn" onpointerdown="sendCmd('/move?dir=right')" onpointerup="sendCmd('/move?dir=stop')" onpointerleave="sendCmd('/move?dir=stop')">►</button>
        
        <div class="empty"></div>
        <button class="btn" onpointerdown="sendCmd('/move?dir=backward')" onpointerup="sendCmd('/move?dir=stop')" onpointerleave="sendCmd('/move?dir=stop')">▼</button>
        <div class="empty"></div>
    </div>

    <!-- Asansör Kontrol -->
    <div class="card">
        <h3 style="color: #ffab40; font-size: 16px;">🏢 Asansör Kat Seçimi</h3>
        <div class="floor-container">
            <button class="floor-btn" onclick="setFloor(0)">Zemin</button>
            <button class="floor-btn" onclick="setFloor(1)">1. Kat</button>
            <button class="floor-btn" onclick="setFloor(2)">2. Kat</button>
        </div>
    </div>

    <script>
        // Fetch API ile arka planda sessizce komut yolla
        function sendCmd(endpoint) {
            fetch(endpoint).catch(err => {
                document.getElementById('conn').innerText = "Bağlantı Koptu!";
                document.getElementById('conn').style.color = "#ff5252";
            });
        }

        // Kat numarasını ekranda güncelle ve robota bildir
        function setFloor(floorNum) {
            document.getElementById('targetFloor').innerText = floorNum + ". Kat";
            sendCmd('/setTarget?floor=' + floorNum);
        }
    </script>
</body>
</html>
)rawliteral";

WiFiElevatorManager::WiFiElevatorManager() : server(80), targetFloor(0) {}

void WiFiElevatorManager::begin() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(SOFT_AP_SSID, SOFT_AP_PASS);
    
    Serial.println(F("[WIFI] AP Agi Baslatildi: OkulRobotu_AP"));
    Serial.print(F("[WIFI] Robot IP Adresi: "));
    Serial.println(WiFi.softAPIP());

    setupRoutes();
    server.begin();
    Serial.println(F("[WEB] Sunucu 80 Portunda Dinlemede. Mobil Arayuz Hazir."));
}

void WiFiElevatorManager::setupRoutes() {
    // Arayüz sayfası
    server.on("/", std::bind(&WiFiElevatorManager::handleRoot, this));
    // Sürüş rotaları
    server.on("/move", std::bind(&WiFiElevatorManager::handleMove, this));
    // Asansör rotaları
    server.on("/setTarget", std::bind(&WiFiElevatorManager::handleSetTarget, this));
}

void WiFiElevatorManager::handleRoot() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send_P(200, "text/html", INDEX_HTML);
}

void WiFiElevatorManager::handleMove() {
    if (server.hasArg("dir")) {
        String dir = server.arg("dir");
        if (dir == "forward") motorCtrl.moveForward(200);
        else if (dir == "backward") motorCtrl.moveBackward(200);
        else if (dir == "left") motorCtrl.turnLeft(180);
        else if (dir == "right") motorCtrl.turnRight(180);
        else if (dir == "stop") motorCtrl.stop();
    }
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "OK");
}

void WiFiElevatorManager::handleSetTarget() {
    if (server.hasArg("floor")) {
        targetFloor = server.arg("floor").toInt();
        Serial.printf("[WEB] Yeni Hedef Kat Secildi: %d\n", targetFloor);
    }
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "OK");
}

void WiFiElevatorManager::handleClient() {
    server.handleClient();
}

int WiFiElevatorManager::getTargetFloor() {
    return targetFloor;
}
