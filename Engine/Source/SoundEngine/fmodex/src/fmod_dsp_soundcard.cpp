#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_SOFTWARE

#include "fmod_downmix.h"
#include "fmod_dsp_filter.h"
#include "fmod_dsp_soundcard.h"
#include "fmod_soundi.h"
#include "fmod_systemi.h"

namespace FMOD
{


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
// OFFSET: 0x00929C20
FMOD_RESULT DSPSoundCard::alloc(FMOD_DSP_DESCRIPTION_EX *description)
{
    FMOD_RESULT result;

    result = DSPI::alloc(description);
    if (result != FMOD_OK)
    {
        return result;
    }

    updateTreeLevel(0); /* Soundcard units always start at level 0 */

    return FMOD_OK;
}



/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPSoundCard::read(void *outbuffer, unsigned int *length, FMOD_SPEAKERMODE speakermode, int speakermodechannels, unsigned int tick)
{
    FMOD_RESULT   result       = FMOD_OK;
    float        *readbuffer   = 0;
    int           outchannels  = 0;
	unsigned int  starttime    = 0;
	unsigned int  endtime      = 0;

    result = DSPFilter::read(&readbuffer, &outchannels, length, speakermode, speakermodechannels, tick);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mSystem->mFlags & FMOD_INIT_ENABLE_PROFILE)
    {   
        FMOD_OS_Time_GetNs(&starttime);
    }

    #if !defined(PLATFORM_PS3)
    if (mSystem->mDownmix)
    {
        mSystem->mDownmix->getOutputChannels(&outchannels);
    }
    else
    #endif
    {
        outchannels = speakermodechannels;
    }

    if (mFlags & FMOD_DSP_FLAG_IDLE)
    {
        unsigned int lenbytes;

        SoundI::getBytesFromSamples(*length, &lenbytes, outchannels, mDescription.mFormat);

        memset(outbuffer, 0, lenbytes);

#if defined(FMOD_SUPPORT_PROFILE_DSP_VOLUMELEVELS) && !defined(PLATFORM_PS3)
        if (mSystem->mFlags & FMOD_INIT_ENABLE_PROFILE)
        {  
            mNumPeakVolumeChans = outchannels;
            memset(mPeakVolume, 0, sizeof(mPeakVolume));
        }
#endif
    }
    else
    {
        float *src;
        bool convertformat = (mDescription.mFormat != FMOD_SOUND_FORMAT_PCMFLOAT);

        #if !defined(PLATFORM_PS3)
        if (mSystem->mDownmix && this == mSystem->mDSPSoundCard)
        {
            src = convertformat ? mSystem->mDSPTempBuff : (float *)outbuffer;
            mSystem->mDownmix->encode(readbuffer, src, *length);
        }
        else
        #endif
        {
            src = readbuffer;
        }

#if defined(FMOD_SUPPORT_PROFILE_DSP_VOLUMELEVELS) && !defined(PLATFORM_PS3)
        if (mSystem->mFlags & FMOD_INIT_ENABLE_PROFILE)
        {  
            calculatePeaks(src, *length, outchannels);
        }
#endif

        /*
            mBuffer being true means the format is not the same so it needs to read as float first, then convert to the soundcard format.
        */
        if (convertformat)
        {
            result = convert(outbuffer, src, mDescription.mFormat, FMOD_SOUND_FORMAT_PCMFLOAT, *length * outchannels, 1, 1, 1.0f);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else if (src != outbuffer)
        {
            unsigned int lenbytes;

            SoundI::getBytesFromSamples(*length, &lenbytes, outchannels, mDescription.mFormat);

            memcpy(outbuffer, src, lenbytes);
        }
    }

    mDSPTick = tick;

    if (mSystem->mFlags & FMOD_INIT_ENABLE_PROFILE)
    {  
	    FMOD_OS_Time_GetNs(&endtime);
        mCPUUsageTemp += endtime - starttime;

        mCPUUsage = mCPUUsageTemp;
        mCPUUsageTemp = 0;
    }

    return FMOD_OK;
}

}

#endif
