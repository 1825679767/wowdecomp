#ifndef WOW_SOURCE_WOWSERVICES_LOGIN_H
#define WOW_SOURCE_WOWSERVICES_LOGIN_H

#include <cstddef>
#include <cstdint>
#include <windows.h>

struct LoginResponse;

class Login {
    public:
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint8_t m_ukn121;
    /* 000D */ char m_username [1280];
    /* 050D */ char m_rawUsername [1280];
    /* 0A0D */ char m_uknStr2 [1280];
    /* 0F0D */ uint8_t unk_0F0D[3];
    /* 0F10 */ char* m_password;
    /* 0F14 */ uint32_t unk_0F14 [98];
    /* 109C */ uint8_t unk_109C;
    /* 109D */ uint8_t m_matrixWidth;
    /* 109E */ uint8_t m_matrixHeight;
    /* 109F */ uint8_t m_matrixMinDigitCount;
    /* 10A0 */ uint8_t m_matrixMaxDigitCount;
    /* 10A1 */ uint8_t m_matrixChallengeCount;
    /* 10A2 */ uint8_t m_matrixFlipCoords;
    /* 10A3 */ uint8_t unk_10A3;
    /* 10A4 */ uint32_t m_loginUkn9;
    /* 10A8 */ uint32_t m_loginUkn10;
    /* 10AC */ uint32_t m_loginUkn11;
    /* 10B0 */ uint32_t m_loginUkn12;
    /* 10B4 */ uint32_t m_loginUkn13;
    /* 10B8 */ uint32_t m_loginUkn14;
    /* 10BC */ uint32_t m_loginUkn15;
    /* 10C0 */ uint32_t m_loginUkn16;
    /* 10C4 */ uint32_t m_loginUkn17;
    /* 10C8 */ uint32_t m_loginUkn18;
    /* 10CC */ uint32_t m_loginUkn19;
    /* 10D0 */ uint32_t m_loginUkn20;
    /* 10D4 */ uint8_t m_tokenRequired;
    /* 10D5 */ uint8_t unk_10D5;
    /* 10D6 */ uint8_t unk_10D6;
    /* 10D7 */ uint8_t unk_10D7;
    /* 10D8 */ uint32_t m_loginUkn2;
    /* 10DC */ uint32_t m_loginUkn3;
    /* 10E0 */ uint32_t m_loginUkn4;
    /* 10E4 */ uint32_t m_loginUkn5;
    /* 10E8 */ uint32_t m_loginUkn6;
    /* 10EC */ LoginResponse* m_loginResponse;

    virtual ~Login();
};

#endif
