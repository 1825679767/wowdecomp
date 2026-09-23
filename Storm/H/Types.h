#ifndef STORM_H_TYPES_H
#define STORM_H_TYPES_H

#include <cstdint>

#define STORMAPI __stdcall

struct _CMDPARAMS {
    /* 0000 */ uint32_t flags;
    /* 0004 */ uint32_t id;
    /* 0008 */ const char* name;
    /* 000C */ void* variable;
    /* 0010 */ uint32_t setvalue;
    /* 0014 */ uint32_t setmask;
    union {
        /* 0018 */ int32_t boolvalue;
        /* 0018 */ int32_t signedvalue;
        /* 0018 */ uint32_t unsignedvalue;
        /* 0018 */ const char* stringvalue;
    };
};

struct _ARGLIST {
    /* 0000 */ uint32_t flags;
    /* 0004 */ uint32_t id;
    /* 0008 */ const char* name;
    /* 000C */ int32_t (STORMAPI* callback)(_CMDPARAMS*, const char*);
};

struct _CMDERROR {
    /* 0000 */ uint32_t errorcode;
    /* 0004 */ const char* itemstr;
    /* 0008 */ const char* errorstr;
};

#endif
