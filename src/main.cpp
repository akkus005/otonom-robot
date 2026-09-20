/*
 * Proje: Okul Robotu - Master Core System
 * Dosya: src/main.cpp
 * Geliştirici: Akkuş
 * Mimari: State-Machine & Modular OOP
 * Açıklama: MotorController, SensorManager ve AdminAuth modüllerini
 * birleştiren tam donanımlı, non-blocking ana kontrol yazılımı.
 */

#include <Arduino.h>
#include "MotorController.h"
#include "SensorManager.h"
#include "AdminAuth.h"

// --- 1. DONANIM PIN TANIMLAMALARI ---
// Sol Motor Pinleri (L298N / TB6612 vb.)
const uint8_t LEFT_IN1 = 2;
const uint8_t LEFT_IN2 = 3;
const uint8_t LEFT_ENA = 5; // PWM Destekli Pin

// Sağ Motor Pinleri
const uint8_t RIGHT_IN1 = 4;
const uint8_t RIGHT_IN2 = 7;
const uint8_t RIGHT_ENB = 6; // PWM Destekli Pin

// Ultrasonik Sensör Pinleri
const uint8_t TRIG_PIN = 8;
const uint8_t ECHO_PIN = 9;

// --- 2. NESNE MİMARİSİ (MODÜLLER) ---
MotorController motors(LEFT_IN1, LEFT_IN2, LEFT_ENA, RIGHT_IN1, RIGHT_IN2, RIGHT_ENB);
SensorManager sensors(TRIG_PIN, ECHO_PIN, 15.0); // 15 cm engel sınırı
AdminAuth admin("akkus_admin123");                // Varsayılan Admin Şifresi

// --- 3. SİSTEM DURUMLARI ---
enum class SystemState {
    INIT,
    IDLE,
    ADMIN_MODE,
    AUTO_DRIVE,
    SAFE_STOP,
    SYSTEM_LOCKED
};

SystemState currentState = SystemState::INIT;

// --- 4. ZAMANLAYICILAR (NON-BLOCKING) ---
unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_INTERVAL = 1000; // 1 Saniye

// --- 5. FONKSİYON PROTOTİPLERİ ---
void processSerialCommands();
void handleAutoDrive();
void handleAdminMode();
void triggerEmergencyStop(const char* reason);

// --- 6. KURULUM (SETUP) ---
void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Serial.println(F("======================================="));
    Serial.println(F("[SYS] Okul Robotu OS v2.0 Baslatiliyor..."));
    Serial.println(F("======================================="));

    // Modülleri Başlat
    motors.begin();
    sensors.begin();
    admin.begin();

    Serial.println(F("[OK] Tum donanimlar ve moduller basariyla yuklendi."));
    Serial.println(F("[SYS] Durum: IDLE. Komut bekleniyor."));
    
    currentState = SystemState::IDLE;
}

// --- 7. ANA DÖNGÜ (LOOP) ---
void loop() {
    unsigned long currentMillis = millis();

    // Sensör verilerini döngüyü aksatmadan güncelle
    sensors.update();

    // Dışarıdan gelen komutları kontrol et
    processSerialCommands();

    // Sistem Güvenlik Kilit Durumu Kontrolü
    if (admin.isLocked()) {
        if (currentState != SystemState::SYSTEM_LOCKED) {
            triggerEmergencyStop("Cok fazla hatali admin girisi! Sistem kilitlendi.");
            currentState = SystemState::SYSTEM_LOCKED;
        }
        return;
    }

    // Durum Makinesi İşletimi
    switch (currentState) {
        case SystemState::INIT:
            break;

        case SystemState::IDLE:
            motors.stopAll();
            break;

        case SystemState::ADMIN_MODE:
            handleAdminMode();
            break;

        case SystemState::AUTO_DRIVE:
            handleAutoDrive();
            break;

        case SystemState::SAFE_STOP:
            motors.stopAll();
            break;

        case SystemState::SYSTEM_LOCKED:
            motors.stopAll();
            break;
    }

    // Sistem Yaşam (Heartbeat) Sinyali
    if (currentMillis - lastHeartbeat >= HEARTBEAT_INTERVAL) {
        lastHeartbeat = currentMillis;
        // Serial.println(F("[SYS] Alive..."));
    }
}

// --- 8. MANTIKSAL FONKSİYONLAR ---

