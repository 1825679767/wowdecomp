#ifndef ENGINE_SOURCE_IMAGES_IMAGEWRITER_H
#define ENGINE_SOURCE_IMAGES_IMAGEWRITER_H

#include <cstddef>
#include <cstdint>

int32_t ImageWriterEmbedData(const void* pixels, uint32_t width, uint32_t height, uint32_t bytesPerPixel, const uint8_t* data, uint32_t dataSize);
int32_t ImageWriterInitialize(uint32_t dataSize, void* (*allocFunc)(size_t bytes), void (*freeFunc)(void* ptr), int32_t flags);
int32_t ImageWriterShutdown();
int32_t ImageWriterWriteJpeg(const char* fileName, const void* pixels, uint32_t width, uint32_t height, uint32_t bytesPerPixel, uint32_t quality);

#endif
