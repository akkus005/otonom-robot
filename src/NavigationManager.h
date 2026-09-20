/*
 * Proje: Okul Robotu - Konumlandırma ve Asansör Modu
 * Dosya: src/NavigationManager.h
 */

#ifndef NAVIGATION_MANAGER_H
#define NAVIGATION_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include "ESP32_Config.h"

struct DoorNode {
    uint8_t id;
    uint8_t floor;
    char code[16];        // Tabela veya Zemin Kodu ("Z005", "ZEMIN_ELEVATOR_EXIT")
    bool isFloorTag;     // Zemin çıkış kodu mu?
};

class NavigationManager {
private:
    float currentYaw;
    unsigned long lastIMURead;
    
    static const uint8_t MAX_NODES = 40;
    DoorNode nodeList[MAX_NODES];
    uint8_t nodeCount;
    int8_t currentNodeIndex;

    bool elevatorModeActive; // Asansör içi dar alan modu
    uint8_t obstacleThreshold;

    const uint8_t MPU_ADDR = 0x68;
    void initMPU6050();

public:
    NavigationManager();

    void begin();
    void updateIMU();
    
    // 360 Kamera Kod Okuduğunda Çalışır
    void processCameraTag(String tagCode);

    // Asansör İçi Dar Alan Modunu Aç/Kapat
    void setElevatorMode(bool active);
    bool isElevatorMode();
    uint8_t getObstacleThreshold();

    bool addNode(uint8_t floor, const char* code, bool isFloorTag = false);
    float getYaw();
    int8_t getCurrentNodeIndex();
};

#endif // NAVIGATION_MANAGER_H
