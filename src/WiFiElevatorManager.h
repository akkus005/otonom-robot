/*
 * Proje: Okul Robotu - Wi-Fi & Fingerbot Asansör Yöneticisi
 * Dosya: src/WiFiElevatorManager.h
 */

#ifndef WIFI_ELEVATOR_MANAGER_H
#define WIFI_ELEVATOR_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include "ESP32_Config.h"

class WiFiElevatorManager {
private:
    WebServer server;
    uint8_t currentTargetFloor;
    bool elevatorTriggerPending;

    void handleRoot();
    void handleSetTarget();

public:
    WiFiElevatorManager();
    
    void begin();
    void handleClient();
    
    // Fingerbot/SwitchBot Cihazına Tuşa Basma İsteği Gönderir
    bool triggerFingerbot(const char* fingerbotIP);
    
    uint8_t getTargetFloor();
    void resetElevatorTrigger();
};

#endif // WIFI_ELEVATOR_MANAGER_H
