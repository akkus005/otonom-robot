/*
 * Proje: Okul Robotu - Sensor Manager Module
 * Dosya: src/SensorManager.h
 * Geliştirici: Akkuş
 * Açıklama: Ultrasonik/Mesafe sensörlerinden gelen verileri
 * Moving Average (Hareketli Ortalama) filtresi ile temizleyen
 * ve gürültüsüz engel algılama sağlayan başlık dosyası.
 */

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>

class SensorManager {
private:
    uint8_t trigPin;
    uint8_t echoPin;

    // Sinyal Parazitlerini Engelleyen Filtreleme Tamponu
    static const uint8_t SAMPLE_COUNT = 5;
    float samples[SAMPLE_COUNT];
    uint8_t sampleIndex;

    // Sistem Ayarları ve Sınır Değerler
    float lastValidDistance;
    float obstacleThresholdCm;
    unsigned long lastReadTime;
    static const unsigned long READ_INTERVAL_MS = 40; // Sensör tetikleme sıklığı (ms)

public:
    SensorManager(uint8_t trig, uint8_t echo, float threshold = 15.0);

    void begin();
    void update(); // Non-blocking (zamanlayıcı tabanlı) sensör güncelleme
    float getRawDistanceCm();
    float getFilteredDistanceCm();
    bool isObstacleDetected();
    void setObstacleThreshold(float threshold);
};

#endif // SENSOR_MANAGER_H
