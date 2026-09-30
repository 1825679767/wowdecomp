#include "GuildRegistrar.h"

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
