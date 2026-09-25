/*$ preserve start $*/

#include "fmod_settings.h"

#include "fmod.hpp"
#include "fmod_channeli.h"
#include "fmod_systemi.h"

namespace FMOD
{
/*$ preserve end $*/


FMOD_RESULT Channel::getSystemObject(System **system)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (system)
        {
            *system = 0;
        }
        return result;
    }
    else
    {
        return channeli->getSystemObject(system);
    }
}


// OFFSET: 0x008D0930
FMOD_RESULT Channel::stop()
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->stop();
    }
}


// OFFSET: 0x008D0960
FMOD_RESULT Channel::setPaused(bool paused)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setPaused(paused);
    }
}


FMOD_RESULT Channel::getPaused(bool *paused)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (paused)
        {
            *paused = false;
        }
        return result;
    }
    else
    {
        return channeli->getPaused(paused);
    }
}


// OFFSET: 0x008D0990
FMOD_RESULT Channel::setVolume(float volume)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setVolume(volume);
    }
}


// OFFSET: 0x008D09C0
FMOD_RESULT Channel::getVolume(float *volume)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (volume)
        {
            *volume = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getVolume(volume);
    }
}


// OFFSET: 0x008D0A00
FMOD_RESULT Channel::setFrequency(float frequency)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setFrequency(frequency);
    }
}


// OFFSET: 0x008D0A30
FMOD_RESULT Channel::getFrequency(float *frequency)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (frequency)
        {
            *frequency = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getFrequency(frequency);
    }
}


FMOD_RESULT Channel::setPan(float pan)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setPan(pan);
    }
}


FMOD_RESULT Channel::getPan(float *pan)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (pan)
        {
            *pan = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getPan(pan);
    }
}


FMOD_RESULT Channel::setDelay(FMOD_DELAYTYPE delaytype, unsigned int delayhi, unsigned int delaylo)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setDelay(delaytype, delayhi, delaylo);
    }
}


FMOD_RESULT Channel::getDelay(FMOD_DELAYTYPE delaytype, unsigned int *delayhi, unsigned int *delaylo)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (delayhi)
        {
            *delayhi = 0;
        }
        if (delaylo)
        {
            *delaylo = 0;
        }
        return result;
    }
    else
    {
        return channeli->getDelay(delaytype, delayhi, delaylo);
    }
}


// OFFSET: 0x008D0A70
FMOD_RESULT Channel::setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setSpeakerMix(frontleft, frontright, center, lfe, backleft, backright, sideleft, sideright);
    }
}


FMOD_RESULT Channel::getSpeakerMix(float *frontleft, float *frontright, float *center, float *lfe, float *backleft, float *backright, float *sideleft, float *sideright)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (frontleft)
        {
            *frontleft = 0.0f;
        }
        if (frontright)
        {
            *frontright = 0.0f;
        }
        if (center)
        {
            *center = 0.0f;
        }
        if (lfe)
        {
            *lfe = 0.0f;
        }
        if (backleft)
        {
            *backleft = 0.0f;
        }
        if (backright)
        {
            *backright = 0.0f;
        }
        if (sideleft)
        {
            *sideleft = 0.0f;
        }
        if (sideright)
        {
            *sideright = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getSpeakerMix(frontleft, frontright, center, lfe, backleft, backright, sideleft, sideright);
    }
}


FMOD_RESULT Channel::setSpeakerLevels(FMOD_SPEAKER speaker, float *levels, int numlevels)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setSpeakerLevels(speaker, levels, numlevels);
    }
}


FMOD_RESULT Channel::getSpeakerLevels(FMOD_SPEAKER speaker, float *levels, int numlevels)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (levels)
        {
            *levels = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getSpeakerLevels(speaker, levels, numlevels);
    }
}


FMOD_RESULT Channel::setInputChannelMix(float *levels, int numlevels)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setInputChannelMix(levels, numlevels);
    }
}


FMOD_RESULT Channel::getInputChannelMix(float *levels, int numlevels)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (levels)
        {
            *levels = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getInputChannelMix(levels, numlevels);
    }
}


FMOD_RESULT Channel::setMute(bool mute)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setMute(mute);
    }
}


FMOD_RESULT Channel::getMute(bool *mute)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (mute)
        {
            *mute = false;
        }
        return result;
    }
    else
    {
        return channeli->getMute(mute);
    }
}


// OFFSET: 0x008D0AD0
FMOD_RESULT Channel::setPriority(int priority)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setPriority(priority);
    }
}


// OFFSET: 0x008D0B00
FMOD_RESULT Channel::getPriority(int *priority)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (priority)
        {
            *priority = 0;
        }
        return result;
    }
    else
    {
        return channeli->getPriority(priority);
    }
}


// OFFSET: 0x008D0B40
FMOD_RESULT Channel::setPosition(unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setPosition(position, postype);
    }
}


