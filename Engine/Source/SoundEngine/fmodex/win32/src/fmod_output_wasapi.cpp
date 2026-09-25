#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_WASAPI

#include "fmod_output_wasapi.h"
#include "fmod_codec_wav.h"
#include "fmod_downmix.h"
#include "fmod_localcriticalsection.h"

namespace FMOD
{

#define RECORD_LENGTH_MS            100
#define MFTIMES_PER_SEC             10000000
#define MFTIMES_PER_MILLISEC        10000
#define FEEDER_TIMEOUT_DEFAULT_MS   500
#define FEEDER_TIMEOUT_PARTIAL_MS   3

#define EXIT_ON_CONDITION(condition, error) \
    if (condition) { fResult = (error); goto Exit; }
#define SAFE_RELEASE(pPointer) \
    if ((pPointer) != NULL) { (pPointer)->Release(); (pPointer) = NULL; }
#define SAFE_FREE(pPointer) \
    if ((pPointer) != NULL) { FMOD_Memory_Free((pPointer)); (pPointer) = NULL; }

FMOD_OUTPUT_DESCRIPTION_EX wasapioutput;

#ifdef PLUGIN_EXPORTS

#ifdef __cplusplus
extern "C" {
#endif

    /*
        FMODGetOutputDescription is mandantory for every fmod plugin.
        This is the symbol the registerplugin function searches for.
        Must be declared with F_API to make it export as stdcall.
    */
    F_DECLSPEC F_DLLEXPORT FMOD_OUTPUT_DESCRIPTION_EX * F_API FMODGetOutputDescriptionEx()
    {
        return OutputWASAPI::getDescriptionEx();
    }

#ifdef __cplusplus
}
#endif

#endif  /* PLUGIN_EXPORTS */


/*
[
	[DESCRIPTION]
    Assign methods for various FMOD output hooks required by output type

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OUTPUT_DESCRIPTION_EX

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x009394E0
FMOD_OUTPUT_DESCRIPTION_EX *OutputWASAPI::getDescriptionEx()
{
    FMOD_memset(&wasapioutput, 0, sizeof(FMOD_OUTPUT_DESCRIPTION_EX));

    wasapioutput.name                   = "FMOD WASAPI Output";
    wasapioutput.version                = 0x00010100;
    wasapioutput.mType                  = FMOD_OUTPUTTYPE_WASAPI;
    wasapioutput.mSize                  = sizeof(OutputWASAPI);
    wasapioutput.polling                = false;
    
    wasapioutput.getnumdrivers          = &OutputWASAPI::getNumDriversCallback;
    wasapioutput.getdriverinfo          = &OutputWASAPI::getDriverInfoCallback;
    wasapioutput.getdriverinfow         = &OutputWASAPI::getDriverInfoWCallback;
    wasapioutput.getdrivercapsex        = &OutputWASAPI::getDriverCapsExCallback;
    wasapioutput.initex                 = &OutputWASAPI::initExCallback;
    wasapioutput.close                  = &OutputWASAPI::closeCallback;
    wasapioutput.start                  = &OutputWASAPI::startCallback;
    wasapioutput.stop                   = &OutputWASAPI::stopCallback;
    wasapioutput.gethandle              = &OutputWASAPI::getHandleCallback;

#ifdef FMOD_SUPPORT_RECORDING
    wasapioutput.record_getnumdrivers   = &OutputWASAPI::recordGetNumDriversCallback;
    wasapioutput.record_getdriverinfo   = &OutputWASAPI::recordGetDriverInfoCallback;
    wasapioutput.record_getdriverinfow  = &OutputWASAPI::recordGetDriverInfoWCallback;
    wasapioutput.record_getdrivercaps   = &OutputWASAPI::recordGetDriverCapsCallback;
    wasapioutput.record_start           = &OutputWASAPI::recordStartCallback;
    wasapioutput.record_stop            = &OutputWASAPI::recordStopCallback;
    wasapioutput.record_getposition     = &OutputWASAPI::recordGetPositionCallback;
    wasapioutput.record_lock            = &OutputWASAPI::recordLockCallback;
#endif

#ifdef FMOD_SUPPORT_MEMORYTRACKER
    wasapioutput.getmemoryused          = &OutputWASAPI::getMemoryUsedCallback;
#endif

    return &wasapioutput;
}


/*
[
	[DESCRIPTION]
    Enumerate the default devices for element 0 of the lists

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x00937370
FMOD_RESULT OutputWASAPI::enumerateDefaultDevices(IMMDeviceEnumerator *pEnumerator)
{
    HRESULT         hResult                                 = S_OK;
    FMOD_RESULT     fResult                                 = FMOD_OK;
    IMMDevice      *pDevice                                 = NULL;
    IPropertyStore *pProperties                             = NULL;
    short           deviceString[FMOD_STRING_MAXNAMELEN]    = {0};
    PROPVARIANT     propDesc;
    PROPVARIANT     propName;
    
    hResult = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    EXIT_ON_CONDITION(FAILED(hResult) && hResult != E_NOTFOUND, FMOD_ERR_OUTPUT_INIT); 
    
    // Add the default output driver if available
    if (hResult != E_NOTFOUND)
    {
        hResult = pDevice->OpenPropertyStore(STGM_READ, &pProperties);
        EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

        PropVariantInit(&propDesc);
        PropVariantInit(&propName);

        // Get the description of the endpoint device, e.g. "Speakers"
        hResult = pProperties->GetValue(PKEY_Device_DeviceDesc, &propDesc);
        EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

        // Get the name of the adapter the endpoint device is connected to, e.g. "XYZ Audio Adapter"
        hResult = pProperties->GetValue(PKEY_DeviceInterface_FriendlyName, &propName);
        EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

        // Combine properties into an output string
        _snwprintf((wchar_t *)deviceString, FMOD_STRING_MAXNAMELEN, L"%s (%s)", propDesc.pwszVal, propName.pwszVal);

        mRenderDrivers[0].name = (short *)FMOD_Memory_Calloc((FMOD_strlenW(deviceString) + 1) * sizeof(short));
        EXIT_ON_CONDITION(mRenderDrivers[0].name == NULL, FMOD_ERR_MEMORY);
        FMOD_strncpyW(mRenderDrivers[0].name, deviceString, FMOD_strlenW(deviceString));
        
        hResult = pDevice->GetId(&mRenderDrivers[0].id);
        EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

        mNumRenderDrivers++;

        // Cleanup
        PropVariantClear(&propDesc);
        PropVariantClear(&propName);
        SAFE_RELEASE(pProperties);
        SAFE_RELEASE(pDevice);
    }

    hResult = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);
    EXIT_ON_CONDITION(FAILED(hResult) && hResult != E_NOTFOUND, FMOD_ERR_OUTPUT_INIT); 
    
    // Add the default input driver if available
    if (hResult != E_NOTFOUND)
    {
        hResult = pDevice->OpenPropertyStore(STGM_READ, &pProperties);
        EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

        PropVariantInit(&propDesc);
        PropVariantInit(&propName);

        // Get the description of the endpoint device, e.g. "Speakers"
        hResult = pProperties->GetValue(PKEY_Device_DeviceDesc, &propDesc);
        EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

        // Get the name of the adapter the endpoint device is connected to, e.g. "XYZ Audio Adapter"
        hResult = pProperties->GetValue(PKEY_DeviceInterface_FriendlyName, &propName);
        EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);
        
        // Combine properties into an output string
        _snwprintf((wchar_t *)deviceString, FMOD_STRING_MAXNAMELEN, L"%s (%s)", propDesc.pwszVal, propName.pwszVal);

        mCaptureDrivers[0].name = (short *)FMOD_Memory_Calloc((FMOD_strlenW(deviceString) + 1) * sizeof(short));
        EXIT_ON_CONDITION(mCaptureDrivers[0].name == NULL, FMOD_ERR_MEMORY);
        FMOD_strncpyW(mCaptureDrivers[0].name, deviceString, FMOD_strlenW(deviceString));

        hResult = pDevice->GetId(&mCaptureDrivers[0].id);
        EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

        mNumCaptureDrivers++;
    }

Exit:
    // Cleanup
    PropVariantClear(&propDesc);
    PropVariantClear(&propName);
    SAFE_RELEASE(pProperties);
    SAFE_RELEASE(pDevice);

    return fResult;
}


/*
[
	[DESCRIPTION]
    Extract information about attached audio devices

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x009376C0
FMOD_RESULT OutputWASAPI::enumerate()
{   
    HRESULT                 hResult         = S_OK;
    FMOD_RESULT             fResult         = FMOD_OK;
    IMMDeviceEnumerator    *pEnumerator     = NULL;
    IMMDeviceCollection    *pCollection     = NULL;
    IMMDevice              *pDevice         = NULL;
    IPropertyStore         *pProperties     = NULL;
	unsigned int            numDevices      = 0;
    unsigned int			deviceIndex     = 0;
    PROPVARIANT             propDesc;
    PROPVARIANT             propName;

    Plugin::init(); /* Set up gGlobal - for debug / file / memory access by this plugin. */

    if (mEnumerated)
    {
        return FMOD_OK;
    }

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::enumerate", "Enumerating...\n"));

	if (!mCoInitialized)
	{
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::enumerate", "Initialise COM library.\n"));
        
        hResult = CoInitialize(NULL);
        if (hResult == S_OK || hResult == S_FALSE)
        {
            mCoInitialized = true;
        }
	}

    cleanUpEnumeration();

