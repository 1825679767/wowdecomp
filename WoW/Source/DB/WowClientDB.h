#ifndef WOW_SOURCE_DB_WOWCLIENTDB_H
#define WOW_SOURCE_DB_WOWCLIENTDB_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Storm/H/storm.h"

extern int32_t g_clientDbPerformCompress;

uint8_t* DecompressRow(const uint8_t* src, uint32_t size, uint8_t* dst);
uint32_t CompressRow(const uint8_t* src, uint32_t size, uint8_t* dst);

class WowClientDB_Base {
    public:
    /* 0004 */ int32_t m_loaded;
    /* 0008 */ int32_t m_numRecords;
    /* 000C */ int32_t m_maxID;
    /* 0010 */ int32_t m_minID;
    /* 0014 */ const char* m_strings;

    virtual ~WowClientDB_Base();
    virtual void Load(const char* file, int32_t line) = 0;
    virtual void Unk08(int32_t a1) = 0;

    WowClientDB_Base();
};

template <class T>
class IDatabase {
    public:
    virtual ~IDatabase();
    virtual T* GetRecord(int32_t id) const = 0;
};

template <class T>
class WowClientDB_Common : public WowClientDB_Base {
    public:
    void Load(const char* file, int32_t line) override;
    void Unk08(int32_t a1) override;
    virtual void LoadRecords(SFileImpl* f, const char* file, int32_t line) = 0;
    virtual int32_t GetRecordByIndex(int32_t index, void* rec) const = 0;
    virtual void Free() = 0;
    virtual void Reset() = 0;
    virtual void* Unk1C() = 0;
};

template <class T>
class WowClientDB : public WowClientDB_Common<T>, public IDatabase<T> {
    public:
    /* 001C */ T* m_records;
    /* 0020 */ T** m_recordsById;

    ~WowClientDB() override;
    void LoadRecords(SFileImpl* f, const char* file, int32_t line) override;
    int32_t GetRecordByIndex(int32_t index, void* rec) const override;
    void Free() override;
    void Reset() override;
    void* Unk1C() override;
    T* GetRecord(int32_t id) const override;

    WowClientDB();
    T* GetRecordByIndex(int32_t index) const;
};

template <class T>
class WowClientDB_RLE : public WowClientDB_Common<T> {
    public:
    /* 0018 */ T* m_data;
    /* 001C */ T** m_records;
    /* 0020 */ T** m_recordsById;

    ~WowClientDB_RLE() override;
    void LoadRecords(SFileImpl* f, const char* file, int32_t line) override;
    int32_t GetRecordByIndex(int32_t index, void* rec) const override;
    void Free() override;
    void Reset() override;
    void* Unk1C() override;

    WowClientDB_RLE();
    int32_t GetRecordByIndex(int32_t index, T* rec) const;
    int32_t GetRecord(int32_t id, T* rec) const;
};

// VERIFY: 936b7da3 auto
// OFFSET: 0x00635DD0
template <class T>
IDatabase<T>::~IDatabase() {
}

