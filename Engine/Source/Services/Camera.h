#ifndef ENGINE_SOURCE_SERVICES_CAMERA_H
#define ENGINE_SOURCE_SERVICES_CAMERA_H

#include <cstddef>
#include <cstdint>

class C3Vector;
class CCamera;

CCamera* CameraCreate();
void CameraSetCoord(CCamera* camera, uint32_t coord, const C3Vector& value);
void CameraSetFloat(CCamera* camera, uint32_t param, float value);

#endif
