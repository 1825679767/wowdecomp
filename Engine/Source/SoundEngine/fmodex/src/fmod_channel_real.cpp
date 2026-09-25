#include "fmod_settings.h"

#include "fmod_3d.h"
#include "fmod_channel_real.h"
#include "fmod_channeli.h"
#include "fmod_channelpool.h"
#include "fmod_output.h"
#include "fmod_systemi.h"
#include "fmod_soundi.h"
#include "fmod_speakermap.h"

#include <string.h>

namespace FMOD
{


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00943900
ChannelReal::ChannelReal()
{
    mSound              = 0;
    mSystem             = 0;
    mOutput             = 0;
    mPool               = 0;
    mLoopCount          = -1;
    mMinFrequency       = 100.0f;
    mMaxFrequency       = 1000000.0f;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00943970
FMOD_RESULT ChannelReal::init(int index, SystemI *system, Output *output, DSPI *dspmixtarget)
{
    mSound              = 0;
    mFlags              = (CHANNELREAL_FLAG)0;
    mMode               = (FMOD_MODE)0;
    mPosition           = 0;
    mDirection          = CHANNELREAL_PLAYDIR_FORWARDS;
    mLoopCount          = -1;
    mOutput             = output;
    mSystem             = system;
    mIndex              = index;

    #ifdef PLATFORM_WII
    mControllerSpeaker  = 0;
    mBiquadActive       = false;
    mBiquadB0           = 0;
    mBiquadB1           = 0;
    mBiquadB2           = 0;
    mBiquadA1           = 0;
    mBiquadA2           = 0;
    mLPFCutoff          = -1;
    #endif

    return FMOD_OK;
}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x009439C0
FMOD_RESULT ChannelReal::close()
{
    return stop();
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x009439D0
FMOD_RESULT ChannelReal::alloc()
{
    mPosition = 0;

    if (mPool)
    {
        mPool->mChannelsUsed++;
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x009439F0
FMOD_RESULT ChannelReal::alloc(DSPI *dsp)
{
    if (mPool)
    {      
        mPool->mChannelsUsed++;
    }
    
    mPosition = 0;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelReal::set2DFreqVolumePanFor3D()
{
    /*
        Channel types that have their own 3d system (ie dsound3d or xbox) will call this,
        software and others use the stereo panning method defined in ChannelRealManual3D.
    */
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelReal::update(int delta)
{
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelReal::updateStream()
{  
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelReal::start()
{
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00943A10
FMOD_RESULT ChannelReal::stop()
{
//    mSound = 0;   /* ChannelI or ChannelStream will clear this out in its stop call. */
//    mDSP   = 0;   /* ChannelI or ChannelStream will clear this out in its stop call. */

    if (mPool)  /* Channel might not belong to a pool */
    {
        mPool->mChannelsUsed--;
    }

    mFlags = (CHANNELREAL_FLAG)(mFlags & ~CHANNELREAL_FLAG_ALLOCATED);
    mFlags = (CHANNELREAL_FLAG)(mFlags & ~CHANNELREAL_FLAG_PLAYING);
    mFlags = (CHANNELREAL_FLAG)(mFlags & ~CHANNELREAL_FLAG_PAUSED);

    mFlags = (CHANNELREAL_FLAG)(mFlags |  CHANNELREAL_FLAG_STOPPED);

    #ifdef PLATFORM_WII
    mControllerSpeaker  = 0;
    mBiquadActive       = false;
    mBiquadB0           = 0;
    mBiquadB1           = 0;
    mBiquadB2           = 0;
    mBiquadA1           = 0;
    mBiquadA2           = 0;
    mLPFCutoff          = -1;
    #endif

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00943A70
FMOD_RESULT ChannelReal::setPaused(bool paused)
{
    if (paused)
    {
        mFlags = (CHANNELREAL_FLAG)(mFlags | CHANNELREAL_FLAG_PAUSED);
    }
    else
    {
        mFlags = (CHANNELREAL_FLAG)(mFlags & ~CHANNELREAL_FLAG_PAUSED);
    }

    return FMOD_OK;
}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00943A90
FMOD_RESULT ChannelReal::getPaused(bool *paused)
{
    if (!paused)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *paused = mFlags & CHANNELREAL_FLAG_PAUSED ? true : false;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelReal::setVolume(float volume)
{
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelReal::setFrequency(float frequency)
{
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelReal::setPan(float pan, float fbpan)
{
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelReal::setDSPClockDelay()
{
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00943AE0
FMOD_RESULT ChannelReal::setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright)
{
    int numsubchannels = 0;

    if (mDSP)
    {
        return FMOD_OK;     /* Only ChannelSoftware supports DSP. */
    }

    if (mSound)
    {
        if (mSound->mSubSampleParent)
        {
            numsubchannels = mSound->mSubSampleParent->mChannels;
        }
        else
        {
            numsubchannels = mSound->mChannels;
        }
    }

    if (numsubchannels > 1)
    {
        if (mSound->mDefaultChannelMask & SPEAKER_ALLMONO)
        {
            setPan(0.0f, 1.0f);
            setVolume(center * mParent->mVolume);
        }
        else if (mSound->mDefaultChannelMask & SPEAKER_ALLSTEREO)
        {
            switch (mSubChannelIndex % 2)
            {
                case 0:
                {
                    setPan(-1.0f, 1.0f);
                    setVolume(frontleft * mParent->mVolume);
                    break;
                }
                case 1:
                {
                    setPan(1.0f, 1.0f);
                    setVolume(frontright * mParent->mVolume);
                    break;
                }
            }
        }
        else
        {
            switch (mSubChannelIndex)
            {
                case 0:
                {
                    setPan(-1.0f, 1.0f);
                    setVolume(frontleft * mParent->mVolume);
                    break;
                }
                case 1:
                {
                    setPan(1.0f, 1.0f);
                    setVolume(frontright * mParent->mVolume);
                    break;
                }
                case 2:
                {
                    setPan(0.0f, 1.0f);
                    setVolume(center * mParent->mVolume);
                    break;
                }
                case 3:
                {
                    setPan(0.0f, 0.0f);
                    setVolume(lfe * mParent->mVolume);
                    break;
                }
                case 4:
                {
                    setPan(-1.0f, -1.0f);
                    setVolume(backleft * mParent->mVolume);
                    break;
                }
                case 5:
                {
                    setPan(1.0f, -1.0f);
                    setVolume(backright * mParent->mVolume);
                    break;
                }
                case 6:
                {
                    setPan(-1.0f, 0.0f);
                    setVolume(sideleft * mParent->mVolume);
                    break;
                }
                case 7:
                {
                    setPan(1.0f, 0.0f);
                    setVolume(sideright * mParent->mVolume);
                    break;
                }
            }
        }
    }
    else
    {
        float vol = 0, lrpan = 0, fbpan = 0;

        vol += frontleft;
        vol += frontright;
        vol += center;
        vol += lfe;
        vol += backleft;
        vol += backright;
        vol += sideleft;
        vol += sideright;

        #if 0

        float sqrtsumsq;
        /*
            Normalise
        */
        sqrtsumsq = frontleft*frontleft + frontright*frontright + backleft*backleft + backright*backright + sideleft*sideleft + sideright*sideright;
        sqrtsumsq = FMOD_SQRT(sqrtsumsq);

        lrpan -= frontleft  / sqrtsumsq;
        lrpan += frontright / sqrtsumsq;
        lrpan -= backleft   / sqrtsumsq;
        lrpan += backright  / sqrtsumsq;
        lrpan -= sideleft   / sqrtsumsq;
        lrpan += sideright  / sqrtsumsq;
        
        /*
            Normalise
        */
        sqrtsumsq = frontleft*frontleft + frontright*frontright + backleft*backleft + backright*backright + center*center;
        sqrtsumsq = FMOD_SQRT(sqrtsumsq);

        fbpan += frontleft  / sqrtsumsq;
        fbpan += frontright / sqrtsumsq;
        fbpan += center     / sqrtsumsq;
        fbpan -= backleft   / sqrtsumsq;
        fbpan -= backright  / sqrtsumsq;

        #else
        
        lrpan -= frontleft;
        lrpan += frontright;
        lrpan -= backleft;
        lrpan += backright;
        lrpan -= sideleft;
        lrpan += sideright;

        fbpan += frontleft;
        fbpan += frontright;
        fbpan += center;
        fbpan -= backleft;
        fbpan -= backright;

        #endif

        vol = vol > 1.0f ? 1.0f : vol;
        setVolume(vol * mParent->mVolume);
        setPan(lrpan < -1.0f ? -1.0f : lrpan > 1.0f ? 1.0f : lrpan, 
               fbpan < -1.0f ? -1.0f : fbpan > 1.0f ? 1.0f : fbpan);
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00943DA0
FMOD_RESULT ChannelReal::setSpeakerLevels(int speaker, float *levels, int numlevels)
{
    if (!mParent)
    {
        return FMOD_OK;
    }

    if (!mParent->mLevels)
    {
        mSystem->mSpeakerLevelsPool.alloc(&mParent->mLevels);
        if (!mParent->mLevels)
        {
            return FMOD_ERR_MEMORY;
        }
    }

    for (int count = 0; count < numlevels; count++)
    {
        float volume = levels[count];

        if (volume < 0)
        {
            volume = 0;
        }
        if (volume > 1.0f)
        {
            volume = 1.0f;
        }

        mParent->mLevels[(speaker * mSystem->mMaxInputChannels) + count] = volume;
    }


    return updateSpeakerLevels(mParent->mVolume);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00943DC0
FMOD_RESULT ChannelReal::updateSpeakerLevels(float volume)
{
    if (mParent && mParent->mLevels)
    {
        int count;
        float vol = 0, pan = 0, fbpan = 0;
        float sumsq = 0.0f, sqrtsumsq;
        int outputchannels = mSystem->mMaxOutputChannels;

        if (mParent->mSpeakerMode == FMOD_SPEAKERMODE_PROLOGIC)
        {
            outputchannels = 6;
        }

        for (count = 0; count < outputchannels; count++)
        {
            if (count == FMOD_SPEAKER_FRONT_CENTER || count == FMOD_SPEAKER_LOW_FREQUENCY)
            {
                continue;
            }

            float val = FMOD_FABS(mParent->mLevels[(count * mSystem->mMaxInputChannels) + mSubChannelIndex]);

            sumsq += val*val;
        }

        sqrtsumsq = FMOD_SQRT(sumsq);
        vol       = sqrtsumsq;

        for (count = 0; count < outputchannels; count++)
        {
            float val = FMOD_FABS(mParent->mLevels[(count * mSystem->mMaxInputChannels) + mSubChannelIndex]);
            
            if (sqrtsumsq)
            {
                val /= sqrtsumsq;
            }
            else
            {
                val = 0;
            }

            if (count == FMOD_SPEAKER_FRONT_LEFT || count == FMOD_SPEAKER_BACK_LEFT || count == FMOD_SPEAKER_SIDE_LEFT)
            {
                pan -= val;
            }
            else if (count == FMOD_SPEAKER_FRONT_RIGHT || count == FMOD_SPEAKER_BACK_RIGHT || count == FMOD_SPEAKER_SIDE_RIGHT)
            {
                pan += val;
            }

            if (count == FMOD_SPEAKER_FRONT_LEFT || count == FMOD_SPEAKER_FRONT_RIGHT )
            {
                fbpan += val;
            }
            else if (count == FMOD_SPEAKER_BACK_LEFT || count == FMOD_SPEAKER_BACK_RIGHT)
            {
                fbpan -= val;
            }
        }
    
        vol = vol > 1.0f ? 1.0f : vol;
        setVolume(vol * volume);
        setPan(pan < -1.0f ? -1.0f : pan > 1.0f ? 1.0f : pan, fbpan < -1.0f ? -1.0f : fbpan > 1.0f ? 1.0f : fbpan);
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x009442D0
FMOD_RESULT ChannelReal::setPosition(unsigned int position, FMOD_TIMEUNIT postype)
{
    if (postype != FMOD_TIMEUNIT_MS && postype != FMOD_TIMEUNIT_PCM && postype != FMOD_TIMEUNIT_PCMBYTES)
    {
        return FMOD_ERR_FORMAT;
    }
        
    if (mSound)
    {
        FMOD_RESULT     result;
        unsigned int    pcm = 0, lengthpcm;

        result = mSound->getLength(&lengthpcm, FMOD_TIMEUNIT_PCM);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (postype == FMOD_TIMEUNIT_PCM)
        {
            pcm = position;
        }
        else if (postype == FMOD_TIMEUNIT_PCMBYTES)
        {
            SoundI::getSamplesFromBytes(position, &pcm, mSound->mChannels, mSound->mFormat);
        }
        else if (postype == FMOD_TIMEUNIT_MS)
        {
            pcm = (unsigned int)((float)position / 1000.0f * mSound->mDefaultFrequency);
        }
        
        if (pcm >= lengthpcm)
        {
            pcm = lengthpcm;
        }

        mPosition = pcm;
    }


    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x009444A0
FMOD_RESULT ChannelReal::getPosition(unsigned int *position, FMOD_TIMEUNIT postype)
{   
    bool getsubsoundtime = false;

    if (!position)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mSound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    postype = (FMOD_TIMEUNIT)(postype & ~FMOD_TIMEUNIT_BUFFERED); /* Not a stream, so buffering is irrelevant. */

    /*
        First check for sentence stuff.
    */
    if (postype == FMOD_TIMEUNIT_SENTENCE_MS)
    {
        postype = FMOD_TIMEUNIT_MS;
        getsubsoundtime = true;
    }
    else if (postype == FMOD_TIMEUNIT_SENTENCE_PCM)
    {
        postype = FMOD_TIMEUNIT_PCM;
        getsubsoundtime = true;
    }
    else if (postype == FMOD_TIMEUNIT_SENTENCE_PCMBYTES)
    {
        postype = FMOD_TIMEUNIT_PCMBYTES;
        getsubsoundtime = true;
    }
    else if (postype == FMOD_TIMEUNIT_SENTENCE || postype == FMOD_TIMEUNIT_SENTENCE_SUBSOUND)
    {
        getsubsoundtime = true;
    }

    if (getsubsoundtime
#ifdef FMOD_SUPPORT_SENTENCING
     && !mSound->mSubSoundList
#endif
        )
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (postype == FMOD_TIMEUNIT_MS || postype == FMOD_TIMEUNIT_PCM || postype == FMOD_TIMEUNIT_PCMBYTES || postype == FMOD_TIMEUNIT_SENTENCE || postype == FMOD_TIMEUNIT_SENTENCE_SUBSOUND)
    {
        int          currentsubsoundid = 0;
        int          currentsentenceid = 0;
        unsigned int pcmcurrent = mPosition;

#ifdef FMOD_SUPPORT_SENTENCING
        if (getsubsoundtime)
        {
            for (currentsubsoundid = 0; currentsubsoundid < mSound->mSubSoundListNum; currentsubsoundid++)
            {
                SoundI *sound = mSound->mSubSound[mSound->mSubSoundList[currentsubsoundid].mIndex];

                if (sound && pcmcurrent >= sound->mLength)
                {
                    pcmcurrent -= sound->mLength;
                    currentsentenceid++;
                }
                else
                {
                    break;
                }
            }
        }
#endif

        if (postype == FMOD_TIMEUNIT_SENTENCE)
        {
            *position = currentsentenceid;
        }
        else if (postype == FMOD_TIMEUNIT_SENTENCE_SUBSOUND)
        {
            *position = currentsubsoundid;
        }
        else if (postype == FMOD_TIMEUNIT_PCM)
        {
            *position = pcmcurrent;
        }
        else if (postype == FMOD_TIMEUNIT_PCMBYTES)
        {
            SoundI::getBytesFromSamples(pcmcurrent, position, mSound->mChannels, mSound->mFormat);
        }
        else if (postype == FMOD_TIMEUNIT_MS)
        {
            *position = (unsigned int)((float)pcmcurrent / mSound->mDefaultFrequency * 1000.0f);
        }
    }
    else
    {
        return FMOD_ERR_FORMAT;
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00943FF0
FMOD_RESULT ChannelReal::setLoopPoints(unsigned int loopstart, unsigned int looplength)
{
    if (!mSound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (loopstart >= mSound->mLength)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (loopstart + looplength > mSound->mLength)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mLoopStart = loopstart;
    mLoopLength = looplength;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
// OFFSET: 0x00944040
FMOD_RESULT ChannelReal::setLoopCount(int loopcount)
{
    mLoopCount = loopcount;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelReal::setLowPassGain(float gain)
{
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelReal::set3DAttributes()
{
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelReal::set3DMinMaxDistance()
{
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00944050
FMOD_RESULT ChannelReal::set3DOcclusion(float directOcclusion, float reverbOcclusion)
{
    if (!mParent)
    {
        return FMOD_OK;
    }

    return setVolume(mParent->mVolume);
}



/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00944070
FMOD_RESULT ChannelReal::setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES *prop)
{
#ifndef FMOD_STATICFORPLUGINS
    if (!prop)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    /*
        Set the reverb's channel properties and update the reverb mix
    */
    if (!mParent)
    {
        return FMOD_OK;
    }

    FMOD_RESULT result;
    int instance, numinstances = 0;
    for (instance = 0; instance < FMOD_REVERB_MAXINSTANCES; instance++)
    {
        if (prop->Flags & (FMOD_REVERB_CHANNELFLAGS_INSTANCE0 << instance))
        {
            numinstances++;
        }
    }

    for (instance = 0; instance < FMOD_REVERB_MAXINSTANCES; instance++)
    {
        if (prop->Flags & (FMOD_REVERB_CHANNELFLAGS_INSTANCE0 << instance) || (!numinstances && !instance))
        {
            result = mSystem->mReverbGlobal.setChanProperties(instance, mParent->mIndex, prop);
            if (numinstances <= 1 && result != FMOD_OK)
            {
                return result;
            }
        }
        else
        {
            // update direct mix on other instances (ignoring errors)
            FMOD_REVERB_CHANNELPROPERTIES cprop;
            result = mSystem->mReverbGlobal.getChanProperties(instance, mParent->mIndex, &cprop);
            cprop.Direct = prop->Direct;
            result = mSystem->mReverbGlobal.setChanProperties(instance, mParent->mIndex, &cprop);
        }
    }

#endif

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x009440B0
FMOD_RESULT ChannelReal::getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES *prop)
{
#ifndef FMOD_STATICFORPLUGINS

    if(!prop)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mParent)
    {
        return FMOD_OK;
    }

    int instance = (prop->Flags & FMOD_REVERB_CHANNELFLAGS_INSTANCE1) ? 1 : 
                   (prop->Flags & FMOD_REVERB_CHANNELFLAGS_INSTANCE2) ? 2 : 
                   (prop->Flags & FMOD_REVERB_CHANNELFLAGS_INSTANCE3) ? 3 : 
                   0;

    return mSystem->mReverbGlobal.getChanProperties(instance, mParent->mIndex, prop, 0);

#endif

    return FMOD_OK;
}
/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x009440F0
FMOD_RESULT ChannelReal::isPlaying(bool *isplaying, bool includethreadlatency)
{
    if (!isplaying)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mFlags & CHANNELREAL_FLAG_PLAYING || mFlags & CHANNELREAL_FLAG_ALLOCATED)
    {
        *isplaying = true;
    }
    else
    {
        *isplaying = false;
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00944130
FMOD_RESULT ChannelReal::isVirtual(bool *isvirtual)
{
    if (!isvirtual)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *isvirtual = false;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00944150
FMOD_RESULT ChannelReal::getSpectrum(float *spectrumarray, int numentries, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
    return FMOD_ERR_NEEDSSOFTWARE;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00944160
FMOD_RESULT ChannelReal::getWaveData(float *wavearray, int numvalues, int channeloffset)
{
    return FMOD_ERR_NEEDSSOFTWARE;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00944170
FMOD_RESULT ChannelReal::getDSPHead(DSPI **dsp)
{   
    *dsp = 0;

    return FMOD_ERR_NEEDSSOFTWARE;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00944190
FMOD_RESULT ChannelReal::setMode(FMOD_MODE mode)
{
    /*
        Allow switching between loop modes.
    */
    if (mode & (FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI))
    {
        mMode = mMode & ~(FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI);

        if (mode & FMOD_LOOP_OFF)
        {
            mMode |= FMOD_LOOP_OFF;
        }
        else if (mode & FMOD_LOOP_NORMAL)
        {
            mMode |= FMOD_LOOP_NORMAL;
        }
        else if (mode & FMOD_LOOP_BIDI)
        {
            mMode |= FMOD_LOOP_BIDI;
        }
    }

    /*
        Allow switching between head relative and world relative.
    */
    if (mode & FMOD_3D_HEADRELATIVE)
    {
        mMode &= ~FMOD_3D_WORLDRELATIVE;
        mMode |=  FMOD_3D_HEADRELATIVE;
    }
    else if (mode & FMOD_3D_WORLDRELATIVE)
    {
        mMode &= ~FMOD_3D_HEADRELATIVE;
        mMode |=  FMOD_3D_WORLDRELATIVE;
    }

    /*
        Allow switching between linear/log/custom rolloff.
    */
    if (mode & FMOD_3D_LOGROLLOFF)
    {
        mMode &= ~FMOD_3D_LINEARROLLOFF;
        mMode &= ~FMOD_3D_CUSTOMROLLOFF;
        mMode |=  FMOD_3D_LOGROLLOFF;
    }
    else if (mode & FMOD_3D_LINEARROLLOFF)
    {
        mMode &= ~FMOD_3D_LOGROLLOFF;
        mMode &= ~FMOD_3D_CUSTOMROLLOFF;
        mMode |=  FMOD_3D_LINEARROLLOFF;
    }
    else if (mode & FMOD_3D_CUSTOMROLLOFF)
    {
        mMode &= ~FMOD_3D_LOGROLLOFF;
        mMode &= ~FMOD_3D_LINEARROLLOFF;
        mMode |=  FMOD_3D_CUSTOMROLLOFF;
    }

    if (mode & FMOD_3D_IGNOREGEOMETRY)
    {
        mMode |= FMOD_3D_IGNOREGEOMETRY;
    }
    else
    {
        mMode &= ~FMOD_3D_IGNOREGEOMETRY;
    }

    if (mode & FMOD_VIRTUAL_PLAYFROMSTART)
    {
        mMode |= FMOD_VIRTUAL_PLAYFROMSTART;
    }
    else
    {
        mMode &= ~FMOD_VIRTUAL_PLAYFROMSTART;
    }

    /*
        Allow switching between 2D and 3D only in software.
    */
#if !defined( PLATFORM_GC ) && !defined( PLATFORM_WII ) && !defined( PLATFORM_PSP ) && !defined( PLATFORM_PS2 )
    if (!(mMode & FMOD_HARDWARE))
#endif
    {
        if (mode & FMOD_2D)
        {
            if (!mParent)
            {
                return FMOD_OK;
            }

            mMode &= ~FMOD_3D;
            mMode |=  FMOD_2D;
            mParent->mVolume3D        = 1.0f;
            mParent->mDirectOcclusion = 0.0f;
            mParent->mReverbDryVolume = 1.0f;
            mParent->mConeVolume3D    = 1.0f;
            mParent->mPitch3D         = 1.0f;
        }
        else if (mode & FMOD_3D)
        {
            mMode &= ~FMOD_2D;
            mMode |=  FMOD_3D;
        }
    }

    return FMOD_OK;
}


#ifdef PLATFORM_WII
/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT  ChannelReal::setLowPassFilter(int cutoff)
{
    mLPFCutoff = cutoff;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT  ChannelReal::getLowPassFilter(int *cutoff)
{
    *cutoff = mLPFCutoff;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT  ChannelReal::setBiquadFilter(bool active, unsigned short b0, unsigned short b1, unsigned short b2, unsigned short a1, unsigned short a2)
{
    mBiquadActive = active;
    mBiquadB0     = b0;
    mBiquadB1     = b1;
    mBiquadB2     = b2;
    mBiquadA1     = a1;
    mBiquadA2     = a2;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT  ChannelReal::getBiquadFilter(bool *active, unsigned short *b0, unsigned short *b1, unsigned short *b2, unsigned short *a1, unsigned short *a2)
{
    *active = mBiquadActive;
    *b0     = mBiquadB0; 
    *b1     = mBiquadB1;
    *b2     = mBiquadB2;
    *a1     = mBiquadA1;
    *a2     = mBiquadA2;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT  ChannelReal::setControllerSpeaker(unsigned int controllerspeaker, int subchannel)
{
    mControllerSpeaker = controllerspeaker;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT  ChannelReal::getControllerSpeaker(unsigned int *controllerspeaker)
{
    *controllerspeaker = mControllerSpeaker;

    return FMOD_OK;
}
#endif

}


