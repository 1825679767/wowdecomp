#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_SOFTWARE

#include "fmod_channel_software.h"
#include "fmod_dsp_wavetable.h"
#include "fmod_dsp_resampler.h"
#include "fmod_dsp_resampler_nointerp.h"
#include "fmod_dsp_resampler_linear.h"
#include "fmod_dsp_resampler_cubic.h"
#include "fmod_dsp_resampler_spline.h"
#include "fmod_sample_software.h"

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
// OFFSET: 0x00929DA0
FMOD_RESULT DSPWaveTable::alloc(FMOD_DSP_DESCRIPTION_EX *description)
{
    FMOD_RESULT result;

    result = DSPI::alloc(description);
    if (result != FMOD_OK)
    {
        return result;
    }

    mFrequency = 0;
    result = mSystem->getSoftwareFormat(&mTargetFrequency, 0, 0, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    mDirection = DSPWAVETABLE_SPEEDDIR_FORWARDS;
    mNewPosition   = 0xFFFFFFFF;
    mDSPFinishTick = 0xFFFFFFFF;
    mDSPTick       = 0xFFFFFFFF;

    if (description->mDSPSoundCard)
    {
        mDSPSoundCard = description->mDSPSoundCard;
    }
    else
    {
        mDSPSoundCard = mSystem->mDSPSoundCard;
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
// OFFSET: 0x00929DE0
FMOD_RESULT DSPWaveTable::addInput(DSPI *target)
{
    FMOD_RESULT result;

    result = DSPI::addInput(target);
    if (result != FMOD_OK)
    {
        return result;
    }

    return  FMOD_OK;
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
FMOD_RESULT DSPWaveTable::read(float **outbuffer, int *outchannels, unsigned int *length, FMOD_SPEAKERMODE speakermode, int speakermodechannels, unsigned int tick)
{
    FMOD_RESULT result = FMOD_OK;
   
    mFlags &= ~FMOD_DSP_FLAG_IDLE;
    mFlags &= ~FMOD_DSP_FLAG_FINISHED;

    if (mDSPTick != tick)
    {
        void            *readbuffer;
        SampleSoftware  *samplesoftware;
        unsigned int     outlength       = *length;
        unsigned int     readoffset      = 0;
        unsigned int     soundoffset     = 0;
        unsigned int     soundlength     = 0;
        FMOD_SINT64P     origspeed;
	    unsigned int     starttime       = 0;
	    unsigned int     endtime         = 0;

        if (mSystem->mFlags & FMOD_INIT_ENABLE_PROFILE)
        {
            FMOD_OS_Time_GetNs(&starttime);
        }
        
        if (mNewPosition != 0xFFFFFFFF)
        {
            mPosition.mHi = mNewPosition;
            mPosition.mLo = 0;
            mNewPosition = 0xFFFFFFFF;
        }

#ifdef FMOD_SUPPORT_SENTENCING
        if (mSound->mSubSoundList)  /* Pick the right sound pointer to mix from, set the offset. */
        {
            int count;

            soundoffset = 0;
            count = 0;
            samplesoftware = SAFE_CAST(SampleSoftware, mSound->mSubSound[mSound->mSubSoundList[count].mIndex]);

            while (count < mChannel->mSubSoundListCurrent)
            {
                if (samplesoftware)
                {
                    soundoffset += samplesoftware->mLength;
                }
                count++;
                samplesoftware = SAFE_CAST(SampleSoftware, mSound->mSubSound[mSound->mSubSoundList[count].mIndex]);
            }

            if (!samplesoftware)
            {
    	        return FMOD_ERR_INVALID_PARAM;
            }
        }
        else
#endif
        {
            samplesoftware = (SampleSoftware *)mSound;
            if (!samplesoftware)
            {
    	        return FMOD_ERR_INVALID_PARAM;
            }
        }

        soundlength = samplesoftware->mLength;

        readbuffer = samplesoftware->mBuffer;
        if (!readbuffer)
        {
            FMOD_memset(mBuffer, 0, outlength * sizeof(float) * mSound->mChannels);
            mDSPFinishTick = tick;
            mFlags |= FMOD_DSP_FLAG_FINISHED;
            *outchannels = mSound->mChannels;
            return FMOD_OK;
        }
        origspeed  = mSpeed;

        if (mDSPClockEnd.mValue > 0 &&
            mDSPClockEnd.mValue < mSystem->mDSPClock.mValue + outlength)
        {
            unsigned int diff = (unsigned int)(mSystem->mDSPClock.mValue + outlength - mDSPClockEnd.mValue);
            if (diff > outlength)
            {
                diff = outlength;
            }

            FMOD_memset(mBuffer + ((outlength - diff) * mSound->mChannels), 0, diff * sizeof(float) * mSound->mChannels);
            outlength -= diff;

            mDSPFinishTick = tick;
            mFlags |= FMOD_DSP_FLAG_FINISHED;
        }
        if (mDSPClockPause.mValue > 0 &&
            mDSPClockPause.mValue < mSystem->mDSPClock.mValue + outlength)
        {
            unsigned int diff = (unsigned int)(mSystem->mDSPClock.mValue + outlength - mDSPClockPause.mValue);
            if (diff > outlength)
            {
                diff = outlength;
            }

            FMOD_memset(mBuffer + ((outlength - diff) * mSound->mChannels), 0, diff * sizeof(float) * mSound->mChannels);
            outlength -= diff;
            
            mFlags &= ~FMOD_DSP_FLAG_ACTIVE;
            mDSPClockPause.mValue = 0;
        }

        if (mDSPClockStart.mValue > mSystem->mDSPClock.mValue && 
            mDSPClockStart.mValue + outlength > mSystem->mDSPClock.mValue)
        {
            unsigned int diff = (unsigned int)(mDSPClockStart.mValue - mSystem->mDSPClock.mValue);
            if (diff > outlength)
            {
                diff = outlength;
            }

            if (diff)
            {
                FMOD_memset(mBuffer, 0, diff * sizeof(float) * mSound->mChannels);
                outlength -= diff;
                readoffset += diff;
            }          
        }

        while (outlength > 0)
        {
            unsigned int        rlength;
            FMOD_SINT64P        speed;
            FMOD_UINT64P        pos;
            FMOD_RESAMPLER_END  endflag = FMOD_RESAMPLER_END_MIXBUFFER;

            /*
                Work out what is going to end first.  The source data or the output?
            */
            pos      = mPosition;
            pos.mHi -= soundoffset;     /* Take it out of 'sentence space' into 'sample space'. */
            speed    = origspeed;
            if (mDirection == DSPWAVETABLE_SPEEDDIR_BACKWARDS)
            {
                speed.mValue = -speed.mValue;
            }

            rlength = outlength;     /* Start it off defaulting to the amount requested */
            if (origspeed.mValue > 0x100)
            {
                FMOD_UINT64P mixesleft, fracleft;
                bool eos = false;

                if (mDirection == DSPWAVETABLE_SPEEDDIR_BACKWARDS)
                {
                    if (mChannel->mMode & (FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI) && mChannel->mLoopCount && pos.mHi >= mChannel->mLoopStart)
                    {
                        mixesleft.mHi = pos.mHi - mChannel->mLoopStart;
                    }
                    else
                    {
                        mixesleft.mHi = pos.mHi;
                    }
                    mixesleft.mLo = 0;

                    if (mixesleft.mHi > soundlength)
                    {
                        mixesleft.mValue = pos.mValue;
                        eos = true;
                    }
                }
                else
                {
                    if (mChannel->mMode & (FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI) && mChannel->mLoopCount)
                    {
                        mixesleft.mHi = mChannel->mLoopStart + mChannel->mLoopLength;
                    }
                    else
                    {
                        mixesleft.mHi = mSound->mLength;
                    }
                    mixesleft.mLo = 0;

                    if (mixesleft.mValue > pos.mValue)
                    {
                        mixesleft.mValue -= pos.mValue;
                    }
                    else
                    {
                        mixesleft.mValue = 0;
                    }

                    if (pos.mHi + mixesleft.mHi > soundlength)    /* Sentence subsound length clamp (or just safety in case somehow one sound mixleft > length */
                    {
                        mixesleft.mHi = soundlength - pos.mHi;
                        eos = true;
                    }
                }
             
                fracleft.mValue   = mixesleft.mValue % origspeed.mValue;
                mixesleft.mValue /= origspeed.mValue;
                if (fracleft.mValue)						/* round the count up (this could be done better) */
                {
					mixesleft.mValue++;
                }

				if (mixesleft.mLo <= rlength)
    			{
                    endflag = FMOD_RESAMPLER_END_SOUND;
					rlength = mixesleft.mLo;

#ifdef FMOD_SUPPORT_SENTENCING
                    if (eos && mSound->mSubSoundList)
                    {
                        endflag = FMOD_RESAMPLER_END_SUBSOUND;
                    }
#endif
				}
            }
            
            /*
                Resample the src buffer into the destination.
            */
            if (origspeed.mHi == 1 && speed.mLo == 0)
            {
                FMOD_Resampler_NoInterp(mBuffer + (readoffset * mSound->mChannels), rlength, readbuffer, mSound->mFormat, &pos, &speed, mSound->mChannels);
            }
            else
            {           
                switch (mSystem->mResampleMethod)
                {
                    case FMOD_DSP_RESAMPLER_NOINTERP:
                    {
                        FMOD_Resampler_NoInterp(mBuffer + (readoffset * mSound->mChannels), rlength, readbuffer, mSound->mFormat, &pos, &speed, mSound->mChannels);
                        break;
                    }
                    case FMOD_DSP_RESAMPLER_LINEAR:
                    {
                        FMOD_Resampler_Linear(mBuffer + (readoffset * mSound->mChannels), rlength, readbuffer, mSound->mFormat, &pos, &speed, mSound->mChannels);
                        break;
                    }
#ifdef FMOD_SUPPORT_RESAMPLER_CUBIC
                    case FMOD_DSP_RESAMPLER_CUBIC:
                    {
                        FMOD_Resampler_Cubic(mBuffer + (readoffset * mSound->mChannels), rlength, readbuffer, mSound->mFormat, &pos, &speed, mSound->mChannels);
                        break;
                    }
#endif
#ifdef FMOD_SUPPORT_RESAMPLER_SPLINE
                    case FMOD_DSP_RESAMPLER_SPLINE:
                    {
                        FMOD_Resampler_Spline(mBuffer + (readoffset * mSound->mChannels), rlength, readbuffer, mSound->mFormat, &pos, &speed, mSound->mChannels);
                        break;
                    }
#endif
                    default:
                    {
                        FMOD_Resampler_Linear(mBuffer + (readoffset * mSound->mChannels), rlength, readbuffer, mSound->mFormat, &pos, &speed, mSound->mChannels);
                        break;
                    }
                }
            }

            pos.mHi += soundoffset; /* Put it back into 'sentence space' */
            mPosition = pos;

            outlength -= rlength;
            readoffset += rlength;

            if (endflag == FMOD_RESAMPLER_END_SOUND)    /* If a sentence, this will skip to FMOD_RESAMPLER_END_SUBSOUND instead. */
            {
                if (mChannel->mMode & FMOD_LOOP_BIDI && mChannel->mLoopCount)
                {
                    if ((int)mPosition.mHi < 0)
                    {
                        mPosition.mHi = 0;
                    }
            
                    if (mDirection == DSPWAVETABLE_SPEEDDIR_FORWARDS)
                    {
                        mDirection = DSPWAVETABLE_SPEEDDIR_BACKWARDS;
                    }
                    else
                    {
                        mDirection = DSPWAVETABLE_SPEEDDIR_FORWARDS;
                    }
                }
                else if (mChannel->mMode & FMOD_LOOP_NORMAL && mChannel->mLoopCount)
                {
                    if (mDirection == DSPWAVETABLE_SPEEDDIR_BACKWARDS)
                    {
                        mPosition.mHi += mChannel->mLoopLength;

                        /*
                            Safety check.  Sometimes the speed is so high it might go past double the length of the loop.
                        */
                        while (mPosition.mHi < mChannel->mLoopStart)
                        {
                            mPosition.mHi += mChannel->mLoopLength;
                        }

                        if (mChannel->mLoopCount > 0)
                        {
                            mChannel->mLoopCount --;
                        }
                    }
                    else
                    {
                        if (mPosition.mHi >= mChannel->mLoopLength)
                        {
                            mPosition.mHi -= mChannel->mLoopLength;
                        }
                        else
                        {
                            mPosition.mHi = 0;
                        }

                        /*
                            Safety check.  Sometimes the speed is so high it might go past double the length of the loop.
                        */
                        while (mPosition.mHi >= mChannel->mLoopStart + mChannel->mLoopLength)
                        {
                            mPosition.mHi -= mChannel->mLoopLength;
                        }

                        if (mChannel->mLoopCount > 0)
                        {
                            mChannel->mLoopCount --;
                        }
                    }
                }
                else
                {
                    mPosition.mHi = samplesoftware->mLength;
                    mPosition.mLo = 0;

                    FMOD_memset(mBuffer + (readoffset * mSound->mChannels), 0, outlength * mSound->mChannels * sizeof(float));
               
                    mDSPFinishTick = tick;
                    mFlags |= FMOD_DSP_FLAG_FINISHED;
                    break;
                }
            }
#ifdef FMOD_SUPPORT_SENTENCING
            else if (endflag == FMOD_RESAMPLER_END_SUBSOUND)
            {
                if (mDirection == DSPWAVETABLE_SPEEDDIR_FORWARDS)
                {
                    mChannel->mSubSoundListCurrent++;
                    if (mChannel->mSubSoundListCurrent >= mSound->mSubSoundListNum)
                    {
                        if (!(mSound->mMode & FMOD_LOOP_NORMAL && mChannel->mLoopCount))
                        {
                            mPosition.mHi = samplesoftware->mLength;
                            mPosition.mLo = 0;

                            FMOD_memset(mBuffer + (readoffset * mSound->mChannels), 0, outlength * mSound->mChannels * sizeof(float));
                       
                            mDSPFinishTick = tick;
                            mFlags |= FMOD_DSP_FLAG_FINISHED;
                            break;
                        }
                        else
                        {
                            mChannel->mSubSoundListCurrent = 0;
                            mPosition.mHi -= mChannel->mLoopLength;
                        }
                    }

                    /* Re-select the right soundoffset. */
                    {                    
                       int count;
           
                       soundoffset = 0;
                       count = 0;
                       samplesoftware = SAFE_CAST(SampleSoftware, mSound->mSubSound[mSound->mSubSoundList[count].mIndex]);
           
                       while (count < mChannel->mSubSoundListCurrent)
                       {
                           if (samplesoftware)
                           {
                               soundoffset += samplesoftware->mLength;
                           }
                           count++;
                           samplesoftware = SAFE_CAST(SampleSoftware, mSound->mSubSound[mSound->mSubSoundList[count].mIndex]);
                       }
                   }                                       
                }
                else
                {
                    mChannel->mSubSoundListCurrent--;
                }

                samplesoftware = SAFE_CAST(SampleSoftware, mSound->mSubSound[mSound->mSubSoundList[mChannel->mSubSoundListCurrent].mIndex]);
                readbuffer     = samplesoftware->mBuffer;
                soundlength    = samplesoftware->mLength;

                if (mDirection == DSPWAVETABLE_SPEEDDIR_BACKWARDS)
                {
                    soundoffset -= samplesoftware->mLength;
                    mPosition.mHi--;    /* Position will be '0' of the previous sample, wind it back one more to make it length - 1. */
                }
            }
#endif
        }

        if (mSystem->mFlags & FMOD_INIT_ENABLE_PROFILE)
        {
            FMOD_OS_Time_GetNs(&endtime);
            mCPUUsage = endtime - starttime;

#if defined(FMOD_SUPPORT_PROFILE_DSP_VOLUMELEVELS) && !defined(PLATFORM_PS3)
            calculatePeaks(mBuffer, *length, mSound->mChannels);
#endif
        }
    }

    *outbuffer   = mBuffer;
    *outchannels = mSound->mChannels;

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
FMOD_RESULT DSPWaveTable::setPositionInternal(unsigned int position)
{
    if (!mSound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (position >= mSound->mLength)
    {
        position = mSound->mLength;
    }

    mNewPosition = position;

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
FMOD_RESULT DSPWaveTable::setParameterInternal(int index, float value)
{
    return FMOD_ERR_INVALID_PARAM;
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
FMOD_RESULT DSPWaveTable::getParameterInternal(int index, float *value, char *valuestr)
{   
    return FMOD_ERR_INVALID_PARAM;
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
// OFFSET: 0x0092A1E0
FMOD_RESULT DSPWaveTable::setFrequency(float frequency)
{
    if (frequency < 0)
    {
        mDirection = DSPWAVETABLE_SPEEDDIR_BACKWARDS;
        frequency = -frequency;
    }
    else 
    {
        SampleSoftware  *samplesoftware = (SampleSoftware *)mSound;

        if (samplesoftware && !(mChannel->mMode & FMOD_LOOP_BIDI))
        {
            mDirection = DSPWAVETABLE_SPEEDDIR_FORWARDS;
        }
    }

    mFrequency = frequency;

#ifdef PLATFORM_PSP
    /*
        Avoid casting to 64 bit, it is software emulated
        and very slow on PSP
    */
    float value = mFrequency / mTargetFrequency;

    mSpeed.mHi = (signed int)value;
    mSpeed.mLo = (unsigned int)((value - (signed int)value) * 4294967296.0f);
#else
    mSpeed.mValue = (FMOD_SINT64)(mFrequency / mTargetFrequency * 4294967296.0f);
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
FMOD_RESULT DSPWaveTable::getFrequency(float *frequency)
{
    if (!frequency)
    {
        return FMOD_ERR_INVALID_PARAM;
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
FMOD_RESULT DSPWaveTable::setFinished(bool finished, bool force)
{
    if (!finished)
    {
        mDSPFinishTick = 0xFFFFFFFF;
        mFlags &= ~FMOD_DSP_FLAG_FINISHED;
        return FMOD_OK;
    }

    if (force)
    {
        #ifndef PLATFORM_PS3_SPU
        FMOD_OS_CriticalSection_Enter(mSystem->mDSPCrit);         /* because it came from virtual voice swap most likely and needs to block/sync. */
        #endif

        mDSPFinishTick = 0;

        #ifndef PLATFORM_PS3_SPU
        FMOD_OS_CriticalSection_Leave(mSystem->mDSPCrit);
        #endif
    }

    if (mDSPSoundCard)
    {
        mDSPFinishTick = mDSPSoundCard->mDSPTick + 1;
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
FMOD_RESULT DSPWaveTable::getFinished(bool *finished) 
{ 
    if (!mDSPSoundCard)
    {
        *finished = true;
        return FMOD_OK;
    }
    
    if (mDSPFinishTick >= mDSPSoundCard->mDSPTick && !(mFlags &FMOD_DSP_FLAG_FINISHED))
    {
        *finished = false;  /* hasn't really finished yet, still mixing. */
    }
    else
    {
        *finished = true;
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
// OFFSET: 0x0092A250
FMOD_RESULT F_CALLBACK DSPWaveTable::setPositionCallback(FMOD_DSP_STATE *dsp, unsigned int position)
{
    DSPWaveTable *wavetable = (DSPWaveTable *)dsp;

    return wavetable->setPositionInternal(position);
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
// OFFSET: 0x0092A2A0
FMOD_RESULT F_CALLBACK DSPWaveTable::setParameterCallback(FMOD_DSP_STATE *dsp, int index, float value)
{
    DSPWaveTable *wavetable = (DSPWaveTable *)dsp;

    return wavetable->setParameterInternal(index, value);
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
// OFFSET: 0x0092A2B0
FMOD_RESULT F_CALLBACK DSPWaveTable::getParameterCallback(FMOD_DSP_STATE *dsp, int index, float *value, char *valuestr)
{
    DSPWaveTable *wavetable = (DSPWaveTable *)dsp;

    return wavetable->getParameterInternal(index, value, valuestr);
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
FMOD_RESULT F_CALLBACK DSPWaveTable::resetCallback(FMOD_DSP_STATE *dsp)
{
    DSPWaveTable *wavetable = (DSPWaveTable *)dsp;

    return wavetable->resetInternal();
}

}

#endif
