/*
 * Proje: Okul Robotu - Konfigürasyon ve Pin Haritası
 * Dosya: src/ESP32_Config.h
 */

#ifndef ESP32_CONFIG_H
#define ESP32_CONFIG_H

// --- Wi-Fi Erişim Noktası (Robotun Kendi Ağı) ---
#define SOFT_AP_SSID          "OkulRobotu_AP"
#define SOFT_AP_PASS          "robot123456"

// --- Motor Sürücü Pin Tanımlamaları ---
#define MOTOR_A_IN1           26
#define MOTOR_A_IN2           27
#define MOTOR_B_IN1           32
#define MOTOR_B_IN2           33
#define MOTOR_ENA             14
#define MOTOR_ENB             12

// --- Ultrasonik Sensör Pinleri ---
#define TRIG_PIN_FRONT        5
#define ECHO_PIN_FRONT        18
#define TRIG_PIN_LEFT         19
#define ECHO_PIN_LEFT         21
#define TRIG_PIN_RIGHT        22
#define ECHO_PIN_RIGHT        23

// --- Manevra ve Mesafe Eşikleri (cm) ---
#define NORMAL_OBSTACLE_DIST  30  // Koridordaki engel mesafesi
#define ELEVATOR_OBSTACLE_DIST 8  // Asansör içi engel mesafesi

#endif // ESP32_CONFIG_H
