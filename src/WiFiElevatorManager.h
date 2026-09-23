/*
 * Proje: Okul Robotu - Wi-Fi AP ve Web Server Yöneticisi (PRO SÜRÜM)
 * Dosya: src/WiFiElevatorManager.h
 */

#ifndef WIFI_ELEVATOR_MANAGER_H
#define WIFI_ELEVATOR_MANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include "ESP32_Config.h"

class WiFiElevatorManager {
private:
    WebServer server;
    int targetFloor;

    void setupRoutes();
    void handleRoot();
    void handleMove();
    void handleSetTarget();

public:
    WiFiElevatorManager();
    void begin();
    void handleClient();
    int getTargetFloor();
};

#endif // WIFI_ELEVATOR_MANAGER_H
