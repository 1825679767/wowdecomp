#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_ITECHO

#include "fmod.h"
#include "fmod_dspi.h"
#include "fmod_dsp_itecho.h"

#ifdef PLATFORM_PS3_SPU
    #include "fmod_systemi_spu.h"
    #include "fmod_common_spu.h"
    #include "fmod_spu_printf.h"
    #include <cell/dma.h>
    #include <vmx2spu.h>
#else
    #include "fmod_systemi.h"
#endif

#include <stdio.h>

#ifdef PLATFORM_PS3
extern unsigned int _binary_spu_fmod_dsp_itecho_pic_start[];
#endif

#ifdef PLATFORM_PS3_SPU
#define USETEMPBUFFERS  // Don't want to use TEMPBUFFERS if it is processing on the PPU for IT
#endif

namespace FMOD
{

FMOD_DSP_DESCRIPTION_EX dspitecho;

#ifdef PLUGIN_EXPORTS

#ifdef __cplusplus
extern "C" {
#endif

    /*
        FMODGetDSPDescription is mandantory for every fmod plugin.  This is the symbol the registerplugin function searches for.
        Must be declared with F_API to make it export as stdcall.
    */
    F_DECLSPEC F_DLLEXPORT FMOD_DSP_DESCRIPTION_EX * F_API FMODGetDSPDescriptionEx()
    {
        return DSPITEcho::getDescriptionEx();
    }

#ifdef __cplusplus
}
#endif

#endif  /* PLUGIN_EXPORTS */


FMOD_DSP_PARAMETERDESC dspitecho_param[5] =
{
    { 0.0f,  100.0f,     50.0f, "WetDryMix",    "",         "Ratio of wet (processed) signal to dry (unprocessed) signal. Must be in the range from 0.0 through 100.0 (all wet). The default value is 50." },
    { 0.0f,  100.0f,     50.0f, "Feedback",     "%",        "Percentage of output fed back into input, in the range from 0.0 through 100.0. The default value is 50." },
    { 1.0f,  2000.0f,   500.0f, "LeftDelay",    "ms",       "Delay for left channel, in milliseconds, in the range from 1.0 through 2000.0. The default value is 500 ms." },
    { 1.0f,  2000.0f,   500.0f, "RightDelay",   "ms",       "Delay for right channel, in milliseconds, in the range from 1.0 through 2000.0. The default value is 500 ms." },
    { 0.0f,  1.0f,        0.0f, "PanDelay",     "",         "Value that specifies whether to swap left and right delays with each successive echo. The default value is zero, meaning no swap. Possible values are defined as 0.0 (equivalent to FALSE) and 1.0 (equivalent to TRUE)." },
};

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
// OFFSET: 0x008EB330
FMOD_DSP_DESCRIPTION_EX *DSPITEcho::getDescriptionEx()
{
#ifndef PLATFORM_PS3_SPU
    FMOD_memset(&dspitecho, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dspitecho.name, "FMOD IT Echo");
    dspitecho.version       = 0x00010100;
    dspitecho.create        = DSPITEcho::createCallback;
    dspitecho.release       = DSPITEcho::releaseCallback;
    dspitecho.reset         = DSPITEcho::resetCallback;

    #ifdef PLATFORM_PS3
    dspitecho.read          = (FMOD_DSP_READCALLBACK)_binary_spu_fmod_dsp_itecho_pic_start;    /* SPU PIC entry address */
    #else
    dspitecho.read          = DSPITEcho::readCallback;
    #endif

    dspitecho.numparameters = sizeof(dspitecho_param) / sizeof(dspitecho_param[0]);
    dspitecho.paramdesc     = dspitecho_param;
    dspitecho.setparameter  = DSPITEcho::setParameterCallback;
    dspitecho.getparameter  = DSPITEcho::getParameterCallback;
#ifdef FMOD_SUPPORT_MEMORYTRACKER
    dspitecho.getmemoryused = &DSPITEcho::getMemoryUsedCallback;
#endif

    dspitecho.mType         = FMOD_DSP_TYPE_ITECHO;
    dspitecho.mCategory     = FMOD_DSP_CATEGORY_FILTER;
    dspitecho.mSize         = sizeof(DSPITEcho);
#else
    dspitecho.read          = DSPITEcho::readCallback;                  /* We only care about read function on SPU */
#endif
    return &dspitecho;
}


#ifndef PLATFORM_PS3_SPU
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
FMOD_RESULT DSPITEcho::createInternal()
{
    int count;
	mOldSpeakerMask = 0xFFFF;

    init();
    
    for (count = 0; count < mDescription.numparameters; count++)
    {
        FMOD_RESULT result;

        result = setParameter(count, mDescription.paramdesc[count].defaultval);
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
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
// OFFSET: 0x008EAC50
FMOD_RESULT DSPITEcho::releaseInternal()
{
    int count;

    for (count = 0; count < 2; count++)
    {
        if (mEchoBufferMem[count])
        {
            FMOD_Memory_Free(mEchoBufferMem[count]);
            mEchoBuffer[count] = mEchoBufferMem[count] = 0;
        }
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
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
// OFFSET: 0x008EACA0
FMOD_RESULT DSPITEcho::resetInternal()
{
    mEchoPosition[0] = 0;
    mEchoPosition[1] = 0;

    if (mEchoBuffer[0])
    {
        FMOD_memset(mEchoBuffer[0], 0, mEchoBufferLengthBytes[0]);
    }
    if (mEchoBuffer[1])
    {
        FMOD_memset(mEchoBuffer[1], 0, mEchoBufferLengthBytes[1]);
    }

    return FMOD_OK;
}
#endif


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
// OFFSET: 0x008EAD00
FMOD_RESULT DSPITEcho::readInternal(float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{   
    unsigned int count;
    int channel;
	int inchannels2 = (inchannels > 2) ? 2 : inchannels;

    if (!inbuffer)
    {
        return FMOD_OK;
    }

    if (speakermask != mOldSpeakerMask) /* A speaker has been disabled: clear the echo buffer for that speaker to prevent artifacts if/when the speaker is reenabled   */
	{
		unsigned short diff = (mOldSpeakerMask ^ speakermask);

		for (channel = 0; channel < inchannels2 ; channel++)
		{
			if (diff & (1 << channel))
			{
				FMOD_memset(mEchoBuffer[channel], 0, mEchoBufferLengthBytes[channel]);
			}
		}
		mOldSpeakerMask = speakermask;
	}

    if (!(speakermask & ((1 << inchannels2)-1)))
    {
        FMOD_memcpy(outbuffer, inbuffer, length * outchannels * sizeof(float));
        return FMOD_OK;
    }

    // Prevent output buffer crosstalk by ensuring every sample is set
    if (inchannels > 2)
    {
        // Prevent output buffer crosstalk by ensuring every sample is set
        FMOD_memcpy(outbuffer, inbuffer, inchannels*length*sizeof(float));
    }

    for (channel = 0; channel < inchannels2; channel++)
    {
        unsigned int len;
        float *out, *in;

        in  = inbuffer + channel;
        out = outbuffer + channel;
		if (!((1 << channel) & speakermask))
		{
			int inc, offset1, offset2, offset3;
			len = length >> 2;
			offset1 = inchannels2;
			offset2 = inchannels2 * 2;
			offset3 = inchannels2 * 3;
			inc = inchannels2 << 2;
			while (len)
			{
				out[0] = in[0];
				out[offset1] = in[offset1];
				out[offset2] = in[offset2];
				out[offset3] = in[offset3];

				in+=inc;
				out+=inc;
				len--;
			}

			len = length & 3;
			while (len)
			{    
				out[0] = in[0];
				len--;
				in += inchannels2;
				out += inchannels2;
			}
			continue; 
		}
		else
		{
            #ifdef USETEMPBUFFERS
            float tempbuff_m[8 * 256 + 4];   // 8k
            float *tempbuff = (float *)FMOD_ALIGNPOINTER(tempbuff_m, 16);

            unsigned int oldpos = mEchoPosition[channel];

            #ifdef PLATFORM_PS3_SPU

            cellDmaGet((void *)tempbuff, (uint64_t)(mEchoBuffer[channel] + mEchoPosition[channel]), length * sizeof(float), TAG1, TID, RID);
            cellDmaWaitTagStatusAll(MASK1);

            #else
            
            FMOD_memcpy((void *)tempbuff, mEchoBuffer[channel] + mEchoPosition[channel], length * sizeof(float));
            
            #endif

            #endif

			len = length;

			while (len)
			{
				unsigned int len2 = len;
				float *echobuff;
	        
                #ifdef USETEMPBUFFERS
                echobuff = tempbuff;
                #else
				echobuff = mEchoBuffer[channel] + mEchoPosition[channel];
                #endif

				if (mEchoPosition[channel] + len > mEchoLength[channel])
				{
					len2 = mEchoLength[channel] - mEchoPosition[channel];
				}

				for (count = 0; count < len2; count++) 
				{
					float in0 = in[0];
					out[0]      = (in0 * (1.0f - mWetDryMix)) + (echobuff[0] * mWetDryMix);
					echobuff[0] = (in0                      ) + (echobuff[0] * mFeedback);

					in  +=inchannels;
					out +=inchannels;
					echobuff++;
				}

				mEchoPosition[channel] += len2;
				if (mEchoPosition[channel] >= mEchoLength[channel])
				{
					mEchoPosition[channel] = 0;
				}
				len -= len2;
			}

            #ifdef USETEMPBUFFERS

            #ifdef PLATFORM_PS3_SPU

            cellDmaPut((void *)tempbuff, (uint64_t)(mEchoBuffer[channel] + oldpos), length * sizeof(float), TAG1, TID, RID);
            cellDmaWaitTagStatusAll(MASK1);

            #else

            FMOD_memcpy((void *)(mEchoBuffer[channel] + oldpos), (void *)tempbuff, length * sizeof(float));

            #endif
            #endif

		}
    }

    return FMOD_OK;
}


#ifndef PLATFORM_PS3_SPU
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
// OFFSET: 0x008EAF20
FMOD_RESULT DSPITEcho::setParameterInternal(int index, float value)
{
    FMOD_RESULT result;
    int         channel;
    float       olddelay[2];
    bool        reset = false;

    olddelay[0] = mDelay[0];
    olddelay[1] = mDelay[1];

    result = mSystem->getSoftwareFormat(&mOutputRate, 0, 0, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    mSystem->lockDSP();
    
    switch (index)
    {
        case FMOD_DSP_ITECHO_WETDRYMIX:
        {
            mWetDryMix = value / 100.0f;
            break;
        }
        case FMOD_DSP_ITECHO_FEEDBACK:
        {
            mFeedback = value / 100.0f;
            break;
        }
        case FMOD_DSP_ITECHO_LEFTDELAY:
        {
            mDelay[0] = value;
            break;
        }
        case FMOD_DSP_ITECHO_RIGHTDELAY:
        {
            mDelay[1] = value;
            break;
        }
        case FMOD_DSP_ITECHO_PANDELAY:
        {
            mPanDelay = value < 0.5f ? false : true;
            break;
        }
    }

    for (channel = 0; channel < 2; channel++)
    {
        if (mDelay[channel] != olddelay[channel] || !mEchoBuffer[channel])
        {
            mEchoLength[channel] = (int)((float)mOutputRate * mDelay[channel]) / 1000;

            #ifdef PLATFORM_PS3
            /* 
                Keep things aligned to the blocksize otherwise we will get wrapping
                with tempbuffers.
            */
            if (mEchoLength[channel] < mSystem->mDSPBlockSize)       
            {
                mEchoLength[channel] = mSystem->mDSPBlockSize;
            }
            else if (mEchoLength[channel] > mSystem->mDSPBlockSize)
            {
                mEchoLength[channel] /= mSystem->mDSPBlockSize;
                mEchoLength[channel] *= mSystem->mDSPBlockSize;
            }
            #endif

            if (mEchoBufferMem[channel])
            {
                FMOD_Memory_Free(mEchoBufferMem[channel]);
                mEchoBuffer[channel] = mEchoBufferMem[channel] = 0;
            }

            mEchoBufferLengthBytes[channel] = mEchoLength[channel];
            mEchoBufferLengthBytes[channel] *= sizeof(float);

            #ifdef PLATFORM_PS3

            mEchoBufferMem[channel] = (float *)FMOD_Memory_Calloc(mEchoBufferLengthBytes[channel] + 128);
            if (!mEchoBufferMem[channel])
            {
                mSystem->unlockDSP();
                return FMOD_ERR_MEMORY;
            }
            mEchoBuffer[channel] = (float *)FMOD_ALIGNPOINTER(mEchoBufferMem[channel], 128);
            
            #else

            mEchoBufferMem[channel] = (float *)FMOD_Memory_Calloc(mEchoBufferLengthBytes[channel]);
            if (!mEchoBufferMem[channel])
            {
                mSystem->unlockDSP();
                return FMOD_ERR_MEMORY;
            }
            mEchoBuffer[channel] = mEchoBufferMem[channel];

            #endif

            reset = true;
        }
    }

    if (reset)
    {
        resetInternal();
    }

    mSystem->unlockDSP();

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
// OFFSET: 0x008EB0C0
FMOD_RESULT DSPITEcho::getParameterInternal(int index, float *value, char *valuestr)
{
    switch (index)
    {
        case FMOD_DSP_ITECHO_WETDRYMIX:
        {
            *value = mWetDryMix * 100.0f;
            sprintf(valuestr, "%.1f", mWetDryMix * 100.0f);
            break;
        }
        case FMOD_DSP_ITECHO_FEEDBACK:
        {
            *value = mFeedback * 100.0f;
            sprintf(valuestr, "%.1f", mFeedback * 100.0f);
            break;
        }
        case FMOD_DSP_ITECHO_LEFTDELAY:
        {
            *value = mDelay[0];
            sprintf(valuestr, "%.02f", mDelay[0]);
            break;
        }
        case FMOD_DSP_ITECHO_RIGHTDELAY:
        {
            *value = mDelay[1];
            sprintf(valuestr, "%.02f", mDelay[1]);
            break;
        }
        case FMOD_DSP_ITECHO_PANDELAY:
        {
            *value = mPanDelay ? 1.0f : 0.0f;
            sprintf(valuestr, "%s", mPanDelay ? "on" : "off");
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

#ifdef FMOD_SUPPORT_MEMORYTRACKER

FMOD_RESULT DSPITEcho::getMemoryUsedImpl(MemoryTracker *tracker)
{
    // Size of this class is already accounted for (via description.mSize). Just add extra allocated memory here.

    for (int channel = 0; channel < 2; channel++)
    {
        if (mEchoBufferMem[channel])
        {
            #ifdef PLATFORM_PS3
            tracker->add(false, FMOD_MEMBITS_DSP, mEchoBufferLengthBytes[channel] + 128);
            #else
            tracker->add(false, FMOD_MEMBITS_DSP, mEchoBufferLengthBytes[channel]);
            #endif
        }
    }

    return FMOD_OK;
}

#endif


/*
    ==============================================================================================================

    CALLBACK INTERFACE

    ==============================================================================================================
*/


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
FMOD_RESULT F_CALLBACK DSPITEcho::createCallback(FMOD_DSP_STATE *dsp)
{
    DSPITEcho *echo = (DSPITEcho *)dsp;

    return echo->createInternal();
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
// OFFSET: 0x008EB230
FMOD_RESULT F_CALLBACK DSPITEcho::releaseCallback(FMOD_DSP_STATE *dsp)
{
    DSPITEcho *echo = (DSPITEcho *)dsp;

    return echo->releaseInternal();
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
// OFFSET: 0x008EB260
FMOD_RESULT F_CALLBACK DSPITEcho::resetCallback(FMOD_DSP_STATE *dsp)
{
    DSPITEcho *echo = (DSPITEcho *)dsp;

    return echo->resetInternal();
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
// OFFSET: 0x008EB290
FMOD_RESULT F_CALLBACK DSPITEcho::readCallback(FMOD_DSP_STATE *dsp, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPITEcho *echo = (DSPITEcho *)dsp;

    return echo->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}


#ifndef PLATFORM_PS3_SPU
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
// OFFSET: 0x008EB2C0
FMOD_RESULT F_CALLBACK DSPITEcho::setParameterCallback(FMOD_DSP_STATE *dsp, int index, float value)
{
    DSPITEcho *echo = (DSPITEcho *)dsp;

    return echo->setParameterInternal(index, value);
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
// OFFSET: 0x008EB300
FMOD_RESULT F_CALLBACK DSPITEcho::getParameterCallback(FMOD_DSP_STATE *dsp, int index, float *value, char *valuestr)
{
    DSPITEcho *echo = (DSPITEcho *)dsp;

    return echo->getParameterInternal(index, value, valuestr);
}


#ifdef FMOD_SUPPORT_MEMORYTRACKER
/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK DSPITEcho::getMemoryUsedCallback(FMOD_DSP_STATE *dsp, MemoryTracker *tracker)
{
    DSPITEcho *itecho = (DSPITEcho *)dsp;    

    return itecho->DSPITEcho::getMemoryUsed(tracker);
}
#endif

#endif

}

#endif
