/*
 * Proje: Otonom Okul Robotu - Ana Kontrol Merkezi (Main)
 * Dosya: src/main.cpp
 */

#include <Arduino.h>
#include "ESP32_Config.h"
#include "MotorController.h"
#include "SensorManager.h"
#include "NavigationManager.h"
#include "WiFiElevatorManager.h"
#include "AdminAuth.h"

// --- Sistem Modülleri ---
MotorController motorCtrl(MOTOR_A_IN1, MOTOR_A_IN2, MOTOR_B_IN1, MOTOR_B_IN2, MOTOR_ENA, MOTOR_ENB);
SensorManager sensorMgr(TRIG_PIN_FRONT, ECHO_PIN_FRONT);
NavigationManager navMgr;
WiFiElevatorManager elevatorMgr;
AdminAuth adminAuth;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println(F("\n=========================================="));
    Serial.println(F("    OKUL ROBOTU SISTEMI BASLATILIYOR     "));
    Serial.println(F("=========================================="));

    // 1. Modüllerin Başlatılması
    motorCtrl.begin();
    sensorMgr.begin();
    navMgr.begin();
    elevatorMgr.begin();

    // 2. Harita Düğümlerini ve Zemin Kodlarını Tanımla
    // Format: addNode(Kat, "KOD_ADI", ZeminCikisKoduMu)
    navMgr.addNode(0, "Z001", false);
    navMgr.addNode(0, "Z005", false);
    navMgr.addNode(0, "ZEMIN_ELEVATOR_EXIT", true); // Asansör Zemin Çıkış Zemin Kodu
    navMgr.addNode(1, "KAT1_ELEVATOR_EXIT", true); // Asansör 1. Kat Çıkış Zemin Kodu
    navMgr.addNode(1, "101", false);

    Serial.println(F("[SISTEM] Tum moduller basariyla kuruldu. Robot hazir."));
}

void loop() {
    // 1. Wi-Fi İstemcilerini ve Web Sunucuyu Dinle
    elevatorMgr.handleClient();

    // 2. IMU / Jiroskop Açısını Güncelle
    navMgr.updateIMU();

    // 3. Mesafe Ölçümü ve Engel Kontrolü
    float frontDistance = sensorMgr.getFrontDistance();
    uint8_t currentThreshold = navMgr.getObstacleThreshold();

    // 4. Sürüş Mantığı
    if (frontDistance > 0 && frontDistance < currentThreshold) {
        // Önümüzde tanımlı eşikten daha yakın engel var
        motorCtrl.stop();
        Serial.printf("[ENGEL DETEKTORU] Engel Algilandi! Mesafe: %.1f cm (Esik: %d cm)\n", 
                      frontDistance, currentThreshold);
    } else {
        // Yol açık, hedef yöne doğru ilerle
        // (Şimdilik test amaçlı yavaş ileri sürüş)
        motorCtrl.moveForward(150); 
    }

    // 5. Seri Porttan Manuel Kamera/Kod Komut Testi
    // Örn: Seri porttan "Z005" veya "ZEMIN_ELEVATOR_EXIT" göndererek test edebilirsin
    if (Serial.available() > 0) {
        String testCode = Serial.readStringUntil('\n');
        navMgr.processCameraTag(testCode);
    }

    delay(20); // Sistem kararlılığı için kısa gecikme
}
