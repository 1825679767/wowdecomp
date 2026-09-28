#include "ComSatInterface.h"
#include "Engine/Source/ComSat/ComSatSoundIOSoundEngine.h"

// STATIC: 0x00D43988
static ComSat::Sound::Manager* unk_D43988;

// STATIC: 0x00D4398C
static ComSat::Network::Manager* unk_D4398C;

// INLINED
ComSat::Allocator::~Allocator() {
}

// VERIFY: d03c9b46 auto
// OFFSET: 0x00880B30
void ComSat::Interface::Destroy(ComSat::Interface* comsat, ComSat::Allocator& allocator) {
    if (comsat) {
        comsat->~Interface();
        allocator.Free(comsat);
    }

    if (unk_D43988) {
        ComSat::Sound::Manager* soundManager = unk_D43988;
        soundManager->~Manager();
        allocator.Free(soundManager);
        unk_D43988 = nullptr;
    }

    if (unk_D4398C) {
        ComSat::Network::Manager* networkManager = unk_D4398C;
        networkManager->~Manager();
        allocator.Free(networkManager);
        unk_D4398C = nullptr;
    }
}