    // Get an enumerator to collect endpoint information
    hResult = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_COM);

    // Get the default devices
    fResult = enumerateDefaultDevices(pEnumerator);
    EXIT_ON_CONDITION(fResult != FMOD_OK, fResult);

    // Get a collection of all endpoints available in the system
    hResult = pEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &pCollection);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    // Retrieve the number of endpoints
    hResult = pCollection->GetCount(&numDevices);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);
    
    // Get details about each endpoint
    for (deviceIndex = 0; deviceIndex < numDevices; deviceIndex++)
    {
        short        deviceString[FMOD_STRING_MAXNAMELEN]   = {0};
        IMMEndpoint *pEndPoint                              = NULL;
        EDataFlow    dataFlow                               = eAll;

        hResult = pCollection->Item(deviceIndex, &pDevice);
        EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

        hResult = pDevice->OpenPropertyStore(STGM_READ, &pProperties);
        EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

        PropVariantInit(&propDesc);
        PropVariantInit(&propName);

        // Get the description of the endpoint device, e.g. "Speakers"
        hResult = pProperties->GetValue(PKEY_Device_DeviceDesc, &propDesc);
        EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

        // Get the name of the adapter the endpoint device is connected to, e.g. "XYZ Audio Adapter"
        hResult = pProperties->GetValue(PKEY_DeviceInterface_FriendlyName, &propName);
        EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);
        
        // Combine properties into an output string
        _snwprintf((wchar_t *)deviceString, FMOD_STRING_MAXNAMELEN, L"%s (%s)", propDesc.pwszVal, propName.pwszVal);

        // Determine if the device in an input device or an output device
        pDevice->QueryInterface(IID_IMMEndpoint, (void**)&pEndPoint);
        pEndPoint->GetDataFlow(&dataFlow);

        // Add to the outputs list (ensure there is still room in the list and this is not the default device already listed)
        if ((dataFlow == eRender || dataFlow == eAll) && (mNumRenderDrivers < FMOD_OUTPUT_MAXDRIVERS) && (!mRenderDrivers[0].name || (FMOD_strcmpW(deviceString, mRenderDrivers[0].name) != 0)))
        {
            mRenderDrivers[mNumRenderDrivers].name = (short *)FMOD_Memory_Calloc((FMOD_strlenW(deviceString) + 1) * sizeof(short));
            EXIT_ON_CONDITION(mRenderDrivers[mNumRenderDrivers].name == NULL, FMOD_ERR_MEMORY);
            FMOD_strncpyW(mRenderDrivers[mNumRenderDrivers].name, deviceString, FMOD_strlenW(deviceString));
            
            hResult = pDevice->GetId(&mRenderDrivers[mNumRenderDrivers].id);
            EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

            mNumRenderDrivers++;
        }
        
        // Add to the inputs list (ensure there is still room in the list and this is not the default device already listed)
        if ((dataFlow == eCapture || dataFlow == eAll) && (mNumCaptureDrivers < FMOD_OUTPUT_MAXDRIVERS) && (!mCaptureDrivers[0].name || (FMOD_strcmpW(deviceString, mCaptureDrivers[0].name) != 0)))
        {
            mCaptureDrivers[mNumCaptureDrivers].name = (short *)FMOD_Memory_Calloc((FMOD_strlenW(deviceString) + 1) * sizeof(short));
            EXIT_ON_CONDITION(mCaptureDrivers[mNumCaptureDrivers].name == NULL, FMOD_ERR_MEMORY);
            FMOD_strncpyW(mCaptureDrivers[mNumCaptureDrivers].name, deviceString, FMOD_strlenW(deviceString));
            
            hResult = pDevice->GetId(&mCaptureDrivers[mNumCaptureDrivers].id);
            EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

            mNumCaptureDrivers++;
        }
        
        {
            short driverName[FMOD_STRING_MAXNAMELEN] = {0};

            FMOD_strncpyW(driverName, deviceString, FMOD_STRING_MAXNAMELEN - 1);
            FMOD_wtoa(driverName);

            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::enumerate", "Found Driver: \"%s\".\n", driverName));
        }

        PropVariantClear(&propDesc);
        PropVariantClear(&propName);
        SAFE_RELEASE(pProperties);
        SAFE_RELEASE(pDevice);
    }

    // Enumeration completed successfully, clean up
    SAFE_RELEASE(pCollection);
    SAFE_RELEASE(pEnumerator);
    
    mEnumerated = true;
    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::enumerate", "Done.\n"));
    return FMOD_OK;

Exit:
    FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "OutputWASAPI::enumerate", "Enumeration Failed!\n"));

    // Enumeration failed, clean up
    PropVariantClear(&propName);
    PropVariantClear(&propDesc);
    SAFE_RELEASE(pProperties);
    SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pCollection);
    SAFE_RELEASE(pEnumerator);
    
    return fResult;
}


/*
[
	[DESCRIPTION]
    Return the number of attached audio output devices 

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWASAPI::getNumDrivers(int *numdrivers)
{   
    if (!numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mEnumerated)
    {
        FMOD_RESULT result = FMOD_OK;

        result = enumerate();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    *numdrivers = mNumRenderDrivers;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Take a wide char UUID string and produce an FMOD GUID

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]
    This parsing is naughty, we are not supposed to make assumptions about the
    structure of a MMDevice ID, but we need a GUID, so we are going to parse it
    NOTE: The Structure of the MMDevice ID is a UUID. e.g.
          "{0.0.0.00000000}.{ac233b1d-4807-4f89-949e-e9256fe037c0}"

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWASAPI::parseUUIDString(WCHAR *srcString, FMOD_GUID *destGUID)
{
    WCHAR  dataChunk[9]  = {0};
    int    index         = 18;  // Start at 18 to jump past the initial 0's section

    // Read 8 wide chars for Data1, then skip the '-'
    FMOD_memcpy(dataChunk, &srcString[index], sizeof(WCHAR) * 8);
    destGUID->Data1 = wcstoul(dataChunk, NULL, 16);
    index += 8 + 1;

    // Read 4 wide chars for Data2, then skip the '-'
    FMOD_memset(dataChunk, 0, 9);
    FMOD_memcpy(dataChunk, &srcString[index], sizeof(WCHAR) * 4);
    destGUID->Data2 = (unsigned short)wcstoul(dataChunk, NULL, 16);
    index += 4 + 1;

    // Read 4 wide chars for Data3, then skip the '-'
    FMOD_memset(dataChunk, 0, 9);
    FMOD_memcpy(dataChunk, &srcString[index], sizeof(WCHAR) * 4);
    destGUID->Data3 = (unsigned short)wcstoul(dataChunk, NULL, 16);
    index += 4 + 1;

    // Read 2 wide chars for Data4[0]
    FMOD_memset(dataChunk, 0, 9);
    FMOD_memcpy(dataChunk, &srcString[index], sizeof(WCHAR) * 2);
    destGUID->Data4[0] = (unsigned char)wcstoul(dataChunk, NULL, 16);
    index += 2 + 0;

    // Read 2 wide chars for Data4[1], then skip the '-'
    FMOD_memset(dataChunk, 0, 9);
    FMOD_memcpy(dataChunk, &srcString[index], sizeof(WCHAR) * 2);
    destGUID->Data4[1] = (unsigned char)wcstoul(dataChunk, NULL, 16);
    index += 2 + 1;

    // Read 2 wide chars at a time for the remainder of Data4
    for (int i = 2; i < 8; i++)
    {
        FMOD_memset(dataChunk, 0, 9);
        FMOD_memcpy(dataChunk, &srcString[index], sizeof(WCHAR) * 2);
        destGUID->Data4[i] = (unsigned char)wcstoul(dataChunk, NULL, 16);
        index += 2 + 0;
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Return the name and GUID of an attached output device given its index

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]
    
    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWASAPI::getDriverInfo(int driver, char *name, int namelen, FMOD_GUID *guid)
{
    if (!mEnumerated)
    {
        FMOD_RESULT result = FMOD_OK;

        result = enumerate();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (driver < 0 || driver >= mNumRenderDrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (name && namelen >= 1)
    {
        short driverName[FMOD_STRING_MAXNAMELEN] = {0};

        FMOD_strncpyW(driverName, mRenderDrivers[driver].name, FMOD_STRING_MAXNAMELEN - 1);
        FMOD_wtoa(driverName);

        FMOD_strncpy(name, (char *)driverName, namelen - 1);
        name[namelen - 1] = 0;
    }

    if (guid)
    {
        parseUUIDString(mRenderDrivers[driver].id, guid);
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Return the name and GUID of an attached output device given its index

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]
    
    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWASAPI::getDriverInfoW(int driver, short *name, int namelen, FMOD_GUID *guid)
{
    if (!mEnumerated)
    {
        FMOD_RESULT result = FMOD_OK;

        result = enumerate();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (driver < 0 || driver >= mNumRenderDrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (name && namelen >= 1)
    {
        FMOD_strncpyW(name, mRenderDrivers[driver].name, namelen - 1);
        name[namelen - 1] = 0;
    }

    if (guid)
    {
        parseUUIDString(mRenderDrivers[driver].id, guid);
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Determine capabilities of the device specified

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]
    Currently only shared mode caps are reported as the user doesn't specify that they want
    excusive mode until after Init

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x00937BE0
FMOD_RESULT OutputWASAPI::getDriverCapsEx(int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency, FMOD_SPEAKERMODE *controlpanelspeakermode)
{
    FMOD_RESULT           fResult            = FMOD_OK;
    HRESULT               hResult            = S_OK;
    IMMDeviceEnumerator  *pEnumerator        = NULL;
    IMMDevice            *pDevice            = NULL;
    IAudioClient         *pAudioClient       = NULL;
    WAVE_FORMATEX        *pWindowsMixFormat  = NULL;  
    
    // Enumerate devices if haven't already
    if (!mEnumerated)
    {
        fResult = enumerate();
        if (fResult != FMOD_OK)
        {
            return fResult;
        }
    }

    // Get an enumerator to collect endpoint information
    hResult = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    // Get the selected driver
    hResult = pEnumerator->GetDevice(mRenderDrivers[id].id, &pDevice);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);    

    // Create connection to audio engine
    hResult = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    // Get the control panel mix format details
    hResult = pAudioClient->GetMixFormat(&pWindowsMixFormat);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    /*
        Establish the speaker mode
        Note: Number of channels must be equal to mix format
    */
    switch (pWindowsMixFormat->nChannels)
    {
        case 2  :  *controlpanelspeakermode = FMOD_SPEAKERMODE_STEREO;
                   break;
        case 4  :  *controlpanelspeakermode = FMOD_SPEAKERMODE_QUAD;
                   break;
        case 6  :  *controlpanelspeakermode = FMOD_SPEAKERMODE_5POINT1;
                   break;
        case 8  :  *controlpanelspeakermode = FMOD_SPEAKERMODE_7POINT1;
                   break;
        default :  *controlpanelspeakermode = FMOD_SPEAKERMODE_STEREO;
                   break;
    }

    /*
        Establish the min / max frequencies
        Note: Sample rate must be equal to mix format
    */
    *minfrequency = *maxfrequency = pWindowsMixFormat->nSamplesPerSec;

    /*
        Establish the caps
        Note: WASAPI supports any format (trivial conversion)
    */
    *caps = FMOD_CAPS_OUTPUT_FORMAT_PCMFLOAT | FMOD_CAPS_OUTPUT_FORMAT_PCM32 |
            FMOD_CAPS_OUTPUT_FORMAT_PCM24    | FMOD_CAPS_OUTPUT_FORMAT_PCM16 |
            FMOD_CAPS_OUTPUT_FORMAT_PCM8;

    // Multichannel support is tied to the mix format
    if (pWindowsMixFormat->nChannels > 2)
    {
        *caps |= FMOD_CAPS_OUTPUT_MULTICHANNEL;
    }

