#ifndef ENGINE_SOURCE_MODEL2_M2SHARED_H
#define ENGINE_SOURCE_MODEL2_M2SHARED_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Storm/H/stpl.h"

struct CAsyncObject;
struct CGxBuf;
struct CGxPool;
class CM2Cache;
class CM2Model;
class CM2Shared;
struct M2Data;
struct M2SkinProfile;

struct M2Bounds {
    /* 0000 */ CAaBox m_extent;
    /* 0018 */ float m_radius;
};

class CM2SequenceLoad {
    public:
    /* 0000 */ TSLink<void> m_link;
    /* 0008 */ CAsyncObject* m_asyncObject;
    /* 000C */ CM2Shared* m_shared;
    /* 0010 */ uint16_t m_sequenceIndex;
    /* 0012 */ uint16_t m_sequenceBufferIndex;
    /* 0014 */ TSList<void> m_playbacks;
};

class CM2Shared {
    public:
    /* 0000 */ uint32_t m_refCount;
    /* 0004 */ CM2Cache* m_cache;
    /* 0008 */ uint32_t m_flags;
    /* 000C */ CAsyncObject* m_asyncObject;
    /* 0010 */ CM2Model* m_callbackList;
    /* 0014 */ CM2Model** m_callbackListTail;
    /* 0018 */ TSList<void> unk_0018;
    /* 0024 */ uint32_t m_lowPrioritySequenceCapacity;
    /* 0028 */ uint32_t m_lowPrioritySequenceBuffers;
    /* 002C */ uint32_t m_lowPrioritySequenceCount;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ char m_fileName [260];
    /* 0140 */ char* m_fileNameWithoutPath;
    /* 0144 */ CM2Shared* m_previous;
    /* 0148 */ CM2Shared* m_next;
    /* 014C */ uint32_t m_fileNameHash;
    /* 0150 */ M2Data* m_data;
    /* 0154 */ CAaBox m_boundingBox;
    /* 016C */ uint32_t m_fileSize;
    /* 0170 */ M2SkinProfile* m_skinData;
    /* 0174 */ uint32_t unk_0174;
    /* 0178 */ CGxPool* m_indexPool;
    /* 017C */ CGxBuf* m_indexBuf;
    /* 0180 */ uint32_t unk_0180;
    /* 0184 */ uint32_t unk_0184;
    /* 0188 */ uint32_t unk_0188;
    /* 018C */ uint32_t unk_018C;
    /* 0190 */ uint32_t unk_0190;
    /* 0194 */ uint32_t unk_0194;
    /* 0198 */ uint32_t unk_0198;
    /* 019C */ uint32_t unk_019C;
    /* 01A0 */ uint32_t unk_01A0;
    /* 01A4 */ uint32_t unk_01A4;
};

struct M2Array {
    /* 0000 */ uint32_t m_count;
    /* 0004 */ uint32_t m_offset;
};

struct M2Batch {
    /* 0000 */ uint8_t m_flags;
    /* 0001 */ int8_t m_priorityPlane;
    /* 0002 */ uint16_t m_shader;
    /* 0004 */ uint16_t m_skinSectionIndex;
    /* 0006 */ uint16_t m_geosetIndex;
    /* 0008 */ uint16_t m_colorIndex;
    /* 000A */ uint16_t m_materialIndex;
    /* 000C */ uint16_t m_materialLayer;
    /* 000E */ uint16_t m_textureCount;
    /* 0010 */ uint16_t m_textureComboIndex;
    /* 0012 */ uint16_t m_textureCoordComboIndex;
    /* 0014 */ uint16_t m_textureWeightComboIndex;
    /* 0016 */ uint16_t m_textureTransformComboIndex;
};

struct M2TrackBase {
    /* 0000 */ uint16_t m_trackType;
    /* 0002 */ uint16_t m_loopIndex;
    /* 0004 */ M2Array m_sequenceTimes;
};

struct M2Track : public M2TrackBase {
    /* 000C */ M2Array m_sequenceKeys;
};

struct M2CompBone {
    /* 0000 */ uint32_t m_boneId;
    /* 0004 */ uint32_t m_flags;
    /* 0008 */ uint16_t m_parentIndex;
    /* 000A */ uint16_t m_uDistToParent;
    /* 000C */ uint32_t m_boneNameCRC;
    /* 0010 */ M2Track m_translationTrack;
    /* 0024 */ M2Track m_rotationTrack;
    /* 0038 */ M2Track m_scaleTrack;
    /* 004C */ C3Vector m_pivot;
};

struct M2Data {
    /* 0000 */ uint32_t m_md20;
    /* 0004 */ uint32_t m_version;
    /* 0008 */ M2Array m_name;
    /* 0010 */ uint32_t m_flags;
    /* 0014 */ M2Array m_loops;
    /* 001C */ M2Array m_sequences;
    /* 0024 */ M2Array m_sequenceIdxHashById;
    /* 002C */ M2Array m_bones;
    /* 0034 */ M2Array m_boneIndicesById;
    /* 003C */ M2Array m_vertices;
    /* 0044 */ uint32_t m_numSkinProfiles;
    /* 0048 */ M2Array m_colors;
    /* 0050 */ M2Array m_textures;
    /* 0058 */ M2Array m_textureWeights;
    /* 0060 */ M2Array m_textureTransforms;
    /* 0068 */ M2Array m_textureIndicesById;
    /* 0070 */ M2Array m_materials;
    /* 0078 */ M2Array m_boneCombos;
    /* 0080 */ M2Array m_textureCombos;
    /* 0088 */ M2Array m_textureCoordCombos;
    /* 0090 */ M2Array m_textureWeightCombos;
    /* 0098 */ M2Array m_textureTransformCombos;
    /* 00A0 */ M2Bounds m_bounds;
    /* 00BC */ M2Bounds m_collisionBounds;
    /* 00D8 */ M2Array m_collisionIndices;
    /* 00E0 */ M2Array m_collisionPositions;
    /* 00E8 */ M2Array m_collisionFaceNormals;
    /* 00F0 */ M2Array m_attachments;
    /* 00F8 */ M2Array m_attachmentIndicesById;
    /* 0100 */ M2Array m_events;
    /* 0108 */ M2Array m_lights;
    /* 0110 */ M2Array m_cameras;
    /* 0118 */ M2Array m_cameraIndicesById;
    /* 0120 */ M2Array m_ribbons;
    /* 0128 */ M2Array m_particles;
    /* 0130 */ M2Array m_textureCombinerCombos;
};

struct M2SkinProfile {
    /* 0000 */ uint32_t m_magic;
    /* 0004 */ M2Array m_vertices;
    /* 000C */ M2Array m_indices;
    /* 0014 */ M2Array m_bones;
    /* 001C */ M2Array m_skinSections;
    /* 0024 */ M2Array m_batches;
    /* 002C */ uint32_t m_boneCountMax;
};

#endif