// VERIFY: 8ccd798a auto
// OFFSET: [0x004052F0, 0x00636E50, 0x006370B0, 0x00637300, 0x00637550, 0x006377A0, 0x006379F0, 0x00637C50, 0x00637EB0, 0x00638100, 0x00638350, 0x006385A0, 0x006387F0, 0x00638A40, 0x00638C90, 0x00638EF0, 0x00639150, 0x006393A0, 0x006395F0, 0x00639840, 0x00639A90, 0x00639CE0, 0x00639F40, 0x0063A1A0, 0x0063A3F0, 0x0063A650, 0x0063A8A0, 0x0063AB00, 0x0063AD60, 0x0063AFB0, 0x0063B200, 0x0063B450, 0x0063B6A0, 0x0063B8F0, 0x0063BB40, 0x0063BD90, 0x0063BFF0, 0x0063C240, 0x0063C490, 0x0063C6E0, 0x0063C930, 0x0063CB80, 0x0063CDD0, 0x0063D020, 0x0063D270, 0x0063D4C0, 0x0063D710, 0x0063D960, 0x0063DBB0, 0x0063DE00, 0x0063E050, 0x0063E2A0, 0x0063E4F0, 0x0063E740, 0x0063E990, 0x0063EBE0, 0x0063EE30, 0x0063F080, 0x0063F2E0, 0x0063F530, 0x0063F780, 0x0063F9D0, 0x0063FC20, 0x0063FE70, 0x006400C0, 0x00640310, 0x00640560, 0x006407B0, 0x00640A00, 0x00640C50, 0x00640EA0, 0x006410F0, 0x00641340, 0x00641590, 0x006417E0, 0x00641A30, 0x00641C80, 0x00641ED0, 0x00642120, 0x00642370, 0x006425C0, 0x00642810, 0x00642A60, 0x00642CB0, 0x00642F00, 0x00643150, 0x006433A0, 0x006435F0, 0x00643840, 0x00643A90, 0x00643CE0, 0x00643F30, 0x00644190, 0x006443E0, 0x00644630, 0x00644880, 0x00644AD0, 0x00644D20, 0x00644F70, 0x006451C0, 0x00645410, 0x00645660, 0x006458B0, 0x00645B00, 0x00645D50, 0x00645FB0, 0x00646200, 0x00646460, 0x006466B0, 0x00646900, 0x00646B50, 0x00646DA0, 0x00646FF0, 0x00647240, 0x006474A0, 0x00647700, 0x00647950, 0x00647BA0, 0x00647DF0, 0x00648050, 0x006482B0, 0x00648510, 0x00648770, 0x006489C0, 0x00648C10, 0x00648E60, 0x006490B0, 0x00649300, 0x00649550, 0x006497A0, 0x006499F0, 0x00649C40, 0x00649E90, 0x0064A0E0, 0x0064A330, 0x0064A580, 0x0064A7D0, 0x0064AA20, 0x0064AC70, 0x0064AEC0, 0x0064B110, 0x0064B360, 0x0064B5B0, 0x0064B800, 0x0064BA50, 0x0064BCA0, 0x0064BEF0, 0x0064C140, 0x0064C390, 0x0064C5E0, 0x0064C830, 0x0064CA90, 0x0064CCE0, 0x0064CF30, 0x0064D190, 0x0064D3E0, 0x0064D630, 0x0064D880, 0x0064DAD0, 0x0064DD20, 0x0064DF70, 0x0064E1C0, 0x0064E410, 0x0064E660, 0x0064E8C0, 0x0064EB10, 0x0064ED60, 0x0064EFB0, 0x0064F200, 0x0064F450, 0x0064F6A0, 0x0064F8F0, 0x0064FB50, 0x0064FDA0, 0x0064FFF0, 0x00650240, 0x00650490, 0x006506E0, 0x00650940, 0x00650BA0, 0x00650DF0, 0x00651050, 0x006512A0, 0x00651500, 0x00651750, 0x006519A0, 0x00651C00, 0x00651E50, 0x006520A0, 0x006522F0, 0x00652540, 0x00652790, 0x006529E0, 0x00652C30, 0x00652E80, 0x006530D0, 0x00653320, 0x00653570, 0x006537C0, 0x00653A20, 0x00653C70, 0x00653EC0, 0x00654110, 0x00654360, 0x006545B0, 0x00654800, 0x00654A60, 0x00654CC0, 0x00654F10, 0x00655160, 0x006553B0, 0x00655600, 0x00655850, 0x00655AA0, 0x00655CF0, 0x00655F40, 0x00656190, 0x006563E0, 0x00656630, 0x00656890, 0x00656AE0, 0x00656D30, 0x00656F80, 0x006571D0, 0x00657420, 0x00657670, 0x006578C0, 0x00657B10, 0x00657D60, 0x00657FB0, 0x00658200, 0x00658450, 0x006586A0, 0x006588F0, 0x00658B40, 0x0076B7E0, 0x007EB2F0, 0x007EB540, 0x007EB7A0, 0x007EBA00, 0x007EBC50]
template <class T>
void WowClientDB_Common<T>::Load(const char* file, int32_t line) {
    if (this->m_loaded) {
        DebugLog("%s already loaded! Aborting to prevent memory leak!", T::GetFilename());
        return;
    }

    SFileImpl* f;

    if (!SFile::OpenEx(nullptr, T::GetFilename(), 0x20000, &f)) {
        SErrDisplayAppFatalCustom(0x85100079, "Unable to open %s", T::GetFilename());
    }

    uint32_t signature;

    if (!SFileReadFile(f, &signature, sizeof(signature), nullptr, nullptr, 0)) {
        SErrDisplayAppFatalCustom(0x85100079, "Unable to read signature from %s", T::GetFilename());
    }

    if (signature != 'CBDW') {
        SErrDisplayAppFatalCustom(0x85100079, "Invalid signature 0x%x from %s", signature, T::GetFilename());
    }

    if (!SFileReadFile(f, &this->m_numRecords, sizeof(this->m_numRecords), nullptr, nullptr, 0)) {
        SErrDisplayAppFatalCustom(0x85100079, "Unable to read record count from %s", T::GetFilename());
    }

    if (!this->m_numRecords) {
        SFileCloseFile(f);
        return;
    }

    int32_t numColumns;

    if (!SFileReadFile(f, &numColumns, sizeof(numColumns), nullptr, nullptr, 0)) {
        SErrDisplayAppFatalCustom(0x85100079, "Unable to read column count from %s", T::GetFilename());
    }

    if (numColumns != T::s_numColumns) {
        SErrDisplayAppFatalCustom(0x85100079, "%s has wrong number of columns (found %i, expected %i)", T::GetFilename(), numColumns, T::s_numColumns);
    }

    int32_t rowSize;

    if (!SFileReadFile(f, &rowSize, sizeof(rowSize), nullptr, nullptr, 0)) {
        SErrDisplayAppFatalCustom(0x85100079, "Unable to read row size from %s", T::GetFilename());
    }

    if (rowSize != T::s_rowSize) {
        SErrDisplayAppFatalCustom(0x85100079, "%s has wrong row size (found %i, expected %i)", T::GetFilename(), rowSize, T::s_rowSize);
    }

    uint32_t stringSize;

    if (!SFileReadFile(f, &stringSize, sizeof(stringSize), nullptr, nullptr, 0)) {
        SErrDisplayAppFatalCustom(0x85100079, "Unable to read string size from %s", T::GetFilename());
    }

    char* strings = static_cast<char*>(SMemAlloc(stringSize, file, line, 0));
    this->m_strings = strings;
    this->m_maxID = 0;
    this->m_minID = 0xFFFFFFF;

    this->LoadRecords(f, file, line);

    if (!SFileReadFile(f, strings, stringSize, nullptr, nullptr, 0)) {
        SErrDisplayAppFatal("%s: Cannot read string table", T::GetFilename());
    }

    SFileCloseFile(f);
    this->m_loaded = 1;
}