Exit:
    CoTaskMemFree(pWindowsMixFormat);
    SAFE_RELEASE(pAudioClient);
    SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pEnumerator);

    return fResult;
}


/*
[
	[DESCRIPTION]
    Initialise the audio output device

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x00937D60
FMOD_RESULT OutputWASAPI::initEx(int selecteddriver, FMOD_INITFLAGS flags, int *outputrate, int outputchannels, FMOD_SOUND_FORMAT *outputformat, FMOD_SPEAKERMODE *speakermode, int dspbufferlength, int dspnumbuffers, int max2dchannels, int max3dchannels, void *extradriverdata)
{
    FMOD_RESULT             fResult               = FMOD_OK;
    HRESULT                 hResult               = S_OK;
    BOOL                    bResult               = FALSE;
    IMMDeviceEnumerator    *pEnumerator           = NULL;
    IMMDeviceCollection    *pCollection           = NULL;
    IMMDevice              *pDevice               = NULL;    
    WAVE_FORMATEX          *pWindowsMixFormat     = NULL;
    WAVE_FORMATEX          *pSimilarWaveFormat    = NULL;
    WAVE_FORMATEXTENSIBLE   waveFormat;   
	int					    bits			      = 0;
    REFERENCE_TIME          hnsBufferLengthTime   = 0;
    REFERENCE_TIME          hnsPeriod             = 0;
    BYTE                   *pLockedBuffer         = NULL;
    LARGE_INTEGER           eventStartTime;
      
    // Enumerate devices if haven't already
    if (!mEnumerated)
    {
        fResult = enumerate();
        if (fResult != FMOD_OK)
        {
            return fResult;
        }
    }

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::initEx", "Initialising...\n"));

    // Cannot initialise without at least one device
    if (mNumRenderDrivers == 0)
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "OutputWASAPI::initEx", "No output sound devices found\n"));
        return FMOD_ERR_OUTPUT_INIT;
    }

    // Extract the mode from the init flags
    if (flags & FMOD_INIT_WASAPI_EXCLUSIVE)
    {
        mExclusiveMode = true;
    }

    // Get an enumerator to collect endpoint information
    hResult = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    // Get the selected device
    hResult = pEnumerator->GetDevice(mRenderDrivers[selecteddriver].id, &pDevice);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    // Create connection to audio engine
    hResult = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&mRenderAudioClient);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    // Prepare for format details
    FMOD_memset(&waveFormat, 0, sizeof(WAVE_FORMATEXTENSIBLE));
    hResult = mRenderAudioClient->GetMixFormat(&pWindowsMixFormat);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    // Get the number of bits per sample
    fResult = SoundI::getBitsFromFormat(*outputformat, &bits);
    EXIT_ON_CONDITION(fResult != FMOD_OK, fResult);
    
    // We may need to force the FMOD channel count if it doesn't match nicely with the output
    if (outputchannels != pWindowsMixFormat->nChannels)
    {
        if ((outputchannels == 1 || outputchannels == 2) && (outputchannels < pWindowsMixFormat->nChannels))
        {
            // Exception case, allow mono or stereo output if there are enough channels for it
        }
        else
        {
            // Force the output to the windows mix format
            outputchannels = pWindowsMixFormat->nChannels;
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::initEx", "Forcing output channels to match Windows mix format: %d channel(s)\n", outputchannels));
        }
    }

    // We need to update the output channels here so the DSP system knows (incase we use a resampler)
    if (!mSystem->mDownmix)
    {
        mSystem->mMaxOutputChannels = outputchannels;
    }

    // Override the speaker mode, unless its set to raw
    if (*speakermode != FMOD_SPEAKERMODE_RAW)
    {
        switch (outputchannels)
        {
            case 1:  *speakermode = FMOD_SPEAKERMODE_MONO;
                     break;
            case 2:  *speakermode = FMOD_SPEAKERMODE_STEREO;
                     break;
            case 4:  *speakermode = FMOD_SPEAKERMODE_QUAD;
                     break;
            case 5:  *speakermode = FMOD_SPEAKERMODE_SURROUND;
                     break;
            case 6:  *speakermode = FMOD_SPEAKERMODE_5POINT1;
                     break;
            case 8:  *speakermode = FMOD_SPEAKERMODE_7POINT1;
                     break;
        }
    }

    // Set the device to use the user requested values
    waveFormat.Format.wFormatTag            = WAVE_FORMAT_EXTENSIBLE;
    waveFormat.Format.nChannels             = pWindowsMixFormat->nChannels;
    waveFormat.Format.wBitsPerSample        = bits;
    waveFormat.Format.nBlockAlign           = waveFormat.Format.nChannels * waveFormat.Format.wBitsPerSample / 8;
    waveFormat.Format.nSamplesPerSec        = *outputrate;
    waveFormat.Format.nAvgBytesPerSec	    = waveFormat.Format.nSamplesPerSec * waveFormat.Format.nBlockAlign;
    waveFormat.Format.cbSize                = 22;  // Designates extra data
    waveFormat.Samples.wValidBitsPerSample  = bits;
    FMOD_memcpy(&waveFormat.SubFormat, (*outputformat == FMOD_SOUND_FORMAT_PCMFLOAT ? &KSDATAFORMAT_SUBTYPE_IEEE_FLOAT : &KSDATAFORMAT_SUBTYPE_PCM), sizeof(GUID));

    // See if the device can do the requested format (may work in exclusive mode)
    hResult = mRenderAudioClient->IsFormatSupported((mExclusiveMode ? AUDCLNT_SHAREMODE_EXCLUSIVE : AUDCLNT_SHAREMODE_SHARED), (WAVE_FORMATEX*)&waveFormat, &pSimilarWaveFormat);
    if (hResult == AUDCLNT_E_UNSUPPORTED_FORMAT || hResult == S_FALSE)
    {
        waveFormat.Format.nSamplesPerSec   = pWindowsMixFormat->nSamplesPerSec;
        waveFormat.Format.nAvgBytesPerSec  = waveFormat.Format.nSamplesPerSec * waveFormat.Format.nBlockAlign;
    }

    // Setup a DSPResampler to handle resampling if required
    mMixerResamplerDSP = NULL;
    if (*outputrate != (int)waveFormat.Format.nSamplesPerSec)
    {
        FMOD_RESULT              result       = FMOD_OK;
        unsigned int             blocklength  = 0;
        FMOD_DSP_DESCRIPTION_EX  description;
 
        blocklength = dspbufferlength;
        blocklength /= 16;    /* Round down to nearest 16 bytes */
        blocklength *= 16;
        
        FMOD_memset(&description, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));
        description.channels                  = outputchannels;
        description.mFormat                   = FMOD_SOUND_FORMAT_PCMFLOAT;    /* Read callback will always give floats */
        description.userdata                  = this;
        description.read                      = mixerResampleReadCallback;
        description.mResamplerBlockLength     = blocklength;

        mMixerResamplerDSP                    = FMOD_Object_Alloc(DSPResampler);
        mMixerResamplerDSP->mSystem           = mSystem;
        
        result = mMixerResamplerDSP->alloc(&description);
        EXIT_ON_CONDITION(result != FMOD_OK, result);

        mMixerResamplerDSP->mTargetFrequency  = (int)waveFormat.Format.nSamplesPerSec;  /* Need to overwrite the default target frequency set in alloc */
        mMixerResamplerDSP->mLength           = (unsigned int)-1;
        mMixerResamplerDSP->mLoopCount        = 0;
        
        result = mMixerResamplerDSP->setFrequency((float)*outputrate);
        EXIT_ON_CONDITION(result != FMOD_OK, result);

        result = mMixerResamplerDSP->setFinished(false);
        EXIT_ON_CONDITION(result != FMOD_OK, result);

        // Adjust the dspbufferlength so that after resampling we achieve the same latency desired by the user (don't do setDSPBufferSize, might screw other things up)
        dspbufferlength = (int)(((float)dspbufferlength / (float)*outputrate) * waveFormat.Format.nSamplesPerSec);
    }

    // Create a buffer to handle converting the number of channels to the output channel count
    mMixerChannelConversionBuffer  = NULL;
    mRenderChannels                = pWindowsMixFormat->nChannels;
    if (outputchannels != waveFormat.Format.nChannels)
    {
        mMixerChannelConversionBufferSizeBytes = dspbufferlength * outputchannels * sizeof(float);
        mMixerChannelConversionBuffer = (float *)FMOD_Memory_Calloc(mMixerChannelConversionBufferSizeBytes);
        EXIT_ON_CONDITION(mMixerChannelConversionBuffer == NULL, FMOD_ERR_MEMORY);
    }

    // Due to sample rate or channel conversions we will take care of format conversion also, prevents dsp_soundcard doing it (we need floats)
    mMixerFormatConversionBuffer  = NULL;
    mRenderFormat                 = *outputformat;
    if (*outputformat != FMOD_SOUND_FORMAT_PCMFLOAT)
    {
        mMixerFormatConversionBufferSizeBytes = dspbufferlength * mRenderChannels * sizeof(float);
        mMixerFormatConversionBuffer = (float *)FMOD_Memory_Calloc(mMixerFormatConversionBufferSizeBytes);
        EXIT_ON_CONDITION(mMixerFormatConversionBuffer == NULL, FMOD_ERR_MEMORY);

        *outputformat = FMOD_SOUND_FORMAT_PCMFLOAT;  // This will stop dsp_soundcard converting
    }

    if (mExclusiveMode)
    {
        REFERENCE_TIME hnsRequestedDuration = 0;
        
        // Get the minimum duration for lowest latency
        hResult = mRenderAudioClient->GetDevicePeriod(NULL, &hnsRequestedDuration);
        EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

        // Internally WASAPI will double buffer for exclusive mode
        hnsPeriod            = hnsRequestedDuration;
        hnsBufferLengthTime  = hnsRequestedDuration;    // Must equal period
    }
    else
    {
        REFERENCE_TIME hnsRequestedDuration = 0;
        
        // Get the default duration (fixed shared poll rate, should be 10ms)
        hResult = mRenderAudioClient->GetDevicePeriod(&hnsRequestedDuration, NULL);
        EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

        hnsPeriod            = hnsRequestedDuration;    // Must be shared poll rate
        hnsBufferLengthTime  = hnsRequestedDuration * dspnumbuffers;
    }    

    // This time init for real, this may still format error if the mix format isnt supported in exclusive mode
    hResult = mRenderAudioClient->Initialize((mExclusiveMode ? AUDCLNT_SHAREMODE_EXCLUSIVE : AUDCLNT_SHAREMODE_SHARED), 0, hnsBufferLengthTime, hnsPeriod, (WAVE_FORMATEX*)&waveFormat, NULL);
    EXIT_ON_CONDITION(hResult == AUDCLNT_E_UNSUPPORTED_FORMAT, FMOD_ERR_OUTPUT_FORMAT);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    // Store the block alignment for later conversions
    mRenderBlockAlign = waveFormat.Format.nBlockAlign;
    // Store the buffer size information
    mMixerBufferBlockSize = dspbufferlength;
    mMixerBufferNumBlocks = dspnumbuffers;

    // Create and set an event handle to wait on in the feeder thread
    mFeederTimeout = FEEDER_TIMEOUT_DEFAULT_MS;
    eventStartTime.QuadPart = -10000;   // Start the timer 1ms after setting it
    mFeederEventHandle = CreateWaitableTimer(NULL, FALSE, NULL);
    EXIT_ON_CONDITION(mFeederEventHandle == NULL, FMOD_ERR_OUTPUT_INIT);
    bResult = SetWaitableTimer(mFeederEventHandle, &eventStartTime, (int)(hnsPeriod / MFTIMES_PER_MILLISEC), NULL, NULL, FALSE);
    EXIT_ON_CONDITION(bResult == 0, FMOD_ERR_OUTPUT_INIT);    

    // Calculate the actual length of the buffer in frames
    hResult = mRenderAudioClient->GetBufferSize(&mRenderBufferLength);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    // Get a rendering interface for the stream to output sound
    hResult = mRenderAudioClient->GetService(IID_IAudioRenderClient, (void**)&mRenderClient);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    // Initialise the shared buffer with silence by getting it all then releasing
    hResult = mRenderClient->GetBuffer(mRenderBufferLength, &pLockedBuffer);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);
    hResult = mRenderClient->ReleaseBuffer(mRenderBufferLength, 0);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    // Setup and allocate the mixer buffer
    mMixerReadPosition   = 0;
    mMixerWritePosition  = 0;
    mMixerBufferLength   = mMixerBufferBlockSize * mMixerBufferNumBlocks;
    mMixerBuffer         = (BYTE*)FMOD_Memory_Calloc(mMixerBufferLength * mRenderBlockAlign);
    EXIT_ON_CONDITION(mMixerBuffer == NULL, FMOD_ERR_MEMORY);

    // Successfully initialised audio output, cleanup   
    mInitialised = true;
    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::initEx", "Done.\n"));

