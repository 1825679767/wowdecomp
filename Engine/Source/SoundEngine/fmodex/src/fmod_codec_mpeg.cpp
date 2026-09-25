#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_MPEG

#include "fmod.h"
#include "fmod_codec_mpeg.h"
#include "fmod_codec_wav.h"
#include "fmod_debug.h"
#include "fmod_dsp_codec.h"
#include "fmod_file.h"
#include "fmod_metadata.h"
#include "fmod_soundi.h"
#include "fmod_string.h"
#include "fmod_types.h"

#ifdef PLATFORM_PS3_SPU
    #include "fmod_common_spu.h"
    #include "fmod_systemi_spu.h"
    #include "fmod_spu_printf.h"
    #include <cell/dma.h>
    #include <sys/spu_event.h>
    #include "../lib/sony/spu/Mp3DecSpuLib.h"
#else
    #include "fmod_systemi.h"
#endif

#include <string.h>
#include <stdio.h>

#ifdef FMOD_SUPPORT_MPEG_SPU
    #include <cell/atomic.h>
    #include "fmod_output_ps3.h"
    #include "fmod_common_spu.h"
#endif

namespace FMOD
{

FMOD_CODEC_DESCRIPTION_EX mpegcodec;

#if defined(PLUGIN_EXPORTS) && !defined(PLUGIN_FSB)

#ifdef __cplusplus
extern "C" {
#endif

    /*
        FMODGetCodecDescription is mandantory for every fmod plugin.  This is the symbol the registerplugin function searches for.
        Must be declared with F_API to make it export as stdcall.
    */
    F_DECLSPEC F_DLLEXPORT FMOD_CODEC_DESCRIPTION_EX * F_API FMODGetCodecDescriptionEx()
    {
        return CodecMPEG::getDescriptionEx();
    }

#ifdef __cplusplus
}
#endif

#endif  /* PLUGIN_EXPORTS */

#ifdef __cplusplus
extern "C" {
#endif
    float   *FMOD_Mpeg_DecWin = 0;
#ifdef __cplusplus
}
#endif

/*
    Globals
*/

#ifdef FMOD_SUPPORT_MPEG_SPU
FMOD_OS_CRITICALSECTION *gDecodeCrit = 0;
#endif

bool     CodecMPEG::gInitialized = false;

#ifndef FMOD_SUPPORT_MPEG_SONYDECODER

float    CodecMPEG::gDecWinMem[512+32+16];                /* +16 for alignment */
float    CodecMPEG::gCos64[16];
float    CodecMPEG::gCos32[8];
float    CodecMPEG::gCos16[4];
float    CodecMPEG::gCos8[2];
float    CodecMPEG::gCos4[1];
float   *CodecMPEG::gPnts[5] = { gCos64,gCos32,gCos16,gCos8,gCos4 };

int      CodecMPEG::gIntWinBase[] = 
{
     0,    -1,    -1,    -1,    -1,    -1,    -1,    -2,    -2,    -2,
    -2,    -3,    -3,    -4,    -4,    -5,    -5,    -6,    -7,    -7,
    -8,    -9,   -10,   -11,   -13,   -14,   -16,   -17,   -19,   -21,
   -24,   -26,   -29,   -31,   -35,   -38,   -41,   -45,   -49,   -53,
   -58,   -63,   -68,   -73,   -79,   -85,   -91,   -97,  -104,  -111,
  -117,  -125,  -132,  -139,  -147,  -154,  -161,  -169,  -176,  -183,
  -190,  -196,  -202,  -208,  -213,  -218,  -222,  -225,  -227,  -228,
  -228,  -227,  -224,  -221,  -215,  -208,  -200,  -189,  -177,  -163,
  -146,  -127,  -106,   -83,   -57,   -29,     2,    36,    72,   111,
   153,   197,   244,   294,   347,   401,   459,   519,   581,   645,
   711,   779,   848,   919,   991,  1064,  1137,  1210,  1283,  1356,
  1428,  1498,  1567,  1634,  1698,  1759,  1817,  1870,  1919,  1962,
  2001,  2032,  2057,  2075,  2085,  2087,  2080,  2063,  2037,  2000,
  1952,  1893,  1822,  1739,  1644,  1535,  1414,  1280,  1131,   970,
   794,   605,   402,   185,   -45,  -288,  -545,  -814, -1095, -1388,
 -1692, -2006, -2330, -2663, -3004, -3351, -3705, -4063, -4425, -4788,
 -5153, -5517, -5879, -6237, -6589, -6935, -7271, -7597, -7910, -8209,
 -8491, -8755, -8998, -9219, -9416, -9585, -9727, -9838, -9916, -9959,
 -9966, -9935, -9863, -9750, -9592, -9389, -9139, -8840, -8492, -8092,
 -7640, -7134, -6574, -5959, -5288, -4561, -3776, -2935, -2037, -1082,
   -70,   998,  2122,  3300,  4533,  5818,  7154,  8540,  9975, 11455,
 12980, 14548, 16155, 17799, 19478, 21189, 22929, 24694, 26482, 28289,
 30112, 31947, 33791, 35640, 37489, 39336, 41176, 43006, 44821, 46617,
 48390, 50137, 51853, 53534, 55178, 56778, 58333, 59838, 61289, 62684,
 64019, 65290, 66494, 67629, 68692, 69679, 70590, 71420, 72169, 72835,
 73415, 73908, 74313, 74630, 74856, 74992, 75038 };

#endif

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
// OFFSET: 0x008F6CC0
FMOD_CODEC_DESCRIPTION_EX *CodecMPEG::getDescriptionEx()
{
    FMOD_memset(&mpegcodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    mpegcodec.name        = "FMOD MPEG Codec";
    mpegcodec.version     = 0x00010100;
    mpegcodec.timeunits   = FMOD_TIMEUNIT_PCM | FMOD_TIMEUNIT_RAWBYTES;
    mpegcodec.open        = &CodecMPEG::openCallback;
    mpegcodec.close       = &CodecMPEG::closeCallback;
    mpegcodec.read        = &CodecMPEG::readCallback;
    mpegcodec.setposition = &CodecMPEG::setPositionCallback;
    mpegcodec.soundcreate = &CodecMPEG::soundCreateCallback;
    mpegcodec.reset       = &CodecMPEG::resetCallback;

    mpegcodec.mType       = FMOD_SOUND_TYPE_MPEG;
    mpegcodec.mSize       = sizeof(CodecMPEG);

    return &mpegcodec;
}

#endif // !PLATFORM_PS3_SPU

#ifndef FMOD_SUPPORT_MPEG_SONYDECODER
/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
// OFFSET: 0x008F57E0
FMOD_RESULT CodecMPEG::makeTables(int scaleval)
{
	int i,j,k,kr,divv;
	float *table,*costab;
 
	for(i=0;i<5;i++)
	{
		kr=0x10>>i; divv=0x40>>i;
		costab = gPnts[i];

		for(k=0;k<kr;k++)
        {
			costab[k] = (float)(1.0f / (2.0f * FMOD_COS(FMOD_PI * ((float) k * 2.0f + 1.0f) / (float) divv)));
        }
	}

	FMOD_Mpeg_DecWin = (float *)FMOD_ALIGNPOINTER(&gDecWinMem[0], 16);

	table = FMOD_Mpeg_DecWin;
	scaleval = -scaleval;

	for(i=0,j=0;i<256;i++,j++,table+=32)
	{
		if(table < FMOD_Mpeg_DecWin+512+16)
        {
			table[16] = table[0] = (float)((float)gIntWinBase[j] / 65536.0f * (float) scaleval);
        }

		if(i % 32 == 31)
        {
			table -= 1023;
        }

		if(i % 64 == 63)
        {
			scaleval = - scaleval;
        }
	}

	for( /* i=256 */ ;i<512;i++,j--,table+=32)
	{
		if(table < FMOD_Mpeg_DecWin+512+16)
        {
			table[16] = table[0] = (float)((float) gIntWinBase[j] / 65536.0f * (float) scaleval);
        }

		if(i % 32 == 31)
        {
			table -= 1023;
        }

		if(i % 64 == 63)
        {
			scaleval = - scaleval;
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

	[SEE_ALSO]
]
*/
// OFFSET: 0x008F5C70
FMOD_RESULT CodecMPEG::initAll() 
{
    FMOD_RESULT result;

    #ifdef FMOD_SUPPORT_MPEG_SPU
    if (!gDecodeCrit)
    {
        result = FMOD_OS_CriticalSection_Create(&gDecodeCrit);
        if (result != FMOD_OK)
        {
            return result;
        }
    }
    #endif    

    #ifndef FMOD_SUPPORT_MPEG_SONYDECODER

	FMOD_Mpeg_DecWin = (float *)FMOD_ALIGNPOINTER(&gDecWinMem[0], 16);

	result = makeTables(1);
    if (result != FMOD_OK)
    {
        return result;
    }

    #if defined(PLATFORM_PS3_SPU_STREAMDECODE) || !defined(PLATFORM_PS3_SPU)

        #ifdef FMOD_SUPPORT_MPEG_LAYER2
	    result = initLayer2();
        if (result != FMOD_OK)
        {
            return result;
        }
        #endif

        #ifdef FMOD_SUPPORT_MPEG_LAYER3
        result = initLayer3(SBLIMIT);
        if (result != FMOD_OK)
        {
            return result;
        }
        #endif

    #else

        #ifdef FMOD_SUPPORT_MPEG_LAYER2
        gInitLayer2(gGrp3Tab, gGrp5Tab, gGrp9Tab);
        #endif

        #ifdef FMOD_SUPPORT_MPEG_LAYER3
        gInitLayer3(SBLIMIT, gGainPow2, gAaCa, gAaCs, gWin, gWin1,
                                     gMapBuf0, gMapBuf1, gMapBuf2, gTfCos36, gTfCos12,
                                     gCos6_1, gCos6_2, gCos9, gCos18, gTan1_1, gTan1_2, gTan2_1, gTan2_2,
                                     gPow1_1, gPow2_1, gPow1_2, gPow2_2, gBandInfo,
                                     gMap, gMapEnd, gLongLimit, gShortLimit, gN_SLen2, gI_SLen2);
        #endif

    #endif

    #endif

    return FMOD_OK;
}

#ifndef PLATFORM_PS3_SPU

#ifdef FMOD_SUPPORT_MPEG_SPU
/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT CodecMPEG::closeAll() 
{
    if (gDecodeCrit)
    {
        FMOD_RESULT result;
        
        result = FMOD_OS_CriticalSection_Free(gDecodeCrit);
        if (result != FMOD_OK)
        {
            return result;
        }        
        gDecodeCrit = 0;
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

	[SEE_ALSO]
]
*/
// OFFSET: 0x008F5CA0
FMOD_RESULT CodecMPEG::getPCMLength() 
{
    FMOD_RESULT     result;
	unsigned int    frames = 0;
    unsigned int    byteoffset = 0;
	unsigned int    oldpos;

    result = mFile->tell(&oldpos);
    if (result != FMOD_OK)
    {
        return result;
    }

    mMemoryBlock->mNumFrames = 0;
    waveformat[0].lengthpcm = 0;

	do
	{
		unsigned int header;
        int framesize;

        if (byteoffset >= waveformat[0].lengthbytes)
        {
            break;
        }

		result = mFile->read(&header, 1, 4, 0);
        if (result != FMOD_OK)
        {
            break;
        }

		result = decodeHeader(&header, 0, 0, &framesize);
		if (result == FMOD_OK && byteoffset + framesize < waveformat[0].lengthbytes)
		{
			if (frames >= mMemoryBlock->mNumFrames)
			{
				mMemoryBlock->mNumFrames += 1000;
				mMemoryBlock->mFrameOffset = (unsigned int *)FMOD_Memory_ReAlloc(mMemoryBlock->mFrameOffset, mMemoryBlock->mNumFrames * sizeof(unsigned int));
                if (!mMemoryBlock->mFrameOffset)
                {
                    return FMOD_ERR_MEMORY;
                }
			}
			mMemoryBlock->mFrameOffset[frames] = byteoffset;
            
			waveformat[0].lengthpcm += mPCMFrameLengthBytes;
			byteoffset += (framesize + 4);
			frames++;

			result = mFile->seek(framesize, SEEK_CUR);
            if (result != FMOD_OK)
            {
                break;
            }
		}
        else
        {
    		mFile->seek(-3, SEEK_CUR);  /* increment a byte at a time! */
        }
	} while (1);

    result = mFile->seek(oldpos, SEEK_SET);
    if (result != FMOD_OK)
    {
        return result;
    }

    mMemoryBlock->mNumFrames = frames;

    return result;
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
// OFFSET: 0x008F64F0
FMOD_RESULT CodecMPEG::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO *userexinfo)
{
    FMOD_RESULT     result = FMOD_OK;
	char		    header[4];
    unsigned int    count;
    int             framesize, channels, frequency;
    bool            validheader;
    bool            manualsizecalc = false;
    unsigned int    lengthbytes = 0;

    init(FMOD_SOUND_TYPE_MPEG);

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "CodecMPEG::openInternal", "attempting to open as MPEG..\n"));

	mSrcDataOffset = 0;

	result = mFile->seek(0, SEEK_SET);
    if (result != FMOD_OK)
    {
        return result;
    }

	/*
        Support RIFF wrapped MP3?
    */
    {
        CodecWav tempwav;
        WAVE_CHUNK chunk;
        FMOD_CODEC_WAVEFORMAT tempwaveformat;
        
        FMOD_memset(&tempwav, 0, sizeof(CodecWav));
        FMOD_memset(&tempwaveformat, 0, sizeof(FMOD_CODEC_WAVEFORMAT));

        tempwav.mFile = mFile;
        tempwav.mSrcDataOffset = (unsigned int)-1;
        tempwav.waveformat = &tempwaveformat;
	    
	    /*
            Read header
        */
        result = mFile->read(&chunk, 1, sizeof(WAVE_CHUNK), 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (!FMOD_strncmp((const char *)chunk.id, "RIFF", 4))
        {
            char wave[4];
            
            result = mFile->read(wave, 1, 4, 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (!FMOD_strncmp(wave, "WAVE", 4))
            {
                #ifdef PLATFORM_ENDIAN_BIG
                chunk.size = FMOD_SWAPENDIAN_DWORD(chunk.size);
                #endif
            
                result = tempwav.parseChunk(chunk.size);
                if (result == FMOD_OK && tempwav.mSrcFormat && tempwav.mSrcDataOffset != (unsigned int)-1)
                {
                    int format = tempwav.mSrcFormat->Format.wFormatTag;

                    if (format == WAVE_FORMAT_MPEG || format == WAVE_FORMAT_MPEGLAYER3)
                    {
                        mSrcDataOffset          = tempwav.mSrcDataOffset;
                        lengthbytes             = tempwav.waveformat[0].lengthbytes;
                        mLoopPoints[0]          = tempwav.mLoopPoints[0];
                        mLoopPoints[1]          = tempwav.mLoopPoints[1];
                        mSyncPoint              = tempwav.mSyncPoint;
                        mNumSyncPoints          = tempwav.mNumSyncPoints;
                    }
                    else
                    {
    		            result = FMOD_ERR_FORMAT;
                    }

	                if (tempwav.mSrcFormat)
                    {
		                FMOD_Memory_Free(tempwav.mSrcFormat);
                        tempwav.mSrcFormat = 0;
                    }  

                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }
                else
                {
	                if (tempwav.mSrcFormat)
                    {
		                FMOD_Memory_Free(tempwav.mSrcFormat);
                        tempwav.mSrcFormat = 0;
                    }  
                }
            }
        }
    }

	/*
        If there wasnt a riff size chunk
    */
	if (!lengthbytes)
	{
		/*
            Get size of stream in bytes
        */
        result = mFile->getSize(&lengthbytes);
        if (result != FMOD_OK)
        {
            return result;
        }

        manualsizecalc = true;
	}

    mMemoryBlockMemory = (CodecMPEG_MemoryBlock *)FMOD_Memory_Calloc(sizeof(CodecMPEG_MemoryBlock) + 32);
    if (!mMemoryBlockMemory)
    {
        return FMOD_ERR_MEMORY;
    }
    mMemoryBlock = (CodecMPEG_MemoryBlock *)FMOD_ALIGNPOINTER(mMemoryBlockMemory, 16);

    mMemoryBlock->mHasXingNumFrames  = false;
    mMemoryBlock->mHasXingToc        = false;

    #ifndef FMOD_SUPPORT_MPEG_SONYDECODER
    mMemoryBlock->mFrameSizeOld      = -1;
	mMemoryBlock->mSynthBo           = 1;
    mMemoryBlock->mSynthBuffs        = (float *)FMOD_ALIGNPOINTER(mMemoryBlock->mSynthBuffsMem, 16);
    #endif

	/*
	    Scan the next 4k or more for a valid header..(arent i nice)
	*/
    do
    {
        unsigned int searchlen;

	    validheader = false;

        searchlen = lengthbytes;

        if (searchlen > 4096 && !(usermode & FMOD_MPEGSEARCH))
        {
            searchlen = 4096;
        }

	    for (count = 0; count < searchlen; count++)
	    {
		    result = mFile->seek(mSrcDataOffset, SEEK_SET);
            if (result != FMOD_OK && result != FMOD_ERR_FILE_COULDNOTSEEK)
            {
                return result;
            }

            if (result != FMOD_ERR_FILE_COULDNOTSEEK)
            {
		        result = mFile->read(header, 1, 4, 0);
                if (result != FMOD_OK)
                {
                    return result;
                }

                #ifndef FMOD_SUPPORT_MPEG_SONYDECODER
                mMemoryBlock->mLayer = 0;
                #endif

		        result = decodeHeader(header, &frequency, &channels, &framesize);
		        if (result == FMOD_OK)
                {
                    validheader = true;
			        break;
                }
            }

		    mSrcDataOffset++;
	    }

	    if (!validheader)
	    {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "CodecMPEG::openInternal", "failed to open as mpeg\n"));
	
		    return FMOD_ERR_FORMAT;
	    }

	    /*
	        Just in case we FLUKED a wav with an mpeg type header in it .. try again on the next frame.
	    */
	    result = mFile->seek(framesize, SEEK_CUR);
        if (result != FMOD_OK)
        {
            return result;
        }

	    result = mFile->read(header, 1, 4, 0);

        if (result != FMOD_ERR_FILE_EOF)
        {
            if (result != FMOD_OK)
            {
                return result;
            }

	        result = decodeHeader(header, &frequency, &channels, 0);
            if (result != FMOD_OK)
            {
   		        mSrcDataOffset++;
                validheader = false;
            }
        }

    } while (!validheader);

    #ifndef FMOD_SUPPORT_MPEG_SONYDECODER
    mMemoryBlock->mFrameSizeOld = -1;
    mMemoryBlock->mSynthBo = 1;
    mMemoryBlock->mLayer = 0;
    #endif

    mWaveFormatMemory = (FMOD_CODEC_WAVEFORMAT *)FMOD_Memory_Calloc(sizeof(FMOD_CODEC_WAVEFORMAT));
    if (!mWaveFormatMemory)
    {
        return FMOD_ERR_MEMORY;
    }
    waveformat = mWaveFormatMemory;

    waveformat[0].frequency = frequency;
    waveformat[0].lengthbytes = lengthbytes;
    waveformat[0].channels = channels;

	framesize += 4;     /* It was size-4 previously */

    if (!gInitialized)
    {
        initAll();
        gInitialized = true;
    }

	result = mFile->seek(mSrcDataOffset, SEEK_SET);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (usermode & FMOD_CREATECOMPRESSEDSAMPLE)
    {
        waveformat[0].format = FMOD_SOUND_FORMAT_MPEG;

        if (!(usermode & (FMOD_SOFTWARE | FMOD_HARDWARE)))
        {
            if (mSystem->mOutputType == FMOD_OUTPUTTYPE_OPENAL)
            {
                waveformat[0].mode |= FMOD_HARDWARE;
            }
            else
            {
                waveformat[0].mode |= FMOD_SOFTWARE;
            }
        }
    }
    else
    {
        waveformat[0].format = FMOD_SOUND_FORMAT_PCM16;
    }


    /*
        Check for XING header and decode 1 frame to determine uncompressed frame size.
    */
    {
        static unsigned char in[MAXFRAMESIZE];
        static unsigned char out[4608];
        FMOD_SOUND_FORMAT fmt;

	    result = mFile->read(in, 1, framesize, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        /*
            Check if it has a Xing header
        */
        result = decodeXingHeader(in, mMemoryBlock->mXingToc, &mMemoryBlock->mNumFrames);
        if (result == FMOD_OK)
        {
            mSrcDataOffset += framesize;

	        result = mFile->read(in, 1, framesize, 0);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
                  
        /*
	        Unpack 1 frame to get a rough compression ratio
	    */
        fmt = waveformat[0].format;

        waveformat[0].format = FMOD_SOUND_FORMAT_PCM16;

        decodeFrame(in, (short *)out, &mPCMFrameLengthBytes);

        waveformat[0].format = fmt;
    }

	result = mFile->seek(mSrcDataOffset, SEEK_SET);
    if (result != FMOD_OK)
    {
        return result;
    }

	/*
        If for some reason the frame decode didnt go as planned
    */
	if (!mPCMFrameLengthBytes)
	{
		mPCMFrameLengthBytes = 1152 * sizeof(signed short) * waveformat[0].channels;
	}
	
    framesize += 1;
	framesize &= ~1;

	if (usermode & FMOD_ACCURATETIME && mFile->mFlags & FMOD_FILE_SEEKABLE)
	{
		result = mFile->seek(mSrcDataOffset, SEEK_SET);

		result = getPCMLength();
        if (result != FMOD_OK)
        {
            return result;
        }
	}
	else
    {
        if (waveformat[0].lengthbytes == (unsigned int)-1)
        {
            waveformat[0].lengthpcm = waveformat[0].lengthbytes;
        }
        else
        {
            if (mMemoryBlock->mHasXingNumFrames)
            {
                waveformat[0].lengthpcm = 1152 * mMemoryBlock->mNumFrames;
            }
            else
            {
                /*
                    CBR calculation
                */
                int numframes = ((waveformat[0].lengthbytes + (framesize - 1)) / framesize) + 1;     /* round up, but not sure why i have to add 1 */
		        waveformat[0].lengthpcm = mPCMFrameLengthBytes * numframes;

                mFlags &= ~FMOD_CODEC_ACCURATELENGTH;
            }
        }
    }

    if (manualsizecalc && waveformat[0].lengthbytes != (unsigned int)-1)
    {
        waveformat[0].lengthbytes -= mSrcDataOffset;
    }

    if (waveformat[0].lengthpcm != (unsigned int)-1)
    {
        if (!mMemoryBlock->mHasXingNumFrames || (usermode & FMOD_ACCURATETIME))
        {
            waveformat[0].lengthpcm = waveformat[0].lengthpcm / sizeof(signed short) / waveformat[0].channels;
        }
    }

    if (usermode & FMOD_CREATECOMPRESSEDSAMPLE)
    {
        /*
            Get rid of the mem block for this sound, we don't need it.
        */
        if (mMemoryBlockMemory)
        {
            if (mMemoryBlock->mFrameOffset)
            {
                FMOD_Memory_Free(mMemoryBlock->mFrameOffset);
                mMemoryBlock->mFrameOffset = 0;
            }

            FMOD_Memory_Free(mMemoryBlockMemory);
            mMemoryBlockMemory = mMemoryBlock = 0;
        }
    }
    else
    {
        /*
            NOT FMOD_CREATECOMPRESSEDSAMPLE
        */
        mPCMBufferLength = 1152;
        mPCMBufferLengthBytes = 1152 * sizeof(signed short) * 2;

        mPCMBufferMemory = (unsigned char *)FMOD_Memory_Calloc(mPCMBufferLengthBytes + 16);
        if (!mPCMBufferMemory)
        {
            return FMOD_ERR_MEMORY;
        }
        mPCMBuffer = (unsigned char *)FMOD_ALIGNPOINTER(mPCMBufferMemory, 16);
    }

    waveformat[0].blockalign = 1152 * sizeof(signed short) * waveformat[0].channels;
    mPCMBufferLengthBytes    = mPCMBufferLength * sizeof(signed short) * waveformat[0].channels;

    #ifdef FMOD_SUPPORT_DSPCODEC
    if (waveformat[0].format == FMOD_SOUND_FORMAT_MPEG && !mSystem->mDSPCodecPool_MPEG.mNumDSPCodecs)
    {
        int count;

        result = mSystem->mDSPCodecPool_MPEG.init(FMOD_DSP_CATEGORY_DSPCODECMPEG, 1152, mSystem->mAdvancedSettings.maxMPEGcodecs ? mSystem->mAdvancedSettings.maxMPEGcodecs : FMOD_ADVANCEDSETTINGS_MAXMPEGCODECS);
        if (result != FMOD_OK)
        {
            return result;
        }

        for (count = 0; count < mSystem->mDSPCodecPool_MPEG.mNumDSPCodecs; count++)
        {
            DSPCodec *dspcodec = SAFE_CAST(DSPCodec, mSystem->mDSPCodecPool_MPEG.mPool[count]);
            CodecMPEG *mpeg    = (CodecMPEG *)dspcodec->mCodec;

            mpeg->mSrcDataOffset    = 0;    /* Raw data will start at 0. */
            mpeg->mWaveFormatMemory = 0;    /* This will be set up upon play. */                      
            mpeg->resetFrame();            

        }       
    }
    #endif

    /*
        Fill out base class members, also pointing to or allocating storage for them.
    */
    numsubsounds = 0;

    resetFrame();

    return result;
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
// OFFSET: 0x008F5E20
FMOD_RESULT CodecMPEG::closeInternal()
{
    if (mPCMBufferMemory)
    {
        FMOD_Memory_Free(mPCMBufferMemory);
        mPCMBufferMemory = mPCMBuffer = 0;
    }
    if (mWaveFormatMemory)
    {
        FMOD_Memory_Free(mWaveFormatMemory);
        mWaveFormatMemory = 0;
    }
    if (mMemoryBlockMemory)
    {
        if (mMemoryBlock->mFrameOffset)
        {
            FMOD_Memory_Free(mMemoryBlock->mFrameOffset);
            mMemoryBlock->mFrameOffset = 0;
        }

        FMOD_Memory_Free(mMemoryBlockMemory);
        mMemoryBlockMemory = mMemoryBlock = 0;
    }

    return FMOD_OK;
}

#endif // !PLATFORM_PS3_SPU

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
// OFFSET: 0x008F5EE0
FMOD_RESULT CodecMPEG::readInternal(void *buffer, unsigned int sizebytes, unsigned int *bytesread)
{
    int channel;
    FMOD_RESULT result = FMOD_OK;
    unsigned char *decodebuffer = mPCMBuffer ? mPCMBuffer : (unsigned char *)buffer;
    unsigned int bytesreadinternal = 0;

    #if defined(PLATFORM_PS3_SPU) && defined(FMOD_SUPPORT_MPEG_SONYDECODER)
    FMOD_PS3_SPU_TempStoreMixBuffers(true);
    #endif

    *bytesread = 0;

    channel = 0;
    do
    {
        int mpegchannels = 0;
    #ifndef PLATFORM_PS3_SPU_STREAMDECODE
        int  retries = 0, framesize = 0;

    #ifdef FMOD_SUPPORT_MPEG_SPU       
        unsigned char *readbuffer = (unsigned char *)gMpegReadBuffer;
	    unsigned char readbuffertemp[MAXFRAMESIZE];
    #else
        unsigned char readbuffer[MAXFRAMESIZE];
        FMOD_memset(readbuffer, 0, MAXFRAMESIZE);
    #endif

        if (waveformat->channels > 2)
        {
            mMemoryBlock = (CodecMPEG_MemoryBlock *)FMOD_ALIGNPOINTER(mMemoryBlockMemory, 16);
            mMemoryBlock = &mMemoryBlock[channel];
        }

	    do
	    {
            #ifdef FMOD_SUPPORT_MPEG_SPU
		    result = mFile->read(readbuffertemp, 1, 4, 0);
            #else
 		    result = mFile->read(readbuffer, 1, 4, 0);
            #endif
		    if (result != FMOD_OK)
		    {
                break;
		    }

            if (retries)
            {
                mMemoryBlock->mLayer = 0;
            }

            #ifdef FMOD_SUPPORT_MPEG_SPU
            /*
                On the PPU for stream decoder
            */
            result = decodeHeader(readbuffertemp, 0, &mpegchannels, &framesize);
            #else
            /*
                On the SPU
            */
            result = decodeHeader(readbuffer, 0, &mpegchannels, &framesize);
            #endif

		    /*
                If we had a bad header followed by a good header, be suspicious of it.
		        Skip forward 1 frame to check for sure, and decode that header.
		        Its nasty but it should only happen for bad data, and misaligned frame seeks.
            */
		    if ((result == FMOD_OK) && (retries 
#ifdef FMOD_SUPPORT_MPEG_LAYER2
                || mFlags & FMOD_CODEC_SEEKING /* Make double sure so it doesn't try and decode an mp2 frame by accident thanks to random bytes) */
#endif
             ))
		    {
        	    unsigned int oldpos;
			    unsigned int header;

                result = mFile->tell(&oldpos);
		        if (result != FMOD_OK)
		        {
                    break;
		        }

			    mFile->seek(framesize, SEEK_CUR);
                if (result != FMOD_OK)
                {
                    break;
                }

			    mFile->read(&header, 1, 4, 0);
                if (result != FMOD_OK)
                {
                    break;
                }

			    #ifndef PLATFORM_ENDIAN_BIG
			    header = FMOD_SWAPENDIAN_DWORD(header);
			    #endif

                int nextlay = 4-((header>>17)&3);
			    if (((header & 0xFFE00000) != 0xFFE00000) 
                    ||
                    #ifdef FMOD_SUPPORT_MPEG_SONYDECODER
                    nextlay != mMemoryBlock->mLayer
                    #else
                    nextlay != mMemoryBlock->mFrame.lay
                    #endif
                    )
                {
				    result = FMOD_ERR_FILE_BAD;
                }

                if (mFile->mFlags & FMOD_FILE_SEEKABLE)
                {
                    mFile->seek(oldpos, SEEK_SET);
                }
		    }

		    if (result != FMOD_OK)
            {
                #ifdef FMOD_SUPPORT_MPEG_SPU
                if (!FMOD_strncmp((char *)readbuffertemp, "TAG", 3))
                #else
                if (!FMOD_strncmp((char *)readbuffer, "TAG", 3))
                #endif
                {
                    mFile->seek(128-4, SEEK_CUR);    /* skip past an id3 tag */
                }
                else
                {
        	        mFile->seek(-3, SEEK_CUR);    /* increment a byte at a time! */
                }
            }

		    retries++;

	    } while (result != FMOD_OK);

        if (result == FMOD_OK)
        {
        #ifdef FMOD_SUPPORT_MPEG_SPU
		    result = mFile->read((char *)readbuffertemp+4, 1, framesize, 0);

            FMOD_OS_CriticalSection_Enter(gDecodeCrit);    

		    FMOD_memcpy(readbuffer, readbuffertemp, framesize + 4);
        #else
	        result = mFile->read((char *)readbuffer+4, 1, framesize, 0);
        #endif

	        if (result != FMOD_OK)
	        {
                #ifdef FMOD_SUPPORT_MPEG_SPU
                FMOD_OS_CriticalSection_Leave(gDecodeCrit);
                #endif            	   
                return result;
	        }

        #ifdef FMOD_SUPPORT_MPEG_SPU
            /*
                Signal SPU to do decodeFrame
            */
            sys_event_t  event;
            OutputPS3   *output;

            output = (OutputPS3 *)mSystem->mOutput;

            output->mMpegAddresses[0] = (unsigned int)mMemoryBlock;
            output->mMpegAddresses[1] = (unsigned int)decodebuffer;
            output->mMpegAddresses[2] = (unsigned int)&output->mMpegBytesRead;
            output->mMpegAddresses[3] = (unsigned int)mChannels;
            output->mMpegAddresses[4] = (unsigned int)channel;
            output->mMpegAddresses[5] = (unsigned int)waveformat->channels;
            output->mMpegAddresses[6] = bytesreadinternal;

            #ifdef FMOD_SPURS
            {
                uint16_t waitmask = 0xffff;

                /*
                    SPURS TASK
                */

                if (output->mSPURSMode == FMOD_PS3_SPURSMODE_TWOTASKS)
                {
                    /*
                        Wake up SPURS MPEG decode Task
                    */
                    cellSpursEventFlagSet(&output->mMpegEventFlagSPU, 0x01);
                }
                else
                {
                    /*
                        Atomic increment to tell SPU to decode
                    */
                    cellAtomicIncr32((unsigned int *)&output->mStreamAtomic);

                    if (output->mSPURSMode == FMOD_PS3_SPURSMODE_CREATECONTEXT)
                    {
                        /*
                            Wake up SPURS Task
                        */
                        cellSpursEventFlagSet(&output->mMixEventFlagSPU, 0x01);
                    }
                }

                /*
                    Wait for message back from SPU to say decode is done.
                */
                cellSpursEventFlagWait(&output->mMpegEventFlag, &waitmask, CELL_SPURS_EVENT_FLAG_OR);

                if (waitmask == 0xffff)
                {
                    waitmask = 0;
                }

                bytesreadinternal = waitmask;
            }
            #else
            {
                /*
                    SPU THREAD
                */
                /*
                    Atomic increment to tell SPU to decode
                */
                cellAtomicIncr32((unsigned int *)&output->mStreamAtomic);

                /*
                    Wake up SPU Thread
                */
                if (output->mSPURSMode == FMOD_PS3_SPURSMODE_TWOTASKS)
                {
                    sys_event_port_send(output->mMpegEventPortSPU, 0, 0, 0);
                }
                else
                {
                    sys_event_port_send(output->mEventPortMixSPU, 0, 0, 0);
                }

                /*
                    Wait for message back from SPU to say decode is done, this can return the bytes read
                */
                sys_event_queue_receive(output->mMpegEventQueue, &event, SYS_NO_TIMEOUT);

                bytesreadinternal = event.data3;
            }
            #endif

            FMOD_OS_CriticalSection_Leave(gDecodeCrit);

        #else   /* FMOD_SUPPORT_MPEG_SPU */

            result = decodeFrame(readbuffer, decodebuffer, &bytesreadinternal);	
            if (result != FMOD_OK)
            {
                /*
                    Corrupted frame, ignore it.
                */
                result = FMOD_OK;
            }

        #endif  /* FMOD_SUPPORT_MPEG_SPU */

            *bytesread += bytesreadinternal;
        }
        #endif  /* PLATFORM_PS3_SPU_STREAMDECODE */

        /*
            If there is an error, set mpegchannels, mpegchannels being 0 may cause
            an infinite loop.
        */
        if (!mpegchannels)
        {
            mpegchannels = 1;
        }

        channel += mpegchannels;

        #if !defined(PLATFORM_PS3) || defined(PLATFORM_PS3_SPU_STREAMDECODE)
        decodebuffer += (sizeof(short) * mpegchannels);
        #endif
    }
    while (channel < waveformat->channels);

    if (waveformat->channels > 2)
    {
        mMemoryBlock = (CodecMPEG_MemoryBlock *)FMOD_ALIGNPOINTER(mMemoryBlockMemory, 16);
    }

    #if defined(PLATFORM_PS3_SPU) && defined(FMOD_SUPPORT_MPEG_SONYDECODER)
    FMOD_PS3_SPU_TempStoreMixBuffers(false);
    #endif

    return result;
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
// OFFSET: 0x008F60E0
FMOD_RESULT CodecMPEG::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
#ifndef PLATFORM_PS3_SPU_STREAMDECODE
    FMOD_RESULT result = FMOD_OK;
    unsigned int raw = 0;
    unsigned int frame, pcmbytes, bytespersample;
    unsigned int excessbytes = 0;
    int reset = 0;

    if (postype == FMOD_TIMEUNIT_RAWBYTES)
    {
        result = mFile->seek(mSrcDataOffset + position, SEEK_SET);
        return result;
    }

    bytespersample = sizeof(signed short) * waveformat[0].channels;
    pcmbytes = position * bytespersample;
    frame = pcmbytes / mPCMFrameLengthBytes;

    /*
        Calculate how far we have gone past a frame boundary, and determine how many samples are needed and which to discard inside the mpeg block.
        Also, rewind a whole frame (but discard it) so the mpeg block gets a 'previous frame decode' to eliminate chirping!
    */
    if (pcmbytes)
    {
        unsigned int framerewind = mFlags & FMOD_CODEC_FROMFSB ? 3 : 9; /* FSB doesn't use bit reservoir. */

        excessbytes = pcmbytes - (frame * mPCMFrameLengthBytes);

        if (frame < framerewind)
        {
            framerewind = frame;
        }

        frame -= framerewind;
        excessbytes += (mPCMFrameLengthBytes * framerewind);
    }
    else
    {
        excessbytes = 0;
        position = 0;
#if 0
        int numframes = waveformat[0].lengthpcm / 1152;
        unsigned int framerewind = mFlags & FMOD_CODEC_FROMFSB ? 3 : 9; /* FSB doesn't use bit reservoir. */
        frame = numframes - framerewind;
        position = waveformat[0].lengthpcm;
        excessbytes += (1152 * bytespersample * framerewind);
        reset = true;
#endif
    }

	if (mMode & FMOD_ACCURATETIME && mMemoryBlock->mFrameOffset)
    {
		if (frame > mMemoryBlock->mNumFrames)
        {
			frame = mMemoryBlock->mNumFrames - 1;
        }
		raw = mMemoryBlock->mFrameOffset[frame];
    }
    else if (mMemoryBlock->mHasXingToc)
    {
        float percent;
        int   index;
        float fa, fb, fx;

        /*
            Calculate percentage into song to seek to
        */
        if (position > (excessbytes / bytespersample))
        {
            percent = ((float)(position - (excessbytes / bytespersample)) / (float)waveformat[0].lengthpcm) * 100.0f;
        }
        else
        {
            percent = 0;
        }

        /*
            Interpolate in TOC to get file seek point
        */
        if (percent < 0.0f)
        {
            percent = 0.0f;
        }
        if (percent > 100.0f)
        {
            percent = 100.0f;
        }

        index = (int)percent;
        
        if (index > 99)
        {
            index = 99;
        }
        fa = mMemoryBlock->mXingToc[index];

        if (index < 99)
        {
            fb = mMemoryBlock->mXingToc[index+1];
        }
        else
        {
            fb = 256.0f;
        }

        fx = fa + (fb-fa) * (percent - index);

        raw = (unsigned int)((1.0f/256.0f) * fx * waveformat[0].lengthbytes);

        if (index > 0)
        {
            /*
                First frame just had xing info so seek forward one frame.
            */
            raw += mPCMFrameLengthBytes;
        }
    }
    else
    {
        if (position  > (excessbytes / bytespersample))
        {
//            int numframes = (int)(((waveformat[0].lengthpcm + 2304) * bytespersample) / mPCMFrameLengthBytes);    /* Old fsbank added 2 frames of pcm more than compressedbytes did. */
            int numframes = (int)((waveformat[0].lengthpcm * bytespersample) / mPCMFrameLengthBytes);
            int compressedframesize = waveformat[0].lengthbytes / numframes;
            raw = frame * compressedframesize;
        }
        else
        {
            raw = 0;
        }
    }

	raw += mSrcDataOffset;

    if (raw > (unsigned int)mSrcDataOffset + (unsigned int)waveformat[0].lengthbytes)
    {
        raw = mSrcDataOffset;
    }

    result = mFile->seek(raw, SEEK_SET);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (!(mFlags & FMOD_CODEC_FROMFSB))
    {
        mFlags |= FMOD_CODEC_SEEKING;
    }

    while (excessbytes)
    {
        char buff[4608];
        unsigned int read = 0, toread = 4608;

        if (toread > excessbytes)
        {
            toread = excessbytes;
        }

        result = Codec::read(buff, toread, &read);
        if (result != FMOD_OK)
        {
            break;
        }

        /*
            During the first read after the seek, decode sometimes fails because the decoder
            hasn't built up state yet, just push on, decoded frames are dropped anyway.
        */
        if (read == 0)
        {
            read = toread;
        }

        if (excessbytes >= read)
        {
            excessbytes -= read;
        }
        else
        {
            excessbytes = 0;
        }
        
        /*
            If unbuffered, and if the amount left is smaller than a normal mpeg frame size dont keep reading, especially if it means going past the EOF.
        */
        if ((!mPCMBuffer || !mPCMBufferLengthBytes) && excessbytes < read)
        {
            excessbytes = 0;
        }
    }


    if (reset)
    {
        result = mFile->seek(mSrcDataOffset, SEEK_SET);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    mFlags &= ~FMOD_CODEC_SEEKING;

    return result;
#else
    return FMOD_OK;
#endif
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
// OFFSET: 0x008F6360
FMOD_RESULT CodecMPEG::soundCreateInternal(int subsound, FMOD_SOUND *sound)
{
#ifndef PLATFORM_PS3_SPU
    FMOD_RESULT  result = FMOD_OK;
    SoundI      *s = (SoundI *)sound;

    if (mNumSyncPoints && mSyncPoint)
    {
        int count;

        for (count = 0; count < mNumSyncPoints; count++)
        {
            s->addSyncPointInternal(mSyncPoint[count].mOffset, FMOD_TIMEUNIT_PCM, mSyncPoint[count].mName, 0, 0, false);
        }

        s->syncPointFixIndicies();

        FMOD_Memory_Free(mSyncPoint);
        mSyncPoint = 0;
    }

    return result;
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
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
// OFFSET: 0x008F6C80
FMOD_RESULT F_CALLBACK CodecMPEG::openCallback(FMOD_CODEC_STATE *codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO *userexinfo)
{
    CodecMPEG *mpeg = (CodecMPEG *)codec;

    return mpeg->openInternal(usermode, userexinfo);
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
// OFFSET: 0x008F63F0
FMOD_RESULT F_CALLBACK CodecMPEG::closeCallback(FMOD_CODEC_STATE *codec)
{
    CodecMPEG *mpeg = (CodecMPEG *)codec;

    return mpeg->closeInternal();
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
// OFFSET: 0x008F6420
FMOD_RESULT F_CALLBACK CodecMPEG::soundCreateCallback(FMOD_CODEC_STATE *codec, int subsound, FMOD_SOUND *sound)
{
    CodecMPEG *mpeg = (CodecMPEG *)codec;    

    return mpeg->soundCreateInternal(subsound, sound);
}


#endif // !PLATFORM_PS3_SPU

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
// OFFSET: 0x008F6460
FMOD_RESULT F_CALLBACK CodecMPEG::readCallback(FMOD_CODEC_STATE *codec, void *buffer, unsigned int sizebytes, unsigned int *bytesread)
{
    CodecMPEG *mpeg = (CodecMPEG *)codec;

    return mpeg->readInternal(buffer, sizebytes, bytesread);
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
// OFFSET: 0x008F6490
FMOD_RESULT F_CALLBACK CodecMPEG::setPositionCallback(FMOD_CODEC_STATE *codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    CodecMPEG *mpeg = (CodecMPEG *)codec;

    return mpeg->setPositionInternal(subsound, position, postype);
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
// OFFSET: 0x008F64C0
FMOD_RESULT F_CALLBACK CodecMPEG::resetCallback(FMOD_CODEC_STATE *codec)
{
    CodecMPEG *mpeg = (CodecMPEG *)codec;    

    return mpeg->resetInternal();
}


}

#endif


