#ifndef WOW_SOURCE_UI_CHATFRAME_H
#define WOW_SOURCE_UI_CHATFRAME_H

#include <cstddef>
#include <cstdint>

class CGChat {
    public:
    static int32_t m_paused;

    static int32_t GetActiveVoiceSession();
    static const char* GetChannelStringForDisplay(int32_t session, int32_t* type);
    static int32_t IsPaused();
    static bool IsVoiceChatAllowed();
    static void LeaveWorld();
    static void Resume();
    static void SetActiveVoiceSession(const char* channel, int32_t type, int32_t index);
};

class PENDINGCHAT {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t m_dwordC;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint8_t unk_0034 [128];
    /* 00B4 */ uint8_t unk_00B4 [28];
    /* 00D0 */ uint32_t m_dwordD0;
    /* 00D4 */ uint32_t m_dwordD4;
};

#endif
