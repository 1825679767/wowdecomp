#if defined(WOWDECOMP_ARCH_32)

#include <cmath>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"

// VERIFY: 8fa175ce manual
// OFFSET: 0x00684D70
int32_t CGxDevice::AdjustWindowRectToWorkArea(float aspectRatio, C2iVector* adjust, tagRECT* rect) {
    POINT origin = { 0, 0 };
    HMONITOR monitor = MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);

    MONITORINFO info;
    info.cbSize = sizeof(info);

    if (!GetMonitorInfoA(monitor, &info)) {
        return 0;
    }

    int32_t height = rect->bottom - rect->top;
    int32_t width = rect->right - rect->left;

    rect->top = info.rcWork.top;
    rect->left = info.rcWork.left;
    rect->right = width + info.rcWork.left + adjust->x;
    rect->bottom = height + info.rcWork.top + adjust->y;

    if (rect->right > info.rcWork.right) {
        rect->right = info.rcWork.right;

        if (aspectRatio != 0.0f) {
            int32_t workWidth = info.rcWork.right - info.rcWork.left;
            int32_t maxWidth = workWidth - adjust->x;
            int32_t newWidth = maxWidth;
            float invAspect = 1.0f / aspectRatio;

            for (; newWidth >= width / 2; newWidth--) {
                float newHeight = newWidth * invAspect;

                if (newHeight - floor(newHeight) < 0.01f) {
                    break;
                }
            }

            double newHeight;

            if (newWidth < width / 2) {
                newHeight = maxWidth / aspectRatio + 0.5f;
            } else {
                workWidth = newWidth + adjust->x;
                newHeight = newWidth * invAspect;
            }

            rect->right = info.rcWork.left + workWidth;
            rect->bottom = info.rcWork.top + static_cast<int32_t>(adjust->y + floor(newHeight));
        }
    }

    if (rect->bottom > info.rcWork.bottom) {
        rect->bottom = info.rcWork.bottom;

        if (aspectRatio != 0.0f) {
            int32_t workHeight = info.rcWork.bottom - info.rcWork.top;
            int32_t maxHeight = workHeight - adjust->y;
            int32_t newHeight = maxHeight;

            for (; newHeight >= height / 2; newHeight--) {
                float newWidth = newHeight * aspectRatio;

                if (newWidth - floor(newWidth) < 0.01f) {
                    break;
                }
            }

            int32_t newWidth;

            if (newHeight < height / 2) {
                newWidth = static_cast<int32_t>(floor(maxHeight * aspectRatio + 0.5f));
            } else {
                workHeight = adjust->y + newHeight;
                newWidth = static_cast<int32_t>(floor(newHeight * aspectRatio));
            }

            rect->right = info.rcWork.left + adjust->x + newWidth;
            rect->bottom = info.rcWork.top + workHeight;
        }
    }

    int32_t spaceY = info.rcWork.bottom - rect->bottom;
    int32_t spaceX = info.rcWork.right - rect->right;

    rect->right += spaceX / 2;
    rect->left = spaceX / 2 + info.rcWork.left;
    rect->bottom += spaceY / 2;
    rect->top = spaceY / 2 + info.rcWork.top;
    rect->right -= adjust->x;
    rect->bottom -= adjust->y;

    return 1;
}

#endif