template <class T>
void WowClientDB_Common<T>::Unk08(int32_t a1) {
}

// INLINED
template <class T>
WowClientDB<T>::WowClientDB() {
    this->m_records = nullptr;
    this->m_recordsById = nullptr;
}

// VERIFY: 5252a44e manual
// OFFSET: [0x004059C0, 0x006635F0, 0x00663640, 0x00663690, 0x006636E0, 0x00663730, 0x00663780, 0x006637D0, 0x00663820, 0x00663870, 0x006638C0, 0x00663910, 0x00663960, 0x006639B0, 0x00663A00, 0x00663A50, 0x00663AA0, 0x00663AF0, 0x00663B40, 0x00663B90, 0x00663BE0, 0x00663C30, 0x00663C80, 0x00663CD0, 0x00663D20, 0x00663D70, 0x00663DC0, 0x00663E10, 0x00663E60, 0x00663EB0, 0x00663F00, 0x00663F50, 0x00663FA0, 0x00663FF0, 0x00664040, 0x00664090, 0x006640E0, 0x00664130, 0x00664180, 0x006641D0, 0x00664220, 0x00664270, 0x006642C0, 0x00664310, 0x00664360, 0x006643B0, 0x00664400, 0x00664450, 0x006644A0, 0x006644F0, 0x00664540, 0x00664590, 0x006645E0, 0x00664630, 0x00664680, 0x006646D0, 0x00664720, 0x00664770, 0x006647C0, 0x00664810, 0x00664860, 0x006648B0, 0x00664900, 0x00664950, 0x006649A0, 0x006649F0, 0x00664A40, 0x00664A90, 0x00664AE0, 0x00664B30, 0x00664B80, 0x00664BD0, 0x00664C20, 0x00664C70, 0x00664CC0, 0x00664D10, 0x00664D60, 0x00664DB0, 0x00664E00, 0x00664E50, 0x00664EA0, 0x00664EF0, 0x00664F40, 0x00664F90, 0x00664FE0, 0x00665030, 0x00665080, 0x006650D0, 0x00665120, 0x00665170, 0x006651C0, 0x00665210, 0x00665260, 0x006652B0, 0x00665300, 0x00665350, 0x006653E0, 0x00665430, 0x00665480, 0x006654D0, 0x00665520, 0x00665570, 0x006655C0, 0x00665610, 0x00665660, 0x006656B0, 0x00665700, 0x00665750, 0x006657A0, 0x006657F0, 0x00665840, 0x00665890, 0x006658E0, 0x00665930, 0x00665980, 0x006659D0, 0x00665A20, 0x00665A70, 0x00665AC0, 0x00665B10, 0x00665B60, 0x00665BB0, 0x00665C00, 0x00665C50, 0x00665CA0, 0x00665CF0, 0x00665D40, 0x00665D90, 0x00665DE0, 0x00665E30, 0x00665E80, 0x00665ED0, 0x00665F20, 0x00665F70, 0x00665FC0, 0x00666010, 0x00666060, 0x006660B0, 0x00666100, 0x00666150, 0x006661A0, 0x006661F0, 0x00666240, 0x00666290, 0x006662E0, 0x00666330, 0x00666380, 0x006663D0, 0x00666420, 0x00666470, 0x006664C0, 0x00666510, 0x00666560, 0x006665B0, 0x00666600, 0x00666650, 0x006666A0, 0x006666F0, 0x00666740, 0x00666790, 0x006667E0, 0x00666830, 0x00666880, 0x006668D0, 0x00666920, 0x00666970, 0x006669C0, 0x00666A10, 0x00666A60, 0x00666AB0, 0x00666B00, 0x00666B50, 0x00666BA0, 0x00666BF0, 0x00666C40, 0x00666C90, 0x00666CE0, 0x00666D70, 0x00666DC0, 0x00666E10, 0x00666E60, 0x00666EB0, 0x00666F00, 0x00666F50, 0x00666FA0, 0x00666FF0, 0x00667040, 0x00667090, 0x006670E0, 0x00667130, 0x00667180, 0x006671D0, 0x00667220, 0x00667270, 0x006672C0, 0x00667310, 0x00667360, 0x006673B0, 0x00667400, 0x00667450, 0x006674A0, 0x006674F0, 0x00667540, 0x00667590, 0x006675E0, 0x00667630, 0x00667680, 0x006676D0, 0x00667720, 0x00667770, 0x006677C0, 0x00667810, 0x00667860, 0x006678B0, 0x00667900, 0x00667950, 0x006679A0, 0x006679F0, 0x00667A40, 0x00667A90, 0x00667AE0, 0x00667B30, 0x00667B80, 0x00667BD0, 0x00667C20, 0x00667C70, 0x00667CC0, 0x00667D10, 0x00667D60, 0x00667DB0, 0x00667E00, 0x00667E50, 0x00667EA0, 0x0076BD00, 0x007EC9C0, 0x007ECA90, 0x007ECAE0]
template <class T>
WowClientDB<T>::~WowClientDB() {
    if (this->m_loaded) {
        this->Free();
    }
}

