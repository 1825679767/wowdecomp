#ifndef WOW_SOURCE_UI_UILCD_WINDOWS_EZ_LCD_PAGE_H
#define WOW_SOURCE_UI_UILCD_WINDOWS_EZ_LCD_PAGE_H

#include <cstdint>

class CEzLcdPage {
    public:
    /* 0004 */ uint8_t unk_0004[276];

    CEzLcdPage();
    virtual ~CEzLcdPage();
};

#endif
