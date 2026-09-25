#include "fmod_settings.h"

#include "fmod_dspi.h"
#include "fmod_dsp_filter.h"
#include "fmod_dsp_resampler.h"
#include "fmod_dsp_soundcard.h"
#include "fmod_localcriticalsection.h"
#include "fmod_memory.h"
#include "fmod_soundi.h"
#include "fmod_string.h"
#include "fmod_3d.h"

#if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE) 
#include "../ps3/src/fmod_common_spu.h"
#include "fmod_dsp_sfxreverb.h"
#endif

#ifdef PLATFORM_PS3_SPU
    #include <cell/dma.h>
    #include "fmod_systemi_spu.h"
    #include "fmod_spu_printf.h"
#else
    #include "fmod_systemi.h"
#endif

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
// OFFSET: 0x008DC390
FMOD_RESULT DSPI::doesUnitExist(DSPI *target, bool protect)
{
#ifndef PLATFORM_PS3_SPU
    FMOD_RESULT result;
    int count;
    int numinputs;

    if (this == target)
    {
        return FMOD_OK;
    }

    result = getNumInputs(&numinputs, protect);
    if (result != FMOD_OK)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    for (count=0; count < numinputs; count++)
    {
        DSPConnectionI *connection;

        result = getInput(count, 0, &connection, protect);
        if (result != FMOD_OK)
        {
            return result;
        }        

        result = connection->mInputUnit->doesUnitExist(target, protect);
        if (result == FMOD_OK)
        {
            return result;
        }
    }
#endif
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
// OFFSET: 0x008DA410
FMOD_RESULT DSPI::alloc(FMOD_DSP_DESCRIPTION_EX *description)
{
#ifndef PLATFORM_PS3_SPU
    FMOD_RESULT result;
    int inputchannels,rate;

    if (!description)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (description->channels < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = mSystem->getSoftwareFormat(&rate, 0, 0, &inputchannels, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    /*
        Do various error checking based on the type of unit being created.
    */
    switch (description->mCategory)
    {
        case FMOD_DSP_CATEGORY_FILTER:
        {
            if (description->channels > inputchannels)
            {
                return FMOD_ERR_TOOMANYCHANNELS;
            }
            break;
        }
        case FMOD_DSP_CATEGORY_DSPCODECMPEG:
        case FMOD_DSP_CATEGORY_DSPCODECRAW:
        case FMOD_DSP_CATEGORY_DSPCODECXMA:
        case FMOD_DSP_CATEGORY_DSPCODECCELT:
        case FMOD_DSP_CATEGORY_DSPCODECADPCM:
        {
            if (!description->channels)
            {
                return FMOD_ERR_INVALID_PARAM;
            }
            break;
        }
        case FMOD_DSP_CATEGORY_RESAMPLER:
        {
            if (description->channels)
            {
                return FMOD_ERR_INVALID_PARAM;
            }
            break;
        }
        case FMOD_DSP_CATEGORY_WAVETABLE:
        {
            break;
        }
        case FMOD_DSP_CATEGORY_SOUNDCARD:
        {
            break;
        }
        default:
        {
            return FMOD_ERR_INVALID_PARAM;
        }
    }

    FMOD_memcpy(&mDescription, description, sizeof(FMOD_DSP_DESCRIPTION_EX));

    mDSPTick = 0;

    mFlags &= ~FMOD_DSP_FLAG_ACTIVE;
    mFlags &= ~FMOD_DSP_FLAG_USEDADDDSP;
	speakermask = 0xFFFF;

    #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
    mFlagsMramAddress = (unsigned int)&mFlags;
    #endif

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
// OFFSET: 0x008DBEF0
FMOD_RESULT DSPI::getInput(int index, DSPI **inputdsp, DSPConnectionI **input, bool protect)
{
#ifdef FMOD_SUPPORT_SOFTWARE
#ifndef PLATFORM_PS3_SPU
    LocalCriticalSection crit(mSystem->mDSPConnectionCrit);

    if (protect)
    {
        mSystem->flushDSPConnectionRequests();

        crit.enter();
    }

    int count;
    LinkedListNode *current;
    DSPConnectionI *connection;
    
    if (index >= mNumInputs)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    current = mInputHead.getNext();
    if (current == &mInputHead)
    {
        return FMOD_ERR_INTERNAL;
    }

    count = 0;
    while (count < index)
    {
        current = current->getNext();
        count++;
    }
    connection = (DSPConnectionI *)current->getData();
    
    if (input)
    {
        *input = connection;
    }
    if (inputdsp)
    {
        *inputdsp = connection->mInputUnit;
    }

    if (protect)
    {
        crit.leave();
    }
#endif

    return FMOD_OK;
#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
// OFFSET: 0x008DBF90
FMOD_RESULT DSPI::getOutput(int index, DSPI **outputdsp, DSPConnectionI **output, bool protect)
{
#ifdef FMOD_SUPPORT_SOFTWARE
#ifndef PLATFORM_PS3_SPU
    LocalCriticalSection crit(mSystem->mDSPConnectionCrit);

    if (protect)
    {
        mSystem->flushDSPConnectionRequests();

        crit.enter();
    }

    int count;
    LinkedListNode *current;
    DSPConnectionI *connection;

    if (index >= mNumOutputs)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    current = mOutputHead.getNext();
    if (current == &mOutputHead)
    {
        return FMOD_ERR_INTERNAL;
    }

    count = 0;
    while (count < index)
    {
        current = current->getNext();
        count++;
    }
    connection = (DSPConnectionI *)current->getData();
    
    if (output)
    {
        *output = connection;
    }
    if (outputdsp)
    {
        *outputdsp = connection->mOutputUnit;
    }

    if (protect)
    {
        crit.leave();
    }
#endif
    return FMOD_OK;
#else
    return FMOD_ERR_NEEDSSOFTWARE;
#endif
}


#ifndef FMOD_SUPPORT_MIXER_NONRECURSIVE

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPI::read(float **outbuffer, int *outchannels, unsigned int *length, FMOD_SPEAKERMODE speakermode, int speakermodechannels, unsigned int tick)
{
    return FMOD_ERR_INTERNAL;
}

#endif

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPI::read(void  *outbuffer, unsigned int *length, FMOD_SPEAKERMODE speakermode, int speakermodechannels, unsigned int tick)
{
    return read(0, 0, length, speakermode, speakermodechannels, tick);
}


#ifdef FMOD_SUPPORT_MIXER_NONRECURSIVE

#ifdef PLATFORM_WINDOWS_PS3MODE

/*
[
	[DESCRIPTION]

	[PARAMETERS]
  
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPI::stepBack(LinkedListNode *&current, DSPConnectionI *&connection, DSPI *&t, LinkedListNode *&next, bool dspionly)
{
    current = t->mPrevious;                             // Reading from LS, making current point to mainRAM address.
    connection = (DSPConnectionI *)current->getData();   // Point to LS           
    t = connection->mOutputUnit;                        // Point to LS
    next = connection->mInputNode.getNext();            // Next points to MainRAM

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
FMOD_RESULT DSPI::stepForwards(LinkedListNode *&current, DSPConnectionI *&connection, DSPConnectionI *&prevconnection, DSPI *&t, LinkedListNode *&next)
{
    connection = (DSPConnectionI *)current->getData();           // Point to LS
    t = connection->mInputUnit;                                 // Point to LS
    next = connection->mInputNode.getNext();                    // next points to MainRAM
    t->mPrevious = current;                                     // LS variable pointing to MainRAM address.
    prevconnection = connection;
    current = t->mInputHead.getNext();                          // Point to MainRAM

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
FMOD_RESULT DSPI::updateHistory(DSPI *t, float *buffer, int length, int channels)
{
    float *srcptr, *destptr;
    int len;

    destptr = t->mHistoryBuffer;
    srcptr = (float *)buffer;

    len = length;
    while (len)
    { 
        int size = len;
        if (t->mHistoryPosition + size > FMOD_HISTORYBUFFERLEN)
        {
            size = FMOD_HISTORYBUFFERLEN - t->mHistoryPosition;
        }

        FMOD_memcpy(destptr + (t->mHistoryPosition * channels), srcptr, size * channels * sizeof(float));

        len -= size;
        srcptr += (size * channels);
    
        t->mHistoryPosition += size;
        if (t->mHistoryPosition >= FMOD_HISTORYBUFFERLEN)
        {
            t->mHistoryPosition = 0;
        }
    }

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
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPI::run(float **outbuffer, int *outchannels, unsigned int *length, FMOD_SPEAKERMODE speakermode, int speakermodechannels, unsigned int tick)
{
#ifdef FMOD_SUPPORT_SOFTWARE
    FMOD_RESULT result = FMOD_OK;
    DSPI *t = this;
    LinkedListNode *current   = mInputHead.getNext();
    DSPConnectionI *connection = 0, *prevconnection = 0;

#ifdef PLATFORM_PS3_SPU
    unsigned int starttime, endtime;

    FMOD_memcpy((void *)gDMAMemoryDSP, (void *)gDMAMemorySoundCard, t->mDescription.mSize);
    t = (DSPI *)gDMAMemoryDSP;

    *outbuffer = (float *)gTargetMixBuffer;
#else
    if (mBuffer)
    {
        *outbuffer = mBuffer;
    }
#endif

    t->mFlags |= FMOD_DSP_FLAG_FIRSTMIX;

    while (current != mInputHeadAddress)
    {
        /*
            ==============================================================================================
            Stepping forwards through DSP tree
            ==============================================================================================
        */
#ifdef PLATFORM_PS3_SPU
        if (gSystemFlags & FMOD_INIT_ENABLE_PROFILE)
        {
            FMOD_PS3_SPU_DSP_timer_start(&starttime);
        }
#endif

        if (current != t->mInputHeadAddress)
        {
            bool goback = false;
            LinkedListNode *next;

            if (t->mDSPTick == tick)
            {
                goback = true;
            }
            else if (t->mDescription.mCategory == FMOD_DSP_CATEGORY_RESAMPLER || 
                     t->mDescription.mCategory == FMOD_DSP_CATEGORY_DSPCODECMPEG ||
                     t->mDescription.mCategory == FMOD_DSP_CATEGORY_DSPCODECRAW ||
                     t->mDescription.mCategory == FMOD_DSP_CATEGORY_DSPCODECADPCM)
            {
#if defined(PLATFORM_PS3_SPU)
                FMOD_SPU_PROFILE_START(gProfileResults[FMOD_SPU_PROFILE_LAZYDMA1]);

                cellDmaLargeGet((void *)(gDMAMemoryDSP + gDSPISize), t->mMramAddress + gDSPISize, t->mDescription.mSize - gDSPISize, TAG1, TID, RID);
                cellDmaWaitTagStatusAll(MASK1);
                FMOD_PS3_SPU_DSPPointersToLS(t, false);

                t = (DSPI *)gDMAMemoryDSP;

                FMOD_SPU_PROFILE_STOP(gProfileResults[FMOD_SPU_PROFILE_LAZYDMA1]);
#endif
                DSPResampler *resampler = SAFE_CAST(DSPResampler, t);
                if (!resampler->mFill)
                {
                    goback = true;
                }
            }

            if (goback)
            {
                prevconnection = 0;
                current = t->mInputHeadAddress;
                t->mLastChannels = *outchannels;
                continue;
            }

#ifdef PLATFORM_PS3_SPU
            if (gSystemFlags & FMOD_INIT_ENABLE_PROFILE)
            {
                FMOD_PS3_SPU_DSP_timer_end(starttime, &t->mCPUUsageTemp);
            }
#endif

            t->mBufferChannels = speakermodechannels;

            stepForwards(current, connection, prevconnection, t, next);

            t->mFlags |= FMOD_DSP_FLAG_IDLE;

            if (mDSPTick != tick)
            {
                /*
                    Reset CPU usage of dsp node if it is the first time
                    it has been stepped into.
                */
                t->mCPUUsage = t->mCPUUsageTemp;
                t->mCPUUsageTemp = 0;
            }

            connection = 0;

            if (!(t->mFlags & FMOD_DSP_FLAG_ACTIVE) || t->mFlags & (FMOD_DSP_FLAG_FINISHED | FMOD_DSP_FLAG_QUEUEDFORDISCONNECT))
            {
                t->mDSPTick = tick;

#ifdef PLATFORM_PS3_SPU
                cellDmaPutUint32(t->mFlags, t->mFlagsMramAddress, TAG1, TID, RID);
#endif

                stepBack(current, connection, t, next, true);

                if (t->mLastChannels > 0)
                {
                    *outchannels = t->mLastChannels;
                }
                else
                {
                    *outchannels = speakermodechannels;
                }

                /*
                    Clear the buffer if first input was inactive
                */
                if (t->mBuffer && (t->mFlags & FMOD_DSP_FLAG_FIRSTMIX))
                {
                    t->mFlags &= ~FMOD_DSP_FLAG_FIRSTMIX;
#ifdef PLATFORM_PS3_SPU
                    *outbuffer = (float *)gTargetMixBuffer;
            
                    FMOD_memset(*outbuffer, 0, *length * *outchannels * sizeof(float));

                    cellDmaPut((void *)*outbuffer, (uint64_t)t->mBuffer, *length * *outchannels * sizeof(float), TAG1, TID, RID);
                    cellDmaWaitTagStatusAll(MASK1);
#else
                    FMOD_memset(t->mBuffer, 0, *length * *outchannels * sizeof(float));
                    *outbuffer = t->mBuffer;
#endif
                }
                current = next;
                continue;
            }            

            t->mFlags |= FMOD_DSP_FLAG_FIRSTMIX;

#ifdef PLATFORM_PS3_SPU
            cellDmaPutUint32(t->mFlags, t->mFlagsMramAddress, TAG1, TID, RID);
#endif
        }
        else
        {
            /*
                ==============================================================================================
                Stepping backwards through DSP tree
                ==============================================================================================
            */
            LinkedListNode *next;
            bool wholedspdma = false;

#ifdef PLATFORM_PS3_SPU
            if (gSystemFlags & FMOD_INIT_ENABLE_PROFILE)
            {
                FMOD_PS3_SPU_DSP_timer_start(&starttime);
            }
#endif
            if (t->mDSPTick == tick)
            {
#ifdef PLATFORM_PS3_SPU
                if (t->mBuffer)
                {
                    cellDmaGet((void *)gTargetMixBuffer, (uint64_t)t->mBuffer, MIXBUFFERSIZE, TAG1, TID, RID);
                    cellDmaWaitTagStatusAll(MASK1);
                }

                *outbuffer = (float *)gTargetMixBuffer;
#else
                if (t->mBuffer)
                {
                    *outbuffer = t->mBuffer;
                }
#endif
                *outchannels = t->mBufferChannels;
                t->mFlags &= ~FMOD_DSP_FLAG_IDLE;
            }
            else
            {
                if (t->mDescription.mCategory == FMOD_DSP_CATEGORY_RESAMPLER || 
                    t->mDescription.mCategory == FMOD_DSP_CATEGORY_DSPCODECMPEG ||
                    t->mDescription.mCategory == FMOD_DSP_CATEGORY_DSPCODECRAW ||
                    t->mDescription.mCategory == FMOD_DSP_CATEGORY_DSPCODECADPCM)
                {
#if defined(PLATFORM_PS3_SPU)
                    FMOD_SPU_PROFILE_START(gProfileResults[FMOD_SPU_PROFILE_LAZYDMA2]);

                    /*
                        Need to DMA in the whole unit
                    */
                    cellDmaLargeGet((void *)(gDMAMemoryDSP + gDSPISize), t->mMramAddress + gDSPISize, t->mDescription.mSize - gDSPISize, TAG1, TID, RID);
                    cellDmaWaitTagStatusAll(MASK1);

                    FMOD_PS3_SPU_DSPPointersToLS(t, false);

                    t = (DSPI *)gDMAMemoryDSP;

                    FMOD_SPU_PROFILE_STOP(gProfileResults[FMOD_SPU_PROFILE_LAZYDMA2]);

                    wholedspdma = true;
#endif
                    DSPResampler *resampler = SAFE_CAST(DSPResampler, t);

                    FMOD_SPU_PROFILE_START(gProfileResults[FMOD_SPU_PROFILE_RESAMPLE]);

                    result = resampler->update(*length, outchannels, (void **)outbuffer, tick);

                    FMOD_SPU_PROFILE_STOP(gProfileResults[FMOD_SPU_PROFILE_RESAMPLE]);

                    t->mBufferChannels = *outchannels;

                    if (result == FMOD_ERR_FILE_EOF)
                    {              
#if defined(PLATFORM_PS3_SPU)
                        FMOD_PS3_SPU_DSPPointersToMRAM(t, false);
                        cellDmaLargePut((void *)gDMAMemoryDSP, t->mMramAddress, t->mDescription.mSize, TAG1, TID, RID);
                        cellDmaWaitTagStatusAll(MASK1);
#endif
                        current = t->mInputHead.getNext();  /* Make it start again on the subtree */

#ifdef PLATFORM_PS3_SPU
                        if (gSystemFlags & FMOD_INIT_ENABLE_PROFILE)
                        {
                            FMOD_PS3_SPU_DSP_timer_end(starttime, &t->mCPUUsageTemp);
                        }
#endif
                        continue;
                    }
                    else
                    {
                        if (gSystemFlags & FMOD_INIT_ENABLE_PROFILE)
                        {
                            #if defined(FMOD_SUPPORT_PROFILE_DSP_VOLUMELEVELS)          
                            if (t->mDescription.mCategory != FMOD_DSP_CATEGORY_SOUNDCARD)
                            {
                                calculatePeaks(*outbuffer, *length, *outchannels, t);
                            }
                            #endif
                        }
                    }

#ifdef PLATFORM_PS3_SPU
                    if (t->mBuffer)
                    {
                        FMOD_SPU_PROFILE_START(gProfileResults[FMOD_SPU_PROFILE_DMA3]);

                        cellDmaPut((void *)*outbuffer, (uint64_t)t->mBuffer, *length * *outchannels * sizeof(float), TAG1, TID, RID);
                        cellDmaWaitTagStatusAll(MASK1);

                        FMOD_SPU_PROFILE_STOP(gProfileResults[FMOD_SPU_PROFILE_DMA3]);
                    }
#endif
                }

                /*
                    Call the plugin callback to actually process or generate the data for this unit.

                    mSystem->mDSPTempBuff actually just points to gSourceMixBuffer
                */
                if (t->mDescription.read && !(t->mFlags & FMOD_DSP_FLAG_BYPASS) && t->mDescription.mCategory != FMOD_DSP_CATEGORY_DSPCODECMPEG && t->mDescription.mCategory != FMOD_DSP_CATEGORY_DSPCODECADPCM && t->mDescription.mCategory != FMOD_DSP_CATEGORY_DSPCODECRAW)
                {
                    if (t->mDescription.channels)
                    {
                        *outchannels = t->mDescription.channels;
                        FMOD_memset(mSystem->mDSPTempBuff, 0, *length * t->mDescription.channels * sizeof(float));
                    }
                    else if (!t->mNumInputs)
                    {
                        *outbuffer = mSystem->mDSPTempBuff; /* mSystem->mDSPTempBuff actually just points to gSourceMixBuffer */
                        FMOD_memset(*outbuffer, 0, *length * *outchannels * sizeof(float));
                    }

#if defined(PLATFORM_PS3_SPU)
                    FMOD_SPU_PROFILE_START(gProfileResults[FMOD_SPU_PROFILE_LAZYDMA3]);
                    cellDmaLargeGet((void *)(gDMAMemoryDSP + gDSPISize), t->mMramAddress + gDSPISize, t->mDescription.mSize - gDSPISize, TAG1, TID, RID);
                    cellDmaWaitTagStatusAll(MASK1);
                    FMOD_SPU_PROFILE_STOP(gProfileResults[FMOD_SPU_PROFILE_LAZYDMA3]);

                    t = (DSPI *)gDMAMemoryDSP;
                    FMOD_PS3_SPU_DSPPointersToLS(t, false);
                    wholedspdma = true;
#endif
                    t->instance = (FMOD_DSP *)t;
                    FMOD_SPU_PROFILE_START(gProfileResults[FMOD_SPU_PROFILE_DSPREAD]);
                    t->mDescription.read((FMOD_DSP_STATE *)t, (float *)*outbuffer, (float *)mSystem->mDSPTempBuff, *length, *outchannels, *outchannels);
                    FMOD_SPU_PROFILE_STOP(gProfileResults[FMOD_SPU_PROFILE_DSPREAD]);

                    *outbuffer = mSystem->mDSPTempBuff;
                    t->mFlags &= ~FMOD_DSP_FLAG_IDLE;                    
                }

                t->mDSPTick = tick;

                t->mLastChannels = *outchannels;

#ifdef PLATFORM_PS3_SPU
                if (gSystemFlags & FMOD_INIT_ENABLE_PROFILE)
                {
                    FMOD_PS3_SPU_DSP_timer_end(starttime, &t->mCPUUsageTemp);
                }
#endif
            }

            if (t->mFlags & FMOD_DSP_FLAG_IDLE)
            {
                t->mDSPTick = tick;

                stepBack(current, connection, t, next, !wholedspdma);
    
                if (t->mLastChannels)
                {
                    *outchannels = t->mLastChannels;
                }
                else
                {
                    *outchannels = speakermodechannels;
                }

                if (t->mFlags & FMOD_DSP_FLAG_FIRSTMIX)
                {
#ifdef PLATFORM_PS3_SPU
                    FMOD_memset((void *)gSourceMixBuffer, 0, *length * *outchannels * sizeof(float));
                    
                    cellDmaPut((void *)gSourceMixBuffer, (uint64_t)t->mBuffer, *length * *outchannels * sizeof(float), TAG1, TID, RID);
                    cellDmaWaitTagStatusAll(MASK1);
                    *outbuffer = (float *)gSourceMixBuffer;
#else
                    FMOD_memset((void *)t->mBuffer, 0, *length * *outchannels * sizeof(float));
#endif
                    t->mFlags &= ~FMOD_DSP_FLAG_FIRSTMIX;
                }
                else
                {
                    /*
                        Re-get the source buffer from this level, otherwise we're just returning the idle buffer.
                    */
#ifdef PLATFORM_PS3_SPU
                    cellDmaGet((void *)gSourceMixBuffer, (uint64_t)t->mBuffer, *length * *outchannels * sizeof(float), TAG1, TID, RID);
                    cellDmaWaitTagStatusAll(MASK1);
                    *outbuffer = (float *)gSourceMixBuffer;
#else
                    *outbuffer = t->mBuffer;
#endif
                }           
            }
            else
            {
                t->mDSPTick = tick;

                stepBack(current, connection, t, next, !wholedspdma);

                t->mFlags &= ~FMOD_DSP_FLAG_IDLE;

#ifdef PLATFORM_PS3_SPU
                if (gSystemFlags & FMOD_INIT_ENABLE_PROFILE)
                {
                    FMOD_PS3_SPU_DSP_timer_start(&starttime);
                }
                /*
                    Bring in the connection levels
                */
                cellDmaGet((void *)gDMAMemoryConnectionLevels, (uint64_t)connection->mMramAddressLevels, connection->mMaxOutputLevels * connection->mMaxInputLevels * 3 * sizeof(DSP_LEVEL_TYPE), TAG1, TID, RID);
                cellDmaWaitTagStatusAll(MASK1);
                FMOD_PS3_SPU_ConnectionLevelsToLS(connection);
#endif

                /*
                    Now that we know how many channels are coming in from the input, does the pan need updating?
                */
                bool passthru = false;
                if (t->mNumInputs <= 1 && connection->mVolume == 1.0f &&
                   ((!connection->mSetLevelsUsed && t->mDescription.mCategory != FMOD_DSP_CATEGORY_SOUNDCARD) || connection->checkUnity(*outchannels, speakermodechannels) == FMOD_OK))
                {
                    passthru = true;
                }

                if (t->mDescription.read && t->mDescription.channels && t->mDescription.channels != *outchannels)
                {
                    passthru = false;
                }

                /*
                    Now either do a 'pass through' if the data is 1:1 and unscaled by volume, or do a mix.
                */
                if (passthru)
                {
                    if (t->mNumOutputs > 1 && t->mDescription.mCategory != FMOD_DSP_CATEGORY_RESAMPLER) /* A multiinput resampler doesnt need its mbuffer stomped on. */
                    {
#ifdef PLATFORM_PS3_SPU
                        if (*outbuffer)
                        {
                            if (*outbuffer != gTargetMixBuffer)
                            {
                                FMOD_memcpy((void *)gTargetMixBuffer, *outbuffer, *length * *outchannels * sizeof(float));
                            }
                        }
                        else
                        {
                            FMOD_memset((void *)gTargetMixBuffer, 0, *length * *outchannels * sizeof(float));
                        }
                        *outbuffer = (float *)gTargetMixBuffer;

                        cellDmaPut((void *)*outbuffer, (uint64_t)t->mBuffer, *length * *outchannels * sizeof(float), TAG1, TID, RID);
                        cellDmaWaitTagStatusAll(MASK1);
#else
                        if (*outbuffer)
                        {
                            FMOD_memcpy(t->mBuffer, (float *)*outbuffer, *length * *outchannels * sizeof(float));
                        }
                        else
                        {
                            FMOD_memset(t->mBuffer, 0, *length * *outchannels * sizeof(float));
                        }

                        *outbuffer = t->mBuffer;        /* Return the already processed buffer */
#endif
                        t->mBufferChannels = *outchannels;
                    }
                    if (connection->mRampCount)
                    {
                        int count, count2;

                        for (count = 0; count < connection->mMaxOutputLevels; count++)
                        {
                            for (count2 = 0; count2 < connection->mMaxInputLevels; count2++)
                            {
                                connection->mLevelCurrent[count][count2] = connection->mLevel[count][count2];
                                connection->mLevelDelta  [count][count2] = 0;
                            }
                        }
                        connection->mRampCount = 0;

#ifdef PLATFORM_PS3_SPU
                        cellDmaPut((void *)gDMAMemoryConnectionLevels, (uint64_t)connection->mMramAddressLevels, connection->mMaxOutputLevels * connection->mMaxInputLevels * 3 * sizeof(DSP_LEVEL_TYPE), TAG1, TID, RID);
                        cellDmaWaitTagStatusAll(MASK1);
#endif
                    }
                }
                else
                {
#ifdef PLATFORM_PS3_SPU
                    float *mixbuffer = (float *)gTargetMixBuffer;

                    if (*outbuffer == gTargetMixBuffer)
                    {
                        FMOD_memcpy((void *)gSourceMixBuffer, (void *)gTargetMixBuffer, *length * *outchannels * sizeof(float));

                        *outbuffer = (float *)gSourceMixBuffer;
                    }
                    if (t->mBuffer)
                    {
                        cellDmaGet((void *)gTargetMixBuffer, (uint64_t)t->mBuffer, *length * speakermodechannels * sizeof(float), TAG1, TID, RID);
                        cellDmaWaitTagStatusAll(MASK1);
                    }
#else               
                    float *mixbuffer = t->mBuffer;                
            
                    if (!mixbuffer)
                    {
                        mixbuffer = mSystem->mDSPTempBuff;
                    }
#endif

                    /*
                        If a new connection happened and no pan has been set, set it here.
                    */
                    if (!connection->mSetLevelsUsed)
                    {
                        if (speakermodechannels == *outchannels  && connection->mVolume == 1.0f)
                        {
                            connection->setUnity();
                        }
                        else
                        {
                            connection->setPan(0.0f, speakermodechannels, *outchannels, speakermode);
                        }
                        connection->mSetLevelsUsed = true;
                    }

                    /*
                        If it is the first input, give it a blank buffer to mix to.
                    */
                    if (t->mFlags & FMOD_DSP_FLAG_FIRSTMIX)
                    {
                        FMOD_memset(mixbuffer, 0, *length * speakermodechannels * sizeof(float));
                        t->mFlags &= ~FMOD_DSP_FLAG_FIRSTMIX;
                    }

                    FMOD_SPU_PROFILE_START(gProfileResults[FMOD_SPU_PROFILE_MIX]);
                                    // out          // in
                    connection->mix(mixbuffer, (float *)*outbuffer, speakermodechannels, *outchannels, *length);

                    FMOD_SPU_PROFILE_STOP(gProfileResults[FMOD_SPU_PROFILE_MIX]);

                    *outbuffer   = mixbuffer;
                    *outchannels = speakermodechannels;
#ifdef PLATFORM_PS3_SPU
                    FMOD_SPU_PROFILE_START(gProfileResults[FMOD_SPU_PROFILE_DMA4]);
                    unsigned int waitmask = 0;
                    if (t->mBuffer)
                    {
                        cellDmaPut((void *)*outbuffer, (uint64_t)t->mBuffer, *length * *outchannels * sizeof(float), TAG1, TID, RID);
                        cellDmaWaitTagStatusAll(MASK1);
                        waitmask |= MASK1;
                    }
                    cellDmaPut((void *)gDMAMemoryConnectionLevels, (uint64_t)connection->mMramAddressLevels, connection->mMaxOutputLevels * connection->mMaxInputLevels * 3 * sizeof(DSP_LEVEL_TYPE), TAG2, TID, RID);
                    waitmask |= MASK2;
                    cellDmaWaitTagStatusAll(waitmask);
                    FMOD_SPU_PROFILE_STOP(gProfileResults[FMOD_SPU_PROFILE_DMA4]);
 #endif
                }

                /*
                    DMA connection and DSP unit back to MRAM
                */
#ifdef PLATFORM_PS3_SPU
                unsigned int waitmask = 0;
                FMOD_SPU_PROFILE_START(gProfileResults[FMOD_SPU_PROFILE_DMAUNITSBACK]);

                FMOD_PS3_SPU_ConnectionLevelsToMRAM(connection);

                if (connection->mMramAddress)
                {
                    cellDmaPut((void *)gDMAMemoryConnection, (uint64_t)connection->mMramAddress, gDSPConnectionSize, TAG1, TID, RID);
                    waitmask |= MASK1;
                }
                FMOD_PS3_SPU_DSPPointersToMRAM(t, true);
                {
                    cellDmaLargePut((void *)gDMAMemoryDSP, (uint64_t)t->mMramAddress, gDSPISize, TAG2, TID, RID);
                    waitmask |= MASK2;
                    cellDmaWaitTagStatusAll(waitmask);
                }
                FMOD_PS3_SPU_DSPPointersToLS(t, true);

                FMOD_SPU_PROFILE_STOP(gProfileResults[FMOD_SPU_PROFILE_DMAUNITSBACK]);

                if (gSystemFlags & FMOD_INIT_ENABLE_PROFILE)
                {
                    FMOD_PS3_SPU_DSP_timer_end(starttime, &t->mCPUUsageTemp);

                    #if defined(FMOD_SUPPORT_PROFILE_DSP_VOLUMELEVELS)          
                    if (t->mDescription.mCategory != FMOD_DSP_CATEGORY_SOUNDCARD)
                    {
                        calculatePeaks(*outbuffer, *length, *outchannels, t);
                    }
                    #endif
                }
#endif
            }

            /*
                If the history buffer option is set, buffer off this data into a separate ring buffer.
            */
            if (t->mHistoryBuffer)
            {
                updateHistory(t, *outbuffer, *length, *outchannels);
            }

            current = next;
        }
    };

    /*
        THIS IS FOR DSP SOUNDCARD UNIT, since its not part of the above loop

        If the history buffer option is set, buffer off this data into a separate ring buffer.
    */
    if (mHistoryBuffer)
    {
        updateHistory(this, *outbuffer, *length, *outchannels);
    }
    
    if (gSystemFlags & FMOD_INIT_ENABLE_PROFILE)
    {
        calculatePeaks(*outbuffer, *length, *outchannels);

        mCPUUsage = mCPUUsageTemp;
        mCPUUsageTemp = 0;

        FMOD_PS3_SPU_DSPPointersToMRAM(this, true);

        cellDmaPut((void *)gDMAMemorySoundCard, (uint64_t)gDSPSoundCardMram, gDSPSoundCardSize, TAG1, TID, RID);
        cellDmaWaitTagStatusAll(1<<TAG1);
    }

    mDSPTick = tick;

#ifdef PLATFORM_PS3_SPU
    cellDmaPutUint32(mDSPTick, gDSPSoundCardDSPTickMram, TAG1, TID, RID);
#endif

    return result;
#else
    return FMOD_ERR_NEEDSSOFTWARE;
#endif
}

#endif


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
// OFFSET: 0x008DA520
FMOD_RESULT DSPI::validate(DSP *dsp, DSPI **dspi)
{
    if (!dspi)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!dsp)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    *dspi = (DSPI *)dsp;

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
// OFFSET: 0x008DC420
FMOD_RESULT DSPI::setPosition(unsigned int position, bool processinputs)
{
    FMOD_RESULT result;

    if (processinputs)
    {
        int count, numinputs = 0;
    
        result = getNumInputs(&numinputs);
        if (result != FMOD_OK)
        {
            return result;
        }
    
        for (count = 0; count < numinputs; count++)
        {
            DSPI *dsp = 0;
        
            getInput(count, &dsp);
        
            dsp->setPosition(position, true);
        }
    }

    if (!mDescription.setposition)
    {
        return FMOD_OK;
    }
    
    instance = (FMOD_DSP *)this;
    
    return mDescription.setposition((FMOD_DSP_STATE *)this, position);
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
// OFFSET: 0x008DA550
FMOD_RESULT DSPI::calculateSpeakerLevels(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright, FMOD_SPEAKERMODE speakermode, int channels, FMOD_SPEAKERMAPTYPE speakermap, float *outlevels, int *numinputlevels)
{
    *numinputlevels = 0;

    switch (speakermode)
    {
        case FMOD_SPEAKERMODE_RAW:
        {
            switch (channels)
            {
                case 1:
                {
                    break;
                }
                case 2:
                {
                    break;
                }
                case 4:
                {
                    break;
                }
                case 6:
                {
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case FMOD_SPEAKERMODE_MONO:
        {
            switch (channels)
            {
                case 1:
                {
                    float levels[1][1] = 
                    {
                        { FMOD_SQRT(frontleft*frontleft + center*center*0.5f + backleft*backleft * 0.25f)    },
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 1;
                    break;
                }
                case 2:
                {
                    float levels[1][2] = 
                    {   
                        { 
                            FMOD_SQRT(frontleft*frontleft + center*center*0.5f + backleft*backleft * 0.25f), FMOD_SQRT(frontright*frontright + center*center*0.5f + backleft*backleft * 0.25f),  
                        },
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 2;
                    break;
                }
                case 4:
                {
                    float levels[1][4] = 
                    {    
                        { frontleft,       0,                backleft*0.5f,    0                },    // FL
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 4;
                    break;
                }
                case 6:
                {
                    float levels[1][6] = 
                    {    
                        { frontleft,   frontright,  center,    lfe,   backleft,    backright }
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 6;
                    break;
                }
                case 8:
                {
                    float levels[1][8] = 
                    {    
                        { frontleft,   frontright,  center,    lfe,   backleft,    backright, sideleft, sideright }
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 8;
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case FMOD_SPEAKERMODE_STEREO:
        case FMOD_SPEAKERMODE_STEREO_LINEAR:
        {
            if (speakermap == FMOD_SPEAKERMAPTYPE_ALLMONO)
            {
                int count;

                FMOD_memset(outlevels, 0, 2 * channels * sizeof(float));

                for (count = 0; count < channels; count++)
                {
                    outlevels[(channels * FMOD_SPEAKER_FRONT_LEFT)  + count] = center * .707f;
                    outlevels[(channels * FMOD_SPEAKER_FRONT_RIGHT) + count] = center * .707f;
                }

                *numinputlevels = channels;
                break;
            }
            else if (speakermap == FMOD_SPEAKERMAPTYPE_ALLSTEREO || channels > 8)
            {
                int count;

                FMOD_memset(outlevels, 0, 2 * channels * sizeof(float));

                for (count = 0; count < channels; count++)
                {
                    if (count & 1)
                    {
                        outlevels[(channels * FMOD_SPEAKER_FRONT_RIGHT) + count] = frontright;
                    }
                    else
                    {
                        outlevels[(channels * FMOD_SPEAKER_FRONT_LEFT) + count] = frontleft;
                    }
                }

                *numinputlevels = channels;
                break;
            }

            switch (channels)
            {
                case 1:
                {
                    float levels[2][1] = 
                    {
                        { FMOD_SQRT(frontleft*frontleft + center*center*0.5f + backleft*backleft * 0.25f)    },
                        { FMOD_SQRT(frontright*frontright + center*center*0.5f + backright*backright * 0.25f)  }
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 1;
                    break;
                }
                case 2:
                {
                    float levels[2][2] = 
                    {   
                        { 
                            FMOD_SQRT(frontleft*frontleft + center*center*0.5f + backleft*backleft * 0.25f), 
                            0  
                        },
                        { 
                            0, 
                            FMOD_SQRT(frontright*frontright + center*center*0.5f + backright*backright * 0.25f)
                        }
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 2;
                    break;
                }
                case 4:
                {
                    float levels[2][4] = 
                    {    
                        { frontleft,       0,                backleft*0.5f,    0                },    // FL
                        { 0,               frontright,       0,                backright*0.5f   },    // FR
                    };
                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 4;
                    break;
                }
                case 6:
                {
                    if (speakermap == FMOD_SPEAKERMAPTYPE_51_PROTOOLS)
                    {
                        float levels[2][6] = 
                        {    /*  FL      C              FR          BL             BR             LFE         */
                            { frontleft, center * 0.5f, 0,          backleft*0.5f, 0,              lfe * 0.5f },    // FL
                            { 0,         center * 0.5f, frontright, 0,             backright*0.5f, lfe * 0.5f },    // FR
                        };
                        FMOD_memcpy(outlevels, levels, sizeof(levels));
                    }
                    else
                    {
                        float levels[2][6] = 
                        {    /*  FL      FR          C              LFE         BL             BR             */
                            { frontleft, 0,          center * 0.5f, lfe * 0.5f, backleft*0.5f, 0              },    // FL
                            { 0,         frontright, center * 0.5f, lfe * 0.5f, 0,             backright*0.5f },    // FR
                        };
                        FMOD_memcpy(outlevels, levels, sizeof(levels));
                    }
                    *numinputlevels = 6;
                    break;
                }
                case 8:
                {
                    float levels[2][8] = 
                    {    /*  FL      FR          C              LFE         BL             BR              SL                SR                */
                        { frontleft, 0,          center * 0.5f, lfe * 0.5f, backleft*0.5f, 0,              sideleft * 0.75f, 0                 },  // FL
                        { 0,         frontright, center * 0.5f, lfe * 0.5f, 0,             backright*0.5f, 0,                sideright * 0.75f },  // FR
                    };
                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 8;
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case FMOD_SPEAKERMODE_QUAD:
        {
            if (speakermap == FMOD_SPEAKERMAPTYPE_ALLMONO)
            {
                int count;

                FMOD_memset(outlevels, 0, 4 * channels * sizeof(float));

                for (count = 0; count < channels; count++)
                {
                    outlevels[(channels * FMOD_SPEAKER_FRONT_LEFT)  + count] = center * .707f;
                    outlevels[(channels * FMOD_SPEAKER_FRONT_RIGHT) + count] = center * .707f;
                }

                *numinputlevels = channels;
                break;
            }
            else if (speakermap == FMOD_SPEAKERMAPTYPE_ALLSTEREO || channels > 8)
            {
                int count;

                FMOD_memset(outlevels, 0, 4 * channels * sizeof(float));

                for (count = 0; count < channels; count++)
                {
                    if (count & 1)
                    {
                        outlevels[(channels * FMOD_SPEAKER_FRONT_RIGHT) + count] = frontright;
                    }
                    else
                    {
                        outlevels[(channels * FMOD_SPEAKER_FRONT_LEFT) + count] = frontleft;
                    }
                }

                *numinputlevels = channels;
                break;
            }

            switch (channels)
            {
                case 1:
                {
                    float levels[4][1] = 
                    {
                        { frontleft  + center },    /* FIX THIS - CLIPPING */
                        { frontright + center },    /* FIX THIS - CLIPPING */
                        { backleft            },
                        { backright           }
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 1;
                    break;
                }
                case 2:
                {
                    float levels[4][2] = 
                    {    /*  left        right  */
                        { frontleft,     0          },
                        { 0,             frontright }, 
                        { backleft,      0          },
                        { 0,             backright  }
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 2;
                    break;
                }
                case 4:
                {
                    float levels[4][4] = 
                    {    
                        { frontleft,       0,                0,               0               },    // FL
                        { 0,               frontright,       0,               0               },    // FR
                        { 0,               0,                backleft*0.5f,   0               },    // RL
                        { 0,               0,                0,               backright*0.5f  }     // RR
                    };
                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 4;
                    break;
                }
                case 6:
                {
                    if (speakermap == FMOD_SPEAKERMAPTYPE_51_PROTOOLS)
                    {
                        float levels[4][6] = 
                        {    /*  FL            C                 FR                BL                BR         LFE               */
                            { frontleft,       center * 0.5f,    0,                0,                0,         lfe * 0.25f      },    // FL
                            { 0,               center * 0.5f,    frontright,       0,                0,         lfe * 0.25f      },    // FR
                            { 0,               0,                0,                backleft,         0,         lfe * 0.25f      },    // RL
                            { 0,               0,                0,                0,                backright, lfe * 0.25f      },    // RR
                        };
                        FMOD_memcpy(outlevels, levels, sizeof(levels));
                    }
                    else
                    {
                        float levels[4][6] = 
                        {    /*  FL            FR                C                 LFE               BL                BR       */
                            { frontleft,       0,                center * 0.5f,    lfe * 0.25f,      0,                0        },    // FL
                            { 0,               frontright,       center * 0.5f,    lfe * 0.25f,      0,                0        },    // FR
                            { 0,               0,                0,                lfe * 0.25f,      backleft,         0        },    // RL
                            { 0,               0,                0,                lfe * 0.25f,      0,                backright},    // RR
                        };
                        FMOD_memcpy(outlevels, levels, sizeof(levels));
                    }
                    *numinputlevels = 6;
                    break;
                }
                case 8:
                {
                    float levels[4][8] = 
                    {    /*  FL            FR                C                 LFE               BL                BR               SL                SR              */
                        { frontleft,       0,                center * 0.5f,    lfe * 0.25f,      0,                0,               sideleft * 0.5f,  0               },  // FL
                        { 0,               frontright,       center * 0.5f,    lfe * 0.25f,      0,                0,               0,                sideright * 0.5f},  // FR
                        { 0,               0,                0,                lfe * 0.25f,      backleft,         0,               sideleft * 0.5f,  0               },  // RL
                        { 0,               0,                0,                lfe * 0.25f,      0,                backright,       0,                sideright * 0.5f},  // RR
                    };
                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 8;
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case FMOD_SPEAKERMODE_SURROUND:
        {
            if (speakermap == FMOD_SPEAKERMAPTYPE_ALLMONO)
            {
                int count;

                FMOD_memset(outlevels, 0, 5 * channels * sizeof(float));

                for (count = 0; count < channels; count++)
                {
                    outlevels[(channels * FMOD_SPEAKER_FRONT_CENTER) + count] = center;
                }

                *numinputlevels = channels;
                break;
            }
            else if (speakermap == FMOD_SPEAKERMAPTYPE_ALLSTEREO || channels > 8)
            {
                int count;

                FMOD_memset(outlevels, 0, 5 * channels * sizeof(float));

                for (count = 0; count < channels; count++)
                {
                    if (count & 1)
                    {
                        outlevels[(channels * FMOD_SPEAKER_FRONT_RIGHT) + count] = frontright;
                    }
                    else
                    {
                        outlevels[(channels * FMOD_SPEAKER_FRONT_LEFT) + count] = frontleft;
                    }
                }

                *numinputlevels = channels;
                break;
            }

            switch (channels)
            {
                case 1:
                {
                    float levels[5][1] = 
                    {
                        { frontleft },
                        { frontright },
                        { center },
                        { backleft },
                        { backright }
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 1;
                    break;
                }
                case 2:
                {
                    float levels[5][2] = 
                    {    /*  left          right  */
                        { frontleft,     0 },
                        { 0,             frontright }, 
                        { center * 0.5f, center * 0.5f },  /* This is a mono channel with both left and right going into it, so keep the volume consistent by multiplying by 0.5f. */
                        { backleft,      0 },
                        { 0,             backright },
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 2;
                    break;
                }
                case 4:
                {
                    float levels[5][4] = 
                    {   //   L             R                 C                 S                      
                        { frontleft,       0,                0,                0                },    // FL
                        { 0,               frontright,       0,                0                },    // FR
                        { center * 0.5f,   center * 0.5f,    0,                0                },    // C
                        { 0,               0,                backleft,         0                },    // RL
                        { 0,               0,                0,                backright        }     // RR
                    };
                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 4;
                    break;
                }
                case 6:
                {
                    if (speakermap == FMOD_SPEAKERMAPTYPE_51_PROTOOLS)
                    {
                        float levels[5][6] = 
                        {    /*  FL            C                 FR                BL                BR                LFE              */
                            { frontleft,       0,                0,                0,                0,                0          },    // FL
                            { 0,               0,                frontright,       0,                0,                0          },    // FR
                            { 0,               center,           0,                0,                0,                0          },    // C
                            { 0,               0,                0,                backleft,         0,                0          },    // RL
                            { 0,               0,                0,                0,                backright,        0          }     // RR
                        };
                        FMOD_memcpy(outlevels, levels, sizeof(levels));
                    }
                    else
                    {
                        float levels[5][6] = 
                        {    /*  FL            FR                C                 LFE               BL                BR               */
                            { frontleft,       0,                0,                0,                0,                0          },    // FL
                            { 0,               frontright,       0,                0,                0,                0          },    // FR
                            { 0,               0,                center,           0,                0,                0          },    // C
                            { 0,               0,                0,                0,                backleft,         0          },    // RL
                            { 0,               0,                0,                0,                0,                backright  }     // RR
                        };
                        FMOD_memcpy(outlevels, levels, sizeof(levels));
                    }
                    *numinputlevels = 6;
                    break;
                }
                case 8:
                {
                    float levels[5][8] = 
                    {    /*  FL            FR                C                 LFE               BL                BR               SL                SR              */
                        { frontleft,       0,                0,                0,                0,                0,               sideleft * 0.5f,  0               },  // FL
                        { 0,               frontright,       0,                0,                0,                0,               0,                sideright * 0.5f},  // FR
                        { 0,               0,                center,           0,                0,                0,               0,                0               },  // C
                        { 0,               0,                0,                0,                backleft,         0,               sideleft * 0.5f,  0               },  // RL
                        { 0,               0,                0,                0,                0,                backright,       0,                sideright * 0.5f},  // RR
                    };
                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 8;
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case FMOD_SPEAKERMODE_5POINT1:
        {
            if (speakermap == FMOD_SPEAKERMAPTYPE_ALLMONO)
            {
                int count;

                FMOD_memset(outlevels, 0, 6 * channels * sizeof(float));

                for (count = 0; count < channels; count++)
                {
                    outlevels[(channels * FMOD_SPEAKER_FRONT_CENTER) + count] = center;
                }

                *numinputlevels = channels;
                break;
            }
            else if (speakermap == FMOD_SPEAKERMAPTYPE_ALLSTEREO || channels > 8)
            {
                int count;

                FMOD_memset(outlevels, 0, 6 * channels * sizeof(float));

                for (count = 0; count < channels; count++)
                {
                    if (count & 1)
                    {
                        outlevels[(channels * FMOD_SPEAKER_FRONT_RIGHT) + count] = frontright;
                    }
                    else
                    {
                        outlevels[(channels * FMOD_SPEAKER_FRONT_LEFT) + count] = frontleft;
                    }
                }

                *numinputlevels = channels;
                break;
            }

            switch (channels)
            {
                case 1:
                {
                    float levels[6][1] = 
                    {
                        { frontleft },
                        { frontright },
                        { center },
                        { lfe },
                        { backleft },
                        { backright }
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 1;
                    break;
                }
                case 2:
                {
                    float levels[6][2] = 
                    {    /*  left        right  */
                        { frontleft,     0 },
                        { 0,             frontright }, 
                        { center * 0.5f, center * 0.5f },  /* This is a mono channel with both left and right going into it, so keep the volume consistent by multiplying by 0.5f. */
                        { lfe    * 0.5f, lfe    * 0.5f },  /* This is a mono channel with both left and right going into it, so keep the volume consistent by multiplying by 0.5f. */
                        { backleft,      0 },
                        { 0,             backright },
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 2;
                    break;
                }
                case 4:
                {
                    float levels[6][4] = 
                    {    
                        { frontleft,       0,                0,                0                },    // FL
                        { 0,               frontright,       0,                0                },    // FR
                        { center * 0.5f,   center * 0.5f,    0,                0                },    // C
                        { lfe    * 0.5f,   lfe    * 0.5f,    0,                0                },    // LFE
                        { 0,               0,                backleft,         0                },    // RL
                        { 0,               0,                0,                backright        }     // RR
                    };
                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 4;
                    break;
                }
                case 6:
                {
                    if (speakermap == FMOD_SPEAKERMAPTYPE_51_PROTOOLS)
                    {
                        float levels[6][6] = 
                        {    /*  FL            C                 FR                BL                BR               LFE         */
                            { frontleft,       0,                0,                0,                0,               0           },    // FL
                            { 0,               0,                frontright,       0,                0,               0           },    // FR
                            { 0,               center,           0,                0,                0,               0           },    // C
                            { 0,               0,                0,                0,                0,               lfe         },    // LFE
                            { 0,               0,                0,                backleft,         0,               0           },    // RL
                            { 0,               0,                0,                0,                backright,       0           }     // RR
                        };
                        FMOD_memcpy(outlevels, levels, sizeof(levels));
                    }
                    else
                    {
                        float levels[6][6] = 
                        {    /*  FL            FR                C                 LFE               BL                BR         */
                            { frontleft,       0,                0,                0,                0,                0          },    // FL
                            { 0,               frontright,       0,                0,                0,                0          },    // FR
                            { 0,               0,                center,           0,                0,                0          },    // C
                            { 0,               0,                0,                lfe,              0,                0          },    // LFE
                            { 0,               0,                0,                0,                backleft,         0          },    // RL
                            { 0,               0,                0,                0,                0,                backright  }     // RR
                        };
                        FMOD_memcpy(outlevels, levels, sizeof(levels));
                    }
                    *numinputlevels = 6;
                    break;
                }
                case 8:
                {
                    float levels[6][8] = 
                    {    /*  FL            FR                C                 LFE               BL                BR               SL                SR              */
                        { frontleft,       0,                0,                0,                0,                0,               sideleft * 0.5f,  0               },  // FL
                        { 0,               frontright,       0,                0,                0,                0,               0,                sideright * 0.5f},  // FR
                        { 0,               0,                center,           0,                0,                0,               0,                0               },  // C
                        { 0,               0,                0,                lfe,              0,                0,               0,                0               },  // LFE
                        { 0,               0,                0,                0,                backleft,         0,               sideleft * 0.5f,  0               },  // RL
                        { 0,               0,                0,                0,                0,                backright,       0,                sideright * 0.5f},  // RR
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 8;
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case FMOD_SPEAKERMODE_7POINT1:
        {
            if (speakermap == FMOD_SPEAKERMAPTYPE_ALLMONO)
            {
                int count;

                FMOD_memset(outlevels, 0, 8 * channels * sizeof(float));

                for (count = 0; count < channels; count++)
                {
                    outlevels[(channels * FMOD_SPEAKER_FRONT_CENTER) + count] = center;
                }

                *numinputlevels = channels;
                break;
            }
            else if (speakermap == FMOD_SPEAKERMAPTYPE_ALLSTEREO || channels > 8)
            {
                int count;

                FMOD_memset(outlevels, 0, 8 * channels * sizeof(float));

                for (count = 0; count < channels; count++)
                {
                    if (count & 1)
                    {
                        outlevels[(channels * FMOD_SPEAKER_FRONT_RIGHT) + count] = frontright;
                    }
                    else
                    {
                        outlevels[(channels * FMOD_SPEAKER_FRONT_LEFT) + count] = frontleft;
                    }
                }

                *numinputlevels = channels;
                break;
            }

            switch (channels)
            {
                case 1:
                {
                    float levels[8][1] = 
                    {
                        { frontleft },
                        { frontright },
                        { center },
                        { lfe },
                        { backleft },
                        { backright },
                        { sideleft },
                        { sideright }
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 1;
                    break;
                }
                case 2:
                {
                    float levels[8][2] = 
                    {    /*  left        right  */
                        { frontleft,     0 },
                        { 0,             frontright }, 
                        { center * 0.5f, center * 0.5f },  /* This is a mono channel with both left and right going into it, so keep the volume consistent by multiplying by 0.5f. */
                        { lfe    * 0.5f, lfe    * 0.5f },  /* This is a mono channel with both left and right going into it, so keep the volume consistent by multiplying by 0.5f. */
                        { backleft,      0 },
                        { 0,             backright },
                        { sideleft,      0 },
                        { 0,             sideright }
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 2;
                    break;
                }
                case 4:
                {
                    float levels[8][4] = 
                    {   //  FL            FR                RL                RR              
                        { frontleft,       0,                0,                0                },    // FL
                        { 0,               frontright,       0,                0                },    // FR
                        { center * 0.5f,   center * 0.5f,    0,                0                },    // C
                        { lfe    * 0.5f,   lfe    * 0.5f,    0,                0                },    // LFE
                        { 0,               0,                backleft*0.5f,    0                },    // RL
                        { 0,               0,                0,                backright*0.5f   },    // RR
                        { 0,               0,                sideleft*0.5f,    0                },    // SL
                        { 0,               0,                0,                sideright*0.5f   },    // SR
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 4;
                    break;
                }
                case 6:
                {
                    if (speakermap == FMOD_SPEAKERMAPTYPE_51_PROTOOLS)
                    {                
                        float levels[8][6] = 
                        {    /*  FL            C                 FR                BL                BR               LFE               */
                            { frontleft,       0,                0,                0,                0,               0                 },    // FL
                            { 0,               0,                frontright,       0,                0,               0                 },    // FR
                            { 0,               center,           0,                0,                0,               0                 },    // C
                            { 0,               0,                0,                0,                0,               lfe               },    // LFE
                            { 0,               0,                0,                backleft,         0,               0                 },    // RL
                            { 0,               0,                0,                0,                backright,       0                 },    // RR
                            { sideleft * 0.5f, 0,                0,                sideleft * 0.5f,  0,               0                 },    // SL
                            { 0,               0,                sideright * 0.5f, 0,                sideleft * 0.5f, 0                 },    // SR
                        };

                        FMOD_memcpy(outlevels, levels, sizeof(levels));
                    }
                    else
                    {
                        float levels[8][6] = 
                        {    /*  FL            FR                C                 LFE               BL                BR               */
                            { frontleft,       0,                0,                0,                0,                0                },    // FL
                            { 0,               frontright,       0,                0,                0,                0                },    // FR
                            { 0,               0,                center,           0,                0,                0                },    // C
                            { 0,               0,                0,                lfe,              0,                0                },    // LFE
                            { 0,               0,                0,                0,                backleft,         0                },    // RL
                            { 0,               0,                0,                0,                0,                backright        },    // RR
                            { sideleft * 0.5f, 0,                0,                0,                sideleft * 0.5f,  0                },    // SL
                            { 0,               sideright * 0.5f, 0,                0,                0,                sideleft * 0.5f  },    // SR
                        };

                        FMOD_memcpy(outlevels, levels, sizeof(levels));
                    }
                    *numinputlevels = 6;
                    break;
                }
                case 8:
                {
                    float levels[8][8] = 
                    {    /*  FL            FR                C                 LFE               BL                BR               SL              SR          */
                        { frontleft,       0,                0,                0,                0,                0,               0,              0           },    // FL
                        { 0,               frontright,       0,                0,                0,                0,               0,              0           },    // FR
                        { 0,               0,                center,           0,                0,                0,               0,              0           },    // C
                        { 0,               0,                0,                lfe,              0,                0,               0,              0           },    // LFE
                        { 0,               0,                0,                0,                backleft,         0,               0,              0           },    // RL
                        { 0,               0,                0,                0,                0,                backright,       0,              0           },    // RR
                        { 0,               0,                0,                0,                0,                0,               sideleft,       0           },    // SL
                        { 0,               0,                0,                0,                0,                0,               0,              sideright   },    // SR
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 8;
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case FMOD_SPEAKERMODE_PROLOGIC:
        {
            if (speakermap == FMOD_SPEAKERMAPTYPE_ALLMONO)
            {
                int count;

                FMOD_memset(outlevels, 0, 2 * channels * sizeof(float));

                for (count = 0; count < channels; count++)
                {
                    outlevels[(channels * FMOD_SPEAKER_FRONT_LEFT)  + count] = center * .707f;
                    outlevels[(channels * FMOD_SPEAKER_FRONT_RIGHT) + count] = center * .707f;
                }

                *numinputlevels = channels;
                break;
            }
            else if (speakermap == FMOD_SPEAKERMAPTYPE_ALLSTEREO || channels > 8)
            {
                int count;

                FMOD_memset(outlevels, 0, 2 * channels * sizeof(float));

                for (count = 0; count < channels; count++)
                {
                    if (count & 1)
                    {
                        outlevels[(channels * FMOD_SPEAKER_FRONT_RIGHT) + count] = frontright;
                    }
                    else
                    {
                        outlevels[(channels * FMOD_SPEAKER_FRONT_LEFT) + count] = frontleft;
                    }
                }

                *numinputlevels = channels;
                break;
            }

            /*
                Square the input levels to conform with Pro Logic linear gain  
            */
            float fl2 = frontleft * frontleft;
            float fr2 = frontright * frontright;
            float c2 = center * center;
            float bl2 = backleft * backleft;
            float br2 = backright * backright;

            switch (channels)
            {
                case 1:
                {              
                    float levels[2] = 
                    {
                        fl2 + (c2 *0.707f) + (-0.872f * bl2) + (-0.49f * br2),
                        fr2 + (c2 *0.707f) + (0.49f   * bl2) + (0.872f * br2)
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 1;
                    break;
                }
                case 2:
                {
                    float levels[2][2] = 
                    {
                        { fl2 + (c2 *0.707f) + (-0.872f * bl2), 0   + (c2 *0.707f) + (-0.49f * br2) },
                        { 0   + (c2 *0.707f) + (0.49f   * bl2), fr2 + (c2 *0.707f) + (0.872f * br2) }
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 2;
                    break;
                }
                case 4:
                {
                    float levels[2][4] = 
                    {    
                        { fl2,             0,         0.5f * bl2,           0          },    // FL
                        { 0,               fr2,       0              ,      0.5f * br2 }     // FR
                    };

                    FMOD_memcpy(outlevels, levels, sizeof(levels));
                    *numinputlevels = 4;
                    break;
                }
                case 6:
                {
                    if (speakermap == FMOD_SPEAKERMAPTYPE_51_PROTOOLS)
                    {                
                        float levels[2][6] = 
                        {    /*  FL            C                 FR           BL                BR         LFE          */
                            { fl2,             c2 * 0.5f,        0,           bl2,              0,         lfe * 0.5f  },    // FL
                            { 0,               c2 * 0.5f,        fr2,         0,                br2,       lfe * 0.5f  },    // FR
                        };

                        FMOD_memcpy(outlevels, levels, sizeof(levels));
                    }
                    else
                    {
                        float levels[2][6] = 
                        {    /*  FL            FR           C                 LFE               BL                BR       */
                            { fl2,             0,           c2 * 0.5f,        lfe * 0.5f,       bl2,              0},      // FL
                            { 0,               fr2,         c2 * 0.5f,        lfe * 0.5f,       0,                br2},    // FR
                        };

                        FMOD_memcpy(outlevels, levels, sizeof(levels));
                    }
                    *numinputlevels = 6;
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        default:
        {
            break;
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
// OFFSET: 0x008DC4A0
DSPI::DSPI()
{
    mBuffer             = 0;
    mOutputBuffer       = 0;
    mFlags              = 0;
                        
    mNumInputs          = 0;
    mNumOutputs         = 0;
    mTreeLevel          = -1;
#if defined(FMOD_SUPPORT_MIXER_NONRECURSIVE) || defined(PLATFORM_PS3)
    mLastChannels       = 0;
#endif
                        
    mDefaultVolume      = 1.0f;
    mDefaultFrequency   = DEFAULT_FREQUENCY;
    mDefaultPan         = 0.0f;
    mDefaultPriority    = FMOD_CHANNEL_DEFAULTPRIORITY;

#if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
    mMemory             = 0;
    mInputHeadAddress   = &mInputHead;
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
// OFFSET: 0x008DC540
FMOD_RESULT DSPI::release(bool freethis)
{
#ifdef FMOD_SUPPORT_SOFTWARE
    
    #ifndef PLATFORM_PS3_SPU
    
    FMOD_RESULT result;

    if (mSystem)
    {
        result = mSystem->stopDSP(this);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    mSystem->flushDSPConnectionRequests();  /* If we're about to free DSP memory, dont leave any in queue! */

    result = removeInternal(true);
    if (result != FMOD_OK)
    {
        result = disconnectFromInternal(0, 0, true);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (mOutputBuffer)
    {
        FMOD_Memory_Free(mOutputBuffer);
        mOutputBuffer = 0;
    }

    removeNode();

    if (mDescription.release)
    {
        instance = (FMOD_DSP *)this;

        mDescription.release((FMOD_DSP_STATE *)this);
    }

    #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
    stopBuffering();
    #endif

    #ifdef FMOD_SUPPORT_VSTPLUGIN
    if (mDescription.mType == FMOD_DSP_TYPE_VSTPLUGIN)
    {
        if (!mSystem->mVSTPluginsListHead.isEmpty())
        {
            LinkedListNode *dspvstnode = mSystem->mVSTPluginsListHead.getNext();
            while (dspvstnode != &mSystem->mVSTPluginsListHead)
            {
                DSPI *dspvst = (DSPI *)dspvstnode->getData();

                /*
                    Check if this is the node associated with this dsp unit.
                    If so, then remove it and free it.
                */
                if (dspvst == this)
                {
                    dspvstnode->removeNode();
                    FMOD_Memory_Free(dspvstnode);
                    break;
                }

                dspvstnode = dspvstnode->getNext();
            }
        }
    }
    #endif

    if (freethis)
    {
        #ifdef PLATFORM_PS3
        if (mMemory)
        {
            FMOD_Memory_Free(mMemory);
        }
        else
        #endif
        {
            FMOD_Memory_Free(this);
        }
    }
    
    #endif
    
    return FMOD_OK;

#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
FMOD_RESULT DSPI::getSystemObject(System **system)
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
// OFFSET: 0x008DB720
FMOD_RESULT DSPI::updateTreeLevel(int level)
{
#ifdef FMOD_SUPPORT_SOFTWARE
#ifndef PLATFORM_PS3_SPU
    LinkedListNode *current;

    if (mNumOutputs > 1 && level < mTreeLevel)  /* Don't make it move backwards if it is already connected to something else that is deeper. */
    {
        return FMOD_OK;
    }

    if (level >= FMOD_DSP_MAXTREEDEPTH)
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "DSPI::addInputInteral", "ERROR.  Exceeded maximum DSP tree depth.  Either added too many DSP effects by accident or encountered a recursive add.\n"));
        
        if (mSystem->mCallback)
        {
            mSystem->mCallback((FMOD_SYSTEM *)mSystem, FMOD_SYSTEM_CALLBACKTYPE_BADDSPLEVEL, (FMOD_DSP *)this, 0);
        }

        return FMOD_ERR_DSP_CONNECTION;
    }

    mTreeLevel = level;

    if (!mOutputBuffer || (FMOD_UINT_NATIVE)mBuffer != FMOD_ALIGNPOINTER(mOutputBuffer, 16))
    {
        if (!mSystem->mDSPMixBuff[level])
        {
            mSystem->mDSPMixBuff[level] = (float *)FMOD_Memory_CallocType((mSystem->mDSPBlockSize * (mSystem->mMaxOutputChannels < mSystem->mMaxInputChannels ? mSystem->mMaxInputChannels : mSystem->mMaxOutputChannels) * sizeof(float)) + 16, FMOD_MEMORY_PERSISTENT);
            if (!mSystem->mDSPMixBuff[level])
            {
                return FMOD_ERR_MEMORY;
            }           
        }
    }

    current = mInputHead.getNext();
    while (current != &mInputHead)
    {
        DSPConnectionI *connection = (DSPConnectionI *)current->getData();

        connection->mInputUnit->updateTreeLevel(mTreeLevel + 1);

        current = current->getNext();
    }

    if (!mOutputBuffer || (FMOD_UINT_NATIVE)mBuffer != FMOD_ALIGNPOINTER(mOutputBuffer, 16))
    {
        mBuffer = (float *)FMOD_ALIGNPOINTER(mSystem->mDSPMixBuff[mTreeLevel], 16);
    }

#endif
    return FMOD_OK;
#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
FMOD_RESULT DSPI::updateDSPTick(unsigned int tick)
{
    LinkedListNode *current;

    mDSPTick = tick;

    current = mInputHead.getNext();
    while (current != &mInputHead)
    {
        DSPConnectionI *connection = (DSPConnectionI *)current->getData();

        connection->mInputUnit->updateDSPTick(tick);

        current = current->getNext();
    }

    return FMOD_OK;
}



#ifndef PLATFORM_PS3_SPU
/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPI::createHistoryBuffer(float **ptr, int numchannels)
{
    return mSystem->mHistoryBufferPool.alloc(ptr, numchannels);
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
FMOD_RESULT DSPI::releaseHistoryBuffer(float *ptr)
{
    return mSystem->mHistoryBufferPool.free(ptr);
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

// OFFSET: 0x008DC600
FMOD_RESULT DSPI::addInputInternal(DSPI *target, bool checkcircular, DSPConnectionI *connection, DSPConnectionI **connection_out, bool protect)
{
#ifdef FMOD_SUPPORT_SOFTWARE

#ifndef PLATFORM_PS3_SPU
    FMOD_RESULT             result;
    LocalCriticalSection    crit_dsp(mSystem->mDSPCrit);
    LocalCriticalSection    crit_dspconnect(mSystem->mDSPConnectionCrit);
    bool                    reset = false;

    if (!target)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    /*
        Resamplers cant connect to something that already has an output.
    */
    if (mDescription.mCategory == FMOD_DSP_CATEGORY_RESAMPLER && target->mNumOutputs)
    {
        return FMOD_ERR_DSP_CONNECTION;
    }

    /*
        Generators dont have inputs.
    */
//    if (mDescription.mCategory == FMOD_DSP_CATEGORY_GENERATOR)
//    {
//        return FMOD_ERR_DSP_CONNECTION;
//    }

    /*
        Soundcards cant be connected to.
    */
#if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
    if (target->mTreeLevel == 0)
    {
        return FMOD_ERR_DSP_CONNECTION;
    }
#else
    if (target->mDescription.mCategory == FMOD_DSP_CATEGORY_SOUNDCARD)
    {
        return FMOD_ERR_DSP_CONNECTION;
    }
#endif

    /*
        Dont allow a connection to this target if it is a child of a resampler.
    */

    /*
        Dont allow connections to be circular!
    */
    if (checkcircular)
    {
        result = target->doesUnitExist(this, protect);
        if (result == FMOD_OK)
        {
            FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "DSPI::addInputInteral", "ERROR.  Tried to make a circular connection!.\n"));
            
            if (mSystem->mCallback)
            {
                mSystem->mCallback((FMOD_SYSTEM *)mSystem, FMOD_SYSTEM_CALLBACKTYPE_BADDSPCONNECTION, (FMOD_DSP *)this, (FMOD_DSP *)target);
            }

            return FMOD_ERR_DSP_CONNECTION;
        }
    }

    if (protect)
    {
        crit_dsp.enter();
        crit_dspconnect.enter();
    }
    {
        if (!connection)
        {
            /*
                Create a new connection for this unit.
            */
            result = mSystem->mDSPConnectionPool.alloc(&connection);
            if (result != FMOD_OK)
            {
                return result;
            }

            reset = true;
        }

        connection->mInputNode.addBefore(&mInputHead);
        mNumInputs++;
        connection->mOutputNode.addBefore(&target->mOutputHead);
        target->mNumOutputs++;

        connection->mInputUnit = target;
        connection->mOutputUnit = this;
    #ifdef PLATFORM_PS3
        connection->mInputUnitSize = target->mDescription.mSize;
        connection->mOutputUnitSize = mDescription.mSize;
    #endif

        if (reset)
        {
            connection->reset();
        }

        if (mTreeLevel >= 0)
        {
            target->updateTreeLevel(mTreeLevel + 1);
        }
        if (!mOutputBuffer)
        {
            mBuffer = (float *)FMOD_ALIGNPOINTER(mSystem->mDSPMixBuff[mTreeLevel], 16);
        }

        /*
            Allocate mBuffer for the target if it now has more than 1 input or output
        */
        if (target->mNumOutputs > 1)
        {
            if (!target->mOutputBuffer)
            {
                target->mOutputBuffer = (float *)FMOD_Memory_Calloc((mSystem->mDSPBlockSize * (mSystem->mMaxOutputChannels < mSystem->mMaxInputChannels ? mSystem->mMaxInputChannels : mSystem->mMaxOutputChannels) * sizeof(float)) + 16);
                if (!target->mOutputBuffer)
                {
                    return FMOD_ERR_MEMORY;
                }
            }

            target->mBuffer = (float *)FMOD_ALIGNPOINTER(target->mOutputBuffer, 16);
        }
    }
    if (protect)
    {
        crit_dsp.leave();
        crit_dspconnect.leave();
    }
      
    if (connection_out)
    {
        *connection_out = connection;
    }

#endif
    return FMOD_OK;

#else

    return FMOD_ERR_NEEDSSOFTWARE;

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
// OFFSET: 0x008DCC10
FMOD_RESULT DSPI::addInput(DSPI *target, DSPConnectionI **connection)
{
    if (!mSystem)
    {
        return FMOD_ERR_UNINITIALIZED;
    }

#ifndef PLATFORM_PS3_SPU
    return addInputQueued(target, true, 0, connection);
#else
    return FMOD_OK;
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
// OFFSET: 0x008DC820
FMOD_RESULT DSPI::addInputQueued(DSPI *target, bool checkcircular, DSPConnectionI *connection_old, DSPConnectionI **connection_out)
{
#ifdef FMOD_SUPPORT_SOFTWARE

#ifndef PLATFORM_PS3_SPU
    FMOD_RESULT             result;
    DSPConnectionI         *connection;

    if (!target)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    /*
        Resamplers cant connect to something that already has an output.
    */
    if (mDescription.mCategory == FMOD_DSP_CATEGORY_RESAMPLER && target->mNumOutputs)
    {
        return FMOD_ERR_DSP_CONNECTION;
    }

    /*
        Generators dont have inputs.
    */
//    if (mDescription.mCategory == FMOD_DSP_CATEGORY_GENERATOR)
//    {
//        return FMOD_ERR_DSP_CONNECTION;
//    }

    /*
        Soundcards cant be connected to.
    */
#if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
    if (target->mTreeLevel == 0)
    {
        return FMOD_ERR_DSP_CONNECTION;
    }
#else
    if (target->mDescription.mCategory == FMOD_DSP_CATEGORY_SOUNDCARD)
    {
        return FMOD_ERR_DSP_CONNECTION;
    }
#endif

    /*
        Create a new connection for this unit.
    */
    result = mSystem->mDSPConnectionPool.alloc(&connection);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (connection_old)
    {
        connection->copy(connection_old);
    }
    else
    {    
        connection->reset();
    }

    FMOD_OS_CriticalSection_Enter(mSystem->mDSPConnectionCrit);
    {
        DSPConnectionRequest *request;

        if (mSystem->mConnectionRequestFreeHead.isEmpty())
        {
            mSystem->flushDSPConnectionRequests();
        }

        request = (DSPConnectionRequest *)mSystem->mConnectionRequestFreeHead.getNext();

        request->removeNode();        
        request->addBefore(&mSystem->mConnectionRequestUsedHead);

        request->mThis = this;
        request->mTarget = target;
        request->mConnection = connection;
        if (checkcircular)
        {
            request->mRequest = DSPCONNECTION_REQUEST_ADDINPUT_ERRCHECK;
        }
        else
        {
            request->mRequest = DSPCONNECTION_REQUEST_ADDINPUT;
        }
    }
    FMOD_OS_CriticalSection_Leave(mSystem->mDSPConnectionCrit);

    if (connection_out)
    {
        *connection_out = connection;
    }
#endif
    return FMOD_OK;

#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
// OFFSET: 0x008DC030
FMOD_RESULT DSPI::disconnectFromInternal(DSPI *target, DSPConnectionI *connection, bool protect)
{
#ifdef FMOD_SUPPORT_SOFTWARE
#ifndef PLATFORM_PS3_SPU
    FMOD_RESULT result;
    LocalCriticalSection    crit_dsp(mSystem->mDSPCrit);
    LocalCriticalSection    crit_dspconnect(mSystem->mDSPConnectionCrit);

    if (!target)
    {
        int numinputs = 0, numoutputs = 0;

        /*
            Disconnect all inputs connecting to this unit.
        */
        getNumInputs(&numinputs, protect);
        while (numinputs)
        {
            DSPI *input;
            DSPConnectionI *inputconnection;

            result = getInput(0, &input, &inputconnection, protect);
            if (result != FMOD_OK)
            {
                return result;
            }
            disconnectFromInternal(input, inputconnection, protect);

            getNumInputs(&numinputs, protect);
        }

        /*
            Disconnect outputs connecting to this unit.
        */
        getNumOutputs(&numoutputs, protect);
        while (numoutputs)
        {
            DSPI *output;
            DSPConnectionI *outputconnection;

            result = getOutput(0, &output, &outputconnection, protect);
            if (result != FMOD_OK)
            {
                return result;
            }
            output->disconnectFromInternal(this, outputconnection, protect);
            
            getNumOutputs(&numoutputs, protect);
        }       

        return FMOD_OK;
    }

    if (protect)
    {
        crit_dspconnect.enter();
        crit_dsp.enter();
    }

    if (!connection)
    {
        int count;
        DSPConnectionI *inputconnection;

        for (count=0; count < mNumInputs; count++)
        {
            result = getInput(count, 0, &inputconnection, protect);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (inputconnection->mInputUnit == target)
            {
                connection = inputconnection;
                break;
            }
        }
    }

    if (!connection)
    {
        return FMOD_ERR_DSP_NOTFOUND;
    }

    if (!connection->mInputUnit && !connection->mOutputUnit)
    {
        return FMOD_OK; /* Already been disconnected, must have been a double disconnect. */
    }

    connection->mInputNode.removeNode();
    mNumInputs--;         
    if (mOutputBuffer && mNumOutputs <= 1)
    {
        FMOD_Memory_Free(mOutputBuffer);

        if ((FMOD_UINT_NATIVE)mBuffer == FMOD_ALIGNPOINTER(mOutputBuffer, 16))
        {
            mBuffer = (float *)FMOD_ALIGNPOINTER(mSystem->mDSPMixBuff[mTreeLevel], 16);
        }

        mOutputBuffer = 0;
    }

    connection->mOutputNode.removeNode();
    target->mNumOutputs--;

    result = mSystem->mDSPConnectionPool.free(connection, protect);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (protect)
    {
        crit_dsp.leave();
        crit_dspconnect.leave();
    }

#endif
    return FMOD_OK;

#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
// OFFSET: 0x008DB830
FMOD_RESULT DSPI::disconnectFrom(DSPI *target, DSPConnectionI *connection)
{    
#ifdef FMOD_SUPPORT_SOFTWARE
#ifndef PLATFORM_PS3_SPU

    FMOD_OS_CriticalSection_Enter(mSystem->mDSPConnectionCrit);
    {
        DSPConnectionRequest *request;

        if (mSystem->mConnectionRequestFreeHead.isEmpty())
        {
            mSystem->flushDSPConnectionRequests();
        }

        request = (DSPConnectionRequest *)mSystem->mConnectionRequestFreeHead.getNext();

        request->removeNode();        
        request->addBefore(&mSystem->mConnectionRequestUsedHead);

        request->mThis = this;
        request->mTarget = target;
        request->mConnection = connection;
        request->mRequest = DSPCONNECTION_REQUEST_DISCONNECTFROM;

        if (target)
        {
            request->mTarget->mFlags |= FMOD_DSP_FLAG_QUEUEDFORDISCONNECT;
        }
        else
        {
            request->mThis->mFlags |= FMOD_DSP_FLAG_QUEUEDFORDISCONNECT;
        }
    }
    FMOD_OS_CriticalSection_Leave(mSystem->mDSPConnectionCrit);

#endif
    return FMOD_OK;
#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
// OFFSET: 0x008DC950
FMOD_RESULT DSPI::disconnectAllInternal(bool inputs, bool outputs, bool protect)
{
#ifdef FMOD_SUPPORT_SOFTWARE
#ifndef PLATFORM_PS3_SPU
    FMOD_RESULT result;
    int count, numinputs, numoutputs;

    if (inputs)
    {
        result = getNumInputs(&numinputs, protect);
        if (result != FMOD_OK)
        {
            return result;
        }

        for (count = 0; count < numinputs; count++)
        {
            DSPI *input;
            DSPConnectionI *inputconnection;

            result = getInput(0, &input, &inputconnection, protect);    // keep getting the first one.
            if (result != FMOD_OK)
            {
                return result;
            }

            result = disconnectFromInternal(input, inputconnection, protect);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    if (outputs)
    {
        result = getNumOutputs(&numoutputs, protect);
        if (result != FMOD_OK)
        {
            return result;
        }

        for (count = 0; count < numoutputs; count++)
        {
            DSPI *output;
            DSPConnectionI *outputconnection;

            result = getOutput(0, &output, &outputconnection, protect);    // keep getting the first one.
            if (result != FMOD_OK)
            {
                return result;
            }

            result = output->disconnectFromInternal(this, outputconnection, protect);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }
#endif

    return FMOD_OK;

#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
// OFFSET: 0x008DC340
FMOD_RESULT DSPI::disconnectAll(bool inputs, bool outputs)
{    
#ifdef FMOD_SUPPORT_SOFTWARE
#ifndef PLATFORM_PS3_SPU

    if (!inputs && !outputs)
    {
        return FMOD_OK;
    }

    FMOD_OS_CriticalSection_Enter(mSystem->mDSPConnectionCrit);
    {
        DSPConnectionRequest *request;

        if (mSystem->mConnectionRequestFreeHead.isEmpty())
        {
            mSystem->flushDSPConnectionRequests();
        }

        request = (DSPConnectionRequest *)mSystem->mConnectionRequestFreeHead.getNext();

        request->removeNode();        
        request->addBefore(&mSystem->mConnectionRequestUsedHead);

        request->mThis = this;
        request->mTarget = 0;
        if (inputs && outputs)
        {
            request->mRequest = DSPCONNECTION_REQUEST_DISCONNECTALL;
            request->mThis->mFlags |= FMOD_DSP_FLAG_QUEUEDFORDISCONNECT;
        }
        else if (inputs)
        {
            request->mRequest = DSPCONNECTION_REQUEST_DISCONNECTALLINPUTS;
        }
        else
        {
            request->mRequest = DSPCONNECTION_REQUEST_DISCONNECTALLOUTPUTS;
            request->mThis->mFlags |= FMOD_DSP_FLAG_QUEUEDFORDISCONNECT;
        }
    }
    FMOD_OS_CriticalSection_Leave(mSystem->mDSPConnectionCrit);

#endif
    return FMOD_OK;
#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
FMOD_RESULT DSPI::insertInputBetween(DSPI *dsptoinsert, int inputindex, bool search, DSPConnectionI **connection_out)
{    
#ifdef FMOD_SUPPORT_SOFTWARE
#ifndef PLATFORM_PS3_SPU
    DSPConnectionI *connection;
    FMOD_RESULT result;

    /*
        Create a new connection for this unit.
    */
    result = mSystem->mDSPConnectionPool.alloc(&connection);
    if (result != FMOD_OK)
    {
        return result;
    }
    
    connection->reset();

    FMOD_OS_CriticalSection_Enter(mSystem->mDSPConnectionCrit);
    {
        DSPConnectionRequest *request;

        if (mSystem->mConnectionRequestFreeHead.isEmpty())
        {
            mSystem->flushDSPConnectionRequests();
        }

        request = (DSPConnectionRequest *)mSystem->mConnectionRequestFreeHead.getNext();

        request->removeNode();        
        request->addBefore(&mSystem->mConnectionRequestUsedHead);

        request->mThis = this;
        request->mTarget = dsptoinsert;
        request->mConnection = connection;
        request->mInputIndex = inputindex;
        request->mRequest = search ? DSPCONNECTION_REQUEST_INSERTINBETWEEN_SEARCH : DSPCONNECTION_REQUEST_INSERTINBETWEEN;
        
        dsptoinsert->mFlags |= FMOD_DSP_FLAG_USEDADDDSP;
    }
    FMOD_OS_CriticalSection_Leave(mSystem->mDSPConnectionCrit);

    if (connection_out)
    {
        *connection_out = connection;
    }

#endif
    return FMOD_OK;
#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
FMOD_RESULT DSPI::insertInputBetweenInternal(DSPI *dsp, int inputindex, bool search, DSPConnectionI *connection, bool protect)
{
#ifdef FMOD_SUPPORT_SOFTWARE
#ifndef PLATFORM_PS3_SPU
    FMOD_RESULT result;
    DSPI *target = 0;
    DSPConnectionI *targetconnection;

    /*
        Disconnect it from wherever it used to be.
    */
    if (dsp->mFlags & FMOD_DSP_FLAG_USEDADDDSP)
    {
        DSPI *current = dsp;

        current->disconnectAllInternal(false, true, protect);   /* Disconnect outputs from start of chain. */

        while (1)
        {
            DSPI *next;

            result = current->getInput(0, &next, 0, protect);
            if (result != FMOD_OK)
            {
                break;
            }

            if (!(next->mFlags & FMOD_DSP_FLAG_USEDADDDSP))
            {
                break;
            }

            current = next;
        }        

        current->disconnectAllInternal(true, false, protect);   /* Disconnect outputs from end of chain. */
    }
    else
    {
        result = dsp->disconnectFromInternal(0, 0, protect);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        Find the next unit to insert between.  If not found, we'll just add it to the end.
    */
    getInput(inputindex, &target, &targetconnection, protect);

    /*
        Disconnect the head from its first input.
    */     
    if (target)
    {
        result = disconnectFromInternal(target, targetconnection, protect);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        Add the new input to the head.
    */
    result = addInputInternal(dsp, false, connection, 0, protect);
    if (result != FMOD_OK)
    {
        return result;
    }

    /*
        If more than 1 unit, move the pointer forward to the end so we can reconnect the 'target' onto the end of this subchain.
    */
    if (search)
    {
        while (1)
        {
            DSPI *next;

            result = dsp->getInput(0, &next, 0, protect);
            if (result != FMOD_OK)
            {
                break;
            }

            if (!(next->mFlags & FMOD_DSP_FLAG_USEDADDDSP))
            {
                break;
            }

            dsp = next;
        }
    }

    /*
        Add the old first input to this new unit.
    */
    if (target)
    { 
        result = dsp->addInputInternal(target, false, 0, 0, protect);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

#endif
    return FMOD_OK;
#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
// OFFSET: 0x008DCAC0
FMOD_RESULT DSPI::remove()
{
    return removeInternal();
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
FMOD_RESULT DSPI::removeInternal(bool protect)
{
#ifndef PLATFORM_PS3_SPU
    FMOD_RESULT result;
    DSPI *output, *input;
    int numinputs, numoutputs;

    if (!(mFlags & FMOD_DSP_FLAG_USEDADDDSP))
    {
        return disconnectFromInternal(0, 0, protect);
    }

    result = getNumInputs(&numinputs, protect);
    if (result != FMOD_OK)
    {
        return result;
    }
    result = getNumOutputs(&numoutputs, protect);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (!numinputs && !numoutputs)
    {        
        return FMOD_OK;
    }

    if (numinputs != 1 || numoutputs != 1)
    {        
        mFlags &= ~FMOD_DSP_FLAG_USEDADDDSP;
        return disconnectFromInternal(0, 0, protect);
    }

    result = setActive(false);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = getInput(0, &input, 0, protect);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = getOutput(0, &output, 0, protect);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = disconnectFromInternal(0, 0, protect);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = output->addInputInternal(input, false, 0, 0, protect);
    if (result != FMOD_OK)
    {
        return result;
    }

    mFlags &= ~FMOD_DSP_FLAG_USEDADDDSP;
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
// OFFSET: 0x008DB990
FMOD_RESULT DSPI::getNumInputs(int *numinputs, bool protect)
{
#ifdef FMOD_SUPPORT_SOFTWARE
    
    #ifndef PLATFORM_PS3_SPU
    
    if (!numinputs)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (protect)
    {
        mSystem->flushDSPConnectionRequests();

        FMOD_OS_CriticalSection_Enter(mSystem->mDSPConnectionCrit);
    }

    *numinputs = mNumInputs;

    if (protect)
    {
        FMOD_OS_CriticalSection_Leave(mSystem->mDSPConnectionCrit);
    }
    
    #endif
    
    return FMOD_OK;

#else
    return FMOD_ERR_NEEDSSOFTWARE;
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

// OFFSET: 0x008DB9E0
FMOD_RESULT DSPI::getNumOutputs(int *numoutputs, bool protect)
{
#ifdef FMOD_SUPPORT_SOFTWARE
    
    #ifndef PLATFORM_PS3_SPU
    
    if (!numoutputs)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (protect)
    {
        mSystem->flushDSPConnectionRequests();

        FMOD_OS_CriticalSection_Enter(mSystem->mDSPConnectionCrit);
    }

    *numoutputs = mNumOutputs;

    if (protect)
    {
        FMOD_OS_CriticalSection_Leave(mSystem->mDSPConnectionCrit);
    }
    
    #endif
    
    return FMOD_OK;

#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
// OFFSET: 0x008DBA30
FMOD_RESULT DSPI::reset()
{
#ifdef FMOD_SUPPORT_SOFTWARE
#ifndef PLATFORM_PS3_SPU
    if (!mDescription.reset)
    {
        return FMOD_ERR_UNSUPPORTED;
    }

    instance = (FMOD_DSP *)this;

    return mDescription.reset((FMOD_DSP_STATE *)this);
#else
    return FMOD_ERR_INTERNAL;
#endif  
#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
// OFFSET: 0x008DBA50
FMOD_RESULT DSPI::setParameter(int index, float value)
{
#ifdef FMOD_SUPPORT_SOFTWARE
#ifndef PLATFORM_PS3_SPU
    FMOD_RESULT result;

    if (!mDescription.setparameter)
    {
        return FMOD_ERR_UNSUPPORTED;
    }

    if (index < 0 || index > mDescription.numparameters)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    #ifdef FMOD_DEBUG
    
    result = FMOD_CHECKFLOAT(value);
    if (result != FMOD_OK)
    {
        return result;
    }
    
    #endif

    if (value < mDescription.paramdesc[index].min)
    {
        value = mDescription.paramdesc[index].min;
    }
    if (value > mDescription.paramdesc[index].max)
    {
        value = mDescription.paramdesc[index].max;
    }

    instance = (FMOD_DSP *)this;

    result = mDescription.setparameter((FMOD_DSP_STATE *)this, index, value);
    if (result != FMOD_OK)
    {
        return result;
    }
#endif
    return FMOD_OK;
#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
// OFFSET: 0x008DBAD0
FMOD_RESULT DSPI::getParameter(int index, float *value, char *valuestr, int valuestrlen)
{
#ifdef FMOD_SUPPORT_SOFTWARE
#ifndef PLATFORM_PS3_SPU
    FMOD_RESULT result;
    float   v;
    char    s[32];

    if (!mDescription.getparameter)
    {
        return FMOD_ERR_UNSUPPORTED;
    }

    if (index < 0 || index > mDescription.numparameters)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    instance = (FMOD_DSP *)this;

    result = mDescription.getparameter((FMOD_DSP_STATE *)this, index, &v, s);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (value)
    {
        *value = v;
    }

    if (valuestr)
    {
        FMOD_strncpy(valuestr, s, valuestrlen > 16 ? 16 : valuestrlen);
    }
#endif
    return FMOD_OK;
#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
FMOD_RESULT DSPI::getNumParameters(int *numparams)
{
#ifndef PLATFORM_PS3_SPU
    if (!numparams)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numparams = mDescription.numparameters;
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
// OFFSET: 0x008DBB60
FMOD_RESULT DSPI::getParameterInfo(int index, char *name, char *label, char *description, int descriptionlen, float *min, float *max)
{
#ifdef FMOD_SUPPORT_SOFTWARE
#ifndef PLATFORM_PS3_SPU
    if (index < 0 || index >= mDescription.numparameters)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (name)
    {
        FMOD_strcpy(name, mDescription.paramdesc[index].name);
    }
    if (description && descriptionlen)
    {
        if (mDescription.paramdesc[index].description)
        {
            FMOD_strncpy(description, mDescription.paramdesc[index].description, descriptionlen);
        }
        else
        {
            *description = 0;
        }
    }
    if (label)
    {
        FMOD_strcpy(label, mDescription.paramdesc[index].label);
    }
    if (min)
    {
        *min = mDescription.paramdesc[index].min;
    }
    if (max)
    {
        *max = mDescription.paramdesc[index].max;
    }
#endif
    return FMOD_OK;
#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
// OFFSET: 0x008DBC40
FMOD_RESULT DSPI::showConfigDialog(void *hwnd, bool show)
{
#ifdef FMOD_SUPPORT_SOFTWARE

    #ifndef PLATFROM_PS3_SPU

    if (!mDescription.config)
    {
        return FMOD_ERR_UNSUPPORTED;
    }

    instance = (FMOD_DSP *)this;

    return mDescription.config((FMOD_DSP_STATE *)this, hwnd, show);
    
    #else
    
    return FMOD_ERR_INTERNAL;
    
    #endif
#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
// OFFSET: 0x008DBC70
FMOD_RESULT DSPI::getInfo(char *name, unsigned int *version, int *channels, int *configwidth, int *configheight)
{
#ifdef FMOD_SUPPORT_SOFTWARE
    
    #ifndef PLATFORM_PS3_SPU
    
    if (name)
    {
        FMOD_strncpy(name, mDescription.name, 32);
    }

    if (version)
    {
        *version = mDescription.version;
    }

    if (channels)
    {
        *channels = mDescription.channels;
    }

    if (configwidth)
    {
        *configwidth = mDescription.configwidth;
    }

    if (configheight)
    {
        *configheight = mDescription.configheight;
    }
    
    #endif
    
    return FMOD_OK;

#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
// OFFSET: 0x008DBCD0
FMOD_RESULT DSPI::getType(FMOD_DSP_TYPE *type)
{
    if (type)
    {
        *type = mDescription.mType;
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
// OFFSET: 0x008DBCF0
FMOD_RESULT DSPI::setDefaults(float frequency, float volume, float pan, int priority)
{
#ifdef FMOD_SUPPORT_SOFTWARE
    
    #ifndef PLATFORM_PS3_SPU
    
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
    
    #endif
    
    return FMOD_OK;

#else

    return FMOD_ERR_NEEDSSOFTWARE;
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
// OFFSET: 0x008DBD80
FMOD_RESULT DSPI::getDefaults(float *frequency, float *volume, float *pan, int *priority)
{
#ifdef FMOD_SUPPORT_SOFTWARE
    
    #ifndef PLATFORM_PS3_SPU
    
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
    
    #endif
    
    return FMOD_OK;

#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
// OFFSET: 0x008DBDD0
FMOD_RESULT DSPI::setUserData(void *userdata)
{
#ifndef PLATFORM_PS3_SPU
    mDescription.userdata = userdata;
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
// OFFSET: 0x008DBDF0
FMOD_RESULT DSPI::getUserData(void **userdata)
{
#ifndef PLATFORM_PS3_SPU
    if (!userdata)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *userdata = mDescription.userdata;
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
// OFFSET: 0x008DBEA0
FMOD_RESULT DSPI::setTargetFrequency(int frequency)
{
#ifndef PLATFORM_PS3_SPU
    mTargetFrequency = frequency;
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
// OFFSET: 0x008DBEB0
FMOD_RESULT DSPI::getTargetFrequency(int *frequency)
{
#ifndef PLATFORM_PS3_SPU
    if (!frequency)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *frequency = mTargetFrequency;
#endif
    return FMOD_OK;
}


#ifdef FMOD_SUPPORT_PROFILE_DSP_VOLUMELEVELS
/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPI::calculatePeaks(const float *buffer, unsigned int length, unsigned int numchannels, DSPI *t)
{
    float           peakvolume[16]  = { 0 };
    unsigned int    sampleoffset    = 0;
    unsigned int    channeloffset   = 0;

    if (!t)
    {
        t = this;
    }

    FMOD_memset(t->mPeakVolume, 0, sizeof(t->mPeakVolume));
    
    if (buffer == NULL)
    {
        t->mNumPeakVolumeChans = 0;    
        return FMOD_OK;
    }

    while (sampleoffset < length)
    {
        float absValue = buffer[sampleoffset] < 0 ? -buffer[sampleoffset] : buffer[sampleoffset];

        if (absValue > peakvolume[channeloffset])
        {
            peakvolume[channeloffset] = absValue;
        }
        
        sampleoffset++;
        channeloffset++;

        if (channeloffset >= numchannels)
        {
            channeloffset = 0;
        }
    }

    for (channeloffset = 0; channeloffset < DSP_MAXLEVELS_MAX; channeloffset++)
    {
        unsigned short peakvalue = 0;

        /* Channel is really silent (not mearly very quiet) */
        if (peakvolume[channeloffset] <= DSP_LEVEL_SMALLVAL)
        {
            peakvalue = 0;
        }
        /* Channel volume has clipped our maximum */
        else if (peakvolume[channeloffset] > 1.0f)
        {
            peakvalue = 31;
        }
        /* Convert volume to dB (1 to 30 represents -2 to -60 dB) */
        else
        {
            float dB = FMOD_LOG10(peakvolume[channeloffset]) * 20.0f;

            peakvalue = (short)(-dB + 0.5f);
            peakvalue = peakvalue / 2;
            peakvalue = (peakvalue > 30) ? 30 : (peakvalue < 1) ? 1 : peakvalue;
        }

        /* Pack result value, 5-bits per value, stored big endian */
        {
            unsigned char   bitoffset   = channeloffset * 5;
            unsigned char   byteoffset  = bitoffset >> 3;
            unsigned char   excessbits  = bitoffset & 7;
            unsigned char   upperbound  = sizeof(t->mPeakVolume) - sizeof(short);
            unsigned short *peakdata    = NULL;

            /* Our sliding window is type short, so don't slide off the end of the array */
            excessbits  = (byteoffset > upperbound) ? excessbits + 8 : excessbits;
            byteoffset  = (byteoffset > upperbound) ? upperbound : byteoffset;

            peakvalue <<= excessbits;

            #ifdef PLATFORM_ENDIAN_LITTLE
            peakvalue = FMOD_SWAPENDIAN_WORD(peakvalue);
            #endif

            peakdata    = (unsigned short *)&t->mPeakVolume[upperbound - byteoffset];
            peakvalue  |= *peakdata;
            
            /* 
                This should be: *peakdata = peakvalue;
                Cannot do above because PS3 SPU seems to treat *peakdata as a char instead of
                a short, so we will manually assign the two bytes of the short. 
                This is because the short needs to be 2 byte aligned on the SPU.
            */
            {
                unsigned char *p = (unsigned char *)peakdata;
                unsigned char *v = (unsigned char *)&peakvalue;
                
                p[0] = v[0];
                p[1] = v[1];
            }
        }
    }

    t->mNumPeakVolumeChans = numchannels;
    return FMOD_OK;
}
#endif

#if !defined(PLATFORM_PS3_SPU) && (defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE))
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
FMOD_RESULT DSPI::startBuffering()
{
    FMOD_RESULT result;
    int channels;
    LocalCriticalSection  criticalsection(mSystem->mDSPCrit);

    if (mHistoryBuffer)
    {
        return FMOD_OK;
    }

    criticalsection.enter();

    result = mSystem->getSoftwareFormat(0, 0, &channels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mHistoryBufferMemory)
    {
        result = releaseHistoryBuffer(mHistoryBufferMemory);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    result = createHistoryBuffer(&mHistoryBufferMemory, channels);
    if (result != FMOD_OK)
    {
        return result;
    }
    mHistoryBuffer = (float *)FMOD_ALIGNPOINTER(mHistoryBufferMemory, 16);

    mHistoryPosition = 0;

    criticalsection.leave();

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
FMOD_RESULT DSPI::getHistoryBuffer(float **buffer, unsigned int *position, unsigned int *length)
{
    if (buffer)
    {
        *buffer = mHistoryBuffer;
    }

    if (position)
    {
        *position = mHistoryPosition;
    }

    if (length)
    {
        *length = FMOD_HISTORYBUFFERLEN;
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
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPI::stopBuffering()
{
    if (mHistoryBufferMemory)
    {
        FMOD_RESULT result;
        LocalCriticalSection  criticalsection(mSystem->mDSPCrit, true);

        result = releaseHistoryBuffer(mHistoryBufferMemory);
        if (result != FMOD_OK)
        {
            return result;
        }
        mHistoryBuffer = mHistoryBufferMemory = 0;
    }

    return FMOD_OK;
}

#else


FMOD_RESULT DSPI::stopBuffering()
{
    return FMOD_OK;
}

#endif


/*
#if defined(FMOD_SUPPORT_MEMORYTRACKER) && !defined(PLATFORM_PS3_SPU)

FMOD_RESULT DSPI::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(MEMTYPE_DSPI, sizeof(*this));

//        LinkedListNode          mInputHead;
//        LinkedListNode          mOutputHead;
//        int                     mNumInputs;
//        int                     mNumOutputs;

//        float                  *mOutputBuffer;
        #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
//        LinkedListNode         *mPrevious;
        #endif

//		static FMOD_OS_CRITICALSECTION *gCrit;

//        FMOD_DSP_DESCRIPTION_EX mDescription;
//        float                  *mBuffer;
//        int                     mBufferChannels;
//        char                   *mWantsToFinish;

#if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
//        DSPI                   *mMemory;
//        LinkedListNode         *mInputHeadAddress;

//        float                  *mHistoryBuffer;
//        float                  *mHistoryBufferMemory;
#endif

    return FMOD_OK;
}

#endif

*/
/*
[API]
[
	[DESCRIPTION]
    This callback is called once when a user creates a DSP unit of this type.  It is used to allocate memory, initialize variables and the like.

	[PARAMETERS]
    'dsp_state'     Pointer to the plugin state.  The user can use this variable to access runtime plugin specific variables and plugin writer user data.  Do not cast this to FMOD_DSP!  The handle to the user created DSP handle is stored within the FMOD_DSP_STATE structure.

	[RETURN_VALUE]

	[REMARKS]
    Functions that the user would have to call for this callback to be called.<br>
    System::createDSP<br>
    System::createDSPByType<br>
    System::createDSPByPlugin<br>
    Sometimes a user will re-use a DSP unit instead of releasing it and creating a new one, so it may be useful to implement FMOD_DSP_RESETCALLBACK to reset any variables or buffers when the user calls it.<br>
    <br>
    Remember to return FMOD_OK at the bottom of the function, or an appropriate error code from FMOD_RESULT.<br>

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    FMOD_DSP_STATE
    System::createDSP
    System::createDSPByType
    System::createDSPByPlugin
    FMOD_DSP_RESETCALLBACK
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_DSP_CREATECALLBACK(FMOD_DSP_STATE *dsp_state);
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
    This callback is called when the user releases the DSP unit.  It is used to free any resources allocated during the course of the lifetime of the DSP or perform any shut down code needed to clean up the DSP unit.

	[PARAMETERS]
    'dsp_state'     Pointer to the plugin state.  The user can use this variable to access runtime plugin specific variables and plugin writer user data.  Do not cast this to FMOD_DSP!  The handle to the user created DSP handle is stored within the FMOD_DSP_STATE structure.

	[RETURN_VALUE]

	[REMARKS]
    Functions that the user would have to call for this callback to be called.<br>
    DSP::release<br>
    <br>
    Remember to return FMOD_OK at the bottom of the function, or an appropriate error code from FMOD_RESULT.<br>

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    FMOD_DSP_STATE
    DSP::release
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_DSP_RELEASECALLBACK(FMOD_DSP_STATE *dsp_state);
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
    This callback function is called by DSP::reset to allow the effect to reset itself to a default state.<br>
    This is useful if an effect is for example still holding audio data for a sound that has stopped, and the unit wants to be relocated to a new sound.  Resetting the unit would clear any buffers, put the effect back to its initial state, and get it ready for new sound data.

	[PARAMETERS]
    'dsp_state'     Pointer to the plugin state.  The user can use this variable to access runtime plugin specific variables and plugin writer user data.  Do not cast this to FMOD_DSP!  The handle to the user created DSP handle is stored within the FMOD_DSP_STATE structure.

	[RETURN_VALUE]

	[REMARKS]
    Functions that the user would have to call for this callback to be called.<br>
    DSP::reset<br>
    <br>
    Remember to return FMOD_OK at the bottom of the function, or an appropriate error code from FMOD_RESULT.<br>

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    FMOD_DSP_STATE
    DSP::reset
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_DSP_RESETCALLBACK(FMOD_DSP_STATE *dsp_state);
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
    This callback is called back regularly when the unit has been created, inserted to the DSP network, and set to active by the user.<br>
    This callback requires the user to fill the output pointer with data.  Incoming data is provided and may be filtered on its way to the output pointer.<br>

	[PARAMETERS]
    'dsp_state'     Pointer to the plugin state.  The user can use this variable to access runtime plugin specific variables and plugin writer user data.  Do not cast this to FMOD_DSP!  The handle to the user created DSP handle is stored within the FMOD_DSP_STATE structure.
    'inbuffer'      Pointer to incoming floating point -1.0 to +1.0 ranged data.
    'outbuffer'     Pointer to outgoing floating point -1.0 to +1.0 ranged data.  The dsp writer must write to this pointer else there will be silence.
    'length'        The length of the incoming and outgoing buffer in samples.  To get the length of the buffer in bytes, the user must multiply this number by the number of channels coming in (and out, they may be different) and then multiply by 4 for 1 float = 4 bytes.
    'inchannels'    The number of channels of interleaved PCM data in the inbuffer parameter.  A mono signal coming in would be 1.  A stereo signal coming in would be 2.
    'outchannels'   The number of channels of interleaved PCM data in the outbuffer parameter.  A mono signal going out would be 1.  A stereo signal going out would be 2.

	[RETURN_VALUE]

	[REMARKS]
    Functions that the user would have to call for this callback to be called.<br>
    <i>None</i>.<br>
    This callback is called automatically and periodically when the DSP engine updates.<br>
    For a read update to be called it would have to be enabled, and this is done with DSP::setActive.
    <br>
    The range of -1 to 1 is a soft limit. In the case of the inbuffer it is not guaranteed to be in that range, and in the case of the outbuffer FMOD will accept values outside that range. However all values will be clamped to the range of -1 to 1 in the final mix.<br>
    <br>
    Remember to return FMOD_OK at the bottom of the function, or an appropriate error code from FMOD_RESULT.<br>

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    FMOD_DSP_STATE
    DSP::setActive
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_DSP_READCALLBACK(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels);
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
    Callback that is called when the user sets the position of a channel with Channel::setPosition.

	[PARAMETERS]
    'dsp_state'     Pointer to the plugin state.  The user can use this variable to access runtime plugin specific variables and plugin writer user data.  Do not cast this to FMOD_DSP!  The handle to the user created DSP handle is stored within the FMOD_DSP_STATE structure.
    'position'      Position in channel stream to set to.  Units are PCM samples (ie FMOD_TIMEUNIT_PCM).

	[RETURN_VALUE]

	[REMARKS]
    Functions that the user would have to call for this callback to be called.<br>
    Channel::setPosition.<br>
    If a DSP unit is attached to a channel and the user calls Channel::setPosition then this funciton will be called.
    <br>
    Remember to return FMOD_OK at the bottom of the function, or an appropriate error code from FMOD_RESULT.<br>

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    FMOD_DSP_STATE
    Channel::setPosition
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_DSP_SETPOSITIONCALLBACK(FMOD_DSP_STATE *dsp_state, unsigned int position);
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
    This callback is called when the user wants the plugin to display a configuration dialog box.  This is not always nescessary, so this can be left blank if wanted.

	[PARAMETERS]
    'dsp_state'     Pointer to the plugin state.  The user can use this variable to access runtime plugin specific variables and plugin writer user data.  Do not cast this to FMOD_DSP!  The handle to the user created DSP handle is stored within the FMOD_DSP_STATE structure.
    'hwnd'          This is the target hwnd to display the dialog in.  It must not pop up on this hwnd, it must actually be drawn within it.
    'show'          1 = show the dialog, 0 = hide/remove the dialog.

	[RETURN_VALUE]

	[REMARKS]
    Functions that the user would have to call for this callback to be called.<br>
    DSP::showConfigDialog.<br>
    <br>
    Remember to return FMOD_OK at the bottom of the function, or an appropriate error code from FMOD_RESULT.<br>

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    FMOD_DSP_STATE
    DSP::showConfigDialog
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_DSP_DIALOGCALLBACK(FMOD_DSP_STATE *dsp_state, void *hwnd, int show);
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
    This callback is called when the user wants to set a parameter for a DSP unit.

	[PARAMETERS]
    'dsp_state'     Pointer to the plugin state.  The user can use this variable to access runtime plugin specific variables and plugin writer user data.  Do not cast this to FMOD_DSP!  The handle to the user created DSP handle is stored within the FMOD_DSP_STATE structure.
    'index'         The index into the parameter list for the parameter the user wants to set.
    'value'         The value passed in by the user to set for the selected parameter.

	[RETURN_VALUE]

	[REMARKS]
    Functions that the user would have to call for this callback to be called.<br>
    DSP::setParameter.<br>
    <br>
    Range checking is not needed.  FMOD will clamp the incoming value to the specified min/max.
    <br>
    Remember to return FMOD_OK at the bottom of the function, or an appropriate error code from FMOD_RESULT.<br>

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    FMOD_DSP_STATE
    DSP::setParameter
    FMOD_DSP_GETPARAMCALLBACK
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_DSP_SETPARAMCALLBACK(FMOD_DSP_STATE *dsp_state, int index, float value);
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
    This callback is called when the user wants to get an indexed parameter from a DSP unit.

	[PARAMETERS]
    'dsp_state'     Pointer to the plugin state.  The user can use this variable to access runtime plugin specific variables and plugin writer user data.  Do not cast this to FMOD_DSP!  The handle to the user created DSP handle is stored within the FMOD_DSP_STATE structure.
    'index'         The index into the parameter list for the parameter the user wants to get.
    'value'         Pointer to a floating point variable to receive the selected parameter value.
    'valuestr'      A pointer to a string to receive the value of the selected parameter, but in text form.  This might be useful to display words instead of numbers.  For example "ON" or "OFF" instead of 1.0 and 0.0.  The length of the buffer being passed in is always 16 bytes, so do not exceed this.

	[RETURN_VALUE]

	[REMARKS]
    Functions that the user would have to call for this callback to be called.<br>
    DSP::getParameter.<br>
    FMOD_DSP_GETPARAMCALLBACK.<br>
    <br>
    Remember to return FMOD_OK at the bottom of the function, or an appropriate error code from FMOD_RESULT.<br>

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    FMOD_DSP_STATE
    DSP::getParameter
    FMOD_DSP_SETPARAMCALLBACK
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_DSP_GETPARAMCALLBACK(FMOD_DSP_STATE *dsp_state, int index, float *value, char *valuestr)
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
FMOD_RESULT DSPI::getMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, FMOD_MEMORY_USAGE_DETAILS *memoryused_details)
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
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/

#if defined(FMOD_SUPPORT_MEMORYTRACKER) && !defined(PLATFORM_PS3_SPU)

FMOD_RESULT DSPI::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(false, FMOD_MEMBITS_DSP, mDescription.mSize);

    if (mOutputBuffer)
    {
        tracker->add(false, FMOD_MEMBITS_DSP, (mSystem->mDSPBlockSize * (mSystem->mMaxOutputChannels < mSystem->mMaxInputChannels ? mSystem->mMaxInputChannels : mSystem->mMaxOutputChannels) * sizeof(float)) + 16);
    }

#if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)

    if (mHistoryBufferMemory)
    {
        int channels;
        CHECK_RESULT(mSystem->getSoftwareFormat(0, 0, &channels, 0, 0, 0));
        tracker->add(false, FMOD_MEMBITS_DSP, FMOD_HISTORYBUFFERLEN * channels * sizeof(float) + 16);
    }
#endif

    if (mDescription.getmemoryused)
    {
        CHECK_RESULT(mDescription.getmemoryused(this, tracker));
    }

    return FMOD_OK;
}

#endif

}