// VERIFY: a0cd6555 manual
// OFFSET: [0x00405860, 0x00658D90, 0x00658EA0, 0x00658FB0, 0x006590C0, 0x006591D0, 0x006592E0, 0x006593F0, 0x00659590, 0x006596A0, 0x006597B0, 0x00659910, 0x00659A70, 0x00659B80, 0x00659CA0, 0x00659DB0, 0x00659F00, 0x0065A010, 0x0065A120, 0x0065A240, 0x0065A390, 0x0065A4A0, 0x0065A5B0, 0x0065A6C0, 0x0065A810, 0x0065A920, 0x0065AA30, 0x0065AB90, 0x0065ACE0, 0x0065AE30, 0x0065AF50, 0x0065B060, 0x0065B190, 0x0065B2A0, 0x0065B3B0, 0x0065B4E0, 0x0065B670, 0x0065B780, 0x0065B8A0, 0x0065B9B0, 0x0065BB00, 0x0065BC10, 0x0065BD40, 0x0065BE50, 0x0065BFA0, 0x0065C180, 0x0065C330, 0x0065C4E0, 0x0065C610, 0x0065C760, 0x0065C880, 0x0065CBE0, 0x0065CD20, 0x0065CE70, 0x0065CF80, 0x0065D0D0, 0x0065D220, 0x0065D330, 0x0065D450, 0x0065D560, 0x0065D670, 0x0065D790, 0x0065D8A0, 0x0065D9B0, 0x0065DAD0, 0x0065DBE0, 0x0065DCF0, 0x0065DE00, 0x0065DF10, 0x0065E020, 0x0065E130, 0x0065E240, 0x0065E3C0, 0x0065E4E0, 0x0065E5F0, 0x0065E710, 0x0065E820, 0x0065E930, 0x0065EA40, 0x0065EB90, 0x0065ECA0, 0x0065EDB0, 0x0065EEC0, 0x0065EFD0, 0x0065F0E0, 0x0065F200, 0x0065F310, 0x0065F420, 0x0065F570, 0x0065F6E0, 0x0065F830, 0x0065F940, 0x0065FA50, 0x0065FB60, 0x0065FC70, 0x0065FD90, 0x0065FEC0, 0x00660020, 0x00660130, 0x006602C0, 0x00660410, 0x00660570, 0x00660680, 0x006607A0, 0x00660AE0, 0x00660BF0, 0x00660D00, 0x00660E30, 0x00660F80, 0x00661090, 0x006611C0, 0x00661310, 0x00661420, 0x00661560, 0x00661680, 0x006617C0, 0x006618E0, 0x00661A10, 0x00661B20, 0x00661C30, 0x00661D80, 0x00661EF0, 0x00662040, 0x00662150, 0x00662260, 0x00662370, 0x00662490, 0x006625A0, 0x006626F0, 0x00662800, 0x00662960, 0x00662AD0, 0x00662BE0, 0x00662CF0, 0x00662E00, 0x00662F10, 0x00663020, 0x00663170, 0x00663290, 0x006633D0, 0x006634E0, 0x0076BBF0, 0x007EC7A0, 0x007EC8B0]
template <class T>
void WowClientDB<T>::LoadRecords(SFileImpl* f, const char* file, int32_t line) {
    this->m_records = static_cast<T*>(SMemAlloc(sizeof(T) * this->m_numRecords, file, line, 0));

    for (int32_t i = 0; i < this->m_numRecords; i++) {
        new (&this->m_records[i]) T();
        this->m_records[i].Read(f, this->m_strings);

        int32_t id;

        if constexpr (T::s_generatedID) {
            this->m_records[i].m_generatedID = i;
            id = this->m_records[i].m_generatedID;
        } else {
            id = this->m_records[i].m_id;
        }

        this->m_maxID = id > this->m_maxID ? id : this->m_maxID;
        this->m_minID = id < this->m_minID ? id : this->m_minID;
    }

    this->m_recordsById = new (__FILE__, __LINE__) T*[this->m_maxID - this->m_minID + 1];
    memset(this->m_recordsById, 0, sizeof(T*) * (this->m_maxID - this->m_minID + 1));

    for (int32_t i = 0; i < this->m_numRecords; i++) {
        if constexpr (T::s_generatedID) {
            this->m_recordsById[this->m_records[i].m_generatedID - this->m_minID] = &this->m_records[i];
        } else {
            this->m_recordsById[this->m_records[i].m_id - this->m_minID] = &this->m_records[i];
        }
    }
}

