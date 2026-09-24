#ifndef ENGINE_SOURCE_GX_GX_H
#define ENGINE_SOURCE_GX_GX_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Tempest/C2iVector.h"

class C3Vector;
class C44Matrix;
class CGxBatch;
class CGxBuf;
class CGxQuery;
class CGxTex;
class CGxTexParms;
class CImVector;
class CRect;

char* GxBufLock(CGxBuf* buf);
void GxBufUnlock(CGxBuf* buf, uint32_t size);
void GxBufData(CGxBuf* buf, const void* data, uint32_t size, uint32_t offset);
void GxCapsWindowSizeInScreenCoords(CRect& rect);
void GxDraw(CGxBatch* batch, int32_t count);
void GxDrawLockedElements();
BlitFormat GxGetBlitFormat(EGxTexFormat format);
uint32_t GxGetMaxFPS();
uint32_t GxGetMaxFPSBk();
int32_t GxHasContext();
int32_t GxIsWindowVisible();
int32_t GxMasterEnable(EGxMasterEnables state);
void GxQueryCreate(CGxQuery*& query, EGxQueryType type);
void GxQueryDestroy(CGxQuery*& query);
void GxSceneClear(uint32_t mask, CImVector color);
void GxSetMaxFPS(uint32_t maxFPS);
void GxSetMaxFPSBk(uint32_t maxFPSBk);
void GxShaderConstantsSet(EGxShTarget target, uint32_t index, const float* constants, uint32_t count);
int32_t GxTexCopy(CGxTex* dest, CGxTex* source, const C2iVector& pos, const C2iVector& size, uint32_t level, uint32_t plane);
void GxTexDestroy(CGxTex* texId);
int32_t GxTexReusable(CGxTexParms& parms);
void GxXformPop(EGxXform xf);
void GxXformProjection(C44Matrix& matrix);
void GxXformPush(EGxXform xf);
void GxXformScale(EGxXform xf, const C3Vector& scale);
void GxXformSetProjection(const C44Matrix& matrix);
void GxXformSetView(const C44Matrix& matrix);
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
