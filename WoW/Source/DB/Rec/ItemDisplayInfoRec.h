#ifndef WOW_SOURCE_DB_REC_ITEMDISPLAYINFOREC_H
#define WOW_SOURCE_DB_REC_ITEMDISPLAYINFOREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct ItemDisplayInfoRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_modelName [2];
    /* 000C */ const char* m_modelTexture [2];
    /* 0014 */ const char* m_inventoryIcon [2];
    /* 001C */ int32_t m_geosetGroup [3];
    /* 0028 */ int32_t m_flags;
    /* 002C */ int32_t m_spellVisualID;
    /* 0030 */ int32_t m_groupSoundIndex;
    /* 0034 */ int32_t m_helmetGeosetVisID [2];
    /* 003C */ const char* m_texture [8];
    /* 005C */ int32_t m_itemVisual;
    /* 0060 */ int32_t m_particleColorID;

// VERIFY: 7222f6cf auto
    // OFFSET: 0x008A7FC0
    static const char* GetFilename() {
        return "DBFilesClient\\ItemDisplayInfo.dbc";
    }

// VERIFY: a00caf07 manual
    // OFFSET: 0x008A7FD0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t modelNameOffset[2];
        uint32_t modelTextureOffset[2];
        uint32_t inventoryIconOffset[2];
        uint32_t textureOffset[8];

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t modelNameRead0 = SFileReadFile(file, &modelNameOffset[0], sizeof(modelNameOffset[0]), nullptr, nullptr, 0);
        int32_t modelNameRead1 = SFileReadFile(file, &modelNameOffset[1], sizeof(modelNameOffset[1]), nullptr, nullptr, 0);
        int32_t modelTextureRead0 = SFileReadFile(file, &modelTextureOffset[0], sizeof(modelTextureOffset[0]), nullptr, nullptr, 0);
        int32_t modelTextureRead1 = SFileReadFile(file, &modelTextureOffset[1], sizeof(modelTextureOffset[1]), nullptr, nullptr, 0);
        int32_t inventoryIconRead0 = SFileReadFile(file, &inventoryIconOffset[0], sizeof(inventoryIconOffset[0]), nullptr, nullptr, 0);
        int32_t inventoryIconRead1 = SFileReadFile(file, &inventoryIconOffset[1], sizeof(inventoryIconOffset[1]), nullptr, nullptr, 0);
        int32_t geosetGroupRead = SFileReadFile(file, this->m_geosetGroup, sizeof(this->m_geosetGroup), nullptr, nullptr, 0);
        int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);
        int32_t spellVisualIDRead = SFileReadFile(file, &this->m_spellVisualID, sizeof(this->m_spellVisualID), nullptr, nullptr, 0);
        int32_t groupSoundIndexRead = SFileReadFile(file, &this->m_groupSoundIndex, sizeof(this->m_groupSoundIndex), nullptr, nullptr, 0);
        int32_t helmetGeosetVisIDRead = SFileReadFile(file, this->m_helmetGeosetVisID, sizeof(this->m_helmetGeosetVisID), nullptr, nullptr, 0);
        int32_t textureRead0 = SFileReadFile(file, &textureOffset[0], sizeof(textureOffset[0]), nullptr, nullptr, 0);
        int32_t textureRead1 = SFileReadFile(file, &textureOffset[1], sizeof(textureOffset[1]), nullptr, nullptr, 0);
        int32_t textureRead2 = SFileReadFile(file, &textureOffset[2], sizeof(textureOffset[2]), nullptr, nullptr, 0);
        int32_t textureRead3 = SFileReadFile(file, &textureOffset[3], sizeof(textureOffset[3]), nullptr, nullptr, 0);
        int32_t textureRead4 = SFileReadFile(file, &textureOffset[4], sizeof(textureOffset[4]), nullptr, nullptr, 0);
        int32_t textureRead5 = SFileReadFile(file, &textureOffset[5], sizeof(textureOffset[5]), nullptr, nullptr, 0);
        int32_t textureRead6 = SFileReadFile(file, &textureOffset[6], sizeof(textureOffset[6]), nullptr, nullptr, 0);
        int32_t textureRead7 = SFileReadFile(file, &textureOffset[7], sizeof(textureOffset[7]), nullptr, nullptr, 0);
        int32_t itemVisualRead = SFileReadFile(file, &this->m_itemVisual, sizeof(this->m_itemVisual), nullptr, nullptr, 0);
        int32_t particleColorIDRead = SFileReadFile(file, &this->m_particleColorID, sizeof(this->m_particleColorID), nullptr, nullptr, 0);

        if (!idRead
            | !modelNameRead0
            | !modelNameRead1
            | !modelTextureRead0
            | !modelTextureRead1
            | !inventoryIconRead0
            | !inventoryIconRead1
            | !geosetGroupRead
            | !flagsRead
            | !spellVisualIDRead
            | !groupSoundIndexRead
            | !helmetGeosetVisIDRead
            | !textureRead0
            | !textureRead1
            | !textureRead2
            | !textureRead3
            | !textureRead4
            | !textureRead5
            | !textureRead6
            | !textureRead7
            | !itemVisualRead
            | !particleColorIDRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_modelName[0] = &stringBuffer[modelNameOffset[0]];
            this->m_modelName[1] = &stringBuffer[modelNameOffset[1]];
            this->m_modelTexture[0] = &stringBuffer[modelTextureOffset[0]];
            this->m_modelTexture[1] = &stringBuffer[modelTextureOffset[1]];
            this->m_inventoryIcon[0] = &stringBuffer[inventoryIconOffset[0]];
            this->m_inventoryIcon[1] = &stringBuffer[inventoryIconOffset[1]];
            this->m_texture[0] = &stringBuffer[textureOffset[0]];
            this->m_texture[1] = &stringBuffer[textureOffset[1]];
            this->m_texture[2] = &stringBuffer[textureOffset[2]];
            this->m_texture[3] = &stringBuffer[textureOffset[3]];
            this->m_texture[4] = &stringBuffer[textureOffset[4]];
            this->m_texture[5] = &stringBuffer[textureOffset[5]];
            this->m_texture[6] = &stringBuffer[textureOffset[6]];
            this->m_texture[7] = &stringBuffer[textureOffset[7]];
        } else {
            this->m_modelName[0] = "";
            this->m_modelName[1] = "";
            this->m_modelTexture[0] = "";
            this->m_modelTexture[1] = "";
            this->m_inventoryIcon[0] = "";
            this->m_inventoryIcon[1] = "";
            this->m_texture[0] = "";
            this->m_texture[1] = "";
            this->m_texture[2] = "";
            this->m_texture[3] = "";
            this->m_texture[4] = "";
            this->m_texture[5] = "";
            this->m_texture[6] = "";
            this->m_texture[7] = "";
        }

        return true;
    }
};

#endif