// VERIFY: 2a32e487 auto
// OFFSET: [0x006594F0, 0x006598B0, 0x00659A10, 0x00659EB0, 0x0065A340, 0x0065A7C0, 0x0065AB30, 0x0065AC90, 0x0065ADE0, 0x0065B5E0, 0x0065BAB0, 0x0065BF50, 0x0065C0D0, 0x0065C2C0, 0x0065C710, 0x0065CE20, 0x0065D080, 0x0065D1D0, 0x0065E370, 0x0065F520, 0x0065F690, 0x0065F7E0, 0x0065FFC0, 0x00660260, 0x006603C0, 0x00660510, 0x00660F30, 0x006612C0, 0x00661780, 0x00661D30, 0x00661E80, 0x00661FF0, 0x006626A0, 0x00662900, 0x00662A60, 0x00663120]
template <class T>
int32_t WowClientDB<T>::GetRecordByIndex(int32_t index, void* rec) const {
    if (index < 0 || index >= this->m_numRecords) {
        return 0;
    }

    memcpy(rec, &this->m_records[index], sizeof(T));

    return 1;
}

// VERIFY: d4dec6f6 auto
// OFFSET: [0x00405280, 0x00635E00, 0x00635E70, 0x00635EE0, 0x00635F60, 0x00635FD0, 0x00636040, 0x006360B0, 0x00636120, 0x00636190, 0x00636200, 0x00636270, 0x006362F0, 0x00636360, 0x00636410, 0x00636490, 0x00636500, 0x00636570, 0x006365E0, 0x00636650, 0x006366C0, 0x00636740, 0x006367B0, 0x00636830, 0x006368A0, 0x00636910, 0x00636990, 0x00636A10, 0x00636A80, 0x00636B00, 0x00636B80, 0x00636C20, 0x00636C90, 0x00636D00, 0x00636D70, 0x00636DE0, 0x007EB280]
template <class T>
void WowClientDB<T>::Free() {
    for (int32_t i = 0; i < this->m_numRecords; i++) {
        this->m_records[i].~T();
    }

    SMemFree(this->m_records, __FILE__, __LINE__, 0);
    this->m_records = nullptr;

    if (this->m_recordsById) {
        delete[] this->m_recordsById;
    }

    this->m_recordsById = nullptr;

    if (this->m_strings) {
        SMemFree(const_cast<char*>(this->m_strings), nullptr, 0, 0);
        this->m_strings = nullptr;
    }
}

