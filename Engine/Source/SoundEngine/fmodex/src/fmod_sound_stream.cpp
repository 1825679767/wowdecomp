#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_STREAMING

#include "fmod_async.h"
#include "fmod_codeci.h"
#include "fmod_channel_stream.h"
#include "fmod_file.h"
#include "fmod_sound_sample.h"
#include "fmod_sound_stream.h"
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
// OFFSET: 0x0091E480
Stream::Stream()
{
    mLastPos          = 0;
    mSubSound         = 0;
    mLoopCountCurrent = -1;
    mBlockSize        = 1;
    mChannel          = 0;
    mInitialPosition  = 0;
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
// OFFSET: 0x0091DE80
FMOD_RESULT Stream::fill(unsigned int offset, unsigned int length, unsigned int *read_out, bool calledfromsentence)
{
    FMOD_RESULT result = FMOD_OK;
    unsigned int read = 0;
    Sample *sample = mSample;

    if (read_out)
    {
        *read_out = 0;
    }

    if (mSubSoundParent)
    {
        sample = ((Stream *)mSubSoundParent)->mSample;
    }

    if (!(mFlags & FMOD_SOUND_FLAG_FINISHED))
    {
        unsigned int r;
        unsigned int size;
        unsigned int len;

#ifdef FMOD_SUPPORT_SENTENCING
        if (mSubSoundList)
        {
            Stream *stream = this;
            int count = 0;

            do
            {       
                stream = SAFE_CAST(Stream, mSubSound[mSubSoundList[mChannel->mSubSoundListCurrent].mIndex]);
                if (!stream)
                {
                    if (count >= mSubSoundListNum)
                    {
                        return FMOD_ERR_SUBSOUNDS;
                    }
                    
                    count++;
                    mChannel->mSubSoundListCurrent++;
                    if (mChannel->mSubSoundListCurrent >= mSubSoundListNum)
                    {
                        if (!(mMode & FMOD_LOOP_NORMAL && mLoopCountCurrent))
                        {
                            mChannel->mSubSoundListCurrent = mSubSoundListNum - 1;
                            mPosition = mLength;
                            mFlags |= FMOD_SOUND_FLAG_FINISHED;
                            return FMOD_ERR_FILE_EOF;
                        }
                        else
                        {
                            mChannel->mSubSoundListCurrent = 0;
                            mPosition = 0;
                        }
                    }
                }
            } while (!stream);

            len = length;
            do
            {
                unsigned int r;
                unsigned int endpoint;

                if (mMode & FMOD_LOOP_NORMAL && mLoopCountCurrent)
                {
                    endpoint = mLoopStart + mLoopLength - 1;
                }
                else
                {
                    if (sample->mCodec->mFlags & FMOD_CODEC_ACCURATELENGTH)
                    {
                        endpoint = mLength - 1;
                    }
                    else
                    {
                        endpoint = (unsigned int)-1;
                    }
                }

                size = len;
                if (mPosition > endpoint)
                {
                    size = 0;
                }
                else if (mPosition + size > endpoint)
                {
                    size = (endpoint - mPosition) + 1;
                }

                result = stream->fill(offset, size, &r, true);
			    if ((result != FMOD_OK) && (result != FMOD_ERR_FILE_EOF))
			    {
				    return result;
			    }
                
                read += r;
                len -= r;
                size -= r;
                offset += r;

                mPosition += r;

                if (mPosition > endpoint || result == FMOD_ERR_FILE_EOF)
                {                
                    bool finished = false;
                
                    if (mPosition > endpoint)
                    {
                        if (mMode & FMOD_LOOP_NORMAL && mLoopCountCurrent)
                        {
                            setPosition(mLoopStart, FMOD_TIMEUNIT_PCM);

                            if (mLoopCountCurrent > 0)
                            {
                                mLoopCountCurrent--;
                            }
                            
                            stream = SAFE_CAST(Stream, mSubSound[mSubSoundIndex]);
                        }
                        else
                        {
                            mPosition = mLength;
                            mFlags |= FMOD_SOUND_FLAG_FINISHED;
                            break;
                        }
                    }
                    else
                    {
                        do 
                        {
                            mChannel->mSubSoundListCurrent++;
                            if (mChannel->mSubSoundListCurrent >= mSubSoundListNum)
                            {
                                if (!(mMode & FMOD_LOOP_NORMAL && mLoopCountCurrent))
                                {
                                    finished = true;
                                    mChannel->mSubSoundListCurrent = mSubSoundListNum - 1;
                                    mPosition = mLength;
                                    mFlags |= FMOD_SOUND_FLAG_FINISHED;
                                    break;
                                }
                                else
                                {
                                    mChannel->mSubSoundListCurrent = 0;
                                    mPosition = 0;
                                }
                            }

                            mSubSoundIndex = mSubSoundList[mChannel->mSubSoundListCurrent].mIndex;

                            stream = SAFE_CAST(Stream, mSubSound[mSubSoundIndex]);
                        
                            if (mSubSoundShared)
                            {
                                stream->updateSubSound(mSubSoundIndex, true);
                            }

                        } while (!stream && !finished);                             /* While loop skips past null entries. */

                        if (finished)
                        {
                            break;
                        }

                        if (stream)
                        {
                            sample->mCodec = stream->mCodec;   /* Switch parent level codec to the new substream's codec */
                            if (mCodec != sample->mCodec)
                            {
                                result = sample->seek(-1, 0);
                            }
                            else
                            {
                                result = sample->seek(mSubSoundIndex, 0);
                            }
                            stream->mPosition = 0;
                            stream->mFlags &= ~FMOD_SOUND_FLAG_FINISHED;    /* If it was read and finished before, the seek to 0 will unfinish it. */
                        }
                    }
                }

            } while (len);
        }
		else
#endif
        {
			len = length;
			do 
			{
				unsigned int endpoint, streamlength = mLength;
				Stream *stream = this;
                unsigned int bytestoread = 0;

				if (mSubSound)
				{
					stream = SAFE_CAST(Stream, mSubSound[mSubSoundIndex]);
					if (!stream)
					{
						break;
					}
				}
                
				if (mSubSoundShared)
				{
					FMOD_CODEC_WAVEFORMAT waveformat;

					stream->mCodec->mDescription.getwaveformat(stream->mCodec, mSubSoundIndex, &waveformat);

					streamlength = waveformat.lengthpcm;
				}
				else
				{
					streamlength = stream->mLength;
				}

				if (mMode & FMOD_LOOP_NORMAL && mLoopCountCurrent && !calledfromsentence)   /* Ignore loop modes/points in a sentence. */
				{
					endpoint = stream->mLoopStart + stream->mLoopLength - 1;
				}
				else
				{
					if (sample->mCodec->mFlags & FMOD_CODEC_ACCURATELENGTH)
					{
						endpoint = streamlength - 1;
					}
					else
					{
						endpoint = (unsigned int)-1;
					}
				}

				size = len;

				if (offset + size > sample->mLength)
				{
					size = sample->mLength - offset;
				}
				if (stream->mPosition > endpoint)
				{
					size = 0;
				}
				else if (stream->mPosition + size > endpoint)
				{
					size = (endpoint - stream->mPosition) + 1;
				}

                /*
                    Handle unaligned read at the end.
                */
                getBytesFromSamples(size, &bytestoread, false);
                if (!bytestoread)
                {
                    r = 0;
                    result = FMOD_ERR_FILE_EOF;
                }
                else
                {
				    result = sample->read(offset, size, &r);
				    if ((result != FMOD_OK) && (result != FMOD_ERR_FILE_EOF) && (result != FMOD_ERR_FILE_DISKEJECTED))
				    {
					    return result;
				    }
                }
            
				if (result == FMOD_OK && !r && size)
				{
					break;
				}
            
				read += r;
				len -= r;
				size -= r;
				offset += r;
				if (offset >= sample->mLength)
				{
					offset = 0;
				}

				stream->mLastPos = stream->mPosition;
				stream->mPosition += r;

				if (read_out)
				{
					*read_out = read;
				}

				if (stream->mPosition > endpoint || result == FMOD_ERR_FILE_EOF)
				{
					if (calledfromsentence)
					{
						return FMOD_ERR_FILE_EOF; /* Let the parent sentence behavior take care of the transition. */
					}

					if (mMode & FMOD_LOOP_NORMAL && mLoopCountCurrent)
					{
						stream->mPosition = mLoopStart;

						if (mLength != (unsigned int)-1)
						{
							result = sample->seek(mSubSoundIndex, stream->mPosition);
							if (result != FMOD_OK)
							{
								return result;
							}

							stream->mPosition = sample->mPosition;
						}

						if (mLoopCountCurrent > 0)
						{
							mLoopCountCurrent--;
						}
					}
					else
					{
						if (stream != this)
						{
							mPosition = mLength;
							mFlags |= FMOD_SOUND_FLAG_FINISHED;
						}
						stream->mPosition = streamlength;
						stream->mFlags |= FMOD_SOUND_FLAG_FINISHED;

                        if (stream->mSubSoundParent)
                        {
                            stream->mSubSoundParent->mFlags |= FMOD_SOUND_FLAG_FINISHED;
                        }
						break;
					}
				}
				else
				{
					if (!r)
					{
						break;
					}
				}

			} while (len);
		}
    }

    if (read < length)
    {
        unsigned int size;
        unsigned int len;
                
        len = length - read;
        do 
        {
            size = len;
            if (offset + size > sample->mLength)
            {
                size = sample->mLength - offset;
            }

            sample->clear(offset, size);

            len -= size;
            offset += size;
            if (offset >= sample->mLength)
            {
                offset = 0;
            }
        } while (len);
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
// OFFSET: 0x0091E1D0
FMOD_RESULT Stream::flush()
{
    if (mSample)
    {
        FMOD_RESULT result;
        unsigned int filllength;

        if (!(mMode & FMOD_OPENUSER) && mLength <= mSample->mLength && !mSubSoundList && mSample->mMode & FMOD_SOFTWARE)
        {
            filllength = mLength;
        }
        else
        {
            filllength = mSample->mLength;
        }

        result = fill(0, filllength);
        if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
        {
            return result;
        }

        if (filllength < mSample->mLength)
        {
            result = mSample->clear(filllength, mSample->mLength - filllength);
        }
    }

    mFlags &= ~FMOD_SOUND_FLAG_WANTSTOFLUSH;

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
// OFFSET: 0x0091E200
FMOD_RESULT Stream::setPosition(unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result = FMOD_OK;
    bool seekable = true;
    unsigned int endpoint;

    if (postype == FMOD_TIMEUNIT_PCM)
    {  
        if (mMode & FMOD_LOOP_OFF)
        {
            endpoint = mLength - 1;
        }
        else
        {
            endpoint = mLoopStart + mLoopLength - 1;
        }

        if (position > endpoint)
        {
            return FMOD_ERR_INVALID_POSITION;
        }
    }

    if (mCodec->mFile)
    {
        seekable = mCodec->mFile->mFlags & FMOD_FILE_SEEKABLE ? true : false;
    }

    mFlags &= ~(FMOD_SOUND_FLAG_FINISHED | FMOD_SOUND_FLAG_THREADFINISHED);
    if (mSubSoundParent)
    {
        mSubSoundParent->mFlags &= ~(FMOD_SOUND_FLAG_FINISHED | FMOD_SOUND_FLAG_THREADFINISHED);
    }

    if (seekable)
    {
#ifdef FMOD_SUPPORT_SENTENCING
        if (mSubSound && mSubSoundList && postype == FMOD_TIMEUNIT_PCM)
        {
            int count;
            unsigned int offset = 0;

            for (count = 0; count < mSubSoundListNum; count++)
            {     
                SoundI *subsound = mSubSound[mSubSoundList[count].mIndex];

                if (subsound)
                {
                    FMOD_RESULT result;

                    if (!mSubSoundList)
                    {
                        return FMOD_ERR_INVALID_PARAM;
                    }

                    if (position >= offset && position < offset + mSubSoundList[count].mLength)
                    {
                        Stream *substream = SAFE_CAST(Stream, subsound);

                        mChannel->mSubSoundListCurrent = count;
                        mSubSoundIndex       = mSubSoundList[count].mIndex;

                        if (mSubSoundShared)    /* Same codec. */
                        {
                            result = substream->updateSubSound(mSubSoundIndex, true);
                        }
                        else
                        {
                            substream->mSubSoundIndex = mSubSoundIndex;
                            mSample->mCodec = substream->mCodec;   /* Switch parent level codec to the new substream's codec */
                        }
                        
                        result = substream->setPosition(position - offset, postype);
                        break;
                    }

                    offset += mSubSoundList[count].mLength;
                }
            }
        }
        else 
#endif
        if (mSubSound && postype == FMOD_TIMEUNIT_PCM)
        {
            SoundI *subsound = mSubSound[mSubSoundIndex];

            if (subsound)
            {
                Stream *substream = SAFE_CAST(Stream, subsound);

                result = substream->setPosition(position, postype);
            }
        }
        else
        {
            mCodec->reset();

            result = mCodec->setPosition(((mSubSoundParent && mSubSoundParent->mNumSubSounds) || mNumSubSounds) ? mSubSoundIndex : 0, position, postype);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (mSubSoundParent)
            {
                mSubSoundParent->mSubSoundIndex = mSubSoundIndex;
            }
        }

        if (mSample && mSample->mPostSetPositionCallback)
        {
            mSample->mPostSetPositionCallback((FMOD_SOUND *)this, mSubSoundIndex, position, postype);
        }

        if (postype != FMOD_TIMEUNIT_MS && postype != FMOD_TIMEUNIT_PCM && postype != FMOD_TIMEUNIT_PCMBYTES)
        {
            position = 0;
        }

        mLastPos = mPosition = position;
    }
    else
    {
        if (mLastPos != 0 || position != 0)
        {
            return FMOD_ERR_FILE_COULDNOTSEEK;
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
// OFFSET: 0x0091E4D0
FMOD_RESULT Stream::getPosition(unsigned int *position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;

    if (mOpenState != FMOD_OPENSTATE_READY && mOpenState != FMOD_OPENSTATE_SETPOSITION)
    {
        return FMOD_ERR_NOTREADY;
    }

    if (!position)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (postype == (FMOD_TIMEUNIT)(FMOD_TIMEUNIT_SENTENCE_SUBSOUND | FMOD_TIMEUNIT_BUFFERED))
    {
        *position = mChannel->mSubSoundListCurrent;
    }
    else if (postype == FMOD_TIMEUNIT_PCM || postype == FMOD_TIMEUNIT_PCMBYTES || postype == FMOD_TIMEUNIT_MS)
    {
        if (postype == FMOD_TIMEUNIT_PCM)
        {
            *position = mLastPos;
        }
        else if (postype == FMOD_TIMEUNIT_MS)
        {
            *position = (unsigned int)((float)mLastPos / 1000.0f * mDefaultFrequency);
        }
        else if (postype == FMOD_TIMEUNIT_PCM)
        {            
            getBytesFromSamples(mLastPos, position);
        }
    }
    else
    {
        result = mCodec->getPosition(position, postype);
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
// OFFSET: 0x0091E450
FMOD_RESULT Stream::setLoopCount(int loopcount)
{
    if (mOpenState != FMOD_OPENSTATE_READY && mOpenState != FMOD_OPENSTATE_SETPOSITION)
    {
        return FMOD_ERR_NOTREADY;
    }

    mLoopCountCurrent = loopcount;
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
#ifdef FMOD_SUPPORT_MEMORYTRACKER

FMOD_RESULT Stream::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(false, FMOD_MEMBITS_SOUND, sizeof(Stream) - sizeof(SoundI));

    if (mSample && (!mSubSoundParent || (mSubSoundParent && ((Stream *)mSubSoundParent)->mSample != mSample)))
    {
        CHECK_RESULT(mSample->getMemoryUsed(tracker));
    }

    if (mChannel && (!mSubSoundParent || (mSubSoundParent && ((Stream *)mSubSoundParent)->mChannel != mChannel)))
    {
        tracker->add(false, FMOD_MEMBITS_SOUND, sizeof(ChannelStream));
    }

    return SoundI::getMemoryUsedImpl(tracker);
}

#endif

}

#endif  /* #ifdef FMOD_SUPPORT_STREAMING */
