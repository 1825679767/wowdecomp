#include "PetitionVendor.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
// STATIC: 0x00C0F7C8
WOWGUID CGPetitionVendorInfo::m_vendor;

// STATIC: 0x00C0F7D0
uint32_t CGPetitionVendorInfo::m_numPetitionItems;

// STATIC: 0x00C0F7D8
WOWGUID CGPetitionVendorInfo::m_petitionItem;


// VERIFY: 03561478 auto
// OFFSET: 0x005A2880
void PetitionVendorClose() {
    if (!CGPetitionVendorInfo::m_vendor) {
        return;
    }

    CGGameUI::ClearInteractTarget(CGPetitionVendorInfo::m_vendor, 0);
    CGPetitionVendorInfo::m_vendor = WOWGUID();
    FrameScript_SignalEvent(SCRIPT_EVENT_PETITION_VENDOR_CLOSED, nullptr);
}

// VERIFY: 670e5b68 auto
// OFFSET: 0x005A1F50
void CGPetitionVendorInfo::EnterWorld() {
    CGPetitionVendorInfo::m_numPetitionItems = 0;
    CGPetitionVendorInfo::m_petitionItem = WOWGUID{ 0 };
}