Exit:
    CoTaskMemFree(pSimilarWaveFormat);
    CoTaskMemFree(pWindowsMixFormat);
    SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pEnumerator);

    return fResult;
}


/*
[
	[DESCRIPTION]
    Shutdown and cleanup resources created

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x009381B0
FMOD_RESULT OutputWASAPI::close()
{
    Plugin::init(); /* Set up gGlobal - for debug / file / memory access by this plugin. */

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::close", "Closing...\n"));
    
    SAFE_RELEASE(mRenderClient);
    SAFE_RELEASE(mRenderAudioClient);

    cleanUpEnumeration();

    if (mMixerResamplerDSP)
    {
        mMixerResamplerDSP->release(true);
        mMixerResamplerDSP = NULL;
    }
    
    SAFE_FREE(mMixerChannelConversionBuffer);
    SAFE_FREE(mMixerFormatConversionBuffer);
    SAFE_FREE(mMixerBuffer);
    
    if (mFeederEventHandle)
    {
        CloseHandle(mFeederEventHandle);
        mFeederEventHandle = NULL;
    }
    
    if (mCoInitialized)
    {
        CoUninitialize();
        mCoInitialized = false;
    }

    mInitialised = false;
    mEnumerated  = false;
    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::close", "Done.\n"));
    
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Return a handle to the rendering client instance

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWASAPI::getHandle(void **handle)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *handle = mRenderClient;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Update the hardware / Vista SW mixer

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]
    This thread is high priority, it must execute fast, simply copy
    the mix data from the mixer thread to the hardware.

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWASAPI::feederUpdate()
{
    FMOD_RESULT   fResult             = FMOD_OK;
    HRESULT       hResult             = S_OK;
    DWORD         dResult             = 0;
    unsigned int  numFramesPadding    = 0;
    unsigned int  numFramesAvailable  = 0;
    BYTE         *pLockedBuffer       = NULL;

    if (!mFeederThreadElevated)
    {
        FMOD_OS_LIBRARY *avrtHandle = NULL;

        fResult = FMOD_OS_Library_Load("avrt.dll", &avrtHandle);
        if (fResult == FMOD_OK)
        {
            HANDLE (WINAPI *setThreadTask)(LPCTSTR, LPDWORD);

            fResult = FMOD_OS_Library_GetProcAddress(avrtHandle, "AvSetMmThreadCharacteristicsA", (void **)&setThreadTask);
            if (fResult == FMOD_OK)
            {
                DWORD taskIndex = 0;

                setThreadTask(mExclusiveMode ? "Pro Audio" : "Audio", &taskIndex);
                FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::feederUpdate", "WASAPI feeder thread set to '%s' characteristic\n", mExclusiveMode ? "Pro Audio" : "Audio"));
            }

            FMOD_OS_Library_Free(avrtHandle);
        }

        // Thread is elevated or the OS doesn't support elevation, either way don't try again
        mFeederThreadElevated = true;
    }
    
    dResult = WaitForSingleObject(mFeederEventHandle, mFeederTimeout);
    if (dResult == WAIT_FAILED || (dResult == WAIT_TIMEOUT && mFeederTimeout != FEEDER_TIMEOUT_PARTIAL_MS))
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "OutputWASAPI::feederUpdate", "WASAPI feeder event timer failed or is not responding...\n"));
        return FMOD_ERR_OUTPUT_DRIVERCALL;
    }
    
    // Find out how much of the buffer is filled (padded)
    hResult = mRenderAudioClient->GetCurrentPadding(&numFramesPadding);
    if (FAILED(hResult))
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "OutputWASAPI::feederUpdate", "Cannot determine buffer usage, aborting mix.\n"));
        return FMOD_ERR_OUTPUT_DRIVERCALL;
    }

    numFramesAvailable = mRenderBufferLength - numFramesPadding;
    if (numFramesPadding == 0)
    {
        FLOG((FMOD_DEBUG_LEVEL_WARNING, __FILE__, __LINE__, "OutputWASAPI::feederUpdate", "Starvation detected in WASAPI output buffer!\n"));
    }

    // Ensure we don't ask for more than we can provide
    if (numFramesAvailable > (unsigned int)mMixerBufferLength)
    {
        numFramesAvailable = mMixerBufferLength;
        mFeederTimeout = FEEDER_TIMEOUT_PARTIAL_MS;
        FLOG((FMOD_DEBUG_LEVEL_WARNING, __FILE__, __LINE__, "OutputWASAPI::feederUpdate", "DSP buffer not large enough to satisfy requested read!\n"));
    }
    else
    {
        mFeederTimeout = FEEDER_TIMEOUT_DEFAULT_MS;
    }
    
    hResult = mRenderClient->GetBuffer(numFramesAvailable, &pLockedBuffer);
    if (FAILED(hResult))
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "OutputWASAPI::feederUpdate", "Cannot lock the buffer to write data, aborting mix.\n"));
        return FMOD_ERR_OUTPUT_DRIVERCALL;
    }

    /*
        Copy the required data from the mix buffer
    */
    {
        int requiredData            = numFramesAvailable;
        int requiredDataBytes       = requiredData       * mRenderBlockAlign;
        int mixerBufferLengthBytes  = mMixerBufferLength * mRenderBlockAlign;
        int mixerReadPositionBytes  = mMixerReadPosition * mRenderBlockAlign;

        // Do we need to wrap read from the mixer buffer
        if (mixerReadPositionBytes + requiredDataBytes > mixerBufferLengthBytes)
        {
            int sizeBytes = mixerBufferLengthBytes - mixerReadPositionBytes;
            
            FMOD_memcpy(pLockedBuffer,             mMixerBuffer + mixerReadPositionBytes, sizeBytes);
            FMOD_memcpy(pLockedBuffer + sizeBytes, mMixerBuffer                         , requiredDataBytes - sizeBytes);
        }
        else
        {
            FMOD_memcpy(pLockedBuffer,             mMixerBuffer + mixerReadPositionBytes, requiredDataBytes);
        }
        
        mMixerReadPosition += requiredData;
        mMixerReadPosition %= mMixerBufferLength;
    }

    hResult = mRenderClient->ReleaseBuffer(numFramesAvailable, 0);
    if (FAILED(hResult))
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "OutputWASAPI::feederUpdate", "Cannot release the write buffer, aborting mix.\n"));
        return FMOD_ERR_OUTPUT_DRIVERCALL;
    }

    // Tell the mixer to check if it needs to mix
    mMixerThread.wakeupThread();
