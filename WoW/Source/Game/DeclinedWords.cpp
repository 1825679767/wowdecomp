#include "WoW/Source/Game/DeclinedWords.h"

// STATIC: 0x00D37DE4
static TSHashTable<DECLINEDWORD, HASHKEY_CONSTSTRI> s_declinedWords;

// VERIFY: 6162c5b1 manual
// OFFSET: 0x007E01B0
void DeclinedWordsDestroy() {
    s_declinedWords.Clear();
}
