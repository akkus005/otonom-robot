/*
 * Proje: Okul Robotu - Admin Authorization Module
 * Dosya: src/AdminAuth.cpp
 * Geliştirici: Akkuş
 * Açıklama: AdminAuth sınıfının kaynak kodudur. 
 * Şifre doğrulama, kaba kuvvet (brute-force) koruması ve kilitlenme mantığını içerir.
 */

#include "AdminAuth.h"

// Kurucu Fonksiyon: Varsayılan şifre ve değişkenleri ayarla
AdminAuth::AdminAuth(String defaultPassword) {
    currentPassword = defaultPassword;
    authenticated = false;
    failedAttempts = 0;
}

// Başlangıç Ayarları
void AdminAuth::begin() {
    authenticated = false;
    failedAttempts = 0;
}

// Şifre kontrolü ve sisteme giriş
bool AdminAuth::login(String enteredPassword) {
    // Eğer sistem kilitliyse (çok fazla yanlış deneme) girişe direkt ret ver
    if (isLocked()) {
        return false;
    }

    // Şifre Doğruysa
    if (enteredPassword == currentPassword) {
        authenticated = true;
        failedAttempts = 0; // Başarılı girişte hata sayacını sıfırla
        return true;
    } 
    // Şifre Yanlışsa
    else {
        authenticated = false;
        failedAttempts++;
        return false;
    }
}

// Güvenli çıkış (Admin modundan çık)
void AdminAuth::logout() {
    authenticated = false;
}

// Admin oturumu açık mı? (Sistem kilitliyse false döner)
bool AdminAuth::isLoggedIn() {
    return (authenticated && !isLocked());
}

// Sistem kilitli mi? (Maksimum hatalı giriş denemesi aşıldıysa)
bool AdminAuth::isLocked() {
    return (failedAttempts >= MAX_FAILED_ATTEMPTS);
}

// Kilit durumunu sıfırla (Donanımsal veya özel bir komutla çağrılır)
void AdminAuth::resetLockout() {
    failedAttempts = 0;
    authenticated = false;
}