// VERIFY: 8b24b7bc auto
// OFFSET: 0x00636C00
template <class T>
void WowClientDB<T>::Reset() {
    this->m_records = nullptr;
    this->m_recordsById = nullptr;
    this->m_strings = nullptr;
    this->m_loaded = 0;
    this->m_numRecords = 0;
    this->m_maxID = -1;
    this->m_minID = 0xFFFFFFF;
}

// VERIFY: 17d0c4f6 auto
// OFFSET: 0x00662470
template <class T>
void* WowClientDB<T>::Unk1C() {
    return &this->m_recordsById;
}

// VERIFY: 8c53256a auto
// OFFSET: 0x0065C290
template <class T>
T* WowClientDB<T>::GetRecord(int32_t id) const {
    if (id < this->m_minID || id > this->m_maxID) {
        return nullptr;
    }

    return this->m_recordsById[id - this->m_minID];
}

// INLINED
template <class T>
WowClientDB_RLE<T>::WowClientDB_RLE() {
    this->m_data = nullptr;
    this->m_records = nullptr;
    this->m_recordsById = nullptr;
}

// VERIFY: 7d620386 manual
// OFFSET: [0x006653A0, 0x00666D30, 0x007ECA10, 0x007ECA50]
template <class T>
WowClientDB_RLE<T>::~WowClientDB_RLE() {
    if (this->m_loaded) {
        this->Free();
    }
}

// VERIFY: ac967174 manual
// OFFSET: [0x0065C9A0, 0x006608B0, 0x007EC520]
template <class T>
void WowClientDB_RLE<T>::LoadRecords(SFileImpl* f, const char* file, int32_t line) {
    T rec;
    uint32_t compressedSize = 0;

    if (!g_clientDbPerformCompress) {
        this->m_data = static_cast<T*>(SMemAlloc(sizeof(T) * this->m_numRecords, file, line, 0));
    }

    for (int32_t i = 0; i < this->m_numRecords; i++) {
        if (g_clientDbPerformCompress) {
            rec.Read(f, this->m_strings);
            compressedSize += CompressRow(reinterpret_cast<const uint8_t*>(&rec), sizeof(T), nullptr);

            if (rec.m_id > this->m_maxID) {
                this->m_maxID = rec.m_id;
            }

            if (rec.m_id < this->m_minID) {
                this->m_minID = rec.m_id;
            }
        } else {
            this->m_data[i].Read(f, this->m_strings);

            if (this->m_data[i].m_id > this->m_maxID) {
                this->m_maxID = this->m_data[i].m_id;
            }

            if (this->m_data[i].m_id < this->m_minID) {
                this->m_minID = this->m_data[i].m_id;
            }
        }
    }

    T** index = static_cast<T**>(SMemAlloc(sizeof(T*) * this->m_numRecords + sizeof(T*) * (this->m_maxID - this->m_minID) + sizeof(T*), file, line, 0));
    this->m_records = index;
    this->m_recordsById = &index[this->m_numRecords];
    memset(this->m_recordsById, 0, sizeof(T*) * (this->m_maxID - this->m_minID) + sizeof(T*));

    if (g_clientDbPerformCompress) {
        this->m_data = static_cast<T*>(SMemAlloc(compressedSize, file, line, 0));
        SFile::SetFilePointer(f, 20, nullptr, 0);

        uint8_t* dst = reinterpret_cast<uint8_t*>(this->m_data);

        for (int32_t i = 0; i < this->m_numRecords; i++) {
            rec.Read(f, this->m_strings);
            this->m_records[i] = reinterpret_cast<T*>(dst);
            this->m_recordsById[rec.m_id - this->m_minID] = reinterpret_cast<T*>(dst);
            dst += CompressRow(reinterpret_cast<const uint8_t*>(&rec), sizeof(T), dst);
        }
    } else {
        for (int32_t i = 0; i < this->m_numRecords; i++) {
            this->m_records[i] = &this->m_data[i];
            this->m_recordsById[this->m_data[i].m_id - this->m_minID] = &this->m_data[i];
        }
    }
}

