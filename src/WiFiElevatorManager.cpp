/*
 * Proje: Okul Robotu - Wi-Fi & Fingerbot Asansör Yöneticisi
 * Dosya: src/WiFiElevatorManager.cpp
 */

#include "WiFiElevatorManager.h"

WiFiElevatorManager::WiFiElevatorManager() : server(80) {
    currentTargetFloor = 0;
    elevatorTriggerPending = false;
}

void WiFiElevatorManager::begin() {
    // Robotun Kendi Wi-Fi Ağını Başlat (AP Modu)
    WiFi.softAP(SOFT_AP_SSID, SOFT_AP_PASS);
    
    Serial.print(F("[WIFI] AP Agi Baslatildi: "));
    Serial.println(SOFT_AP_SSID);
    Serial.print(F("[WIFI] Robot IP: "));
    Serial.println(WiFi.softAPIP());

    // Web Arayüz Rotaları
    server.on("/", std::bind(&WiFiElevatorManager::handleRoot, this));
    server.on("/setTarget", std::bind(&WiFiElevatorManager::handleSetTarget, this));
    server.begin();
}

void WiFiElevatorManager::handleClient() {
    server.handleClient();
}

void WiFiElevatorManager::handleRoot() {
    String html = "<h1>Okul Robotu Kontrol Paneli</h1>";
    html += "<p>Hedef Kat: " + String(currentTargetFloor) + "</p>";
    html += "<a href='/setTarget?floor=0'><button>Zemin Kat</button></a> ";
    html += "<a href='/setTarget?floor=1'><button>1. Kat</button></a>";
    server.send(200, "text/html", html);
}

void WiFiElevatorManager::handleSetTarget() {
    if (server.hasArg("floor")) {
        currentTargetFloor = server.arg("floor").toInt();
        elevatorTriggerPending = true;
        server.send(200, "text/plain", "Hedef kat güncellendi: " + String(currentTargetFloor));
    } else {
        server.send(400, "text/plain", "Eksik parametre");
    }
}

// Wi-Fi üzerinden Fingerbot akıllı tuş basıcısını tetikler
bool WiFiElevatorManager::triggerFingerbot(const char* fingerbotIP) {
    if (WiFi.status() != WL_CONNECTED && WiFi.softAPgetStationNum() == 0) {
        Serial.println(F("[FINGERBOT] Baglantı yok, tetiklenemedi."));
        return false;
    }

    HTTPClient http;
    String url = "http://" + String(fingerbotIP) + "/press";
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
        Serial.printf("[FINGERBOT] Tusa Basildi! Yanit: %d\n", httpCode);
        http.end();
        return true;
    } else {
        Serial.printf("[FINGERBOT] Hata: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        return false;
    }
}

uint8_t WiFiElevatorManager::getTargetFloor() {
    return currentTargetFloor;
}

void WiFiElevatorManager::resetElevatorTrigger() {
    elevatorTriggerPending = false;
}
