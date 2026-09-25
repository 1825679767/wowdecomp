#include "fmod_settings.h"

#include "fmod_async.h"
#include "fmod_codeci.h"
#include "fmod_localcriticalsection.h"
#include "fmod_soundi.h"
#include "fmod_sound_sample.h"
#include "fmod_sound_stream.h"
#include "fmod_systemi.h"
#include "fmod_stringw.h"

namespace FMOD
{


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT SoundI::validate(Sound *sound, SoundI **soundi)
{
    if (!soundi)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!sound)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    *soundi = (SoundI *)sound;

    #if 0
    {
        SystemI *system = SAFE_CAST(SystemI, gGlobal->gSystemHead->getNext());
        while (system != gGlobal->gSystemHead)
        {
            LinkedListNode *currentsound = SAFE_CAST(SoundI, system->mSoundListHead.getNext());

            while (currentsound != &system->mSoundListHead)
            {
                if (currentsound == *soundi || ((*soundi)->mSubSoundParent && currentsound == (*soundi)->mSubSoundParent))
                {
                    return FMOD_OK;
                }

                currentsound = currentsound->getNext();
            }

            system = SAFE_CAST(SystemI, system->getNext());
        }

        return FMOD_ERR_INVALID_HANDLE;
    }
    #endif

    return FMOD_OK;
}

