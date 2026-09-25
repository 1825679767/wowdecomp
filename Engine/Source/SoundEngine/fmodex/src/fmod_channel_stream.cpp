#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_STREAMING

#include "fmod_async.h"
#include "fmod_channel_stream.h"
#include "fmod_codeci.h"
#include "fmod_localcriticalsection.h"
#include "fmod_soundi.h"
#include "fmod_sound_stream.h"
#include "fmod_sound_sample.h"
#include "fmod_speakermap.h"
#include "fmod_systemi.h"


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
// OFFSET: 0x0091EF10
ChannelStream::ChannelStream()
{
    int count;

    for (count = 0; count < FMOD_CHANNEL_MAXREALSUBCHANNELS; count++)
    {
        mRealChannel[count] = 0;
    }

    mSamplesPlayed = mSamplesWritten = 0;    
    mNumRealChannels    = 1;
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
FMOD_RESULT ChannelStream::setRealChannel(ChannelReal *realchan)
{
    mRealChannel[0] = realchan;

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
// OFFSET: 0x0091E5C0
FMOD_RESULT ChannelStream::set2DFreqVolumePanFor3D()
{ 
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->set2DFreqVolumePanFor3D(); 
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
// OFFSET: 0x0091E5F0
FMOD_RESULT ChannelStream::moveChannelGroup(ChannelGroupI *oldchannelgroup, ChannelGroupI *newchannelgroup, bool forcedspreconnect)
{ 
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->moveChannelGroup(oldchannelgroup, newchannelgroup, forcedspreconnect); 
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
// OFFSET: 0x0091E630
FMOD_RESULT ChannelStream::alloc()
{    
    FMOD_RESULT result;
    Stream *stream = SAFE_CAST(Stream, mSound);
    int count;
    
    mFlags = (CHANNELREAL_FLAG)(mFlags & ~CHANNELREAL_FLAG_STOPPED);
    
    mSystem               = stream->mSystem;
    mFinished             = false;
    mLastPCM              = 0;
    mDecodeOffset         = 0;
    stream->mFlags       &= ~FMOD_SOUND_FLAG_THREADFINISHED;

    if (stream->mSubSoundParent)
    {
        Stream *streamParent = SAFE_CAST(Stream, stream->mSubSoundParent);
        mPosition            = streamParent->mInitialPosition;
        streamParent->mFlags &= ~FMOD_SOUND_FLAG_THREADFINISHED;
    }
    else
    {
        mPosition            = stream->mInitialPosition;
    }
    
    mSamplesPlayed        = 0;
    mSamplesWritten       = 0;
    mMinFrequency         = mRealChannel[0]->mMinFrequency;
    if (mMinFrequency < 100)
    {
        mMinFrequency = 100;    /* Streams can't go backwards. */
    }
    mMaxFrequency         = mRealChannel[0]->mMaxFrequency;

    for (count = 0; count < mNumRealChannels; count++)
    {
        Sample *sample = SAFE_CAST(Sample, stream->mSample);

        if (sample)
        {        
            if (!(stream->mMode & FMOD_OPENUSER) && stream->mLength <= sample->mLength && !stream->mSubSoundList && sample->mMode & FMOD_SOFTWARE)
            {
                stream->mFlags |= FMOD_SOUND_FLAG_FULLYBUFFERED;
                stream->mFlags |= FMOD_SOUND_FLAG_FINISHED;         /* Nothing more to read. */

                if (mMode & (FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI))
                {
                    sample->setMode(FMOD_LOOP_NORMAL);
                    sample->setLoopPoints(stream->mLoopStart, FMOD_TIMEUNIT_PCM, stream->mLoopStart + stream->mLoopLength - 1, FMOD_TIMEUNIT_PCM);
                }
                else
                {
                    sample->setMode(FMOD_LOOP_OFF);
                    sample->setLoopPoints(0, FMOD_TIMEUNIT_PCM, sample->mLength - 1, FMOD_TIMEUNIT_PCM);
                }
            }
            else
            {
                stream->mFlags &= ~FMOD_SOUND_FLAG_FULLYBUFFERED;

                sample->setMode(FMOD_LOOP_NORMAL);
                sample->setLoopPoints(0, FMOD_TIMEUNIT_PCM, sample->mLength - 1, FMOD_TIMEUNIT_PCM);
            }

            if (sample->mNumSubSamples)
            {
                sample = (Sample *)sample->mSubSample[count];
            }

            sample->mSubSoundParent = stream;

            mRealChannel[count]->mMode            = sample->mMode;
            mRealChannel[count]->mLoopStart       = sample->mLoopStart;
            mRealChannel[count]->mLoopLength      = sample->mLoopLength;
            mRealChannel[count]->mLength          = sample->mLength;
        }
        mRealChannel[count]->mSound           = sample;
        mRealChannel[count]->mSubChannelIndex = count;
        mRealChannel[count]->mDSP             = 0;
        mRealChannel[count]->mLoopCount       = -1;     /* Reset any previous loop counts that might be left over. */
        mRealChannel[count]->mParent          = mParent;

        result = mRealChannel[count]->alloc();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    FMOD_OS_CriticalSection_Enter(mSystem->mStreamListCrit);
    {
        mStreamNode.setData(this);
        mStreamNode.addBefore(&mSystem->mStreamListChannelHead);
    }
    FMOD_OS_CriticalSection_Leave(mSystem->mStreamListCrit);

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]
    CALLED FROM THREAD

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x0091E7D0
FMOD_RESULT ChannelStream::start()
{ 
    FMOD_RESULT result = FMOD_OK;
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
// OFFSET: 0x0091E830
FMOD_RESULT ChannelStream::update(int delta)
{ 
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->update(delta); 
    }

    return result;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]
    CALLED FROM THREAD

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x0091EF50
FMOD_RESULT ChannelStream::updateStream()
{
    FMOD_RESULT result = FMOD_OK;
    Sample *sample;
    Stream *stream;
    unsigned int pcm = 0;
    int delta;
	LocalCriticalSection crit(mSystem->mStreamRealchanCrit);

    crit.enter();
    {
        if (!mSound)    /* Left critical section in other thread, now it is null! */
        {
            return FMOD_OK;
        }

        stream = SAFE_CAST(Stream, mSound);
        sample = stream->mSample;
       
        if (stream->mOpenState != FMOD_OPENSTATE_READY)
        {
            if (stream->mOpenState == FMOD_OPENSTATE_SETPOSITION)
            {
                stream->mFlags |= FMOD_SOUND_FLAG_SETPOS_SAFE;
            }
            return FMOD_ERR_NOTREADY;
        }

        stream->mFlags &= ~FMOD_SOUND_FLAG_SETPOS_SAFE;

        if (mFlags & CHANNELREAL_FLAG_STOPPED)
        {
            return FMOD_OK;
        }

        if (mRealChannel[0])
        {
            bool isplaying;

            result = mRealChannel[0]->isPlaying(&isplaying);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (!isplaying)
            {
                mFinished = true;
            }
        }

        if (mFinished)
        {
            stream->mFlags |= FMOD_SOUND_FLAG_FINISHED;
            return FMOD_OK; /* Another thread set it */
        }
        
        if (mRealChannel[0])
        {
            result = mRealChannel[0]->updateStream();
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mRealChannel[0]->getPosition(&pcm, FMOD_TIMEUNIT_PCM);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }
    crit.leave();

//    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "ChannelStream::updateStream", "mSamplesPlayed %d mSamplesWritten %d pcm %d / %d stream->mBlockSize %d\n", mSamplesPlayed, mSamplesWritten, pcm, stream->mSample->mLength, stream->mBlockSize));

    while ((mSamplesPlayed > mSamplesWritten && mSamplesPlayed - mSamplesWritten >= (unsigned int)stream->mBlockSize) ||
           (mSamplesPlayed && mSamplesPlayed < mSamplesWritten && mSamplesWritten - mSamplesPlayed >= (unsigned int)stream->mBlockSize))    // <- unsigned int wraparound case.
    {
        unsigned int len;

        crit.enter();
        {
            if (mFlags & CHANNELREAL_FLAG_STOPPED || stream->mFlags & FMOD_SOUND_FLAG_FULLYBUFFERED)
            {
                break;
            }

            /*
                ==================================================================================================
                Stream SOUND logic.  
                Stream CHANNEL related logic is in the next section.
                ==================================================================================================
            */
            len = stream->mBlockSize;

            if (mDecodeOffset > sample->mLength)
            {
                len = 0;
            }
            else if (mDecodeOffset + len > sample->mLength)
            {
                len = sample->mLength - mDecodeOffset;
            }
        }
        crit.leave();

        /*
            Fill the channel stream buffer.
        */
        result = stream->fill(mDecodeOffset, len, 0);

        crit.enter();
        {
			/*
				Handle fatal errors.
			*/
    		if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF && result != FMOD_ERR_FILE_DISKEJECTED)
			{
				int count;

				for (count = 0; count < mNumRealChannels; count++)
				{
					if (mRealChannel[count])
					{
						mRealChannel[count]->setPaused(true);
					}		    
				}

				stream->mOpenState = FMOD_OPENSTATE_ERROR;
				if (stream->mAsyncData)
				{
					stream->mAsyncData->mResult = result;
				}

				mFinished = true;
				return result;
			}
        
			stream->mFlags |= FMOD_SOUND_FLAG_WANTSTOFLUSH;
    
			/*
				Update fill cursor
			*/
			mDecodeOffset += len;
			if (mDecodeOffset >= sample->mLength)
			{
				mDecodeOffset -= sample->mLength;
			}
			mSamplesWritten += len;
        }
		crit.leave();
    }     


    delta = pcm - mLastPCM;
    if (delta < 0)
    {
        delta += sample->mLoopLength;
    }

    if (delta < 0)  /* Sanity check - just in case something goes bad. */
    {
        delta = 0;
    }
        
    /*
		==================================================================================================
		Stream CHANNEL logic.
		Update 'channel' position, which is latency adjusted and 'asynchronous' to the 'sound' position.  
		The 'sound 'position is the fill/read ahead position, and will be ahead of this value.
		==================================================================================================
	*/
    {     
        unsigned int endpoint;
        unsigned int len = delta;

		if (stream->mLength < mLoopStart + mLoopLength)
		{
			mLoopLength = stream->mLength - mLoopStart;
		}

		if (mMode & FMOD_LOOP_NORMAL && mLoopCount) 
		{
			endpoint = mLoopStart + mLoopLength -1;
		}
		else
		{
			endpoint = stream->mLength - 1;
		}

    	mPosition += len;

		if (mPosition > endpoint)
		{
			if ((mMode & FMOD_LOOP_NORMAL && mLoopCount) || stream->mLength == (unsigned int)-1)  /* 0xFFFFFFFF should mean it is an infinite netstream */
			{
				mPosition -= mLoopLength;

				if (mLoopCount > 0)
				{
					mLoopCount--;
				}
			}
			else if (stream->mFlags & FMOD_SOUND_FLAG_FINISHED)
			{
				int count;

				mPosition = stream->mLength;

				for (count = 0; count < mNumRealChannels; count++)
				{
					if (mRealChannel[count])
					{
						mRealChannel[count]->setPaused(true);
					}
				}

				mFinished = true;
			}
		}
	}

    mSamplesPlayed += delta;
    mLastPCM = pcm;
              
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
// OFFSET: 0x0091E870
FMOD_RESULT ChannelStream::setMode(FMOD_MODE mode)
{
    FMOD_RESULT result;
    int count;

    result = ChannelReal::setMode(mode);
    if (result != FMOD_OK)
    {
        return result;
    }
    
    result = mSound->setMode(mode);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setMode(mode & ~(FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI));
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
// OFFSET: 0x0091E8E0
FMOD_RESULT ChannelStream::stop()
{ 
    FMOD_RESULT result = FMOD_OK;
    FMOD_UINT_NATIVE id;

    mFinished = true;

    FMOD_OS_Thread_GetCurrentID(&id);

    if (!(mMode & FMOD_NONBLOCKING && id == mSystem->mMainThreadID))
    {
        if (mSound && mSound->mCodec && mSound->mCodec->mFile)
        {
            mSound->mCodec->mFile->cancel();                            /* Set cancel flag.  Don't do this for nonblocking osunds it might cancel when channel is restarted due to async file flip function. */
        }

        FMOD_OS_CriticalSection_Enter(mSystem->mStreamUpdateCrit);      /* Protect the whole lot. */
    }
   
    FMOD_OS_CriticalSection_Enter(mSystem->mStreamRealchanCrit);        /* protect the realchan. */
    {
        int count;
      
        for (count = 0; count < mNumRealChannels; count++)
        {
            if (mRealChannel[count])
            {
                mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_IN_USE;
                mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_ALLOCATED;
                mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_PLAYING;
                mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_PAUSED;
                mRealChannel[count]->mFlags |=  CHANNELREAL_FLAG_STOPPED;

                result = mRealChannel[count]->stop();
                mRealChannel[count]->mSound = 0;
                mRealChannel[count]->mDSP = 0;
                mRealChannel[count]->mParent = 0;
                mRealChannel[count] = 0;
            }
        }
    }
    FMOD_OS_CriticalSection_Leave(mSystem->mStreamRealchanCrit);        /* protect the realchan. */

    if (!(mMode & FMOD_NONBLOCKING && id == mSystem->mMainThreadID))
    {
        FMOD_OS_CriticalSection_Leave(mSystem->mStreamUpdateCrit);      /* Protect the whole lot. */
    }


    FMOD_OS_CriticalSection_Enter(mSystem->mStreamListCrit);
    {
        if (mSystem->mStreamListChannelNext == &mStreamNode)
        {
            mSystem->mStreamListChannelNext = mStreamNode.getNext();
        }

        mStreamNode.removeNode();
        mStreamNode.setData(0);
    }
    FMOD_OS_CriticalSection_Leave(mSystem->mStreamListCrit);


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
// OFFSET: 0x0091EAA0
FMOD_RESULT ChannelStream::setPaused(bool paused)
{ 
    FMOD_RESULT result = FMOD_OK;
    int count;
	LocalCriticalSection crit(mSystem->mStreamRealchanCrit);

    if (mFlags & CHANNELREAL_FLAG_PAUSEDFORSETPOS)
    {
        return FMOD_OK;
    }

    crit.enter();

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setPaused(paused); 
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
// OFFSET: 0x0091EAE0
FMOD_RESULT ChannelStream::setVolume(float volume)
{ 
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (mNumRealChannels > 1 && mParent->mLevels && mParent->mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERLEVELS)
    {
        for (count = 0; count < mNumRealChannels; count++)
        {
            result = mRealChannel[count]->updateSpeakerLevels(volume);
        }
    }
    else if (mNumRealChannels > 1 && mParent->mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERMIX)
    {
        for (count = 0; count < mNumRealChannels; count++)
        {
            result = mRealChannel[count]->setSpeakerMix(mParent->mSpeakerFL, mParent->mSpeakerFR, mParent->mSpeakerC, mParent->mSpeakerLFE, mParent->mSpeakerBL, mParent->mSpeakerBR, mParent->mSpeakerSL, mParent->mSpeakerSR);
        }
    }
    else
    {
        for (count = 0; count < mNumRealChannels; count++)
        {
            result = mRealChannel[count]->setVolume(volume); 
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
// OFFSET: 0x0091EB60
FMOD_RESULT ChannelStream::setFrequency(float frequency)
{ 
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setFrequency(frequency); 
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
// OFFSET: 0x0091EBA0
FMOD_RESULT ChannelStream::setPan(float pan, float fbpan)
{ 
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        if (mNumRealChannels > 1)
        {
            if (mNumRealChannels == 2 || (mSound && mSound->mDefaultChannelMask == SPEAKER_ALLSTEREO))
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

        result = mRealChannel[count]->setPan(pan, fbpan); 
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
FMOD_RESULT ChannelStream::setDSPClockDelay()
{ 
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setDSPClockDelay(); 
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
// OFFSET: 0x0091EC50
FMOD_RESULT ChannelStream::setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright)
{ 
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setSpeakerMix(frontleft, frontright, center, lfe, backleft, backright, sideleft, sideright); 
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
// OFFSET: 0x0091ECC0
FMOD_RESULT ChannelStream::setSpeakerLevels(int speaker, float *levels, int numlevels)
{ 
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setSpeakerLevels(speaker, levels, numlevels); 
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
FMOD_RESULT ChannelStream::setPositionEx(unsigned int position, FMOD_TIMEUNIT postype, bool fromasync)
{
    FMOD_RESULT  result = FMOD_OK;
    Stream      *stream;

    if (mFlags & CHANNELREAL_FLAG_STOPPED)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    stream = SAFE_CAST(Stream, mSound);
    if (!stream)
    {
		return FMOD_ERR_INVALID_PARAM;
    }

    if (stream->mOpenState == FMOD_OPENSTATE_SETPOSITION && !fromasync)
    {
        return FMOD_ERR_NOTREADY;
    }

    /*
        This isn't redundant, we need this conversion so we can do an mPosition == newpos check
        and mPosition only wants to know about PCM, not bytes or ms.
    */
    if (postype == FMOD_TIMEUNIT_MS || postype == FMOD_TIMEUNIT_PCM || postype == FMOD_TIMEUNIT_PCMBYTES)
    {
        switch (postype)
        {
            case FMOD_TIMEUNIT_MS: 
            {
                position = (unsigned int)((float)position / 1000.0f * stream->mDefaultFrequency);
                postype = FMOD_TIMEUNIT_PCM;
                break;
            }
            case FMOD_TIMEUNIT_PCM:
            {
                break;
            }
            case FMOD_TIMEUNIT_PCMBYTES:
            {
                stream->getSamplesFromBytes(position, &position);
                postype = FMOD_TIMEUNIT_PCM;
                break;
            }
            default:
            {
                break;
            }
        }
    }

#ifdef FMOD_SUPPORT_SENTENCING
    if (postype == FMOD_TIMEUNIT_SENTENCE)
    {
        stream->mSubSoundIndex = stream->mSubSoundList[position].mIndex;
        postype = FMOD_TIMEUNIT_MS;
        position = 0;
    }
#endif

    if (stream->mFlags & FMOD_SOUND_FLAG_FULLYBUFFERED)
    {
        int count;

        for (count = 0; count < mNumRealChannels; count++)
        {
            mRealChannel[count]->setPosition(position, FMOD_TIMEUNIT_PCM);
        }

        mLastPCM              = position;
        mDecodeOffset         = 0;
        mPosition             = position;
        mSamplesWritten       = 0;
        mSamplesPlayed        = 0;
    }
    else if (postype != FMOD_TIMEUNIT_PCM ||                                                  /* Could be mod/s3m/xm/it order/row type of seek */
        stream->mSubSoundIndex != stream->mCodec->mSubSoundIndex ||                      /* Switching subsounds.  Most definitely need to seek. */
       (stream->mFlags & FMOD_SOUND_FLAG_WANTSTOFLUSH) ||                                /* The channel stream thread has filled some data and moved the mPosition value. */
       (mFlags & CHANNELREAL_FLAG_PLAYING && !(mParent->mFlags & CHANNELI_FLAG_JUSTWENTVIRTUAL))) /* If it is playing then the position cursor must have moved (even if mPosition hasn't, because we might not have been serviced by the ChannelStream thread yet, so we better do the seek to get the seekcallback and reset the voice and reset the mPosition variable.  If it just went virtual, then it must be 0. */
    {
        int count;

#ifdef FMOD_SUPPORT_NONBLOCKSETPOS
        if (stream->mMode & FMOD_NONBLOCKING && !fromasync)
        {
            result = AsyncThread::getAsyncThread(stream);
            if (result != FMOD_OK)
            {
                return result;
            }

            /*
                Pause the real channel so it doesn't stutter in the stream thread.
            */
            mFlags |= CHANNELREAL_FLAG_PAUSEDFORSETPOS;

            for (count = 0; count < mNumRealChannels; count++)
            {
                mRealChannel[count]->setPaused(true);
            }
            stream->mSample->clear(0, stream->mSample->mLength);


            FMOD_OS_CriticalSection_Enter(stream->mAsyncData->mThread->mCrit);
            {
                stream->mOpenState = FMOD_OPENSTATE_SETPOSITION;
                if (stream->mSubSoundParent)
                {
                    stream->mSubSoundParent->mOpenState = FMOD_OPENSTATE_SETPOSITION;
                }
                
                stream->mAsyncData->mPosition = position;
                stream->mAsyncData->mPositionType = postype;
                stream->mAsyncData->mNode.setData(stream);
                stream->mAsyncData->mNode.addBefore(&stream->mAsyncData->mThread->mHead);
            }
            FMOD_OS_CriticalSection_Leave(stream->mAsyncData->mThread->mCrit);

            stream->mAsyncData->mThread->mThread.wakeupThread();
        }
        else
#endif
        {
            bool paused = false;

            if (!fromasync)
            {
                FMOD_OS_CriticalSection_Enter(mSystem->mStreamUpdateCrit);
   
                result = mRealChannel[0]->getPaused(&paused);
                if (result != FMOD_OK)
                {
                    FMOD_OS_CriticalSection_Leave(mSystem->mStreamUpdateCrit);
                    return result;
                }

                /*
                    Pause the real channel and reset it if it is playing
                */
                for (count = 0; count < mNumRealChannels; count++)
                {
                    mRealChannel[count]->setPaused(true);
                }
            }
   
            /*
                Call the sound stream to setposition.
            */    
            result = stream->setPosition(position, postype);
            if (result == FMOD_OK)
            {
                /*
                    Now reset and fill the pcm buffer.
                */

                FMOD_OS_CriticalSection_Enter(stream->mSystem->mStreamRealchanCrit);
                for (count = 0; count < mNumRealChannels; count++)
                {
                    if (mRealChannel[count])
                    {
                        mRealChannel[count]->setPosition(0, FMOD_TIMEUNIT_PCM);
                    }
                }
                FMOD_OS_CriticalSection_Leave(stream->mSystem->mStreamRealchanCrit);

                mLastPCM              = 0;
                mDecodeOffset         = 0;
                mPosition             = position;
                mSamplesWritten       = 0;
                mSamplesPlayed        = 0;

                result = stream->flush();
            }
   
            if (!fromasync)
            {    
                /*
                   Set the pause state of the channel to what it used to be before
                */
                for (count = 0; count < mNumRealChannels; count++)
                {
                   mRealChannel[count]->setPaused(paused);  
                }
                FMOD_OS_CriticalSection_Leave(mSystem->mStreamUpdateCrit);
           }
        }
    }
    else 
    {
        /*
            If this is a sentence, and the user started screwing around with getSubSound, make sure it is reset.
        */
#ifdef FMOD_SUPPORT_SENTENCING
        if (stream->mSubSoundList && stream->mSubSoundShared && stream->mSubSoundIndex != stream->mSubSoundShared->mSubSoundIndex)
        {
            stream->mSubSoundShared->updateSubSound(stream->mSubSoundIndex, true);
        }
#endif

#ifdef PLATFORM_PS3   
        /* 
            PS3 only? ..  The raw codec and resampler never gets reset, so reset it here. 
        */
        if (mFlags & CHANNELREAL_FLAG_PAUSED)
        {
            int count;
            /*
                Now reset and fill the pcm buffer.
            */
            for (count = 0; count < mNumRealChannels; count++)
            {
                mRealChannel[count]->setPosition(0, FMOD_TIMEUNIT_PCM);
            }
        }
#endif
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
// OFFSET: 0x0091F260
FMOD_RESULT ChannelStream::getPosition(unsigned int *position, FMOD_TIMEUNIT postype)
{
    Stream *stream;
    bool getsubsoundtime = false;
    
    if (!position)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    stream = SAFE_CAST(Stream, mSound);
    if (!stream)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    /*
        First check for subsound stuff.
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

    if (postype == FMOD_TIMEUNIT_MS || 
        postype == FMOD_TIMEUNIT_PCM || 
        postype == FMOD_TIMEUNIT_PCMBYTES || 
        postype == FMOD_TIMEUNIT_SENTENCE || 
        postype == FMOD_TIMEUNIT_SENTENCE_SUBSOUND)
    {
        unsigned int pos;
        int currentsubsoundid = 0;
        int currentsentenceid = 0;

        pos = mPosition;

#ifdef FMOD_SUPPORT_SENTENCING
        if (getsubsoundtime)
        {
            for (currentsubsoundid = 0; currentsubsoundid < mSound->mSubSoundListNum; currentsubsoundid++)
            {
                unsigned int lengthpcm = 0;
                SoundI *sound = mSound->mSubSound[mSound->mSubSoundList[currentsubsoundid].mIndex];

                if (sound)
                {
                    if (mSound->mSubSoundShared)
                    {
                        FMOD_RESULT result;
                        FMOD_CODEC_WAVEFORMAT waveformat;

                        result = stream->mCodec->mDescription.getwaveformat(stream->mCodec, stream->mSubSoundList[currentsubsoundid].mIndex, &waveformat);
                        if (result != FMOD_OK)
                        {
                            return result;
                        }        

                        lengthpcm = waveformat.lengthpcm;
                    }
                    else
                    {
                        lengthpcm = sound->mLength;
                    }
                }
       
                if (pos >= lengthpcm)
                {
                    pos -= lengthpcm;
                }
                else
                {
                    break;
                }

                currentsentenceid++;
            }
        }
#endif

        if (postype == FMOD_TIMEUNIT_SENTENCE)
        {
            *position = currentsentenceid;
        }
#ifdef FMOD_SUPPORT_SENTENCING
        else if (postype == FMOD_TIMEUNIT_SENTENCE_SUBSOUND)
        {
            *position = mSound->mSubSoundList[currentsentenceid].mIndex;
        }
#endif
        else if (postype == FMOD_TIMEUNIT_PCM)
        {
            *position = pos;
        }
        else if (postype == FMOD_TIMEUNIT_PCMBYTES)
        {
            SoundI::getBytesFromSamples(pos, position, mSound->mChannels, mSound->mFormat);
        }
        else if (postype == FMOD_TIMEUNIT_MS)
        {
            *position = (unsigned int)((float)pos / mSound->mDefaultFrequency * 1000.0f);
        }
    }
    else
    {
        return stream->getPosition(position, postype);
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
// OFFSET: 0x0091ED00
FMOD_RESULT ChannelStream::setLoopPoints(unsigned int loopstart, unsigned int looplength)
{
    FMOD_RESULT result;

    result = ChannelReal::setLoopPoints(loopstart, looplength);
    if (result != FMOD_OK)
    {
        return FMOD_OK;
    }

    result = mSound->setLoopPoints(loopstart, FMOD_TIMEUNIT_PCM, loopstart + looplength - 1, FMOD_TIMEUNIT_PCM);    
    if (result != FMOD_OK)
    {
        return FMOD_OK;
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
// OFFSET: 0x0091ED40
FMOD_RESULT ChannelStream::setLoopCount(int loopcount)
{
    FMOD_RESULT result;

    result = ChannelReal::setLoopCount(loopcount);
    if (result != FMOD_OK)
    {
        return FMOD_OK;
    }

    result = mSound->setLoopCount(loopcount);
    if (result != FMOD_OK)
    {
        return FMOD_OK;
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
// OFFSET: 0x0091ED70
FMOD_RESULT ChannelStream::set3DAttributes()
{ 
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->set3DAttributes(); //pos, vel); 
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
FMOD_RESULT ChannelStream::setLowPassGain(float gain)
{ 
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setLowPassGain(gain);
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
// OFFSET: 0x0091EDA0
FMOD_RESULT ChannelStream::set3DMinMaxDistance()
{ 
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->set3DMinMaxDistance();
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
// OFFSET: 0x0091EDD0
FMOD_RESULT ChannelStream::set3DOcclusion(float directocclusion, float reverbocclusion)
{ 
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->set3DOcclusion(directocclusion, reverbocclusion); 
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
// OFFSET: 0x0091EE10
FMOD_RESULT ChannelStream::setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES *prop)
{ 
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setReverbProperties(prop); 
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
// OFFSET: 0x0091EE50
FMOD_RESULT ChannelStream::getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES *prop)
{ 
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels && count < 1; count++)
    {
        result = mRealChannel[count]->getReverbProperties(prop); 
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
// OFFSET: 0x0091EE90
FMOD_RESULT ChannelStream::isPlaying(bool *isplaying, bool includethreadlatency)
{ 
    *isplaying = !mFinished;     

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
// OFFSET: 0x0091EEB0
FMOD_RESULT ChannelStream::getSpectrum(float *spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{ 
    return mRealChannel[0]->getSpectrum(spectrumarray, numvalues, channeloffset, windowtype); 
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
// OFFSET: 0x0091EED0
FMOD_RESULT ChannelStream::getWaveData(float *wavearray, int numvalues, int channeloffset) 
{ 
    return mRealChannel[0]->getWaveData(wavearray, numvalues, channeloffset); 
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
// OFFSET: 0x0091EEF0
FMOD_RESULT ChannelStream::getDSPHead(DSPI **dsp)
{ 
    return mRealChannel[0]->getDSPHead(dsp); 
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
FMOD_RESULT ChannelStream::setLowPassFilter(int cutoff)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

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
FMOD_RESULT  ChannelStream::getLowPassFilter(int *cutoff)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

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
FMOD_RESULT  ChannelStream::setBiquadFilter(bool active, unsigned short b0, unsigned short b1, unsigned short b2, unsigned short a1, unsigned short a2)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

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
FMOD_RESULT  ChannelStream::getBiquadFilter(bool *active, unsigned short *b0, unsigned short *b1, unsigned short *b2, unsigned short *a1, unsigned short *a2)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

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
FMOD_RESULT ChannelStream::setControllerSpeaker (unsigned int controllerspeaker, int subchannel)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2;

        if (subchannel == count || subchannel < 0)
        {
            result2 = mRealChannel[count]->setControllerSpeaker(controllerspeaker);
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
FMOD_RESULT  ChannelStream::getControllerSpeaker(unsigned int *controllerspeaker)
{
    *controllerspeaker = mControllerSpeaker;

    return FMOD_OK;
}
#endif  /* #ifdef PLATFORM_WII */
}

#endif /* #ifdef FMOD_SUPPORT_STREAMING */

