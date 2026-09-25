#include "fmod_settings.h"

#include "fmod_pluginfactory.h"
#include "fmod_codeci.h"
#ifdef FMOD_SUPPORT_DSPCODEC
#include "fmod_dsp_codec.h"
#endif
#include "fmod_dsp_oscillator.h"
#include "fmod_dsp_resampler.h"
#include "fmod_dsp_resampler_multiinput.h"
#include "fmod_dsp_soundcard.h"
#ifdef FMOD_SUPPORT_VSTPLUGIN
#include "fmod_dsp_vstplugin.h"
#endif
#include "fmod_dsp_wavetable.h"
#ifdef FMOD_SUPPORT_WINAMPPLUGIN
#include "fmod_dsp_winampplugin.h"
#endif
#ifdef FMOD_SUPPORT_OPENAL
#include "fmod_output_openal.h"
#endif
#include "fmod_output_polled.h"
#include "fmod_memory.h"
#include "fmod_file.h"
#include "fmod_os_misc.h"
#include "fmod_systemi.h"

#ifdef PLATFORM_PS3
#include "fmod_codec_mpeg_pic.h"
#include "fmod_common_spu.h"
#include "fmod_output_ps3.h"
#endif

#include <stdio.h>

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
// OFFSET: 0x008F4130
PluginFactory::PluginFactory()
{
    mSystem = 0;
    mCurrentPluginHandle = 1;
    FMOD_memset(mPluginPath, 0, FMOD_STRING_MAXPATHLEN);

    mDSPHead.initNode();
    mCodecHead.initNode();
    mOutputHead.initNode();
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
// OFFSET: 0x008F4190
FMOD_RESULT PluginFactory::release()
{
    FMOD_RESULT result;
    int num, count;

    /*
        Cleanup codecs
    */
    result = getNumCodecs(&num);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < num; count++)
    {
        unsigned int handle;

        getCodecHandle(0, &handle);

        result = unloadPlugin(handle);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        Cleanup DSPs
    */
    result = getNumDSPs(&num);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < num; count++)
    {
        unsigned int handle;

        getDSPHandle(0, &handle);

        result = unloadPlugin(handle);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        Cleanup Outputs
    */
    result = getNumOutputs(&num);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < num; count++)
    {
        unsigned int handle;

        getOutputHandle(0, &handle);

        result = unloadPlugin(handle);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    FMOD_Memory_Free(this);

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
// OFFSET: 0x008F38D0
FMOD_RESULT PluginFactory::setSystem(SystemI *system)  
{ 
    mSystem = system;  
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
FMOD_RESULT PluginFactory::getSystem(SystemI **system) 
{ 
    *system = mSystem; 
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
// OFFSET: 0x008F38F0
FMOD_RESULT PluginFactory::setPluginPath(const char *path)
{
    if (FMOD_strlen(path) >= FMOD_STRING_MAXPATHLEN)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_strncpy(mPluginPath, path, FMOD_STRING_MAXPATHLEN);

    return FMOD_OK;
}

#ifdef FMOD_SUPPORT_DLLS

FMOD_RESULT PluginFactory::tryLoadPlugin(const char *dllname, unsigned int *handle, bool calledinternally, unsigned int priority)
{
    FMOD_RESULT result = loadPlugin(dllname, handle, calledinternally, priority);

    if (result != FMOD_ERR_FILE_NOTFOUND && result != FMOD_ERR_FILE_BAD)
    {
        CHECK_RESULT(result);
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
FMOD_RESULT PluginFactory::loadPlugin(const char *dllname, unsigned int *handle, bool calledinternally, unsigned int priority)
{
    FMOD_RESULT                 result;
    FMOD_OS_LIBRARY            *dllhandle;
    FMOD_CODEC_DESCRIPTION     *(F_CALLBACK *getcodecdesc   )() = 0;
    FMOD_CODEC_DESCRIPTION_EX  *(F_CALLBACK *getcodecdescex )() = 0;
    FMOD_DSP_DESCRIPTION       *(F_CALLBACK *getdspdesc     )() = 0;
    FMOD_DSP_DESCRIPTION_EX    *(F_CALLBACK *getdspdescex   )() = 0;
    FMOD_OUTPUT_DESCRIPTION    *(F_CALLBACK *getoutputdesc  )() = 0;
    FMOD_OUTPUT_DESCRIPTION_EX *(F_CALLBACK *getoutputdescex)() = 0;
#ifdef FMOD_SUPPORT_VSTPLUGIN
    AEffect                    *(VSTCALLBACK *VSTmain)(audioMasterCallback) = 0;
#endif
#ifdef FMOD_SUPPORT_WINAMPPLUGIN
    winampDSPHeader            *( *winampGetHeader)() = 0;
#endif
    char                        path[FMOD_STRING_MAXPATHLEN];
    const char                 *beginning;
    const char                 *end;
    char                        symbol[50];

    FMOD_strncpy(path, mPluginPath, FMOD_STRING_MAXPATHLEN);
    if (FMOD_strlen(path) && path[FMOD_strlen(path) - 1] != '\\' && path[FMOD_strlen(path) - 1] != '/')
    {
        FMOD_strcat(path, "/");
    }
    FMOD_strcat(path, dllname);

    #if defined(PLATFORM_WINDOWS64) || defined(PLATFORM_LINUX64)
    if (calledinternally)
    {
        FMOD_strcat(path, "64");
    }
    #endif

    if (FMOD_strlen(path))
    {
        #if defined(PLATFORM_WINDOWS)
        if (FMOD_strncmp(&path[FMOD_strlen(path)-4], ".dll", 4))
        {
            FMOD_strcat(path, ".dll");
        }
        #elif defined(PLATFORM_MAC)
        if (FMOD_strncmp(&path[FMOD_strlen(path)-6], ".dylib", 6))
        {
            /*
                Could be a VST / Bundle
            */
            if ((FMOD_strncmp(&path[FMOD_strlen(path)-4], ".vst", 4)) && (FMOD_strncmp(&path[FMOD_strlen(path)-7], ".bundle", 7)))
            {
                FMOD_strcat(path, ".dylib");
            }
        }
        #elif defined(PLATFORM_LINUX) || defined(PLATFORM_SOLARIS)
        if (FMOD_strncmp(&path[FMOD_strlen(path)-3], ".so", 3))
        {
            FMOD_strcat(path, ".so");
        }
        #endif
    }

    result = FMOD_OS_Library_Load(path, &dllhandle);
    if (result != FMOD_OK)
    {
        #if defined(PLATFORM_WINDOWS64) || defined(PLATFORM_LINUX64)
        if (calledinternally)
        {
            char dllname64[64]; /* No internal FMOD plugin names are more than 62 characters */
            FMOD_strcpy(dllname64, dllname);
            FMOD_strcat(dllname64, "64");
            result = FMOD_OS_Library_Load(dllname64, &dllhandle);    /* Try without the search path, in case they were actually using a relative path to the app */
        }
        #else
        result = FMOD_OS_Library_Load(dllname, &dllhandle);    /* Try without the search path, in case they were actually using a relative path to the app */
        #endif
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    #if defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS64)
    beginning = "";
    end       = "";
    #elif defined(PLATFORM_WINDOWS)
    beginning = "_";
    end       = "@0";
    #elif defined(PLATFORM_MAC_PPC)
    beginning = "_";
    end       = "";
    #else
    beginning = "";
    end       = "";
    #endif
    
    sprintf(symbol, "%sFMODGetCodecDescription%s", beginning, end); 

    result = FMOD_OS_Library_GetProcAddress(dllhandle, symbol, (void **)&getcodecdesc);
    if (result == FMOD_OK)
    {
        return registerCodec(getcodecdesc(), handle, priority);
    }

    sprintf(symbol, "%sFMODGetCodecDescriptionEx%s", beginning, end);
    result = FMOD_OS_Library_GetProcAddress(dllhandle, symbol, (void **)&getcodecdescex);
    if (result == FMOD_OK)
    {
        return registerCodec(getcodecdescex(), handle, priority);
    }

    sprintf(symbol, "%sFMODGetDSPDescription%s", beginning, end);
    result = FMOD_OS_Library_GetProcAddress(dllhandle, symbol, (void **)&getdspdesc);
    if (result == FMOD_OK)
    {
        return registerDSP(getdspdesc(), handle);
    }

    sprintf(symbol, "%sFMODGetDSPDescriptionEx%s", beginning, end);
    result = FMOD_OS_Library_GetProcAddress(dllhandle, symbol, (void **)&getdspdescex);
    if (result == FMOD_OK)
    {
        return registerDSP(getdspdescex(), handle);
    }

    sprintf(symbol, "%sFMODGetOutputDescription%s", beginning, end);
    result = FMOD_OS_Library_GetProcAddress(dllhandle, symbol, (void **)&getoutputdesc);
    if (result == FMOD_OK)
    {
        return registerOutput(getoutputdesc(), handle);
    }

    sprintf(symbol, "%sFMODGetOutputDescriptionEx%s", beginning, end);
    result = FMOD_OS_Library_GetProcAddress(dllhandle, symbol, (void **)&getoutputdescex);
    if (result == FMOD_OK)
    {
        return registerOutput(getoutputdescex(), handle);
    }

#ifdef FMOD_SUPPORT_VSTPLUGIN
    result = FMOD_OS_Library_GetProcAddress(dllhandle, "VSTPluginMain", (void **)&VSTmain);     // VST 2.4 and onward use VSTPluginMain
    if (result != FMOD_OK)
    {    
        #ifdef PLATFORM_MAC
        result = FMOD_OS_Library_GetProcAddress(dllhandle, "main_macho", (void **)&VSTmain);
        #else
        result = FMOD_OS_Library_GetProcAddress(dllhandle, "main", (void **)&VSTmain);
        #endif
    }
    if (result == FMOD_OK)
    {
        AEffect *effect;
        
        effect = VSTmain(DSPVSTPlugin::audioMasterCB);

        if (effect->magic == kEffectMagic)
        {
            struct ERect
            {
	            short top;
	            short left;
	            short bottom;
	            short right;
            } *rect = 0;
            FMOD_DSP_DESCRIPTION_EX *dspdesc;
            FMOD_DSP_PARAMETERDESC  *dspparams = (FMOD_DSP_PARAMETERDESC *)FMOD_Memory_Calloc(sizeof(FMOD_DSP_PARAMETERDESC) * effect->numParams);

            dspdesc           = DSPVSTPlugin::getDescriptionEx();
            dspdesc->mAEffect = effect;

            dspdesc->version  = effect->dispatcher(effect, effGetVendorVersion, 0, 0, NULL, 0);

            sprintf(dspdesc->name, "VST ");
            effect->dispatcher(effect, effGetEffectName, 0, 0, (void *)(dspdesc->name + 4), 0);
            effect->dispatcher(effect, effEditGetRect, 0, 0, &rect, 0);

            /*
                If this plugin has no name, derive one from its filename
            */
            if (FMOD_strlen(dspdesc->name) == 4)
            {
                int i;

                for (i=FMOD_strlen(dllname) - 1; i > 0; i--)
                {
                    if ((dllname[i] == '/') || (dllname[i] == '\\'))
                    {
                        i++;
                        break;
                    }
                }

                int   len = FMOD_strlen(&dllname[i]);
                char *p   = (char *)&dllname[i] + len - 4;
                if (p >= &dllname[i])
                {
                    if ((FMOD_tolower(p[0]) == '.') &&
                        (FMOD_tolower(p[1]) == 'd') &&
                        (FMOD_tolower(p[2]) == 'l') &&
                        (FMOD_tolower(p[3]) == 'l'))
                    {
                        len -= 4;
                    }
                }

                FMOD_strncat(dspdesc->name, &dllname[i], len);
            }

            if (rect)
            {
                dspdesc->configheight = rect->bottom - rect->top;
                dspdesc->configwidth  = rect->right  - rect->left;
            }

            /*
                Fill in dspparams
            */
            for (int count = 0; count < effect->numParams; count++)
            {
                dspparams[count].min         = 0.0f;
                dspparams[count].max         = 1.0f;
                dspparams[count].defaultval  = effect->getParameter(effect, count);
                dspparams[count].description = NULL;

                effect->dispatcher(effect, effGetParamName, count, 0, dspparams[count].name, 0);
                effect->dispatcher(effect, effGetParamLabel, count, 0, dspparams[count].label, 0);
            }
            dspdesc->paramdesc      = dspparams;
            dspdesc->numparameters  = effect->numParams;

            return registerDSP(dspdesc, handle);
        }
    }
#endif

#ifdef FMOD_SUPPORT_WINAMPPLUGIN
    result = FMOD_OS_Library_GetProcAddress(dllhandle, "winampDSPGetHeader2", (void **)&winampGetHeader);
    if (result == FMOD_OK)
    {
        winampDSPHeader         *header;

        header = winampGetHeader();

        /*
            Register each module as a seperate plugin
        */
        for (int count = 0; ; count++)
        {
            FMOD_DSP_DESCRIPTION_EX *dspdesc = NULL;
            winampDSPModule         *module  = NULL;

            module = header->getModule(count);
            if (!module)
            {
                break;
            }

            module->hDllInstance = dllhandle;

            dspdesc           = DSPWinampPlugin::getDescriptionEx();
            dspdesc->mAEffect = module;

            dspdesc->version  = header->version;

            sprintf(dspdesc->name, "WINAMP ");

            FMOD_strncpy((dspdesc->name + 7), module->description, 25);

            dspdesc->name[31] = '\0';

            /*
                If this plugin has no name, derive one from its filename
            */
            if (FMOD_strlen(dspdesc->name) == 7)
            {
                int i;

                for (i=FMOD_strlen(dllname) - 1; i > 0; i--)
                {
                    if ((dllname[i] == '/') || (dllname[i] == '\\'))
                    {
                        i++;
                        break;
                    }
                }

                int   len = FMOD_strlen(&dllname[i]);
                char *p   = (char *)&dllname[i] + len - 4;
                if (p >= &dllname[i])
                {
                    if ((FMOD_tolower(p[0]) == '.') &&
                        (FMOD_tolower(p[1]) == 'd') &&
                        (FMOD_tolower(p[2]) == 'l') &&
                        (FMOD_tolower(p[3]) == 'l'))
                    {
                        len -= 4;
                    }
                }

                FMOD_strncat(dspdesc->name, &dllname[i], len);
            }

            dspdesc->numparameters  = 0;

            result = registerDSP(dspdesc, handle);
        }
    }
#endif

    return result;
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
FMOD_RESULT PluginFactory::unloadPlugin(unsigned int handle)
{
    FMOD_RESULT result;
    FMOD_OUTPUT_DESCRIPTION_EX *output;
    FMOD_CODEC_DESCRIPTION_EX *codec;
    FMOD_DSP_DESCRIPTION_EX *dsp;

    result = getOutput(handle, &output);
    if (result == FMOD_OK)
    {
        #ifdef FMOD_SUPPORT_DLLS
        if (output->mModule)
        {
            FMOD_OS_Library_Free(output->mModule);
        }
        #endif
        output->removeNode();
        FMOD_Memory_Free(output);
        return FMOD_OK;
    }
    else if (result == FMOD_ERR_PLUGIN_MISSING)
    {
        result = getCodec(handle, &codec);
        if (result == FMOD_OK)
        {
            #ifdef FMOD_SUPPORT_DLLS
            if (codec->mModule)
            {
                FMOD_OS_Library_Free(codec->mModule);
            }
            #endif
            codec->removeNode();
            FMOD_Memory_Free(codec);
        }
        else if (result == FMOD_ERR_PLUGIN_MISSING)
        {
            result = getDSP(handle, &dsp);
            if (result == FMOD_OK)
            {
                #ifdef FMOD_SUPPORT_DLLS
                if (dsp->mAEffect)
                {
                    if (dsp->paramdesc)
                    {
                        FMOD_Memory_Free(dsp->paramdesc);
                    }
                }
                #endif

                #ifdef FMOD_SUPPORT_DLLS
                if (dsp->mModule)
                {
                    FMOD_OS_Library_Free(dsp->mModule);
                }
                #endif
                dsp->removeNode();
                FMOD_Memory_Free(dsp);
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
// OFFSET: 0x008F3930
FMOD_RESULT PluginFactory::registerCodec(FMOD_CODEC_DESCRIPTION_EX *description, unsigned int *handle, unsigned int priority)
{
    FMOD_CODEC_DESCRIPTION_EX *newdesc;

    if (!description)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    newdesc = FMOD_Object_Calloc(FMOD_CODEC_DESCRIPTION_EX);
    if (!newdesc)
    {
        return FMOD_ERR_MEMORY;
    }

    newdesc->name                    = description->name;
    newdesc->version                 = description->version;
    newdesc->timeunits               = description->timeunits;
    newdesc->defaultasstream         = description->defaultasstream;
    newdesc->open                    = description->open;
    newdesc->close                   = description->close;
    newdesc->read                    = description->read;
    newdesc->getlength               = description->getlength;
    newdesc->setposition             = description->setposition;
    newdesc->getposition             = description->getposition;
    newdesc->soundcreate             = description->soundcreate;
    newdesc->getwaveformat           = description->getwaveformat;
    newdesc->mType                   = description->mType;
    newdesc->mSize                   = description->mSize;
    newdesc->mModule                 = description->mModule;
    newdesc->reset                   = description->reset;
    newdesc->canpoint                = description->canpoint;
    newdesc->getmusicnumchannels     = description->getmusicnumchannels;
    newdesc->setmusicchannelvolume   = description->setmusicchannelvolume;
    newdesc->getmusicchannelvolume   = description->getmusicchannelvolume;
    newdesc->gethardwaremusicchannel = description->gethardwaremusicchannel;
    newdesc->update                  = description->update;
    newdesc->getmemoryused           = description->getmemoryused;
    newdesc->mHandle                 = mCurrentPluginHandle++;
    newdesc->init                    = description->init;
 
    newdesc->addAt(&mCodecHead, &mCodecHead, priority);
    
    if (handle)
    {
        *handle = newdesc->mHandle;
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
FMOD_RESULT PluginFactory::registerCodec(FMOD_CODEC_DESCRIPTION *description, unsigned int *handle, unsigned int priority)
{
    FMOD_CODEC_DESCRIPTION_EX *newdesc;

    if (!description)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    newdesc = FMOD_Object_Calloc(FMOD_CODEC_DESCRIPTION_EX);
    if (!newdesc)
    {
        return FMOD_ERR_MEMORY;
    }

    newdesc->name                    = description->name;
    newdesc->version                 = description->version;
    newdesc->timeunits               = description->timeunits;
    newdesc->defaultasstream         = description->defaultasstream;
    newdesc->open                    = description->open;
    newdesc->close                   = description->close;
    newdesc->read                    = description->read;
    newdesc->getlength               = description->getlength;
    newdesc->setposition             = description->setposition;
    newdesc->getposition             = description->getposition;
    newdesc->soundcreate             = description->soundcreate;
    newdesc->getwaveformat           = description->getwaveformat;
    newdesc->mType                   = FMOD_SOUND_TYPE_UNKNOWN;
    newdesc->mSize                   = sizeof(Codec);
    newdesc->mModule                 = 0;
    newdesc->reset                   = 0;
    newdesc->canpoint                = 0;
    newdesc->getmusicnumchannels     = 0;
    newdesc->setmusicchannelvolume   = 0;
    newdesc->getmusicchannelvolume   = 0;
    newdesc->gethardwaremusicchannel = 0;
    newdesc->update                  = 0;
    newdesc->getmemoryused           = 0;
    newdesc->mHandle                 = mCurrentPluginHandle++;
 
    newdesc->addAt(&mCodecHead, &mCodecHead, priority);
    
    if (handle)
    {
        *handle = newdesc->mHandle;
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
// OFFSET: 0x008F3A30
FMOD_RESULT PluginFactory::registerDSP(FMOD_DSP_DESCRIPTION_EX *description, unsigned int *handle)
{
    FMOD_DSP_DESCRIPTION_EX *newdesc;

    if (!description)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    newdesc = FMOD_Object_Calloc(FMOD_DSP_DESCRIPTION_EX);
    if (!newdesc)
    {
        return FMOD_ERR_MEMORY;
    }

    FMOD_strcpy(newdesc->name, description->name);
    newdesc->version              = description->version;
    newdesc->channels             = description->channels;
    newdesc->create               = description->create;
    newdesc->release              = description->release;
    newdesc->reset                = description->reset;
    newdesc->read                 = description->read;
    newdesc->setposition          = description->setposition;
                                  
    newdesc->numparameters         = description->numparameters;
    newdesc->paramdesc             = description->paramdesc;
    newdesc->setparameter          = description->setparameter;
    newdesc->getparameter          = description->getparameter;
    newdesc->config                = description->config;
    newdesc->configwidth           = description->configwidth;
    newdesc->configheight          = description->configheight;
    newdesc->userdata              = description->userdata;
                                   
    newdesc->mType                 = description->mType;
    newdesc->mCategory             = description->mCategory;
    newdesc->mSize                 = description->mSize;
#ifdef FMOD_SUPPORT_DLLS
    newdesc->mModule               = description->mModule;
    newdesc->mAEffect              = description->mAEffect;
#endif
    newdesc->mFormat               = description->mFormat;
    newdesc->mResamplerBlockLength = description->mResamplerBlockLength;
    newdesc->getmemoryused         = description->getmemoryused;
    newdesc->update                = description->update;
    newdesc->mHandle               = mCurrentPluginHandle++;

#ifdef FMOD_SUPPORT_DLLS
    newdesc->configidle            = description->configidle;
#endif

    newdesc->addBefore(&mDSPHead);

    if (handle)
    {
        *handle = newdesc->mHandle;
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
FMOD_RESULT PluginFactory::registerDSP(FMOD_DSP_DESCRIPTION *description, unsigned int *handle)
{
    FMOD_DSP_DESCRIPTION_EX *newdesc;

    if (!description)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    newdesc = FMOD_Object_Calloc(FMOD_DSP_DESCRIPTION_EX);
    if (!newdesc)
    {
        return FMOD_ERR_MEMORY;
    }

    FMOD_strcpy(newdesc->name, description->name);
    newdesc->version               = description->version;
    newdesc->channels              = description->channels;
    newdesc->create                = description->create;
    newdesc->release               = description->release;
    newdesc->reset                 = description->reset;
    newdesc->read                  = description->read;
    newdesc->setposition           = description->setposition;
                                   
    newdesc->numparameters         = description->numparameters;
    newdesc->paramdesc             = description->paramdesc;
    newdesc->setparameter          = description->setparameter;
    newdesc->getparameter          = description->getparameter;
    newdesc->config                = description->config;
    newdesc->configwidth           = description->configwidth;
    newdesc->configheight          = description->configheight;
    newdesc->userdata              = description->userdata;
                                   
    newdesc->mType                 = FMOD_DSP_TYPE_UNKNOWN;
    newdesc->mSize                 = sizeof(DSPI);
    newdesc->mFormat               = FMOD_SOUND_FORMAT_PCMFLOAT;
    newdesc->mHandle               = mCurrentPluginHandle++;

    newdesc->addBefore(&mDSPHead);

    if (handle)
    {
        *handle = newdesc->mHandle;
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
// OFFSET: 0x008F3B60
FMOD_RESULT PluginFactory::registerOutput(FMOD_OUTPUT_DESCRIPTION_EX *description, unsigned int *handle)
{
    FMOD_OUTPUT_DESCRIPTION_EX *newdesc;

    if (!description)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    newdesc = FMOD_Object_Calloc(FMOD_OUTPUT_DESCRIPTION_EX);
    if (!newdesc)
    {
        return FMOD_ERR_MEMORY;
    }

    newdesc->name                   = description->name;
    newdesc->version                = description->version;
    newdesc->polling                = description->polling;
    newdesc->getnumdrivers          = description->getnumdrivers;
    newdesc->getdrivername          = description->getdrivername;
    newdesc->getdrivercaps          = description->getdrivercaps;
    newdesc->init                   = description->init;
    newdesc->close                  = description->close;
    newdesc->start                  = description->start;
    newdesc->stop                   = description->stop;
    newdesc->update                 = description->update;
    newdesc->gethandle              = description->gethandle;
    newdesc->getposition            = description->getposition;
    newdesc->lock                   = description->lock;
    newdesc->unlock                 = description->unlock;

    newdesc->getsamplemaxchannels   = description->getsamplemaxchannels;
    newdesc->getdriverinfo          = description->getdriverinfo;
    newdesc->getdriverinfow         = description->getdriverinfow;
    newdesc->getdrivercapsex        = description->getdrivercapsex;
    newdesc->getdrivercapsex2       = description->getdrivercapsex2;
    newdesc->initex                 = description->initex;
    newdesc->start                  = description->start;
    newdesc->stop                   = description->stop;
    newdesc->updatefinished         = description->updatefinished;
    newdesc->createsample           = description->createsample;
    newdesc->getsoundram            = description->getsoundram;
    newdesc->record_getnumdrivers   = description->record_getnumdrivers;
    newdesc->record_getdriverinfo   = description->record_getdriverinfo;
    newdesc->record_getdriverinfow  = description->record_getdriverinfow;
    newdesc->record_getdrivercaps   = description->record_getdrivercaps;
    newdesc->record_start           = description->record_start;
    newdesc->record_stop            = description->record_stop;
    newdesc->record_getposition     = description->record_getposition;
    newdesc->record_lock            = description->record_lock;
    newdesc->record_unlock          = description->record_unlock;
    newdesc->reverb_setproperties   = description->reverb_setproperties;
	newdesc->postmixcallback		= description->postmixcallback;
    newdesc->mType                  = description->mType;
    newdesc->mSize                  = description->mSize;
    newdesc->mModule                = description->mModule;
    newdesc->getmemoryused          = description->getmemoryused;
    newdesc->mHandle                = mCurrentPluginHandle++;

    newdesc->addBefore(&mOutputHead);

    if (handle)
    {
        *handle = newdesc->mHandle;
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
FMOD_RESULT PluginFactory::registerOutput(FMOD_OUTPUT_DESCRIPTION *description, unsigned int *handle)
{
    FMOD_OUTPUT_DESCRIPTION_EX *newdesc;

    if (!description)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    newdesc = FMOD_Object_Calloc(FMOD_OUTPUT_DESCRIPTION_EX);
    if (!newdesc)
    {
        return FMOD_ERR_MEMORY;
    }

    newdesc->name           = description->name;
    newdesc->version        = description->version;
    newdesc->polling        = description->polling;
    newdesc->getnumdrivers  = description->getnumdrivers;
    newdesc->getdrivername  = description->getdrivername;
    newdesc->init           = description->init;
    newdesc->close          = description->close;
    newdesc->update         = description->update;
    newdesc->gethandle      = description->gethandle;
    newdesc->getposition    = description->getposition;
    newdesc->lock           = description->lock;
    newdesc->unlock         = description->unlock;

    newdesc->mType          = FMOD_OUTPUTTYPE_UNKNOWN;
    newdesc->mSize          = sizeof(Output);
    newdesc->mModule        = 0;
    newdesc->getmemoryused  = 0;
    newdesc->mHandle        = mCurrentPluginHandle++;

    newdesc->addBefore(&mOutputHead);

    if (handle)
    {
        *handle = newdesc->mHandle;
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
// OFFSET: 0x008F3D00
FMOD_RESULT PluginFactory::getNumCodecs(int *numcodecs)
{
    if (!numcodecs)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    *numcodecs = mCodecHead.count();

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
// OFFSET: 0x008F3D50
FMOD_RESULT PluginFactory::getNumDSPs(int *numdsps)
{
    if (!numdsps)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numdsps = mDSPHead.count();

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
// OFFSET: 0x008F3DA0
FMOD_RESULT PluginFactory::getNumOutputs(int *numoutputs)
{
    if (!numoutputs)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    *numoutputs = mOutputHead.count();

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
FMOD_RESULT PluginFactory::getCodecHandle(int index, unsigned int *handle)
{
    LinkedListNode *node;

    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    node = mCodecHead.getNodeByIndex(index);
    if (!node)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    *handle = ((FMOD_CODEC_DESCRIPTION_EX *)node)->mHandle;      
    
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
FMOD_RESULT PluginFactory::getDSPHandle(int index, unsigned int *handle)
{
    LinkedListNode *node;

    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    node = mDSPHead.getNodeByIndex(index);
    if (!node)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    *handle = ((FMOD_DSP_DESCRIPTION_EX *)node)->mHandle;      

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
FMOD_RESULT PluginFactory::getOutputHandle(int index, unsigned int *handle)
{
    LinkedListNode *node;

    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    node = mOutputHead.getNodeByIndex(index);
    if (!node)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *handle = ((FMOD_OUTPUT_DESCRIPTION_EX *)node)->mHandle;      

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
// OFFSET: 0x008F3DF0
FMOD_RESULT PluginFactory::getCodec(unsigned int handle, FMOD_CODEC_DESCRIPTION_EX **codecdesc)
{
    FMOD_CODEC_DESCRIPTION_EX *head, *current;

    if (!codecdesc)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    *codecdesc = 0;

    head = &mCodecHead;
    current = (FMOD_CODEC_DESCRIPTION_EX *)(head->getNext());
    while (current != head)
    {
        if (current->mHandle == handle)
        {
            *codecdesc = current;
            return FMOD_OK;
        }

        current = (FMOD_CODEC_DESCRIPTION_EX *)(current->getNext());
    }
       
    return FMOD_ERR_PLUGIN_MISSING;
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
// OFFSET: 0x008F3E60
FMOD_RESULT PluginFactory::getDSP(unsigned int handle, FMOD_DSP_DESCRIPTION_EX **dspdesc)
{
    FMOD_DSP_DESCRIPTION_EX *head, *current;

    if (!dspdesc)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *dspdesc = 0;

    head     = &mDSPHead;
    current  = (FMOD_DSP_DESCRIPTION_EX *)(head->getNext());
    while (current != head)
    {
        if (current->mHandle == handle)
        {
            *dspdesc = current;
            return FMOD_OK;
        }

        current = (FMOD_DSP_DESCRIPTION_EX *)(current->getNext());
    }

    return FMOD_ERR_PLUGIN_MISSING;
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
// OFFSET: 0x008F3ED0
FMOD_RESULT PluginFactory::getOutput(unsigned int handle, FMOD_OUTPUT_DESCRIPTION_EX **outputdesc)
{
    FMOD_OUTPUT_DESCRIPTION_EX *head, *current;

    if (!outputdesc)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *outputdesc = 0;    

    head = &mOutputHead;
    current = (FMOD_OUTPUT_DESCRIPTION_EX *)(head->getNext());
    while (current != head)
    {
        if (current->mHandle == handle)
        {
            *outputdesc = current;
            return FMOD_OK;
        }

        current = (FMOD_OUTPUT_DESCRIPTION_EX *)(current->getNext());
    }
        
    return FMOD_ERR_PLUGIN_MISSING;
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
// OFFSET: 0x008F43B0
FMOD_RESULT PluginFactory::createCodec(FMOD_CODEC_DESCRIPTION_EX *codecdesc, Codec **codec)
{
    Codec *newcodec;

    if (!codecdesc || !codec)
    {
        return FMOD_ERR_INVALID_PARAM;      
    }

    /*
        Maybe this should call an 'alloc callback' to let the codec allocate itself?
        This would allow the object to use proper inheritence etc.  'Codec' is not good enough.

        the 'Ex' stuff could possibly be deleted if it did this, as the hidden stuff could just
        be a member of the codec class and the codecs just derive from it anyway?
    */
    newcodec = FMOD_Object_CallocSize(Codec, (unsigned int)codecdesc->mSize);
    if (!newcodec)
    {
        return FMOD_ERR_MEMORY;
    }
    
    /*
        Copy the description out of the source codec to the new codec.
    */
    FMOD_memcpy(&(newcodec->mDescription), codecdesc, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    *codec = newcodec;

    if (!newcodec->mDescription.getwaveformat)
    {
        newcodec->mDescription.getwaveformat = &Codec::defaultGetWaveFormat;
    }

    return FMOD_OK;
}

#ifdef FMOD_SUPPORT_SOFTWARE

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
// OFFSET: 0x008F4670
FMOD_RESULT PluginFactory::createDSP(FMOD_DSP_DESCRIPTION_EX *dspdesc, DSPI **dsp)
{
    FMOD_RESULT result;
    #ifdef PLATFORM_PS3
    DSPI *newdspmemory = 0;
    #endif
    DSPI *newdsp;
    unsigned int size = 0;

    if (!dspdesc || !dsp)
    {
        return FMOD_ERR_INVALID_PARAM;      
    }

    size = dspdesc->mSize;

    if (*dsp)
    {
        newdsp = *dsp;
    }
    else
    {
        switch (dspdesc->mCategory)
        {
            case FMOD_DSP_CATEGORY_SOUNDCARD:
#if !defined(PLATFORM_PS3) && !defined(PLATFORM_WINDOWS_PS3MODE)
            {
                dspdesc->mSize = dspdesc->mSize < sizeof(DSPSoundCard) ? sizeof(DSPSoundCard) : (int)size;

                newdsp = FMOD_Object_CallocSize(DSPSoundCard, (unsigned int)dspdesc->mSize);
                break;
            }
#endif
            case FMOD_DSP_CATEGORY_FILTER:
            {
                #ifdef PLATFORM_PS3

                size = dspdesc->mSize < (int)sizeof(DSPFilter) ? sizeof(DSPFilter) : dspdesc->mSize;
                size = (size + 15) & ~15;

                if (size > DSPSIZE)
                {
                    FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "PluginFactory::createDSP", "DSP unit of size %d is too big! Must be <= %d\n", size, DSPSIZE));
                    return FMOD_ERR_INTERNAL;
                }

                dspdesc->mSize = size;

                newdspmemory = (DSPI *)FMOD_Memory_Calloc(size + 128);
                if (!newdspmemory)
                {
                    return FMOD_ERR_MEMORY;
                }
                newdsp = (DSPI *)FMOD_ALIGNPOINTER(newdspmemory, 128);

                if (
                     FMOD_strcmp ("FMOD SoundCard Unit", dspdesc->name)           && 
                     FMOD_strcmp ("FMOD ChannelGroup Target Unit", dspdesc->name) &&
                     FMOD_strcmp ("FMOD Mixer unit", dspdesc->name)               &&
                     FMOD_strncmp("ChannelGroup", dspdesc->name, 12)              &&
                    (FMOD_strncmp("FMOD IT", dspdesc->name, 7) || dspdesc->mType == FMOD_DSP_TYPE_ITLOWPASS || dspdesc->mType == FMOD_DSP_TYPE_ITECHO)    /* It DSP units remain on PPU, IT lowpass can be used as a normal effect on SPU */
                    )
                {                
                    /*
                        Is an FMOD effect, or USER effect that needs to be streamed onto SPU
                    */
                    int totalsize = 0;
                    int clearsize = 0;

                    // FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "PluginFactory::createDSP", "Getting SPU PIC details for %s\n", dspdesc->name));

                    /*
                        Get info about the pic
                    */
                    result = OutputPS3::getSPUPICProperties((unsigned int)dspdesc->read, &newdsp->mCodeMramAddress, &newdsp->mCodeEntryAddress, &newdsp->mCodeSize, &newdsp->mCodeLSOffset, &newdsp->mCodeFillLocation, &newdsp->mCodeFillSize, &newdsp->mCodeFillValue);
                    if (result != FMOD_OK)
                    {
                        FMOD_Memory_Free(newdspmemory);
                        return result;
                    }

                    clearsize = newdsp->mCodeFillLocation - newdsp->mCodeSize + newdsp->mCodeFillSize;
                    clearsize = clearsize < 0 ? 0 : clearsize;
                    totalsize = dspdesc->mSize + newdsp->mCodeSize + clearsize;

                    if (totalsize > DSPSIZE)
                    {
                        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "PluginFactory::createDSP", "DSP unit (+ PIC code) of size %d is too big! Must be <= %d\n", totalsize, DSPSIZE));
                        return FMOD_ERR_INTERNAL;
                    }

                    // FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "PluginFactory::createDSP", "DSP unit (+ PIC code) size: %d\n", totalsize));

                    dspdesc->config = (FMOD_DSP_DIALOGCALLBACK)1;    /* Tell SPU to upload code */
                }
                else
                {
                    dspdesc->config = (FMOD_DSP_DIALOGCALLBACK)0;
                }

                #else
            
                dspdesc->mSize = size < sizeof(DSPFilter) ? sizeof(DSPFilter) : size;

                newdsp = (DSPI *)FMOD_Memory_Calloc(dspdesc->mSize);
                if (!newdsp)
                {
                    return FMOD_ERR_MEMORY;
                }

                #endif

                new (newdsp) DSPFilter;
                
                break;
            }
#ifdef FMOD_SUPPORT_DSPCODEC
            case FMOD_DSP_CATEGORY_DSPCODECMPEG:
            case FMOD_DSP_CATEGORY_DSPCODECADPCM:
            case FMOD_DSP_CATEGORY_DSPCODECXMA:
            case FMOD_DSP_CATEGORY_DSPCODECCELT:
            case FMOD_DSP_CATEGORY_DSPCODECRAW:
            {
            #ifdef PLATFORM_PS3
                size = dspdesc->mSize < (int)sizeof(DSPCodec) ? sizeof(DSPCodec) : dspdesc->mSize;
                size = (size + 15) & ~15;

                if (size > DSPSIZE)
                {
                    FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "PluginFactory::createDSP", "DSP unit of size %d is too big! Must be <= %d\n", size, DSPSIZE));
                    return FMOD_ERR_INTERNAL;
                }

                dspdesc->mSize = size;

                newdspmemory = (DSPI *)FMOD_Memory_CallocType(size + 16, FMOD_MEMORY_PERSISTENT);
                if (!newdspmemory)
                {
                    return FMOD_ERR_MEMORY;
                }

                newdsp = (DSPI *)FMOD_ALIGNPOINTER(newdspmemory, 16);

                {                
                    int totalsize = 0;
                    unsigned int codec_entry = 0;

                    /*
                        Get code address from codec
                    */
                    
                    switch(dspdesc->mCategory)
                    {
                    #ifdef FMOD_SUPPORT_RAW
                    case FMOD_DSP_CATEGORY_DSPCODECRAW:
                        {
                            CodecRaw codecraw;
                            codec_entry = (unsigned int)codecraw.getDescriptionEx()->mModule;

                            //FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "PluginFactory::createDSP", "Getting SPU PIC details for %s (%s)\n", dspdesc->name, codecraw.getDescriptionEx()->name));

                            /*
                                Get info about the pic
                            */
                            result = OutputPS3::getSPUPICProperties(codec_entry, &newdsp->mCodeMramAddress, &newdsp->mCodeEntryAddress, &newdsp->mCodeSize, &newdsp->mCodeLSOffset, &newdsp->mCodeFillLocation, &newdsp->mCodeFillSize, &newdsp->mCodeFillValue);
                            if (result != FMOD_OK)
                            {
                                FMOD_Memory_Free(newdspmemory);
                                return result;
                            }
                            break;
                        }
                    #endif
                    #ifdef FMOD_SUPPORT_IMAADPCM
                    case FMOD_DSP_CATEGORY_DSPCODECADPCM:
                        {
                            CodecWav codecwav;
                            codec_entry = (unsigned int)codecwav.getDescriptionEx()->mModule;

                            
                            //FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "PluginFactory::createDSP", "Getting SPU PIC details for %s (%s)\n", dspdesc->name, codecwav.getDescriptionEx()->name));

                            /*
                                Get info about the pic
                            */
                            result = OutputPS3::getSPUPICProperties(codec_entry, &newdsp->mCodeMramAddress, &newdsp->mCodeEntryAddress, &newdsp->mCodeSize, &newdsp->mCodeLSOffset, &newdsp->mCodeFillLocation, &newdsp->mCodeFillSize, &newdsp->mCodeFillValue);
                            if (result != FMOD_OK)
                            {
                                FMOD_Memory_Free(newdspmemory);
                                return result;
                            }
                            break;
                        }
                    #endif
                    #if defined(FMOD_SUPPORT_MPEG) && defined(FMOD_SUPPORT_MPEG_LAYER3) && !defined(FMOD_SUPPORT_MPEG_SONYDECODER)
                    case FMOD_DSP_CATEGORY_DSPCODECMPEG:
                        {
                            CodecMPEG codecmpeg;

                            codec_entry = (unsigned int)FMODGetMPEGFunctionsPICDescriptionEx()->entry;
   
                            //FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "PluginFactory::createDSP", "Getting SPU PIC details for %s (%s)\n", dspdesc->name, codecmpeg.getDescriptionEx()->name));

                            /*
                                Get info about the pic
                            */
                            result = OutputPS3::getSPUPICProperties(codec_entry, &newdsp->mCodeMramAddress, &newdsp->mCodeEntryAddress, &newdsp->mCodeSize, &newdsp->mCodeLSOffset, &newdsp->mCodeFillLocation, &newdsp->mCodeFillSize, &newdsp->mCodeFillValue);
                            if (result != FMOD_OK)
                            {
                                FMOD_Memory_Free(newdspmemory);
                                return result;
                            }
                            break;
                        }
                    #endif
                    default:
                        break;
                    }

                    totalsize = dspdesc->mSize + newdsp->mCodeSize + newdsp->mCodeFillSize;

                    if (totalsize > DSPSIZE)
                    {
                        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "PluginFactory::createDSP", "DSP unit (+ PIC code) of size %d is too big! Must be <= %d\n", totalsize, DSPSIZE));
                        return FMOD_ERR_INTERNAL;
                    }

                    //FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "PluginFactory::createDSP", "DSP unit (+ PIC code) size: %d\n", totalsize));
                }
            #else
                FMOD_MEMORY_TYPE memtype;
           
                size = size < sizeof(DSPCodec) ? sizeof(DSPCodec) : size;

                memtype = FMOD_MEMORY_PERSISTENT;
               
                #ifdef PLATFORM_XENON
                if (dspdesc->mCategory == FMOD_DSP_CATEGORY_DSPCODECXMA)
                {
                    memtype |= FMOD_MEMORY_XBOX360_PHYSICAL;
                }
                #endif
               
                newdsp = (DSPI *)FMOD_Memory_CallocType(size, memtype);
				if (!newdsp)
                {
                    return FMOD_ERR_MEMORY;
                }          
            #endif

                if (0)
                {
                }
                #ifdef FMOD_SUPPORT_MPEG
                else if (dspdesc->mCategory == FMOD_DSP_CATEGORY_DSPCODECMPEG)
                {
                    new (newdsp) DSPCodecMPEG;
                }
                #endif
                #ifdef FMOD_SUPPORT_IMAADPCM
                else if (dspdesc->mCategory == FMOD_DSP_CATEGORY_DSPCODECADPCM)
                {
                    new (newdsp) DSPCodecADPCM;
                }
                #endif
                #ifdef FMOD_SUPPORT_XMA
                else if (dspdesc->mCategory == FMOD_DSP_CATEGORY_DSPCODECXMA)
                {
                    new (newdsp) DSPCodecXMA;
                }
                #endif
                #ifdef FMOD_SUPPORT_CELT
                else if (dspdesc->mCategory == FMOD_DSP_CATEGORY_DSPCODECCELT)
                {
                    new (newdsp) DSPCodecCELT;
                }
                #endif
                #ifdef FMOD_SUPPORT_RAW
                else if (dspdesc->mCategory == FMOD_DSP_CATEGORY_DSPCODECRAW)
                {
                    new (newdsp) DSPCodecRaw;
                }
                #endif
                else
                {
                    return FMOD_ERR_FORMAT;
                }
                break;
            }
#endif
            case FMOD_DSP_CATEGORY_RESAMPLER:
            {
                #ifdef PLATFORM_PS3

                size = size < sizeof(DSPResamplerPS3) ? sizeof(DSPResamplerPS3) : size;
                size = (size + 15) & ~15;

                if (size > DSPSIZE)
                {
                    FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "PluginFactory::createDSP", "DSP unit of size %d is too big! Must be <= %d\n", size));
                    return FMOD_ERR_MEMORY;
                }

                dspdesc->mSize = size;

                newdspmemory = (DSPI *)FMOD_Memory_Calloc(size + 16);
                if (!newdspmemory)
                {
                    return FMOD_ERR_MEMORY;
                }

                newdsp = (DSPI *)FMOD_ALIGNPOINTER(newdspmemory, 16);
            
                #else
            
                #ifdef PLATFORM_WINDOWS_PS3MODE
                size = size < sizeof(DSPResampler) ? sizeof(DSPResampler) : size;
                #else
                size = size < sizeof(DSPResamplerMultiInput) ? sizeof(DSPResamplerMultiInput) : size;
                #endif

                newdsp = (DSPI *)FMOD_Memory_Calloc(size);
                if (!newdsp)
                {
                    return FMOD_ERR_MEMORY;
                }
            
                #endif

                #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
                new (newdsp) DSPResampler;
                #else
                new (newdsp) DSPResamplerMultiInput;
                #endif

                break;
            }
            case FMOD_DSP_CATEGORY_WAVETABLE:
            {
                #ifdef PLATFORM_PS3

                size = dspdesc->mSize < (int)sizeof(DSPWaveTable) ? sizeof(DSPWaveTable) : dspdesc->mSize;
                size = (size + 15) & ~15;

                if (size > DSPSIZE)
                {
                    FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "PluginFactory::createDSP", "DSP unit of size %d is too big! Must be <= %d\n", size));
                    return FMOD_ERR_INTERNAL;
                }

                dspdesc->mSize = size;

                newdspmemory = (DSPI *)FMOD_Memory_Calloc(size + 16);
                if (!newdspmemory)
                {
                    return FMOD_ERR_MEMORY;
                }

                newdsp = (DSPI *)FMOD_ALIGNPOINTER(newdspmemory, 16);
            
                #else
            
                size = size < sizeof(DSPWaveTable) ? sizeof(DSPWaveTable) : size;

                newdsp = (DSPI *)FMOD_Memory_Calloc(size);
                if (!newdsp)
                {
                    return FMOD_ERR_MEMORY;
                }
            
                #endif

                new (newdsp) DSPWaveTable;

                break;
            }
            default:
            {
                return FMOD_ERR_INVALID_PARAM;
            }
        }
    }

    if (!newdsp)
    {
        *dsp = 0;
        return FMOD_ERR_MEMORY;
    }

    newdsp->mSystem = mSystem;
       
    result = newdsp->alloc(dspdesc);
    if (result != FMOD_OK)
    {
        #ifdef PLATFORM_PS3
        FMOD_Memory_Free(newdspmemory);
        #else
        FMOD_Memory_Free(newdsp);
        #endif
        return result;
    }

    if (dspdesc->create)
    {
#ifdef FMOD_SUPPORT_DLLS
        if (dspdesc->mAEffect)
        {
            #ifdef FMOD_SUPPORT_VSTPLUGIN
            {
                AEffect *effect = (AEffect *)dspdesc->mAEffect;

                if (effect->magic == kEffectMagic)
                {
                    ((DSPVSTPlugin *)newdsp)->mEffect = effect;

                    LinkedListNode *newvstnode = FMOD_Object_Calloc(LinkedListNode);
                    if (!newvstnode)
                    {
                        return FMOD_ERR_MEMORY;
                    }

                    newvstnode->setData((void *)newdsp);
                    newvstnode->addAfter(&mSystem->mVSTPluginsListHead);
                }
            }
            #endif

            #ifdef FMOD_SUPPORT_WINAMPPLUGIN
            {
                winampDSPModule *effect = (winampDSPModule *)dspdesc->mAEffect;

                if (!FMOD_strncmp(dspdesc->name, "WINAMP", 6))
                {
                    ((DSPWinampPlugin *)newdsp)->mEffect = effect;
                }
            }
            #endif
        }
#endif

        newdsp->instance = (FMOD_DSP *)newdsp;

        result = dspdesc->create((FMOD_DSP_STATE *)newdsp);
        if (result != FMOD_OK)
        {
            #ifdef PLATFORM_PS3
            FMOD_Memory_Free(newdspmemory);
            #else
            FMOD_Memory_Free(newdsp);
            #endif
            return result;
        }
    }

    #ifdef PLATFORM_PS3
    newdsp->mMemory      = newdspmemory;
    newdsp->mMramAddress = (unsigned int)newdsp;
    #endif

    *dsp = newdsp;

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
// OFFSET: 0x008F3FC0
FMOD_RESULT PluginFactory::createOutput(FMOD_OUTPUT_DESCRIPTION_EX *outputdesc, Output **output)
{
    Output *newoutput;

    if (!outputdesc || !output)
    {
        return FMOD_ERR_INVALID_PARAM;      
    }

#if defined(FMOD_SUPPORT_OPENAL) && !defined(FMOD_USE_PLUGINS)
	if (outputdesc->mType == FMOD_OUTPUTTYPE_OPENAL)	/* OpenAL has a special virtual function getFreeChannel so we need to detect it and object alloc it to set up vtables. */
	{
		newoutput = FMOD_Object_Calloc(OutputOpenAL);
	}
	else
#endif
    if (outputdesc->polling)
    {
        newoutput = FMOD_Object_CallocSize(OutputPolled, (unsigned int)outputdesc->mSize);
    }
    else
    {
        newoutput = FMOD_Object_CallocSize(Output, (unsigned int)outputdesc->mSize);
    }

    if (!newoutput)
    {
        *output = 0;
        return FMOD_ERR_MEMORY;
    }
    
    /*
        Copy the description out of the source output to the new output.
    */
    FMOD_memcpy(&newoutput->mDescription, outputdesc, sizeof(FMOD_OUTPUT_DESCRIPTION_EX));

    newoutput->mSystem       = mSystem;

#ifdef FMOD_SUPPORT_SOFTWARE
    newoutput->readfrommixer = Output::mixCallback;
#endif

    *output = newoutput;

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

FMOD_RESULT PluginFactory::getMemoryUsedImpl(MemoryTracker *tracker)
{
    LinkedListNode *node;

    tracker->add(false, FMOD_MEMBITS_PLUGINS, sizeof(*this));

    for (node = mCodecHead.getNext(); node != &mCodecHead; node = node->getNext())
    {
        tracker->add(false, FMOD_MEMBITS_PLUGINS, sizeof(FMOD_CODEC_DESCRIPTION_EX));
    }

    for (node = mDSPHead.getNext(); node != &mDSPHead; node = node->getNext())
    {
        tracker->add(false, FMOD_MEMBITS_PLUGINS, sizeof(FMOD_DSP_DESCRIPTION_EX));
    }

    for (node = mOutputHead.getNext(); node != &mOutputHead; node = node->getNext())
    {
        tracker->add(false, FMOD_MEMBITS_PLUGINS, sizeof(FMOD_OUTPUT_DESCRIPTION_EX));
    }

    return FMOD_OK;
}

#endif

}