#ifndef __FMOD_EVENT

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
// OFFSET: 0x008E1ED0
SoundI::SoundI()
{
    mMode                    = FMOD_DEFAULT;
    mFormat                  = FMOD_SOUND_FORMAT_NONE;
    mName                    = 0;
    mChannels                = 1;
    mMinDistance             = 1.0f;
    mMaxDistance             = 10000.0f;
    mConeInsideAngle         = 360.0f;
    mConeOutsideAngle        = 360.0f;
    mConeOutsideVolume       = 1.0f;
                             
    mDefaultVolume           = 1.0f;
    mDefaultFrequency        = DEFAULT_FREQUENCY;
    mDefaultPan              = 0.0f;
    mDefaultPriority         = FMOD_CHANNEL_DEFAULTPRIORITY;
    mFrequencyVariation      = 0;
    mVolumeVariation         = 0;
    mPanVariation            = 0;
    mLoopStart               = 0;
    mLoopLength              = 0;
    mLoopCount               = -1;
    mUserData                = 0;
    mMemoryUsed              = 0;
    mCodec                   = 0;
    mSubSound                = 0;
    mSubSoundParent          = 0;
#ifdef FMOD_SUPPORT_SENTENCING
    mSubSoundList            = 0;
#endif
    mNumSubSounds            = 0;
    mNumActiveSubSounds      = 0;
    mAsyncData               = 0;
    mNumSyncPoints           = 0;
    mOpenState               = FMOD_OPENSTATE_READY;
    mSubSampleParent         = this;     /* unless made a sample with subsamples where it will point to a higher sound */
    mRolloffPoint            = 0;
    mNumRolloffPoints        = 0;
    mSyncPointHead           = 0;
    mSyncPointTail           = 0;
    mPostReadCallback        = 0;
    mPostSetPositionCallback = 0;
    mPostCallbackSound       = 0;
    mFlags                   = 0;

    mSoundGroupNode.initNode();
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
// OFFSET: 0x008E4590
FMOD_RESULT SoundI::loadSubSound(int index, FMOD_MODE mode)
{
    FMOD_RESULT result;
    Sample *sample = 0;
    FMOD_CODEC_WAVEFORMAT waveformat;

    if (!mNumSubSounds)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (index < 0 || index >= mNumSubSounds)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = mCodec->mDescription.getwaveformat(mCodec, index, &waveformat);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mSystem->createSample(mode, &waveformat, &sample);
    if (result != FMOD_OK)
    {
        return result;
    }

    sample->mType = mType;
    sample->mCodec = mCodec;

    if (mCodec->mDescription.soundcreate)
    {
        SoundI *s = SAFE_CAST(SoundI, sample);

        result = mCodec->mDescription.soundcreate(mCodec, index, (FMOD_SOUND *)s);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    mCodec->reset();

    result = mCodec->setPosition(index, 0, FMOD_TIMEUNIT_PCM);
    if (result != FMOD_OK)
    {
        return result;
    }
    if (mPostSetPositionCallback)
    {
        mPostSetPositionCallback((FMOD_SOUND *)this, index, 0, FMOD_TIMEUNIT_PCM);
    }

    if (!(mode & FMOD_OPENONLY))
    {
        result = sample->read(0, sample->mLength, 0);
        if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
        {
            return result;
        }
    }

//    mSubSound[index] = sample;
    result = setSubSound(index, sample);
    if (result != FMOD_OK)
    {
        return result;
    }
    
    result = sample->setPositionInternal(0);
    if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
    {
        return result;
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
// OFFSET: 0x008E3100
FMOD_RESULT SoundI::read(unsigned int offset, unsigned int numsamples, unsigned int *read)
{
    FMOD_RESULT result;
    unsigned int offbytes, numbytes, readbytes, chunksize;
    FMOD_CODEC_WAVEFORMAT waveformat;

    mPosition = offset;

    if (mMode & FMOD_CREATECOMPRESSEDSAMPLE)
    {
        offbytes = offset;
        numbytes = numsamples;
    }
    else
    {
        getBytesFromSamples(offset, &offbytes, false);
        getBytesFromSamples(numsamples, &numbytes, false);
    }
        
    readbytes = 0;

    if (read)
    {
        *read = 0;
    }

    result = mCodec->mDescription.getwaveformat(mCodec, mCodec->mSubSoundIndex, &waveformat);
    if (result != FMOD_OK)
    {
        return result;
    }

    chunksize = SOUND_READCHUNKSIZE;
    
    if (mMode & FMOD_CREATESTREAM)
    {
        if (mCodec->mBlockAlign)
        {
            chunksize /= mCodec->mBlockAlign;
            chunksize *= mCodec->mBlockAlign;

            if (!chunksize)
            {
                chunksize = mCodec->mBlockAlign;
            }
        }
    }
    else
    {
        if (waveformat.blockalign)
        {
            chunksize /= waveformat.blockalign;
            chunksize *= waveformat.blockalign;

            if (!chunksize)
            {
                chunksize = waveformat.blockalign;
            }
        }
    }

    if (chunksize > SOUND_READCHUNKSIZE)
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "SoundI::read", "ERROR - Contact support.  A codec has specified a block alignment of %d which is bigger than the maximum codec read size of %d\n", waveformat.blockalign, SOUND_READCHUNKSIZE));
        return FMOD_ERR_INTERNAL;
    }

    if (mCodec->mNonInterleaved && mNumSubSamples)
    {
        numsamples /= mNumSubSamples;
        offset     /= mNumSubSamples;

        for(int count = 0; count < mNumSubSamples; count++)
        {
            getBytesFromSamples(offset, &offbytes);
            numbytes = mSubSample[count]->mLengthBytes;

            while (numbytes)
            {
                FMOD_RESULT     result2;
                int             size = numbytes > chunksize ? chunksize : numbytes;
                unsigned int    bytesreadtotal = 0;
                void           *ptr1, *ptr2;
                unsigned int    len1, len2;

                result = mSubSample[count]->lock(offbytes, size, &ptr1, &ptr2, &len1, &len2);
                if (result != FMOD_OK)
                {
                    return result;
                }

                if (!len1 && !len2)
                {
                    mSubSample[count]->unlock(ptr1, ptr2, len1, len2); /* Unlock the resource before returning. */
                    FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "SoundI::read", "Lock error.  len1 was 0 and len2 was 0.\n"));
                    return FMOD_ERR_FILE_BAD;
                }

                if (ptr1 && len1)
                {
                    unsigned int bytesread = 0;

                    result = readData(ptr1, len1, &bytesread);
                    if ((result != FMOD_OK) && (result != FMOD_ERR_FILE_EOF))
                    {
                        mSubSample[count]->unlock(ptr1, ptr2, len1, len2); /* Unlock the resource before returning. */
                        return result;
                    }

                    bytesreadtotal += bytesread;
                }

                if (ptr2 && len2)
                {
                    unsigned int bytesread = 0;

                    result = readData(ptr2, len2, &bytesread);
                    if ((result != FMOD_OK) && (result != FMOD_ERR_FILE_EOF))
                    {
                        mSubSample[count]->unlock(ptr1, ptr2, len1, len2); /* Unlock the resource before returning. */
                        return result;
                    }

                    bytesreadtotal += bytesread;
                }

                /*
                    File was truncated somehow, but allow it to load.
                */
                if (result == FMOD_ERR_FILE_EOF)
                {
                    numbytes = bytesreadtotal;
                }

                result2 = mSubSample[count]->unlock(ptr1, ptr2, len1, len2);
                if (result2 != FMOD_OK)
                {
                    return result2;
                }

                numbytes  -= bytesreadtotal;
                offbytes  += bytesreadtotal;
                readbytes += bytesreadtotal;
            }
        }
    }
    else
    {
        while (numbytes)
        {
            FMOD_RESULT     result2;
            int             size = numbytes > chunksize ? chunksize : numbytes;
            unsigned int    bytesreadtotal = 0;
            void           *ptr1, *ptr2;
            unsigned int    len1, len2;

            result = lock(offbytes, size, &ptr1, &ptr2, &len1, &len2);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (!len1 && !len2)
            {
                unlock(ptr1, ptr2, len1, len2); /* Unlock the resource before returning. */
                FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SoundI::read", "Lock error.  len1 was 0 and len2 was 0.\n"));
                return FMOD_ERR_FILE_BAD;
            }

            if (ptr1 && len1)
            {
                unsigned int bytesread = 0;

                result = readData(ptr1, len1, &bytesread);
                if ((result != FMOD_OK) && (result != FMOD_ERR_FILE_EOF))
                {
                    unlock(ptr1, ptr2, len1, len2); /* Unlock the resource before returning. */
                    return result;
                }

                bytesreadtotal += bytesread;
            }

            if (ptr2 && len2)
            {
                unsigned int bytesread = 0;

                result = readData(ptr2, len2, &bytesread);
                if ((result != FMOD_OK) && (result != FMOD_ERR_FILE_EOF))
                {
                    unlock(ptr1, ptr2, len1, len2); /* Unlock the resource before returning. */
                    return result;
                }

                bytesreadtotal += bytesread;
            }

            /*
                File was truncated somehow, but allow it to load.
            */
            if (result == FMOD_ERR_FILE_EOF)
            {
                numbytes = bytesreadtotal;
            }

            result2 = unlock(ptr1, ptr2, len1, len2);
            if (result2 != FMOD_OK)
            {
                return result2;
            }

            numbytes  -= bytesreadtotal;
            offbytes  += bytesreadtotal;
            readbytes += bytesreadtotal;
        }
    }

    if (read)
    {
        getSamplesFromBytes(readbytes, read);
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
// OFFSET: 0x008E10A0
FMOD_RESULT SoundI::seek(int subsound, unsigned int position)
{
    FMOD_RESULT result;
    unsigned int newpos;
    
    if (!mCodec->mFile)
    {
        return FMOD_ERR_FILE_COULDNOTSEEK;
    }

    result = mCodec->setPosition(subsound, position, FMOD_TIMEUNIT_PCM);

    newpos = position;
    if (mCodec->getPosition(&newpos, FMOD_TIMEUNIT_PCM) == FMOD_OK)
    {
        position = newpos;
    }

    mPosition = newpos;

    if (mPostSetPositionCallback)
    {
        mPostSetPositionCallback((FMOD_SOUND *)this, subsound, newpos, FMOD_TIMEUNIT_PCM);
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
// OFFSET: 0x008E3700
FMOD_RESULT SoundI::clear(unsigned int offset, unsigned int numsamples)
{
    FMOD_RESULT result;
    unsigned int offbytes, numbytes;
    void *ptr1, *ptr2;
    unsigned int len1, len2, chunksize;
    FMOD_CODEC_WAVEFORMAT waveformat;

    mPosition = offset;

    if (mMode & FMOD_CREATECOMPRESSEDSAMPLE)
    {
        offbytes = offset;
        numbytes = numsamples;
    }
    else
    {
        getBytesFromSamples(offset, &offbytes, false);
        getBytesFromSamples(numsamples, &numbytes, false);
    }
        
    result = mCodec->mDescription.getwaveformat(mCodec, mCodec->mSubSoundIndex, &waveformat);
    if (result != FMOD_OK)
    {
        return result;
    }

    chunksize = SOUND_READCHUNKSIZE;
    
    if (mMode & FMOD_CREATESTREAM)
    {
        if (mCodec->mBlockAlign)
        {
            chunksize /= mCodec->mBlockAlign;
            chunksize *= mCodec->mBlockAlign;

            if (!chunksize)
            {
                chunksize = mCodec->mBlockAlign;
            }
        }
    }
    else
    {
        if (waveformat.blockalign)
        {
            chunksize /= waveformat.blockalign;
            chunksize *= waveformat.blockalign;

            if (!chunksize)
            {
                chunksize = waveformat.blockalign;
            }
        }
    }

    if (chunksize > SOUND_READCHUNKSIZE)
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "SoundI::read", "ERROR - Contact support.  A codec has specified a block alignment of %d which is bigger than the maximum codec read size of %d\n", waveformat.blockalign, SOUND_READCHUNKSIZE));
        return FMOD_ERR_INTERNAL;
    }

    while (numbytes)
    {
        int size = numbytes > chunksize ? chunksize : numbytes, bytesreadtotal = 0;

        result = lock(offbytes, size, &ptr1, &ptr2, &len1, &len2);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (ptr1 && len1)
        {
            FMOD_memset(ptr1, 0, len1);
            bytesreadtotal += len1;
        }

        if (ptr2 && len2)
        {
            FMOD_memset(ptr2, 0, len2);
            bytesreadtotal += len2;
        }

        result = unlock(ptr1, ptr2, len1, len2);
        if (result != FMOD_OK)
        {
            return result;
        }

        numbytes  -= bytesreadtotal;
        offbytes  += bytesreadtotal;
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
// OFFSET: 0x008E1FC0
FMOD_RESULT SoundI::release(bool freethis)
{
    FMOD_RESULT           result;
    Codec                *oldcodec;
    AsyncData            *oldasyncdata;
    ChannelReal          *oldstreamchannel;

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SoundI::release", "%s (%p)\n", mName ? mName : "(null)", this));

    if (mFlags & FMOD_SOUND_FLAG_RELEASING)
    {
        return FMOD_ERR_INVALID_HANDLE;     /* If it gets recursive because stopsound below calls a channel callback which tries to release it again, return. */
    }
    
    if (mFlags & FMOD_SOUND_FLAG_PRELOADEDFSB)
    {
        return FMOD_ERR_PRELOADED;
    }

    if (mFlags & FMOD_SOUND_FLAG_PROGRAMMERSOUND)
    {
        return FMOD_ERR_PROGRAMMERSOUND;
    }

    mFlags |= FMOD_SOUND_FLAG_RELEASING;

    /*
        The reason there is a sleep loop here and not a criticalsection is that we don't want one sound release stalling on other non blocking sounds, 
        and we also don't want the non blocking thread stalling on release calls (not such a huge issue but it slows loading down unescessarily).
    */
    while ((mOpenState != FMOD_OPENSTATE_READY && mOpenState != FMOD_OPENSTATE_ERROR) || (mFlags & FMOD_SOUND_FLAG_DONOTRELEASE))
    {
        FMOD_OS_Time_Sleep(2);
    }

    if (mCodec)
    {
        /*
            Cancel any pending file requests.
            This is needed for Wii / GC.
        */
        if (mCodec->mFile)
        {
            mCodec->mFile->cancel();
        }
    }

    /*
        Stop any channels that are playing this sound.
    */
    if (mSystem)
    {       
        result = mSystem->stopSound(this);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        Delete all syncpoints if they exist.
    */
    if (mSyncPointHead)
    {
        SyncPoint *point = SAFE_CAST(SyncPoint, mSyncPointHead->getNext());

        while (point != mSyncPointTail)
        {
            deleteSyncPointInternal((FMOD_SYNCPOINT *)point, true);

            point = SAFE_CAST(SyncPoint, mSyncPointHead->getNext());
        }

        FMOD_Memory_Free(mSyncPointHead);   // mSyncPointTail is part of this memory block as well
        mSyncPointHead = 0;
        mSyncPointTail = 0;
    }

    if (mSyncPointMemory)
    {
        FMOD_Memory_Free(mSyncPointMemory);
        mSyncPointMemory = 0;
    }

    oldcodec = mCodec;
    oldasyncdata = mAsyncData;
    oldstreamchannel = 0;

    /*
        Release the stream samples if this is a stream
    */
    #ifdef FMOD_SUPPORT_STREAMING
    if (isStream() && mCodec && (!mSubSoundParent || mSubSoundParent == this || (mSubSoundParent && mCodec != mSubSoundParent->mCodec)))
    {
        Stream *stream = SAFE_CAST(Stream, this);

        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SoundI::release", "remove stream samples. (%p)\n", this));

        /*
            Wait for the stream to be totally done with before removing.
        */
        if (stream->mChannel)
        {
            stream->mChannel->mFinished = true; /* Make the thread set the finished flag. */
            
            if (!(mFlags & FMOD_SOUND_FLAG_PLAYED))
            {
                stream->mFlags |= FMOD_SOUND_FLAG_THREADFINISHED;
            }
        }

        if (!(mSystem->mFlags & FMOD_INIT_STREAM_FROM_UPDATE) && !stream->mStreamNode.isEmpty())
        {
            while (!(stream->mFlags & FMOD_SOUND_FLAG_THREADFINISHED))
            {
                FMOD_OS_Time_Sleep(2);
            }
        }

        /*
            Safely remove from list. (fast)
        */
        FMOD_OS_CriticalSection_Enter(mSystem->mStreamListCrit);
        {
            stream->mStreamNode.removeNode();
            stream->mStreamNode.setData(0);
        }
        FMOD_OS_CriticalSection_Leave(mSystem->mStreamListCrit);                

        oldstreamchannel = stream->mChannel;

        if (stream->mSample)
        {
            stream->mSample->mCodec = 0;
            FLOG_INDENT(4);
            {
                stream->mSample->release();
            }
            FLOG_INDENT(-4);
            stream->mSample = 0;
        }
    }
    #endif

    /*
        Remove all subsounds of this sound if they exist!
    */
    if (mNumSubSounds && mSubSound)
    {
        int count;

        if (mNumActiveSubSounds)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SoundI::release", "release subsounds. (%p)\n", this));
            FLOG_INDENT(4);
            for (count = 0; count < mNumSubSounds; count++)
            {
                if (mSubSound[count])
                {
                    /*
                        Points to the same codec as the parent sound so don't release it more than once
                    */
                    if (mSubSound[count]->mCodec == oldcodec)
                    {
                        mSubSound[count]->mCodec = 0;
                    }
                    if (mSubSound[count]->mAsyncData == oldasyncdata)
                    {
                        mSubSound[count]->mAsyncData = 0;
                    }

                    #ifdef FMOD_SUPPORT_STREAMING
                    if (mSubSound[count]->isStream())
                    {
                        Stream *stream = SAFE_CAST(Stream, mSubSound[count]);

                        if (stream->mChannel == oldstreamchannel)
                        {
                            stream->mChannel = 0;
                        }
                    }
                    #endif

                    if (mSubSound[count]->mSubSoundShared)  /* Null out all the other shared pointers that point to this address. */
                    {
                        int count2;

                        for (count2 = count + 1; count2 < mNumSubSounds; count2++)
                        {
                            if (mSubSound[count2] == mSubSound[count])
                            {
                                mSubSound[count2] = 0;
                            }
                        }
                    }

                    mSubSound[count]->release();
                    mSubSound[count] = 0;
                }
            }
            FLOG_INDENT(-4);
        }

        FMOD_Memory_Free(mSubSound);
        mSubSound = 0;       

        if (mSubSoundShared)
        {
            FMOD_Memory_Free(mSubSoundShared);
            mSubSoundShared = 0;       
        }
    }

    /*
        Release the codec if it exists
    */
    if (mCodec)
    {
        if (!mSubSoundParent || mSubSoundParent == this || (mSubSoundParent && mCodec != mSubSoundParent->mCodec)) /* Don't release the parent's codec. */
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SoundI::release", "release codec. (%p)\n", this));

            mCodec->release();
            mCodec = 0;
        }
    }

    #ifdef FMOD_SUPPORT_STREAMING
    if (isStream())
    {
        Stream *stream = SAFE_CAST(Stream, this);
        Stream *parentstream = SAFE_CAST(Stream, stream->mSubSoundParent);

        if (parentstream && parentstream->mChannel == stream->mChannel)
        {
            stream->mChannel = 0;   /* Don't free the parent's channel object. */
        }

        if (stream->mChannel)   /* This must be freed after sample release, as stream->mSample tries to stopSound as well (and reference the channel). */
        {
            FMOD_Memory_Free(stream->mChannel);
            stream->mChannel = 0;
        }
    }
    #endif

    if (mAsyncData)
    {
        if (!mSubSoundParent || mSubSoundParent->mAsyncData != mAsyncData)
        {
            FMOD_Memory_Free(mAsyncData);
        }
        mAsyncData = 0;
    }

    /*
        Null out any parent references to this sound if this sound is a subsound of another sound.
    */
    if (mSubSoundParent)
    {
        int count;

        for (count = 0; count < mSubSoundParent->mNumSubSounds; count++)
        {
            if (mSubSoundParent->mSubSound && mSubSoundParent->mSubSound[count] == this)
            {
                mSubSoundParent->setSubSoundInternal(count, 0, true);
                break;
            }
        }
    }

#ifdef FMOD_SUPPORT_SENTENCING
    if (mSubSoundList)
    {
        FMOD_Memory_Free(mSubSoundList);
        mSubSoundList = 0;
    }
#endif

    if (mName)
    {
        FMOD_Memory_Free(mName);
        mName = 0;
    }

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SoundI::release", "free this. (%p)\n", this));

    FMOD_OS_CriticalSection_Enter(mSystem->gSoundListCrit);
    {
        removeNode();
        mSoundGroupNode.removeNode();
    }
    FMOD_OS_CriticalSection_Leave(mSystem->gSoundListCrit);

    if (freethis && !mSubSoundShared)
    {   
        FMOD_Memory_Free(this);
    }

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SoundI::release", "done (%p)\n", this));

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
// OFFSET: 0x008E1140
FMOD_RESULT SoundI::getSystemObject(System **system)
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
// OFFSET: 0x008E1170
FMOD_RESULT SoundI::lock(unsigned int offset, unsigned int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2)
{
    return FMOD_ERR_BADCOMMAND;
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
// OFFSET: 0x008E1180
FMOD_RESULT SoundI::unlock(void *ptr1, void *ptr2, unsigned int len1, unsigned int len2)
{
    return FMOD_ERR_BADCOMMAND;
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
// OFFSET: 0x008E1190
FMOD_RESULT SoundI::setDefaults(float frequency, float volume, float pan, int priority)
{
    if (volume > 1)
    {
        volume = 1;
    }
    if (volume < 0)
    {
        volume = 0;
    }
    if (pan < -1)
    {
        pan = -1;
    }
    if (pan > 1)
    {
        pan = 1;
    }
    if (priority < 0)
    {
        priority = 0;
    }
    if (priority > 256)
    {
        priority = 256;
    }

    mDefaultFrequency = frequency;
    mDefaultVolume    = volume;
    mDefaultPan       = pan;
    mDefaultPriority  = priority;

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
// OFFSET: 0x008E1210
FMOD_RESULT SoundI::getDefaults(float *frequency, float *volume, float *pan, int *priority)
{
    if (frequency)
    {
        *frequency = mDefaultFrequency;
    }
    if (volume)
    {
        *volume = mDefaultVolume;
    }
    if (pan)
    {
        *pan = mDefaultPan;
    }
    if (priority)
    {
        *priority = mDefaultPriority;
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
// OFFSET: 0x008E1250
FMOD_RESULT SoundI::setVariations(float frequencyvar, float volumevar, float panvar)
{
    if (frequencyvar >= 0)
    {
        mFrequencyVariation = frequencyvar;
    }
    if (volumevar >= 0)
    {
        mVolumeVariation = volumevar;
    }
    if (panvar >= 0)
    {
        mPanVariation = panvar;
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
// OFFSET: 0x008E12A0
FMOD_RESULT SoundI::getVariations(float *frequencyvar, float *volumevar, float *panvar)
{
	if (frequencyvar) 
    {
        *frequencyvar = mFrequencyVariation;
    }

	if (volumevar)
    {
        *volumevar = mVolumeVariation;
    }

	if (panvar)
    {
        *panvar = mPanVariation;
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
// OFFSET: 0x008E12D0
FMOD_RESULT SoundI::set3DMinMaxDistance(float min, float max)
{
    if (min < 0 || max < 0 || max < min)
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SoundI::set3DMinMaxDistance", "ERROR! Invalid parameter passed in.  min = %.02f = max %.02f\n", min, max));
        return FMOD_ERR_INVALID_PARAM;
    }

    mMinDistance = min;
    mMaxDistance = max;

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
// OFFSET: 0x008E1320
FMOD_RESULT SoundI::get3DMinMaxDistance(float *min, float *max)
{
    if (min)
    {
        *min = mMinDistance;
    }

    if (max)
    {
        *max = mMaxDistance;
    }

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
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x008E1350
FMOD_RESULT SoundI::set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume)
{
    if (insideconeangle < 0)
    {
        insideconeangle = 0;
    }
    if (outsideconeangle < 0)
    {
        outsideconeangle = 0;
    }
    if (insideconeangle > 360)
    {
        insideconeangle = 360;
    }
    if (outsideconeangle > 360)
    {
        outsideconeangle = 360;
    }
    if (outsidevolume < 0)
    {
        outsidevolume = 0;
    }
    if (outsidevolume > 1)
    {
        outsidevolume = 1;
    }

    mConeInsideAngle = insideconeangle;
    mConeOutsideAngle = outsideconeangle;
    mConeOutsideVolume = outsidevolume;

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
    
	[SEE_ALSO]
]
*/
// OFFSET: 0x008E13E0
FMOD_RESULT SoundI::get3DConeSettings(float *insideconeangle, float *outsideconeangle, float *outsidevolume)
{
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
// OFFSET: 0x008E1410
FMOD_RESULT  SoundI::set3DCustomRolloff(FMOD_VECTOR *points, int numpoints)
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
// OFFSET: 0x008E14A0
FMOD_RESULT SoundI::get3DCustomRolloff(FMOD_VECTOR **points, int *numpoints)
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
// OFFSET: 0x008E14D0
FMOD_RESULT SoundI::setSubSound(int index, SoundI *subsound)
{
    return setSubSoundInternal(index, subsound, false);
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
FMOD_RESULT SoundI::setSubSoundInternal(int index, SoundI *subsound, bool calledfromrelease)
{
    int count;
    unsigned int oldlength = 0, newlength = 0;

    if (index < 0 || index >= mNumSubSounds)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (subsound && subsound->mSubSoundParent)
    {
        return FMOD_ERR_SUBSOUND_ALLOCATED;
    }

    /*
        Cannot detach or replace a subsound if the parent stream shares subsounds
    */
    if (mSubSoundShared)
    {
        return FMOD_ERR_SUBSOUND_CANTMOVE;
    }

#ifdef FMOD_SUPPORT_SENTENCING
    /*
        Do error checking that the new subsound is valid
    */
    if (subsound && mSubSoundList)
    {
        if (subsound->isStream() != isStream())
        {
            return FMOD_ERR_SUBSOUND_MODE;     /* Tried to mix a stream and a sample. */
        }

        if ((subsound->mMode & FMOD_CREATECOMPRESSEDSAMPLE) != (mMode & FMOD_CREATECOMPRESSEDSAMPLE))
        {
            return FMOD_ERR_SUBSOUND_MODE;
        }

        if (subsound->mFormat != mFormat)
        {
            return FMOD_ERR_FORMAT;     /* Tried to mix different sample formats. */
        }
        
        if (subsound->mChannels != mChannels)
        {
            return FMOD_ERR_FORMAT;     /* Tried to mix different channel counts. */
        }

        if (!(subsound->mMode & FMOD_SOFTWARE) && !subsound->isStream())
        {
            return FMOD_ERR_NEEDSSOFTWARE;
        }
    }
#endif

#ifdef FMOD_SUPPORT_STREAMING
    bool blockstream = false;

    if (isStream())
    {
        SoundI *soundi = this;

        if (soundi->mSubSoundShared)
        {
            soundi = soundi->mSubSoundShared;
        }
        else if (soundi->mSubSound)
        {
            soundi = soundi->mSubSound[soundi->mSubSoundIndex];
        }

        if (soundi && soundi->mFlags & FMOD_SOUND_FLAG_PLAYED && !(soundi->mFlags & FMOD_SOUND_FLAG_FINISHED))
        {
            blockstream = true;
        }
    }

    if (blockstream)
    {
        FMOD_OS_CriticalSection_Enter(mSystem->mStreamUpdateCrit);
    }
#endif

#ifdef FMOD_SUPPORT_SOFTWARE
    bool blockdsp = false;

    if (mMode & FMOD_SOFTWARE && !calledfromrelease)
    {
        blockdsp = true;
    }

    if (blockdsp)
    {
        FMOD_OS_CriticalSection_Enter(mSystem->mDSPCrit);
    }
#endif

    SoundI *oldsubsound = mSubSound[index];

    if (oldsubsound)
    {
        if (oldsubsound->mSubSoundShared)
        {
            FMOD_CODEC_WAVEFORMAT waveformat;
        
            mCodec->mDescription.getwaveformat(mCodec, index, &waveformat);

            oldlength = waveformat.lengthpcm;
        }
        else
        {
            oldlength = oldsubsound->mLength;
        }
    }

    /*
        Setup the new subsound
    */
    if (subsound)
    {
        if (!subsound->isStream())
        {
            Sample *sample = (Sample *)subsound;

            for (int i = 0; i < sample->mNumSubSamples; i++)
            {
                sample->mSubSample[i]->mCodec           = mCodec;

                if (oldsubsound)
                {
                    Sample *oldsample = (Sample *)oldsubsound;

                    sample->mSubSample[i]->mSubSampleParent = oldsample->mSubSampleParent;
                }
            }
        }

        subsound->mSubSoundIndex   = index;
        subsound->mSubSoundParent  = this;

        if (subsound->mSubSoundShared)
        {
            FMOD_CODEC_WAVEFORMAT waveformat;
        
            subsound->mCodec->mDescription.getwaveformat(mCodec, subsound->mSubSoundIndex, &waveformat);

            newlength = waveformat.lengthpcm;
        }
        else
        {
            newlength = subsound->mLength;
        }
    }

    /*
        If there was an old subsound here, remove the link to this parent.
    */
    if (oldsubsound)
    {
        if (!isStream() && oldsubsound->mSubSoundParent->mCodec == oldsubsound->mCodec)
        {
            oldsubsound->mCodec = 0;
        }

        oldsubsound->mSubSoundParent = 0;

        if (!subsound)
        {
            mNumActiveSubSounds--;
        }
    }
    else
    {
        if (subsound)
        {
            mNumActiveSubSounds++;
        }
    }

    mSubSound[index] = subsound;

    /*
        Recalculate the length of the sentence if one exists.
    */
#ifdef FMOD_SUPPORT_SENTENCING
    if (mSubSoundListNum)
    {       
        if (!(mCodec && mCodec->mFlags & FMOD_CODEC_USERLENGTH))
        {
            mLength -= oldlength;
            mLength += newlength;
        }

        for (count = 0; count < mSubSoundListNum; count++)
        {
            if (mSubSoundList[count].mIndex == index)
            {
                mSubSoundList[count].mLength = newlength;
            }
        }
    }
#endif

    mLoopStart  = 0;
    mLoopLength = mLength;

#ifdef FMOD_SUPPORT_STREAMING
    if (isStream())
    {
        Stream *stream = SAFE_CAST(Stream, this);

        stream->mChannel->setLoopPoints(0, mLength - 1);
    }
#endif

#ifndef PLATFORM_PS3
    /*
        For wavetable pcm, a shift of mPosition is needed to adjust for subsound changes if the sentence is already playing.
        PS3 doesn't use wavetable for PCM, it uses a codec instead.
    */
#ifdef FMOD_SUPPORT_STREAMING
    else
#endif
    if (mMode & FMOD_SOFTWARE && mFormat >= FMOD_SOUND_FORMAT_PCM8 && mFormat <= FMOD_SOUND_FORMAT_PCMFLOAT && oldlength != newlength)
    {
        for (count = 0; count < mSystem->mNumChannels; count++)
        {
            FMOD::SoundI *soundi = 0;

            mSystem->mChannel[count].getCurrentSound(&soundi);

            if (soundi == this)
            {
                unsigned int pos, currsubsound;

                mSystem->mChannel[count].setLoopPoints(mLoopStart, FMOD_TIMEUNIT_PCM, mLoopLength - 1, FMOD_TIMEUNIT_PCM);

                /*
                    Now adjust the channel's position by how much the sentence has been shifted by.
                */
                mSystem->mChannel[count].getPosition(&pos, FMOD_TIMEUNIT_PCM);
                mSystem->mChannel[count].getPosition(&currsubsound, FMOD_TIMEUNIT_SENTENCE_SUBSOUND);

                if (currsubsound > (unsigned int)index)
                {
                    pos -= oldlength;
                    pos += newlength;

                    mSystem->mChannel[count].setPosition(pos, FMOD_TIMEUNIT_PCM);
                }
            }
        }
    }
#endif


#ifdef FMOD_SUPPORT_SOFTWARE
    if (blockdsp)
    {
        FMOD_OS_CriticalSection_Leave(mSystem->mDSPCrit);
    }
#endif

#ifdef FMOD_SUPPORT_STREAMING
    if (isStream())
    {
        Stream *stream = SAFE_CAST(Stream, this);
        
        /*
            If there is a new subsound, and a new playsound occurs, tell the stream to reflush its buffer.
        */
        if (subsound)
        {
            stream->mFlags |= FMOD_SOUND_FLAG_WANTSTOFLUSH;
        }

        if (blockstream)
        {        
            FMOD_OS_CriticalSection_Leave(mSystem->mStreamUpdateCrit);
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
// OFFSET: 0x008E3A60
FMOD_RESULT SoundI::getSubSound(int index, SoundI **subsound)
{
    if (!subsound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *subsound = 0;

    #ifdef FMOD_DEBUG
    if (isStream())
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SoundI::getSubSound", "sound %p.  Subsound index %d / %d\n", this, index, mNumSubSounds));
    }
    #endif
    
    if (index < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (index >= mNumSubSounds)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    *subsound = mSubSound[index];

#ifdef FMOD_SUPPORT_NONBLOCKING
    if (mSubSound[index] && mMode & FMOD_NONBLOCKING && mSubSound[index]->isStream())
    {
        if (mSubSound[index]->mOpenState == FMOD_OPENSTATE_SEEKING)
        {
            return FMOD_ERR_NOTREADY;
        }
    }
#endif

    if (mSubSound[index] && mSubSound[index]->mSubSoundShared)
    {
        mSubSound[index]->updateSubSound(index, false);
    }
    
    if (mSubSound[index])
    {
#ifdef FMOD_SUPPORT_STREAMING
        if (mSubSound[index]->isStream())
        {
            Stream *stream = SAFE_CAST(Stream, *subsound);

            #ifdef FMOD_SUPPORT_NONBLOCKING
            if (mMode & FMOD_NONBLOCKING)
            {
                FMOD_RESULT result;
                Stream *streamParent = SAFE_CAST(Stream, stream->mSubSoundParent);

                if ((index == stream->mSubSoundParent->mSubSoundIndex && (stream->mPosition == streamParent->mInitialPosition + stream->mSample->mLength || (stream->mLength < stream->mSample->mLength && stream->mPosition == streamParent->mInitialPosition + stream->mLength))) && !(stream->mFlags & FMOD_SOUND_FLAG_WANTSTOFLUSH))
                {
                    return FMOD_OK;
                }

                FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SoundI::getSubSound", "starting non blocking seek and setting sound to not ready state\n"));

                stream->mOpenState = FMOD_OPENSTATE_SEEKING;
                mOpenState         = FMOD_OPENSTATE_SEEKING;
            
                result = AsyncThread::getAsyncThread(stream);
                if (result != FMOD_OK)
                {
                    return result;
                }

                FMOD_OS_CriticalSection_Enter(stream->mAsyncData->mThread->mCrit);
                {
                    stream->mAsyncData->mNode.setData(stream);
                    stream->mAsyncData->mNode.addBefore(&stream->mAsyncData->mThread->mHead);
                }
                FMOD_OS_CriticalSection_Leave(stream->mAsyncData->mThread->mCrit);

                stream->mAsyncData->mThread->mThread.wakeupThread();

                FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SoundI::getSubSound", "finished preparing nonblocking getSubSound seek\n"));
            }
            else
            #endif
            {
                if (stream->mSubSoundIndex != stream->mCodec->mSubSoundIndex)
                {
                    FMOD_RESULT result;

                    result = stream->setPosition(0, FMOD_TIMEUNIT_PCM);
                    if (result == FMOD_OK)
                    {
                        stream->flush();
                    }
                }
            }
        }
#endif
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
// OFFSET: 0x008E1690
FMOD_RESULT SoundI::setSubSoundSentence(int *subsoundlist, int numsubsounds)
{
    FMOD_RESULT result;
#ifdef FMOD_SUPPORT_SENTENCING
    int count;
#ifndef FMOD_STATICFORPLUGINS
    LocalCriticalSection crit(mSystem->mStreamUpdateCrit, true);
#endif

    if (!mNumSubSounds)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (subsoundlist && !numsubsounds)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!(mMode & FMOD_SOFTWARE) && !isStream())
    {
        return FMOD_ERR_NEEDSSOFTWARE;
    }

    #ifdef PLATFORM_PS3
    if (numsubsounds > FMOD_PS3_SUBSOUNDLISTMAXITEMS)
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "SoundI::setSubSoundSentence", "Maximum number of subsound sentences (%d) reached for PS3!", FMOD_PS3_SUBSOUNDLISTMAXITEMS));
        return FMOD_ERR_INTERNAL;
    }
    #endif
  
    result = mSystem->stopSound(this); 
    if (result != FMOD_OK)
    {
        return result;
    }
  
    /*
        Do error checking that the subsounds within the parent are valid.
    */
    if (subsoundlist)
    {
        if (!isStream())    /* Samples are more flexible, there is no stream buffer to worry about.  Reset the mode here */
        {
            mFormat   = FMOD_SOUND_FORMAT_NONE;
            mChannels = 0;
        }

        for (count = 0; count < numsubsounds; count++)
        {
            SoundI *subsound;

            if (subsoundlist[count] < 0 || subsoundlist[count] >= mNumSubSounds)
            {
                return FMOD_ERR_INVALID_PARAM;
            }

            subsound = mSubSound[subsoundlist[count]];
            if(!subsound)
            {
                continue;
            }

            if (subsound->isStream() != isStream())
            {
                return FMOD_ERR_SUBSOUND_MODE;     /* Tried to mix a stream and a sample. */
            }

            if ((subsound->mMode & FMOD_CREATECOMPRESSEDSAMPLE) != (mMode & FMOD_CREATECOMPRESSEDSAMPLE))
            {
                return FMOD_ERR_SUBSOUND_MODE;
            }

            if (mSubSoundShared)
            {
                FMOD_CODEC_WAVEFORMAT waveformat;
                
                mCodec->mDescription.getwaveformat(mCodec, subsoundlist[count], &waveformat);

                if (!mFormat && !mChannels)
                {
                    mFormat  = waveformat.format;
                    mChannels = waveformat.channels;
                }
                else 
                {
                    if (mFormat != waveformat.format)
                    {
                        return FMOD_ERR_FORMAT;     /* Tried to mix different sample formats. */
                    }
                    if (mChannels != waveformat.channels)
                    {
                        return FMOD_ERR_FORMAT;     /* Tried to mix different channel counts. */
                    }
                }
            }
            else 
            {
                if (!mFormat && !mChannels)
                {
                    mFormat = subsound->mFormat;
                    mChannels = subsound->mChannels;
                }
                else 
                {
                    if (mFormat != subsound->mFormat)
                    {
                        return FMOD_ERR_FORMAT;     /* Tried to mix different sample formats. */
                    }
                    if (mChannels != subsound->mChannels)
                    {
                        return FMOD_ERR_FORMAT;     /* Tried to mix different channel counts. */
                    }
                }
            }

            if (!(subsound->mMode & FMOD_SOFTWARE) && !subsound->isStream())
            {
                return FMOD_ERR_NEEDSSOFTWARE;
            }
        }
    }

    if (mSubSoundList)
    {
        FMOD_Memory_Free(mSubSoundList);
        mSubSoundList = NULL;
    }

    mSubSoundListNum = numsubsounds;

    if (mSubSoundListNum)
    {
        mSubSoundList = (SoundSentenceEntry *)FMOD_Memory_Calloc(sizeof(SoundSentenceEntry) * mSubSoundListNum);
        if (!mSubSoundList)
        {
            return FMOD_ERR_MEMORY;
        }

        if (!(mCodec && mCodec->mFlags & FMOD_CODEC_USERLENGTH))
        {
            mLength = 0;
        }

        for (count = 0; count < mSubSoundListNum; count++)
        {
            if (subsoundlist)
            {
                mSubSoundList[count].mIndex = subsoundlist[count];
            }
            else
            {
                mSubSoundList[count].mIndex = count;
            }

            if (mSubSound[mSubSoundList[count].mIndex])
            {
                if (mSubSoundShared)
                {
                    FMOD_CODEC_WAVEFORMAT waveformat;
                    
                    mCodec->mDescription.getwaveformat(mCodec, mSubSoundList[count].mIndex, &waveformat);

                    mSubSoundList[count].mLength = waveformat.lengthpcm;
                }
                else
                {
                    mSubSoundList[count].mLength = mSubSound[mSubSoundList[count].mIndex]->mLength;
                }

                if (!(mCodec && mCodec->mFlags & FMOD_CODEC_USERLENGTH))
                {
                    mLength += mSubSoundList[count].mLength;
                }
            }
            else
            {
                mSubSoundList[count].mLength = 0;
            }
        }
    }

    if (!(mCodec && mCodec->mFlags & FMOD_CODEC_USERLENGTH))
    {
        mLoopStart           = 0;
        mLoopLength          = mLength;
    }

#ifdef FMOD_SUPPORT_STREAMING
    if (isStream() && mSubSoundList)
    {
        Stream *parent    = SAFE_CAST(Stream, this);
        Stream *substream = SAFE_CAST(Stream, mSubSound[mSubSoundList[0].mIndex]);

        if (!substream)
        {
            return FMOD_OK;
        }
        
        parent->mFlags &= ~FMOD_SOUND_FLAG_FULLYBUFFERED;

        #ifdef FMOD_SUPPORT_NONBLOCKING
        if (mMode & FMOD_NONBLOCKING)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SoundI::getSubSound", "starting non blocking seek and setting sound to not ready state\n"));

            substream->mOpenState = FMOD_OPENSTATE_SEEKING;
            mOpenState         = FMOD_OPENSTATE_SEEKING;
        
            result = AsyncThread::getAsyncThread(this);
            if (result != FMOD_OK)
            {
                return result;
            }

            mAsyncData->mNode.setData(this);

            FMOD_OS_CriticalSection_Enter(mAsyncData->mThread->mCrit);
            {
                mAsyncData->mNode.addBefore(&mAsyncData->mThread->mHead);
            }
            FMOD_OS_CriticalSection_Leave(mAsyncData->mThread->mCrit);

            mAsyncData->mThread->mThread.wakeupThread();

            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SoundI::getSubSound", "finished preparing nonblocking getSubSound seek\n"));
        }
        else
        #endif
        {
            FMOD_RESULT result;
            
            mFlags &= ~FMOD_SOUND_FLAG_FULLYBUFFERED;

            result = parent->setPosition(0, FMOD_TIMEUNIT_PCM);
            if (result == FMOD_OK)
            {
                parent->flush();
            }
        }
    }
#endif
    
    return FMOD_OK;
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
// OFFSET: 0x008E1890
FMOD_RESULT SoundI::getName(char *name, int namelen)
{
    if (!name)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (namelen > FMOD_STRING_MAXNAMELEN)
    {
        namelen = FMOD_STRING_MAXNAMELEN;
    }

    if (mName)
    {
        if (mMode & FMOD_UNICODE)
        {
            FMOD_strncpyW((short *)name, (short *)mName, namelen / 2);
        }
        else
        {
            FMOD_strncpy(name, mName, namelen);
        }
    }
    else
    {
        if (mMode & FMOD_UNICODE)
        {
            FMOD_strncpyW((short *)name, (const short *)L"(null)", namelen / 2);
        }
        else
        {
            FMOD_strncpy(name, "(null)", namelen);
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
// OFFSET: 0x008E3C30
FMOD_RESULT SoundI::getLength(unsigned int *length, FMOD_TIMEUNIT lengthtype)
{
    if (!length)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

#ifdef FMOD_SUPPORT_SENTENCING
    if (lengthtype == FMOD_TIMEUNIT_SENTENCE)
    {
        *length = mSubSoundListNum;
    }
    else 
#endif
    if (lengthtype == FMOD_TIMEUNIT_PCM)
    {
        *length = mLength;
    }
    else if (lengthtype == FMOD_TIMEUNIT_MS)
    {
        if (!mDefaultFrequency)
        {
            *length = (unsigned int)-1;
        }
        else if (mLength == 0xFFFFFFFF)
        {
            *length = (unsigned int)-1;
        }
        else
        {
            *length = (unsigned int)((FMOD_UINT64)mLength * (FMOD_UINT64)1000 / (FMOD_UINT64)mDefaultFrequency);
        }
    }
    else if (lengthtype == FMOD_TIMEUNIT_PCMBYTES)
    {
        if (mLength == 0xFFFFFFFF)
        {
            *length = (unsigned int)-1;
        }
        else if (mMode & FMOD_CREATECOMPRESSEDSAMPLE)
        {
            *length = mLength * sizeof(signed short) * mChannels;
        }
        else
        {
            getBytesFromSamples(mLength, length);
        }
    }
    else
    {
        if (!mCodec)
        {
            return FMOD_ERR_INVALID_PARAM;
        }

        return mCodec->getLength(length, lengthtype);
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
// OFFSET: 0x008E2500
FMOD_RESULT SoundI::getFormat(FMOD_SOUND_TYPE *type, FMOD_SOUND_FORMAT *format, int *channels, int *bits)
{
    if (type)
    {
        *type = mType;
    }

    if (format)
    {
        *format = mFormat;
    }

    if (channels)
    {
        *channels = mChannels;
    }

    if (bits)
    {
        SoundI::getBitsFromFormat(bits);
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
FMOD_RESULT SoundI::getNumSubSounds(int *numsubsounds)
{
    if (!numsubsounds)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numsubsounds = mNumSubSounds;

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
FMOD_RESULT SoundI::getNumTags(int *numtags, int *numtagsupdated)
{
    if (!numtags && !numtagsupdated)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (numtags)
    {
        *numtags = 0;
    }
    if (numtagsupdated)
    {
        *numtagsupdated = 0;
    }

    if (mCodec && mCodec->mMetadata)
    {
        return mCodec->mMetadata->getNumTags(numtags, numtagsupdated);
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
// OFFSET: 0x008E1970
FMOD_RESULT SoundI::getTag(const char *name, int index, FMOD_TAG *tag)
{
    if (!tag)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mCodec || !mCodec->mMetadata)
    {
        return FMOD_ERR_TAGNOTFOUND;
    }

    return mCodec->mMetadata->getTag(name, index, tag);
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
// OFFSET: 0x008E19C0
FMOD_RESULT SoundI::getOpenState(FMOD_OPENSTATE *openstate, unsigned int *percentbuffered, bool *starving)
{
    if (openstate)
    {
        *openstate = mOpenState;
        if (mOpenState == FMOD_OPENSTATE_READY && (mFlags & FMOD_SOUND_FLAG_DONOTRELEASE))
        {
            *openstate = FMOD_OPENSTATE_BUFFERING;
        }
        
        if (mFlags & FMOD_SOUND_FLAG_PLAYED)
        {
            #ifdef FMOD_SUPPORT_STREAMING    
            if (isStream())
            {
                if (*openstate == FMOD_OPENSTATE_READY && mCodec && !(mSystem->mFlags & FMOD_INIT_STREAM_FROM_UPDATE))
                {
                    bool busy = false;

                    if (mCodec && mCodec->mFile)
                    {
                        mCodec->mFile->isBusy(&busy, 0);
                    }
           
                    if (!(mFlags & FMOD_SOUND_FLAG_THREADFINISHED) || busy)
                    {
                        *openstate = FMOD_OPENSTATE_STREAMING;
                    }                       
                }
            }
            else
            #endif // #ifdef FMOD_SUPPORT_STREAMING       
            {
                if (mNumAudible || mLastAudibleDSPClock == mSystem->mDSPClock.mValue)
                {
                    *openstate = FMOD_OPENSTATE_STREAMING;
                }
            }
        }
    }

    if (percentbuffered)
    {
        if (mCodec && mCodec->mFile && (mOpenState == FMOD_OPENSTATE_BUFFERING || mOpenState == FMOD_OPENSTATE_READY))
        {
            mCodec->mFile->isBusy(0, percentbuffered);
        }
        else
        {
            *percentbuffered = 0;
        }
    }

    if (starving)
    {
        *starving = false;

        if (mCodec && mCodec->mFile)
        {
            mCodec->mFile->isStarving(starving);
        }
    }

    return mAsyncData ? mAsyncData->mResult : FMOD_OK;
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
// OFFSET: 0x008E3FA0
FMOD_RESULT SoundI::readData(void *buffer, unsigned int numbytes, unsigned int *read)
{
    FMOD_RESULT           result;
    unsigned int          offbytes, readbytes, scale, position = 0, chunksize, blockalign;
    int                   bits;
    FMOD_CODEC_WAVEFORMAT waveformat;

    if (!mCodec)
    {
        return FMOD_ERR_PLUGIN_MISSING;
    }

    if (!buffer)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = getBitsFromFormat(&bits);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (!mCodec)
    {
        return FMOD_ERR_UNSUPPORTED;
    }

    if (!mCodec->mFile)
    {
        return FMOD_ERR_UNSUPPORTED;
    }
   
    result = mCodec->mDescription.getwaveformat(mCodec, mCodec->mSubSoundIndex, &waveformat);
    if (result != FMOD_OK)
    {
        mFlags &= ~FMOD_SOUND_FLAG_DONOTRELEASE;
        return result;
    }

    offbytes  = 0;
    readbytes = 0;

    scale = 1;
    if (mChannels != waveformat.channels && mChannels == 1)
    {
        scale = waveformat.channels;
    }

    chunksize = SOUND_READCHUNKSIZE;

    blockalign = 1;
    if (mMode & FMOD_CREATESTREAM)
    {
        if (mCodec->mBlockAlign)
        {
            blockalign = mCodec->mBlockAlign;
        }
    }
    else
    {
        if (waveformat.blockalign)
        {
            blockalign = waveformat.blockalign;
        }
    }

    chunksize /= blockalign;
    chunksize *= blockalign;
    if (!chunksize)
    {
        chunksize = blockalign;
    }

    while (numbytes)
    {
        unsigned int    size = numbytes > (chunksize / scale) ? (chunksize / scale) : numbytes;
        unsigned int    bytesreadtotal = 0, bytesread = 0;
        void           *ptr;

        ptr = (char *)buffer + offbytes; 

        if (mMode & FMOD_CREATECOMPRESSEDSAMPLE)
        {
            result = mCodec->mFile->read(ptr, 1, size, &bytesread);
            if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
            {
                mFlags &= ~FMOD_SOUND_FLAG_DONOTRELEASE;
                return result;
            }
        }
        else
        {
            result = mCodec->read(ptr, size, &bytesread);
            if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
            {
                mFlags &= ~FMOD_SOUND_FLAG_DONOTRELEASE;
                return result;
            }
        }

        if (mPostReadCallback && bytesread)
        {
            mPostReadCallback(mPostCallbackSound, ptr, bytesread);
        }

        bytesreadtotal += (bytesread / scale);

        /*
            File was truncated somehow, but allow it to load.
        */
        if (result == FMOD_ERR_FILE_EOF)
        {
            numbytes = bytesreadtotal;
        }

        numbytes  -= bytesreadtotal;
        offbytes  += bytesreadtotal;
        readbytes += bytesreadtotal;

        getSamplesFromBytes(bytesreadtotal, &position);
        mPosition += position;

        if (mPosition > mLength)
        {
            mPosition = mLength;
        }
    }

    if (read)
    {
        *read = readbytes;
    }

    mFlags &= ~FMOD_SOUND_FLAG_DONOTRELEASE;

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
// OFFSET: 0x008E1A60
FMOD_RESULT SoundI::seekData(unsigned int position)
{
    if (mCodec)
    {
        mCodec->reset();
    }

    return seek(mSubSoundIndex, position);
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
// OFFSET: 0x008E1AC0
FMOD_RESULT SoundI::setSoundGroup(SoundGroupI *soundgroup)
{
    Sample *streamsample = 0;

    #ifdef FMOD_SUPPORT_STREAMING
    if (isStream())
    {
        Stream *stream = SAFE_CAST(Stream, this);
        
        streamsample = stream->mSample;
    }
    #endif

    mSoundGroup = soundgroup;
    if (streamsample)
    {
        streamsample->mSoundGroup = soundgroup;
    }


    if (!mSoundGroup)
    {
        mSoundGroup = mSystem->mSoundGroup;
        if (streamsample)
        {
            streamsample->mSoundGroup = mSystem->mSoundGroup;
        }
    }

    FMOD_OS_CriticalSection_Enter(mSystem->gSoundListCrit);
    {
        mSoundGroupNode.removeNode();
        mSoundGroupNode.addBefore(&mSoundGroup->mSoundHead);
        mSoundGroupNode.setData(this);

        mSoundGroup->removeNode();
        mSoundGroup->addAfter(&mSystem->mSoundGroupUsedHead);
    }
    FMOD_OS_CriticalSection_Leave(mSystem->gSoundListCrit);

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
// OFFSET: 0x008E1BA0
FMOD_RESULT SoundI::getSoundGroup(SoundGroupI **soundgroup)
{
    if (!soundgroup)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *soundgroup = mSoundGroup;

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
// OFFSET: 0x008E1BD0
FMOD_RESULT SoundI::getNumSyncPoints(int *numsyncpoints)
{
    if (!numsyncpoints)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mSubSoundShared)    /* The syncpoint list is global to the shared sound, so we have to manually scan and find out how many belong to this subsound. */
    {
        *numsyncpoints = 0;

        if (mSyncPointHead)
        {
            SyncPoint *current = SAFE_CAST(SyncPoint, mSyncPointHead->getNext());

            while (current != mSyncPointTail)
            {
                if (current->mSubSoundIndex == mSubSoundIndex)
                {
                    (*numsyncpoints)++;
                }

                current = SAFE_CAST(SyncPoint, current->getNext());
            }
        }
    }
    else
    {
        *numsyncpoints = mNumSyncPoints;
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
// OFFSET: 0x008E1C00
FMOD_RESULT SoundI::getSyncPoint(int index, FMOD_SYNCPOINT **point)
{
    FMOD_RESULT result;
    SyncPoint *current;
    int count, numsyncpoints;

    result = getNumSyncPoints(&numsyncpoints);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (index < 0 || index >= numsyncpoints || !point)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    current = SAFE_CAST(SyncPoint, mSyncPointHead->getNext());

    count = 0;
    while (current != mSyncPointTail)
    {
        bool execute = true;

        if (mSubSoundShared && current->mSubSoundIndex != mSubSoundIndex)
        {
            execute = false;
        }

        if (execute)
        {
            if (count >= index)
            {
                *point = (FMOD_SYNCPOINT *)current;
                break;
            }
            count++;
        }
        current = SAFE_CAST(SyncPoint, current->getNext());
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
// OFFSET: 0x008E25B0
FMOD_RESULT SoundI::getSyncPointInfo(FMOD_SYNCPOINT *point, char *name, int namelen, unsigned int *offset, FMOD_TIMEUNIT offsettype)
{
    SyncPoint *spoint = (SyncPoint *)point;

    if (!point)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!name && !offset)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (name)
    {
        if (spoint->mName)
        {
            FMOD_strncpy(name, spoint->mName, namelen);
        }
        else
        {
            FMOD_strncpy(name, "", namelen);
        }
    }

    if (offset)
    {
        if (offsettype == FMOD_TIMEUNIT_PCM)
        {
            *offset = spoint->mOffset;
        }
        else if (offsettype == FMOD_TIMEUNIT_PCMBYTES)
        {
            SoundI::getBytesFromSamples(spoint->mOffset, offset, mChannels, mFormat);
        }
        else if (offsettype == FMOD_TIMEUNIT_MS)
        {
            *offset = (unsigned int)((float)spoint->mOffset * 1000.0f / mDefaultFrequency);
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
// OFFSET: 0x008E4300
FMOD_RESULT SoundI::addSyncPoint(unsigned int offset, FMOD_TIMEUNIT offsettype, const char *name, FMOD_SYNCPOINT **syncpoint, int subsound, bool fixupindicies)
{
    if (syncpoint)
    {
        *syncpoint = 0;
    }

    return addSyncPointInternal(offset, offsettype, name, syncpoint, subsound, fixupindicies);
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
FMOD_RESULT SoundI::addSyncPointInternal(unsigned int offset, FMOD_TIMEUNIT offsettype, const char *name, FMOD_SYNCPOINT **syncpoint, int subsound, bool fixupindicies)
{
    unsigned int pcm = 0;
    SyncPoint *point = 0;

    if (!mSyncPointHead)
    {
        mSyncPointHead = (SyncPoint *)FMOD_Memory_Calloc(sizeof(SyncPoint) * 2);
        if (!mSyncPointHead)
        {
            return FMOD_ERR_MEMORY;
        }
        mSyncPointTail = mSyncPointHead + 1;

        new (mSyncPointHead) SyncPoint;
        new (mSyncPointTail) SyncPoint;

        mSyncPointHead->mOffset = (unsigned int)0;
        mSyncPointHead->mName = 0;
        mSyncPointTail->mOffset = (unsigned int)-1;
        mSyncPointTail->mName = 0;

        mSyncPointTail->addAfter(mSyncPointHead);
    }

    if (syncpoint && *syncpoint)
    {
        point = (SyncPoint *)*syncpoint;

        if (name)
        {
            new (point) SyncPointNamed;
        }
        else
        {
            new (point) SyncPoint;
        }

        point->mStatic = 1;
    }
    else
    {
        if (name)
        {
            point = FMOD_Object_Alloc(SyncPointNamed);
            if (!point)
            {
                return FMOD_ERR_MEMORY;
            }
            point->mName = ((SyncPointNamed *)point)->mNameMemory;
        }
        else
        {
            point = FMOD_Object_Alloc(SyncPoint);
            if (!point)
            {
                return FMOD_ERR_MEMORY;
            }
            point->mName = 0;
        }

        if (syncpoint)
        {
            *syncpoint = (FMOD_SYNCPOINT *)point;
        }

        point->mStatic = 0;
    }

    if (offsettype == FMOD_TIMEUNIT_PCM)
    {
        pcm = offset;
    }
    else if (offsettype == FMOD_TIMEUNIT_PCMBYTES)
    {
        SoundI::getSamplesFromBytes(offset, &pcm, mChannels, mFormat);
    }
    else if (offsettype == FMOD_TIMEUNIT_MS)
    {
        pcm = (unsigned int)((float)offset / 1000.0f * mDefaultFrequency);
    }
    else
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    if (point->mName && name)
    {
        FMOD_strncpy(point->mName, name, FMOD_STRING_MAXNAMELEN);
    }
    point->mOffset = pcm;

    point->addAt(mSyncPointHead, mSyncPointTail, pcm);

    if (subsound == -1)
    {
        subsound = mSubSoundIndex;
    }

    point->mSubSoundIndex = subsound;
    point->mSound         = this;

    mNumSyncPoints++;

    /*
        Fix up indicies.
    */
    if (fixupindicies)
    {
        syncPointFixIndicies();
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
FMOD_RESULT SoundI::deleteSyncPointInternal(FMOD_SYNCPOINT *point, bool ignoresubsoundindices)
{
    SyncPoint *spoint = (SyncPoint *)point;

    if (!point)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (this != spoint->mSound)
    {
        return FMOD_ERR_INVALID_SYNCPOINT;
    }

    spoint->removeNode();

    if (!spoint->mStatic)
    {
        FMOD_Memory_Free(point);
    }

    mNumSyncPoints--;

    if (!ignoresubsoundindices)
    {
        syncPointFixIndicies();     /* Resort/shuffle syncpoints. */
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
// OFFSET: 0x008E1C40
FMOD_RESULT SoundI::deleteSyncPoint(FMOD_SYNCPOINT *point)
{
    return deleteSyncPointInternal(point, false);
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
// OFFSET: 0x008E1CB0
FMOD_RESULT SoundI::setMode(FMOD_MODE mode)
{
    /*
        Allow switching between loop modes.
    */
    if (mode & (FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI))
    {
        mMode &= ~(FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI);

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

#ifdef FMOD_SUPPORT_STREAMING
        if (isStream())
        {
            Stream *stream = SAFE_CAST(Stream, this);

            if (!stream)
            {
                return FMOD_ERR_INTERNAL;
            }

            /* Don't clear FMOD_SOUND_FLAG_FINISHED if we are fully buffered since we have already read everything required to loop */
            if ((mode & (FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI)) && !(stream->mFlags & FMOD_SOUND_FLAG_FULLYBUFFERED))
            {
                stream->mFlags &= ~(FMOD_SOUND_FLAG_FINISHED | FMOD_SOUND_FLAG_THREADFINISHED);
                if (stream->mSubSoundParent)
                {
                    stream->mSubSoundParent->mFlags &= ~(FMOD_SOUND_FLAG_FINISHED | FMOD_SOUND_FLAG_THREADFINISHED);
                }
            }

            stream->mChannel->mMode &= ~(FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI);
            stream->mChannel->mMode |= (mMode & (FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI));
        }
#endif

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
            mMode &= ~FMOD_3D;
            mMode |= FMOD_2D;
        }
        else if (mode & FMOD_3D)
        {
            mMode &= ~FMOD_2D;
            mMode |= FMOD_3D;
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
// OFFSET: 0x008E1DE0
FMOD_RESULT SoundI::getMode(FMOD_MODE *mode)
{
    if (!mode)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *mode = mMode;

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
// OFFSET: 0x008E1E00
FMOD_RESULT SoundI::setLoopCount(int loopcount)
{
    if (loopcount < -1)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

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
// OFFSET: 0x008E1E40
FMOD_RESULT SoundI::getLoopCount(int *loopcount)
{
    if (!loopcount)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *loopcount = mLoopCount;

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
// OFFSET: 0x008E2810
FMOD_RESULT SoundI::setLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype)
{
    unsigned int loopstartpcm = 0, loopendpcm = 0;

    if ((loopstarttype != FMOD_TIMEUNIT_MS && loopstarttype != FMOD_TIMEUNIT_PCM && loopstarttype != FMOD_TIMEUNIT_PCMBYTES) || 
        (loopendtype   != FMOD_TIMEUNIT_MS && loopendtype   != FMOD_TIMEUNIT_PCM && loopendtype   != FMOD_TIMEUNIT_PCMBYTES))
    {
        return FMOD_ERR_FORMAT;
    }

    if (loopstarttype == FMOD_TIMEUNIT_PCM)
    {
        loopstartpcm = loopstart;
    }
    else if (loopstarttype == FMOD_TIMEUNIT_PCMBYTES)
    {
        SoundI::getSamplesFromBytes(loopstart, &loopstartpcm, mChannels, mFormat);
    }
    else if (loopstarttype == FMOD_TIMEUNIT_MS)
    {
        loopstartpcm = (unsigned int)((float)loopstart / 1000.0f * mDefaultFrequency);
    }

    if (loopendtype == FMOD_TIMEUNIT_PCM)
    {
        loopendpcm = loopend;
    }
    else if (loopendtype == FMOD_TIMEUNIT_PCMBYTES)
    {
        SoundI::getSamplesFromBytes(loopend, &loopendpcm, mChannels, mFormat);
    }
    else if (loopendtype == FMOD_TIMEUNIT_MS)
    {
        loopendpcm = (unsigned int)((float)loopend / 1000.0f * mDefaultFrequency);
    }
    
    if (loopstartpcm >= mLength)
    {
        loopstartpcm = 0;
    }
    if (loopendpcm >= mLength)
    {
        loopendpcm = mLength - 1;
    }

    if (!loopendpcm)
    {
        loopendpcm = mLength - 1;
    }

    if (loopstartpcm >= loopendpcm)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mLoopStart = loopstartpcm;
    mLoopLength = loopendpcm - loopstartpcm + 1;

    if (isStream() && (mLoopStart != 0 || mLoopLength != mLength) && mMode & (FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI))
    {
        mFlags |= FMOD_SOUND_FLAG_WANTSTOFLUSH;
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
// OFFSET: 0x008E2BB0
FMOD_RESULT SoundI::getLoopPoints(unsigned int *loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int *loopend, FMOD_TIMEUNIT loopendtype)
{
    if ((loopstarttype != FMOD_TIMEUNIT_MS && loopstarttype != FMOD_TIMEUNIT_PCM && loopstarttype != FMOD_TIMEUNIT_PCMBYTES) || 
        (loopendtype   != FMOD_TIMEUNIT_MS && loopendtype   != FMOD_TIMEUNIT_PCM && loopendtype   != FMOD_TIMEUNIT_PCMBYTES))
    {
        return FMOD_ERR_FORMAT;
    }

    if (loopstart)
    {
        if (loopstarttype == FMOD_TIMEUNIT_PCM)
        {
            *loopstart = mLoopStart;
        }
        else if (loopstarttype == FMOD_TIMEUNIT_PCMBYTES)
        {
            SoundI::getBytesFromSamples(mLoopStart, loopstart, mChannels, mFormat);
        }
        else if (loopstarttype == FMOD_TIMEUNIT_MS)
        {
            *loopstart = (unsigned int)((float)mLoopStart * 1000.0f / mDefaultFrequency);
        }
    }

    if (loopend)
    {
        unsigned int lend = mLoopStart + mLoopLength - 1;

        if (loopendtype == FMOD_TIMEUNIT_PCM)
        {
            *loopend = lend;
        }
        else if (loopendtype == FMOD_TIMEUNIT_PCMBYTES)
        {
            SoundI::getBytesFromSamples(lend, loopend, mChannels, mFormat);
        }
        else if (loopendtype == FMOD_TIMEUNIT_MS)
        {
            *loopend = (unsigned int)((float)lend * 1000.0f / mDefaultFrequency);
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
FMOD_RESULT SoundI::setPosition(unsigned int pos)
{
    return setPositionInternal(pos);
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
FMOD_RESULT SoundI::setPositionInternal(unsigned int pcm)
{
    mPosition = pcm;

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
FMOD_RESULT SoundI::getPosition(unsigned int *pcm)
{
    if (!pcm)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *pcm = mPosition;

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
FMOD_RESULT SoundI::getMusicNumChannels(int *numchannels)
{
    if (mCodec && mCodec->mDescription.getmusicnumchannels)
    {
        return mCodec->mDescription.getmusicnumchannels(mCodec, numchannels);
    }

    return FMOD_ERR_FORMAT;
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
FMOD_RESULT SoundI::setMusicChannelVolume(int channel, float volume)
{
    if (mCodec && mCodec->mDescription.setmusicchannelvolume)
    {
        return mCodec->mDescription.setmusicchannelvolume(mCodec, channel, volume);
    }

    return FMOD_ERR_FORMAT;
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
FMOD_RESULT SoundI::getMusicChannelVolume(int channel, float *volume)
{
    if (mCodec && mCodec->mDescription.getmusicchannelvolume)
    {
        return mCodec->mDescription.getmusicchannelvolume(mCodec, channel, volume);
    }

    return FMOD_ERR_FORMAT;
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
// OFFSET: 0x008E1E80
FMOD_RESULT SoundI::setUserData(void *userdata)
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
// OFFSET: 0x008E1EA0
FMOD_RESULT SoundI::getUserData(void **userdata)
{
    if (!userdata)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *userdata = mUserData;
    
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
FMOD_RESULT SoundI::setUserDataInternal(void *userdata)
{
    mUserDataInternal = userdata;
    
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
FMOD_RESULT SoundI::getUserDataInternal(void **userdata)
{
    if (!userdata)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *userdata = mUserDataInternal;
    
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
// OFFSET: 0x008E2FA0
FMOD_RESULT SoundI::updateSubSound(int index, bool updateinfoonly)
{
    FMOD_RESULT result;
    FMOD_CODEC_WAVEFORMAT waveformat;
#ifndef FMOD_STATICFORPLUGINS
    LocalCriticalSection crit(mSystem->mStreamUpdateCrit);
    bool frommainthread = true;
    FMOD_UINT_NATIVE threadid;
       
    FMOD_OS_Thread_GetCurrentID(&threadid);
    if (threadid != mSystem->mMainThreadID)
    {
        frommainthread = false;
    }

    /* 
        If it is playing, we can't replace it while it is still running. 
    */
    if (frommainthread && !updateinfoonly)
    {
        mSubSoundIndex = index;
    
        mSystem->stopSound(this);  

        /* 
            Just stop only if false, async thread will do stream swap later. 
        */
        if (mMode & FMOD_NONBLOCKING && isStream())
        {
            return FMOD_OK;
        }
    }
   
    if (isStream() && (mFlags & FMOD_SOUND_FLAG_PLAYED))
    {
        Stream *stream = (Stream *)this;
        
        if (!(stream->mFlags & FMOD_SOUND_FLAG_FINISHED))    
        {
            crit.enter();   /* Ok, in async thread, so lets fully block this against the stream thread. */
        }

        stream->mChannel->mFinished = false;
    }
#endif
    
    mFlags &= ~(FMOD_SOUND_FLAG_FINISHED | FMOD_SOUND_FLAG_THREADFINISHED);
    mFlags &= ~FMOD_SOUND_FLAG_PLAYED;
    if (mSubSoundParent)
    {
        mSubSoundParent->mFlags &= ~(FMOD_SOUND_FLAG_FINISHED | FMOD_SOUND_FLAG_THREADFINISHED);

        mMode &= ~(FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI);
        mMode |= (mSubSoundParent->mMode & (FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI));
    }

    mSubSoundIndex = index;

    result = mCodec->mDescription.getwaveformat(mCodec, index, &waveformat);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mName)
    {
        FMOD_strcpy(mName,  waveformat.name);
    }
    mFormat             = waveformat.format;
    mChannels           = waveformat.channels;
    mDefaultFrequency   = (float)waveformat.frequency;
    mDefaultChannelMask = waveformat.channelmask;
    mLoopStart          = waveformat.loopstart;
    mLoopLength         = waveformat.loopend - waveformat.loopstart + 1;
    mLength             = waveformat.lengthpcm;

    setLoopPoints(waveformat.loopstart, FMOD_TIMEUNIT_PCM, waveformat.loopend, FMOD_TIMEUNIT_PCM);
    
#ifndef FMOD_STATICFORPLUGINS
    if (isStream())
    {
        Stream *stream = (Stream *)this;

        // The channel gets the channelmask from mSample, so set it correctly here
        stream->mSample->mDefaultChannelMask = waveformat.channelmask;
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
FMOD_RESULT SoundI::syncPointFixIndicies()
{
    int count, numsyncpoints = 0;
    FMOD_RESULT result;

    result = getNumSyncPoints(&numsyncpoints);
    if (result == FMOD_OK)
    {
        for (count = 0; count < numsyncpoints; count++)
        {
            SyncPoint *point;

            result = getSyncPoint(count, (FMOD_SYNCPOINT **)&point);
            if (result == FMOD_OK)
            {
                point->mIndex = count;
            }
        }
    }

    return FMOD_OK;
}


/*
[API]
[
	[DESCRIPTION]
    Used for 2 purposes.<br>
    One use is for user created sounds when FMOD_OPENUSER is specified when creating the sound.<br>
    The other use is to 'piggyback' on FMOD's read functions when opening a normal sound, therefore the callee can read (rip) or even write back new PCM data while FMOD is opening the sound.

	[PARAMETERS]
    'sound'     Pointer to the sound.  C++ users see remarks.
    'data'          Pointer to raw PCM data that the user can either read or write to.
    'datalen'       Length of the data in bytes.

	[RETURN_VALUE]

	[REMARKS]
    <u>C++ Users</u>.  Cast <b>FMOD_SOUND *</b> to <b>FMOD::Sound *</b> inside the callback and use as normal.<br>
    <br>
    The format of the sound can be retrieved with Sound::getFormat from this callback.  This will allow the user to determine what type of pointer to use if they are not sure what format the sound is.<br>
    If the callback is used for the purpose of 'piggybacking' normal FMOD sound loads, then you do not have to do anything at all, and it can be treated as purely informational.  The return value is also ignored.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    Sound::getFormat
    FMOD_SOUND_PCMSETPOSCALLBACK
    System::createSound
    System::createStream
    FMOD_CREATESOUNDEXINFO
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_SOUND_PCMREADCALLBACK(FMOD_SOUND *sound, void *data, unsigned int datalen)
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
    Callback for when the caller calls a seeking function such as Channel::setTime or Channel::setPosition.<br>
    If the sound is a user created sound, this can be used to seek within the user's resource.<br>

	[PARAMETERS]
    'sound'         Pointer to the sound.  C++ users see remarks.
    'subsound'      In a multi subsound type sound (ie fsb/dls/cdda), this will contain the index into the list of sounds.
    'position'      Position to seek to that has been requested.  This value will be of format FMOD_TIMEUNIT and must be parsed to determine what it is.  Generally FMOD_TIMEUNIT_PCM will be the most common format.
    'postype'       Position type that the user wanted to seek with.  If the sound is a user create sound and the seek type is unsupported return FMOD_ERR_FORMAT.

	[RETURN_VALUE]

	[REMARKS]
    <u>C++ Users</u>.  Cast <b>FMOD_SOUND *</b> to <b>FMOD::Sound *</b> inside the callback and use as normal.<br>
    <br>
    If the callback is used for the purpose of 'piggybacking' normal FMOD sound loads, then you do not have to do anything at all, and it can be treated as purely informational.  The return value is also ignored.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    FMOD_SOUND_PCMREADCALLBACK
    System::createSound
    System::createStream
    FMOD_CREATESOUNDEXINFO
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_SOUND_PCMSETPOSCALLBACK(FMOD_SOUND *sound, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
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
    Callback to be called when a sound has finished loading.

	[PARAMETERS]
    'sound'     Pointer to the sound.  C++ users see remarks.
    'result'    Error code. FMOD_OK if sound was created successfully, or an error code otherwise.

	[RETURN_VALUE]

	[REMARKS]
    <u>C++ Users</u>.  Cast <b>FMOD_SOUND *</b> to <b>FMOD::Sound *</b> inside the callback and use as normal.<br>
    <br>
    Return code currently ignored.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    System::createSound
    FMOD_CREATESOUNDEXINFO
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_SOUND_NONBLOCKCALLBACK(FMOD_SOUND *sound, FMOD_RESULT result)
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
FMOD_RESULT SoundI::getMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, FMOD_MEMORY_USAGE_DETAILS *memoryused_details)
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

FMOD_RESULT SoundI::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(false, FMOD_MEMBITS_SOUND, sizeof(SoundI));

    if (mName)
    {
        tracker->add(false, FMOD_MEMBITS_STRING, FMOD_STRING_MAXNAMELEN);
    }

    if (mSyncPointHead)
    {
        SyncPoint *point = SAFE_CAST(SyncPoint, mSyncPointHead->getNext());

        while (point != mSyncPointTail)
        {
            if (point->mName)
            {
                tracker->add(false, FMOD_MEMBITS_SYNCPOINT, sizeof(SyncPointNamed));
            }
            else
            {
                tracker->add(false, FMOD_MEMBITS_SYNCPOINT, sizeof(SyncPoint));
            }
               
            point = SAFE_CAST(SyncPoint, point->getNext());
        }

        tracker->add(false, FMOD_MEMBITS_SYNCPOINT, sizeof(SyncPoint) * 2);    /* mSyncPointHead & mSyncPointTail */
    }

#ifdef FMOD_SUPPORT_STREAMING
    if (isStream() && mCodec && (!mSubSoundParent || mSubSoundParent == this || (mSubSoundParent && mCodec != mSubSoundParent->mCodec)))
    {
        Stream *stream = SAFE_CAST(Stream, this);

        if (mCodec)
        {
            CHECK_RESULT(mCodec->getMemoryUsed(tracker));
        }
    }
    else
#endif
    {
        if (mCodec && ((mSubSoundParent && mCodec != mSubSoundParent->mCodec) || !mSubSoundParent))
        {
            CHECK_RESULT(mCodec->getMemoryUsed(tracker));
        }
    }

    /*
        Remove all subsounds of this sound if they exist!
    */
    if (mNumSubSounds && mSubSound)
    {
        int count;

        if (mNumActiveSubSounds)
        {
            for (count = 0; count < mNumSubSounds; count++)
            {
                if (mSubSound[count] && mSubSound[count] != mSubSoundShared)
                {
                    mSubSound[count]->getMemoryUsed(tracker);
                }
            }
        }

        tracker->add(false, FMOD_MEMBITS_SOUND, mNumSubSounds * sizeof(SoundI *));    /* mSubSound */
    
        if (mSubSoundShared)
        {
            mSubSoundShared->getMemoryUsed(tracker);
        }
    }

    if (mAsyncData)
    {
        tracker->add(false, FMOD_MEMBITS_SOUND, sizeof(mAsyncData));
    }

#ifdef FMOD_SUPPORT_SENTENCING
    if (mSubSoundList)
    {
        tracker->add(false, FMOD_MEMBITS_SOUND, sizeof(SoundSentenceEntry) * mSubSoundListNum);
    }
#endif

    return FMOD_OK;
}

#endif

#endif

}