// OFFSET: 0x008D0B70
FMOD_RESULT Channel::getPosition(unsigned int *position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (position)
        {
            *position = 0;
        }
        return result;
    }
    else
    {
        return channeli->getPosition(position, postype);
    }
}


// OFFSET: 0x008D0BB0
FMOD_RESULT Channel::setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES *prop)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setReverbProperties(prop);
    }
}


// OFFSET: 0x008D0BE0
FMOD_RESULT Channel::getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES *prop)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->getReverbProperties(prop);
    }
}


FMOD_RESULT Channel::setLowPassGain(float gain)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setLowPassGain(gain);
    }
}


FMOD_RESULT Channel::getLowPassGain(float *gain)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (gain)
        {
            *gain = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getLowPassGain(gain);
    }
}


// OFFSET: 0x008D0C10
FMOD_RESULT Channel::setChannelGroup(ChannelGroup *channelgroup)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setChannelGroup((ChannelGroupI *)channelgroup);
    }
}


FMOD_RESULT Channel::getChannelGroup(ChannelGroup **channelgroup)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (channelgroup)
        {
            *channelgroup = 0;
        }
        return result;
    }
    else
    {
        return channeli->getChannelGroup((ChannelGroupI **)channelgroup);
    }
}


// OFFSET: 0x008D0C40
FMOD_RESULT Channel::setCallback(FMOD_CHANNEL_CALLBACK callback)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setCallback(callback);
    }
}


// OFFSET: 0x008D0C70
FMOD_RESULT Channel::set3DAttributes(const FMOD_VECTOR *pos, const FMOD_VECTOR *vel)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DAttributes(pos, vel);
    }
}


// OFFSET: 0x008D0CA0
FMOD_RESULT Channel::get3DAttributes(FMOD_VECTOR *pos, FMOD_VECTOR *vel)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (pos)
        {
            pos->x = pos->y = pos->z = 0.0f;
        }
        if (vel)
        {
            vel->x = vel->y = vel->z = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->get3DAttributes(pos, vel);
    }
}


// OFFSET: 0x008D0D00
FMOD_RESULT Channel::set3DMinMaxDistance(float mindistance, float maxdistance)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DMinMaxDistance(mindistance, maxdistance);
    }
}


// OFFSET: 0x008D0D40
FMOD_RESULT Channel::get3DMinMaxDistance(float *mindistance, float *maxdistance)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (mindistance)
        {
            *mindistance = 0.0f;
        }
        if (maxdistance)
        {
            *maxdistance = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->get3DMinMaxDistance(mindistance, maxdistance);
    }
}


// OFFSET: 0x008D0D90
FMOD_RESULT Channel::set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DConeSettings(insideconeangle, outsideconeangle, outsidevolume);
    }
}


FMOD_RESULT Channel::get3DConeSettings(float *insideconeangle, float *outsideconeangle, float *outsidevolume)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (insideconeangle)
        {
            *insideconeangle = 0.0f;
        }
        if (outsideconeangle)
        {
            *outsideconeangle = 0.0f;
        }
        if (outsidevolume)
        {
            *outsidevolume = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->get3DConeSettings(insideconeangle, outsideconeangle, outsidevolume);
    }
}


// OFFSET: 0x008D0DD0
FMOD_RESULT Channel::set3DConeOrientation(FMOD_VECTOR *orientation)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DConeOrientation(orientation);
    }
}


FMOD_RESULT Channel::get3DConeOrientation(FMOD_VECTOR *orientation)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (orientation)
        {
            orientation->x = orientation->y = orientation->z = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->get3DConeOrientation(orientation);
    }
}


FMOD_RESULT Channel::set3DCustomRolloff(FMOD_VECTOR *points, int numpoints)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DCustomRolloff(points, numpoints);
    }
}


FMOD_RESULT Channel::get3DCustomRolloff(FMOD_VECTOR **points, int *numpoints)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (points)
        {
            *points = 0;
        }
        if (numpoints)
        {
            *numpoints = 0;
        }
        return result;
    }
    else
    {
        return channeli->get3DCustomRolloff(points, numpoints);
    }
}


FMOD_RESULT Channel::set3DOcclusion(float directocclusion, float reverbocclusion)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DOcclusion(directocclusion, reverbocclusion);
    }
}


FMOD_RESULT Channel::get3DOcclusion(float *directocclusion, float *reverbocclusion)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (directocclusion)
        {
            *directocclusion = 0.0f;
        }
        if (reverbocclusion)
        {
            *reverbocclusion = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->get3DOcclusion(directocclusion, reverbocclusion);
    }
}


FMOD_RESULT Channel::set3DSpread(float angle)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DSpread(angle);
    }
}


FMOD_RESULT Channel::get3DSpread(float *angle)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (angle)
        {
            *angle = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->get3DSpread(angle);
    }
}