void handleAutoDrive() {
    // Sensör engeli algıladı mı?
    if (sensors.isObstacleDetected()) {
        motors.stopAll();
        Serial.print(F("[ENGEL] Yakin mesafe algilandi: "));
        Serial.print(sensors.getFilteredDistanceCm());
        Serial.println(F(" cm. Otomatik durus yapildi."));
        currentState = SystemState::SAFE_STOP;
        return;
    }

    // Engel yoksa güvenli hızda ileri git
    motors.moveForward(180);
}

void handleAdminMode() {
    // Admin modundayken yapılacak özel iş mantıkları (Sürekli duruşta bekleme vb.)
}

void processSerialCommands() {
    if (Serial.available() == 0) return;

    String rawInput = Serial.readStringUntil('\n');
    rawInput.trim();

    if (rawInput.length() == 0) return;

    // Komut Parçalama: "LOGIN akkus_admin123" veya "MOVE FORWARD 200"
    int spaceIndex = rawInput.indexOf(' ');
    String command = (spaceIndex == -1) ? rawInput : rawInput.substring(0, spaceIndex);
    String param = (spaceIndex == -1) ? "" : rawInput.substring(spaceIndex + 1);

    command.toUpperCase();

    // --- GENEL KOMUTLAR ---
    if (command == "STOP") {
        triggerEmergencyStop("Kullanici tarafindan STOP komutu verildi.");
        return;
    }

    // --- ADMIN YETKİLENDİRME KOMUTLARI ---
    if (command == "LOGIN") {
        if (admin.login(param)) {
            currentState = SystemState::ADMIN_MODE;
            Serial.println(F("[AUTH] Admin girisi basarili. Admin moduna gecildi."));
        } else {
            Serial.println(F("[HATA] Hatali sifre!"));
            if (admin.isLocked()) {
                Serial.println(F("[KRITIK] Maksimum deneme asildi. Sistem kilitlendi!"));
            }
        }
        return;
    }

    if (command == "LOGOUT") {
        admin.logout();
        currentState = SystemState::IDLE;
        motors.stopAll();
        Serial.println(F("[AUTH] Admin oturumu kapatildi. IDLE moduna gecildi."));
        return;
    }

    // --- ADMIN YETKİSİ GEREKTİREN KOMUTLAR ---
    if (!admin.isLoggedIn()) {
        Serial.println(F("[RED] Bu komut icin Admin oturumu acmalisiniz! Usg: LOGIN <sifre>"));
        return;
    }

    // Yetki Alınmış Komutlar
    if (command == "START_AUTO") {
        currentState = SystemState::AUTO_DRIVE;
        Serial.println(F("[MODE] Otonom Surus Modu Baslatildi."));
    } 
    else if (command == "FORWARD") {
        uint8_t spd = param.length() > 0 ? param.toInt() : 150;
        motors.moveForward(spd);
        Serial.print(F("[MANUEL] Ileri git: Hiz "));
        Serial.println(spd);
    }
    else if (command == "BACKWARD") {
        uint8_t spd = param.length() > 0 ? param.toInt() : 150;
        motors.moveBackward(spd);
        Serial.print(F("[MANUEL] Geri git: Hiz "));
        Serial.println(spd);
    }
    else if (command == "LEFT") {
        uint8_t spd = param.length() > 0 ? param.toInt() : 150;
        motors.turnLeft(spd);
        Serial.println(F("[MANUEL] Sola donus"));
    }
    else if (command == "RIGHT") {
        uint8_t spd = param.length() > 0 ? param.toInt() : 150;
        motors.turnRight(spd);
        Serial.println(F("[MANUEL] Saga donus"));
    }
    else if (command == "GET_DIST") {
        Serial.print(F("[SENSOR] Ham Mesafe: "));
        Serial.print(sensors.getRawDistanceCm());
        Serial.print(F(" cm | Filtreli Mesafe: "));
        Serial.print(sensors.getFilteredDistanceCm());
        Serial.println(F(" cm"));
    }
    else if (command == "SET_LIMIT") {
        float limit = param.toFloat();
        sensors.setObstacleThreshold(limit);
        Serial.print(F("[CONFIG] Yeni engel siniri ayarlandi: "));
        Serial.print(limit);
        Serial.println(F(" cm"));
    }
    else {
        Serial.println(F("[HATA] Bilinmeyen Admin Komutu!"));
    }
}

void triggerEmergencyStop(const char* reason) {
    motors.stopAll();
    currentState = SystemState::SAFE_STOP;
    Serial.print(F("[ALARM] ACIL DURUS! Nedeni: "));
    Serial.println(reason);
}
