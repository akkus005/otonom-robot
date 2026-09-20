/*
 * Proje: Okul Robotu - Konumlandırma ve Asansör Modu
 * Dosya: src/NavigationManager.cpp
 */

#include "NavigationManager.h"

NavigationManager::NavigationManager() {
    currentYaw = 0.0;
    lastIMURead = 0;
    nodeCount = 0;
    currentNodeIndex = -1;
    elevatorModeActive = false;
    obstacleThreshold = NORMAL_OBSTACLE_DIST; // 30 cm
}

void NavigationManager::begin() {
    Wire.begin(21, 22);
    initMPU6050();
    Serial.println(F("[NAV] MPU6050 ve Navigasyon Modulu Hazir."));
}

void NavigationManager::initMPU6050() {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x6B);
    Wire.write(0x00);
    Wire.endTransmission(true);
}

void NavigationManager::updateIMU() {
    unsigned long now = millis();
    float dt = (now - lastIMURead) / 1000.0;
    if (dt < 0.02) return;
    lastIMURead = now;

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, (uint8_t)6, (uint8_t)true);

    if (Wire.available() >= 6) {
        Wire.read(); Wire.read();
        Wire.read(); Wire.read();
        int16_t gyroZ = Wire.read() << 8 | Wire.read();

        float gyroZrate = (float)gyroZ / 131.0;
        if (abs(gyroZrate) > 0.5) {
            currentYaw += gyroZrate * dt;
        }

        if (currentYaw >= 360.0) currentYaw -= 360.0;
        if (currentYaw < 0.0) currentYaw += 360.0;
    }
}

// 360 Kamera Duvar/Kapı/Zemin Kodu Algıladığında
void NavigationManager::processCameraTag(String tagCode) {
    tagCode.trim();
    tagCode.toUpperCase();

    for (uint8_t i = 0; i < nodeCount; i++) {
        if (tagCode.equals(nodeList[i].code)) {
            currentNodeIndex = i;
            
            // Asansör çıkışındaki zemin kodu algılandıysa
            if (nodeList[i].isFloorTag) {
                setElevatorMode(false); // Asansör modundan çık
                Serial.print(F("[ZEMIN KODU ALGINLANDI] Kat Cikisi Dogrulandi: "));
                Serial.println(nodeList[i].code);
            } else {
                Serial.print(F("[KONUM DOGRULANDI] Kod: "));
                Serial.println(nodeList[i].code);
            }
            return;
        }
    }
}

// Asansör Modu: Engel mesafesini dar alan için 8 cm'ye düşürür
void NavigationManager::setElevatorMode(bool active) {
    elevatorModeActive = active;
    if (active) {
        obstacleThreshold = ELEVATOR_OBSTACLE_DIST; // 8 cm
        Serial.println(F("[MOD] Asansor Modu AKTIF: Mesafe esigi 8cm'ye indirildi."));
    } else {
        obstacleThreshold = NORMAL_OBSTACLE_DIST;   // 30 cm
        Serial.println(F("[MOD] Normal Mod: Mesafe esigi 30cm'ye cikarildi."));
    }
}

bool NavigationManager::isElevatorMode() {
    return elevatorModeActive;
}

uint8_t NavigationManager::getObstacleThreshold() {
    return obstacleThreshold;
}

bool NavigationManager::addNode(uint8_t floor, const char* code, bool isFloorTag) {
    if (nodeCount >= MAX_NODES) return false;

    nodeList[nodeCount].id = nodeCount;
    nodeList[nodeCount].floor = floor;
    strncpy(nodeList[nodeCount].code, code, sizeof(nodeList[nodeCount].code) - 1);
    nodeList[nodeCount].isFloorTag = isFloorTag;

    nodeCount++;
    return true;
}

float NavigationManager::getYaw() {
    return currentYaw;
}

int8_t NavigationManager::getCurrentNodeIndex() {
    return currentNodeIndex;
}
