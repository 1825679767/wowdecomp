#ifndef WOW_SOURCE_DB_REC_VEHICLESEATREC_H
#define WOW_SOURCE_DB_REC_VEHICLESEATREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct VehicleSeatRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_field33512213001;
    /* 0008 */ int32_t m_attachmentID;
    /* 000C */ float m_attachmentOffset [3];
    /* 0018 */ float m_field33512213004;
    /* 001C */ float m_enterSpeed;
    /* 0020 */ float m_enterGravity;
    /* 0024 */ float m_enterMinDuration;
    /* 0028 */ float m_enterMaxDuration;
    /* 002C */ float m_enterMinArcHeight;
    /* 0030 */ float m_enterMaxArcHeight;
    /* 0034 */ int32_t m_enterAnimStart;
    /* 0038 */ int32_t m_enterAnimLoop;
    /* 003C */ int32_t m_rideAnimStart;
    /* 0040 */ int32_t m_rideAnimLoop;
    /* 0044 */ int32_t m_rideUpperAnimStart;
    /* 0048 */ int32_t m_rideUpperAnimLoop;
    /* 004C */ float m_field33512213017;
    /* 0050 */ float m_exitSpeed;
    /* 0054 */ float m_exitGravity;
    /* 0058 */ float m_exitMinDuration;
    /* 005C */ float m_exitMaxDuration;
    /* 0060 */ float m_exitMinArcHeight;
    /* 0064 */ float m_exitMaxArcHeight;
    /* 0068 */ int32_t m_exitAnimStart;
    /* 006C */ int32_t m_exitAnimLoop;
    /* 0070 */ int32_t m_exitAnimEnd;
    /* 0074 */ float m_field33512213027;
    /* 0078 */ float m_passengerPitch;
    /* 007C */ float m_field33512213029;
    /* 0080 */ int32_t m_passengerAttachmentID;
    /* 0084 */ int32_t m_vehicleEnterAnim;
    /* 0088 */ int32_t m_vehicleExitAnim;
    /* 008C */ int32_t m_vehicleRideAnimLoop;
    /* 0090 */ int32_t m_field33512213034;
    /* 0094 */ int32_t m_vehicleExitAnimBone;
    /* 0098 */ int32_t m_vehicleEnterAnimBone;
    /* 009C */ float m_field33512213037;
    /* 00A0 */ float m_field33512213038;
    /* 00A4 */ int32_t m_vehicleAbilityDisplay;
    /* 00A8 */ int32_t m_enterUisoundID;
    /* 00AC */ int32_t m_field33512213041;
    /* 00B0 */ int32_t m_uiSkin;
    /* 00B4 */ float m_field33512213043;
    /* 00B8 */ float m_field33512213044;
    /* 00BC */ float m_field33512213045;
    /* 00C0 */ int32_t m_field33512213046;
    /* 00C4 */ float m_field33512213047;
    /* 00C8 */ float m_field33512213048;
    /* 00CC */ float m_field33512213049;
    /* 00D0 */ float m_field33512213050;
    /* 00D4 */ float m_field33512213051;
    /* 00D8 */ float m_field33512213052;
    /* 00DC */ float m_field33512213053;
    /* 00E0 */ float m_field33512213054;
    /* 00E4 */ float m_field33512213055;

// VERIFY: 17a7cd0b auto
    // OFFSET: 0x008BAD50
    static const char* GetFilename() {
        return "DBFilesClient\\VehicleSeat.dbc";
    }

