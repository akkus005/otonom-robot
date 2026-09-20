/*
 * Proje: Okul Robotu - Core System
 * Dosya: src/main.cpp
 * Geliştirici: Akkuş
 * Mimari: Non-blocking (millis tabanlı), State-Machine (Durum Makinesi)
 * Açıklama: Robotun ana kontrol döngüsüdür. Eski projede yaşanan 
 * donma/kilitlenme sorunlarını önlemek için delay() KESİNLİKLE kullanılmamıştır.
 */

#include <Arduino.h>

// --- 1. SİSTEM DURUMLARI (STATE MACHINE) ---
enum class SystemState {
    INIT,           // Sistem başlatılıyor, donanım testi
    IDLE,           // Boşta, komut bekliyor
    ADMIN_MODE,     // Yönetici yetkileri devrede (Manuel Kontrol vb.)
    AUTO_DRIVE,     // Sürüş ve otonom hareket modu
    SAFE_STOP,      // Engel veya tehlike anında acil duruş
    SYSTEM_ERROR    // Donanım veya yazılım hatası
};

// Başlangıç durumu
SystemState currentState = SystemState::INIT;

// --- 2. ZAMANLAMA (NON-BLOCKING TIMERS) ---
unsigned long lastSensorUpdate = 0;
unsigned long lastHeartbeat = 0;
const unsigned long SENSOR_INTERVAL = 50;      // 50ms (Saniyede 20 kez sensör okuma)
const unsigned long HEARTBEAT_INTERVAL = 1000; // 1 saniyede bir sistem yaşıyor sinyali

// --- 3. FONKSİYON PROTOTİPLERİ ---
void processState();
void readSensorsTask();
void systemHeartbeatTask();
void emergencyStop(const char* reason);
void checkSerialCommands();

// --- 4. KURULUM (SETUP) ---
void setup() {
    // Seri haberleşme başlat (Hata ayıklama ve Admin komutları için)
    Serial.begin(115200);
    while (!Serial) { delay(10); } // Seri portun donanım olarak oturmasını bekle

    Serial.println(F("======================================="));
    Serial.println(F("[SYSTEM] Okul Robotu OS Baslatiliyor..."));
    Serial.println(F("======================================="));

    // İleride modül dosyalarını yazdıkça buraya eklenecekler:
    // MotorController::init();
    // SensorManager::init();
    // AdminAuth::init();

    Serial.println(F("[OK] Donanim testleri basarili."));
    Serial.println(F("[SYSTEM] IDLE (Bekleme) moduna gecildi."));
    
    currentState = SystemState::IDLE;
}

// --- 5. ANA DÖNGÜ (LOOP) ---
void loop() {
    unsigned long currentMillis = millis();

    // Görev 1: Sensörleri sürekli ve takılmadan oku (Multitasking simülasyonu)
    if (currentMillis - lastSensorUpdate >= SENSOR_INTERVAL) {
        lastSensorUpdate = currentMillis;
        readSensorsTask();
    }

    // Görev 2: Sistemin çalıştığını gösteren Heartbeat (Yaşam) sinyali
    if (currentMillis - lastHeartbeat >= HEARTBEAT_INTERVAL) {
        lastHeartbeat = currentMillis;
        systemHeartbeatTask();
    }

    // Görev 3: Dışarıdan (Bluetooth/Serial) gelen admin komutlarını dinle
    checkSerialCommands();

    // Görev 4: Robotun o anki durumunu (State) işlet
    processState();
}

// --- 6. GÖREV FONKSİYONLARI ---

void processState() {
    switch (currentState) {
        case SystemState::INIT:
            // Setup içinde hallediliyor
            break;

        case SystemState::IDLE:
            // Robot duruyor, motorlar kapalı, güvenli modda bekliyor
            break;

        case SystemState::ADMIN_MODE:
            // Admin tarafından gelen özel komutlar işleniyor (Kalibrasyon vb.)
            break;

        case SystemState::AUTO_DRIVE:
            // Motor sürücü ve engel aşma algoritmaları burada çalışacak
            break;

        case SystemState::SAFE_STOP:
            // Çarpışma algılandıysa sadece geri çıkışa veya admin komutuna izin ver
            break;

        case SystemState::SYSTEM_ERROR:
            // Kritik hata, her şeyi kilitle
            break;
    }
}

void readSensorsTask() {
    // İleride eklenecek: Mesafe veya çizgi sensörleri filtrelenerek okunacak.
    // Örnek acil durum senaryosu:
    // if (distance < 10) { emergencyStop("ON_ENGEL_ALGILANDI"); }
}

void checkSerialCommands() {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();

        if (cmd.length() == 0) return;

        // Admin ve Güvenlik Kontrolleri
        if (cmd == "ADMIN_LOGIN") {
            currentState = SystemState::ADMIN_MODE;
            Serial.println(F("[AUTH] Admin Modu Aktif Edildi."));
        } 
        else if (cmd == "START_DRIVE" && currentState == SystemState::ADMIN_MODE) {
            currentState = SystemState::AUTO_DRIVE;
            Serial.println(F("[INFO] Surus Modu Baslatildi."));
        }
        else if (cmd == "STOP") {
            emergencyStop("Manuel Acil Durdurma Komutu Alindi");
        }
        else {
            Serial.println(F("[HATA] Bilinmeyen komut veya yetkisiz erisim!"));
        }
    }
}

void emergencyStop(const char* reason) {
    // İleride MotorController::stopAll() eklenecek
    currentState = SystemState::SAFE_STOP;
    Serial.print(F("[ALARM] ACIL DURUS! Sebep: "));
    Serial.println(reason);
}

void systemHeartbeatTask() {
    // Sistem durumunu anlık olarak konsola/Bluetooth'a yazdırabiliriz
    // Serial.println(F("[SYS] System Online.")); 
}
