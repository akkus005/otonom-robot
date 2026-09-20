/*
 * Proje: Okul Robotu - Sensor Manager Module
 * Dosya: src/SensorManager.cpp
 * Geliştirici: Akkuş
 * Açıklama: SensorManager sınıfının kaynak kodudur. 
 * Zaman aşımı korumalı ultrasonik okuma ve hareketli ortalama filtresini içerir.
 */

#include "SensorManager.h"

// Kurucu Fonksiyon: Pin atamalarını ve tampon dizisini sıfırlar
SensorManager::SensorManager(uint8_t trig, uint8_t echo, float threshold) {
    trigPin = trig;
    echoPin = echo;
    obstacleThresholdCm = threshold;
    sampleIndex = 0;
    lastValidDistance = 400.0; // Varsayılan güvenli başlangıç mesafesi (cm)
    lastReadTime = 0;

    for (uint8_t i = 0; i < SAMPLE_COUNT; i++) {
        samples[i] = lastValidDistance;
    }
}

// Donanım pinlerini ayarlar
void SensorManager::begin() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);
}

// Ham (Filtresiz) Mesafe Okuma
float SensorManager::getRawDistanceCm() {
    // Sensörü tetikle (10 mikrosaniyelik HIGH palatması)
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Yankı süresini ölç (Kilitlenmeyi önlemek için max 30000us / ~5 metre timeout)
    long duration = pulseIn(echoPin, HIGH, 30000);

    // Zaman aşımı veya hatalı okuma durumu (Sinyal dönmediyse)
    if (duration == 0) {
        return lastValidDistance; // Sistem çökmesin diye son geçerli mesafeyi dön
    }

    // Ses hızı hesabı (343 m/s -> 0.0343 cm/us), gidiş-dönüş olduğu için 2'ye bölüyoruz
    float distance = (duration * 0.0343) / 2.0;

    // Fiziksel sınır denetimi (2 cm - 400 cm dışındaki saçma verileri ayıkla)
    if (distance < 2.0 || distance > 400.0) {
        return lastValidDistance;
    }

    lastValidDistance = distance;
    return distance;
}

// Non-blocking (Döngüyü durdurmayan) zamanlayıcı güncellemesi
void SensorManager::update() {
    unsigned long now = millis();
    if (now - lastReadTime >= READ_INTERVAL_MS) {
        lastReadTime = now;

        // Tampona yeni okunan veriyi kaydet
        samples[sampleIndex] = getRawDistanceCm();
        sampleIndex = (sampleIndex + 1) % SAMPLE_COUNT; // Dairesel indeks yönetimi
    }
}

// Moving Average (Hareketli Ortalama) Filtreli Mesafe Okuma
float SensorManager::getFilteredDistanceCm() {
    float sum = 0.0;
    for (uint8_t i = 0; i < SAMPLE_COUNT; i++) {
        sum += samples[i];
    }
    return sum / (float)SAMPLE_COUNT;
}

// Engelin kritik sınır içinde olup olmadığını kontrol et
bool SensorManager::isObstacleDetected() {
    return (getFilteredDistanceCm() <= obstacleThresholdCm);
}

// Dinamik olarak engel sınırını güncelleme (Admin paneli için)
void SensorManager::setObstacleThreshold(float threshold) {
    if (threshold > 5.0 && threshold < 200.0) {
        obstacleThresholdCm = threshold;
    }
}
