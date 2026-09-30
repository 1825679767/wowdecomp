#include "PetitionVendor.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
// STATIC: 0x00C0F7C8
WOWGUID CGPetitionVendorInfo::m_vendor;

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
