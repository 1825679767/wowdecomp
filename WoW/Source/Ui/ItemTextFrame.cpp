#include "ItemTextFrame.h"

// STATIC: 0x00BFC948
WOWGUID CGItemText::m_itemGUID;

// STATIC: 0x00BFC940
uint32_t CGItemText::m_currentPage;

// STATIC: 0x00BFA9F8
uint8_t CGItemText::m_hasItemText;

// STATIC: 0x00BFAA00
char CGItemText::m_text[8000];

// STATIC: 0x00BFC958
TSGrowableArray<int32_t> CGItemText::m_pages;

// VERIFY: 6900c62a manual
// OFFSET: 0x00589DC0
void CGItemText::InitializeGame() {
    CGItemText::m_itemGUID.m_guid = 0;
    CGItemText::m_hasItemText = 0;
    CGItemText::m_currentPage = 0;
    CGItemText::m_pages.SetCount(2);
    CGItemText::m_pages[0] = 0;
    CGItemText::m_pages[1] = 0;
    CGItemText::m_text[0] = '\0';
}

// VERIFY: 954655b8 auto
// OFFSET: 0x00589E50
void CGItemText::ShutdownGame() {
    CGItemText::m_pages.Clear();
}