//MGB    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::feederUpdate", "Read %d samples from mixer buffer\n", numFramesAvailable));

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Buffer up mix data so the feeder thread can copy the pre-mixed data quickly

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWASAPI::mixerUpdate()
{
    FMOD_RESULT  result       = FMOD_OK;
    int          writeBlock   = mMixerWritePosition / mMixerBufferBlockSize;
    int          readBlock    = mMixerReadPosition / mMixerBufferBlockSize;
    int          numChannels  = 0;

    mSystem->mDSPTimeStamp.stampIn();

    if (mSystem->mDownmix)
    {
        mSystem->mDownmix->getOutputChannels(&numChannels);
    }
    else
    {
        result = mSystem->getSoftwareFormat(NULL, NULL, &numChannels, NULL, NULL, NULL);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    while (writeBlock != readBlock)
    {              
        void *mixLocation = mMixerBuffer + ((writeBlock * mMixerBufferBlockSize) * mRenderBlockAlign);
        
        // Do we need to resample to a set rate for the output?
        if (mMixerResamplerDSP)
        {
            // Do we need to change the number of channels, i.e. fill output with a subset of the channels
            if (mMixerChannelConversionBuffer)
            {
                // Do we need to change the format from float to the output format?
                if (mMixerFormatConversionBuffer)
                {
                    // Resample pulls on the mix via a resample read callback
                    mMixerResamplerDSP->mBuffer = mMixerChannelConversionBuffer;
                    result = mMixerResamplerDSP->read(&mMixerChannelConversionBuffer, &numChannels, &mMixerBufferBlockSize, (FMOD_SPEAKERMODE)0, numChannels, mDSPTick);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }

                    // Mix a mono or stereo source into the front left and front right for the multichannel output
                    result = channelConvert(mMixerFormatConversionBuffer, mMixerChannelConversionBuffer, mRenderChannels, numChannels, mMixerBufferBlockSize);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }

                    // We must convert here since the above resampler / channel convert needs floats (i.e. we cant use the convert in dsp_soundcard)
                    result = DSPI::convert(mixLocation, mMixerFormatConversionBuffer, mRenderFormat, FMOD_SOUND_FORMAT_PCMFLOAT, mMixerBufferBlockSize * mRenderChannels, 1, 1, 1.0f);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }
                // No Format conversion required
                else
                {
                    // Resample pulls on the mix via a resample read callback
                    mMixerResamplerDSP->mBuffer = mMixerChannelConversionBuffer;
                    result = mMixerResamplerDSP->read(&mMixerChannelConversionBuffer, &numChannels, &mMixerBufferBlockSize, (FMOD_SPEAKERMODE)0, numChannels, mDSPTick);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }

                    // Mix a mono or stereo source into the front left and front right for the multichannel output
                    result = channelConvert((float*)mixLocation, mMixerChannelConversionBuffer, mRenderChannels, numChannels, mMixerBufferBlockSize);
                    if (result != FMOD_OK)
                    {
                        return result;
                    } 
                }                
            }
            else
            {
                // Do we need to change the format from float to the output format?
                if (mMixerFormatConversionBuffer)
                {
                    // Resample pulls on the mix via a resample read callback
                    mMixerResamplerDSP->mBuffer = mMixerFormatConversionBuffer;
                    result = mMixerResamplerDSP->read(&mMixerFormatConversionBuffer, &numChannels, &mMixerBufferBlockSize, (FMOD_SPEAKERMODE)0, numChannels, mDSPTick);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }

                    // We must convert here since the above resampler needs floats (i.e. we cant use the convert in dsp_soundcard)
                    result = DSPI::convert(mixLocation, mMixerFormatConversionBuffer, mRenderFormat, FMOD_SOUND_FORMAT_PCMFLOAT, mMixerBufferBlockSize * mRenderChannels, 1, 1, 1.0f);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }
                // No Format conversion required
                else
                {
                    // Resample pulls on the mix via a resample read callback
                    mMixerResamplerDSP->mBuffer = (float*)mixLocation;
                    result = mMixerResamplerDSP->read((float**)&mixLocation, &numChannels, &mMixerBufferBlockSize, (FMOD_SPEAKERMODE)0, numChannels, mDSPTick);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }
            }

            mDSPTick++;
        }
        // No resample required
        else
        {
            // Do we need to change the number of channels, i.e. fill output with a subset of the channels
            if (mMixerChannelConversionBuffer)
            {
                // Do we need to change the format from float to the output format?
                if (mMixerFormatConversionBuffer)
                {
                    // Straight mix of the DSP tree
                    result = mix(mMixerChannelConversionBuffer, mMixerBufferBlockSize);
                    if (result != FMOD_OK)
                    {
                        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::mixerUpdate", "Mix failed!\n"));
                        return result;
                    }

                    // Mix a mono or stereo source into the front left and front right for the multichannel output
                    result = channelConvert(mMixerFormatConversionBuffer, mMixerChannelConversionBuffer, mRenderChannels, numChannels, mMixerBufferBlockSize);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }                  

                    // We must convert here since the above channel convert needs floats (i.e. we cant use the convert in dsp_soundcard)
                    result = DSPI::convert(mixLocation, mMixerFormatConversionBuffer, mRenderFormat, FMOD_SOUND_FORMAT_PCMFLOAT, mMixerBufferBlockSize * mRenderChannels, 1, 1, 1.0f);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }
                // No Format conversion required
                else
                {
                    // Straight mix of the DSP tree
                    result = mix(mMixerChannelConversionBuffer, mMixerBufferBlockSize);
                    if (result != FMOD_OK)
                    {
                        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::mixerUpdate", "Mix failed!\n"));
                        return result;
                    }

                    // Mix a mono or stereo source into the front left and front right for the multichannel output
                    result = channelConvert((float*)mixLocation, mMixerChannelConversionBuffer, mRenderChannels, numChannels, mMixerBufferBlockSize);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }  
                }                
            }
            else
            {
                // Do we need to change the format from float to the output format?
                if (mMixerFormatConversionBuffer)
                {
                    // Straight mix of the DSP tree
                    result = mix(mMixerFormatConversionBuffer, mMixerBufferBlockSize);
                    if (result != FMOD_OK)
                    {
                        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::mixerUpdate", "Mix failed!\n"));
                        return result;
                    }

                    // We must convert here since the above resampler needs floats (i.e. we cant use the convert in dsp_soundcard)
                    result = DSPI::convert(mixLocation, mMixerFormatConversionBuffer, mRenderFormat, FMOD_SOUND_FORMAT_PCMFLOAT, mMixerBufferBlockSize * mRenderChannels, 1, 1, 1.0f);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }
                // No Format conversion required
                else
                {
                    // Straight mix of the DSP tree
                    result = mix(mixLocation, mMixerBufferBlockSize);
                    if (result != FMOD_OK)
                    {
                        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::mixerUpdate", "Mix failed!\n"));
                        return result;
                    }
                }
            }
        }
        
        writeBlock++;
        writeBlock %= mMixerBufferNumBlocks;
    }

    mMixerWritePosition = mMixerBufferBlockSize * writeBlock;

    mSystem->mDSPTimeStamp.stampOut(95);

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Read for the mixer resampler

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWASAPI::mixerResampleRead(float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{
    FMOD_RESULT result = FMOD_OK;

    result = mix(outbuffer, length);
    if (result != FMOD_OK)
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::mixerResampleRead", "Mix failed!\n"));
        return result;
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Start playing the shared buffer

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWASAPI::start()
{
    FMOD_RESULT  fResult  = FMOD_OK;
    HRESULT      hResult  = S_OK;

    mFeederThreadElevated = false;
    fResult = mFeederThread.initThread("FMOD (WASAPI) feeder thread", feederThreadCallback, this, Thread::PRIORITY_NORMAL, 0, 0, false, 0, mSystem);
    if (fResult != FMOD_OK)
    {
        return fResult;
    } 

    fResult = mMixerThread.initThread("FMOD mixer thread", mixerThreadCallback, this, Thread::PRIORITY_HIGH, 0, 0, true, 0, mSystem);
    if (fResult != FMOD_OK)
    {
        mFeederThread.closeThread();
        return fResult;
    } 

    hResult = mRenderAudioClient->Start();
    if (FAILED(hResult))
    {
        mFeederThread.closeThread();
        mMixerThread.closeThread();
        return FMOD_ERR_OUTPUT_DRIVERCALL;
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Stop playing the shared buffer

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWASAPI::stop()
{
    FMOD_RESULT fResult = FMOD_OK;
    HRESULT     hResult = S_OK;

    if (mInitialised)
    {
        fResult = mFeederThread.closeThread();
        if (fResult != FMOD_OK)
        {
            return fResult;
        }
        mFeederThreadElevated = false;

        fResult = mMixerThread.closeThread();
        if (fResult != FMOD_OK)
        {
            return fResult;
        }

        hResult = mRenderAudioClient->Stop();
        if (FAILED(hResult))
        {
            return FMOD_ERR_OUTPUT_DRIVERCALL;
        }
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Handle converting from one channel layout to another

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWASAPI::channelConvert(float *outbuffer, float *inbuffer, int outchannels, int inchannels, int length)
{
    // Only allow mono or stereo source
    if (inchannels <= 0 || inchannels > 2)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    // Don't allow down mixing channels
    if (outchannels < inchannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    // Mix a mono or stereo source into the front left and right for the multichannel output
    FMOD_memset(outbuffer, 0, length * outchannels * sizeof(float));
    for (int i = 0; i < length; i++)
    {
        if (inchannels == 1)  // Source is mono
        {
           outbuffer[(i * outchannels) + 0] = inbuffer[(i * inchannels) + 0] * 0.707f;
           outbuffer[(i * outchannels) + 1] = inbuffer[(i * inchannels) + 0] * 0.707f;
        }
        else if (inchannels == 2)  // Source is stereo
        {
           outbuffer[(i * outchannels) + 0] = inbuffer[(i * inchannels) + 0];
           outbuffer[(i * outchannels) + 1] = inbuffer[(i * inchannels) + 1];
        }
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Handle converting from one channel layout to another

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWASAPI::cleanUpEnumeration()
{
    int i;
    // Clean up any driver enumeration (may have enumerated once already)
    for (i = 0; i < FMOD_OUTPUT_MAXDRIVERS; i++)
    {
        SAFE_FREE(mRenderDrivers[i].name);
        CoTaskMemFree(mRenderDrivers[i].id);
        mRenderDrivers[i].id = 0;
    }
    for (i = 0; i < FMOD_OUTPUT_MAXDRIVERS; i++)
    {
        SAFE_FREE(mCaptureDrivers[i].name);
        CoTaskMemFree(mCaptureDrivers[i].id);
        mCaptureDrivers[i].id = 0;
    }
    mNumRenderDrivers = 0;
    mNumCaptureDrivers = 0;

    return FMOD_OK;
}

#ifdef FMOD_SUPPORT_RECORDING
/*
[
	[DESCRIPTION]
    Return the number of attached audio input devices 

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWASAPI::recordGetNumDrivers(int *numdrivers)
{
    if (!numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mEnumerated)
    {
        FMOD_RESULT result = FMOD_OK;

        result = enumerate();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    *numdrivers = mNumCaptureDrivers;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Return the name of an attached input device given its index

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x00938640
FMOD_RESULT OutputWASAPI::recordGetDriverInfo(int id, char *name, int namelen, FMOD_GUID *guid)
{
    if (!mEnumerated)
    {
        FMOD_RESULT result = FMOD_OK;

        result = enumerate();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (id < 0 || id >= mNumCaptureDrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (name && namelen >= 1)
    {
        short driverName[FMOD_STRING_MAXNAMELEN] = {0};

        FMOD_strncpyW(driverName, mCaptureDrivers[id].name, FMOD_STRING_MAXNAMELEN - 1);
        FMOD_wtoa(driverName);

        FMOD_strncpy(name, (char *)driverName, namelen - 1);
        name[namelen - 1] = 0;
    }

    if (guid)
    {
        parseUUIDString(mCaptureDrivers[id].id, guid);
    }    

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Return the name of an attached input device given its index

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWASAPI::recordGetDriverInfoW(int id, short *name, int namelen, FMOD_GUID *guid)
{
    if (!mEnumerated)
    {
        FMOD_RESULT result = FMOD_OK;

        result = enumerate();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (id < 0 || id >= mNumCaptureDrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (name && namelen >= 1)
    {
        FMOD_strncpyW(name, mCaptureDrivers[id].name, namelen - 1);
        name[namelen - 1] = 0;
    }

    if (guid)
    {
        parseUUIDString(mCaptureDrivers[id].id, guid);
    }    

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Determine capabilities of the device specified

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]
    Currently only shared mode caps are reported as the user doesn't specify that they want
    excusive mode until after Init

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x009386A0
FMOD_RESULT OutputWASAPI::recordGetDriverCaps(int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency)
{
    FMOD_RESULT           fResult            = FMOD_OK;
    HRESULT               hResult            = S_OK;
    IMMDeviceEnumerator  *pEnumerator        = NULL;
    IMMDevice            *pDevice            = NULL;
    IAudioClient         *pAudioClient       = NULL;
    WAVE_FORMATEX        *pWindowsMixFormat  = NULL;  
    
    // Enumerate devices if haven't already
    if (!mEnumerated)
    {
        fResult = enumerate();
        if (fResult != FMOD_OK)
        {
            return fResult;
        }
    }

    // Get an enumerator to collect endpoint information
    hResult = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    // Get the selected driver
    hResult = pEnumerator->GetDevice(mCaptureDrivers[id].id, &pDevice);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);    

    // Create connection to audio engine
    hResult = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    // Get the control panel mix format details
    hResult = pAudioClient->GetMixFormat(&pWindowsMixFormat);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    /*
        Establish the min / max frequencies
        Note: Sample rate must be equal to mix format
    */
    *minfrequency = *maxfrequency = pWindowsMixFormat->nSamplesPerSec;

    /*
        Establish the caps
        Note: WASAPI supports any format (trivial conversion)
    */
    *caps = FMOD_CAPS_OUTPUT_FORMAT_PCMFLOAT | FMOD_CAPS_OUTPUT_FORMAT_PCM32 |
            FMOD_CAPS_OUTPUT_FORMAT_PCM24    | FMOD_CAPS_OUTPUT_FORMAT_PCM16 |
            FMOD_CAPS_OUTPUT_FORMAT_PCM8;

    // Multichannel support is tied to the mix format
    if (pWindowsMixFormat->nChannels > 2)
    {
        *caps |= FMOD_CAPS_OUTPUT_MULTICHANNEL;
    }

Exit:
    CoTaskMemFree(pWindowsMixFormat);
    SAFE_RELEASE(pAudioClient);
    SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pEnumerator);

    return fResult;
}


/*
[
	[DESCRIPTION]
    Start recording from the selected input source

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x009387C0
FMOD_RESULT OutputWASAPI::recordStart(FMOD_RECORDING_INFO *recordinfo, Sound *sound, bool loop)
{
    FMOD_RESULT                 fResult             = FMOD_OK;
    HRESULT                     hResult             = S_OK;
    BOOL                        bResult             = FALSE;
    SoundI                     *pSoundI             = (SoundI *)sound;
    IMMDeviceEnumerator        *pEnumerator         = NULL;
    IMMDeviceCollection        *pCollection         = NULL;
    IMMDevice                  *pDevice             = NULL;    
    WAVE_FORMATEX              *pWindowsMixFormat   = NULL;
    WAVE_FORMATEX              *pSimilarWaveFormat  = NULL;
    WAVE_FORMATEXTENSIBLE       waveFormat          = {0};
    REFERENCE_TIME              hnsBufferLengthTime = 0;
    REFERENCE_TIME              hnsPeriod           = 0;
    int                         bits                = 0;
    LARGE_INTEGER               eventStartTime      = {0};
    FMOD_WASAPIRecordMembers   *recordMembers       = NULL;

    if (!pSoundI || pSoundI->mLength < 1)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mEnumerated)
    {
        fResult = enumerate();
        EXIT_ON_CONDITION(fResult != FMOD_OK, fResult);
    }

    if (mNumCaptureDrivers == 0)
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "OutputWASAPI::recordStart", "No input sound devices found"));
        EXIT_ON_CONDITION(true, FMOD_ERR_OUTPUT_INIT);
    }

    recordinfo->mRecordPlatformSpecific = recordMembers = (FMOD_WASAPIRecordMembers *)FMOD_Object_Calloc(FMOD_WASAPIRecordMembers);
    EXIT_ON_CONDITION(recordMembers == NULL, FMOD_ERR_MEMORY);

    hResult = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void **)&pEnumerator);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    hResult = pEnumerator->GetDevice(mCaptureDrivers[recordinfo->mRecordId].id, &pDevice);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    hResult = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void **)&recordMembers->mCaptureAudioClient);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    hResult = recordMembers->mCaptureAudioClient->GetMixFormat(&pWindowsMixFormat);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);
    
    fResult = SoundI::getBitsFromFormat(pSoundI->mFormat, &bits);
    EXIT_ON_CONDITION(fResult != FMOD_OK, fResult);
  
    waveFormat.Format.wFormatTag            = WAVE_FORMAT_EXTENSIBLE;
    waveFormat.Format.nChannels             = pSoundI->mChannels;
    waveFormat.Format.wBitsPerSample        = bits;
    waveFormat.Format.nBlockAlign           = waveFormat.Format.nChannels * waveFormat.Format.wBitsPerSample / 8;
    waveFormat.Format.nSamplesPerSec        = (DWORD)pSoundI->mDefaultFrequency;
    waveFormat.Format.nAvgBytesPerSec	    = waveFormat.Format.nSamplesPerSec * waveFormat.Format.nBlockAlign;
    waveFormat.Format.cbSize                = 22;  // Designates extra data
    waveFormat.Samples.wValidBitsPerSample  = bits;
    FMOD_memcpy(&waveFormat.SubFormat, (pSoundI->mFormat == FMOD_SOUND_FORMAT_PCMFLOAT ? &KSDATAFORMAT_SUBTYPE_IEEE_FLOAT : &KSDATAFORMAT_SUBTYPE_PCM), sizeof(GUID));

    // See if the device can do the requested format (may work in exclusive mode)
    hResult = recordMembers->mCaptureAudioClient->IsFormatSupported((mExclusiveMode ? AUDCLNT_SHAREMODE_EXCLUSIVE : AUDCLNT_SHAREMODE_SHARED), (WAVE_FORMATEX *)&waveFormat, &pSimilarWaveFormat);
    if (hResult == AUDCLNT_E_UNSUPPORTED_FORMAT || hResult == S_FALSE)
    {
        // Usually the mix format number of channels must be used, but mono isn't normally provided so allow a simple conversion case from stereo to mono
        if (pSoundI->mChannels == 1 && pWindowsMixFormat->nChannels == 2)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::recordStart", "Cannot use requested number of channels natively, FMOD will remix\n"));
            waveFormat.Format.nChannels        = pWindowsMixFormat->nChannels;
            waveFormat.Format.nBlockAlign      = waveFormat.Format.nChannels * waveFormat.Format.wBitsPerSample / 8;
            waveFormat.Format.nAvgBytesPerSec  = waveFormat.Format.nSamplesPerSec * waveFormat.Format.nBlockAlign;
            recordMembers->mRecordStereoToMono = true;
        }

        // FMODs resampler will be used to deliver the required frequency, don't change pSoundI->mDefaultFrequency (FMOD will resample to this)
        if (pSoundI->mDefaultFrequency != pWindowsMixFormat->nSamplesPerSec)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::recordStart", "Cannot use requested record frequency natively, FMOD will resample\n"));
            waveFormat.Format.nSamplesPerSec   = pWindowsMixFormat->nSamplesPerSec;
            waveFormat.Format.nAvgBytesPerSec  = waveFormat.Format.nSamplesPerSec * waveFormat.Format.nBlockAlign;
        }
    }

    if (mExclusiveMode)
    {
        REFERENCE_TIME hnsRequestedDuration = 0;
        
        // Get the minimum duration for lowest latency
        hResult = recordMembers->mCaptureAudioClient->GetDevicePeriod(NULL, &hnsRequestedDuration);
        EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

        // Internally WASAPI will double buffer for exclusive mode
        hnsPeriod            = hnsRequestedDuration;
        hnsBufferLengthTime  = hnsRequestedDuration;    // Must equal period
    }
    else
    {
        REFERENCE_TIME hnsRequestedDuration = 0;
        
        // Get the default duration (fixed shared poll rate, should be 10ms)
        hResult = recordMembers->mCaptureAudioClient->GetDevicePeriod(&hnsRequestedDuration, NULL);
        EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

        hnsPeriod            = hnsRequestedDuration;    // Must be shared poll rate
        hnsBufferLengthTime  = RECORD_LENGTH_MS * MFTIMES_PER_MILLISEC;
    }

    // This time init for real, this may still format error if the mix format isnt supported in exclusive mode
    hResult = recordMembers->mCaptureAudioClient->Initialize((mExclusiveMode ? AUDCLNT_SHAREMODE_EXCLUSIVE : AUDCLNT_SHAREMODE_SHARED), 0, hnsBufferLengthTime, hnsPeriod, (WAVE_FORMATEX*)&waveFormat, NULL);
    EXIT_ON_CONDITION(hResult == AUDCLNT_E_UNSUPPORTED_FORMAT, FMOD_ERR_FORMAT);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    // Remember the hardware frequency so FMOD can resample to pSoundI->mDefaultFrequency if required
    recordinfo->mRecordRate = waveFormat.Format.nSamplesPerSec;
    // Set the hardware format to the desired format, WASAPI handles format conversion, not FMOD
    recordinfo->mRecordFormat = pSoundI->mFormat;

    hResult = recordMembers->mCaptureAudioClient->GetBufferSize(&recordMembers->mCaptureBufferLength);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    hResult = recordMembers->mCaptureAudioClient->GetService(IID_IAudioCaptureClient, (void **)&recordMembers->mCaptureClient);
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

    // Setup and allocate the record buffer
    recordinfo->mRecordBufferLength     = (RECORD_LENGTH_MS * recordinfo->mRecordRate) / 1000;
    recordMembers->mRecordWritePosition = 0;
    recordMembers->mRecordBlockAlign    = pSoundI->mChannels * (waveFormat.Format.wBitsPerSample / 8);
    recordMembers->mRecordBuffer = (BYTE *)FMOD_Memory_Calloc(recordinfo->mRecordBufferLength * recordMembers->mRecordBlockAlign);
    EXIT_ON_CONDITION(recordMembers->mRecordBuffer == NULL, FMOD_ERR_MEMORY);

    if (!mRecordInitialised)
    {
        // Create and set an event handle to wait on in the record thread
        eventStartTime.QuadPart = -10000;   // Start the timer 1ms after setting it
        mRecordEventHandle = CreateWaitableTimer(NULL, FALSE, NULL);
        EXIT_ON_CONDITION(mRecordEventHandle == NULL, FMOD_ERR_OUTPUT_INIT);
        bResult = SetWaitableTimer(mRecordEventHandle, &eventStartTime, (int)(hnsPeriod / MFTIMES_PER_MILLISEC), NULL, NULL, FALSE);
        EXIT_ON_CONDITION(bResult == 0, FMOD_ERR_OUTPUT_INIT);

        // Start the record thread (waits on timer event handle)
        fResult = mRecordThread.initThread("FMOD record thread", recordThreadCallback, this, Thread::PRIORITY_NORMAL, 0, 0, false, 0, mSystem);
        EXIT_ON_CONDITION(fResult != FMOD_OK, fResult);

        mRecordInitialised = true;
    }

    hResult = recordMembers->mCaptureAudioClient->Start();
    EXIT_ON_CONDITION(FAILED(hResult), FMOD_ERR_OUTPUT_INIT);

Exit:
    CoTaskMemFree(pSimilarWaveFormat);
    CoTaskMemFree(pWindowsMixFormat);
    SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pEnumerator);

    return fResult;
}


/*
[
	[DESCRIPTION]
    Stop the recording of an input source

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x00938C60
FMOD_RESULT OutputWASAPI::recordStop(FMOD_RECORDING_INFO *recordinfo)
{
    FMOD_RESULT fResult = FMOD_OK;
    HRESULT     hResult = S_OK;

    if (recordinfo)
    {
        FMOD_WASAPIRecordMembers *recordMembers = (FMOD_WASAPIRecordMembers *)recordinfo->mRecordPlatformSpecific;

        hResult = recordMembers->mCaptureAudioClient->Stop();
        if (FAILED(hResult))
        {
            return FMOD_ERR_OUTPUT_DRIVERCALL;
        }

        SAFE_RELEASE(recordMembers->mCaptureClient);
        SAFE_RELEASE(recordMembers->mCaptureAudioClient);
        SAFE_FREE(recordMembers->mRecordBuffer);
        SAFE_FREE(recordinfo->mRecordPlatformSpecific);
    }

    if (mRecordNumActive == 0)
    {
        fResult = mRecordThread.closeThread();
        CHECK_RESULT(fResult);

        CloseHandle(mRecordEventHandle);
        mRecordEventHandle = 0;

        mRecordThreadElevated = false;
        mRecordInitialised = false;
    }

    return FMOD_OK;
}


/*
[
    [DESCRIPTION]
    Function to populate the ring buffer with recorded data from the hardware

    [PARAMETERS]

    [RETURN_VALUE]
    FMOD_OK

    [REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x00938440
FMOD_RESULT OutputWASAPI::recordUpdate()
{
    FMOD_RESULT fResult = FMOD_OK;
    DWORD       dResult = 0;

    if (!mRecordNumActive)
    {
        return FMOD_OK;
    }

    if (!mRecordThreadElevated)
    {
        FMOD_OS_LIBRARY *avrtHandle = NULL;

        fResult = FMOD_OS_Library_Load("avrt.dll", &avrtHandle);
        if (fResult == FMOD_OK)
        {
            HANDLE (WINAPI *setThreadTask)(LPCTSTR, LPDWORD);

            fResult = FMOD_OS_Library_GetProcAddress(avrtHandle, "AvSetMmThreadCharacteristicsA", (void **)&setThreadTask);
            if (fResult == FMOD_OK)
            {
                DWORD taskIndex = 0;

                setThreadTask(mExclusiveMode ? "Pro Audio" : "Audio", &taskIndex);
                FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::recordUpdate", "Record thread set to '%s' characteristic\n", mExclusiveMode ? "Pro Audio" : "Audio"));
            }

            FMOD_OS_Library_Free(avrtHandle);
        }

        // Thread is elevated or the OS doesn't support elevation, either way don't try again
        mRecordThreadElevated = true;
    }

    dResult = WaitForSingleObject(mRecordEventHandle, 2000);
    if (dResult != WAIT_OBJECT_0)
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::recordUpdate", "Record event timer not responding...\n"));
        return FMOD_ERR_OUTPUT_DRIVERCALL;
    }

    {
        FMOD_RECORDING_INFO    *current = NULL;
        LocalCriticalSection    recordInfoCrit(mRecordInfoCrit, true);

        current = SAFE_CAST(FMOD_RECORDING_INFO, mRecordInfoHead.getNext());
        while (current != &mRecordInfoHead)
        {
            HRESULT                     hResult         = S_OK;
            UINT32                      nextPacketSize  = 0;
            FMOD_RECORDING_INFO        *next            = SAFE_CAST(FMOD_RECORDING_INFO, current->getNext());
            FMOD_WASAPIRecordMembers   *recordMembers   = (FMOD_WASAPIRecordMembers *)current->mRecordPlatformSpecific;

            // Stop querying the hardware if we already know it's finish / unplugged, system update will stop it
            if (current->mRecordFinished)
            {
                current = next;
                continue;
            }

            // Find out if there is a packet waiting, size of 0 indicates no packet
            hResult = recordMembers->mCaptureClient->GetNextPacketSize(&nextPacketSize);
            if (hResult == AUDCLNT_E_DEVICE_INVALIDATED)
            {
                FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::recordUpdate", "Recording device has been unplugged, aborting capture.\n"));
                
                // Mark this recording device for stopping in system update
                current->mRecordFinished = true;
                current = next;

                continue;
            }
            else if (FAILED(hResult))
            {
                FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::recordUpdate", "Cannot determine buffer usage, aborting capture.\n"));
                return FMOD_ERR_OUTPUT_DRIVERCALL;
            }

            if (nextPacketSize == recordMembers->mCaptureBufferLength)
            {
                FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "OutputWASAPI::recordUpdate", "Overrun detected in WASAPI input buffer!\n"));
            }

            while (nextPacketSize)
            {
                UINT32  framesInPacket      = 0;
                BYTE   *pPacketData         = NULL;
                DWORD   flags               = 0;
                int     recordWritePosition = recordMembers->mRecordWritePosition;
                BYTE   *pRecordPosition     = recordMembers->mRecordBuffer + (recordMembers->mRecordWritePosition * recordMembers->mRecordBlockAlign);

                hResult = recordMembers->mCaptureClient->GetBuffer(&pPacketData, &framesInPacket, &flags, NULL, NULL);
                if (FAILED(hResult))
                {
                    return FMOD_ERR_OUTPUT_DRIVERCALL;
                }

                if (recordMembers->mRecordStereoToMono)
                {
                    int bytesPerSample = recordMembers->mRecordBlockAlign;  // Since we know the target buffer is mono, blockAlign == bytesPerSample
                    
                    for (unsigned int i = 0; i <= framesInPacket; i++)
                    {
                        FMOD_memcpy(pRecordPosition, pPacketData, bytesPerSample);
                        
                        pPacketData      += (bytesPerSample * 2);
                        pRecordPosition  += (bytesPerSample * 1);

                        if (pRecordPosition == (recordMembers->mRecordBuffer + (current->mRecordBufferLength * recordMembers->mRecordBlockAlign)))
                        {
                            pRecordPosition = recordMembers->mRecordBuffer;
                        }
                    }
                }
                else
                {
                    // If we dont have to wrap the packet around the ring buffer, copy directly
                    if (recordWritePosition + framesInPacket <= current->mRecordBufferLength)
	                {
                        FMOD_memcpy(pRecordPosition, pPacketData, framesInPacket * recordMembers->mRecordBlockAlign);        
	                }
                    // Othewise copy to the end of the buffer, then continue at the start
                    else
	                {
		                int endFrameLength    = current->mRecordBufferLength - recordWritePosition;
                        int startFrameLength  = framesInPacket - endFrameLength;

                        FMOD_memcpy(pRecordPosition, pPacketData, endFrameLength * recordMembers->mRecordBlockAlign);
                        
                        pPacketData += endFrameLength * recordMembers->mRecordBlockAlign;
                        pRecordPosition = recordMembers->mRecordBuffer;
                        
                        FMOD_memcpy(pRecordPosition, pPacketData, startFrameLength * recordMembers->mRecordBlockAlign);
	                }
                }

                hResult = recordMembers->mCaptureClient->ReleaseBuffer(framesInPacket);
                if (FAILED(hResult))
                {
                    return FMOD_ERR_OUTPUT_DRIVERCALL;
                }

                hResult = recordMembers->mCaptureClient->GetNextPacketSize(&nextPacketSize);
                if (FAILED(hResult))
                {
                    return FMOD_ERR_OUTPUT_DRIVERCALL;
                }

                recordWritePosition += framesInPacket;
                recordWritePosition %= current->mRecordBufferLength;
                recordMembers->mRecordWritePosition = recordWritePosition;
            }

            current = next;
        }
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Return a position value in samples so that FMOD knows how much has been recorded

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWASAPI::recordGetPosition(FMOD_RECORDING_INFO *recordinfo, unsigned int *pcm)
{
    FMOD_WASAPIRecordMembers *recordMembers = (FMOD_WASAPIRecordMembers *)recordinfo->mRecordPlatformSpecific;

    *pcm = recordMembers->mRecordWritePosition;
    
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Lock the requested number of bytes

	[PARAMETERS]

	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
// OFFSET: 0x00938D10
FMOD_RESULT OutputWASAPI::recordLock(FMOD_RECORDING_INFO *recordinfo, unsigned int offset, unsigned int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2)
{  
    FMOD_WASAPIRecordMembers   *recordMembers   = (FMOD_WASAPIRecordMembers *)recordinfo->mRecordPlatformSpecific;
    unsigned int                lenbytes        = recordinfo->mRecordBufferLength * recordMembers->mRecordBlockAlign;

    // Cant lock more than the sample length
	if (length > lenbytes) 
    {
		length = lenbytes;
    }

	// Ensure parameters are valid
    if (offset >= lenbytes || offset < 0 || length < 0) 
	{
		*ptr1 = *ptr2 = NULL;
		*len1 = *len2 = 0;
		return FMOD_ERR_INVALID_PARAM;
	}

    // If it is not a wrapping lock just fill out ptr1
	if (offset + length <= lenbytes)
	{
        *ptr1 = recordMembers->mRecordBuffer + offset;
		*len1 = length;
		*ptr2 = NULL;
		*len2 = 0;
	}
    // Otherwise return wrapped pointers in ptr1 and ptr2
	else
	{
		*ptr1 = recordMembers->mRecordBuffer + offset;
		*len1 = lenbytes - offset;
		*ptr2 = recordMembers->mRecordBuffer;
		*len2 = length - (lenbytes - offset);
	}

    return FMOD_OK;
}
#endif  /* FMOD_SUPPORT_RECORDING */

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
FMOD_RESULT OutputWASAPI::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(false, FMOD_MEMBITS_OUTPUT, sizeof(*this));

    for (int i = 0; i < mNumRenderDrivers; i++)
    {
        if (mRenderDrivers[i].name)
        {
            tracker->add(false, FMOD_MEMBITS_STRING, FMOD_strlenW((mRenderDrivers[i].name) + 1) * sizeof(short));
        }
    }
    
    for (int j = 0; j < mNumCaptureDrivers; j++)
    {
        if (mCaptureDrivers[j].name)
        {
            tracker->add(false, FMOD_MEMBITS_STRING, FMOD_strlenW((mCaptureDrivers[j].name) + 1) * sizeof(short));
        }
    }

    if (mMixerResamplerDSP)
    {
        CHECK_RESULT(mMixerResamplerDSP->getMemoryUsed(tracker));
    }

    if (mMixerChannelConversionBuffer)
    {
        tracker->add(false, FMOD_MEMBITS_OUTPUT, mMixerChannelConversionBufferSizeBytes);
    }

    if (mMixerFormatConversionBuffer)
    {
        tracker->add(false, FMOD_MEMBITS_OUTPUT, mMixerFormatConversionBufferSizeBytes);
    }

    if (mMixerBuffer)
    {
        tracker->add(false, FMOD_MEMBITS_OUTPUT, mMixerBufferLength * mRenderBlockAlign);
    }

    CHECK_RESULT(Output::getMemoryUsedImpl(tracker));

    return FMOD_OK;
}
#endif  /* FMOD_SUPPORT_MEMORYTRACKER */

/*
    ==============================================================================================================

    CALLBACK INTERFACE

    ==============================================================================================================
*/

// OFFSET: 0x00938DD0
FMOD_RESULT F_CALLBACK OutputWASAPI::getNumDriversCallback(FMOD_OUTPUT_STATE *output, int *numdrivers)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->getNumDrivers(numdrivers);
}


FMOD_RESULT F_CALLBACK OutputWASAPI::getDriverInfoCallback(FMOD_OUTPUT_STATE *output, int id, char *name, int namelen, FMOD_GUID *guid)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->getDriverInfo(id, name, namelen, guid);
}


FMOD_RESULT F_CALLBACK OutputWASAPI::getDriverInfoWCallback(FMOD_OUTPUT_STATE *output, int id, short *name, int namelen, FMOD_GUID *guid)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->getDriverInfoW(id, name, namelen, guid);
}


// OFFSET: 0x00938E40
FMOD_RESULT F_CALLBACK OutputWASAPI::getDriverCapsExCallback(FMOD_OUTPUT_STATE *output, int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency, FMOD_SPEAKERMODE *controlpanelspeakermode)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->getDriverCapsEx(id, caps, minfrequency, maxfrequency, controlpanelspeakermode);
}


// OFFSET: 0x00938E70
FMOD_RESULT F_CALLBACK OutputWASAPI::initExCallback(FMOD_OUTPUT_STATE *output, int selecteddriver, FMOD_INITFLAGS flags, int *outputrate, int outputchannels, FMOD_SOUND_FORMAT *outputformat, FMOD_SPEAKERMODE *speakermode, int dspbufferlength, int dspnumbuffers, int max2dchannels, int max3dchannels, void *extradriverdata)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->initEx(selecteddriver, flags, outputrate, outputchannels, outputformat, speakermode, dspbufferlength, dspnumbuffers, max2dchannels, max3dchannels, extradriverdata);
}


// OFFSET: 0x00938EC0
FMOD_RESULT F_CALLBACK OutputWASAPI::closeCallback(FMOD_OUTPUT_STATE *output)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->close();
}


