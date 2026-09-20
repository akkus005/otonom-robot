/*
 * Proje: Okul Robotu - Motor Controller Module
 * Dosya: src/MotorController.h
 * Geliştirici: Akkuş
 * Açıklama: Robotun motor sürücü pinlerini, hız (PWM) ayarlarını ve 
 * yön kontrol fonksiyonlarını tanımlayan nesne yönelimli (OOP) başlık dosyası.
 */

#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>

class MotorController {
private:
    // Sol Motor Pinleri
    uint8_t leftIn1;
    uint8_t leftIn2;
    uint8_t leftEnable;

    // Sağ Motor Pinleri
    uint8_t rightIn1;
    uint8_t rightIn2;
    uint8_t rightEnable;

    // Anlık Hız Değerleri (0 - 255)
    uint8_t currentLeftSpeed;
    uint8_t currentRightSpeed;

    // Güvenlik için maksimum hız sınırı
    static const uint8_t MAX_SPEED = 255;
    static const uint8_t MIN_SPEED = 0;

public:
    // Kurucu Fonksiyon (Constructor)
    MotorController(uint8_t lIn1, uint8_t lIn2, uint8_t lEn, 
                    uint8_t rIn1, uint8_t rIn2, uint8_t rEn);

    // Başlatma ve Donanım Ayarları
    void begin();

    // Sürüş Komutları
    void moveForward(uint8_t speed);
    void moveBackward(uint8_t speed);
    void turnLeft(uint8_t speed);
    void turnRight(uint8_t speed);
    void stopAll();

    // Özel Hassas Sürüş (Tank Sürüşü - Her iki motor ayrı kontrol edilebilir)
    void driveCustom(int16_t leftSpeed, int16_t rightSpeed);
};

#endif // MOTOR_CONTROLLER_H
