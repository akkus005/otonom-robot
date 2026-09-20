/*
 * Proje: Okul Robotu - Admin Authorization Module
 * Dosya: src/AdminAuth.h
 * Geliştirici: Akkuş
 * Açıklama: Sisteme yetkisiz erişimi engellemek için
 * şifreli giriş, oturum kontrolü ve güvenlik durumlarını yöneten başlık dosyası.
 */

#ifndef ADMIN_AUTH_H
#define ADMIN_AUTH_H

#include <Arduino.h>

class AdminAuth {
private:
    String currentPassword;
    bool authenticated;
    uint8_t failedAttempts;
    
    // Güvenlik için maksimum hatalı giriş sınırı
    static const uint8_t MAX_FAILED_ATTEMPTS = 3;

public:
    // Kurucu Fonksiyon: Varsayılan şifreyi belirler
    AdminAuth(String defaultPassword = "akkus_admin123");

    // Başlangıç ayarları
    void begin();

    // Şifre kontrolü ve sisteme giriş
    bool login(String enteredPassword);

    // Güvenli çıkış
    void logout();

    // Adminin içeride olup olmadığını döndürür
    bool isLoggedIn();

    // Çok fazla hatalı deneme olursa sistemi kilitler
    bool isLocked();

    // Sistemin kilidini açar (Sadece donanımsal reset veya özel durumda)
    void resetLockout();
};

#endif // ADMIN_AUTH_H