// OFFSET: 0x00938EF0
FMOD_RESULT F_CALLBACK OutputWASAPI::startCallback(FMOD_OUTPUT_STATE *output)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->start();
}


// OFFSET: 0x00938F30
FMOD_RESULT F_CALLBACK OutputWASAPI::stopCallback(FMOD_OUTPUT_STATE *output)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->stop();
}


FMOD_RESULT F_CALLBACK OutputWASAPI::getHandleCallback(FMOD_OUTPUT_STATE *output, void **handle)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->getHandle(handle);
}


void OutputWASAPI::feederThreadCallback(void *data)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)data;

    wasapi->feederUpdate();
}


void OutputWASAPI::mixerThreadCallback(void *data)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)data;

    wasapi->mixerUpdate();
}


FMOD_RESULT F_CALLBACK OutputWASAPI::mixerResampleReadCallback(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPResampler  *resampler  = (DSPResampler *)dsp_state;
    OutputWASAPI  *wasapi     = NULL;
        
    resampler->getUserData((void **)&wasapi);

    return wasapi->mixerResampleRead(inbuffer, outbuffer, length, inchannels, outchannels);
}


#ifdef FMOD_SUPPORT_RECORDING
// OFFSET: 0x00938FE0
FMOD_RESULT F_CALLBACK OutputWASAPI::recordGetNumDriversCallback(FMOD_OUTPUT_STATE *output, int *numdrivers)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->recordGetNumDrivers(numdrivers);
}


