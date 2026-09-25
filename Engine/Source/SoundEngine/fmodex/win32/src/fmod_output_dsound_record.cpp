#include "fmod_settings.h"

#if defined(FMOD_SUPPORT_DSOUND) && defined(FMOD_SUPPORT_RECORDING)

#include "fmod.hpp"
#include "fmod_output_dsound.h"
#include "fmod_soundi.h"
#include "fmod_stringw.h"
#include "fmod_codec_wav.h"

#include <dxsdkver.h>
#if (_DXSDK_PRODUCT_MAJOR < 9 || (_DXSDK_PRODUCT_MAJOR == 9 && _DXSDK_PRODUCT_MINOR < 21))
    #include <dplay.h>  /* This defines DWORD_PTR for dsound.h to use. */
#endif
#include <dsound.h>

namespace FMOD
{

static const FMOD_GUID KSDATAFORMAT_SUBTYPE_PCM             = { 0x00000001, 0x0000, 0x0010, { 0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71} };
static const FMOD_GUID KSDATAFORMAT_SUBTYPE_IEEE_FLOAT      = { 0x00000003, 0x0000, 0x0010, { 0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71} };


/*
[
	[DESCRIPTION]
    Callback to enumerate each found input driver

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]
    The first enumerated driver is skipped as it has no GUID, (it's a duplicate
    anyway), the next driver enumerated is always the default, therefore it will be
    in element 0 of FMODs driver list as required.

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x009512C0
BOOL CALLBACK FMOD_Output_DSound_RecordEnumProc(LPGUID lpGUID, LPCWSTR lpszDesc, LPCWSTR lpszDrvName, LPVOID lpContext)
{
    FMOD::OutputDSound *outputdsound = (FMOD::OutputDSound *)lpContext;

    // Don't allow more drivers than the maximum number of drivers
	if (outputdsound->mRecordNumDrivers < FMOD_OUTPUT_MAXDRIVERS)
	{
        // Don't add pseudo driver "Primary..." (no GUID), it is just a dupe anyway
        if (lpGUID)
        {
            outputdsound->mRecordDriverName[outputdsound->mRecordNumDrivers] = (short *)FMOD_Memory_Calloc((FMOD_strlenW((short *)lpszDesc) + 1) * sizeof(short));
            if (outputdsound->mRecordDriverName[outputdsound->mRecordNumDrivers])
            {
                FMOD_strncpyW(outputdsound->mRecordDriverName[outputdsound->mRecordNumDrivers], (short *)lpszDesc, FMOD_strlenW((short *)lpszDesc));
            }

            FMOD_memcpy(&outputdsound->mRecordGUID[outputdsound->mRecordNumDrivers], lpGUID, sizeof(GUID));
	        
            {
                short driverName[FMOD_STRING_MAXNAMELEN] = {0};

                FMOD_strncpyW(driverName, (short *)lpszDesc, FMOD_STRING_MAXNAMELEN);
                FMOD_wtoa(driverName);
                
                FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "FMOD_Output_DSound_RecordEnumProc", "Enumerating \"%s\"\n", driverName));
            }

	        outputdsound->mRecordNumDrivers++;
        }
	}

	return TRUE;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x009513A0
FMOD_RESULT OutputDSound::recordEnumerate()
{
    FMOD_RESULT result;

    if (mRecordEnumerated)
    {
        return FMOD_OK;
    }

    result = registerDLL();
    if (result != FMOD_OK)
    {
        return result;
    }

    for (int i = 0; i < mRecordNumDrivers; i++)
    {
         if (mRecordDriverName[i])
         {
             FMOD_Memory_Free(mRecordDriverName[i]);
             mRecordDriverName[i] = NULL;
         }
    }
    mRecordNumDrivers = 0;

	if (mDirectSoundCaptureEnumerate)
    {
		(*mDirectSoundCaptureEnumerate)((LPDSENUMCALLBACKW)FMOD_Output_DSound_RecordEnumProc, (void *)this);
    }

    mRecordEnumerated = true;

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
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x00951420
FMOD_RESULT OutputDSound::recordGetNumDrivers(int *numdrivers)
{
    if (!numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRecordEnumerated)
    {
        FMOD_RESULT result;

        result = recordEnumerate();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    *numdrivers = mRecordNumDrivers;

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
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x00951450
FMOD_RESULT OutputDSound::recordGetDriverInfo(int id, char *name, int namelen, FMOD_GUID *guid)
{
    if (!mRecordEnumerated)
    {
        FMOD_RESULT result;

        result = recordEnumerate();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (id < 0 || id >= mRecordNumDrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (name && namelen >= 1)
    {
        short driverName[FMOD_STRING_MAXNAMELEN] = {0};

        FMOD_strncpyW(driverName, mRecordDriverName[id], FMOD_STRING_MAXNAMELEN - 1);
        FMOD_wtoa(driverName);
        
        FMOD_strncpy(name, (char *)driverName, namelen - 1);
        name[namelen - 1] = 0;
    }

    if (guid)
    {
        FMOD_memcpy(guid, &mRecordGUID[id], sizeof(FMOD_GUID));
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
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputDSound::recordGetDriverInfoW(int id, short *name, int namelen, FMOD_GUID *guid)
{
    if (!mRecordEnumerated)
    {
        FMOD_RESULT result;

        result = recordEnumerate();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (id < 0 || id >= mRecordNumDrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (name && namelen >= 1)
    {
        FMOD_strncpyW(name, mRecordDriverName[id], namelen - 1);
        name[namelen - 1] = 0;
    }

    if (guid)
    {
        FMOD_memcpy(guid, &mRecordGUID[id], sizeof(FMOD_GUID));
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
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x009514B0
FMOD_RESULT OutputDSound::recordStart(FMOD_RECORDING_INFO *recordinfo, Sound *sound, bool loop)
{
    FMOD_RESULT                 result;
	DSCBUFFERDESC               dscbd;
    WAVE_FORMATEXTENSIBLE       wfx;
    SoundI                     *soundi;
    int                         bits;
	HRESULT                     hr;
    FMOD_DSoundRecordMembers   *recordMembers   = NULL;

    soundi = (SoundI *)sound;
    if (!soundi)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

	result = registerDLL();
    if (result != FMOD_OK)
    {
        return result;
    }

	if (!mDirectSoundCaptureCreate)
	{
		return FMOD_ERR_OUTPUT_DRIVERCALL;
	}
    
    recordinfo->mRecordPlatformSpecific = recordMembers = (FMOD_DSoundRecordMembers *)FMOD_Object_Calloc(FMOD_DSoundRecordMembers);
    if (recordMembers == NULL)
    {
        return FMOD_ERR_MEMORY;   
    }

 	// ========================================================================================================
	// CREATE CAPTURE SYSTEM
	// ========================================================================================================
	hr = (*mDirectSoundCaptureCreate)(&mRecordGUID[recordinfo->mRecordId], &recordMembers->mDirectSoundCapture, 0);
	if (hr != DS_OK)
	{
		return FMOD_ERR_OUTPUT_DRIVERCALL;
	}

 	// ========================================================================================================
	// CREATE AND START CAPTURE BUFFER
	// ========================================================================================================
    recordinfo->mRecordFormat       = soundi->mFormat;
    recordinfo->mRecordChannels     = soundi->mChannels;
    recordinfo->mRecordBufferLength = soundi->mLength;
    recordinfo->mRecordRate         = (int)soundi->mDefaultFrequency;

    SoundI::getBitsFromFormat(recordinfo->mRecordFormat, &bits);

    FMOD_memset(&wfx, 0, sizeof(WAVE_FORMATEXTENSIBLE));
    wfx.Format.wFormatTag            = WAVE_FORMAT_EXTENSIBLE;
    wfx.Format.nChannels             = recordinfo->mRecordChannels;
    wfx.Format.wBitsPerSample        = bits;
    wfx.Format.nBlockAlign           = wfx.Format.nChannels * wfx.Format.wBitsPerSample / 8;
    wfx.Format.nSamplesPerSec        = recordinfo->mRecordRate;
    wfx.Format.nAvgBytesPerSec	     = wfx.Format.nSamplesPerSec * wfx.Format.nBlockAlign;
    wfx.Format.cbSize                = 22;  // Designates extra data
    wfx.Samples.wValidBitsPerSample  = bits;
    FMOD_memcpy(&wfx.SubFormat, (soundi->mFormat == FMOD_SOUND_FORMAT_PCMFLOAT ? &KSDATAFORMAT_SUBTYPE_IEEE_FLOAT : &KSDATAFORMAT_SUBTYPE_PCM), sizeof(GUID));

	dscbd.dwSize		= sizeof(DSCBUFFERDESC);
	dscbd.dwFlags		= 0;
	dscbd.dwBufferBytes = recordinfo->mRecordBufferLength * wfx.Format.nBlockAlign;
	dscbd.dwReserved	= 0;
	dscbd.lpwfxFormat	= (WAVEFORMATEX*)&wfx;
	dscbd.dwFXCount		= 0;
	dscbd.lpDSCFXDesc	= 0;

    hr = recordMembers->mDirectSoundCapture->CreateCaptureBuffer(&dscbd, &recordMembers->mDirectSoundCaptureBuffer, 0);
    if (hr != DS_OK)
    {
		return FMOD_ERR_FORMAT;
	}

	hr = recordMembers->mDirectSoundCaptureBuffer->Start(loop);
	if (hr != DS_OK)
    {
		return FMOD_ERR_OUTPUT_DRIVERCALL;
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
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x00951690
FMOD_RESULT OutputDSound::recordStop(FMOD_RECORDING_INFO *recordinfo)
{
    FMOD_DSoundRecordMembers *recordMembers = (FMOD_DSoundRecordMembers *)recordinfo->mRecordPlatformSpecific;

    if (recordinfo)
    {
	    if (recordMembers->mDirectSoundCaptureBuffer)
	    {
		    recordMembers->mDirectSoundCaptureBuffer->Stop();
		    recordMembers->mDirectSoundCaptureBuffer->Release();
		    recordMembers->mDirectSoundCaptureBuffer = 0;
	    }

        if (recordMembers->mDirectSoundCapture)
        {
            recordMembers->mDirectSoundCapture->Release();
            recordMembers->mDirectSoundCapture = 0;
        }

        FMOD_Memory_Free(recordinfo->mRecordPlatformSpecific);
        recordinfo->mRecordPlatformSpecific = 0;
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
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x009516E0
FMOD_RESULT OutputDSound::recordGetPosition(FMOD_RECORDING_INFO *recordinfo, unsigned int *pcm)
{
    FMOD_RESULT         result;
	HRESULT             hr = DS_OK;
	unsigned int        position = 0, lengthbytes;
    FMOD_SOUND_FORMAT   format;
    int                 bits, channels;
    FMOD_DSoundRecordMembers *recordMembers = (FMOD_DSoundRecordMembers *)recordinfo->mRecordPlatformSpecific;

	if (!recordMembers->mDirectSoundCaptureBuffer)
    {
        return FMOD_ERR_OUTPUT_DRIVERCALL;
    }

    hr = recordMembers->mDirectSoundCaptureBuffer->GetCurrentPosition(0, (LPDWORD)&position);
	if (hr != DS_OK)
	{
		return FMOD_ERR_OUTPUT_DRIVERCALL;
	}

    result = recordinfo->mRecordSound->getLength(&lengthbytes, FMOD_TIMEUNIT_PCMBYTES);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = recordinfo->mRecordSound->getFormat(0, &format, &channels, &bits);
    if (result != FMOD_OK)
    {
        return result;
    }

	if (position >= lengthbytes)
    {
        *pcm = 0;
		return FMOD_ERR_OUTPUT_DRIVERCALL;;
    }

    position *= 8;
    position /= bits;
    position /= channels;

	if (pcm)
    {
        *pcm = position;
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
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x00951780
FMOD_RESULT OutputDSound::recordLock(FMOD_RECORDING_INFO *recordinfo, unsigned int offset, unsigned int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2)
{
    FMOD_DSoundRecordMembers *recordMembers = (FMOD_DSoundRecordMembers *)recordinfo->mRecordPlatformSpecific;

    recordMembers->mDirectSoundCaptureBuffer->Lock(offset, length, ptr1, (DWORD *)len1, ptr2, (DWORD *)len2, 0);
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
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x009517C0
FMOD_RESULT OutputDSound::recordUnlock(FMOD_RECORDING_INFO *recordinfo, void *ptr1, void *ptr2, unsigned int len1, unsigned int len2)
{
    FMOD_DSoundRecordMembers *recordMembers = (FMOD_DSoundRecordMembers *)recordinfo->mRecordPlatformSpecific;

    recordMembers->mDirectSoundCaptureBuffer->Unlock(ptr1, len1, ptr2, len2);
    return FMOD_OK;
}

}

#endif