#ifndef ENGINE_SOURCE_MODEL2_M2EXTERNALSEQUENCE_H
#define ENGINE_SOURCE_MODEL2_M2EXTERNALSEQUENCE_H
#include "Engine/Source/Model2/M2Data.h"
#include <cstdint>

// Read-only preflight: outer track tables live in the relocated model; selected
// non-global leaf offsets still refer to the not-yet-published .anim allocation.
namespace M2ExternalSequence {
template<class T> bool ModelArray(const M2Array<T>& a,const M2Data& data,uint32_t size) {
    if(!a.m_count)return true;
    const auto begin=reinterpret_cast<uintptr_t>(&data),address=reinterpret_cast<uintptr_t>(a.Data());
    return address>=begin&&address-begin<=size&&uint64_t(a.m_count)*sizeof(T)<=size-(address-begin);
}
template<class T> bool Leaf(const M2Array<T>& a,uint32_t size) {
    return !a.m_count||(a.m_offset<=size&&uint64_t(a.m_count)*sizeof(T)<=uint64_t(size)-a.m_offset);
}
template<class T> bool Selected(const M2Array<M2Array<T>>& a,uint32_t index,const M2Data& data,uint32_t modelSize,uint32_t streamSize) {
    return ModelArray(a,data,modelSize)&&(index>=a.m_count||Leaf(a[index],streamSize));
}
inline bool Track(const M2TrackBase& t,uint32_t i,const M2Data& data,uint32_t modelSize,uint32_t streamSize) {
    return t.m_loopIndex!=0xffff||Selected(t.m_sequenceTimes,i,data,modelSize,streamSize);
}
template<class T> bool Track(const M2Track<T>& t,uint32_t i,const M2Data& data,uint32_t modelSize,uint32_t streamSize) {
    return t.m_loopIndex!=0xffff||(Selected(t.m_sequenceTimes,i,data,modelSize,streamSize)&&Selected(t.m_sequenceKeys,i,data,modelSize,streamSize));
}
template<class T,class F> bool Rows(const M2Array<T>& a,const M2Data& data,uint32_t size,F check) {
    if(!ModelArray(a,data,size))return false;
    for(uint32_t i=0;i<a.m_count;++i)if(!check(a[i]))return false;
    return true;
}
inline bool Validate(const M2Data& data,uint32_t modelSize,uint32_t index,uint32_t streamSize) {
    if(modelSize<sizeof(M2Data)||!ModelArray(data.m_sequences,data,modelSize)||index>=data.m_sequences.m_count)return false;
    if(data.m_sequences[index].m_flags&0x20)return true; // already relocated, never relocate twice
    const auto track=[&](const auto& t){return Track(t,index,data,modelSize,streamSize);};
    const auto rows=[&](const auto& a,auto check){return Rows(a,data,modelSize,check);};
    return rows(data.m_bones,[&](const auto& r){return track(r.m_translationTrack)&&track(r.m_rotationTrack)&&track(r.m_scaleTrack);})
        &&rows(data.m_colors,[&](const auto& r){return track(r.m_color)&&track(r.m_alpha);})
        &&rows(data.m_textureWeights,[&](const auto& r){return track(r.m_weight);})
        &&rows(data.m_textureTransforms,[&](const auto& r){return track(r.m_translation)&&track(r.m_rotation)&&track(r.m_scaling);})
        &&rows(data.m_attachments,[&](const auto& r){return track(r.m_animateAttached);})
        &&rows(data.m_events,[&](const auto& r){return track(r.m_enabled);})
        &&rows(data.m_lights,[&](const auto& r){return track(r.m_ambientColor)&&track(r.m_ambientIntensity)&&track(r.m_diffuseColor)&&track(r.m_diffuseIntensity)&&track(r.m_attenuationStart)&&track(r.m_attenuationEnd)&&track(r.m_visibility);})
        &&rows(data.m_cameras,[&](const auto& r){return track(r.m_positions)&&track(r.m_targetPositions)&&track(r.m_roll);})
        &&rows(data.m_ribbons,[&](const auto& r){return track(r.m_colorTrack)&&track(r.m_alphaTrack)&&track(r.m_heightAboveTrack)&&track(r.m_heightBelowTrack)&&track(r.m_textureSlotTrack)&&track(r.m_visibilityTrack);})
        &&rows(data.m_particles,[&](const auto& r){return track(r.m_speedTrack)&&track(r.m_variationTrack)&&track(r.m_latitudeTrack)&&track(r.m_longitudeTrack)&&track(r.m_gravityTrack)&&track(r.m_lifeTrack)&&track(r.m_emissionRateTrack)&&track(r.m_widthTrack)&&track(r.m_lengthTrack)&&track(r.m_zsourceTrack)&&track(r.m_visibilityTrack);});
}
}
#endif