// OFFSET: 0x008D0E00
FMOD_RESULT Channel::set3DPanLevel(float level)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DPanLevel(level);
    }
}


// OFFSET: 0x008D0E30
FMOD_RESULT Channel::get3DPanLevel(float *level)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (level)
        {
            *level = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->get3DPanLevel(level);
    }
}


FMOD_RESULT Channel::set3DDopplerLevel(float level)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DDopplerLevel(level);
    }
}


FMOD_RESULT Channel::get3DDopplerLevel(float *level)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (level)
        {
            *level = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->get3DDopplerLevel(level);
    }
}


FMOD_RESULT Channel::getDSPHead(DSP **dsp)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (dsp)
        {
            *dsp = 0;
        }
        return result;
    }
    else
    {
        return channeli->getDSPHead((DSPI **)dsp);
    }
}


// OFFSET: 0x008D0E70
FMOD_RESULT Channel::addDSP(DSP *dsp, DSPConnection **connection)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->addDSP((DSPI *)dsp, (DSPConnectionI **)connection);
    }
}


// OFFSET: 0x008D0EA0
FMOD_RESULT Channel::isPlaying(bool *isplaying)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (isplaying)
        {
            *isplaying = false;
        }
        return result;
    }
    else
    {
        return channeli->isPlaying(isplaying);
    }
}


// OFFSET: 0x008D0EE0
FMOD_RESULT Channel::isVirtual(bool *isvirtual)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (isvirtual)
        {
            *isvirtual = false;
        }
        return result;
    }
    else
    {
        return channeli->isVirtual(isvirtual);
    }
}


// OFFSET: 0x008D0F20
FMOD_RESULT Channel::getAudibility(float *audibility)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (audibility)
        {
            *audibility = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getAudibility(audibility);
    }
}


FMOD_RESULT Channel::getCurrentSound(Sound **sound)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (sound)
        {
            *sound = 0;
        }
        return result;
    }
    else
    {
        return channeli->getCurrentSound((SoundI **)sound);
    }
}


FMOD_RESULT Channel::getSpectrum(float *spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (spectrumarray)
        {
            *spectrumarray = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getSpectrum(spectrumarray, numvalues, channeloffset, windowtype);
    }
}


FMOD_RESULT Channel::getWaveData(float *wavearray, int numvalues, int channeloffset)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (wavearray)
        {
            *wavearray = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getWaveData(wavearray, numvalues, channeloffset);
    }
}


FMOD_RESULT Channel::getIndex(int *index)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (index)
        {
            *index = 0;
        }
        return result;
    }
    else
    {
        return channeli->getIndex(index);
    }
}


FMOD_RESULT Channel::setMode(FMOD_MODE mode)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setMode(mode);
    }
}


// OFFSET: 0x008D0FA0
FMOD_RESULT Channel::getMode(FMOD_MODE *mode)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (mode)
        {
            *mode = (FMOD_MODE)0;
        }
        return result;
    }
    else
    {
        return channeli->getMode(mode);
    }
}


FMOD_RESULT Channel::setLoopCount(int loopcount)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setLoopCount(loopcount);
    }
}


FMOD_RESULT Channel::getLoopCount(int *loopcount)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (loopcount)
        {
            *loopcount = 0;
        }
        return result;
    }
    else
    {
        return channeli->getLoopCount(loopcount);
    }
}


FMOD_RESULT Channel::setLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setLoopPoints(loopstart, loopstarttype, loopend, loopendtype);
    }
}


FMOD_RESULT Channel::getLoopPoints(unsigned int *loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int *loopend, FMOD_TIMEUNIT loopendtype)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (loopstart)
        {
            *loopstart = 0;
        }
        if (loopend)
        {
            *loopend = 0;
        }
        return result;
    }
    else
    {
        return channeli->getLoopPoints(loopstart, loopstarttype, loopend, loopendtype);
    }
}


// OFFSET: 0x008D0FE0
FMOD_RESULT Channel::setUserData(void *_userdata)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setUserData(_userdata);
    }
}


// OFFSET: 0x008D1010
FMOD_RESULT Channel::getUserData(void **_userdata)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (_userdata)
        {
            *_userdata = 0;
        }
        return result;
    }
    else
    {
        return channeli->getUserData(_userdata);
    }
}


FMOD_RESULT Channel::getMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, FMOD_MEMORY_USAGE_DETAILS *memoryused_details)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (memoryused)
        {
            *memoryused = 0;
        }
        if (memoryused_details)
        {
            FMOD_memset(memoryused_details, 0, sizeof(FMOD_MEMORY_USAGE_DETAILS));
        }
        return result;
    }
    else
    {
        return channeli->getMemoryInfo(memorybits, event_memorybits, memoryused, memoryused_details);
    }
}


/*$ preserve start $*/
}
/*$ preserve end $*/
