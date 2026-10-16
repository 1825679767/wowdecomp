// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
namespace wxl::appearance {struct Selection;}
class CCharacterComponent;
namespace wxl::native::alternate_appearances {
struct Stamp {
    uint64_t generation=0, primarySerial=0, alternateSerial=0;
    const CCharacterComponent* primary=nullptr;
    const CCharacterComponent* alternate=nullptr;
    uint32_t primaryHandle=0, alternateHandle=0, primaryRevision=0, alternateRevision=0;
    bool operator==(const Stamp& other) const {
        return generation==other.generation&&primarySerial==other.primarySerial&&alternateSerial==other.alternateSerial
            &&primary==other.primary&&alternate==other.alternate&&primaryHandle==other.primaryHandle
            &&alternateHandle==other.alternateHandle&&primaryRevision==other.primaryRevision&&alternateRevision==other.alternateRevision;
    }
};
// Main/FrameScript owner thread only. Primary remains the native creation actor.
void UpdateCreation() noexcept;
void ReleaseCreation() noexcept;
CCharacterComponent* Editor() noexcept;
bool Available() noexcept;
bool IsAlternate() noexcept;
bool ShowAlternate(bool) noexcept;
void SetFacing(float) noexcept;
bool RandomizeEditor() noexcept;
// Capture is read-only: it never constructs, mirrors or defaults either form.
bool Capture(wxl::appearance::Selection&,Stamp&) noexcept;
// Both model authorities validate/allocate before either component changes.
bool Restore(uint64_t guid,uint32_t revision,const wxl::appearance::Selection&) noexcept;
void InitializeLua();
}