// VERIFY: a86e6f35 auto
    // OFFSET: 0x008BAD60
    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t field33512213001Read = SFileReadFile(file, &this->m_field33512213001, sizeof(this->m_field33512213001), nullptr, nullptr, 0);
        int32_t attachmentIDRead = SFileReadFile(file, &this->m_attachmentID, sizeof(this->m_attachmentID), nullptr, nullptr, 0);
        int32_t attachmentRead0 = SFileReadFile(file, &this->m_attachmentOffset[0], sizeof(this->m_attachmentOffset[0]), nullptr, nullptr, 0);
        int32_t attachmentRead1 = SFileReadFile(file, &this->m_attachmentOffset[1], sizeof(this->m_attachmentOffset[1]), nullptr, nullptr, 0);
        int32_t attachmentRead2 = SFileReadFile(file, &this->m_attachmentOffset[2], sizeof(this->m_attachmentOffset[2]), nullptr, nullptr, 0);
        int32_t field33512213004Read = SFileReadFile(file, &this->m_field33512213004, sizeof(this->m_field33512213004), nullptr, nullptr, 0);
        int32_t enterSpeedRead = SFileReadFile(file, &this->m_enterSpeed, sizeof(this->m_enterSpeed), nullptr, nullptr, 0);
        int32_t enterGravityRead = SFileReadFile(file, &this->m_enterGravity, sizeof(this->m_enterGravity), nullptr, nullptr, 0);
        int32_t enterMinDurationRead = SFileReadFile(file, &this->m_enterMinDuration, sizeof(this->m_enterMinDuration), nullptr, nullptr, 0);
        int32_t enterMaxDurationRead = SFileReadFile(file, &this->m_enterMaxDuration, sizeof(this->m_enterMaxDuration), nullptr, nullptr, 0);
        int32_t enterMinArcHeightRead = SFileReadFile(file, &this->m_enterMinArcHeight, sizeof(this->m_enterMinArcHeight), nullptr, nullptr, 0);
        int32_t enterMaxArcHeightRead = SFileReadFile(file, &this->m_enterMaxArcHeight, sizeof(this->m_enterMaxArcHeight), nullptr, nullptr, 0);
        int32_t enterAnimStartRead = SFileReadFile(file, &this->m_enterAnimStart, sizeof(this->m_enterAnimStart), nullptr, nullptr, 0);
        int32_t enterAnimLoopRead = SFileReadFile(file, &this->m_enterAnimLoop, sizeof(this->m_enterAnimLoop), nullptr, nullptr, 0);
        int32_t rideAnimStartRead = SFileReadFile(file, &this->m_rideAnimStart, sizeof(this->m_rideAnimStart), nullptr, nullptr, 0);
        int32_t rideAnimLoopRead = SFileReadFile(file, &this->m_rideAnimLoop, sizeof(this->m_rideAnimLoop), nullptr, nullptr, 0);
        int32_t rideUpperAnimStartRead = SFileReadFile(file, &this->m_rideUpperAnimStart, sizeof(this->m_rideUpperAnimStart), nullptr, nullptr, 0);
        int32_t rideUpperAnimLoopRead = SFileReadFile(file, &this->m_rideUpperAnimLoop, sizeof(this->m_rideUpperAnimLoop), nullptr, nullptr, 0);
        int32_t field33512213017Read = SFileReadFile(file, &this->m_field33512213017, sizeof(this->m_field33512213017), nullptr, nullptr, 0);
        int32_t exitSpeedRead = SFileReadFile(file, &this->m_exitSpeed, sizeof(this->m_exitSpeed), nullptr, nullptr, 0);
        int32_t exitGravityRead = SFileReadFile(file, &this->m_exitGravity, sizeof(this->m_exitGravity), nullptr, nullptr, 0);
        int32_t exitMinDurationRead = SFileReadFile(file, &this->m_exitMinDuration, sizeof(this->m_exitMinDuration), nullptr, nullptr, 0);
        int32_t exitMaxDurationRead = SFileReadFile(file, &this->m_exitMaxDuration, sizeof(this->m_exitMaxDuration), nullptr, nullptr, 0);
        int32_t exitMinArcHeightRead = SFileReadFile(file, &this->m_exitMinArcHeight, sizeof(this->m_exitMinArcHeight), nullptr, nullptr, 0);
        int32_t exitMaxArcHeightRead = SFileReadFile(file, &this->m_exitMaxArcHeight, sizeof(this->m_exitMaxArcHeight), nullptr, nullptr, 0);
        int32_t exitAnimStartRead = SFileReadFile(file, &this->m_exitAnimStart, sizeof(this->m_exitAnimStart), nullptr, nullptr, 0);
        int32_t exitAnimLoopRead = SFileReadFile(file, &this->m_exitAnimLoop, sizeof(this->m_exitAnimLoop), nullptr, nullptr, 0);
        int32_t exitAnimEndRead = SFileReadFile(file, &this->m_exitAnimEnd, sizeof(this->m_exitAnimEnd), nullptr, nullptr, 0);
        int32_t field33512213027Read = SFileReadFile(file, &this->m_field33512213027, sizeof(this->m_field33512213027), nullptr, nullptr, 0);
        int32_t passengerPitchRead = SFileReadFile(file, &this->m_passengerPitch, sizeof(this->m_passengerPitch), nullptr, nullptr, 0);
        int32_t field33512213029Read = SFileReadFile(file, &this->m_field33512213029, sizeof(this->m_field33512213029), nullptr, nullptr, 0);
        int32_t passengerAttachmentIDRead = SFileReadFile(file, &this->m_passengerAttachmentID, sizeof(this->m_passengerAttachmentID), nullptr, nullptr, 0);
        int32_t vehicleEnterAnimRead = SFileReadFile(file, &this->m_vehicleEnterAnim, sizeof(this->m_vehicleEnterAnim), nullptr, nullptr, 0);
        int32_t vehicleExitAnimRead = SFileReadFile(file, &this->m_vehicleExitAnim, sizeof(this->m_vehicleExitAnim), nullptr, nullptr, 0);
        int32_t vehicleRideAnimLoopRead = SFileReadFile(file, &this->m_vehicleRideAnimLoop, sizeof(this->m_vehicleRideAnimLoop), nullptr, nullptr, 0);
        int32_t field33512213034Read = SFileReadFile(file, &this->m_field33512213034, sizeof(this->m_field33512213034), nullptr, nullptr, 0);
        int32_t vehicleExitAnimBoneRead = SFileReadFile(file, &this->m_vehicleExitAnimBone, sizeof(this->m_vehicleExitAnimBone), nullptr, nullptr, 0);
        int32_t vehicleEnterAnimBoneRead = SFileReadFile(file, &this->m_vehicleEnterAnimBone, sizeof(this->m_vehicleEnterAnimBone), nullptr, nullptr, 0);
        int32_t field33512213037Read = SFileReadFile(file, &this->m_field33512213037, sizeof(this->m_field33512213037), nullptr, nullptr, 0);
        int32_t field33512213038Read = SFileReadFile(file, &this->m_field33512213038, sizeof(this->m_field33512213038), nullptr, nullptr, 0);
        int32_t vehicleAbilityDisplayRead = SFileReadFile(file, &this->m_vehicleAbilityDisplay, sizeof(this->m_vehicleAbilityDisplay), nullptr, nullptr, 0);
        int32_t enterUisoundIDRead = SFileReadFile(file, &this->m_enterUisoundID, sizeof(this->m_enterUisoundID), nullptr, nullptr, 0);
        int32_t field33512213041Read = SFileReadFile(file, &this->m_field33512213041, sizeof(this->m_field33512213041), nullptr, nullptr, 0);
        int32_t uiSkinRead = SFileReadFile(file, &this->m_uiSkin, sizeof(this->m_uiSkin), nullptr, nullptr, 0);
        int32_t field33512213043Read = SFileReadFile(file, &this->m_field33512213043, sizeof(this->m_field33512213043), nullptr, nullptr, 0);
        int32_t field33512213044Read = SFileReadFile(file, &this->m_field33512213044, sizeof(this->m_field33512213044), nullptr, nullptr, 0);
        int32_t field33512213045Read = SFileReadFile(file, &this->m_field33512213045, sizeof(this->m_field33512213045), nullptr, nullptr, 0);
        int32_t field33512213046Read = SFileReadFile(file, &this->m_field33512213046, sizeof(this->m_field33512213046), nullptr, nullptr, 0);
        int32_t field33512213047Read = SFileReadFile(file, &this->m_field33512213047, sizeof(this->m_field33512213047), nullptr, nullptr, 0);
        int32_t field33512213048Read = SFileReadFile(file, &this->m_field33512213048, sizeof(this->m_field33512213048), nullptr, nullptr, 0);
        int32_t field33512213049Read = SFileReadFile(file, &this->m_field33512213049, sizeof(this->m_field33512213049), nullptr, nullptr, 0);
        int32_t field33512213050Read = SFileReadFile(file, &this->m_field33512213050, sizeof(this->m_field33512213050), nullptr, nullptr, 0);
        int32_t field33512213051Read = SFileReadFile(file, &this->m_field33512213051, sizeof(this->m_field33512213051), nullptr, nullptr, 0);
        int32_t field33512213052Read = SFileReadFile(file, &this->m_field33512213052, sizeof(this->m_field33512213052), nullptr, nullptr, 0);
        int32_t field33512213053Read = SFileReadFile(file, &this->m_field33512213053, sizeof(this->m_field33512213053), nullptr, nullptr, 0);
        int32_t field33512213054Read = SFileReadFile(file, &this->m_field33512213054, sizeof(this->m_field33512213054), nullptr, nullptr, 0);
        int32_t field33512213055Read = SFileReadFile(file, &this->m_field33512213055, sizeof(this->m_field33512213055), nullptr, nullptr, 0);

        if (!idRead
            | !field33512213001Read
            | !attachmentIDRead
            | !attachmentRead0
            | !attachmentRead1
            | !attachmentRead2
            | !field33512213004Read
            | !enterSpeedRead
            | !enterGravityRead
            | !enterMinDurationRead
            | !enterMaxDurationRead
            | !enterMinArcHeightRead
            | !enterMaxArcHeightRead
            | !enterAnimStartRead
            | !enterAnimLoopRead
            | !rideAnimStartRead
            | !rideAnimLoopRead
            | !rideUpperAnimStartRead
            | !rideUpperAnimLoopRead
            | !field33512213017Read
            | !exitSpeedRead
            | !exitGravityRead
            | !exitMinDurationRead
            | !exitMaxDurationRead
            | !exitMinArcHeightRead
            | !exitMaxArcHeightRead
            | !exitAnimStartRead
            | !exitAnimLoopRead
            | !exitAnimEndRead
            | !field33512213027Read
            | !passengerPitchRead
            | !field33512213029Read
            | !passengerAttachmentIDRead
            | !vehicleEnterAnimRead
            | !vehicleExitAnimRead
            | !vehicleRideAnimLoopRead
            | !field33512213034Read
            | !vehicleExitAnimBoneRead
            | !vehicleEnterAnimBoneRead
            | !field33512213037Read
            | !field33512213038Read
            | !vehicleAbilityDisplayRead
            | !enterUisoundIDRead
            | !field33512213041Read
            | !uiSkinRead
            | !field33512213043Read
            | !field33512213044Read
            | !field33512213045Read
            | !field33512213046Read
            | !field33512213047Read
            | !field33512213048Read
            | !field33512213049Read
            | !field33512213050Read
            | !field33512213051Read
            | !field33512213052Read
            | !field33512213053Read
            | !field33512213054Read
            | !field33512213055Read) {
            return false;
        }

        return true;
    }
};

#endif