// OFFSET: 0x00939020
FMOD_RESULT F_CALLBACK OutputWASAPI::recordGetDriverInfoCallback(FMOD_OUTPUT_STATE *output, int id, char *name, int namelen, FMOD_GUID *guid)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->recordGetDriverInfo(id, name, namelen, guid);
}


FMOD_RESULT F_CALLBACK OutputWASAPI::recordGetDriverInfoWCallback(FMOD_OUTPUT_STATE *output, int id, short *name, int namelen, FMOD_GUID *guid)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->recordGetDriverInfoW(id, name, namelen, guid);
}


// OFFSET: 0x00939050
FMOD_RESULT F_CALLBACK OutputWASAPI::recordGetDriverCapsCallback(FMOD_OUTPUT_STATE *output, int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->recordGetDriverCaps(id, caps, minfrequency, maxfrequency);
}


// OFFSET: 0x00939080
FMOD_RESULT F_CALLBACK OutputWASAPI::recordStartCallback(FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo, FMOD_SOUND *sound, int loop)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->recordStart(recordinfo, (Sound *)sound, loop ? true : false);
}


// OFFSET: 0x009390B0
FMOD_RESULT F_CALLBACK OutputWASAPI::recordStopCallback(FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->recordStop(recordinfo);
}


void OutputWASAPI::recordThreadCallback(void *data)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)data;

    wasapi->recordUpdate();
}


// OFFSET: 0x00915B60
FMOD_RESULT F_CALLBACK OutputWASAPI::recordGetPositionCallback(FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo, unsigned int *pcm)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->recordGetPosition(recordinfo, pcm);
}


// OFFSET: 0x009390E0
FMOD_RESULT F_CALLBACK OutputWASAPI::recordLockCallback(FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo, unsigned int offset, unsigned int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->recordLock(recordinfo, offset, length, ptr1, ptr2, len1, len2);
}
#endif  /* FMOD_SUPPORT_RECORDING */

#ifdef FMOD_SUPPORT_MEMORYTRACKER
FMOD_RESULT F_CALLBACK OutputWASAPI::getMemoryUsedCallback(FMOD_OUTPUT_STATE *output, MemoryTracker *tracker)
{
    OutputWASAPI *wasapi = (OutputWASAPI *)output;

    return wasapi->getMemoryUsed(tracker);
}
#endif  /* FMOD_SUPPORT_MEMORYTRACKER */

}       /* namespace FMOD */

#endif  /* FMOD_SUPPORT_WASAPI */
