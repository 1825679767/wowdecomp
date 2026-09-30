#include "GuildRegistrar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"

// STATIC: 0x00C0E570
WOWGUID CGGuildRegistrar::m_registrar;

// STATIC: 0x00C0E558
GuildPetitionInfo CGGuildRegistrar::m_petition;

// VERIFY: 0eca3b09 auto
// OFFSET: 0x00599DD0
void CGGuildRegistrar::EnterWorld() {
    CGGuildRegistrar::m_petition.unk_0000 = 0;
    CGGuildRegistrar::m_petition.m_itemID = 0;
    CGGuildRegistrar::m_petition.m_displayID = 0;
    CGGuildRegistrar::m_petition.m_cost = 0;
    CGGuildRegistrar::m_petition.unk_0010 = 0;
    CGGuildRegistrar::m_petition.unk_0014 = 0;
}

// VERIFY: 6f5e7e79 auto
// OFFSET: 0x00599E90
void CGGuildRegistrar::CloseRegistrar() {
    if (!CGGuildRegistrar::m_registrar) {
        return;
    }

    CGGameUI::ClearInteractTarget(CGGuildRegistrar::m_registrar, 0);
    CGGuildRegistrar::m_registrar = WOWGUID();
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_REGISTRAR_CLOSED, nullptr);
}
