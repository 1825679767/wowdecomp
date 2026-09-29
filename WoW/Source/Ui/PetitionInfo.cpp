#include "PetitionInfo.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// VERIFY: a4620e5c auto
// OFFSET: 0x005CEC40
void CGPetitionInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_OFFER_PETITION_ERROR, Packet_SMSG_OFFER_PETITION_ERROR, nullptr);
}
