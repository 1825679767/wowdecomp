#include "fmod_settings.h"

#include "fmod_channeli.h"
#include "fmod_channel_emulated.h"
#include "fmod_debug.h"
#include "fmod_dspi.h"
#include "fmod_sound_stream.h"
#include "fmod_soundi.h"
#include "fmod_systemi.h"
#include "fmod_os_misc.h"

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
// OFFSET: 0x00944A70
ChannelEmulated::ChannelEmulated()
{
#ifdef FMOD_SUPPORT_SOFTWARE
    mDSPHead = 0;
#endif
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x00944AB0
FMOD_RESULT ChannelEmulated::init(int index, SystemI *system, Output *output, DSPI *dspmixtarget)
{
    ChannelReal::init(index, system, output, dspmixtarget);
  
#ifdef FMOD_SUPPORT_SOFTWARE
    if (!(mSystem->mFlags & FMOD_INIT_SOFTWARE_DISABLE))
    {
        FMOD_RESULT result;
        FMOD_DSP_DESCRIPTION_EX descriptionex;

        /*
            Create a head unit that things can connect to.
        */
        FMOD_memset(&descriptionex, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));
        FMOD_strcpy(descriptionex.name, "EmulatedChannel DSPHead Unit");
        descriptionex.version   = 0x00010100;
        descriptionex.mCategory = FMOD_DSP_CATEGORY_FILTER;
        descriptionex.mFormat   = FMOD_SOUND_FORMAT_PCMFLOAT;

        #ifdef PLATFORM_PS3
        mDSPHead = (DSPI *)FMOD_ALIGNPOINTER(&mDSPHeadMemory, 16);
        mDSPHead = new (mDSPHead) (DSPFilter);         
        #else
        mDSPHead = &mDSPHeadMemory;
        #endif

        result = mSystem->createDSP(&descriptionex, &mDSPHead, false);
        if (result != FMOD_OK)
        {
            return result;
        }

        #ifdef PLATFORM_PS3
        mDSPHead->mMramAddress       = (unsigned int)mDSPHead;
        mDSPHead->mDescription.mSize = (sizeof(DSPFilter) + 15) & ~15;
        #endif

        mMinFrequency = -mMaxFrequency;
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
// OFFSET: 0x009447D0
FMOD_RESULT ChannelEmulated::alloc()
{
    FMOD_RESULT result;

    result = ChannelReal::alloc();
    if (result != FMOD_OK)
    {
        return result;
    }

#ifdef FMOD_SUPPORT_SOFTWARE
    if (mDSPHead)
    {
        result = mDSPHead->disconnectFrom(0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mParent->mChannelGroup->mDSPMixTarget->addInputQueued(mDSPHead, false, 0, 0);
        if (result != FMOD_OK)
        {
            return result;
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
// OFFSET: 0x00944820
FMOD_RESULT ChannelEmulated::stop()
{  
#ifdef FMOD_SUPPORT_STREAMING
    if (mSound)
    {
        if (mSound->isStream())
        {
            Stream *stream = SAFE_CAST(Stream, mSound);
            if (stream->mChannel)
            {
                stream->mChannel->mFinished = true;
            }
        }
    }
#endif

#ifdef FMOD_SUPPORT_SOFTWARE
    if (mDSPHead)
    {
        mDSPHead->setActive(false);
        mDSPHead->disconnectAll(false, true);
    }
#endif

#ifdef FMOD_SUPPORT_REVERB
    if (mParent)
    {
        int instance;  

        for (instance = 0; instance < FMOD_REVERB_MAXINSTANCES; instance++)
        {
            mSystem->mReverbGlobal.resetConnectionPointer(instance, mParent->mIndex);
        }

#ifdef FMOD_SUPPORT_MULTIREVERB
        {
            ReverbI     *reverb_c;

            mSystem->mReverb3D.resetConnectionPointer(0, mParent->mIndex);

            reverb_c = SAFE_CAST(ReverbI, mSystem->mReverb3DHead.getNext());
            while (reverb_c != &(mSystem->mReverb3DHead))
            {      
                reverb_c->resetConnectionPointer(0, mParent->mIndex);
                reverb_c = SAFE_CAST(ReverbI, reverb_c->getNext());
            }
        }
#endif
    }
#endif

    return ChannelReal::stop();
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
// OFFSET: 0x009448C0
FMOD_RESULT ChannelEmulated::close()
{
    FMOD_RESULT result;

    result = ChannelReal::close();
    if (result != FMOD_OK)
    {
        return result;
    }

#ifdef FMOD_SUPPORT_SOFTWARE
    if (mDSPHead)
    {
        mDSPHead->release(false);       /* false = dont free this, as it is not alloced. */
        mDSPHead= 0;
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
// OFFSET: 0x009448F0
FMOD_RESULT ChannelEmulated::update(int delta)
{
    FMOD_RESULT result;

    result = ChannelReal::update(delta);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mFlags & CHANNELREAL_FLAG_PAUSED || !(mFlags & CHANNELREAL_FLAG_PLAYING))
    {
        return FMOD_OK;
    }

    if ((mParent->mFlags & CHANNELI_FLAG_JUSTWENTVIRTUAL && !(mParent->mFlags & CHANNELI_FLAG_FORCEVIRTUAL)) || mMode & FMOD_VIRTUAL_PLAYFROMSTART)
    {
        return FMOD_OK;
    }
    
    if (mSystem->mDSPClock.mValue < mParent->mDSPClockDelay.mValue)
    {
        return FMOD_OK;
    }


    /*
        Convert from ms to samples
    */
    delta = delta * (int)(mParent->mFrequency * mParent->mPitch3D) / 1000;

    if (mDirection == CHANNELREAL_PLAYDIR_BACKWARDS)
    {
        delta = -delta;
    }

    if ((int)mPosition + delta > 0)
    {
        mPosition += delta;
    }
    else
    {
        mPosition = 0;
    }

    if (mSound)
    {
        SoundI *soundi = SAFE_CAST(SoundI, mSound);

        if (mMode & FMOD_LOOP_NORMAL || mMode & FMOD_LOOP_BIDI && mLoopCount)
        {
            while ((mParent->mFrequency > 0 && mPosition >= mLoopStart + mLoopLength) ||
                   (mParent->mFrequency < 0 && mPosition <= mLoopStart))
            {
                if (!mLoopCount)
                {
                    if (mDirection == CHANNELREAL_PLAYDIR_FORWARDS && mParent->mFrequency > 0)
                    {
                        mPosition = mLoopStart + mLoopLength;
                    }
                    else
                    {
                        mPosition = mLoopStart;
                    }

                    mFlags = (CHANNELREAL_FLAG)(mFlags & ~CHANNELREAL_FLAG_PLAYING);
                    break;
                }
                else if (mMode & FMOD_LOOP_NORMAL)
                {
                    if (mParent->mFrequency > 0)
                    {
                        mPosition -= mLoopLength;
                    }
                    else
                    {
                        mPosition += mLoopLength;
                    }
                }
                else if (mMode & FMOD_LOOP_BIDI)
                {
                    if (mDirection == CHANNELREAL_PLAYDIR_FORWARDS)
                    {
                        mDirection = CHANNELREAL_PLAYDIR_BACKWARDS;
                    }
                    else
                    {
                        mDirection = CHANNELREAL_PLAYDIR_FORWARDS;
                    }
                    mPosition += -delta;
                }

                if (mLoopCount >= 0)
                {
                    mLoopCount--;
                }
            }
        }
        else
        {
            if (mPosition >= soundi->mLength)
            {
                mPosition = soundi->mLength;

                mFlags = (CHANNELREAL_FLAG)(mFlags & ~CHANNELREAL_FLAG_PLAYING);
            }
        }

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
// OFFSET: 0x00944A20
FMOD_RESULT ChannelEmulated::isVirtual(bool *isvirtual)
{
    if (!isvirtual)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *isvirtual = true;

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
// OFFSET: 0x00944A40
FMOD_RESULT ChannelEmulated::getDSPHead(DSPI **dsp)
{
    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

#ifdef FMOD_SUPPORT_SOFTWARE
    *dsp = mDSPHead;
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
FMOD_RESULT ChannelEmulated::setSpeakerLevels(int speaker, float *levels, int numlevels)
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
FMOD_RESULT ChannelEmulated::setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright)
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
FMOD_RESULT ChannelEmulated::moveChannelGroup(ChannelGroupI *oldchannelgroup, ChannelGroupI *newchannelgroup)
{
#ifdef FMOD_SUPPORT_SOFTWARE
    FMOD_RESULT result;

    if (oldchannelgroup == newchannelgroup || !mDSPHead)
    {
        return FMOD_OK;
    }

    /*
        1. disconnect from previous channel group's head.
    */
    if (oldchannelgroup && oldchannelgroup->mDSPMixTarget)
    {
        result = oldchannelgroup->mDSPMixTarget->disconnectFrom(mDSPHead); 
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        2. reconnect to DSP group's head.
    */
    result = newchannelgroup->mDSPMixTarget->addInputQueued(mDSPHead, false, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }
#endif

    return FMOD_OK;
}



}

