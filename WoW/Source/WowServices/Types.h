#ifndef WOW_SOURCE_WOWSERVICES_TYPES_H
#define WOW_SOURCE_WOWSERVICES_TYPES_H

#include <cstddef>
#include <cstdint>

class RealmResponse {
    public:
    virtual ~RealmResponse();
};

enum WOW_CONN_STATE {
    WOWC_UNINITIALIZED = 0,
    WOWC_INITIALIZED = 1,
    WOWC_CONNECTING = 2,
    WOWC_LISTENING = 3,
    WOWC_ACCEPTED = 4,
    WOWC_CONNECTED = 5,
    WOWC_DISCONNECTED = 6,
    WOWC_DISCONNECTING = 7,
    WOWC_ERROR = 8,
};

#endif