// VERIFY: 8403c210 auto
// OFFSET: [0x0065CBD0, 0x00660AC0, 0x007EC790]
template <class T>
int32_t WowClientDB_RLE<T>::GetRecordByIndex(int32_t index, void* rec) const {
    return this->GetRecordByIndex(index, static_cast<T*>(rec));
}

// VERIFY: 300e9a19 auto
// OFFSET: 0x007EB1B0
template <class T>
void WowClientDB_RLE<T>::Free() {
    if (this->m_records) {
        SMemFree(this->m_records, __FILE__, __LINE__, 0);
        this->m_records = nullptr;
    }

    if (this->m_data) {
        SMemFree(this->m_data, __FILE__, __LINE__, 0);
        this->m_data = nullptr;
    }

    if (this->m_strings) {
        SMemFree(const_cast<char*>(this->m_strings), nullptr, 0, 0);
        this->m_strings = nullptr;
    }
}

// VERIFY: 822cdb97 auto
// OFFSET: 0x006363E0
template <class T>
void WowClientDB_RLE<T>::Reset() {
    this->m_data = nullptr;
    this->m_records = nullptr;
    this->m_recordsById = nullptr;
    this->m_strings = nullptr;
    this->m_loaded = 0;
    this->m_numRecords = 0;
    this->m_maxID = -1;
    this->m_minID = 0xFFFFFFF;
}

template <class T>
void* WowClientDB_RLE<T>::Unk1C() {
    return &this->m_recordsById;
}

// VERIFY: 944c2958 auto
// OFFSET: [0x0061DC30, 0x0065CB70, 0x007EC730]
template <class T>
int32_t WowClientDB_RLE<T>::GetRecordByIndex(int32_t index, T* rec) const {
    if (index < 0 || index >= this->m_numRecords) {
        return 0;
    }

    if (g_clientDbPerformCompress) {
        DecompressRow(reinterpret_cast<const uint8_t*>(this->m_records[index]), sizeof(T), reinterpret_cast<uint8_t*>(rec));
    } else {
        memcpy(rec, this->m_records[index], sizeof(T));
    }

    return 1;
}

// VERIFY: fad9b6d0 auto
// OFFSET: [0x004CFD20, 0x004CFD90, 0x007EB210]
template <class T>
int32_t WowClientDB_RLE<T>::GetRecord(int32_t id, T* rec) const {
    if (id < this->m_minID || id > this->m_maxID) {
        return 0;
    }

    T** slot = &this->m_recordsById[id - this->m_minID];

    if (!*slot) {
        return 0;
    }

    if (g_clientDbPerformCompress) {
        DecompressRow(reinterpret_cast<const uint8_t*>(*slot), sizeof(T), reinterpret_cast<uint8_t*>(rec));
    } else {
        memcpy(rec, *slot, sizeof(T));
    }

    return 1;
}

// VERIFY: 040a5e1f auto
// OFFSET: [0x004D8580, 0x004DFD50, 0x004DFD70, 0x004FCF90, 0x00514030, 0x0061BD00]
template <class T>
T* WowClientDB<T>::GetRecordByIndex(int32_t index) const {
    if (index < 0 || index >= this->m_numRecords) {
        return nullptr;
    }

    return &this->m_records[index];
}

#endif
