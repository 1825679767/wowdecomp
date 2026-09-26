#include "WoW/Source/DB/Rec/GMSurveySurveysRec.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

GMSurveySurveysRec::GMSurveySurveysRec() {
}

GMSurveySurveysRec::~GMSurveySurveysRec() {
}

// VERIFY: 9720fafd auto
// OFFSET: 0x008AFE40
const char* GMSurveySurveysRec::GetFilename() {
    return "DBFilesClient\\GMSurveySurveys.dbc";
}

// VERIFY: 0111dfdf auto
// OFFSET: 0x008AFE50
bool GMSurveySurveysRec::Read(SFileImpl* file, const char* stringBuffer) {
    int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
    int32_t unk_0004Read = SFileReadFile(file, this->unk_0004, sizeof(this->unk_0004), nullptr, nullptr, 0);

    if (!idRead
        | !unk_0004Read) {
        return false;
    }

    return true;
}
