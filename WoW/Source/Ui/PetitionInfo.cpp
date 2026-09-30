#include "PetitionInfo.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00C22B20
WOWGUID CGPetitionInfo::m_petitionGUID;

// STATIC: 0x00C22B34
int32_t CGPetitionInfo::m_signed;

// STATIC: 0x00C22B40
CGPetition* CGPetitionInfo::m_petition;

// STATIC: 0x00C22B44
TSGrowableArray<PetitionSignerInfo> CGPetitionInfo::m_signers;

// VERIFY: a4620e5c auto
// OFFSET: 0x005CEC40
void CGPetitionInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_OFFER_PETITION_ERROR, Packet_SMSG_OFFER_PETITION_ERROR, nullptr);
}
