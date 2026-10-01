#include "PetitionInfo.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00C22B20
WOWGUID CGPetitionInfo::m_petitionGUID;

// STATIC: 0x00C22B28
WOWGUID CGPetitionInfo::m_petitionItem;

// STATIC: 0x00C22B30
uint32_t CGPetitionInfo::unk_C22B30;

// STATIC: 0x00C22B34
int32_t CGPetitionInfo::m_signed;

// STATIC: 0x00C22B38
uint32_t CGPetitionInfo::m_numPetitionNames;

// STATIC: 0x00C22B3C
int32_t CGPetitionInfo::m_pendingNameQueries;

// STATIC: 0x00C22B40
CGPetition* CGPetitionInfo::m_petition;

// STATIC: 0x00C22B44
TSGrowableArray<PetitionSignerInfo> CGPetitionInfo::m_signers;

// VERIFY: a4620e5c auto
// OFFSET: 0x005CEC40
void CGPetitionInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_OFFER_PETITION_ERROR, Packet_SMSG_OFFER_PETITION_ERROR, nullptr);
}

// VERIFY: 5e76943f manual
// OFFSET: 0x005CF580
void CGPetitionInfo::EnterWorld() {
    CGPetitionInfo::m_petitionGUID = WOWGUID{ 0 };
    CGPetitionInfo::m_petitionItem = WOWGUID{ 0 };
    CGPetitionInfo::unk_C22B30 = 0;
    CGPetitionInfo::m_numPetitionNames = 0;
    CGPetitionInfo::m_pendingNameQueries = 0;
    CGPetitionInfo::m_petition = nullptr;
    CGPetitionInfo::m_signers.SetCount(10);
}

// VERIFY: 8452d77d auto
// OFFSET: 0x005CE9F0
void CGPetitionInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_OFFER_PETITION_ERROR);
}
