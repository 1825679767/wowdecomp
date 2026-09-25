#include "fmod_settings.h"

#include "fmod_3d.h"
#include "fmod_channeli.h"
#include "fmod_channel_real.h"
#include "fmod_codeci.h"
#include "fmod_dspi.h"
#include "fmod_dsp_filter.h"
#include "fmod_output.h"
#include "fmod_output_emulated.h"
#include "fmod_soundi.h"
#include "fmod_sound_sample.h"
#include "fmod_speakermap.h"
#include "fmod_speakerlevels_pool.h"
#include "fmod_systemi.h"

namespace FMOD
{


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    Channel handle are composed like so :

    S = system id       = SystemI::mId
    I = channel index   = index into SystemI::mChannel
    C = reference count

    SSSSIIII IIIIIIII CCCCCCCC CCCCCCCC

	[SEE_ALSO]
]
*/
// OFFSET: 0x008DCD60
FMOD_RESULT F_API ChannelI::validate(Channel *channel, ChannelI **channeli)
{
    unsigned int    handle   = (unsigned int)((FMOD_UINT_NATIVE)channel);
    unsigned int    sysindex = (handle >> SYSTEMID_SHIFT)  & SYSTEMID_MASK;
    unsigned int    index    = (handle >> CHANINDEX_SHIFT) & CHANINDEX_MASK;
    unsigned int    refcount = (handle >> REFCOUNT_SHIFT)  & REFCOUNT_MASK;
    ChannelI       *tmpchanneli;
    SystemI        *sys;
    FMOD_RESULT     result;

    if (!channeli)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *channeli = 0;

    result = SystemI::getInstance(sysindex, &sys);
    if (result != FMOD_OK)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }
    
    if (!sys->mChannel)
    {
        return FMOD_ERR_UNINITIALIZED;
    }

    if ((int)index >= sys->mNumChannels)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    tmpchanneli = &sys->mChannel[index];

    /*
        refcount == 0 means it's an absolute channel handle so it's always valid
    */
    if (refcount && (tmpchanneli->mHandleCurrent != handle))
    {
        /*
            If the reference count is greater by two or more (it's incremented on every play and every stop)
            then the channel has been reused i.e. stolen
        */
        if ((((tmpchanneli->mHandleCurrent >> REFCOUNT_SHIFT) & REFCOUNT_MASK) - refcount) >= 2)
        {
            return FMOD_ERR_CHANNEL_STOLEN;
        }
        else
        {
            return FMOD_ERR_INVALID_HANDLE;
        }
    }

    *channeli = tmpchanneli;

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
// OFFSET: 0x008DCE10
FMOD_RESULT ChannelI::returnToFreeList()
{
    SystemI *systemi;
    
    systemi = SAFE_CAST(SystemI, mSystem);
    if (!systemi)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    /*
        Just remove it from this list, dont add it anywhere else.
    */
    mSortedListNode.removeNode();
    mSoundGroupSortedListNode.removeNode();

    /*
        Remove it from used list, add it to free list.
    */
    removeNode();
    addAfter(&systemi->mChannelFreeListHead);

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
// OFFSET: 0x008DF300
FMOD_RESULT ChannelI::setDefaults()
{
    float realfrequency, realvolume, realpan;    
    int defaultpriority, channels;
    float defaultfrequency;
    float defaultvolume;
    float defaultpan;
    float frequencyvariation;
    float volumevariation;
    float panvariation;
    unsigned int channelmask;
    
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mRealChannel[0]->mSound)
    {
        SoundI *soundi = SAFE_CAST(SoundI, mRealChannel[0]->mSound);

        defaultpriority     = soundi->mDefaultPriority;
        defaultfrequency    = soundi->mDefaultFrequency;
        defaultvolume       = soundi->mDefaultVolume;
        defaultpan          = soundi->mDefaultPan;
        frequencyvariation  = soundi->mFrequencyVariation;
        volumevariation     = soundi->mVolumeVariation;
        panvariation        = soundi->mPanVariation;
        channels            = soundi->mChannels;
        channelmask         = soundi->mDefaultChannelMask;
    }
    else if (mRealChannel[0]->mDSP)
    {
        DSPI *dspi = SAFE_CAST(DSPI, mRealChannel[0]->mDSP);

        defaultpriority     = dspi->mDefaultPriority;
        defaultfrequency    = dspi->mDefaultFrequency;
        defaultvolume       = dspi->mDefaultVolume;
        defaultpan          = dspi->mDefaultPan;
        frequencyvariation  = 0;
        volumevariation     = 0;
        panvariation        = 0;
        channels            = 0;
        channelmask         = 0;
    }
    else
    {
        return FMOD_ERR_INTERNAL;
    }

    /*
        Virtual channel stuff
    */
    mPriority = defaultpriority;

    /*
        Now update the real channel's defaults.
    */
	mDirectOcclusion        = 0.0f;
	mDirectOcclusionTarget  = 0.0f;
	mUserDirectOcclusion    = 0.0f;
	mReverbOcclusion        = 0.0f;
	mReverbOcclusionTarget  = 0.0f;
	mUserReverbOcclusion    = 0.0f;
    mDirectOcclusionRateOfChange = 0.0f;
    mReverbOcclusionRateOfChange = 0.0f;

    realfrequency = defaultfrequency;
    realvolume    = defaultvolume;
    realpan       = defaultpan;

    if (frequencyvariation > 0)
    {
        float randval = ((float)(FMOD_RAND() % 32768) / 16384.0f) - 1.0f;    /* -1.0 to +1.0 */
        realfrequency += frequencyvariation * randval;
    }
    if (volumevariation > 0)
    {
        float randval = ((float)(FMOD_RAND() % 32768) / 16384.0f) - 1.0f;    /* -1.0 to +1.0 */
        realvolume += volumevariation * randval;
    }
    if (panvariation > 0)
    {
        float randval = ((float)(FMOD_RAND() % 32768) / 8192.0f) - 2.0f;     /* -2.0 to +2.0 */
        realpan += panvariation * randval;
    }

    setFrequency(realfrequency);
	setVolume(realvolume);

    if (channelmask & SPEAKER_WAVEFORMAT_MASK)
    {
        int speaker, incount;
        unsigned int maskbit = 1;
        float clevels[DSP_MAXLEVELS_IN];

        FMOD_memset(clevels, 0, DSP_MAXLEVELS_IN * sizeof(float));

        incount = 0;
        for (speaker = 0; speaker < channels; speaker++)
        {
            if (channelmask & maskbit)
            {               
                clevels[incount] = 1.0f;

                incount++;
            }

            maskbit <<= 1;
        }

        setSpeakerMix(clevels[0], clevels[1], clevels[2], clevels[3], clevels[4], clevels[5], clevels[6], clevels[7]);
    }
    else
    {
	    setPan(realpan);
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
// OFFSET: 0x008DCE80
FMOD_RESULT ChannelI::referenceStamp(bool newstamp)
{
    unsigned int sysindex, index, refcount;

    sysindex = (mHandleCurrent >> SYSTEMID_SHIFT)  & SYSTEMID_MASK;
    index    = (mHandleCurrent >> CHANINDEX_SHIFT) & CHANINDEX_MASK;
    refcount = ((newstamp ? mHandleCurrent : mHandleOriginal) >> REFCOUNT_SHIFT) & REFCOUNT_MASK;

    refcount++;
    if (refcount > REFCOUNT_MASK)
    {
        refcount = 1;
    }

    mHandleCurrent = (sysindex << SYSTEMID_SHIFT) | ((index << CHANINDEX_SHIFT) & (CHANINDEX_MASK << CHANINDEX_SHIFT)) | (refcount << REFCOUNT_SHIFT);

    if (newstamp)
    {
        mHandleOriginal = mHandleCurrent;
    }

    return FMOD_OK;
}



/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]
    This function takes 3 states.
    -1000     = Let the function calculate listpos
    0 to 1025 = Supply a listpos manually
    -1        = Supply a code for 'remove me'

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x008E05E0
FMOD_RESULT ChannelI::updatePosition()
{
    unsigned int oldlistposition, oldsglistposition;
    float audibility, audibilitysoundgroup;
    bool forcevirtual = false;
    float realdirecttransmission, realreverbtransmission;
    SoundI *sound = mRealChannel[0]->mSound;

    if (mFlags & CHANNELI_FLAG_PLAYINGPAUSED)
    {
        return FMOD_OK;
    }

    if (!mSystem)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    getAudibilityInternal(&audibilitysoundgroup, false);
    audibility = audibilitysoundgroup * mFadeVolume;

    realdirecttransmission = (1.0f - mDirectOcclusion) *
			                 (1.0f - mUserDirectOcclusion) *
                             mChannelGroup->mRealDirectOcclusionVolume;
    realreverbtransmission = (1.0f - mReverbOcclusion) *
			                 (1.0f - mUserReverbOcclusion) *
                             mChannelGroup->mRealReverbOcclusionVolume;

    if (audibility <= mSystem->mAdvancedSettings.vol0virtualvol && mSystem->mFlags & FMOD_INIT_VOL0_BECOMES_VIRTUAL)
    {
        forcevirtual = true;
    }
    if (realdirecttransmission == 0.0f && realreverbtransmission > 0.0f)    /* Even though dry is silent, there might be a bit of wet. */
    {
        forcevirtual = false;
    }
    if (mPriority == 0 && sound && sound->isStream())
    {
        forcevirtual = false;
    }

    forceVirtual(forcevirtual);

    oldlistposition = mListPosition;
    oldsglistposition = mSoundGroupListPosition;
    mSoundGroupListPosition = mListPosition = mPriority * (FMOD_MAXAUDIBIILITY + 1);                              // 0-1000, 1001-2001, 2002-3002, 3003-4003.
    mListPosition            += (FMOD_MAXAUDIBIILITY - (int)(audibility * FMOD_MAXAUDIBIILITY));                  // 0-1000
    mSoundGroupListPosition  += (FMOD_MAXAUDIBIILITY - (int)(audibilitysoundgroup * FMOD_MAXAUDIBIILITY));        // 0-1000    

    if (mListPosition != oldlistposition)
    {
        mSortedListNode.removeNode();
        mSortedListNode.addAt(&mSystem->mChannelSortedListHead, &mSystem->mChannelSortedListHead, mListPosition);
        mSortedListNode.setData(this);
    }

    if (sound && sound->mSoundGroup && mSoundGroupListPosition != oldsglistposition)
    {
        mSoundGroupSortedListNode.removeNode();
        mSoundGroupSortedListNode.addAt(&sound->mSoundGroup->mChannelListHead, &sound->mSoundGroup->mChannelListHead, mSoundGroupListPosition);
        mSoundGroupSortedListNode.setData(this);
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
// OFFSET: 0x008E0460
FMOD_RESULT ChannelI::forceVirtual(bool force)
{
    FMOD_RESULT result;

    if (force)
    {
        bool isvirtual;

        if (mFlags & CHANNELI_FLAG_FORCEVIRTUAL)
        {
            return FMOD_OK;
        }
        
        result = isVirtual(&isvirtual);
        if (result != FMOD_OK)
        {
            return result;
        }

        mFlags |= CHANNELI_FLAG_FORCEVIRTUAL;

        if (!isvirtual && mRealChannel[0])
        {
            FMOD_CHANNEL_INFO realinfo;
            ChannelReal *emuchannelreal;
            ChannelGroupI *channelgroup;
            bool playingpaused = (mFlags & CHANNELI_FLAG_PLAYINGPAUSED) ? true : false;

            result = mSystem->mEmulated->getFreeChannel(mRealChannel[0]->mMode, &emuchannelreal, 1, 1, 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            channelgroup = mChannelGroup;
            getChannelInfo(&realinfo);
            stopEx(CHANNELI_STOPFLAG_RESETCHANNELGROUP | CHANNELI_STOPFLAG_DONTFREELEVELS);

            /* 
                This channel is going from real to emulated, so just give it 1 subchannel and point it to 1 emulated voice 
            */
            mNumRealChannels = 1;
            mRealChannel[0] = emuchannelreal;

            if (realinfo.mSound)
            {
                result = play(realinfo.mSound->mSubSampleParent, true, false, false);
                setChannelGroup(channelgroup);
                setChannelInfo(&realinfo);
                
                mFlags &= ~CHANNELI_FLAG_PLAYINGPAUSED; /* Stop it from re-evaluating position in setpaused(false). */
                setPaused(realinfo.mPaused);
            }
            else if (realinfo.mDSP)
            {
                result = play(realinfo.mDSP, true, false, false);
                setChannelGroup(channelgroup);
                setChannelInfo(&realinfo);
                
                mFlags &= ~CHANNELI_FLAG_PLAYINGPAUSED; /* Stop it from re-evaluating position in setpaused(false). */
                setPaused(realinfo.mPaused);
            }
            if (playingpaused)
            {
                mFlags |= CHANNELI_FLAG_JUSTWENTVIRTUAL;
            }
        }
    }
    else
    {
        if (!(mFlags & CHANNELI_FLAG_FORCEVIRTUAL))
        {
            return FMOD_OK;
        }

        mFlags &= ~CHANNELI_FLAG_FORCEVIRTUAL;
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
// OFFSET: 0x008DFE20
FMOD_RESULT ChannelI::getChannelInfo(FMOD_CHANNEL_INFO *info)
{        
    info->mRealChannel      = mRealChannel[0];
    info->mLevels           = mLevels;
    info->mSound            = 0;
    info->mDSP              = 0;

    /*
        Other, not stored in ChannelI.
    */
    getMode            (&info->mMode);
    getPosition        (&info->mPCM, FMOD_TIMEUNIT_PCM);
    getLoopPoints      (&info->mLoopStart, FMOD_TIMEUNIT_PCM, &info->mLoopEnd, FMOD_TIMEUNIT_PCM);
    getCurrentSound    (&info->mSound);
    if (!info->mSound)
    {
        getCurrentDSP(&info->mDSP);
    }
    getLoopCount       (&info->mLoopCount);
    getMute            (&info->mMute);
    getPaused          (&info->mPaused);

#ifdef PLATFORM_WII
    getLowPassFilter    (&info->mLowPassCutoff);
    getBiquadFilter     (&info->mBiquadActive, &info->mBiquadB0, &info->mBiquadB1, &info->mBiquadB2, &info->mBiquadA1, &info->mBiquadA2);
    getControllerSpeaker(&info->mControllerSpeaker);
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
// OFFSET: 0x008DFFC0
FMOD_RESULT ChannelI::setChannelInfo(FMOD_CHANNEL_INFO *info)
{
    int count;                           
    
    setMode(info->mMode);

    /*                       
        2D                   
    */                       
    setVolume(mVolume);
    setFrequency(mFrequency);

    /*
        Set speaker levels around
    */
    {
        if (mLastPanMode == FMOD_CHANNEL_PANMODE_PAN)
        {
            setPan(mPan, true);
        }
        else if (mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERMIX)
        {
            setSpeakerMix(mSpeakerFL,  mSpeakerFR,  mSpeakerC,  mSpeakerLFE,  mSpeakerBL,  mSpeakerBR,  mSpeakerSL,  mSpeakerSR, true);
        }
        else if (mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERLEVELS)
        {
            if (mLevels && mLevels != info->mLevels)
            {                                         
                  mSystem->mSpeakerLevelsPool.free(mLevels);
            }

            mLevels = info->mLevels;
            if (mLevels)
            {
                for (count = 0; count < mSystem->mMaxOutputChannels; count++)
                {
                    setSpeakerLevels((FMOD_SPEAKER)count, &mLevels[count * mSystem->mMaxOutputChannels], mSystem->mMaxInputChannels, true);
                }
            }
        }
    }
    
    /*
        3D
    */

    set3DAttributes(&mPosition3D, &mVelocity3D);

    /*
        Other, not stored in ChannelI.
    */
    setDelay           (FMOD_DELAYTYPE_DSPCLOCK_START, mDSPClockDelay.mHi, mDSPClockDelay.mLo);
    setPosition        (info->mPCM, FMOD_TIMEUNIT_PCM);
    setLoopPoints      (info->mLoopStart, FMOD_TIMEUNIT_PCM, info->mLoopEnd, FMOD_TIMEUNIT_PCM);
    setLoopCount       (info->mLoopCount);
    setMute            (info->mMute);

    for (count = 0; count < FMOD_REVERB_MAXINSTANCES; count++)
    {
        FMOD_REVERB_CHANNELPROPERTIES prop;

        FMOD_memset(&prop, 0, sizeof(FMOD_REVERB_CHANNELPROPERTIES));
    
        prop.Flags |= (FMOD_REVERB_CHANNELFLAGS_INSTANCE0 << count);
    
        if (getReverbProperties(&prop) == FMOD_OK)
        {
            setReverbProperties(&prop);
        }
    }


#ifdef PLATFORM_WII
    setLowPassFilter    (info->mLowPassCutoff);
    setBiquadFilter     (info->mBiquadActive, info->mBiquadB0, info->mBiquadB1, info->mBiquadB2, info->mBiquadA1, info->mBiquadA2);
    setControllerSpeaker(info->mControllerSpeaker);
#endif

    if (mAddDSPHead)
    {
        FMOD::DSPI *dsphead;
        FMOD_RESULT result;

        result = getDSPHead(&dsphead);
        if (result == FMOD_OK)
        {
            dsphead->insertInputBetween(mAddDSPHead, 0, true, 0);
        }
    }

    if (mCallback)
    {
        bool isvirtual;

        isVirtual(&isvirtual);

        mCallback((FMOD_CHANNEL *)((FMOD_UINT_NATIVE)mHandleCurrent), FMOD_CHANNEL_CALLBACKTYPE_VIRTUALVOICE, isvirtual ? (void *)1 : 0, 0);
    }
    
    update(0);

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
// OFFSET: 0x008DCEE0
FMOD_RESULT ChannelI::getRealChannel(ChannelReal **realchan, int *subchannels)
{
#ifdef FMOD_SUPPORT_STREAMING
    if (mRealChannel[0]->isStream())
    {
        ChannelStream *channelstream = SAFE_CAST(ChannelStream, mRealChannel[0]);

        if (realchan)
        {
            int count;

            for (count = 0; count < channelstream->mNumRealChannels; count++)
            {
                realchan[count] = channelstream->mRealChannel[count];
            }
        }

        if (subchannels)
        {
            *subchannels = channelstream->mNumRealChannels;
        }
    }
    else
#endif
    {
        if (realchan)
        {
            int count;

            for (count = 0; count < mNumRealChannels; count++)
            {
                realchan[count] = mRealChannel[count];
            }
        }

        if (subchannels)
        {
            *subchannels = mNumRealChannels;
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
// OFFSET: 0x008DCF80
FMOD_RESULT ChannelI::calcVolumeAndPitchFor3D(int delta)
{
    float pitch3d = 1.0f, vol3d = 1.0f, conevol = 1.0f;

#if defined(FMOD_SUPPORT_3DSOUND) && !defined(FMOD_STATICFORPLUGINS)
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        if (!(mRealChannel[count]->mMode & FMOD_2D))
        {
            FMOD_RESULT      result;
            int              count2, numlisteners;
            float            smallestdistance;

            result = mSystem->get3DNumListeners(&numlisteners);
            if (result != FMOD_OK)
            {
                return result;
            }

            smallestdistance = 999999999.0f;

            for (count2 = 0; count2 < numlisteners; count2++)
            {
                FMOD_VECTOR  currdiff;
                float        distance;
                Listener    *listener;
                float        dopplerscale, distancescale, rolloffscale;

                result = mSystem->getListenerObject(count2, &listener);
                if (result != FMOD_OK)
                {
                    return result;
                }

                if (!(mFlags & CHANNELI_FLAG_MOVED) && !listener->mMoved)
                {
                    return FMOD_OK;
                }

                result = mSystem->get3DSettings(&dopplerscale, &distancescale, &rolloffscale);
                if (result != FMOD_OK)
                {
                    return result;
                }

                dopplerscale *= m3DDopplerLevel;

                /*
                    Distance between emitter and listener this frame 
                */
                if (mRealChannel[count]->mMode & FMOD_3D_HEADRELATIVE)
                {
                    currdiff = mPosition3D;
                }
                else
                {
                    FMOD_Vector_Subtract(&mPosition3D, &listener->mPosition, &currdiff);
                }

                if (mSystem->mFlags & FMOD_INIT_3D_RIGHTHANDED)
                {
                    currdiff.z = -currdiff.z;
                }

                distance = (float)FMOD_Vector_GetLengthFast(&currdiff);

                /*
                    VOLUME ATTENUATION
                */
                if (distance < smallestdistance)
                {
                    mDistance = smallestdistance = distance;

                    if (mSystem->mRolloffCallback)
                    {
                        vol3d = mSystem->mRolloffCallback((FMOD_CHANNEL *)mRealChannel[count]->mParent->mHandleCurrent, mDistance);
                    }
                    else if (mRealChannel[count]->mMode & FMOD_3D_CUSTOMROLLOFF)
                    {
                        if (mRolloffPoint && mNumRolloffPoints)
                        {
                            if (distance >= mRolloffPoint[mNumRolloffPoints - 1].x)
                            {
                                vol3d = mRolloffPoint[mNumRolloffPoints - 1].y;
                            }
                            else
                            {
                                int count2;

                                for (count2 = 1; count2 < mNumRolloffPoints; count2++)
                                {
                                    if (distance >= mRolloffPoint[count2-1].x && distance < mRolloffPoint[count2].x)
                                    {
                                        float frac = (distance - mRolloffPoint[count2-1].x) / (mRolloffPoint[count2].x - mRolloffPoint[count2-1].x);

                                        vol3d = ((1.0f - frac) * mRolloffPoint[count2-1].y) + (frac * mRolloffPoint[count2].y);
                                        break;
                                    }
                                }
                            }
                        }
                        else
                        {
                            vol3d = 1.0f;
                        }
                    }
                    else 
                    {
                        if (distance >= mMaxDistance)
                        {
                            distance = mMaxDistance;
                        }
                        if (distance < mMinDistance) 
                        {
                            distance = mMinDistance;
                        }

                        if (mRealChannel[count]->mMode & FMOD_3D_LINEARROLLOFF)
                        {
                            float range;

                            range = mMaxDistance - mMinDistance;
                            if (range <= 0)
                            {
                                vol3d = 1.0f;
                            }
                            else
                            {
                                distance = mMaxDistance - distance;
                                vol3d = distance / range;
                            }
                        }
                        else
                        {
                            if ((distance > mMinDistance) && (rolloffscale != 1.0f))
                            {
                                distance -= mMinDistance;
                                distance *= rolloffscale;
                                distance += mMinDistance;
                            }

                            /*
                                Rolloffscale is 0 safe but mindistance may be 0
                            */

                            if (distance < .000001f)
                            {
                                distance = .000001f;
                            }

                            /*
                                Get the reciprocal.
                            */
                            vol3d = mMinDistance / distance;
                        }
                    }

                    if (vol3d < 0.0f)
                    {
                        vol3d = 0.0f;
                    }
                    if (vol3d > 1.0f)
                    {
                        vol3d = 1.0f;
                    }

                    /*
                        Cone calculation.
                    */
                    if (mConeOutsideAngle < 360.0f || mConeInsideAngle < 360.0f)
                    {     
                        float angle;
                         
                        if (mDistance <= 0)
                        {
                            angle = 0;
                        }
                        else
                        {
                            float       scale           = 1.0f / mDistance;
                            FMOD_VECTOR coneorientation = mConeOrientation;
                            
                            if (mSystem->mFlags & FMOD_INIT_3D_RIGHTHANDED)
                            {
                                coneorientation.z = -coneorientation.z;
                            }
                            
                            /*
                                Normalize
                            */
                            FMOD_Vector_Scale(&currdiff, scale, &currdiff);

                            angle = -FMOD_Vector_DotProduct(&currdiff, &coneorientation);
                            angle = angle < -1.0f ? -1.0f : angle > 1.0f ? 1.0f : angle;    /* clamp just in case */
                            angle = FMOD_ACOS(angle) / FMOD_PI * 180.0f;
                            angle *= 2; // cone angles are from side to side (0 to 360), not from side to axis
                        }

                        if (angle < mConeInsideAngle)
                        {
                            conevol = 1.0f;
                        }
                        else if (angle < mConeOutsideAngle)
                        {
                            float f;
                            f = (angle - mConeInsideAngle) / (mConeOutsideAngle - mConeInsideAngle);
                            conevol = (mConeOutsideVolume * f) + (1.0f * (1.0f -f));
                        }
                        else
                        {
                            conevol = mConeOutsideVolume;
                        }
                    }
                }


                /*
                    DOPPLER EFFECT 
                */
                #define SPEED_OF_SOUND 340.0f       /* same as A3D */

                if ((dopplerscale > 0) && (numlisteners == 1))
                {
                    FMOD_VECTOR lastdiff, lastpos, tmp;
                    float       lastdistance;
                    // if delta is <= 0, set it to 1 second so it has no effect
                    // also clamp delta to 1 second maximum
                    float       delta_sec = (delta <= 0 || delta > 1000) ? 1.0f : delta / 1000.0f;

                    FMOD_Vector_Scale(&mVelocity3D, delta_sec, &tmp);
                    FMOD_Vector_Subtract(&mPosition3D, &tmp, &lastpos);

                    if (mRealChannel[count]->mMode & FMOD_3D_HEADRELATIVE)
                    {
                        lastdiff = lastpos;
                    }
                    else
                    {
                        FMOD_VECTOR listenerlastpos;

                        FMOD_Vector_Scale(&listener->mVelocity, delta_sec, &tmp);
                        FMOD_Vector_Subtract(&listener->mPosition, &tmp,
                                             &listenerlastpos);

                        FMOD_Vector_Subtract(&lastpos, &listenerlastpos, &lastdiff);
                    }
                    lastdistance = (float)FMOD_Vector_GetLengthFast(&lastdiff);

                    pitch3d  = SPEED_OF_SOUND * distancescale;
                    pitch3d -= ((mDistance - lastdistance) / delta_sec * dopplerscale);
                    pitch3d /= (SPEED_OF_SOUND * distancescale);
                }

                if (pitch3d < .000001f)
                {
                    pitch3d = .000001f;
                }
            }
        }
    }
#endif

    mVolume3D     = vol3d;
    mConeVolume3D = conevol;
    mPitch3D      = pitch3d;

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
// OFFSET: 0x008DF530
ChannelI::ChannelI()
{
    init();
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
// OFFSET: 0x008DF570
ChannelI::ChannelI(int index, SystemI *system)
{
    init();

    mIndex          = index;
    mSystem         = system;
    mHandleCurrent  = (mSystem->mIndex << SYSTEMID_SHIFT) | ((mIndex << CHANINDEX_SHIFT) & (CHANINDEX_MASK << CHANINDEX_SHIFT)) | (1 << REFCOUNT_SHIFT);
    mHandleOriginal = mHandleCurrent;
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
// OFFSET: 0x008DD530
FMOD_RESULT ChannelI::init()
{
    int count;

    mSystem               = 0;
    mHandleCurrent        = 0;
    mHandleOriginal       = 0;
    mCallback             = 0;

    for (count = 0; count < FMOD_CHANNEL_MAXREALSUBCHANNELS; count++)
    {
        mRealChannel[count] = 0;
    }
    for (count = 0; count < FMOD_CHANNEL_MAXINPUTCHANNELS; count++)
    {
        mInputMix[count] = 1.0f;
    }

    mNumRealChannels = 1;

    mPriority             = FMOD_CHANNEL_DEFAULTPRIORITY;
    mListPosition         = (unsigned int)-1;
    mIndex                = 0;

    /* 
        2D Stuff 
    */
    mVolume           = 1.0f;
    mFadeVolume       = 1.0f;
    mFadeTarget       = 1.0f;
    mFrequency        = DEFAULT_FREQUENCY;
    mPan              = 0.0f;
    mSpeakerFL        = 1.0f;
    mSpeakerFR        = 1.0f;
    mSpeakerC         = 1.0f;
    mSpeakerLFE       = 1.0f;
    mSpeakerBL        = 1.0f;
    mSpeakerBR        = 1.0f;
    mSpeakerSL        = 1.0f;
    mSpeakerSR        = 1.0f;
    mLevels           = 0;
                      
    /*                
        3D Stuff      
    */                
    mVolume3D         = 1.0f;
    mPitch3D          = 1.0f;
    mConeVolume3D     = 1.0f;
    mDirectOcclusion  = 0.0f;
    mReverbDryVolume  = 1.0f;
    mPosition3D.x     = 0;
    mPosition3D.y     = 0;
    mPosition3D.z     = 0;
    mVelocity3D.x     = 0;
    mVelocity3D.y     = 0;
    mVelocity3D.z     = 0;
    mMinDistance      = 1.0f;
    mMaxDistance      = 1000000000.0f;
    mRolloffPoint     = 0;
    mNumRolloffPoints = 0;

    mFlags            = 0;
    
    mEndDelay          = 0;
    mDSPClockDelay.mHi = 0;
    mDSPClockDelay.mLo = 0;

    mLowPassGain       = 1.0f;
    
    mChannelGroupNode.initNode();

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
FMOD_RESULT ChannelI::getSystemObject(System **system)
{
    if (!system)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *system = (System *)mSystem;

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
// OFFSET: 0x008E0200
FMOD_RESULT ChannelI::play(SoundI *sound, bool paused, bool reset, bool startmuted)
{
    FMOD_RESULT result;
    SoundI      *soundi;

    soundi = SAFE_CAST(SoundI, sound);
    if (!soundi)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    /*
        Allow the channel to allocate any needed resources
    */
    result = alloc(sound, reset); 
    if (result != FMOD_OK)
    {
        return result;
    }

    mFlags |= CHANNELI_FLAG_PLAYINGPAUSED;

    /*
        Start the channel (paused)
    */
	result = setPaused(true);
    if (result != FMOD_OK)
    {
        return result;
    }
    
    /*
        Copy defaults from sound to channel.
    */
    if (reset)
    {
        if (startmuted)
        {
            mFadeVolume = 0.0f;
            mFadeTarget = 0.0f;
        }
        else
        {
            mFadeVolume = 1.0f;
            mFadeTarget = 1.0f;
        }

        result = setDefaults();
        if (result != FMOD_OK)
        {
            return result;
        }

        /*
            Now start the voice in hardware (if it isnt paused)
        */
        result = setPosition(0, FMOD_TIMEUNIT_PCM);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        Now start the voice in hardware (if it isnt paused)
    */
	result = start();
    if (result != FMOD_OK)
    {
        return result;
    }

    sound->mNumAudible++;
    if (sound->mSoundGroup && mSystem)
    {
        FMOD_OS_CriticalSection_Enter(mSystem->gSoundListCrit);
        {
            sound->mSoundGroup->removeNode();
            sound->mSoundGroup->addAfter(&mSystem->mSoundGroupUsedHead);
        }
        FMOD_OS_CriticalSection_Leave(mSystem->gSoundListCrit);
    }
    
    /*
        Set the 3d position of the sound to that of the listener by default.
    */
    if (reset)
    {
        #ifdef FMOD_SUPPORT_3DSOUND
        {
            FMOD_MODE mode;

            sound->getMode(&mode);
            if (mode & FMOD_3D)
            {
	            FMOD_VECTOR vel = { 0, 0, 0 };
        
                result = set3DAttributes(&mSystem->mListener[0].mPosition, &vel);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
        }
        #endif

        /* 
            If the master channelgroup is muted make sure that this channel starts muted.
        */
        if (mSystem && mSystem->mChannelGroup->mMute)
        {
            result = setMute(mFlags & CHANNELI_FLAG_MUTED ? true : false);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    if (sound->mSyncPointHead && sound->mNumSyncPoints)
    {
        mSyncPointCurrent = SAFE_CAST(SyncPoint, sound->mSyncPointHead->getNext());
        mSyncPointLastPos = 0;
    }
       
    /*
        unpause the sound if the user requested it
    */
    if (!paused)
    {
        result = setPaused(paused);
        if (result != FMOD_OK)
        {
            return result;
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
// OFFSET: 0x008E0360
FMOD_RESULT ChannelI::play(DSPI *dsp, bool paused, bool reset, bool startmuted)
{  
    FMOD_RESULT result;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    /*
        Allow the channel to allocate any needed resources
    */
    result = alloc(dsp, reset); 
    if (result != FMOD_OK)
    {
        return result;
    }

    /*
        Start the channel (paused)
    */
	result = setPaused(true);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (startmuted)
    {
        mFadeVolume = 0.0f;
        mFadeTarget = 0.0f;
    }
    else
    {
        mFadeVolume = 1.0f;
        mFadeTarget = 1.0f;
    }

    /*
        Copy defaults from sound to channel, and make sure it is paused to start with.
    */
    if (reset)
    {
        result = setDefaults();
        if (result != FMOD_OK)
        {
            return result;
        }

        /*
            Now start the voice in hardware (if it isn't paused)
        */
        result = setPosition(0, FMOD_TIMEUNIT_PCM);
        if (result != FMOD_OK)
        {
            return result;
        }
    }


    /*
        Now start the voice in hardware (if it isn't paused)
    */
	result = start();
    if (result != FMOD_OK)
    {
        return result;
    }

    /*
        Set the 3d position of the sound to that of the listener by default.
    */
    if (reset)
    {
        #ifdef FMOD_SUPPORT_3DSOUND
        {
            FMOD_MODE mode = 0;

            getMode(&mode);
            if (mode & FMOD_3D)
            {
	            FMOD_VECTOR vel = { 0, 0, 0 };
        
                result = set3DAttributes(&mSystem->mListener[0].mPosition, &vel);
                if (result != FMOD_OK)
                {
                    return result;
                }	        
            }
        }
        #endif
    }

    /*
        unpause the sound if the user requested it
    */
    if (!paused)
    {
        result = setPaused(paused);
        if (result != FMOD_OK)
        {
            return result;
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
// OFFSET: 0x008DD670
FMOD_RESULT ChannelI::alloc(SoundI *sound, bool reset)
{  
    FMOD_RESULT result;
    int count;

    if (sound->mNumSubSounds && (!sound->mLength         
#ifdef FMOD_SUPPORT_SENTENCING
        || !sound->mSubSoundList))
#else
        ))
#endif
    {
        return FMOD_ERR_SUBSOUNDS;
    }

    if (reset)
    {
        mSyncPointCurrent = 0;

        if (mSystem)
        {
            mChannelGroup = mSystem->mChannelGroup;
            mSpeakerMode = mSystem->mSpeakerMode;
        }

        /*  
            2D stuff
        */
        mFlags             &= ~CHANNELI_FLAG_REALMUTE;
        mFlags             &= ~CHANNELI_FLAG_PAUSED;
        mFlags             &= ~CHANNELI_FLAG_MUTED;
        mFlags             &= ~CHANNELI_FLAG_FORCEVIRTUAL;
        mFlags             &= ~CHANNELI_FLAG_USEDPAUSEDELAY;

        for (count = 0; count < FMOD_CHANNEL_MAXINPUTCHANNELS; count++)
        {
            mInputMix[count] = 1.0f;
        }

        mEndDelay          = 0;
        mDSPClockEnd.mHi   = 0;
        mDSPClockEnd.mLo   = 0;
        mDSPClockPause.mHi = 0;
        mDSPClockPause.mLo = 0;

        mLowPassGain       = 1.0f;

        if (mSystem)
        {
            mDSPClockDelay.mHi = mSystem->mDSPClock.mHi;
            mDSPClockDelay.mLo = mSystem->mDSPClock.mLo;
        }
        else
        {
            mDSPClockDelay.mHi = 0;
            mDSPClockDelay.mLo = 0;
        }

        /*
            3D stuff
        */
        mFlags             &= ~CHANNELI_FLAG_MOVED;
        mVolume3D           = 1.0f;
        mConeVolume3D       = 1.0f;
        mPitch3D            = 1.0f;
        mDirectOcclusion    = 0.0f;
        mReverbDryVolume    = 1.0f;
        mMinDistance        = sound->mMinDistance;
        mMaxDistance        = sound->mMaxDistance;
        mDistance           = 0;
        mConeInsideAngle    = sound->mConeInsideAngle;
        mConeOutsideAngle   = sound->mConeOutsideAngle;
        mConeOutsideVolume  = sound->mConeOutsideVolume;
        mConeOrientation.x  = 0.0f;
        mConeOrientation.y  = 0.0f;
        mConeOrientation.z  = 1.0f;
        mRolloffPoint       = sound->mRolloffPoint;
        mNumRolloffPoints   = sound->mNumRolloffPoints;
        m3DPanLevel         = 1.0f;
        m3DDopplerLevel     = 1.0f;

        #ifdef FMOD_SUPPORT_REVERB
        if (mSystem)
        {
            int instance;  

            for (instance = 0; instance < FMOD_REVERB_MAXINSTANCES; instance++)
            {
                mSystem->mReverbGlobal.resetChanProperties(instance, mIndex);
            }

            #ifdef FMOD_SUPPORT_MULTIREVERB
            if (mSystem->mReverb3D.mInstance[0].mDSP)
            {
                mSystem->mReverb3D.resetChanProperties(0, mIndex);
            }
        
            ReverbI *reverb_c = SAFE_CAST(ReverbI, mSystem->mReverb3DHead.getNext());
            while (reverb_c != &(mSystem->mReverb3DHead))
            {      
                if (reverb_c->mInstance[0].mDSP)
                {
                    reverb_c->resetChanProperties(0, mIndex);
                }
                reverb_c = SAFE_CAST(ReverbI, reverb_c->getNext());
            }
            #endif

        }
        #endif

    }
    
    for (count = 0; count < mNumRealChannels; count++)
    {
        if (!mRealChannel[count])
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

#ifdef FMOD_SUPPORT_STREAMING
        if (sound->isStream())
        {
            mRealChannel[count]->mSound = sound;
        }
        else
#endif
        {
            Sample *sample = SAFE_CAST(Sample, sound);

            mRealChannel[count]->mSound = mNumRealChannels > 1 ? sample->mSubSample[count] : sound;
        }

        mRealChannel[count]->mSubChannelIndex     = count;
        mRealChannel[count]->mDSP                 = 0;
        mRealChannel[count]->mMode                = sound->mMode;
        mRealChannel[count]->mLoopStart           = sound->mLoopStart;
        mRealChannel[count]->mLoopLength          = sound->mLoopLength;
        mRealChannel[count]->mLoopCount           = sound->mLoopCount;
        mRealChannel[count]->mLength              = sound->mLength;
        mRealChannel[count]->mParent              = this;
        mRealChannel[count]->mSubSoundListCurrent = 0;
        mRealChannel[count]->mFlags              |= CHANNELREAL_FLAG_HASPLAYED;

        result = mRealChannel[count]->alloc();
        if (result != FMOD_OK)
        {
            return result;
        }

        sound->mFlags |= FMOD_SOUND_FLAG_PLAYED;
        if (sound->mSubSoundParent)
        {
            sound->mSubSoundParent->mFlags |= FMOD_SOUND_FLAG_PLAYED;
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
// OFFSET: 0x008DD870
FMOD_RESULT ChannelI::alloc(DSPI *dsp, bool reset)
{  
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        mRealChannel[count]->mSubChannelIndex = count;
        mRealChannel[count]->mSound           = 0;
        mRealChannel[count]->mDSP             = dsp;
        mRealChannel[count]->mLoopStart       = 0;
        mRealChannel[count]->mLoopLength      = (unsigned int)-1;
        mRealChannel[count]->mLoopCount       = -1;
        mRealChannel[count]->mMode            = FMOD_2D | FMOD_SOFTWARE;
        mRealChannel[count]->mParent          = this;
        mRealChannel[count]->mFlags          |= CHANNELREAL_FLAG_HASPLAYED;
    }

    if (reset)
    {
        /*  
            2D stuff
        */
        mFlags             &= ~CHANNELI_FLAG_REALMUTE;
        mFlags             &= ~CHANNELI_FLAG_PAUSED;
        mFlags             &= ~CHANNELI_FLAG_MUTED;
        mFlags             &= ~CHANNELI_FLAG_FORCEVIRTUAL;

        for (count = 0; count < FMOD_CHANNEL_MAXINPUTCHANNELS; count++)
        {
            mInputMix[count] = 1.0f;
        }

        mEndDelay          = 0;
        mDSPClockDelay.mHi = mSystem->mDSPClock.mHi;
        mDSPClockDelay.mLo = mSystem->mDSPClock.mLo;

        mLowPassGain       = 1.0f;

        /*
            3D stuff
        */
        mFlags             &= ~CHANNELI_FLAG_MOVED;
        mVolume3D           = 1.0f;
        mConeVolume3D       = 1.0f;
        mPitch3D            = 1.0f;
        mDirectOcclusion    = 0.0f;
        mReverbDryVolume    = 1.0f;
        mMinDistance        = 1.0f;         //        mMinDistance        = dsp->mmMinDistance;
        mMaxDistance        = 10000.0f;     //        mMaxDistance        = dsp->mMaxDistance;
        mDistance           = 0;
        mConeInsideAngle    = 360.0f;       //        mConeInsideAngle    = dsp->mConeInsideAngle;
        mConeOutsideAngle   = 360.0f;       //        mConeOutsideAngle   = dsp->mConeOutsideAngle;
        mConeOutsideVolume  = 1.0f;         //        mConeOutsideVolume  = dsp->mConeOutsideVolume;
        mConeOrientation.x  = 0.0f;
        mConeOrientation.y  = 0.0f;
        mConeOrientation.z  = 1.0f;
        mRolloffPoint       = 0;            //        mRolloffPoint       = dsp->mRolloffPoint;
        mNumRolloffPoints   = 0;            //        mNumRolloffPoints   = dsp->mNumRolloffPoints;
        m3DPanLevel         = 1.0f;
        m3DDopplerLevel     = 1.0f;
    }
    
    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result;

        result = mRealChannel[count]->alloc(dsp);
        if (result != FMOD_OK)
        {
            return result;
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
// OFFSET: 0x008DDA10
FMOD_RESULT ChannelI::start()
{
    FMOD_RESULT result;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->start();
        if (result != FMOD_OK)
        {
            return result;
        }

        mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_STOPPED;
        mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_ALLOCATED;
        mRealChannel[count]->mFlags |= CHANNELREAL_FLAG_PLAYING;       
    }

    mFlags &= ~CHANNELI_FLAG_ENDDELAY;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Process sync point callbacks.

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]
    Currently it scans from the start of the syncpoint list to the end which is a bit inefficient but right now it works
    fairly well because it supports even triggering the syncpoints correctly with sounds playing backwards, and complex loops.

    [PLATFORMS]

	[SEE_ALSO]
]
*/
// OFFSET: 0x008DF5E0
FMOD_RESULT ChannelI::updateSyncPoints(bool seeking)
{
    FMOD_RESULT result;
    SoundI *sound = mRealChannel[0]->mSound;

    if (sound)
    {        
        if (sound->mSubSampleParent)
        {
            sound = sound->mSubSampleParent;
        }

        if (sound->mSyncPointHead && sound->mNumSyncPoints && !mSyncPointCurrent)
        {
            mSyncPointCurrent = SAFE_CAST(SyncPoint, sound->mSyncPointHead->getNext());
        }

        if (mSyncPointCurrent)
        {
            unsigned int position;

            result = getPosition(&position, FMOD_TIMEUNIT_PCM);
            if (result == FMOD_OK)
            {
                if (seeking)
                {
                    if (mFrequency > 0)
                    {
                        mSyncPointCurrent = SAFE_CAST(SyncPoint, sound->mSyncPointHead->getNext());
                    }
                    else
                    {
                        mSyncPointCurrent = SAFE_CAST(SyncPoint, sound->mSyncPointTail->getPrev());
                    }

                    while ((mFrequency > 0 && position > mSyncPointCurrent->mOffset) || (mFrequency < 0 && position < mSyncPointCurrent->mOffset))
                    {
                        if (mFrequency > 0)
                        {
                            if (mSyncPointCurrent->mOffset >= position)
                            {
                                break;
                            }

                            mSyncPointCurrent = SAFE_CAST(SyncPoint, mSyncPointCurrent->getNext());
                            if (mSyncPointCurrent == sound->mSyncPointTail)
                            {
                                break;
                            }
                        }
                        else
                        {
                            if (mSyncPointCurrent->mOffset <= position)
                            {
                                break;
                            }

                            mSyncPointCurrent= SAFE_CAST(SyncPoint, mSyncPointCurrent->getPrev());
                            if (mSyncPointCurrent == sound->mSyncPointHead)
                            {
                                break;
                            }
                        }
                    }
                }
                else
                {
                    bool loop = false;

                    if ((mFrequency > 0 && position < mSyncPointLastPos) ||
                        (mFrequency < 0 && position > mSyncPointLastPos))   /* Loop */
                    {
                        loop = true;
                    }

                    while ((mFrequency > 0 && position > mSyncPointCurrent->mOffset) || (mFrequency < 0 && position < mSyncPointCurrent->mOffset) || loop)
                    {
                        if (mFrequency > 0)
                        {
                            if (mSyncPointCurrent != sound->mSyncPointTail)
                            {
                                bool callcallback = true;
                                SyncPoint *next = SAFE_CAST(SyncPoint, mSyncPointCurrent->getNext());

                                if (sound->mSubSoundShared && mSyncPointCurrent->mSubSoundIndex != sound->mSubSoundIndex)
                                {
                                    callcallback = false;
                                }

                                if (callcallback && mCallback)
                                {
                                    mCallback((FMOD_CHANNEL *)((FMOD_UINT_NATIVE)mHandleCurrent), FMOD_CHANNEL_CALLBACKTYPE_SYNCPOINT, (void *)mSyncPointCurrent->mIndex, 0);
                                }

                                mSyncPointCurrent = next;
                            }

                            if (loop && mSyncPointCurrent == sound->mSyncPointTail)
                            {
                                mSyncPointCurrent = SAFE_CAST(SyncPoint, sound->mSyncPointHead->getNext());
                                loop = false;
                            }
                        }
                        else
                        {
                            if (mSyncPointCurrent != sound->mSyncPointHead)
                            {
                                bool callcallback = true;
                                SyncPoint *prev = SAFE_CAST(SyncPoint, mSyncPointCurrent->getPrev());

                                if (sound->mSubSoundShared && mSyncPointCurrent->mSubSoundIndex != sound->mSubSoundIndex || mSyncPointCurrent == sound->mSyncPointHead)
                                {
                                    callcallback = false;
                                }

                                if (callcallback && mCallback)
                                {
                                    mCallback((FMOD_CHANNEL *)((FMOD_UINT_NATIVE)mHandleCurrent), FMOD_CHANNEL_CALLBACKTYPE_SYNCPOINT, (void *)mSyncPointCurrent->mIndex, 0);
                                }

                                mSyncPointCurrent = prev;
                            }

                            if (loop && mSyncPointCurrent == sound->mSyncPointHead)
                            {
                                mSyncPointCurrent = SAFE_CAST(SyncPoint, sound->mSyncPointTail->getPrev());
                                loop = false;
                            }
                        }
                    }
                }
            }

            mSyncPointLastPos = position;
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
// OFFSET: 0x008E0850
FMOD_RESULT ChannelI::update(int delta, bool updategeometrynow)
{  
    FMOD_RESULT result;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }
    
    if (mFlags & CHANNELI_FLAG_PLAYINGPAUSED)
    {
        return FMOD_OK;
    }    

    /*
        Geometry / Polygon based occlusion update.
    */
#ifdef FMOD_SUPPORT_GEOMETRY   

    bool playingpaused = (mRealChannel[0] && mRealChannel[0]->mFlags & CHANNELREAL_FLAG_PLAYING && mRealChannel[0]->mFlags & CHANNELREAL_FLAG_PAUSED) || updategeometrynow;

    if (mRealChannel[0] && (mRealChannel[0]->mMode & FMOD_3D) && !(mRealChannel[0]->mMode & FMOD_3D_IGNOREGEOMETRY))
    {
        float directOcclusionTargetOriginal = mDirectOcclusionTarget;
        float reverbOcclusionTargetOriginal = mReverbOcclusionTarget;
        bool occlusionupdated = false;

        if (((mFlags & CHANNELI_FLAG_MOVED) || mSystem->mListener[0].mMoved || mSystem->mGeometryMgr.mMoved || playingpaused) && mSystem->mGeometryList)
        {
            FMOD_VECTOR channelpos;
            
            /* Convert head relative position to world space */
            if (mRealChannel[0]->mMode & FMOD_3D_HEADRELATIVE)
            {
                FMOD_Vector_Add(&mPosition3D, &mSystem->mListener[0].mPosition, &channelpos);
            }
            else
            {
                FMOD_Vector_Copy(&mPosition3D, &channelpos);
            }

#ifdef FMOD_SUPPORT_GEOMETRY_THREADED
            if (updategeometrynow)
#endif //FMOD_SUPPORT_GEOMETRY_THREADED
            {
                /* Process geometry in main thread */
                mSystem->mGeometryMgr.lineTestAll(&mSystem->mListener[0].mPosition, &channelpos, &mDirectOcclusionTarget, &mReverbOcclusionTarget);
                occlusionupdated = true;

#ifdef FMOD_SUPPORT_MULTIREVERB                
                float gain;

                calculate3DReverbGain(&mSystem->mReverb3D, &channelpos, &gain);
                mSystem->mReverb3D.setPresenceGain(0, mIndex, gain);
#endif //FMOD_SUPPORT_MULTIREVERB
            }
#ifdef FMOD_SUPPORT_GEOMETRY_THREADED
            else
            {
                /* Offload geometry processing to backround thread */
                mSystem->mGeometryMgr.mOcclusionThread.enqueue(mIndex, mHandleCurrent, &channelpos);
            }
#endif //FMOD_SUPPORT_GEOMETRY_THREADED

#ifdef FMOD_SUPPORT_MULTIREVERB
            /*
            Find channel's source contribution to each 3D reverb's input gain
            Listener contributions are calculated by SystemI
            */
            ReverbI* reverb_c;
            for (reverb_c = SAFE_CAST(ReverbI, mSystem->mReverb3DHead.getNext()); reverb_c != &mSystem->mReverb3DHead; reverb_c = SAFE_CAST(ReverbI, reverb_c->getNext()))
            {
                if (reverb_c->getMode() == FMOD_REVERB_PHYSICAL)
                {
                    float gain;

                    calculate3DReverbGain(reverb_c, &channelpos, &gain);

                    //
                    // Set channel's presence gain relative to this reverb
                    //
                    reverb_c->setPresenceGain(0, mIndex, gain);
                }
            }
#endif //FMOD_SUPPORT_MULTIREVERB
        }

#ifdef FMOD_SUPPORT_GEOMETRY_THREADED
        if (!updategeometrynow)
        {
            /* Check if the background thread has updated this channels occlusion values */
            OCCLUSION_TASK *task = mSystem->mGeometryMgr.mOcclusionThread.retrieveOcclusion(mIndex);
            if (task && task->currenthandle == mHandleCurrent)
            {
                mDirectOcclusionTarget = task->directocclusion;
                mReverbOcclusionTarget = task->reverbocclusion;

#ifdef FMOD_SUPPORT_MULTIREVERB
                mSystem->mReverb3D.setPresenceGain(0, mIndex, task->reverbgain);
#endif
                occlusionupdated = true;
            }
        }
#endif //FMOD_SUPPORT_GEOMETRY_THREADED

        if (mCallback && occlusionupdated)
        {
            mCallback((FMOD_CHANNEL *)((FMOD_UINT_NATIVE)mHandleCurrent), FMOD_CHANNEL_CALLBACKTYPE_OCCLUSION, &mDirectOcclusionTarget, &mReverbOcclusionTarget);
        }

        if ((mDirectOcclusion != mDirectOcclusionTarget || mReverbOcclusion != mReverbOcclusionTarget) && (delta || playingpaused))
        {
            // update occlusion
            const float DEFAULT_OCCLUSION_MAX_FADE_TIME = 500;
            const float OCCLUSION_DURATION_OF_CHANGE = mSystem->mAdvancedSettings.geometryMaxFadeTime ? mSystem->mAdvancedSettings.geometryMaxFadeTime : DEFAULT_OCCLUSION_MAX_FADE_TIME;
            const float MIN_OCCLUSION_RATE_OF_CHANGE = 0.0001f;

            if(occlusionupdated)
            {
                if(directOcclusionTargetOriginal != mDirectOcclusionTarget)
                {
                    mDirectOcclusionRateOfChange = (mDirectOcclusionTarget - mDirectOcclusion) / OCCLUSION_DURATION_OF_CHANGE;

                    if(mDirectOcclusionRateOfChange < 0.0f)
                    {
                        mDirectOcclusionRateOfChange = mDirectOcclusionRateOfChange > -MIN_OCCLUSION_RATE_OF_CHANGE ? -MIN_OCCLUSION_RATE_OF_CHANGE : mDirectOcclusionRateOfChange;
                    }
                    else
                    {
                        mDirectOcclusionRateOfChange = mDirectOcclusionRateOfChange < MIN_OCCLUSION_RATE_OF_CHANGE ? MIN_OCCLUSION_RATE_OF_CHANGE : mDirectOcclusionRateOfChange;
                    }
                }
                if(reverbOcclusionTargetOriginal != mReverbOcclusionTarget)
                {
                    mReverbOcclusionRateOfChange = (mReverbOcclusionTarget - mReverbOcclusion) / OCCLUSION_DURATION_OF_CHANGE;

                    if(mReverbOcclusionRateOfChange < 0.0f)
                    {
                         mReverbOcclusionRateOfChange = mReverbOcclusionRateOfChange > -MIN_OCCLUSION_RATE_OF_CHANGE ? -MIN_OCCLUSION_RATE_OF_CHANGE : mReverbOcclusionRateOfChange;
                    }
                    else
                    {
                         mReverbOcclusionRateOfChange = mReverbOcclusionRateOfChange < MIN_OCCLUSION_RATE_OF_CHANGE ? MIN_OCCLUSION_RATE_OF_CHANGE : mReverbOcclusionRateOfChange;
                    }
                }
            }

            if (playingpaused)
            {
                mDirectOcclusion = mDirectOcclusionTarget;
            }
            else if (mDirectOcclusion < mDirectOcclusionTarget)
            {
                mDirectOcclusion += mDirectOcclusionRateOfChange * (float)delta;
                if (mDirectOcclusion > mDirectOcclusionTarget)
                {
                    mDirectOcclusion = mDirectOcclusionTarget;
                }
            }
            else if (mDirectOcclusion > mDirectOcclusionTarget)
            {
                mDirectOcclusion += mDirectOcclusionRateOfChange * (float)delta;
                if (mDirectOcclusion < mDirectOcclusionTarget)
                {
                    mDirectOcclusion = mDirectOcclusionTarget;
                }
            }

            if (playingpaused)
            {
                mReverbOcclusion = mReverbOcclusionTarget;
            }
            if (mReverbOcclusion < mReverbOcclusionTarget)
            {
                mReverbOcclusion += mReverbOcclusionRateOfChange * (float)delta;
                if (mReverbOcclusion > mReverbOcclusionTarget)
                {
                    mReverbOcclusion = mReverbOcclusionTarget;
                }
            }
            else if (mReverbOcclusion > mReverbOcclusionTarget)
            {
                mReverbOcclusion += mReverbOcclusionRateOfChange * (float)delta;
                if (mReverbOcclusion < mReverbOcclusionTarget)
                {
                    mReverbOcclusion = mReverbOcclusionTarget;
                }
            }

            set3DOcclusionInternal(mDirectOcclusion, mReverbOcclusion, false);
        }
    }

    #endif /* FMOD_SUPPORT_GEOMETRY */

    if (mEndDelay && mFlags & CHANNELI_FLAG_ENDDELAY)
    {
        if (mEndDelay <= (unsigned int)delta)
        {
            mEndDelay = 0;
        }
        else
        {
            mEndDelay -= delta;
        }
    }

    result = calcVolumeAndPitchFor3D(delta);
    if (result != FMOD_OK)
    {
        return result;
    }

	for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->set2DFreqVolumePanFor3D();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    result = updateSyncPoints(false);
    if (result != FMOD_OK)
    {
        return result;
    }

    /*
        Check if the sync point has stopped the channel?
    */
    if (mRealChannel[0] && mRealChannel[0]->mFlags & CHANNELREAL_FLAG_STOPPED)
    {
        return FMOD_OK;
    }

    /*
        Call realchannel's update function if it needs to.
    */
	for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->update(delta);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        Update sorted list position based on audibility.
    */
    if ((mFlags & CHANNELI_FLAG_MOVED) || mSystem->mListener[0].mMoved)
    {
        result = updatePosition();
        if (result != FMOD_OK)
        {
            return result;
        }

        mFlags &= ~CHANNELI_FLAG_MOVED;
    }

    return FMOD_OK;
}

#ifdef FMOD_SUPPORT_MULTIREVERB
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
// OFFSET: 0x008DDA70
FMOD_RESULT ChannelI::calculate3DReverbGain(ReverbI *reverb, FMOD_VECTOR *channelpos, float *gain)
{
    if (!gain)
    {
        return FMOD_OK;
    }

    // Make an initial gain value based on whether the
    // channel is 'inside' the reverb's extent
    if (reverb == &(mSystem->mReverb3D))
    {
        //
        // Main 3D reverb extent is infinite
        //
        *gain = 1.0f;
    }
    else
    {
        //
        // Other 3D reverb gain based on min/max distance to reverb
        //
        reverb->calculateDistanceGain(channelpos, gain, 0);
    }

#ifdef FMOD_SUPPORT_GEOMETRY
    /*
        Reverb occlusion gain of the channel-reverb path
    */
    if (*gain > 0.0f)
    {
        float direct_o, reverb_o;
        FMOD_VECTOR reverbpos;

        reverb->get3DAttributes(&reverbpos, 0, 0);

        mSystem->mGeometryMgr.lineTestAll(channelpos, &reverbpos, &direct_o, &reverb_o);    
        
        *gain *= (1.0f - reverb_o);
    }
#endif

    return FMOD_OK;
}
#endif

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
FMOD_RESULT ChannelI::updateStream()
{  
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    return mRealChannel[0]->updateStream();
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
// OFFSET: 0x008E0440
FMOD_RESULT ChannelI::stop()
{
    return stopEx(CHANNELI_STOPFLAG_REFSTAMP | 
                  CHANNELI_STOPFLAG_UPDATELIST | 
                  CHANNELI_STOPFLAG_RESETCALLBACKS | 
                  CHANNELI_STOPFLAG_CALLENDCALLBACK | 
                  CHANNELI_STOPFLAG_RESETCHANNELGROUP |
                  CHANNELI_STOPFLAG_UPDATESYNCPOINTS);
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
// OFFSET: 0x008DF810
FMOD_RESULT ChannelI::stopEx(FMOD_CHANNEL_STOPFLAG stopflag)
{
    FMOD_RESULT result;
    int count;
    ChannelReal *oldrealchannel[FMOD_CHANNEL_MAXREALSUBCHANNELS];
   
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }
    
    if (mRealChannel[0]->mFlags & CHANNELREAL_FLAG_STOPPED)
    {
        return FMOD_OK; /* Already stopped */
    }    

    if (stopflag & CHANNELI_STOPFLAG_PROCESSENDDELAY && mEndDelay)
    {
        mFlags |= CHANNELI_FLAG_ENDDELAY;
        return FMOD_OK;
    }

    if (stopflag & CHANNELI_STOPFLAG_UPDATESYNCPOINTS)
    {
        updateSyncPoints(false);
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_IN_USE;
        mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_ALLOCATED;
        mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_PLAYING;
        mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_PAUSED;
        mRealChannel[count]->mFlags |=  CHANNELREAL_FLAG_STOPPED;
    }

    if (stopflag & CHANNELI_STOPFLAG_RESETCHANNELGROUP)
    {
        result = setChannelGroupInternal(0, false);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

	for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->stop();
        if (result != FMOD_OK)
        {
            return result;
        }

        if (mRealChannel[count]->mSound)
        {
            mRealChannel[count]->mSound->mNumAudible--;
            if (!mRealChannel[count]->mSound->mNumAudible)
            {
                mRealChannel[count]->mSound->mLastAudibleDSPClock = mSystem ? mSystem->mDSPClock.mValue : 0;
            }
        }
    }

    if (stopflag & CHANNELI_STOPFLAG_UPDATELIST)
    {
        mEndDelay = 0;
        mDSPClockDelay.mHi = 0;
        mDSPClockDelay.mLo = 0;

        result = returnToFreeList();
        if (result != FMOD_OK)
        {
            return result;
        }
        mListPosition = (unsigned int)-1;
        
        mFlags &= ~CHANNELI_FLAG_JUSTWENTVIRTUAL;

        mAddDSPHead = 0;
    }

    mFlags &= ~CHANNELI_FLAG_USEDINPUTMIX;
    mFlags &= ~CHANNELI_FLAG_PLAYINGPAUSED;
   
	for (count = 0; count < mNumRealChannels; count++)
    {
        oldrealchannel[count] = mRealChannel[count];
    }

    /*
        End of sound callback.  Warning!  All sorts of things could have been called in this by the user!
    */
    if (stopflag & CHANNELI_STOPFLAG_CALLENDCALLBACK && mCallback)
    {
        mCallback((FMOD_CHANNEL *)((FMOD_UINT_NATIVE)mHandleCurrent), FMOD_CHANNEL_CALLBACKTYPE_END, 0, 0);
    }

    /*
        Real channel may have been re-used, by playsound.  Don't clear out it's attributes if it has.
    */
	for (count = 0; count < mNumRealChannels; count++)
    {
        if (!(mRealChannel[count]->mFlags & CHANNELREAL_FLAG_PLAYING) || oldrealchannel[count] != mRealChannel[count])
        {
            bool crit = false;

            if (oldrealchannel[count]->mSound && oldrealchannel[count]->mSound->isStream())
            {
                crit = true;
            }

#ifdef FMOD_SUPPORT_STREAMING
            if (crit)
            {
                FMOD_OS_CriticalSection_Enter(mSystem->mStreamRealchanCrit);
            }
#endif

            oldrealchannel[count]->mSound  = 0;
            oldrealchannel[count]->mDSP    = 0;
            oldrealchannel[count]->mParent = 0;

#ifdef FMOD_SUPPORT_STREAMING
            if (crit)
            {
                FMOD_OS_CriticalSection_Leave(mSystem->mStreamRealchanCrit);
            }
#endif
        }
    }

    /*
        This ChannelI may have been used.  It had its listposition set to -1 before, so any playsound in the callback would have set it to something else.
    */
    if (mListPosition == (unsigned int)-1 && !(mFlags & CHANNELI_FLAG_PLAYINGPAUSED))
    {
	    for (count = 0; count < mNumRealChannels; count++)
        {
            mRealChannel[count] = 0;
        }

        if (stopflag & CHANNELI_STOPFLAG_RESETCALLBACKS)
        {
            mCallback = 0;
            mSyncPointCurrent = 0;
        }

        if (stopflag & CHANNELI_STOPFLAG_REFSTAMP)
        {
            result = referenceStamp();
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    if (mLevels)
    {
        /*
            Don't free the speaker levels if they have been acquired by getChannelInfo,
            simply disown them by nulling out the pointer.
        */
        if (!(stopflag & CHANNELI_STOPFLAG_DONTFREELEVELS))
        {
            mSystem->mSpeakerLevelsPool.free(mLevels);
        }
        mLevels = NULL;
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
// OFFSET: 0x008DDB00
FMOD_RESULT ChannelI::setPaused(bool paused)
{
    FMOD_RESULT result = FMOD_OK;
    int count;
	ChannelGroupI *current;
        
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (paused)
    {
        mFlags |= CHANNELI_FLAG_PAUSED;
    }
    else
    {
        mFlags &= ~CHANNELI_FLAG_PAUSED;

        if (mFlags & CHANNELI_FLAG_PLAYINGPAUSED)
        {
            FMOD_MODE mode = 0;
            
            mFlags &= ~CHANNELI_FLAG_PLAYINGPAUSED;

            updatePosition();

            getMode(&mode);
            if (mode & FMOD_3D)
            {
                update(0, true);
            }
        }
    }

	/*
		Scan up through channelgroup heirarchy.  If any parent group is paused, then the channel should be paused.
	*/
	current = mChannelGroup;
	do
	{
		if (current->mPaused)
		{
			paused = true;
			break;
		}
		current = current->mParent;
	} while (current);

	for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2;

        result2 = mRealChannel[count]->setPaused(paused);
        if (result == FMOD_OK)
        {
            result = result2;
        }

	    if (paused)
	    {
            mRealChannel[count]->mFlags |= CHANNELREAL_FLAG_PAUSED;
	    }
	    else
	    {
            mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_PAUSED;
	    }
    }
        
    return result;
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
FMOD_RESULT ChannelI::getPaused(bool *paused)
{
    if (!paused)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    *paused = mFlags & CHANNELI_FLAG_PAUSED ? true : false;

    /*
        Make sure by probing the software channel that setactive hasn't been used from within the mixer.
    */
    if (!*paused && mFlags & CHANNELI_FLAG_USEDPAUSEDELAY)
    {
        FMOD_RESULT result;
        
        result = mRealChannel[0]->getPaused(paused);
        if (result != FMOD_OK)
        {
            return result;
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
// OFFSET: 0x008DDB90
FMOD_RESULT ChannelI::setVolume(float volume, bool forceupdatepos)
{
    FMOD_RESULT   result = FMOD_OK;
    int           count;
    bool          updatepos = false;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (volume < 0)
    {
        volume = 0;
    }
    if (volume > 1.0f)
    {
        volume = 1.0f;
    }
    
    if (mVolume != volume)
    {
        updatepos = true;
    }

    mVolume = volume;

    if (mFlags & CHANNELI_FLAG_REALMUTE)
    {
        volume = 0;
    }

    if (mNumRealChannels > 1 && mLevels && mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERLEVELS)
    {
        for (count = 0; count < mNumRealChannels; count++)
        {
            result = mRealChannel[count]->updateSpeakerLevels(volume);
        }
    }
    else if (mNumRealChannels > 1 && mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERMIX)
    {
        for (count = 0; count < mNumRealChannels; count++)
        {
            result = mRealChannel[count]->setSpeakerMix(mSpeakerFL, mSpeakerFR, mSpeakerC, mSpeakerLFE, mSpeakerBL, mSpeakerBR, mSpeakerSL, mSpeakerSR);
        }
    }
    else
    {
        for (count = 0; count < mNumRealChannels; count++)
        {
            result = mRealChannel[count]->setVolume(volume);
        }
    }

    if (updatepos || forceupdatepos)
    {
        result = updatePosition();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    return result;
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
// OFFSET: 0x008DDC90
FMOD_RESULT ChannelI::getVolume(float *volume)
{
    if (!volume)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    *volume = mVolume;

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
// OFFSET: 0x008DDCC0
FMOD_RESULT ChannelI::setFrequency(float frequency)
{
    FMOD_RESULT result = FMOD_OK;
    int count;
    float oldfrequency = mFrequency;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (frequency < mRealChannel[0]->mMinFrequency)
    {
        frequency = mRealChannel[0]->mMinFrequency;
    }
    if (frequency > mRealChannel[0]->mMaxFrequency)
    {
        frequency = mRealChannel[0]->mMaxFrequency;
    }

    mFrequency = frequency;

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2;

        result2 = mRealChannel[count]->setFrequency(mFrequency);
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }


    if (mSyncPointCurrent && ((oldfrequency < 0 && mFrequency > 0) || (oldfrequency > 0 && mFrequency < 0)))
    {
        updateSyncPoints(true);
    }

    return result;
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
// OFFSET: 0x008DDD60
FMOD_RESULT ChannelI::getFrequency(float *frequency)
{
    if (!frequency)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    *frequency = mFrequency;

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
// OFFSET: 0x008DDD90
FMOD_RESULT ChannelI::setPan(float pan, bool calldriver)
{
    FMOD_RESULT result = FMOD_OK;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (pan < -1.0f)
    {
        pan = -1.0f;
    }
    if (pan > 1.0f)
    {
        pan = 1.0f;
    }

    mPan = pan;

    mLastPanMode = FMOD_CHANNEL_PANMODE_PAN;

    if (mRealChannel[0]->mMode & FMOD_3D)
    {
        return FMOD_OK; /* Just return.  The 3d system will pick up the actual driver call later. */
    }
    else if (calldriver)
    {
        int count;

        for (count = 0; count < mNumRealChannels; count++)
        {
            FMOD_RESULT result2;

            if (mNumRealChannels > 1)
            {            
                if (mNumRealChannels == 2 || (mRealChannel[0]->mSound && mRealChannel[0]->mSound->mDefaultChannelMask == SPEAKER_ALLSTEREO))
                {
                    if (!(count & 1))
                    {
                        pan = -1;
                    }
                    else
                    {
                        pan = 1;
                    }
                }
            }

            result2 = mRealChannel[count]->setPan(pan);
            if (result == FMOD_OK)
            {
                result = result2;
            }
        }
    }

    return result;
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
FMOD_RESULT ChannelI::getPan(float *pan)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mRealChannel[0]->mMode & FMOD_3D)
    {
        return FMOD_ERR_NEEDS2D;
    }

    if (!pan)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *pan = mPan;

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
// OFFSET: 0x008DDEA0
FMOD_RESULT ChannelI::setDelay(FMOD_DELAYTYPE delaytype, unsigned int delayhi, unsigned int delaylo)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (delaytype == FMOD_DELAYTYPE_END_MS)
    {
        mEndDelay = delayhi;
    }
    else if (delaytype == FMOD_DELAYTYPE_DSPCLOCK_START)
    {
        mDSPClockDelay.mHi = delayhi;
        mDSPClockDelay.mLo = delaylo;

        for (count = 0; count < mNumRealChannels; count++)
        {
            FMOD_RESULT result2;

            result2 = mRealChannel[count]->setDSPClockDelay();
            if (result == FMOD_OK)
            {
                result = result2;
            }
        }
    }
    else if (delaytype == FMOD_DELAYTYPE_DSPCLOCK_END)
    {
        mDSPClockEnd.mHi = delayhi;
        mDSPClockEnd.mLo = delaylo;

        for (count = 0; count < mNumRealChannels; count++)
        {
            FMOD_RESULT result2;

            result2 = mRealChannel[count]->setDSPClockDelay();
            if (result == FMOD_OK)
            {
                result = result2;
            }
        }
    }
    else if (delaytype == FMOD_DELAYTYPE_DSPCLOCK_PAUSE)
    {
        mDSPClockPause.mHi = delayhi;
        mDSPClockPause.mLo = delaylo;

        for (count = 0; count < mNumRealChannels; count++)
        {
            FMOD_RESULT result2;

            result2 = mRealChannel[count]->setDSPClockDelay();
            if (result == FMOD_OK)
            {
                result = result2;
            }
        }

        mFlags |= CHANNELI_FLAG_USEDPAUSEDELAY;
    }
    else
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    return result;
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
FMOD_RESULT ChannelI::getDelay(FMOD_DELAYTYPE delaytype, unsigned int *delayhi, unsigned int *delaylo)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (delayhi)
    {
        if (delaytype == FMOD_DELAYTYPE_END_MS)
        {
            *delayhi = mEndDelay;
        }
        else if (delaytype == FMOD_DELAYTYPE_DSPCLOCK_START)
        {
            *delayhi = mDSPClockDelay.mHi;
        }
        else if (delaytype == FMOD_DELAYTYPE_DSPCLOCK_END)
        {
            *delayhi = mDSPClockEnd.mHi;
        }
        else if (delaytype == FMOD_DELAYTYPE_DSPCLOCK_PAUSE)
        {
            *delayhi = mDSPClockPause.mHi;
        }
        else
        {
            return FMOD_ERR_INVALID_PARAM;
        }
    }

    if (delaylo)
    {
        if (delaytype == FMOD_DELAYTYPE_END_MS)
        {
            *delaylo = 0;
        }
        else if (delaytype == FMOD_DELAYTYPE_DSPCLOCK_START)
        {
            *delaylo = mDSPClockDelay.mLo;
        }
        else if (delaytype == FMOD_DELAYTYPE_DSPCLOCK_END)
        {
            *delaylo = mDSPClockEnd.mLo;
        }
        else if (delaytype == FMOD_DELAYTYPE_DSPCLOCK_PAUSE)
        {
            *delaylo = mDSPClockPause.mLo;
        }
        else
        {
            return FMOD_ERR_INVALID_PARAM;
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
// OFFSET: 0x008DDF00
FMOD_RESULT ChannelI::setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright, bool calldriver)
{
    FMOD_RESULT result = FMOD_OK;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    mSpeakerFL  = frontleft  < 0 ? 0 : frontleft  > 5.0f ? 5.0f : frontleft ;
    mSpeakerFR  = frontright < 0 ? 0 : frontright > 5.0f ? 5.0f : frontright;
    mSpeakerC   = center     < 0 ? 0 : center     > 5.0f ? 5.0f : center    ;
    mSpeakerLFE = lfe        < 0 ? 0 : lfe        > 5.0f ? 5.0f : lfe;
    mSpeakerBL  = backleft   < 0 ? 0 : backleft   > 5.0f ? 5.0f : backleft  ;
    mSpeakerBR  = backright  < 0 ? 0 : backright  > 5.0f ? 5.0f : backright ;
    mSpeakerSL  = sideleft   < 0 ? 0 : sideleft   > 5.0f ? 5.0f : sideleft  ;
    mSpeakerSR  = sideright  < 0 ? 0 : sideright  > 5.0f ? 5.0f : sideright ;

    mLastPanMode = FMOD_CHANNEL_PANMODE_SPEAKERMIX;

    if (mRealChannel[0]->mMode & FMOD_3D)
    {
        return FMOD_OK; /* Just return, the LFE value has been set and the 3d system will pick up the actual driver call later. */
    }
    else if (calldriver)
    {   
        int count;
     
        for (count = 0; count < mNumRealChannels; count++)
        {
            FMOD_RESULT result2;

            result2 = mRealChannel[count]->setSpeakerMix(mSpeakerFL, mSpeakerFR, mSpeakerC, mSpeakerLFE, mSpeakerBL, mSpeakerBR, mSpeakerSL, mSpeakerSR);
            if (result == FMOD_OK)
            {
                result = result2;
            }
        }
    }

    return result;
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
FMOD_RESULT ChannelI::getSpeakerMix(float *frontleft, float *frontright, float *center, float *lfe, float *backleft, float *backright, float *sideleft, float *sideright)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (frontleft)
    {
        *frontleft = mSpeakerFL;
    }
    if (frontright)
    {
        *frontright = mSpeakerFR;
    }
    if (center)
    {
        *center = mSpeakerC;
    }
    if (lfe)
    {
        *lfe = mSpeakerLFE;
    }
    if (backleft)
    {
        *backleft = mSpeakerBL;
    }
    if (backright)
    {
        *backright = mSpeakerBR;
    }
    if (sideleft)
    {
        *sideleft = mSpeakerSL;
    }
    if (sideright)
    {
        *sideright = mSpeakerSR;
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
// OFFSET: 0x008DE100
FMOD_RESULT ChannelI::setSpeakerLevels(FMOD_SPEAKER speaker, float *levels, int numlevels, bool calldriver)
{
    FMOD_RESULT result = FMOD_OK;
    int count;
    float actuallevels[DSP_MAXLEVELS_IN];

    if (!levels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (numlevels > mSystem->mMaxInputChannels)
    {
        return FMOD_ERR_TOOMANYCHANNELS;
    }

    if (mSpeakerMode == FMOD_SPEAKERMODE_PROLOGIC)
    {
        if (speaker < 0 || speaker >= 6)
        {
            return FMOD_ERR_INVALID_PARAM;
        }
    }
    else
    {
        if (speaker < 0 || speaker >= mSystem->mMaxOutputChannels)
        {
            return FMOD_ERR_INVALID_PARAM;
        }
    }

    switch (mSpeakerMode)
    {
        case FMOD_SPEAKERMODE_MONO:
        {
            if (speaker != FMOD_SPEAKER_MONO)
            {
                return FMOD_ERR_INVALID_SPEAKER;
            }
            break;
        }
        case FMOD_SPEAKERMODE_STEREO:
        {
            if (speaker != FMOD_SPEAKER_FRONT_LEFT && speaker != FMOD_SPEAKER_FRONT_RIGHT)
            {
                return FMOD_ERR_INVALID_SPEAKER;
            }
            break;
        }
        case FMOD_SPEAKERMODE_QUAD:
        {
            if (speaker != FMOD_SPEAKER_FRONT_LEFT && speaker != FMOD_SPEAKER_FRONT_RIGHT &&
                speaker != FMOD_SPEAKER_BACK_LEFT  && speaker != FMOD_SPEAKER_BACK_RIGHT)
            {
                return FMOD_ERR_INVALID_SPEAKER;
            }

            if (speaker == FMOD_SPEAKER_BACK_LEFT)
            {
                speaker = (FMOD_SPEAKER)2;
            }
            if (speaker == FMOD_SPEAKER_BACK_RIGHT)
            {
                speaker = (FMOD_SPEAKER)3;
            }
            break;
        }
        case FMOD_SPEAKERMODE_SURROUND:
        {
            if (speaker != FMOD_SPEAKER_FRONT_LEFT   && speaker != FMOD_SPEAKER_FRONT_RIGHT &&
                speaker != FMOD_SPEAKER_BACK_LEFT    && speaker != FMOD_SPEAKER_BACK_RIGHT &&
                speaker != FMOD_SPEAKER_FRONT_CENTER)
            {
                return FMOD_ERR_INVALID_SPEAKER;
            }
            break;
        }
        case FMOD_SPEAKERMODE_PROLOGIC:
        case FMOD_SPEAKERMODE_5POINT1:
        {
            if (speaker != FMOD_SPEAKER_FRONT_LEFT   && speaker != FMOD_SPEAKER_FRONT_RIGHT &&
                speaker != FMOD_SPEAKER_BACK_LEFT    && speaker != FMOD_SPEAKER_BACK_RIGHT &&
                speaker != FMOD_SPEAKER_FRONT_CENTER && speaker != FMOD_SPEAKER_LOW_FREQUENCY)
            {
                return FMOD_ERR_INVALID_SPEAKER;
            }
            break;
        }
        case FMOD_SPEAKERMODE_7POINT1:
        {
            if (speaker > FMOD_SPEAKER_SIDE_RIGHT)
            {
                return FMOD_ERR_INVALID_SPEAKER;
            }
            break;
        }
        default:
        {
            break;
        }
    };

    FMOD_memset(actuallevels, 0, DSP_MAXLEVELS_IN * sizeof(float));

    if (!mLevels)
    {
        mSystem->mSpeakerLevelsPool.alloc(&mLevels);
        if (!mLevels)
        {
            return FMOD_ERR_MEMORY;
        }
    }

    for (count = 0; count < numlevels; count++)
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

        mLevels[(speaker * mSystem->mMaxInputChannels) + count] = volume;

        actuallevels[count] = volume;
    }

    mLastPanMode = FMOD_CHANNEL_PANMODE_SPEAKERLEVELS;

    if (mRealChannel[0]->mMode & FMOD_3D)
    {
        return FMOD_OK; /* Just return.  The 3d system will pick up the actual driver call later. */
    }  
    else if (calldriver)
    {
        for (count = 0; count < mNumRealChannels; count++)
        {
            FMOD_RESULT result2;

            result2 = mRealChannel[count]->setSpeakerLevels(speaker, actuallevels, numlevels);
            if (result == FMOD_OK)
            {
                result = result2;
            }
        }
    }

    return result;
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
// OFFSET: 0x008DE460
FMOD_RESULT ChannelI::getSpeakerLevels(FMOD_SPEAKER speaker, float *levels, int numlevels)
{
    int count;

    if (!levels || !numlevels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (speaker < 0 || speaker >= mSystem->mMaxOutputChannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mLevels)
    {
        for (count = 0; count < numlevels; count++)
        {
            levels[count] = mLevels[(mSystem->mMaxInputChannels * speaker) + count];
        }
    }
    else
    {
        for (count = 0; count < numlevels; count++)
        {
            levels[count] = 0;
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
FMOD_RESULT ChannelI::setInputChannelMix(float *levels, int numlevels)
{
    int count, diff;

    if (numlevels > FMOD_CHANNEL_MAXINPUTCHANNELS)
    {
        return FMOD_ERR_TOOMANYCHANNELS;
    }

    if (!levels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    diff = 0;
    for (count = 0; count < numlevels; count++)
    {
        if (mInputMix[count] != levels[count])
        {
            diff++;
        }
        mInputMix[count] = levels[count];
    }
    
    if (!diff && (mFlags & CHANNELI_FLAG_USEDINPUTMIX))
    {
        return FMOD_OK;
    }

    mFlags |= CHANNELI_FLAG_USEDINPUTMIX;

    setVolume(mVolume);

    if (mLastPanMode == FMOD_CHANNEL_PANMODE_PAN)
    {
        setPan(mPan, true);
    }
    else if (mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERMIX)
    {
        setSpeakerMix(mSpeakerFL,  mSpeakerFR,  mSpeakerC,  mSpeakerLFE,  mSpeakerBL,  mSpeakerBR,  mSpeakerSL,  mSpeakerSR, true);
    }
    else if (mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERLEVELS)
    {
        int count;

        if (mLevels)
        {
            for (count = 0; count < mSystem->mMaxOutputChannels; count++)
            {
                setSpeakerLevels((FMOD_SPEAKER)count, &mLevels[count * mSystem->mMaxOutputChannels], mSystem->mMaxInputChannels, true);
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
FMOD_RESULT ChannelI::getInputChannelMix(float *levels, int numlevels)
{
    int count;

    if (numlevels > FMOD_CHANNEL_MAXINPUTCHANNELS)
    {
        return FMOD_ERR_TOOMANYCHANNELS;
    }

    if (!levels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    for (count = 0; count < numlevels; count++)
    {
        levels[count] = mInputMix[count];
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
// OFFSET: 0x008DE5A0
FMOD_RESULT ChannelI::setMute(bool mute)
{
    FMOD_RESULT result = FMOD_OK;
    ChannelGroupI *current;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mute)
    {
        mFlags |= CHANNELI_FLAG_MUTED;
    }
    else
    {
        mFlags &= ~CHANNELI_FLAG_MUTED;
    }

	/*
		Scan up through channelgroup heirarchy.  If any parent group is muted, then the channel should be muted.
	*/
	current = mChannelGroup;
	do
	{
		if (current->mMute)
		{
			mute = true;
			break;
		}
		current = current->mParent;
	} while (current);

	if (mute)
    {
        int count;

        mFlags |= CHANNELI_FLAG_REALMUTE;

        for (count = 0; count < mNumRealChannels; count++)
        {
            FMOD_RESULT result2;

            result2 = mRealChannel[count]->setVolume(0);
            if (result == FMOD_OK)
            {
                result = result2;
            }
        }

        if (result == FMOD_OK)
        {
            result = updatePosition();
        }
    }
	else
    {
        mFlags &= ~CHANNELI_FLAG_REALMUTE;

        result = setVolume(mVolume, true);
    }

	return result;
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
FMOD_RESULT ChannelI::getMute(bool *mute)
{
    if (!mute)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mFlags & CHANNELI_FLAG_MUTED)
    {
        *mute = true;
    }
    else
    {
        *mute = false;
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
// OFFSET: 0x008E0700
FMOD_RESULT ChannelI::setPriority(int priority)
{
    if (priority < 0 || priority > FMOD_MAXPRIORITIES)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mPriority = priority;

    return updatePosition();
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
// OFFSET: 0x008DE660
FMOD_RESULT ChannelI::getPriority(int *priority)
{
    if (!priority)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *priority = mPriority;

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
FMOD_RESULT ChannelI::setLowPassGain(float gain)
{
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (gain < 0)
    {
        gain = 0;
    }
    if (gain > 1.0f)
    {
        gain = 1.0f;
    }

    mLowPassGain = gain;

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result = FMOD_OK;

        result = mRealChannel[count]->setLowPassGain(gain);
        if (result == FMOD_OK)
        {
            return result;
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
FMOD_RESULT ChannelI::getLowPassGain(float *gain)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!gain)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *gain = mLowPassGain;

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
// OFFSET: 0x008DE680
FMOD_RESULT ChannelI::set3DAttributes(const FMOD_VECTOR *pos, const FMOD_VECTOR *vel)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (pos)
    {
        #ifdef FMOD_DEBUG
        {
            FMOD_RESULT result;

            result = FMOD_CHECKFLOAT(pos->x);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = FMOD_CHECKFLOAT(pos->y);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = FMOD_CHECKFLOAT(pos->z);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        #endif

        if (mPosition3D.x != pos->x ||
            mPosition3D.y != pos->y ||
            mPosition3D.z != pos->z)
        {
            mFlags |= CHANNELI_FLAG_MOVED;
        }
        mPosition3D     = *pos;
    }

    if (vel)
    {
        #ifdef FMOD_DEBUG
        {
            FMOD_RESULT result;

            result = FMOD_CHECKFLOAT(vel->x);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = FMOD_CHECKFLOAT(vel->y);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = FMOD_CHECKFLOAT(vel->z);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        #endif

        if (mVelocity3D.x != vel->x ||
            mVelocity3D.y != vel->y ||
            mVelocity3D.z != vel->z)
        {
            mFlags |= CHANNELI_FLAG_MOVED;
        }
		mVelocity3D = *vel;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_OK;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2;

        result2 = mRealChannel[count]->set3DAttributes();
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }

    return result;
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
// OFFSET: 0x008DE7B0
FMOD_RESULT ChannelI::get3DAttributes(FMOD_VECTOR *pos, FMOD_VECTOR *vel)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (pos)
    {
        pos->x = mPosition3D.x;
        pos->y = mPosition3D.y;
        pos->z = mPosition3D.z;
    }

    if (vel)
    {
        vel->x = mVelocity3D.x;
        vel->y = mVelocity3D.y;
        vel->z = mVelocity3D.z;
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
// OFFSET: 0x008E0CC0
FMOD_RESULT ChannelI::set3DMinMaxDistance(float mindistance, float maxdistance)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    FMOD_MODE mode = mRealChannel[0]->mMode;

    if (!(mode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

	if (mindistance < 0 || maxdistance < 0 || maxdistance < mindistance)
	{
		return FMOD_ERR_INVALID_PARAM;
	}

    if (mMinDistance != mindistance || 
        mMaxDistance != maxdistance)
    {
        mMinDistance = mindistance;
        mMaxDistance = maxdistance;

        if (mode & (FMOD_3D_LOGROLLOFF | FMOD_3D_LINEARROLLOFF | FMOD_3D_CUSTOMROLLOFF) || mSystem->mRolloffCallback)
        {
            mFlags |= CHANNELI_FLAG_MOVED;

            result = update(0);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = setVolume(mVolume);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else
        {
            for (count = 0; count < mNumRealChannels; count++)
            {
                FMOD_RESULT result2;

                result2 = mRealChannel[count]->set3DMinMaxDistance();
                if (result == FMOD_OK)
                {
                    result = result2;
                }
            }
    
            mFlags |= CHANNELI_FLAG_MOVED;
        }
    }

    return result;
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
// OFFSET: 0x008DE820
FMOD_RESULT ChannelI::get3DMinMaxDistance(float *mindistance, float *maxdistance)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (mindistance)
    {
        *mindistance = mMinDistance;
    }

    if (maxdistance)
    {
        *maxdistance = mMaxDistance;
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
// OFFSET: 0x008DE870
FMOD_RESULT ChannelI::set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume)
{
    FMOD_RESULT result = FMOD_OK;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (insideconeangle > outsideconeangle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (outsidevolume > 1.0f)
    {
        outsidevolume = 1.0f;
    }
    if (outsidevolume < 0.0f)
    {
        outsidevolume = 0.0f;
    }

    mConeInsideAngle = insideconeangle;
    mConeOutsideAngle = outsideconeangle;
    mConeOutsideVolume = outsidevolume;

    mFlags |= CHANNELI_FLAG_MOVED;

    return result;
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
FMOD_RESULT ChannelI::get3DConeSettings(float *insideconeangle, float *outsideconeangle, float *outsidevolume)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (insideconeangle)
    {
        *insideconeangle = mConeInsideAngle;
    }

    if (outsideconeangle)
    {
        *outsideconeangle = mConeOutsideAngle;
    }

    if (outsidevolume)
    {
        *outsidevolume = mConeOutsideVolume;
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
// OFFSET: 0x008DE8F0
FMOD_RESULT ChannelI::set3DConeOrientation(FMOD_VECTOR *orientation)
{
    FMOD_RESULT result = FMOD_OK;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (!orientation)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mConeOrientation = *orientation;

    mFlags |= CHANNELI_FLAG_MOVED;

    return result;
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
FMOD_RESULT ChannelI::get3DConeOrientation(FMOD_VECTOR *orientation)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (orientation)
    {
        *orientation = mConeOrientation;
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
FMOD_RESULT ChannelI::set3DCustomRolloff(FMOD_VECTOR *points, int numpoints)
{
    if (numpoints < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (points)
    {
        int count;

        for (count = 1; count < numpoints; count++)
        {
            if (points[count].x <= points[count-1].x)
            {
                return FMOD_ERR_INVALID_PARAM;
            }
            if (points[count].y < 0 || points[count].y > 1)
            {
                return FMOD_ERR_INVALID_PARAM;
            }
        }
    }

    mRolloffPoint = points;
    mNumRolloffPoints = numpoints;

    mFlags |= CHANNELI_FLAG_MOVED;

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
FMOD_RESULT ChannelI::get3DCustomRolloff(FMOD_VECTOR **points, int *numpoints)
{
    if (points)
    {
        *points = mRolloffPoint;
    }

    if (numpoints)
    {
        *numpoints = mNumRolloffPoints;
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
// OFFSET: 0x008E0730
FMOD_RESULT ChannelI::set3DOcclusionInternal(float direct, float reverb, bool resettarget)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (direct < 0)
    {
        direct = 0;
    }
    if (reverb < 0)
    {
        reverb = 0;
    }
    if (direct > 1)
    {
        direct = 1;
    }
    if (reverb > 1)
    {
        reverb = 1;
    }

    mDirectOcclusion = direct;
    mReverbOcclusion = reverb;

    if (resettarget)
    {
        mDirectOcclusionTarget = mDirectOcclusion;
        mReverbOcclusionTarget = mReverbOcclusion;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->set3DOcclusion(direct, reverb);
    }

    result = updatePosition();
    if (result != FMOD_OK)
    {
        return result;
    }
    
	return result;
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
FMOD_RESULT ChannelI::set3DOcclusion(float direct, float reverb)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (direct < 0)
    {
        direct = 0;
    }
    if (reverb < 0)
    {
        reverb = 0;
    }
    if (direct > 1)
    {
        direct = 1;
    }
    if (reverb > 1)
    {
        reverb = 1;
    }

    mUserDirectOcclusion = direct;
    mUserReverbOcclusion = reverb;

    // recalculate real channel occlusion values
    CHECK_RESULT(set3DOcclusionInternal(mDirectOcclusion, mReverbOcclusion, false));
    
	return FMOD_OK;
}


FMOD_RESULT	ChannelI::get3DOcclusionInternal(float *directOcclusion, float *reverbOcclusion)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (directOcclusion)
    {
        *directOcclusion = mDirectOcclusion;
    }
    if (reverbOcclusion)
    {
        *reverbOcclusion = mReverbOcclusion;
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
FMOD_RESULT ChannelI::get3DOcclusion(float *directOcclusion, float *reverbOcclusion)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (directOcclusion)
    {
        *directOcclusion = mUserDirectOcclusion;
    }
    if (reverbOcclusion)
    {
        *reverbOcclusion = mUserReverbOcclusion;
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
FMOD_RESULT ChannelI::set3DSpread(float angle)
{
    FMOD_RESULT result = FMOD_OK;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (angle < 0.0f || angle > 360.0f)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mSpread = angle;

	return result;
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
FMOD_RESULT ChannelI::get3DSpread(float *angle)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (!angle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *angle = mSpread;

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
// OFFSET: 0x008E0E00
FMOD_RESULT ChannelI::set3DPanLevel(float level)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (level < 0.0f || level > 1.0f)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    m3DPanLevel = level;

    if (mRealChannel[0]->mFlags & CHANNELREAL_FLAG_PAUSED && m3DPanLevel < 1.0f)
    {
        return update(0);    /* If there is a 2d component and we are still in play paused mode, update the speaker levels from what was set in the initial play. */
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
// OFFSET: 0x008DE950
FMOD_RESULT ChannelI::get3DPanLevel(float *level)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (!level)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *level = m3DPanLevel;

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
FMOD_RESULT ChannelI::set3DDopplerLevel(float level)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (level < 0.0f || level > 5.0f)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    m3DDopplerLevel = level;

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
FMOD_RESULT ChannelI::get3DDopplerLevel(float *level)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (!level)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *level = m3DDopplerLevel;

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
// OFFSET: 0x008DE990
FMOD_RESULT ChannelI::setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES *prop)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2;

        result2 = mRealChannel[count]->setReverbProperties(prop);
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }

    return result;
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
// OFFSET: 0x008DE9F0
FMOD_RESULT ChannelI::getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES *prop)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2;

        result2 = mRealChannel[count]->getReverbProperties(prop);
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }

    return result;
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
// OFFSET: 0x008DEA50
FMOD_RESULT ChannelI::setChannelGroupInternal(ChannelGroupI *channelgroup, bool resetattributes, bool forcedspreconnect)
{
    FMOD_RESULT result = FMOD_OK;
    ChannelGroupI *oldchannelgroup = mChannelGroup; 
    int count;

    if (mChannelGroup)
    {
        if (mChannelGroupNode.isEmpty())
        {
            return FMOD_OK;                 /* Possibly doesn't belong to a channel group such as mod/s3m/xm/it/midi and shouldnt be returned to the master list. */
        }

        mChannelGroup->mNumChannels--;
        mChannelGroupNode.removeNode();
    }

    if (!channelgroup)
    {
        channelgroup = mSystem->mChannelGroup;
    }

    mChannelGroup = channelgroup;
    mChannelGroupNode.addAfter(&mChannelGroup->mChannelHead);
    mChannelGroupNode.setData(this);
    mChannelGroup->mNumChannels++;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    /*
        Fix up DSP connection inconsistancies, like volume/freq/pan etc for connections.
    */
    if (resetattributes)
    {
        float levels[DSP_MAXLEVELS_OUT][DSP_MAXLEVELS_IN];
  
        if (mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERLEVELS)
        {
            for (count = 0; count < mSystem->mMaxOutputChannels; count++)
            {
                getSpeakerLevels((FMOD_SPEAKER)count, levels[count], mSystem->mMaxInputChannels);
            }
        }
        
        for (count = 0; count < mNumRealChannels; count++)
        {
            FMOD_RESULT result2;

            result2 = mRealChannel[count]->moveChannelGroup(oldchannelgroup, mChannelGroup, forcedspreconnect);
            if (result == FMOD_OK)
            {
                result = result2;
            }
        }

        /* 
            If channelgroup has overriden mute/paused = true, then mute this voice, otherwise leave it at whatever the channel mute setting was set to. 
        */
        result = setMute(mFlags & CHANNELI_FLAG_MUTED ? true : false);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = setPaused(mFlags & CHANNELI_FLAG_PAUSED ? true : false);
        if (result != FMOD_OK)
        {
            return result;
        }

        /*
            Because the connection has been terminated, with the pan and volume in the old connection,
            we have to reset the pan and volume in the new connection.
        */
        setVolume(mVolume);
    
        if (!(mRealChannel[0]->mMode & FMOD_3D))
        {
            if (mLastPanMode == FMOD_CHANNEL_PANMODE_PAN)
            {
                setPan(mPan);
            }
            else if (mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERMIX)
            {
                setSpeakerMix(mSpeakerFL, mSpeakerFR, mSpeakerC, mSpeakerLFE, mSpeakerBL, mSpeakerBR, mSpeakerSL, mSpeakerSR);
            }
            else if (mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERLEVELS)
            {
                for (count = 0; count < mSystem->mMaxOutputChannels; count++)
                {
                    setSpeakerLevels((FMOD_SPEAKER)count, levels[count], mSystem->mMaxInputChannels);
                }
            }
        }

        setFrequency(mFrequency);
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
// OFFSET: 0x008DECC0
FMOD_RESULT ChannelI::setChannelGroup(ChannelGroupI *channelgroup)
{
    return setChannelGroupInternal(channelgroup, true);
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
FMOD_RESULT ChannelI::getChannelGroup(ChannelGroupI **channelgroup)
{
    if (!channelgroup)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *channelgroup = mChannelGroup;

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
// OFFSET: 0x008DECE0
FMOD_RESULT ChannelI::isPlaying(bool *isplaying)
{
    FMOD_RESULT result;
    int count;

    if (!isplaying)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *isplaying = false;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mRealChannel[0]->mFlags & CHANNELREAL_FLAG_STOPPED)
    {
        *isplaying = false;
        return FMOD_OK;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        bool isplayingtemp;

        result = mRealChannel[count]->isPlaying(&isplayingtemp);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (isplayingtemp) /* If any are playing, final isplaying = true.  Dont let the last channel dictate if the whole voice is on or not. */
        {
            *isplaying = true;
            break;
        }
    }

    if (!*isplaying)
    {
        if (mEndDelay)
        {
            mFlags |= CHANNELI_FLAG_ENDDELAY;
            *isplaying = true;
            return FMOD_OK;
        }
    }


    /*
        A bit of a helper, isplaying can force the sound to the end of the list if the user happens to call it.
    */
    if (!*isplaying)
    {
        mListPosition = (unsigned int)-1;

        if (mSortedListNode.getData()) /* Might not be part of a list. */
        {
            mSortedListNode.removeNode();
            mSortedListNode.addBefore(&mSystem->mChannelSortedListHead);        
            mSortedListNode.setData(this);
        }
        if (mSoundGroupSortedListNode.getData()) /* Might not be part of a list. */
        {
            SoundI *sound = mRealChannel[0]->mSound;
            
            if (sound)
            {
                mSoundGroupSortedListNode.removeNode();
				if( sound->mSoundGroup )
					mSoundGroupSortedListNode.addBefore(&sound->mSoundGroup->mChannelListHead);
                mSoundGroupSortedListNode.setData(this);
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
// OFFSET: 0x008DEDE0
FMOD_RESULT ChannelI::isVirtual(bool *isvirtual)
{
    if (!isvirtual)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        *isvirtual = false;
        return FMOD_ERR_INVALID_HANDLE;
    }

    return mRealChannel[0]->isVirtual(isvirtual);
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
FMOD_RESULT  ChannelI::getAudibilityInternal(float *audibility, bool usefadevolume)
{
    if (!audibility)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mFlags & CHANNELI_FLAG_MUTED)
    {
        *audibility = 0;
        return FMOD_OK;
    }

    if (mRealChannel[0]->mMode & FMOD_3D)
    {
        if (m3DPanLevel < 1.0f)
        {
            /* Sound is partially 2D */
            float non3Dportion = 1.0f - m3DPanLevel;
            *audibility = mVolume * 
                          (non3Dportion + m3DPanLevel * mVolume3D) * 
                          (non3Dportion + m3DPanLevel * mConeVolume3D) *
                          (non3Dportion + m3DPanLevel * (1.0f - mDirectOcclusion)) *
                          (non3Dportion + m3DPanLevel * (1.0f - mUserDirectOcclusion)) *
                          (usefadevolume ? mFadeVolume : 1.0f) *
                          (non3Dportion + m3DPanLevel * mChannelGroup->mRealDirectOcclusionVolume) *
                          mReverbDryVolume *
                          mChannelGroup->mRealVolume;
        }
        else
        {
            *audibility = mVolume * 
                          mVolume3D * 
                          mConeVolume3D *
                          (1.0f - mDirectOcclusion) *
                          (1.0f - mUserDirectOcclusion) *
                          (usefadevolume ? mFadeVolume : 1.0f) *
                          mChannelGroup->mRealDirectOcclusionVolume *
                          mReverbDryVolume *
                          mChannelGroup->mRealVolume;
        }
    }
    else
    {
        *audibility = mVolume * mChannelGroup->mRealVolume * (usefadevolume ? mFadeVolume : 1.0f);
    }

    return FMOD_OK;
}


FMOD_RESULT F_API ChannelI::getFinalFrequency(float *frequency)
{
    *frequency = mFrequency *
                 mChannelGroup->mRealPitch *
                 ((1.0f - m3DPanLevel) + (mPitch3D * m3DPanLevel));

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
// OFFSET: 0x008DEE20
FMOD_RESULT  ChannelI::getAudibility(float *audibility)
{
    return getAudibilityInternal(audibility, true);
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
// OFFSET: 0x008DEEC0
FMOD_RESULT ChannelI::getCurrentSound(SoundI **sound)
{
    if (!sound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        *sound = 0;
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mRealChannel[0]->mSound)
    {
        *sound = mRealChannel[0]->mSound->mSubSampleParent;
    }
    else
    {
        *sound = 0;
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
// OFFSET: 0x008DEF10
FMOD_RESULT ChannelI::getCurrentDSP(DSPI **dsp)
{
    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        *dsp = 0;
        return FMOD_ERR_INVALID_HANDLE;
    }

    *dsp = mRealChannel[0]->mDSP;

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
FMOD_RESULT ChannelI::getSpectrum(float *spectrumarray, int numvalues, int channel, FMOD_DSP_FFT_WINDOW windowtype)
{
#ifdef FMOD_SUPPORT_GETSPECTRUM
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mNumRealChannels > 1)
    {
        if (channel >= mNumRealChannels || channel < 0)
        {
            return FMOD_ERR_INVALID_PARAM;
        }

        return mRealChannel[channel]->getSpectrum(spectrumarray, numvalues, 0, windowtype);
    }
    else
    {
        return mRealChannel[0]->getSpectrum(spectrumarray, numvalues, channel, windowtype);
    }
#else
    return FMOD_ERR_UNSUPPORTED;
#endif
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
FMOD_RESULT ChannelI::getWaveData(float *wavearray, int numvalues, int channel)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mNumRealChannels > 1)
    {
        if (channel >= mNumRealChannels || channel < 0)
        {
            return FMOD_ERR_INVALID_PARAM;
        }

        return mRealChannel[channel]->getWaveData(wavearray, numvalues, 0);
    }
    else
    {
        return mRealChannel[0]->getWaveData(wavearray, numvalues, channel);
    }
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
FMOD_RESULT ChannelI::getIndex(int *index)
{
    if (!index)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *index = mIndex;

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
// OFFSET: 0x008DEF40
FMOD_RESULT ChannelI::setCallback(FMOD_CHANNEL_CALLBACK callback)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    mCallback = callback;

#if 0
    if (type == FMOD_CHANNEL_CALLBACKTYPE_VIRTUALVOICE)
    {
        FMOD_RESULT result;
        bool isvirtual = false;

        result = isVirtual(&isvirtual);
        if (result != FMOD_OK)
        {
            return result;
        }

        mCallback((FMOD_CHANNEL *)((FMOD_UINT_NATIVE)mHandleCurrent), type, isvirtual ? 1 : 0, 0);
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
// OFFSET: 0x008DFA80
FMOD_RESULT ChannelI::setPosition(unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mRealChannel[0]->mSound)
    {
        SoundI *soundi = mRealChannel[0]->mSound->mSubSampleParent;
        unsigned int   length;
        
#ifdef FMOD_SUPPORT_SENTENCING
        if (postype == FMOD_TIMEUNIT_SENTENCE_MS || 
            postype == FMOD_TIMEUNIT_SENTENCE_PCM ||
            postype == FMOD_TIMEUNIT_SENTENCE_PCMBYTES ||
            postype == FMOD_TIMEUNIT_SENTENCE_SUBSOUND)
        {
            unsigned int  sentenceid, subsound;

            if (!soundi->mSubSoundList)
            {
                return FMOD_ERR_INVALID_PARAM;
            }
    
            if (postype == FMOD_TIMEUNIT_SENTENCE_SUBSOUND)
            {
                if (position >= (unsigned int)soundi->mSubSoundListNum)
                {
                    return FMOD_ERR_INVALID_POSITION;
                }

                sentenceid = position;
                postype    = FMOD_TIMEUNIT_PCM;
                position   = 0;
            }
            else
            {
                result = getPosition(&sentenceid, FMOD_TIMEUNIT_SENTENCE);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }

            subsound = soundi->mSubSoundList[sentenceid].mIndex;

            if (postype == FMOD_TIMEUNIT_SENTENCE_MS)
            {
                postype = FMOD_TIMEUNIT_MS;
            }
            else if (postype == FMOD_TIMEUNIT_SENTENCE_PCM)
            {
                postype = FMOD_TIMEUNIT_PCM;
            }
            else if (postype == FMOD_TIMEUNIT_SENTENCE_PCMBYTES)
            {
                postype = FMOD_TIMEUNIT_PCMBYTES;
            }
            

            if (soundi->mSubSoundShared)
            {
                FMOD_CODEC_WAVEFORMAT waveformat;

                soundi->mCodec->mDescription.getwaveformat(soundi->mCodec, subsound, &waveformat);

                length = waveformat.lengthpcm;

                if (postype == FMOD_TIMEUNIT_MS)
                {
                    position = (unsigned int)((float)position / 1000.0f * waveformat.frequency);
                }
                else if (postype == FMOD_TIMEUNIT_PCMBYTES)
                {
                    SoundI::getSamplesFromBytes(position, &position, waveformat.channels, waveformat.format);
                }

                postype = FMOD_TIMEUNIT_PCM;
            }
            else
            {
                result = soundi->mSubSound[subsound]->getLength(&length, postype);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }

            if (position >= length)
            {
                return FMOD_ERR_INVALID_POSITION;
            }

            {
                unsigned int count;

                for (count = 0; count < sentenceid; count++)
                {
                    if (soundi->mSubSoundShared)
                    {
                        FMOD_CODEC_WAVEFORMAT waveformat;

                        soundi->mCodec->mDescription.getwaveformat(soundi->mCodec, soundi->mSubSoundList[count].mIndex, &waveformat);

                        length = waveformat.lengthpcm;
                    }
                    else
                    {
                        SoundI *sound = soundi->mSubSound[soundi->mSubSoundList[count].mIndex];

                        sound->getLength(&length, postype);
                    }

                    position += length;
                }
            }
        }
        else
#endif
        {
            result = soundi->getLength(&length, postype);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (position >= length)
            {
                return FMOD_ERR_INVALID_POSITION;
            }
        }

        /*
            If a sentence, recalculate the sentence current value in the setposition.
        */
#ifdef FMOD_SUPPORT_SENTENCING
        if (soundi->mSubSound && soundi->mSubSoundList)
        {
            int count;
            unsigned int offset = 0, pos = position;

            if (postype == FMOD_TIMEUNIT_MS)
            {
                pos = (unsigned int)((float)position / 1000.0f * soundi->mDefaultFrequency);
            }
            else if (postype == FMOD_TIMEUNIT_PCMBYTES)
            {
                soundi->getSamplesFromBytes(pos, &pos);
            }


            for (count = 0; count < soundi->mSubSoundListNum; count++)
            {     
                SoundI *subsound = soundi->mSubSound[soundi->mSubSoundList[count].mIndex];

                if (subsound)
                {
                    FMOD_RESULT result;
                    unsigned int lengthpcm;

                    if (soundi->mSubSoundShared)
                    {
                        FMOD_CODEC_WAVEFORMAT waveformat;

                        result = soundi->mCodec->mDescription.getwaveformat(soundi->mCodec, soundi->mSubSoundList[count].mIndex, &waveformat);
                        if (result != FMOD_OK)
                        {
                            return result;
                        }

                        lengthpcm = waveformat.lengthpcm;
                    }
                    else
                    {
                        lengthpcm = subsound->mLength;
                    }

                    if (pos >= offset && pos < offset + lengthpcm)
                    {
                        int count2;

                        for (count2 = 0; count2 < mNumRealChannels; count2++)
                        {
                            mRealChannel[count2]->mSubSoundListCurrent = count;
                        }
                        break;
                    }

                    offset += lengthpcm;
                }
            }
        }
#endif
    }


    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2;

        result2 = mRealChannel[count]->setPosition(position, postype);
        if (result2 != FMOD_OK && result2 != FMOD_ERR_INVALID_POSITION)
        {
            return result2;
        }
    }

    result = updateSyncPoints(true);
    if (result != FMOD_OK)
    {
        return result;
    }

    return result;
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
// OFFSET: 0x008DEFD0
FMOD_RESULT ChannelI::getPosition(unsigned int *position, FMOD_TIMEUNIT postype)
{   
    if (!position)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    return mRealChannel[0]->getPosition(position, postype);
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
FMOD_RESULT ChannelI::getDSPHead(DSPI **dsp)
{   
    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    return mRealChannel[0]->getDSPHead(dsp);
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
// OFFSET: 0x008DF010
FMOD_RESULT ChannelI::addDSP(DSPI *dsp, DSPConnectionI **dspconnection)
{   
    FMOD_RESULT result;
    DSPI *dsphead;
 
    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
 
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    #ifdef PLATFORM_PS3
    if (mRealChannel[0]->mSound && mRealChannel[0]->mSound->mChannels > 8)
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "ChannelI::addDSP", "Cannot addDSP to channel with more than 8 channels. Please use a Channel Group.\n"));
        return FMOD_ERR_TOOMANYCHANNELS;
    }
    #endif

    result = getDSPHead(&dsphead);
    if (result != FMOD_OK)
    {
        return result;
    }
 
    result = dsphead->insertInputBetween(dsp, 0, false, dspconnection);
    if (result != FMOD_OK)
    {
        return result;
    }

    mAddDSPHead = dsp;
   
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
// OFFSET: 0x008E0E90
FMOD_RESULT ChannelI::setMode(FMOD_MODE mode)
{
    FMOD_RESULT result;
    int count;
    FMOD_MODE oldmode;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    oldmode = mRealChannel[0]->mMode;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setMode(mode);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (mRealChannel[0]->mMode & FMOD_SOFTWARE)
    {
        if (!(oldmode & FMOD_2D) && mode & FMOD_2D) /* Reset the pan/speaker level settings. */
        {
            unsigned int channelmask = 0;
            int channels = 0;

            result = setVolume(mVolume);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (mRealChannel[0]->mSound)
            {
                SoundI *soundi = SAFE_CAST(SoundI, mRealChannel[0]->mSound);

                channelmask = soundi->mDefaultChannelMask;
                channels = soundi->mChannels;
            }

            if (channelmask & SPEAKER_WAVEFORMAT_MASK)
            {
                int speaker, incount;
                unsigned int maskbit = 1;

                incount = 0;
                for (speaker = 0; speaker < mSystem->mMaxOutputChannels; speaker++)
                {
                    if (channelmask & maskbit)
                    {
                        float clevels[DSP_MAXLEVELS_IN];

                        FMOD_memset(clevels, 0, DSP_MAXLEVELS_IN * sizeof(float));
                
                        clevels[incount] = 1.0f;

                        setSpeakerLevels((FMOD_SPEAKER)speaker, clevels, channels);

                        incount++;
                        if (incount >= channels)
                        {
                            break;
                        }
                    }

                    maskbit <<= 1;
                }
            }
            else
            {
                if (mLastPanMode == FMOD_CHANNEL_PANMODE_PAN)
                {
                    setPan(mPan, true);
                }
                else if (mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERMIX)
                {
                    setSpeakerMix(mSpeakerFL,  mSpeakerFR,  mSpeakerC,  mSpeakerLFE,  mSpeakerBL,  mSpeakerBR,  mSpeakerSL,  mSpeakerSR, true);
                }
                else if (mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERLEVELS)
                {
                    if (mLevels)
                    {
                        for (count = 0; count < mSystem->mMaxOutputChannels; count++)
                        {
                            setSpeakerLevels((FMOD_SPEAKER)count, &mLevels[count * mSystem->mMaxOutputChannels], mSystem->mMaxInputChannels, true);
                        }
                    }
                }
            }
        }
        else if (!(oldmode & FMOD_3D) && mode & FMOD_3D)
        {
            FMOD_VECTOR pos = mPosition3D;

            mPosition3D.x += 1.0f;  /* Make it different so that it updates. */

            result = set3DAttributes(&pos, &mVelocity3D);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }
    else
    {
        if ((mode & FMOD_3D) && (mode & (FMOD_3D_LOGROLLOFF | FMOD_3D_LINEARROLLOFF | FMOD_3D_CUSTOMROLLOFF) || mSystem->mRolloffCallback))
        {
            result = set3DAttributes(&mPosition3D, &mVelocity3D);
            if (result != FMOD_OK)
            {
                return result;
            }

            mFlags |= CHANNELI_FLAG_MOVED;

            result = update(0);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = setVolume(mVolume);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    if ((mode & FMOD_3D_IGNOREGEOMETRY) != (oldmode & FMOD_3D_IGNOREGEOMETRY))
    {
        if( mode & FMOD_3D_IGNOREGEOMETRY )
        {
            set3DOcclusionInternal(mUserDirectOcclusion, mUserReverbOcclusion);
        }
        else
        {
            mFlags |= CHANNELI_FLAG_MOVED;
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
// OFFSET: 0x008DF270
FMOD_RESULT ChannelI::getMode(FMOD_MODE *mode)
{
    if (!mode)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    *mode = mRealChannel[0]->mMode;

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
// OFFSET: 0x008DF2A0
FMOD_RESULT ChannelI::setLoopCount(int loopcount)
{
    FMOD_RESULT result = FMOD_OK;
    int count;
    
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (loopcount < -1)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2;

        result2 = mRealChannel[count]->setLoopCount(loopcount);
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }

    return result;
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
FMOD_RESULT ChannelI::getLoopCount(int *loopcount)
{
    if (!loopcount)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    *loopcount = mRealChannel[0]->mLoopCount;

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
FMOD_RESULT ChannelI::setLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype)
{
    FMOD_RESULT result = FMOD_OK;
    unsigned int loopstartpcm = 0, loopendpcm = 0, looplengthpcm = 0;
    int count;
    SoundI *soundi;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if ((loopstarttype != FMOD_TIMEUNIT_MS && loopstarttype != FMOD_TIMEUNIT_PCM && loopstarttype != FMOD_TIMEUNIT_PCMBYTES) || 
        (loopendtype   != FMOD_TIMEUNIT_MS && loopendtype   != FMOD_TIMEUNIT_PCM && loopendtype   != FMOD_TIMEUNIT_PCMBYTES))
    {
        return FMOD_ERR_FORMAT;
    }

    soundi = SAFE_CAST(SoundI, mRealChannel[0]->mSound);
    if (!soundi)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    soundi = soundi->mSubSampleParent;

    if (loopstarttype == FMOD_TIMEUNIT_PCM)
    {
        loopstartpcm = loopstart;
    }
    else if (loopstarttype == FMOD_TIMEUNIT_PCMBYTES)
    {
        SoundI::getSamplesFromBytes(loopstart, &loopstartpcm, soundi->mChannels, soundi->mFormat);
    }
    else if (loopstarttype == FMOD_TIMEUNIT_MS)
    {
        loopstartpcm = (unsigned int)((float)loopstart / 1000.0f * soundi->mDefaultFrequency);
    }

    if (loopendtype == FMOD_TIMEUNIT_PCM)
    {
        loopendpcm = loopend;
    }
    else if (loopendtype == FMOD_TIMEUNIT_PCMBYTES)
    {
        SoundI::getSamplesFromBytes(loopend, &loopendpcm, soundi->mChannels, soundi->mFormat);
    }
    else if (loopendtype == FMOD_TIMEUNIT_MS)
    {
        loopendpcm = (unsigned int)((float)loopend / 1000.0f * soundi->mDefaultFrequency);
    }

    if (loopstartpcm >= loopendpcm)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    looplengthpcm = loopendpcm - loopstartpcm + 1;

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2;

        result2 = mRealChannel[count]->setLoopPoints(loopstartpcm, looplengthpcm);
        if (result == FMOD_OK)
        {
            result = result2;
        }
   }

    return result;
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
FMOD_RESULT ChannelI::getLoopPoints(unsigned int *loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int *loopend, FMOD_TIMEUNIT loopendtype)
{
    SoundI *soundi;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if ((loopstarttype != FMOD_TIMEUNIT_MS && loopstarttype != FMOD_TIMEUNIT_PCM && loopstarttype != FMOD_TIMEUNIT_PCMBYTES) || 
        (loopendtype  != FMOD_TIMEUNIT_MS && loopendtype  != FMOD_TIMEUNIT_PCM && loopendtype  != FMOD_TIMEUNIT_PCMBYTES))
    {
        return FMOD_ERR_FORMAT;
    }

    soundi = SAFE_CAST(SoundI, mRealChannel[0]->mSound);
    if (!soundi)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    soundi = soundi->mSubSampleParent;

    if (loopstart)
    {
        if (loopstarttype == FMOD_TIMEUNIT_PCM)
        {
            *loopstart = mRealChannel[0]->mLoopStart;
        }
        else if (loopstarttype == FMOD_TIMEUNIT_PCMBYTES)
        {
            SoundI::getBytesFromSamples(mRealChannel[0]->mLoopStart, loopstart, soundi->mChannels, soundi->mFormat);
        }
        else if (loopstarttype == FMOD_TIMEUNIT_MS)
        {
            *loopstart = (unsigned int)((float)mRealChannel[0]->mLoopStart * 1000.0f / soundi->mDefaultFrequency);
        }
    }

    if (loopend)
    {
        unsigned int lend = mRealChannel[0]->mLoopStart + mRealChannel[0]->mLoopLength - 1;

        if (loopendtype == FMOD_TIMEUNIT_PCM)
        {
            *loopend = lend;
        }
        else if (loopendtype == FMOD_TIMEUNIT_PCMBYTES)
        {
            SoundI::getBytesFromSamples(lend, loopend, soundi->mChannels, soundi->mFormat);
        }
        else if (loopendtype == FMOD_TIMEUNIT_MS)
        {
            *loopend = (unsigned int)((float)lend * 1000.0f / soundi->mDefaultFrequency);
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
FMOD_RESULT ChannelI::setUserData(void *userdata)
{
    mUserData = userdata;
    
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
FMOD_RESULT ChannelI::getUserData(void **userdata)
{
    if (!userdata)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *userdata = mUserData;
    
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
FMOD_RESULT ChannelI::setLowPassFilter(int cutoff)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2;

        result2 = mRealChannel[count]->setLowPassFilter(cutoff);
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }

    return result;
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
FMOD_RESULT ChannelI::getLowPassFilter(int *cutoff)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2;

        result2 = mRealChannel[count]->getLowPassFilter(cutoff);
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }
   
    return result;
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
FMOD_RESULT  ChannelI::setBiquadFilter(bool active, unsigned short b0, unsigned short b1, unsigned short b2, unsigned short a1, unsigned short a2)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2;

        result2 = mRealChannel[count]->setBiquadFilter(active, b0, b1, b2, a1, a2);
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }

    return result;
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
FMOD_RESULT  ChannelI::getBiquadFilter(bool *active, unsigned short *b0, unsigned short *b1, unsigned short *b2, unsigned short *a1, unsigned short *a2)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2;

        result2 = mRealChannel[count]->getBiquadFilter(active, b0, b1, b2, a1, a2);
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }
   
    return result;
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
FMOD_RESULT ChannelI::setControllerSpeaker(unsigned int controller, int subchannel)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mRealChannel[0]->isStream())
    {
        result = mRealChannel[0]->setControllerSpeaker(controller, subchannel);
    }
    else
    {
        for (count = 0; count < mNumRealChannels; count++)
        {
            FMOD_RESULT result2;

            if (subchannel == count || subchannel < 0)
            {
                result2 = mRealChannel[count]->setControllerSpeaker(controller, subchannel);
                if (result == FMOD_OK)
                {
                    result = result2;
                }
            }
        }
    }

    return result;
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
FMOD_RESULT ChannelI::getControllerSpeaker(unsigned int *controller)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2;

        result2 = mRealChannel[count]->getControllerSpeaker(controller);

        if (result == FMOD_OK)
        {
            result = result2;
        }
    }
   
    return result;
}
#endif


/*
[API]
[
	[DESCRIPTION]
    Callback for channel events.

	[PARAMETERS]
	'channel'       Pointer to a channel handle.
    'type'          The type of callback.  Refer to FMOD_CHANNEL_CALLBACKTYPE.
    'commanddata1'  The first callback type specific data generated by the callback.  See remarks for meaning.
    'commanddata2'  The second callback type specific data generated by the callback.  See remarks for meaning.

	[RETURN_VALUE]

	[REMARKS]
    <u>C++ Users</u>.  Cast <b>FMOD_CHANNEL *</b> to <b>FMOD::Channel *</b> inside the callback and use as normal.<br>
    <br>
    <u>'commanddata1' and 'commanddata2' meanings.</u><br>
    These 2 values are set by the callback depending on what is happening in the callback and the type of callback.<br>
    <li><b>FMOD_CHANNEL_CALLBACKTYPE_END</b><br>
        <i>commanddata1</i>: Always 0.<br>
        <i>commanddata2</i>: Always 0.<br>
    <li><b>FMOD_CHANNEL_CALLBACKTYPE_VIRTUALVOICE</b><br>
        <i>commanddata1</i>: (cast to int) <b>0</b> when voice is swapped from emulated to real.  <b>1</b> when voice is swapped from real to emulated.<br>
        <i>commanddata2</i>: Always 0.<br>
    <li><b>FMOD_CHANNEL_CALLBACKTYPE_SYNCPOINT</b><br>
        <i>commanddata1</i>: (cast to int) The index of the sync point.  Use Sound::getSyncPointInfo to retrieve the sync point's attributes.<br>
        <i>commanddata2</i>: Always 0.<br>
    <li><b>FMOD_CHANNEL_CALLBACKTYPE_OCCLUSION</b><br>
        <i>commanddata1</i>: (cast to float *) pointer to a floating point direct value that can be read (dereferenced) and modified after the geometry engine has calculated it for this channel.<br>
        <i>commanddata2</i>: (cast to float *) pointer to a floating point reverb value that can be read (dereferenced) and modified after the geometry engine has calculated it for this channel.<br>
    <br>
    <b>Note!</b>  Currently the user must call System::update for these callbacks to trigger!

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    Channel::setCallback
    FMOD_CHANNEL_CALLBACKTYPE
    System::update
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_CHANNEL_CALLBACK(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *commanddata1, void *commanddata2)
{
#if 0
	 // here purely for documentation purposes.
#endif
}
*/

/*
[API]
[
	[DESCRIPTION]
    Callback for system wide 3d channel volume calculation which overrides fmod's internal calculation code.

	[PARAMETERS]
	'channel'       Pointer to a channel handle.
    'distance'      Distance in units (meters by default).

	[RETURN_VALUE]

	[REMARKS]
    <u>C++ Users</u>.  Cast <b>FMOD_CHANNEL *</b> to <b>FMOD::Channel *</b> inside the callback and use as normal.<p>
    NOTE: When using the event system, call Channel::getUserData to get the event instance handle of the event that spawned the channel in question.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    System::set3DRolloffCallback
    System::set3DListenerAttributes
    System::get3DListenerAttributes
    Channel::getUserData
]
*/
/*
float F_CALLBACK FMOD_3D_ROLLOFFCALLBACK(FMOD_CHANNEL *channel, float distance)
{
#if 0
	 // here purely for documentation purposes.
#endif
}
*/


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelI::getMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, FMOD_MEMORY_USAGE_DETAILS *memoryused_details)
{
#ifdef FMOD_SUPPORT_MEMORYTRACKER
    GETMEMORYINFO_IMPL
#else
    return FMOD_ERR_UNIMPLEMENTED;
#endif
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

#ifdef FMOD_SUPPORT_MEMORYTRACKER

FMOD_RESULT ChannelI::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(false, FMOD_MEMBITS_CHANNEL, sizeof(*this));

    return FMOD_OK;
}

#endif

}
