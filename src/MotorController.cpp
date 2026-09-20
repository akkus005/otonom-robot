/*
 * Proje: Okul Robotu - Motor Controller Module
 * Dosya: src/MotorController.cpp
 * Geliştirici: Akkuş
 * Açıklama: MotorController sınıfının kaynak kodudur. 
 * Hız sınırlandırmaları (constrain) ve güvenli duruş fonksiyonlarını içerir.
 */

#include "MotorController.h"

// Kurucu Fonksiyon (Constructor): Pin atamalarını yapar
MotorController::MotorController(uint8_t lIn1, uint8_t lIn2, uint8_t lEn, 
                                 uint8_t rIn1, uint8_t rIn2, uint8_t rEn) {
    leftIn1 = lIn1;
    leftIn2 = lIn2;
    leftEnable = lEn;
    
    rightIn1 = rIn1;
    rightIn2 = rIn2;
    rightEnable = rEn;

    currentLeftSpeed = 0;
    currentRightSpeed = 0;
}

// Donanım pinlerini çıkış olarak ayarlar ve motorları güvenli konumda başlatır
void MotorController::begin() {
    pinMode(leftIn1, OUTPUT);
    pinMode(leftIn2, OUTPUT);
    pinMode(leftEnable, OUTPUT);

    pinMode(rightIn1, OUTPUT);
    pinMode(rightIn2, OUTPUT);
    pinMode(rightEnable, OUTPUT);

    stopAll(); // Başlangıçta motorların aniden dönmesini engelle
}

// İleri Sürüş
void MotorController::moveForward(uint8_t speed) {
    speed = constrain(speed, MIN_SPEED, MAX_SPEED); // Hızı güvenli aralıkta tut
    
    // Sol Motor İleri
    digitalWrite(leftIn1, HIGH);
    digitalWrite(leftIn2, LOW);
    analogWrite(leftEnable, speed);

    // Sağ Motor İleri
    digitalWrite(rightIn1, HIGH);
    digitalWrite(rightIn2, LOW);
    analogWrite(rightEnable, speed);

    currentLeftSpeed = speed;
    currentRightSpeed = speed;
}

// Geri Sürüş
void MotorController::moveBackward(uint8_t speed) {
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);
    
    // Sol Motor Geri
    digitalWrite(leftIn1, LOW);
    digitalWrite(leftIn2, HIGH);
    analogWrite(leftEnable, speed);

    // Sağ Motor Geri
    digitalWrite(rightIn1, LOW);
    digitalWrite(rightIn2, HIGH);
    analogWrite(rightEnable, speed);

    currentLeftSpeed = speed;
    currentRightSpeed = speed;
}

// Sola Dönüş (Tank Dönüşü: Sol geri, Sağ ileri)
void MotorController::turnLeft(uint8_t speed) {
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);
    
    // Sol Motor Geri
    digitalWrite(leftIn1, LOW);
    digitalWrite(leftIn2, HIGH);
    analogWrite(leftEnable, speed);

    // Sağ Motor İleri
    digitalWrite(rightIn1, HIGH);
    digitalWrite(rightIn2, LOW);
    analogWrite(rightEnable, speed);
}

// Sağa Dönüş (Tank Dönüşü: Sol ileri, Sağ geri)
void MotorController::turnRight(uint8_t speed) {
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);
    
    // Sol Motor İleri
    digitalWrite(leftIn1, HIGH);
    digitalWrite(leftIn2, LOW);
    analogWrite(leftEnable, speed);

    // Sağ Motor Geri
    digitalWrite(rightIn1, LOW);
    digitalWrite(rightIn2, HIGH);
    analogWrite(rightEnable, speed);
}

// Tüm Motorları Güvenli Şekilde Durdur
void MotorController::stopAll() {
    digitalWrite(leftIn1, LOW);
    digitalWrite(leftIn2, LOW);
    analogWrite(leftEnable, 0);

    digitalWrite(rightIn1, LOW);
    digitalWrite(rightIn2, LOW);
    analogWrite(rightEnable, 0);

    currentLeftSpeed = 0;
    currentRightSpeed = 0;
}

// Hassas / Otonom Sürüş: Her motora ayrı hız ve yön verebilme
// Pozitif değerler ileri, negatif değerler geri sürüş içindir (-255 ile +255 arası)
void MotorController::driveCustom(int16_t leftSpeed, int16_t rightSpeed) {
    // Hızları taşırma hatalarına karşı sınırla
    leftSpeed = constrain(leftSpeed, -MAX_SPEED, MAX_SPEED);
    rightSpeed = constrain(rightSpeed, -MAX_SPEED, MAX_SPEED);

    // Sol Motor Kontrolü
    if (leftSpeed > 0) {
        digitalWrite(leftIn1, HIGH);
        digitalWrite(leftIn2, LOW);
        analogWrite(leftEnable, leftSpeed);
    } else if (leftSpeed < 0) {
        digitalWrite(leftIn1, LOW);
        digitalWrite(leftIn2, HIGH);
        analogWrite(leftEnable, -leftSpeed); // Negatif hızı pozitife çevirerek PWM'e gönder
    } else {
        digitalWrite(leftIn1, LOW);
        digitalWrite(leftIn2, LOW);
        analogWrite(leftEnable, 0);
    }

    // Sağ Motor Kontrolü
    if (rightSpeed > 0) {
        digitalWrite(rightIn1, HIGH);
        digitalWrite(rightIn2, LOW);
        analogWrite(rightEnable, rightSpeed);
    } else if (rightSpeed < 0) {
        digitalWrite(rightIn1, LOW);
        digitalWrite(rightIn2, HIGH);
        analogWrite(rightEnable, -rightSpeed);
    } else {
        digitalWrite(rightIn1, LOW);
        digitalWrite(rightIn2, LOW);
        analogWrite(rightEnable, 0);
    }
}
