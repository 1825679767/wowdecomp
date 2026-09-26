#ifndef ENGINE_SOURCE_GX_GX_H
#define ENGINE_SOURCE_GX_GX_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Tempest/C2iVector.h"

class C2Vector;
class C3Vector;
class C44Matrix;
class CGxBatch;
struct CGxBuf;
class CGxQuery;
class CGxTex;
class CGxTexParms;
class CImVector;
class CRect;

char* GxBufLock(CGxBuf* buf);
void GxBufUnlock(CGxBuf* buf, uint32_t size);
void GxBufDestroy(CGxBuf* buf);
void GxRenderTargetSet(EGxBuffer buffer, CGxTex* texture, uint32_t plane);
void GxBufData(CGxBuf* buf, const void* data, uint32_t size, uint32_t offset);
CGxBuf* GxBufStream(EGxPoolTarget target, uint32_t itemSize, uint32_t itemCount);
void GxPrimIndexPtr(uint32_t indexCount, const uint16_t* indices);
void GxPrimLockVertexPtrs(uint32_t vertexCount, const C3Vector* pos, uint32_t posStride, const C3Vector* normal, uint32_t normalStride, const CImVector* color, uint32_t colorStride, const uint8_t* bone, uint32_t boneStride, const C2Vector* tex0, uint32_t tex0Stride, const C2Vector* tex1, uint32_t tex1Stride);
void GxPrimVertexPtr(CGxBuf* buf, EGxVertexBufferFormat format);
void GxPrimVertexPtr(uint32_t vertexCount, const C3Vector* pos, uint32_t posStride, const C3Vector* normal, uint32_t normalStride, const CImVector* color, uint32_t colorStride, const C2Vector* tex0, uint32_t tex0Stride, const C2Vector* tex1, uint32_t tex1Stride);
void GxPrimLockIndexPtr(EGxPrim primType, uint32_t indexCount, const uint16_t* indices);
void GxTexParameters(const CGxTex* texId, CGxTexParms& parms);
void GxTexSetCannotUpdate(CGxTex* texId);
void GxTexSetDataFormat(CGxTex* texId, EGxTexFormat dataFormat);
void GxTexSetUserData(CGxTex* texId, GxTexUserFunc userFunc, void* userArg);
void GxTexUpdate(CGxTex* texId, int32_t minX, int32_t minY, int32_t maxX, int32_t maxY, int32_t immediate);
void GxTexUpdate(CGxTex* texId, CiRect& updateRect, int32_t immediate);
int32_t GxTexCreate(const CGxTexParms& parms, CGxTex*& texId);
int32_t GxTexCreate(EGxTexTarget target, uint32_t width, uint32_t height, uint32_t depth, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags flags, void* userArg, GxTexUserFunc userFunc, const char* name, CGxTex*& texId);
int32_t GxTexCreate(uint32_t width, uint32_t height, EGxTexFormat format, CGxTexFlags flags, void* userArg, GxTexUserFunc userFunc, CGxTex*& texId);
void GxXformPush(EGxXform xf, const C44Matrix& matrix);
void GxCapsWindowSizeInScreenCoords(CRect& rect);
void GxFormatColor(CImVector& color);
void GxDraw(CGxBatch* batch, int32_t count);
void GxDrawLockedElements();
extern const char* g_gxApiNames[GxApis_Last];

BlitFormat GxGetBlitFormat(EGxTexFormat format);
uint32_t GxGetMaxFPS();
uint32_t GxGetMaxFPSBk();
int32_t GxHasContext();
void GxLog(const char* format, ...);
void GxLogClose();
void GxLogCrashInfo(char* buffer, uint32_t size);
void GxLogOpen();
int32_t GxIsWindowVisible();
int32_t GxMasterEnable(EGxMasterEnables state);
void GxQueryCreate(CGxQuery*& query, EGxQueryType type);
void GxQueryDestroy(CGxQuery*& query);
void GxRsSet(EGxRenderState which, void* value);
void GxRsSet(EGxRenderState which, int32_t value);
void GxRsSet(EGxRenderState which, float value);
void GxUpdateAlphaRef(EGxRenderState which);
void GxSceneClear(uint32_t mask, CImVector color);
void GxTexSetFlags(CGxTex* texId, CGxTexFlags flags);
void GxTexSetWrap(CGxTex* texId, EGxTexWrapMode wrapU, EGxTexWrapMode wrapV);
void GxSetMaxFPS(uint32_t maxFPS);
void GxSetMaxFPSBk(uint32_t maxFPSBk);
void GxShaderConstantsSet(EGxShTarget target, uint32_t index, const float* constants, uint32_t count);
int32_t GxTexCopy(CGxTex* dest, CGxTex* source, const C2iVector& pos, const C2iVector& size, uint32_t level, uint32_t plane);
void GxTexDestroy(CGxTex* texId);
int32_t GxTexReusable(CGxTexParms& parms);
int32_t GxTexReusable(CGxTex* texId);
void GxXformPop(EGxXform xf);
void GxXformProjection(C44Matrix& matrix);
void GxXformPush(EGxXform xf);
void GxXformScale(EGxXform xf, const C3Vector& scale);
void GxXformSetProjection(const C44Matrix& matrix);
void GxXformSetView(const C44Matrix& matrix);
void GxXformSetViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
void GxXformTranslate(EGxXform xf, const C3Vector& move);
void GxXformView(C44Matrix& matrix);
void GxXformViewport(float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ);
void GxXformViewProj(C44Matrix& matrix);
void GxXformViewProjNativeTranspose(C44Matrix& matrix);
int32_t GxApiSupported(EGxApi api);
uint32_t GxCalcTexelStrideInBytes(EGxTexFormat format, uint32_t width);
uint32_t GxVertexAttribOffset(EGxVertexBufferFormat format, EGxVertexAttrib attrib);
uint32_t GxVertexSize(EGxVertexBufferFormat format);

#endif
