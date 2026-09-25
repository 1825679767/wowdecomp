#include "fmod_settings.h"

#include "fmod.h"
#include "fmod_3d.h"
#ifdef FMOD_SUPPORT_NONBLOCKING
#include "fmod_async.h"
#endif
#include "fmod_autocleanup.h"
#include "fmod_channeli.h"
#include "fmod_channel_real.h"
#include "fmod_channel_stream.h"
#include "fmod_channel_software.h"
#include "fmod_codec_user.h"
#include "fmod_codeci.h"
#include "fmod_debug.h"
#include "fmod_dsp_chorus.h"
#ifdef FMOD_SUPPORT_DSPCODEC
#include "fmod_dsp_codec.h"
#endif

#ifndef FMOD_STATICFORPLUGINS
#include "fmod_dsp_distortion.h"
#include "fmod_dsp_echo.h"
#include "fmod_dsp_delay.h"
#include "fmod_dsp_fft.h"
#include "fmod_dsp_filter.h"
#include "fmod_dsp_flange.h"
#include "fmod_dsp_highpass.h"
#include "fmod_dsp_itecho.h"
#include "fmod_dsp_compressor.h"
#include "fmod_dsp_sfxreverb.h"
#include "fmod_dsp_lowpass.h"
#include "fmod_dsp_lowpass2.h"
#include "fmod_dsp_lowpass_simple.h"
#include "fmod_dsp_normalize.h"
#include "fmod_dsp_oscillator.h"
#include "fmod_dsp_parameq.h"
#include "fmod_dsp_pitchshift.h"
#include "fmod_dsp_reverb.h"
#include "fmod_dsp_tremolo.h"
#endif

#include "fmod_dsp_resampler.h"
#ifndef PLATFORM_PS3
#include "fmod_dsp_soundcard.h"
#endif
#ifdef FMOD_SUPPORT_VSTPLUGIN
#include "fmod_dsp_vstplugin.h"
#endif
#include "fmod_dsp_wavetable.h"
#include "fmod_dspi.h"
#include "fmod_file.h"
#include "fmod_file_cdda.h"
#include "fmod_file_disk.h"
#include "fmod_file_memory.h"
#include "fmod_file_net.h"
#include "fmod_file_null.h"
#include "fmod_file_user.h"
#include "fmod_localcriticalsection.h"
#include "fmod_memory.h"
#include "fmod_metadata.h"
#include "fmod_net.h"
#include "fmod_output_emulated.h"
#include "fmod_output_polled.h"
#include "fmod_output_software.h"
#include "fmod_os_cdda.h"
#include "fmod_os_output.h"
#include "fmod_pluginfactory.h"
#include "fmod_sample_software.h"
#include "fmod_soundi.h"
#include "fmod_sound_sample.h"

#ifdef FMOD_SUPPORT_GEOMETRY
#include "fmod_geometryi.h"
#endif
#ifdef FMOD_SUPPORT_STREAMING
#include "fmod_sound_stream.h"
#endif
#include "fmod_string.h"
#include "fmod_stringw.h"
#include "fmod_systemi.h"
#ifdef FMOD_SUPPORT_OPENAL
#include "fmod_output_openal.h"
#endif
#include "fmod_downmix.h"
#ifdef FMOD_SUPPORT_NEURAL
#include "fmod_downmix_neuralthx.h"
#endif
#ifdef FMOD_SUPPORT_MYEARS
#include "fmod_downmix_myears.h"
#endif

/*
    Codecs
*/
#ifdef FMOD_SUPPORT_AAC
#include "fmod_codec_aac.h"
#endif
#ifdef FMOD_SUPPORT_AIFF
#include "fmod_codec_aiff.h"
#endif
#ifdef FMOD_SUPPORT_ASF
#include "fmod_codec_asf.h"
#endif
#ifdef FMOD_SUPPORT_AT3
#include "fmod_codec_at3.h"
#endif
#ifdef FMOD_SUPPORT_CDDA
#include "fmod_codec_cdda.h"
#endif
#ifdef FMOD_SUPPORT_DLS
#include "fmod_codec_dls.h"
#endif
#ifdef FMOD_SUPPORT_FLAC
#include "fmod_codec_flac.h"
#endif
#ifdef FMOD_SUPPORT_GCADPCM_DSP
#include "fmod_codec_dsp.h"
#endif
#ifdef FMOD_SUPPORT_FSB
#include "fmod_codec_fsb.h"
#endif
#ifdef FMOD_SUPPORT_IT
#include "fmod_codec_it.h"
#endif
#ifdef FMOD_SUPPORT_MOD
#include "fmod_codec_midi.h"
#endif
#ifdef FMOD_SUPPORT_MOD
#include "fmod_codec_mod.h"
#endif
#ifdef FMOD_SUPPORT_MPEG
#include "fmod_codec_mpeg.h"
#endif
#ifdef FMOD_SUPPORT_MPEGPSP
#include "fmod_codec_mpegpsp.h"
#endif
#ifdef FMOD_SUPPORT_OGGVORBIS
#include "fmod_codec_oggvorbis.h"
#endif
#ifdef FMOD_SUPPORT_CELT
#include "fmod_codec_celt.h"
#endif
#ifdef FMOD_SUPPORT_PLAYLIST
#include "fmod_codec_playlist.h"
#endif
#ifdef FMOD_SUPPORT_RAW
#include "fmod_codec_raw.h"
#endif
#ifdef FMOD_SUPPORT_SF2
#include "fmod_codec_sf2.h"
#endif
#ifdef FMOD_SUPPORT_S3M
#include "fmod_codec_s3m.h"
#endif
#ifdef FMOD_SUPPORT_TAGS
#include "fmod_codec_tag.h"
#endif
#ifdef FMOD_SUPPORT_TREMOR
#include "fmod_codec_tremor.h"
#endif
#ifdef FMOD_SUPPORT_VAG
    #if defined(PLATFORM_PS2) || defined(PLATFORM_PSP)
    #include "fmod_codec_vag.h"
    #else
    #include "fmod_codec_swvag.h"
    #endif
#endif
#ifdef FMOD_SUPPORT_WAV
#include "fmod_codec_wav.h"
#endif
#ifdef FMOD_SUPPORT_XM
#include "fmod_codec_xm.h"
#endif
#ifdef FMOD_SUPPORT_XMA
#include "fmod_codec_xma.h"
#endif
#ifdef FMOD_SUPPORT_XWMA
#include "fmod_codec_xwma.h"
#endif

#ifdef FMOD_SUPPORT_IMAADPCM
#include "fmod_codec_wav_imaadpcm.h"
#endif

/*
    Output types
*/
#ifdef FMOD_SUPPORT_NOSOUND
#include "fmod_output_nosound.h"
#endif
#ifdef FMOD_SUPPORT_NOSOUND_NRT
#include "fmod_output_nosound_nrt.h"
#endif
#ifdef FMOD_SUPPORT_WAVWRITER
#include "fmod_output_wavwriter.h"
#endif
#ifdef FMOD_SUPPORT_WAVWRITER_NRT
#include "fmod_output_wavwriter_nrt.h"
#endif

/*
    Profiler modules
*/
#ifdef FMOD_SUPPORT_PROFILE
#include "fmod_profile.h"
#endif
#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_DSP)
#include "fmod_profile_dsp.h"
#endif
#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_MEMORY)
#include "fmod_profile_memory.h"
#endif
#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_CPU)
#include "fmod_profile_cpu.h"
#endif
#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_CHANNEL)
#include "fmod_profile_channel.h"
#endif
#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_CODEC)
#include "fmod_profile_codec.h"
#endif

#include "fmod_speakermap.h"    /* Must be after mmreg include from codec wav */

namespace FMOD
{

FMOD_OS_CRITICALSECTION *SystemI::gSoundListCrit      = 0;

/*
[
	[DESCRIPTION]
    Returns a pointer to the instance of system object with the specified id

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
// OFFSET: 0x008D2800
FMOD_RESULT SystemI::getInstance(unsigned int id, SystemI **sys)
{
    SystemI *current;

    if (sys)
    {
        *sys = 0;
    }

    current = SAFE_CAST(SystemI, gGlobal->gSystemHead->getNext());
    while (current != gGlobal->gSystemHead)
    {
        if (current->mIndex == id)
        {
            if (sys)
            {
                *sys = current;
            }
            return FMOD_OK;
        }

        current = SAFE_CAST(SystemI, current->getNext());
    }

    return FMOD_ERR_INVALID_PARAM;
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
// OFFSET: 0x008D2850
FMOD_RESULT SystemI::validate(System *system, SystemI **systemi)
{
    FMOD::SystemI *sys = (FMOD::SystemI *)system;

    if (!system)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!systemi)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!FMOD::gGlobal->gSystemHead->exists(sys))
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    *systemi = sys;

    return FMOD_OK;
}


#ifdef FMOD_SUPPORT_STREAMING

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
// OFFSET: 0x008D5200
void SystemI::streamThread(void *data)
{
    SystemI *system = (SystemI *)data;
   
    system->updateStreams();
}


/*
    ====================================================================================

    PRIVATE MEMBER FUNCTIONS

    ====================================================================================
*/


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
// OFFSET: 0x008D2890
FMOD_RESULT SystemI::updateStreams()
{
    mStreamTimeStamp.stampIn();

    /*
        Process Stream Channel list.  Stream channels are added and removed with ChannelStream::alloc and ChannelStream::stop
    */
    FMOD_OS_CriticalSection_Enter(mStreamListCrit);
    {
        mStreamListChannelCurrent = mStreamListChannelHead.getNext();
        while (mStreamListChannelCurrent != &mStreamListChannelHead)
        {
            ChannelStream *channelstream = (ChannelStream *)mStreamListChannelCurrent->getData();
            mStreamListChannelNext = mStreamListChannelCurrent->getNext();

            FMOD_OS_CriticalSection_Leave(mStreamListCrit);

            /*
                Update the stream channel.
            */
            FMOD_OS_CriticalSection_Enter(mStreamUpdateCrit);
            if (!channelstream->mFinished)
            {
                channelstream->updateStream();
            }
            FMOD_OS_CriticalSection_Leave(mStreamUpdateCrit);


            FMOD_OS_CriticalSection_Enter(mStreamListCrit);            

            mStreamListChannelCurrent = mStreamListChannelNext;
        }
        mStreamListChannelNext = 0;
    }
    FMOD_OS_CriticalSection_Leave(mStreamListCrit);

    /*
        Process stream sound list.  Stream sounds are added at System::createSound and Sound::release.
        Channel will always be valid until the sound is flagged as finished.
    */
    FMOD_OS_CriticalSection_Enter(mStreamListCrit);
    {
        LinkedListNode *current = mStreamListSoundHead.getNext();
        while (current != &mStreamListSoundHead)
        {
            Stream *stream = (Stream *)current->getData();

            if ((stream->mChannel && stream->mChannel->mFinished))
            {
                stream->mFlags |= FMOD_SOUND_FLAG_THREADFINISHED; /* Main thread will wait for this flag while releasing. */
                if (stream->mSubSound)
                {
                    if (stream->mSubSoundShared)
                    {
                        stream->mSubSoundShared->mFlags |= FMOD_SOUND_FLAG_THREADFINISHED;
                    }
                    else if (stream->mSubSound[stream->mSubSoundIndex])
                    {
                        stream->mSubSound[stream->mSubSoundIndex]->mFlags |= FMOD_SOUND_FLAG_THREADFINISHED;
                    }
                }
            }
            current = current->getNext();
        }
    }
    FMOD_OS_CriticalSection_Leave(mStreamListCrit);

    mStreamTimeStamp.stampOut(95);

    return FMOD_OK;
}

#endif


/*
[
	[DESCRIPTION]
    Updates virtual voices, and any per voice calculations, ie emulated voice calcs, 3d calcs or geometry.

	[PARAMETERS]
    'delta' Delta in milliseconds from last frame.
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
// OFFSET: 0x008D29C0
FMOD_RESULT SystemI::updateChannels(int delta)
{
    int       emulatedused;
    ChannelI *current;

    /*
        If the channel has stop, put it at the end of the list.
    */
    current = SAFE_CAST(ChannelI, mChannelUsedListHead.getNext());
    while (current != &mChannelUsedListHead)
    {
        FMOD_RESULT  result;
        ChannelI    *next = SAFE_CAST(ChannelI, current->getNext());
        bool         playing = false;

        result = current->isPlaying(&playing);
        if (playing)
        {
            current->update(delta);
        }

        result = current->isPlaying(&playing);
        if (!playing)
        {
            current->stopEx(CHANNELI_STOPFLAG_REFSTAMP | 
                            CHANNELI_STOPFLAG_UPDATELIST | 
                            CHANNELI_STOPFLAG_RESETCALLBACKS | 
                            CHANNELI_STOPFLAG_CALLENDCALLBACK | 
                            CHANNELI_STOPFLAG_RESETCHANNELGROUP |
                            CHANNELI_STOPFLAG_PROCESSENDDELAY |
                            CHANNELI_STOPFLAG_UPDATESYNCPOINTS);  /* Reset, return to freelist and call end callback. */
        }
      
        current = next;
    }

    /*
        Now swap any emulated channels that have made it to the front of the list with real channels
    */
    if (mEmulated)
    {
        mEmulated->mChannelPool->getChannelsUsed(&emulatedused);

        if (emulatedused)
        {
            LinkedListNode *emucurrent, *realcurrent[CHANNELREAL_TYPE_MAX];
            int       count;

            emucurrent = mChannelSortedListHead.getNext();

            for (count = 0; count < CHANNELREAL_TYPE_MAX; count++)
            {
                realcurrent[count] = &mChannelSortedListHead;
            }
        
            do
            {
                FMOD_RESULT     result = FMOD_OK;
                ChannelI       *emuchannel = 0;
                ChannelReal    *emuchannelreal [FMOD_CHANNEL_MAXREALSUBCHANNELS] = { 0 };
                ChannelI       *realchannel    [FMOD_CHANNEL_MAXREALSUBCHANNELS] = { 0 };
                ChannelReal    *realchannelreal[FMOD_CHANNEL_MAXREALSUBCHANNELS] = { 0 };
                FMOD_MODE       mode;
                int             realchannelsneeded, realchannelsfound, channelsfound;
                bool            isvirtual;

                /*
                    Search for an emulated channel from the head of the importance list.
                */
                isvirtual = false;
                do
                {
                    emuchannel = (ChannelI *)emucurrent->getData();
                    if (!emuchannel)
                    {
                        /*
                            Back to head of list, didn't find anything. Don't crash.
                        */
                        break;
                    }
                       
                    result = emuchannel->isVirtual(&isvirtual);
                    if (isvirtual && !(emuchannel->mFlags & CHANNELI_FLAG_FORCEVIRTUAL))
                    {
                        break;
                    }
 
                    emucurrent = (ChannelI *)emucurrent->getNext();
                } while (emucurrent != &mChannelSortedListHead);

                if (emucurrent == &mChannelSortedListHead || 
                    emucurrent == realcurrent[0] || 
                    emucurrent == realcurrent[1] || 
                    emucurrent == realcurrent[2] || 
                    result != FMOD_OK)
                {
                    break;
                }

                /*
                    Now we've found an emulated channel, see if there are any free 
                    or lower priority channels that we can swap with of the same type
                */
                channelsfound = 0;          /* ChannelIs */
                realchannelsfound = 0;      /* RealChannels */

                result = emuchannel->getRealChannel(emuchannelreal, 0);
                if (result != FMOD_OK)
                {
                    break;
                }
                               
                mode = emuchannelreal[0]->mMode;

                /*
                    Find out how many subsamples we have to query the number of real channels we need for this emulated voice.
                */

                if (emuchannelreal[0]->mSound)
                {
    #ifdef FMOD_SUPPORT_STREAMING

                    if (emuchannelreal[0]->mSound->isStream())
                    {
                        Stream *stream = SAFE_CAST(Stream, emuchannelreal[0]->mSound);
                        Sample *sample = stream->mSample;

                        realchannelsneeded = sample->mNumSubSamples;
                    }
                    else

    #endif
                    {
                        Sample *sample = SAFE_CAST(Sample, emuchannelreal[0]->mSound);

                        realchannelsneeded = sample->mNumSubSamples;
                    }
                }
                else if (emuchannelreal[0]->mDSP)
                {
                    realchannelsneeded = 1;
                }
                else
                {
                    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::updateChannels", "emuchannelreal[0] has no mSound or mDSP\n"));
                    return FMOD_ERR_INTERNAL;
                }

                if (realchannelsneeded < 1)
                {
                    realchannelsneeded = 1;
                }

                /*
                    First try and find free real voices for this virtual channel, so we can swap with them.
                */
                if (realchannelsfound < realchannelsneeded)
                {
                    int found = 0;
                    ChannelReal *tempchannelreal[FMOD_CHANNEL_MAXREALSUBCHANNELS] = { 0 };

                    if (mode & FMOD_HARDWARE)
                    {
                        if (emuchannelreal[0]->mSound)
                        {
                            result = mOutput->getFreeChannel(mode, tempchannelreal, realchannelsneeded, emuchannelreal[0]->mSound->mChannels, &found, true);
                        }
                        else
                        {
                            return FMOD_ERR_INTERNAL;
                        }
                    }
                    else
                    {
    #ifdef FMOD_SUPPORT_SOFTWARE
                        /*
                            Before looking for a software channel (which there might be available), see if it is a realtime compressed sample.
                        */
                        if (mode & FMOD_CREATECOMPRESSEDSAMPLE
    #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
                            || (emuchannelreal[0]->mSound && emuchannelreal[0]->mSound->mFormat == FMOD_SOUND_FORMAT_PCM16)
    #endif
                            )
                        {
                            Sample *sample = SAFE_CAST(Sample, emuchannelreal[0]->mSound);

                            if (0)
                            {
                            }
    #ifdef FMOD_SUPPORT_DSPCODEC
                            #ifdef FMOD_SUPPORT_MPEG
                            if (sample->mFormat == FMOD_SOUND_FORMAT_MPEG)
                            {
                                result = mDSPCodecPool_MPEG.areAnyFree();
                            }
                            #endif
                            #ifdef FMOD_SUPPORT_XMA
                            else if (sample->mFormat == FMOD_SOUND_FORMAT_XMA)
                            {
                                result = mDSPCodecPool_XMA.areAnyFree();
                            }
                            #endif
                            #ifdef FMOD_SUPPORT_IMAADPCM
                            else if (sample->mFormat == FMOD_SOUND_FORMAT_IMAADPCM)
                            {
                                result = mDSPCodecPool_ADPCM.areAnyFree();
                            }
                            #endif
                            #ifdef FMOD_SUPPORT_CELT
                            else if (sample->mFormat == FMOD_SOUND_FORMAT_CELT)
                            {
                                result = mDSPCodecPool_CELT.areAnyFree();
                            }
                            #endif
                            #ifdef FMOD_SUPPORT_RAWCODEC
                            else if (sample->mFormat == FMOD_SOUND_FORMAT_PCM16)
                            {
                                result = mDSPCodecPool_RAW.areAnyFree();
                            }
                            #endif
    #endif
                            else
                            {
                                result = FMOD_ERR_FORMAT;
                            }
                        }
                        else if (mode & (FMOD_CREATESTREAM | FMOD_NONBLOCKING) && emuchannelreal[0]->mSound && emuchannelreal[0]->mSound->mOpenState == FMOD_OPENSTATE_SETPOSITION)
                        {
                            result = FMOD_ERR_NOTREADY;
                        }
                        else
                        {
                            result = FMOD_OK;
                        }

                        if (result == FMOD_OK)
                        {
                            if (emuchannelreal[0]->mSound)
                            {
                                result = mSoftware->getFreeChannel(mode, tempchannelreal, realchannelsneeded, emuchannelreal[0]->mSound->mChannels, &found, true);
                            }
                            else if(emuchannelreal[0]->mDSP)
                            {
                                result = mSoftware->getFreeChannel(mode, tempchannelreal, realchannelsneeded, 1, &found, true);
                            }
                            else
                            {
                                return FMOD_ERR_INTERNAL;
                            }
                        }
    #else
                        result = FMOD_ERR_NEEDSSOFTWARE;
    #endif
                    }

                    for (count = 0; count < found; count++)
                    {
                        realchannelreal[realchannelsfound] = tempchannelreal[count];
                        realchannelsfound ++;
                    }
                }

    tryformore:
                /*
                    If there are not enough stopped real channels lying around, find a real channel to swap with the virtual channel.
                */
                if (realchannelsfound < realchannelsneeded)
                {
                    CHANNELREAL_TYPE realtype;
                
                    if (mOutput->mChannelPool == mOutput->mChannelPool3D)
                    {
                        realtype = mode & FMOD_SOFTWARE ? CHANNELREAL_TYPE_SW : CHANNELREAL_TYPE_HW;
                    }
                    else
                    {
                        realtype = mode & FMOD_SOFTWARE ? CHANNELREAL_TYPE_SW : mode & FMOD_3D ? CHANNELREAL_TYPE_HW3D : CHANNELREAL_TYPE_HW2D;
                    }
            
                    if (realcurrent[realtype] == &mChannelSortedListHead)
                    {
                        realcurrent[realtype] = realcurrent[realtype]->getPrev();
                    }

                    do
                    {
                        if (realcurrent[realtype] == emucurrent)
                        {
                            result = FMOD_ERR_CHANNEL_ALLOC;
                            break;
                        }
                   
                        realchannel[channelsfound] = (ChannelI *)realcurrent[realtype]->getData();

                        /*
                            TEMPORARY FIX, ARE VOICES BEING SWAPPED AROUND AND THE EMUCURRENT/REALCURRENT pointing to the wrong places?
                        */
                        if (realcurrent[realtype] == &mChannelSortedListHead)
                        {
                            result = FMOD_ERR_CHANNEL_ALLOC;
                            break;
                        }

                        /*
                            Only want a real channel channel
                        */
                        realchannel[channelsfound]->isVirtual(&isvirtual);
                        if (isvirtual)
                        {
                            realchannel[channelsfound]->mFlags &= ~CHANNELI_FLAG_JUSTWENTVIRTUAL;   /* Ok, it didnt swap, its going to come in virtual now. */
                        }
                        else
                        {
                            FMOD_MODE        currentmode;
                            CHANNELREAL_TYPE currentrealtype;

                            /*
                                Only want a channel of the same type.
                            */
                            realchannel[channelsfound]->getMode(&currentmode);                           

                            if (mOutput->mChannelPool == mOutput->mChannelPool3D)
                            {
                                currentrealtype = currentmode & FMOD_SOFTWARE ? CHANNELREAL_TYPE_SW : CHANNELREAL_TYPE_HW;
                            }
                            else
                            {
                                currentrealtype = currentmode & FMOD_SOFTWARE ? CHANNELREAL_TYPE_SW : currentmode & FMOD_3D ? CHANNELREAL_TYPE_HW3D : CHANNELREAL_TYPE_HW2D;
                            }

                            if (realtype == currentrealtype)
                            {
                                /*
                                    AJS 20/09/06  If a FMOD_CREATECOMPRESSEDSAMPLE is going from virtual to real then we need to make sure there's
                                                  a dspcodec free for it to use. To ensure this, we make sure FMOD_CREATECOMPRESSEDSAMPLE only steals
                                                  FMOD_CREATECOMPRESSEDSAMPLE channels of the same format. This is so a dspcodec is freed for us to use.
                                */
                                if (realtype == CHANNELREAL_TYPE_SW)
                                {
                                    if (mode & FMOD_CREATECOMPRESSEDSAMPLE
    #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
                                        || (emuchannelreal[0]->mSound && emuchannelreal[0]->mSound->mFormat == FMOD_SOUND_FORMAT_PCM16)
    #endif
                                        )
                                    {
                                        Sample *sample = SAFE_CAST(Sample, emuchannelreal[0]->mSound);

                                        if (0)
                                        {
                                        }
    #ifdef FMOD_SUPPORT_DSPCODEC
                                        #ifdef FMOD_SUPPORT_MPEG
                                        if (sample->mFormat == FMOD_SOUND_FORMAT_MPEG)
                                        {
                                            result = mDSPCodecPool_MPEG.areAnyFree();
                                        }
                                        #endif
                                        #ifdef FMOD_SUPPORT_XMA
                                        else if (sample->mFormat == FMOD_SOUND_FORMAT_XMA)
                                        {
                                            result = mDSPCodecPool_XMA.areAnyFree();
                                        }
                                        #endif
                                        #ifdef FMOD_SUPPORT_IMAADPCM
                                        else if (sample->mFormat == FMOD_SOUND_FORMAT_IMAADPCM)
                                        {
                                            result = mDSPCodecPool_ADPCM.areAnyFree();
                                        }
                                        #endif
                                        #ifdef FMOD_SUPPORT_CELT
                                        else if (sample->mFormat == FMOD_SOUND_FORMAT_CELT)
                                        {
                                            result = mDSPCodecPool_CELT.areAnyFree();
                                        }
                                        #endif
                                        #ifdef FMOD_SUPPORT_RAWCODEC
                                        else if (sample->mFormat == FMOD_SOUND_FORMAT_PCM16)
                                        {
                                            result = mDSPCodecPool_RAW.areAnyFree();
                                        }
                                        #endif
    #endif
                                        else
                                        {
                                            result = FMOD_ERR_FORMAT;
                                        }

                                        if (result == FMOD_OK)
                                        {
                                            break;
                                        }
                                        else
                                        {
                                            /*
                                                If we stole this channel would it free up a dspcodec of the same type that we want?
                                            */
                                            if (currentmode & FMOD_CREATECOMPRESSEDSAMPLE
    #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
                                                || (emuchannelreal[0]->mSound && emuchannelreal[0]->mSound->mFormat == FMOD_SOUND_FORMAT_PCM16)
    #endif
                                                )
                                            {
                                                ChannelReal *tempcr[FMOD_CHANNEL_MAXREALSUBCHANNELS] = { 0 };

                                                realchannel[channelsfound]->getRealChannel(tempcr, 0);

                                                Sample *currentsample = SAFE_CAST(Sample, tempcr[0]->mSound);

                                                if (currentsample->mFormat == sample->mFormat)
                                                {
                                                    result = FMOD_OK;
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        result = FMOD_OK;
                                        break;
                                    }
                                }
                                else
                                {
                                    result = FMOD_OK;
                                    break;
                                }
                            }
                        }

                        realcurrent[realtype] = realcurrent[realtype]->getPrev();

                    } while (1);

                    if (result == FMOD_OK)
                    {
                        int found = 0;
                        ChannelReal *tempchannelreal[FMOD_CHANNEL_MAXREALSUBCHANNELS] = { 0 };

                        realchannel[channelsfound]->getRealChannel(tempchannelreal, &found);

                        for (count = 0; count < found; count++)
                        {
                            realchannelreal[realchannelsfound] = tempchannelreal[count];
                            realchannelsfound ++;
                        }
                        channelsfound++;

                        /*
                            Ok, if we havent got enough 'realchannelreal's for this virtual channel to become active still, 
                            start stealing multiple 'realchannel's (and their child 'realchannelreal's) :|
                        */
                        if (realchannelsfound < realchannelsneeded)
                        {
                            realcurrent[realtype] = realcurrent[realtype]->getPrev();
                            goto tryformore;
                        }
                    }
                    else
                    {
                        realchannel[channelsfound] = 0;
                    }
                }
           
                /*
                    ============================================================================
                    SWAP THE REALCHANNELS AROUND
                    ============================================================================
                */
                if (realchannelsfound >= realchannelsneeded)
                {
                    FMOD_CHANNEL_INFO emuinfo;
                    FMOD_CHANNEL_INFO realinfo[FMOD_CHANNEL_MAXREALSUBCHANNELS];
                    FMOD_MODE         mode;
                    int               emulatedtoswap = channelsfound;

                    emuchannel->getMode(&mode);
                    
                    /*
                        ============================================================================
                        Step 1.  Get information from the real and emulated channel before swapping.
                        ============================================================================
                    */

                    /*
                        If there is a real voice involved, stop it.
                    */
                    if (channelsfound)  
                    {
                        for (count = 0; count < channelsfound; count++)
                        {
                            realchannel[count]->getChannelInfo(&realinfo[count]);
                            realchannel[count]->stopEx(CHANNELI_STOPFLAG_DONTFREELEVELS);
                        }

                        /*
                            Kill off any extra voices that we stole but aren't used, this will put them back in the 'free' pool.
                        */
                        for (count = realchannelsneeded; count < realchannelsfound; count++)
                        {
                            realchannelreal[count]->mFlags &= ~(CHANNELREAL_FLAG_IN_USE | CHANNELREAL_FLAG_ALLOCATED | CHANNELREAL_FLAG_PLAYING | CHANNELREAL_FLAG_PAUSED);
                            realchannelreal[count]->mFlags |=  CHANNELREAL_FLAG_STOPPED;
                        }
                    }
                    else
                    {
                        emuchannel->getChannelInfo(&emuinfo);
                        emuchannel->stopEx(CHANNELI_STOPFLAG_DONTFREELEVELS);

                        /*
                            ====================================================================
                            Step 2.  Convert 'emulatedchannel' to 'real' and re-start it.
                
                            Note: After this 'realchannel' will be pointing to an emulated voice 
                            and 'emuchannel' will be pointing to a real voice!
                            ====================================================================
                        */
        #ifdef FMOD_SUPPORT_STREAMING
                        // STREAM SPECIAL CASE GOING FROM EMULATED TO REAL.
                        if (emuinfo.mSound && emuinfo.mSound->mMode & FMOD_CREATESTREAM)
                        {
                            Stream *stream = SAFE_CAST(Stream, emuinfo.mSound);
                            ChannelStream *channelstream = stream->mChannel;

                            emuchannel->mRealChannel[0] = channelstream;

                            channelstream->mNumRealChannels = realchannelsneeded;
                            for (count = 0; count < realchannelsneeded; count++)
                            {
                                channelstream->mRealChannel[count] = realchannelreal[count];   /* Set the channelstream's realchannel (the dsound/software/emu channel). */
                                channelstream->mRealChannel[count]->mSubChannelIndex = count;
                            }
                        }
                        else
        #endif
                        {
                            // ORDINARY SAMPLE GOING FROM EMULATED TO REAL.
                            emuchannel->mFlags &= ~CHANNELI_FLAG_JUSTWENTVIRTUAL;
                            emuchannel->mNumRealChannels = realchannelsneeded;
                            for (count = 0; count < realchannelsneeded; count++)
                            {
                                emuchannel->mRealChannel[count] = realchannelreal[count];
                                emuchannel->mRealChannel[count]->mSubChannelIndex = count;
                            }
                        }

                        if (emuinfo.mDSP)
                        {
                            result = emuchannel->play(emuinfo.mDSP, true, false, false);
                        }
                        else
                        {
                            result = emuchannel->play(emuinfo.mSound, true, false, false);
                        }
                        if (result != FMOD_OK)
                        {
                            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::updateChannels", "swapped channel failed to play. ZOMBIE CHANNEL!\n"));
                        }
                        emuchannel->setChannelInfo(&emuinfo);
                        emuchannel->setChannelGroup(emuchannel->mChannelGroup);
                        emuchannel->setPaused(emuinfo.mPaused);
                    }

                    /*
                        ====================================================================
                        Step 3.  Convert 'realchannel' to 'emulated' and re-start it.
                        ====================================================================
                    */
                    for (count = 0; count < emulatedtoswap; count++)
                    {
                        /*
                            ok we've done a swap with an emulated voice, now we'll have to create new emulated voices
                        */
                        result = mEmulated->getFreeChannel(mode, emuchannelreal, 1, 1, 0);

                        /* 
                            This channel is going from real to emulated, so just give it 1 subchannel and point it to 1 emulated voice 
                        */
                        realchannel[count]->mNumRealChannels = 1;
                        realchannel[count]->mRealChannel[0] = emuchannelreal[0];

                        if (realinfo[count].mSound)
                        {
                            result = realchannel[count]->play(realinfo[count].mSound->mSubSampleParent, true, false, false);
                            realchannel[count]->setChannelInfo(&realinfo[count]);
                            realchannel[count]->setChannelGroup(realchannel[count]->mChannelGroup);
                            realchannel[count]->setPaused(realinfo[count].mPaused);
                        }
                        else if (realinfo[count].mDSP)
                        {
                            result = realchannel[count]->play(realinfo[count].mDSP, true, false, false);
                            realchannel[count]->setChannelInfo(&realinfo[count]);
                            realchannel[count]->setChannelGroup(realchannel[count]->mChannelGroup);
                            realchannel[count]->setPaused(realinfo[count].mPaused);
                        }

                        emucurrent = (ChannelI *)emucurrent->getNext();     /* Don't let it try and swap the same channel again. */
                    }
                }
                else
                {
                    emuchannel->mFlags &= ~CHANNELI_FLAG_JUSTWENTVIRTUAL;   /* Ok, it didnt swap, its going to come in virtual now. */
                }

            } while (1);
        }       
    }

    return FMOD_OK;
}

#ifndef FMOD_STATICFORPLUGINS
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
// OFFSET: 0x008D3230
FMOD_RESULT SystemI::updateSoundGroups(int delta)
{
    SoundGroupI *soundgroup;

    FMOD_OS_CriticalSection_Enter(gSoundListCrit);
    {
        soundgroup = (SoundGroupI *)mSoundGroupUsedHead.getNext();
        while (soundgroup != &mSoundGroupUsedHead)
        {
            int numaudible = 0;
            SoundGroupI *next = (SoundGroupI *)soundgroup->getNext();

            soundgroup->mPlayCount = 0;
        
            if (soundgroup->mMaxAudibleBehavior == FMOD_SOUNDGROUP_BEHAVIOR_MUTE)
            {
                soundgroup->getNumPlaying(&numaudible);
            }

            if (numaudible)
            {
                LinkedListNode *current;
                
                current = soundgroup->mChannelListHead.getNext();
                while (current != &soundgroup->mChannelListHead)
                {
                    ChannelI *channel;
                    LinkedListNode *next;

                    channel = (ChannelI *)current->getData();
                    next = current->getNext();        

                    if (soundgroup->mMaxAudibleBehavior == FMOD_SOUNDGROUP_BEHAVIOR_MUTE && soundgroup->mMaxAudible >= 0)
                    {
                        soundgroup->mPlayCount++;
                        if (soundgroup->mPlayCount > soundgroup->mMaxAudible)
                        {
                            channel->mFadeTarget = 0.0f;
                        }
                        else
                        {
                            channel->mFadeTarget = 1.0f;
                        }
                    }
                 
                    if (channel->mFadeVolume != channel->mFadeTarget)
                    {
                        if (soundgroup->mFadeSpeed < 0.001f)
                        {
                            channel->mFadeVolume = channel->mFadeTarget;
                        }
                        else
                        {
                            if (channel->mFadeVolume < channel->mFadeTarget)
                            {
                                channel->mFadeVolume += ((float)delta / (soundgroup->mFadeSpeed * 1000.0f));
                                if (channel->mFadeVolume > channel->mFadeTarget)
                                {
                                    channel->mFadeVolume = channel->mFadeTarget;
                                }
                            }
                            if (channel->mFadeVolume > channel->mFadeTarget)
                            {
                                channel->mFadeVolume -= ((float)delta / (soundgroup->mFadeSpeed * 1000.0f));
                                if (channel->mFadeVolume < channel->mFadeTarget)
                                {
                                    channel->mFadeVolume = channel->mFadeTarget;
                                }
                            }
                        }

                        channel->setVolume(channel->mVolume, true);
                    }

                    current = next;
                }
            }
            else
            {
                /*
                    Put back on the free list.
                */
                soundgroup->removeNode();
                soundgroup->addAfter(&mSoundGroupFreeHead);
            }
            soundgroup = next;
        }
    }
    FMOD_OS_CriticalSection_Leave(gSoundListCrit);

    return FMOD_OK;
}
#endif


#ifdef FMOD_SUPPORT_VSTPLUGIN
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
FMOD_RESULT SystemI::updateVSTPlugins()
{
    if (!mVSTPluginsListHead.isEmpty())
    {
        LinkedListNode *dspvstnode = mVSTPluginsListHead.getNext();
        while (dspvstnode != &mVSTPluginsListHead)
        {
            DSPVSTPlugin *dspvst = (DSPVSTPlugin *)dspvstnode->getData();

            /*
                Call VST idle function
            */
            #ifdef FMOD_SUPPORT_DLLS
            if (dspvst->mDescription.configidle)
            {
                dspvst->mDescription.configidle((FMOD_DSP_STATE *)dspvst);
            }
            #endif

            dspvstnode = dspvstnode->getNext();
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

	[SEE_ALSO]
]
*/
// OFFSET: 0x008D5210
FMOD_RESULT SystemI::findChannel(FMOD_CHANNELINDEX id, SoundI *sound, ChannelI **channel)
{
    ChannelI       *chan = 0;
    int             count, subchannels, found;
    ChannelReal    *realchannel[FMOD_CHANNEL_MAXREALSUBCHANNELS] = { 0 };
    FMOD_RESULT     result;
    FMOD_MODE       mode;

    if (!channel)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = sound->getMode(&mode);
    if (result != FMOD_OK)
    {
        return result;
    }

    /*
        If the sound is unique, stop any instances of it
    */
    if (mode & FMOD_UNIQUE)
	{
        result = stopSound(sound);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (id == FMOD_CHANNEL_REUSE)
    {
        chan = *channel;

        if (!chan)
        {
            id = FMOD_CHANNEL_FREE;
        }
        else
        {
            if (!chan->mRealChannel[0] || chan->mRealChannel[0]->mFlags & CHANNELREAL_FLAG_STOPPED)
            {
                id = (FMOD_CHANNELINDEX)chan->mIndex;
            }
            else
            {
                chan->stopEx(CHANNELI_STOPFLAG_CALLENDCALLBACK | CHANNELI_STOPFLAG_RESETCHANNELGROUP);
            }
        }
    }

    if (id != FMOD_CHANNEL_REUSE)   /* id might be changed above, so it is not a simple else statement. */
    {
        *channel = 0;

        if (id != FMOD_CHANNEL_FREE)
        {
            if (id < 0 || id >= mNumChannels)
            {
                return FMOD_ERR_INVALID_PARAM;
            }
            chan = &mChannel[id];
            chan->stop();
        }
        else
        {           
            if (!mChannelFreeListHead.isEmpty())
            {
                chan = SAFE_CAST(ChannelI, mChannelFreeListHead.getNext());
            }
            else    /* There would be no stopped voices otherwise they would be in the freelist, so find the lowest priority channel and steal it. */
            {           
                LinkedListNode *node = SAFE_CAST(LinkedListNode, mChannelSortedListHead.getPrev());

                if (mChannelSortedListHead.isEmpty())
                {
                    return FMOD_ERR_CHANNEL_ALLOC;      /* This usually only happens if someone sets System::init to 0 channels. */
                }
                
                node = SAFE_CAST(LinkedListNode, mChannelSortedListHead.getPrev());

                chan = (ChannelI *)node->getData();
                chan->stopEx(CHANNELI_STOPFLAG_REFSTAMP | CHANNELI_STOPFLAG_RESETCALLBACKS | CHANNELI_STOPFLAG_CALLENDCALLBACK | CHANNELI_STOPFLAG_RESETCHANNELGROUP);
            }
        }
        
        chan->removeNode();
        chan->addBefore(&mChannelUsedListHead);
    }

#ifdef FMOD_SUPPORT_STREAMING
    if (mode & FMOD_CREATESTREAM)
    {
        Stream *stream = SAFE_CAST(Stream, sound);
        Sample *sample = SAFE_CAST(Sample, stream->mSample);

        if (sample)
        {
            subchannels = sample->mNumSubSamples;
        }
        else
        {
            subchannels = 0;
        }
    }
    else
#endif
    {
        Sample *sample = SAFE_CAST(Sample, sound);

        subchannels = sample->mNumSubSamples;
    }
    
    if (!subchannels)
    {
        subchannels = 1;
    }

    /*
        Assign a 'REAL' channel to the virtual channel
    */
    if (((mode & FMOD_SOFTWARE || mOutputType == FMOD_OUTPUTTYPE_OPENAL) && mode & FMOD_CREATECOMPRESSEDSAMPLE)
#if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
        || (sound->mFormat == FMOD_SOUND_FORMAT_PCM16)
#endif
    )
    {
        #ifdef FMOD_SUPPORT_SOFTWARE
       
        /*
            Before looking for a software channel (which there might be available), see if it is a realtime compressed sample.
        */
        if (0)
        {
        }
#ifdef FMOD_SUPPORT_DSPCODEC
        #ifdef FMOD_SUPPORT_MPEG
        else if (sound->mFormat == FMOD_SOUND_FORMAT_MPEG)
        {
            result = mDSPCodecPool_MPEG.areAnyFree();
        }
        #endif
        #ifdef FMOD_SUPPORT_XMA
        else if (sound->mFormat == FMOD_SOUND_FORMAT_XMA)
        {
            result = mDSPCodecPool_XMA.areAnyFree();
        }
        #endif
        #ifdef FMOD_SUPPORT_IMAADPCM
        else if (sound->mFormat == FMOD_SOUND_FORMAT_IMAADPCM)
        {
            result = mDSPCodecPool_ADPCM.areAnyFree();
        }
        #endif
        #ifdef FMOD_SUPPORT_CELT
        else if (sound->mFormat == FMOD_SOUND_FORMAT_CELT)
        {
            result = mDSPCodecPool_CELT.areAnyFree();
        }
        #endif
        #ifdef FMOD_SUPPORT_RAWCODEC
        else if (sound->mFormat == FMOD_SOUND_FORMAT_PCM16)
        {
            result = mDSPCodecPool_RAW.areAnyFree();
        }
        #endif
#endif
        else
        {
            result = FMOD_ERR_FORMAT;
        }
    }

    if ((mode & FMOD_CREATESTREAM) && sound->mCodec->mFlags & FMOD_CODEC_HARDWAREMUSICVOICES)
    {      
        result = sound->mCodec->getHardwareMusicChannel(&realchannel[0]);
        if (result == FMOD_OK)
        {
            found = 1;
        }
    }
    else if (mode & FMOD_SOFTWARE || !mOutput->mDescription.createsample)
	{
        if (result == FMOD_OK)
        {
            result = mSoftware->getFreeChannel(mode, realchannel, subchannels, sound->mChannels, &found);
        }
        
        #else
        
        result = FMOD_ERR_NEEDSSOFTWARE;
        
        #endif
    }
    else
    {
		result = mOutput->getFreeChannel(mode, realchannel, subchannels, sound->mChannels, &found);
    }

    if (result != FMOD_OK || found != subchannels)
    {
        subchannels = 1;
        result = mEmulated->getFreeChannel(mode, realchannel, subchannels, sound->mChannels, 0);
    }
    if (result != FMOD_OK)
    {
        return result;
    }

    if (realchannel[0]->mOutput == mEmulated)
    {
        chan->mFlags |= CHANNELI_FLAG_JUSTWENTVIRTUAL;
    }

    #ifdef FMOD_SUPPORT_STREAMING
    if (mode & FMOD_CREATESTREAM)
    {
        if (realchannel[0]->mOutput == mEmulated)
        {
            chan->mNumRealChannels = 1;
            chan->mRealChannel[0] = realchannel[0];
            chan->mRealChannel[0]->mSubChannelIndex = 0;
        }
        else
        {
            Stream *stream = SAFE_CAST(Stream, sound);
            ChannelStream *channelstream = stream->mChannel;

            chan->mNumRealChannels = 1;
            chan->mRealChannel[0] = channelstream;   /* Set the ChannelI's realchannel (the channelstream). */

            channelstream->mNumRealChannels = subchannels;
            channelstream->mSubChannelIndex = 0;
            for (count = 0; count < subchannels; count++)
            {
                channelstream->mRealChannel[count] = realchannel[count];     /* Set the channelstream's realchannel (the dsound/software/emu channel). */
                channelstream->mRealChannel[count]->mSubChannelIndex = count;
            }
        }
    }
    else
    #endif
    {
        chan->mNumRealChannels = subchannels;
        for (count = 0; count < subchannels; count++)
        {
            chan->mRealChannel[count] = realchannel[count];
            chan->mRealChannel[count]->mSubChannelIndex = count;
        }
    }

    *channel = chan;

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
// OFFSET: 0x008D3420
FMOD_RESULT SystemI::findChannel(FMOD_CHANNELINDEX id, DSPI *dsp, ChannelI **channel)
{
    ChannelI       *chan = 0;
    ChannelReal    *realchannel[FMOD_CHANNEL_MAXREALSUBCHANNELS] = { 0 };
    FMOD_RESULT     result;

    if (!channel)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (id == FMOD_CHANNEL_REUSE)
    {
        chan = *channel;

        if (!chan)
        {
            id = FMOD_CHANNEL_FREE;
        }
        else
        {
            if (chan->mRealChannel[0]->mFlags & CHANNELREAL_FLAG_STOPPED)
            {
                id = (FMOD_CHANNELINDEX)chan->mIndex;
            }
            else
            {
                chan->stopEx(CHANNELI_STOPFLAG_RESETCHANNELGROUP);
            }
        }
    }

    if (id != FMOD_CHANNEL_REUSE)   /* id might be changed above, so it is not a simple else statement. */
    {
        *channel = 0;

        if (id != FMOD_CHANNEL_FREE)
        {
            chan = &mChannel[id];
            chan->stop();
        }
        else
        {
            if (!mChannelFreeListHead.isEmpty())
            {
                chan = SAFE_CAST(ChannelI, mChannelFreeListHead.getNext());
            }
            else    /* There would be no stopped voices otherwise they would be in the freelist, so find the lowest priority channel and steal it. */
            {           
                LinkedListNode *node = SAFE_CAST(LinkedListNode, mChannelSortedListHead.getPrev());                
                
                if (mChannelSortedListHead.isEmpty())
                {
                    return FMOD_ERR_CHANNEL_ALLOC;      /* This usually only happens if someone sets System::init to 0 channels. */
                }

                node = SAFE_CAST(LinkedListNode, mChannelSortedListHead.getPrev());

                chan = (ChannelI *)node->getData();
                               
                chan->stopEx(CHANNELI_STOPFLAG_REFSTAMP | CHANNELI_STOPFLAG_RESETCALLBACKS | CHANNELI_STOPFLAG_RESETCHANNELGROUP);
            }
        }
        
        chan->removeNode();
        chan->addBefore(&mChannelUsedListHead);
    }

    /*
        Assign a 'REAL' channel to the virtual channel
    */
#ifdef FMOD_SUPPORT_SOFTWARE
    result = mSoftware->getFreeChannel(FMOD_SOFTWARE, realchannel, 1, 1, 0);
    if (result != FMOD_OK)
#endif
    {
        result = mEmulated->getFreeChannel(FMOD_SOFTWARE, realchannel, 1, 1, 0);
    }

    if (result != FMOD_OK)
    {
        return result;
    }

    chan->mRealChannel[0]  = realchannel[0];
    chan->mNumRealChannels = 1;

    *channel = chan;
    
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
// OFFSET: 0x008D5590
FMOD_RESULT SystemI::createSample(FMOD_MODE mode, FMOD_CODEC_WAVEFORMAT *waveformat, Sample **sample_out)
{
    FMOD_RESULT  result;
    Output      *output;
    Sample      *sample = 0;
    AutoRelease<Sample> sample_cleanup;
    int          channels, subsamples, count;
    unsigned int loopstart = 0, loopend = 0;

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSample", "mode %08x length %d samples, lengthbytes %d\n", mode, waveformat ? waveformat->lengthpcm : 0, waveformat ? waveformat->lengthbytes : 0));

    if (!sample_out)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (waveformat->lengthpcm == (unsigned int)-1)
    {
        return FMOD_ERR_MEMORY;
    }

    /*
        Check hardware/software bits in user mode and waveformat mode.
    */
    if (!(mode & (FMOD_HARDWARE | FMOD_SOFTWARE)))
    {
        int num2d, num3d;

        mode |= (waveformat->mode & (FMOD_HARDWARE | FMOD_SOFTWARE | FMOD_2D | FMOD_3D));

        #if defined(PLATFORM_PS2)
        if (waveformat->format != FMOD_SOUND_FORMAT_VAG)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSample", "Sound format = %d, not VAG so using software.\n", waveformat->format));
            mode  |= FMOD_SOFTWARE;
        }
        #endif
      
        getHardwareChannels(&num2d, &num3d, 0);

        if (mode & FMOD_HARDWARE)
        {        
            if (mode & FMOD_3D)
            {
                if (!num3d)
                {
                    mode &= ~FMOD_HARDWARE;
                    mode |= FMOD_SOFTWARE;
                }
            }
            else
            {
                if (!num2d)
                {
                    mode &= ~FMOD_HARDWARE;
                    mode |= FMOD_SOFTWARE;
                }
            }
        }
    }

    if (mode & FMOD_SOFTWARE)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        if (!mSoftware)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSample", "ERROR - Software not initialized\n"));
            return FMOD_ERR_NEEDSSOFTWARE;
        }

        mode &= ~FMOD_HARDWARE;
        output = mSoftware;
#else
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSample", "ERROR - Software is disabled.  Tried to create a software sample.\n"));
        return FMOD_ERR_NEEDSSOFTWARE;
#endif
    }
    else
    {
        mode |= FMOD_HARDWARE;
        output = mOutput;
    }

    /*
        Check 2D/3D bits in user mode and waveformat mode.
    */
    if (!(mode & (FMOD_2D | FMOD_3D)))
    {
        mode |= (waveformat->mode & (FMOD_2D | FMOD_3D));
    }

    if (mode & FMOD_3D)
    {
        mode &= ~FMOD_2D;
    }
    else
    {
        mode |= FMOD_2D;
    }

    /*
        Check loop bits in user mode and waveformat mode
    */
    if (!(mode & (FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI)))
    {
        mode |= (waveformat->mode & (FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI));
    }

    if (mode & FMOD_LOOP_NORMAL)
    {
        mode &= ~FMOD_LOOP_OFF;
        mode &= ~FMOD_LOOP_BIDI;
        mode |= FMOD_LOOP_NORMAL;
    }           
    else if (mode & FMOD_LOOP_BIDI)
    {
        mode &= ~FMOD_LOOP_OFF;
        mode |= FMOD_LOOP_BIDI;
    }           
    else 
    {
        mode |= FMOD_LOOP_OFF;
    }
    
    if (waveformat->mode & FMOD_CREATECOMPRESSEDSAMPLE)
    {
        mode |= FMOD_CREATECOMPRESSEDSAMPLE;
        mode &= ~FMOD_CREATESAMPLE;
    }

    /*
        Create the sample
    */
    channels = waveformat->channels;

    /*
        Check for valid number of channels
    */
    if (channels > FMOD_CHANNEL_MAXREALSUBCHANNELS && channels > (output->mDescription.getsamplemaxchannels ? output->mDescription.getsamplemaxchannels(output, mode, waveformat->format) : 1))
    {
        return FMOD_ERR_TOOMANYCHANNELS;
    }

    /* 
        If a voice is 3d and hardware, and cannot support multichannel input, then make it a multi-mono sample sound.
        When these samples are played, they take up multiple mono voices.
    */    
    if (channels > 1 && channels > (output->mDescription.getsamplemaxchannels ? output->mDescription.getsamplemaxchannels(output, mode, waveformat->format) : 1))
    {
        subsamples = channels;

        if (*sample_out)
        {
            sample = *sample_out;
        }
        else
        {
            sample = FMOD_Object_Calloc(Sample);
            sample_cleanup = sample;
        }
        if (!sample)
        {
            return FMOD_ERR_MEMORY;
        }

        sample->mNumSubSamples = channels;

        if (!sample->mName && !(mode & FMOD_LOWMEM))
        {
            sample->mName = (char *)FMOD_Memory_Calloc(FMOD_STRING_MAXNAMELEN);
            if (!sample->mName)
            {
                return FMOD_ERR_MEMORY;
            }
        }

        /*
            Set the default values
        */        
        if (sample->mName)
        {
            FMOD_strcpy(sample->mName,  waveformat->name);
        }
        sample->mDefaultFrequency   = (float)waveformat->frequency;
        sample->mDefaultChannelMask = waveformat->channelmask;
        sample->mLength             = waveformat->lengthpcm;
        sample->mMode               = mode;
        sample->mLoopStart          = 0;
        sample->mLoopLength         = sample->mLength;
        sample->mFormat             = waveformat->format;
        sample->mChannels           = channels;
        sample->mCodec              = 0;
        sample->mType               = FMOD_SOUND_TYPE_USER;
        sample->mSystem             = this;
        sample->mMinDistance        = 1.0f     * mDistanceScale;
        sample->mMaxDistance        = 10000.0f * mDistanceScale;  
        sample->mLockBuffer         = mMultiSubSampleLockBuffer.alloc(SOUND_READCHUNKSIZE);
        if (!sample->mLockBuffer)
        {
            return FMOD_ERR_MEMORY;
        }

        channels = 1;
    }
    else
    {
        subsamples = 1;
    }

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSample", "subsamples = %d, channels = %d\n", subsamples, channels));

    for (count = 0; count < subsamples; count++)
    {
        Sample *subsample = subsamples == 1 ? *sample_out : 0;
        FMOD_CODEC_WAVEFORMAT subsamplewaveformat;

        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSample", "subsample %d.  output = %p\n", count, output));

        FMOD_memcpy(&subsamplewaveformat, waveformat, sizeof(FMOD_CODEC_WAVEFORMAT));
        subsamplewaveformat.channels = channels;

        if (output->mDescription.createsample)
        {
#ifdef FMOD_SUPPORT_SOFTWARE
            output->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#endif

            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSample", "use hw\n", count));

            result = output->mDescription.createsample(output, mode, &subsamplewaveformat, &subsample);
            mCreatedHardwareSample = true;
        }
        else
        {
#ifdef FMOD_SUPPORT_SOFTWARE
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSample", "mSoftware = %p\n", mSoftware));

            result = mSoftware->createSample(mode, &subsamplewaveformat, &subsample);
#else
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSample", "ERROR - Software is disabled.  Tried to create a software sample.\n"));
            result = FMOD_ERR_NEEDSSOFTWARE;
#endif
        }
        if (result != FMOD_OK)
        {
            if(subsamples > 1)
            {
                /* free the other subsamples created in this loop */
                sample->release(false);
            }
            else if (subsample && subsample != *sample_out)
            {
                /* don't free sample_out that was passed in */
                subsample->release();
            }
            return result;
        }

        if (subsamples > 1)
        {
            sample->mSubSample[count] = subsample;
        }
        else
        {
            if (subsample != *sample_out)
            {
                sample_cleanup = subsample;
            }
            sample = subsample;

            if (!sample->mName && !(mode & FMOD_LOWMEM))
            {
                sample->mName = (char *)FMOD_Memory_Calloc(FMOD_STRING_MAXNAMELEN);
                if (!sample->mName)
                {
                    return FMOD_ERR_MEMORY;
                }
            }
        }

        /*
            Set the default values
        */        
        if (subsample->mName)
        {
            FMOD_strcpy(subsample->mName,  waveformat->name);
        }
        subsample->mDefaultFrequency   = (float)waveformat->frequency;
        subsample->mDefaultChannelMask = waveformat->channelmask;
        subsample->mMode               = mode;
        subsample->mLoopStart          = 0;
        subsample->mLoopLength         = subsample->mLength;
        subsample->mFormat             = waveformat->format;
        subsample->mChannels           = channels;
        subsample->mCodec              = 0;
        subsample->mType               = FMOD_SOUND_TYPE_USER;
        subsample->mSystem             = this;
        subsample->mMinDistance        = 1.0f     * mDistanceScale;
        subsample->mMaxDistance        = 10000.0f * mDistanceScale; 
        subsample->mSubSampleParent    = sample;
    }

    loopstart = waveformat->loopstart;
    loopend   = waveformat->loopend;
    if (!loopend)
    {
        loopend = sample->mLength - 1;
    }
    sample->setLoopPoints(loopstart, FMOD_TIMEUNIT_PCM, loopend, FMOD_TIMEUNIT_PCM);

    *sample_out = sample;
    sample_cleanup.releasePtr();

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSample", "done\n"));

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
// OFFSET: 0x008D67D0
FMOD_RESULT SystemI::createSoundInternal(const char *name_or_data, FMOD_MODE mode_in, unsigned int buffersize, FMOD_TIMEUNIT buffersizetype, FMOD_CREATESOUNDEXINFO *exinfo, bool calledfromasync, SoundI **sound)
{
    FMOD_RESULT            result;
    SoundI               **subsound    = 0;
    AutoFree               subsound_cleanup;
    SoundI                *soundi      = 0;
    Codec                 *codec       = 0;
    AutoRelease<Codec>     codec_cleanup;
    Metadata              *metadata    = 0;
    File                  *file        = 0;
    AutoFree               file_cleanup;
    AutoClose<File>        file_closer;
    FMOD_CODEC_WAVEFORMAT  waveformat;
    bool                   netfile     = false;
    int                    count;
    int                    numsubsounds;
    int                    numcodecs;
    int                    maxchannels = 0;
    FMOD_MODE              originalmode = mode_in;
    void                  *filebufferstack = 0;
    char                   filebufferstackmem[(16*1024) + 32];   /* +32 for wii/gc alignment */

    #ifdef FMOD_DEBUG
    if (mode_in & (FMOD_OPENMEMORY | FMOD_OPENMEMORY_POINT))
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "memory = %p : mode %08x\n", name_or_data, mode_in));
    }
    else
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "filename = %s : mode %08x\n", name_or_data ? name_or_data : "", mode_in));
    }
    if (exinfo)
    {
        if (exinfo->cbsize)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->cbsize             = %d\n", exinfo->cbsize));
        }                                                                                                               
        if (exinfo->length)                                                                                             
        {                                                                                                               
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->length             = %d\n", exinfo->length));
        }                                                                                                               
        if (exinfo->fileoffset)                                                                                         
        {                                                                                                               
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->fileoffset         = %d\n", exinfo->fileoffset));
        }                                                                                                               
        if (exinfo->numchannels)                                                                                        
        {                                                                                                               
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->numchannels        = %d\n", exinfo->numchannels));
        }                                                                                                               
        if (exinfo->defaultfrequency)                                                                                   
        {                                                                                                               
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->defaultfrequency   = %d\n", exinfo->defaultfrequency));
        }                                                                                                               
        if (exinfo->format)                                                                                             
        {                                                                                                               
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->format             = %d\n", exinfo->format));
        }                                                                                                               
        if (exinfo->decodebuffersize)                                                                                   
        {                                                                                                               
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->decodebuffersize   = %d\n", exinfo->decodebuffersize));
        }                                                                                                               
        if (exinfo->initialsubsound)                                                                                    
        {                                                                                                               
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->initialsubsound    = %d\n", exinfo->initialsubsound));
        }                                                                                                               
        if (exinfo->numsubsounds)                                                                                       
        {                                                                                                               
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->numsubsounds       = %d\n", exinfo->numsubsounds));
        }                                                                                                               
        if (exinfo->inclusionlist)                                                                                      
        {                                                                                                               
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->inclusionlist      = %p\n", exinfo->inclusionlist));
        }                                                                                                               
        if (exinfo->inclusionlistnum)                                                                                   
        {                                                                                                               
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->inclusionlistnum   = %d\n", exinfo->inclusionlistnum));
        }                                                                                                               
        if (exinfo->pcmreadcallback)                                                                                    
        {                                                                                                               
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->pcmreadcallback    = %p\n", exinfo->pcmreadcallback));
        }                                                                                                               
        if (exinfo->pcmsetposcallback)                                                                                  
        {                                                                                                               
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->pcmsetposcallback  = %p\n", exinfo->pcmsetposcallback));
        }                                                                                                               
        if (exinfo->nonblockcallback)                                                                                   
        {                                                                                                               
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->nonblockcallback   = %p\n", exinfo->nonblockcallback));
        }                                                                                                               
        if (exinfo->dlsname)                                                                                            
        {                                                                                                               
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->dlsname            = %s\n", exinfo->dlsname));
        }                                                                                                               
        if (exinfo->encryptionkey)                                                                                      
        {                                                                                                               
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->encryptionkey      = %s\n", exinfo->encryptionkey));
        }
        if (exinfo->maxpolyphony)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->maxpolyphony       = %d\n", exinfo->maxpolyphony));
        }
        if (exinfo->userdata)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->userdata           = %p\n", exinfo->userdata));
        }
        if (exinfo->suggestedsoundtype)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->suggestedsoundtype = %d\n", exinfo->suggestedsoundtype));
        }
        if (exinfo->userdata)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->userdata           = %p\n", exinfo->userdata));
        }
        if (exinfo->useropen)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->useropen           = %p\n", exinfo->useropen));
        }
        if (exinfo->userclose)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->userclose          = %p\n", exinfo->userclose));
        }
        if (exinfo->userread)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->userread           = %p\n", exinfo->userread));
        }
        if (exinfo->userseek)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->userseek           = %p\n", exinfo->userseek));
        }
        if (exinfo->speakermap)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->speakermap         = %d\n", exinfo->speakermap));
        }
        if (exinfo->initialsoundgroup)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->initialsoundgroup  = %d\n", exinfo->initialsoundgroup));
        }
        if (exinfo->initialseekposition)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->initialseekposition = %d\n", exinfo->initialseekposition));
        }
        if (exinfo->initialseekpostype)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "exinfo->initialseekpostype = %d\n", exinfo->initialseekpostype));
        }
    }
    #endif

    if (!sound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!(mode_in & FMOD_NONBLOCKING))
    {
        *sound = 0;
    }

    /*
        Check this is a valid exinfo structure.  Future revisions may want to set version numbers 
        here to account for backwards compatibility.
    */
    if (exinfo && exinfo->cbsize != sizeof(FMOD_CREATESOUNDEXINFO))
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "VERSION MISMATCH : cbsize in code (%d) does not match cbsize in lib (%d).  Make sure you are not mixing library and header versions of FMOD!\n", exinfo->cbsize, sizeof(FMOD_CREATESOUNDEXINFO)));
        return FMOD_ERR_INVALID_PARAM;
    }

    /*
        1. Stream overrides sample && realtime sample.
        2. Realtime sample overrides sample.
    */
    if (mode_in & FMOD_CREATESTREAM)
    {
        mode_in &= ~FMOD_CREATESAMPLE;
        mode_in &= ~FMOD_CREATECOMPRESSEDSAMPLE;    /* Decode to PCM through stream thread. */
    }
    else if (mode_in & FMOD_CREATECOMPRESSEDSAMPLE)
    {
        mode_in &= ~FMOD_CREATESAMPLE;
    }

    /*
        Turn off 3d flags if 2d is set
    */
    if (mode_in & FMOD_2D)
    {
        mode_in &= ~(FMOD_3D_HEADRELATIVE | FMOD_3D_WORLDRELATIVE | FMOD_3D_LOGROLLOFF | FMOD_3D_LINEARROLLOFF | FMOD_3D_CUSTOMROLLOFF);
    }

    /*
        Turn on 3d if one of the 3d flags is used.
    */
    if (mode_in & (FMOD_3D_HEADRELATIVE | FMOD_3D_WORLDRELATIVE | FMOD_3D_LOGROLLOFF | FMOD_3D_LINEARROLLOFF | FMOD_3D_CUSTOMROLLOFF))
    {
        mode_in |= FMOD_3D;
    }

    /*
        Fall back to software if hardware doesnt exist.
    */
    if (!(mode_in & FMOD_SOFTWARE))
    {   
        int num2d, num3d;
        
        getHardwareChannels(&num2d, &num3d, 0);

        if (mode_in & FMOD_3D)
        {
            if (!num3d)
            {
                mode_in &= ~FMOD_HARDWARE;
                mode_in |= FMOD_SOFTWARE;
            }
        }
        else
        {
            if (!num2d)
            {
                mode_in &= ~FMOD_HARDWARE;
                mode_in |= FMOD_SOFTWARE;
            }
        }
    }

    if (mode_in & FMOD_OPENUSER || mode_in & FMOD_OPENRAW)
    {
        if (!exinfo)
        {
            return FMOD_ERR_INVALID_PARAM;
        }
        if (exinfo->format == FMOD_SOUND_FORMAT_NONE || 
            exinfo->numchannels <= 0 ||
            exinfo->defaultfrequency == 0)
        {
            return FMOD_ERR_INVALID_PARAM;
        }
    }

    /*
        First open the file
    */
    if (mode_in & (FMOD_OPENMEMORY | FMOD_OPENMEMORY_POINT))
    {
        file_cleanup = file = FMOD_Object_Alloc(MemoryFile);
        CHECK_RESULT(file == NULL ? FMOD_ERR_MEMORY : FMOD_OK);

        file->init(this, 0, 0); /* Set blocksize to 0 to stop it buffering, we don't need it. */
    }
    else if (mode_in & FMOD_OPENUSER)
    {
        file_cleanup = file = FMOD_Object_Alloc(NullFile);
        CHECK_RESULT(file == NULL ? FMOD_ERR_MEMORY : FMOD_OK);

        file->init(this, 0, mBufferSize);
        mode_in &= ~FMOD_OPENRAW;   /* Just in case the user passed this in. */
    }
    else
    {
        if (exinfo && exinfo->useropen && exinfo->userclose && exinfo->userread && exinfo->userseek && !exinfo->ignoresetfilesystem)
        {
            file_cleanup = file = FMOD_Object_Alloc(UserFile);
            CHECK_RESULT(file == NULL ? FMOD_ERR_MEMORY : FMOD_OK);

            result = ((UserFile *)file)->setUserCallbacks(exinfo->useropen, exinfo->userclose, exinfo->userread, exinfo->userseek);
            CHECK_RESULT(result);

            file->init(this, 0, mBufferSize);
        }
        else if (mUsesUserCallbacks && (!exinfo || !exinfo->ignoresetfilesystem))
        {
            file_cleanup = file = FMOD_Object_Alloc(UserFile);
            CHECK_RESULT(file == NULL ? FMOD_ERR_MEMORY : FMOD_OK);

            file->init(this, 0, mBufferSize);
        }
#ifdef FMOD_SUPPORT_NET
        else if ((mode_in & FMOD_UNICODE &&
                 (!FMOD_strnicmpW((const short *)(L"http://"),      (const short *)name_or_data, 7) || 
                  !FMOD_strnicmpW((const short *)(L"http:\\\\"),    (const short *)name_or_data, 7) || 
                  !FMOD_strnicmpW((const short *)(L"https://"),     (const short *)name_or_data, 8) || 
                  !FMOD_strnicmpW((const short *)(L"https:\\\\"),   (const short *)name_or_data, 8) || 
                  !FMOD_strnicmpW((const short *)(L"mms://"),       (const short *)name_or_data, 6) ||
                  !FMOD_strnicmpW((const short *)(L"mms:\\\\"),     (const short *)name_or_data, 6))) ||
                 (!FMOD_strnicmp("http://",                         name_or_data, 7) || 
                  !FMOD_strnicmp("http:\\\\",                       name_or_data, 7) || 
                  !FMOD_strnicmp("https://",                        name_or_data, 8) || 
                  !FMOD_strnicmp("https:\\\\",                      name_or_data, 8) || 
                  !FMOD_strnicmp("mms://",                          name_or_data, 6) ||
                  !FMOD_strnicmp("mms:\\\\",                        name_or_data, 6)))
        {
            file_cleanup = file = FMOD_Object_Alloc(NetFile);
            CHECK_RESULT(file == NULL ? FMOD_ERR_MEMORY : FMOD_OK);
            
            file->init(this, 0, mBufferSize);
            netfile = true;
        }
#endif
#ifdef FMOD_SUPPORT_CDDA
        else if (FMOD_OS_CDDA_IsDeviceName((char *)name_or_data))
        {
            file_cleanup = file = FMOD_Object_Alloc(CddaFile);
            CHECK_RESULT(file == NULL ? FMOD_ERR_MEMORY : FMOD_OK);

            result = ((CddaFile *)file)->init((mode_in & FMOD_CDDA_FORCEASPI) ? true : false, (mode_in & FMOD_CDDA_JITTERCORRECT) ? true : false);
            CHECK_RESULT(result);

            file->init(this, 0, mBufferSize);
        }
#endif
        else
        {
            file_cleanup = file = FMOD_Object_Alloc(DiskFile);
            CHECK_RESULT(file == NULL ? FMOD_ERR_MEMORY : FMOD_OK);

            file->init(this, 0, mBufferSize);
        }
    }

    if (*sound && netfile)
    {
        (*sound)->mOpenState = FMOD_OPENSTATE_CONNECTING;
    }

    if (mode_in & FMOD_CREATESTREAM && file->mBlockSize && file->mBlockSize <= (16*1024))
    {
        filebufferstack = (void *)FMOD_ALIGNPOINTER(filebufferstackmem, 32);

        /*
            Stop it from doing an alloc, so that it doesn't fragment memory when it reallocs later.
            If it failed (not enough stack) it will just do the normal realloc/heap thing.
        */
        file->setBuffer(filebufferstack);
    }

    if (mode_in & FMOD_CREATESTREAM)
    {
        /*
            Set the streaming flag. This is for PS3 FIOS.
        */
        file->mFlags |= FMOD_FILE_STREAMING;
    }

    result = file->open(name_or_data, exinfo ? exinfo->length : 0, (mode_in & FMOD_UNICODE) ? 1 : 0, exinfo ? exinfo->encryptionkey : 0);
    if (result != FMOD_OK)
    {
        return result;
    }
    file_closer = file;

    if (*sound && netfile)
    {
        (*sound)->mOpenState = FMOD_OPENSTATE_BUFFERING;
    }

    result = file->setStartOffset(exinfo ? exinfo->fileoffset: 0);
    if (result != FMOD_OK)
    {
        return result;
    }
   
    /*
        If the file exists, search for the appropriate codec.
    */
    result = mPluginFactory->getNumCodecs(&numcodecs);
    if (result != FMOD_OK)
    {
        return result;
    }

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "%d codecs found.  Scan all until one succeeds\n", numcodecs));

    result = FMOD_ERR_FORMAT;

    for (count = 0; count < numcodecs; count++)
    {
        FMOD_CODEC_DESCRIPTION_EX *codecdesc;
        unsigned int handle;

        result = mPluginFactory->getCodecHandle(count, &handle);
        if (result != FMOD_OK)
        {
            continue;
        }

        result = mPluginFactory->getCodec(handle, &codecdesc);
        if (result != FMOD_OK)
        {
            continue;
        }

        if (exinfo)
        {
            int memberoffset = (int)((FMOD_UINT_NATIVE)&exinfo->suggestedsoundtype - (FMOD_UINT_NATIVE)exinfo);
            
            if (exinfo->cbsize > memberoffset && exinfo->suggestedsoundtype && !(mode_in & FMOD_OPENUSER))
            {
                /*
                    Ignore any codec that isn't the suggested type, except when the suggested type
                    is MPEG, we want to allow the tag codec to process any tags in the file first
                */
                if (codecdesc->mType != exinfo->suggestedsoundtype 
#ifdef FMOD_SUPPORT_TAGS
                    && !(exinfo->suggestedsoundtype == FMOD_SOUND_TYPE_MPEG && codecdesc->mType == FMOD_SOUND_TYPE_TAG)
#endif
                    )
                {
                    continue;
                }
            }
        }

        if (mode_in & FMOD_OPENRAW)
        {
            if (codecdesc->mType != FMOD_SOUND_TYPE_RAW)
            {
                codecdesc = 0;
                continue;
            }
        }
        else if (mode_in & FMOD_OPENUSER)
        {
            if (codecdesc->mType != FMOD_SOUND_TYPE_USER)
            {
                codecdesc = 0;
                continue;
            }
        }
        else
        {
            if (codecdesc->mType == FMOD_SOUND_TYPE_RAW || codecdesc->mType == FMOD_SOUND_TYPE_USER)
            {
                codecdesc = 0;
                continue;
            }
        }

        result = mPluginFactory->createCodec(codecdesc, &codec);
        if (result != FMOD_OK)
        {
            if (result == FMOD_ERR_MEMORY)  /* Better not just skip this error. */
            {
                return result;
            }
            continue;
        }
        
        codec->mFile           = file;
        codec->mMode           = mode_in;
        codec->mOriginalMode   = originalmode;
        codec->filehandle      = file;
        codec->filesize        = file->mLength;
        codec->mSystem         = this;
        codec->mFlags         |= FMOD_CODEC_ACCURATELENGTH;
                   
        result = codec->mDescription.open(codec, mode_in, exinfo);

        if (result == FMOD_OK)
        {
            mode_in = codec->mMode; /* The codec wanted to change the mode. */
            
            result = codec->mDescription.getwaveformat(codec, 0, &waveformat);
            if (result != FMOD_OK)
            {
                return result;
            }
                              
            if (mode_in & FMOD_OPENUSER)
            {
                waveformat.format    = exinfo->format;
                waveformat.channels  = exinfo->numchannels;
                waveformat.frequency = exinfo->defaultfrequency;

                result = SoundI::getSamplesFromBytes(exinfo->length, &waveformat.lengthpcm, exinfo->numchannels, exinfo->format);
                if (result != FMOD_OK)
                {
                    return result;
                }

                waveformat.blockalign = codec->waveformat[0].blockalign = 1;    /* user codec can reference codec->waveformat here because it exists. */
            }
            else if (mode_in & FMOD_OPENRAW)
            {
                unsigned int filesize;

                file->getSize(&filesize);

                waveformat.format    = codec->waveformat[0].format    = exinfo->format;             /* raw codec can reference codec->waveformat here because it exists. */
                waveformat.channels  = codec->waveformat[0].channels  = exinfo->numchannels;        /* raw codec can reference codec->waveformat here because it exists. */
                waveformat.frequency = codec->waveformat[0].frequency = exinfo->defaultfrequency;   /* raw codec can reference codec->waveformat here because it exists. */

                result = SoundI::getSamplesFromBytes(filesize, &waveformat.lengthpcm, exinfo->numchannels, exinfo->format);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }

            maxchannels = waveformat.channels;
            codec->mBlockAlign = waveformat.blockalign; /* keep the first one always */

            if (codec->numsubsounds && mode_in & FMOD_CREATESTREAM)
            {
                for (count = 0; count < codec->numsubsounds; count++)
                {
                    FMOD_CODEC_WAVEFORMAT waveformat2;

                    result = codec->mDescription.getwaveformat(codec, count, &waveformat2);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }

                    if (waveformat2.channels > maxchannels)
                    {
                        maxchannels        = waveformat2.channels;
                        codec->mBlockAlign = waveformat2.blockalign;
                    }
                }
            }           

            #ifdef FMOD_SUPPORT_RAWCODEC
            if (waveformat.format == FMOD_SOUND_FORMAT_PCM16)
            {                  
                if (!mDSPCodecPool_RAW.mNumDSPCodecs)
                {
                    result = mDSPCodecPool_RAW.init(FMOD_DSP_CATEGORY_DSPCODECRAW, 256, mAdvancedSettings.maxPCMcodecs ? mAdvancedSettings.maxPCMcodecs : FMOD_ADVANCEDSETTINGS_MAXPCMCODECS);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }
            }
            #endif
            break;
        }

#ifdef FMOD_SUPPORT_TAGS
        if (codec->mType == FMOD_SOUND_TYPE_TAG && !metadata)
        {
            metadata = codec->mMetadata;
        }
#endif

        codec->mFile = 0;   /* Dont let the release close the file */
        codec->mMetadata = 0;
        codec->release();
        codec = 0;

        if (result != FMOD_ERR_FORMAT && result != FMOD_ERR_FILE_EOF)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "System::createSoundInternal", "Fatal error (%d) scanning the codecs.  (ie not FMOD_ERR_FORMAT or FMOD_ERR_FILE_EOF)\n", result));

            if (metadata)
            {
                metadata->release();
            }

            return result;
        }
    }

    if (!codec)
    {
        if (metadata)
        {
            metadata->release();
        }
        return FMOD_ERR_FORMAT;
    }

    numsubsounds = exinfo && exinfo->numsubsounds ? exinfo->numsubsounds : codec->numsubsounds;

    if (metadata)
    {
        if (codec->mMetadata)
        {
            codec->mMetadata->add(metadata);
            metadata->release();
        }
        else
        {
            codec->mMetadata = metadata;
        }    
    }

    codec->getMetadataFromFile();

    file_closer.releasePtr();
    file_cleanup.releasePtr();
    codec_cleanup = codec;    

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "Format has %d subsounds.\n", numsubsounds));

    if (numsubsounds > 0)
    {
        subsound = (SoundI **)FMOD_Memory_Calloc(numsubsounds * sizeof(SoundI *));
        if (!subsound)
        {
            return FMOD_ERR_MEMORY;
        }

        subsound_cleanup = subsound;
    }
   
    result = codec->mDescription.getwaveformat(codec, 0, &waveformat);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (waveformat.channels > mMaxInputChannels)
    {
        return FMOD_ERR_TOOMANYCHANNELS;
    }

    /*
        Load as a stream if the codec told us to.
    */
    if (codec->mDescription.defaultasstream && !(mode_in & FMOD_CREATESAMPLE))
    {
        mode_in |= FMOD_CREATESTREAM;
    }

    /*
        The file has now been successfully opened via the codec, now load it or prepare a stream.
    */
    if (codec->mType == FMOD_SOUND_TYPE_PLAYLIST)
    {
        /*
            Just create an empty sound container if a playlist was loaded
        */  
        SoundI             *soundcontainer = 0;
        AutoRelease<SoundI> soundcontainer_cleanup;

        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "Create sample for playlist\n"));

        if (mode_in & FMOD_NONBLOCKING)
        {
            soundcontainer = *sound;
        }
        else
        {
            soundcontainer = FMOD_Object_Calloc(Sample);
            if (!soundcontainer)
            {
                return FMOD_ERR_MEMORY;
            }
            soundcontainer_cleanup = soundcontainer;
        }

        if (!soundcontainer->mName && !(mode_in & FMOD_LOWMEM))
        {
            soundcontainer->mName = (char *)FMOD_Memory_Calloc(FMOD_STRING_MAXNAMELEN);
            if (!soundcontainer->mName)
            {
                return FMOD_ERR_MEMORY;
            }
        }

        codec_cleanup.releasePtr();

        soundcontainer->mSystem                  = this;
        soundcontainer->mPostReadCallback        = exinfo ? exinfo->pcmreadcallback : 0;
        soundcontainer->mPostSetPositionCallback = exinfo ? exinfo->pcmsetposcallback : 0;
        soundcontainer->mPostCallbackSound       = (FMOD_SOUND *)soundcontainer;
        soundcontainer->mType                    = codec->mType;
        soundcontainer->mCodec                   = codec;
        soundcontainer->mMinDistance             = 1.0f     * mDistanceScale;
        soundcontainer->mMaxDistance             = 10000.0f * mDistanceScale;
        soundcontainer->mSubSound                = 0;
        soundcontainer->mNumSubSounds            = 0;
        soundcontainer->mSubSoundIndex           = -1;
        soundcontainer->mUserData                = exinfo ? exinfo->userdata : 0;

        soundcontainer_cleanup.releasePtr();
        *sound = soundcontainer;
    }
    else
#ifdef FMOD_SUPPORT_STREAMING
    if (mode_in & FMOD_CREATESTREAM)
    {
        unsigned int          decodebuffersize;
        unsigned int          blocksize = 0;
        Stream               *stream;
        Sample               *sample      = 0;
        FMOD_MODE             sample_mode = mode_in & ~(FMOD_CREATECOMPRESSEDSAMPLE);
        FMOD_CODEC_WAVEFORMAT waveformatstream;
        AutoRelease<Stream>   stream_cleanup;
        
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "Create as FMOD_CREATESTREAM\n"));

        if (mode_in & FMOD_NONBLOCKING)
        {
            stream = SAFE_CAST(Stream, *sound);
        }
        else
        {
            stream = FMOD_Object_Calloc(Stream);
            if (!stream)
            {
                return FMOD_ERR_MEMORY;
            }
            stream_cleanup = stream;
        }       

        if (!stream->mName && !(mode_in & FMOD_LOWMEM))
        {
            stream->mName = (char *)FMOD_Memory_Calloc(FMOD_STRING_MAXNAMELEN);
            if (!stream->mName)
            {
                return FMOD_ERR_MEMORY;
            }
        }

        codec_cleanup.releasePtr();        
        subsound_cleanup.releasePtr();

        stream->mSubSound     = subsound;
        stream->mNumSubSounds = numsubsounds;
        stream->mCodec        = codec;
        stream->mSystem       = this;

        if (!(codec->mFlags & FMOD_CODEC_HARDWAREMUSICVOICES))
        {
            stream->getSamplesFromBytes(codec->mBlockAlign, &blocksize, maxchannels, waveformat.format);

            /*
                Calculate the PCM Buffersize in samples based on the user setting.
            */
            decodebuffersize = exinfo ? exinfo->decodebuffersize : 0;

            if (!decodebuffersize)
            {
                decodebuffersize  = mAdvancedSettings.defaultDecodeBufferSize;
                decodebuffersize *= waveformat.frequency;
                decodebuffersize /= 1000;
            }

            if (!blocksize)
            {
                blocksize = decodebuffersize;
            }

            if (blocksize < 256)
            {
                if (blocksize <= 0)
                {
                    blocksize = 1;
                }

                blocksize = ((256 + (blocksize - 1)) / blocksize) * blocksize;
            }

            decodebuffersize /= blocksize;
            decodebuffersize *= blocksize;
            if (decodebuffersize <= blocksize)
            {
                decodebuffersize = blocksize * 2;
            }

            if (!(mode_in & FMOD_OPENUSER) && numsubsounds == 0 && waveformat.lengthpcm <= decodebuffersize && sample_mode & FMOD_SOFTWARE)
            {
                decodebuffersize = waveformat.lengthpcm;
            }

            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "System::createSoundInternal", "decode buffersize = %d : blocksize = %d : format = %d\n", decodebuffersize, blocksize, waveformat.format));

            /*
                Create a small streaming sample to double buffer into
            */
            sample_mode &= ~FMOD_LOOP_OFF;
            sample_mode &= ~FMOD_LOOP_BIDI;
            sample_mode &= ~FMOD_OPENMEMORY_POINT;
            sample_mode |=  FMOD_LOOP_NORMAL;

            FMOD_memcpy(&waveformatstream, &waveformat, sizeof(FMOD_CODEC_WAVEFORMAT));

            if (exinfo && exinfo->speakermap)
            {
                if (exinfo->speakermap == FMOD_SPEAKERMAPTYPE_ALLMONO)
                {
                    waveformatstream.channelmask = SPEAKER_ALLMONO;
                }
                else if (exinfo->speakermap == FMOD_SPEAKERMAPTYPE_ALLSTEREO)
                {
                    waveformatstream.channelmask = SPEAKER_ALLSTEREO;
                }
                else if (exinfo->speakermap == FMOD_SPEAKERMAPTYPE_51_PROTOOLS)
                {
                    waveformatstream.channelmask = SPEAKER_PROTOOLS;
                }
            }

            waveformatstream.lengthpcm = decodebuffersize;
            waveformatstream.channels  = maxchannels;
            waveformatstream.loopstart = 0;
            waveformatstream.loopend   = waveformatstream.lengthpcm - 1;
                
            result = createSample(sample_mode, &waveformatstream, &sample);
            if (result != FMOD_OK)
            {
                return result;
            }

            /*
                Put in any post sample create info from the user. 
            */
            sample->mPostReadCallback        = exinfo ? exinfo->pcmreadcallback : 0;
            sample->mPostSetPositionCallback = exinfo ? exinfo->pcmsetposcallback : 0;
            sample->mUserData                = exinfo ? exinfo->userdata : 0;
            sample->mPostCallbackSound       = (FMOD_SOUND *)stream;
            sample->mCodec                   = codec;

            if (sample->mNumSubSamples)
            {
                for (int count = 0; count < sample->mNumSubSamples; count++)
                {
                    sample->mSubSample[count]->mCodec = codec;
                }
            }
        }

        /*
            Set up stream members.
        */
        stream->mSample                  = sample;
        stream->mSubSoundIndex           = 0;
        stream->mLength                  = waveformat.lengthpcm;
        stream->mLengthBytes             = waveformat.lengthbytes;
        stream->mDefaultFrequency        = (float)waveformat.frequency;
        stream->mMode                    = sample_mode | FMOD_UNIQUE | FMOD_CREATESTREAM;
        stream->mMode                    = stream->mMode & ~FMOD_LOOP_NORMAL;    /* Don't inherit the loop mode */
        stream->mFormat                  = waveformat.format;
        stream->mChannels                = maxchannels;
        stream->mSystem                  = this;
        stream->mPostReadCallback        = exinfo ? exinfo->pcmreadcallback : 0;
        stream->mPostSetPositionCallback = exinfo ? exinfo->pcmsetposcallback : 0;
        stream->mPostCallbackSound       = (FMOD_SOUND *)stream;
        stream->mBlockSize               = blocksize;
        stream->mType                    = codec->mType;
        stream->mMinDistance             = 1.0f     * mDistanceScale;
        stream->mMaxDistance             = 10000.0f * mDistanceScale;
        stream->mUserData                = exinfo ? exinfo->userdata : 0;
        if (stream->mName)
        {
            FMOD_strcpy(stream->mName, waveformat.name);
        }

        if (sample)
        {
            if (exinfo && exinfo->speakermap)
            {
                if (exinfo->speakermap == FMOD_SPEAKERMAPTYPE_ALLMONO)
                {
                    sample->mDefaultChannelMask = SPEAKER_ALLMONO;
                }
                else if (exinfo->speakermap == FMOD_SPEAKERMAPTYPE_ALLSTEREO)
                {
                    sample->mDefaultChannelMask = SPEAKER_ALLSTEREO;
                }
                else if (exinfo->speakermap == FMOD_SPEAKERMAPTYPE_51_PROTOOLS)
                {
                    sample->mDefaultChannelMask = SPEAKER_PROTOOLS;
                }
            }
            else
            {
                sample->mDefaultChannelMask = waveformat.channelmask;
            }

            /*
                Fix up the hardware/software and 2d/3d bits of the mode.
            */
            stream->mMode &= ~(FMOD_2D | FMOD_3D | FMOD_HARDWARE | FMOD_SOFTWARE);
            stream->mMode |= (sample->mMode & (FMOD_2D | FMOD_3D | FMOD_HARDWARE | FMOD_SOFTWARE));
        }
         
        /*
            Loop information.
        */
        if (waveformat.mode & FMOD_LOOP_OFF || mode_in & FMOD_LOOP_OFF)
        {
            stream->mMode |= FMOD_LOOP_OFF;
        }
        else if (waveformat.mode & FMOD_LOOP_NORMAL || mode_in & FMOD_LOOP_NORMAL)
        {
            stream->mMode |= FMOD_LOOP_NORMAL;
        }
        else
        {
            stream->mMode |= FMOD_LOOP_OFF;
        }
        stream->setLoopPoints(waveformat.loopstart, FMOD_TIMEUNIT_PCM, waveformat.loopend, FMOD_TIMEUNIT_PCM);

        if (!stream->mSoundGroupNode.getData())
        {
            result = stream->setSoundGroup(mSoundGroup);
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        /*
            Allocate stream channel
        */
        stream->mChannel = FMOD_Object_Calloc(ChannelStream);
        if (!stream->mChannel)
        {
            return FMOD_ERR_MEMORY;
        }

        stream->mChannel->mSystem = this;

        FMOD_OS_CriticalSection_Enter(SystemI::mStreamListCrit);
        {
            stream->mStreamNode.setData(stream);
            stream->mStreamNode.addBefore(&mStreamListSoundHead);
        }
        FMOD_OS_CriticalSection_Leave(SystemI::mStreamListCrit);    

        if (numsubsounds > 0 && !(mode_in & FMOD_OPENUSER))
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "System::createSoundInternal", "%d subsounds detected.\n", numsubsounds));

            Stream *substream = 0;

            substream = (Stream *)FMOD_Object_Calloc(Stream);
            if (!substream)
            {
                return FMOD_ERR_MEMORY;
            }
            AutoRelease<Stream> substream_cleanup(substream);

            if (numsubsounds > 1)
            {
                stream->mSubSoundShared = substream;
            }

            if (!(mode_in & FMOD_LOWMEM))
            {
                substream->mName = (char *)FMOD_Memory_Calloc(FMOD_STRING_MAXNAMELEN);
                if (!substream->mName)
                {
                    return FMOD_ERR_MEMORY;
                }
            }

            substream->mSystem           = this;
            substream->mSample           = sample;
            substream->mCodec            = codec;
            substream->mSubSoundIndex    = 0;
            substream->mChannel          = stream->mChannel;
            substream->mMode             = stream->mMode;
            substream->mType             = stream->mType;
            substream->mAsyncData        = stream->mAsyncData;
            substream->mMinDistance      = 1.0f     * mDistanceScale;
            substream->mMaxDistance      = 10000.0f * mDistanceScale;
            substream->mBlockSize        = blocksize;
            substream->mSubSoundParent   = stream;
            if (numsubsounds > 1)
            {
                substream->mSubSoundShared = substream;
            }

            if (!substream->mSoundGroupNode.getData())
            {
                result = substream->setSoundGroup(mSoundGroup);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }

            for (count=0; count < numsubsounds; count++)
            {
                subsound[count] = substream;
            
                if (count < codec->numsubsounds)
                {
                    FMOD_CODEC_WAVEFORMAT waveformat2;

                    result = codec->mDescription.getwaveformat(codec, count, &waveformat2);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }

                    if (substream->mName)
                    {
                        FMOD_strcpy(substream->mName,  waveformat2.name);
                    }
                    substream->mSubSoundIndex      = count;
                    substream->mFormat             = waveformat2.format;
                    substream->mChannels           = waveformat2.channels;
                    substream->mDefaultFrequency   = (float)waveformat2.frequency;
                    substream->mDefaultChannelMask = waveformat2.channelmask;
                    substream->mLoopStart          = waveformat2.loopstart;
                    substream->mLoopLength         = waveformat2.loopend - waveformat2.loopstart + 1;
                    substream->mLength             = waveformat2.lengthpcm;
                    substream->mLengthBytes        = waveformat2.lengthbytes;
                    substream->setLoopPoints(waveformat2.loopstart, FMOD_TIMEUNIT_PCM, waveformat2.loopend, FMOD_TIMEUNIT_PCM);

                    if (codec->mDescription.soundcreate)
                    {
                        result = codec->mDescription.soundcreate(codec, count, (FMOD_SOUND *)substream);
                        if (result != FMOD_OK)
                        {
                            return result;
                        }
                    }
                }

                stream->mNumActiveSubSounds++;
            }

            if (codec->numsubsounds && substream->mSubSoundShared)
            {
                substream->updateSubSound(0, false);
            }

            if (stream->mType == FMOD_SOUND_TYPE_CDDA && !(mode_in & FMOD_LOWMEM) && !(exinfo && exinfo->initialsubsound))
            {
                FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "System::createSoundInternal", "Setting up parent sound to contain a sentence of all subsounds.\n"));

                stream->setSubSoundSentence(0, numsubsounds);
            }

            substream_cleanup.releasePtr();
        }
        else
        {
            if (codec->mDescription.soundcreate)
            {
                result = codec->mDescription.soundcreate(codec, 0, (FMOD_SOUND *)stream);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
        }

        /*
            Convert the initial position time unit to PCM
        */
        if (exinfo)
        {
            switch (exinfo->initialseekpostype)
            {
                case 0:     // Defaults to FMOD_TIMEUNIT_PCM
                {
                    break;
                }
                case FMOD_TIMEUNIT_PCM: 
                {
                    break;
                }
                case FMOD_TIMEUNIT_PCMBYTES:
                {
                    stream->getSamplesFromBytes(exinfo->initialseekposition, &exinfo->initialseekposition);
                    exinfo->initialseekpostype = FMOD_TIMEUNIT_PCM;
                    break;
                }
                case FMOD_TIMEUNIT_MS: 
                {
                    exinfo->initialseekposition = (unsigned int)((float)exinfo->initialseekposition / 1000.0f * stream->mDefaultFrequency);
                    exinfo->initialseekpostype = FMOD_TIMEUNIT_PCM;
                    break;
                }
                default:    // Non-supported time unit chosen
                {
                    return FMOD_ERR_INVALID_PARAM;
                }
            }
        }
         
        if (exinfo && exinfo->initialsubsound && stream->mSubSound)
        {
            Stream *substream = 0;

            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "System::createSoundInternal", "Setting initial subsound. exinfo->initialsubsound = %d, stream->mNumSubSounds = %d.\n", exinfo->initialsubsound, stream->mNumSubSounds));

            if (exinfo->initialsubsound < 0 || exinfo->initialsubsound >= stream->mNumSubSounds)
            {
                return FMOD_ERR_INVALID_PARAM;
            }

            stream->mSubSoundIndex                  = exinfo->initialsubsound;
            stream->mChannel->mSubSoundListCurrent  = exinfo->initialsubsound;

            substream = SAFE_CAST(Stream, stream->mSubSound[stream->mSubSoundIndex]);
            if (substream)
            {
                substream->mSubSoundIndex                 = exinfo->initialsubsound;
                substream->mChannel->mSubSoundListCurrent = exinfo->initialsubsound;

                if (substream->mSubSoundShared)
                {
                    result = substream->updateSubSound(exinfo->initialsubsound, false);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
				}

                stream->mInitialPosition = exinfo->initialseekposition;
                result = substream->setPosition(exinfo->initialseekposition, FMOD_TIMEUNIT_PCM);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
        }
        else
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "System::createSoundInternal", "Seek stream to start\n"));

            /*
                Seek and pre-flush the buffer.
            */
            stream->mInitialPosition = exinfo ? exinfo->initialseekposition : 0;
            result = stream->setPosition(exinfo ? exinfo->initialseekposition : 0, FMOD_TIMEUNIT_PCM);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        
        if (!(mode_in & FMOD_OPENONLY) && !(codec->mFlags & FMOD_CODEC_HARDWAREMUSICVOICES))
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "System::createSoundInternal", "flush stream buffer\n"));

            result = stream->flush();
            if (result != FMOD_OK)
            {
                return result;
            }           

            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "System::createSoundInternal", "flush successful.\n"));
        }

        /*
            Change from a blocking single buffered sector sized file to a large doublebuffered file.
            Dont do it for mod files, they dont have a file buffer at runtime anyway.
        */
        if (!(mode_in & FMOD_OPENUSER) && !(codec->mFlags & FMOD_CODEC_HARDWAREMUSICVOICES) 
            && codec->mType != FMOD_SOUND_TYPE_IT
            && codec->mType != FMOD_SOUND_TYPE_XM
            && codec->mType != FMOD_SOUND_TYPE_S3M
            && codec->mType != FMOD_SOUND_TYPE_MOD
            && codec->mType != FMOD_SOUND_TYPE_MIDI)
        {
            unsigned int sizebytes = 0; 

            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "System::createSoundInternal", "switch file handle from small blocking single buffered to large nonblocking doublebuffered.\n"));

            if (buffersizetype == FMOD_TIMEUNIT_RAWBYTES)
            {
                sizebytes = buffersize;
            }
            else 
            {
                unsigned int pcm = buffersize;

                if (buffersizetype == FMOD_TIMEUNIT_MS)
                {                
                    pcm = buffersize * (unsigned int)stream->mDefaultFrequency / 1000;
                }
                else if (buffersizetype == FMOD_TIMEUNIT_PCMBYTES)
                {
                    stream->getSamplesFromBytes(buffersize, &pcm);
                }

                /*
                    This section tries to guess the size in bytes (sizebytes) it would take with 
                    this compressed format, to read the buffersize worth of output samples (pcm).
                */
                if (stream->mLength == (unsigned int)-1 || file->mLength == (unsigned int)-1)
                {
                    stream->getBytesFromSamples(pcm, &sizebytes);   /* In this case, length is infinite, so make a rough guess based on PCM bytes. */
                }
                else
                {
                    float frac;
                    
                    if (stream->mLengthBytes)
                    {
                        frac = (float)stream->mLengthBytes / (float)stream->mLength;
                    }
                    else
                    {
                        frac = (float)file->mLength / (float)stream->mLength;
                    }

                    sizebytes = (unsigned int)((float)pcm * frac);
                }
            }

            result = file->enableDoubleBuffer(sizebytes, filebufferstack);    /* Now double buffered, size determined by user */
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        stream_cleanup.releasePtr();
        *sound = stream;
    }
    else    /* ^^^^^^ (mode_in & FMOD_CREATESTREAM) ^^^^^^ */
#endif
    {
        Sample *sample = 0, *samplecontainer = 0;        
        AutoRelease<Sample> samplecontainer_cleanup;
        bool realtimesample = false;

        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "Create as FMOD_CREATESAMPLE\n"));

        if (numsubsounds > 0)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "Multi-sample sound (%d subsounds), create a sample container.\n", numsubsounds));

            if (mode_in & FMOD_NONBLOCKING)
            {
                samplecontainer = SAFE_CAST(Sample, *sound);
            }
            else
            {
                samplecontainer = FMOD_Object_Calloc(Sample);
                if (!samplecontainer)
                {
                    return FMOD_ERR_MEMORY;
                }
                samplecontainer_cleanup = samplecontainer;
            }

            if (!samplecontainer->mName && !(mode_in & FMOD_LOWMEM))
            {
                samplecontainer->mName = (char *)FMOD_Memory_Calloc(FMOD_STRING_MAXNAMELEN);
                if (!samplecontainer->mName)
                {
                    return FMOD_ERR_MEMORY;
                }
            }
            codec_cleanup.releasePtr();
            subsound_cleanup.releasePtr();

            samplecontainer->mSystem                  = this;
            samplecontainer->mPostReadCallback        = exinfo ? exinfo->pcmreadcallback : 0;
            samplecontainer->mPostSetPositionCallback = exinfo ? exinfo->pcmsetposcallback : 0;
            samplecontainer->mPostCallbackSound       = (FMOD_SOUND *)samplecontainer;
            samplecontainer->mType                    = codec->mType;
            samplecontainer->mCodec                   = codec;
            samplecontainer->mMinDistance             = 1.0f     * mDistanceScale;
            samplecontainer->mMaxDistance             = 10000.0f * mDistanceScale;
            samplecontainer->mSubSound                = subsound;
            samplecontainer->mNumSubSounds            = numsubsounds;
            samplecontainer->mSubSoundIndex           = -1;
            samplecontainer->mUserData                = exinfo ? exinfo->userdata : 0;
            samplecontainer->mMode                    = mode_in;
            samplecontainer->mDefaultFrequency        = (float)waveformat.frequency;
        }                                             

        for (count = 0; (count < (numsubsounds == 0 ? 1 : numsubsounds)) && (count < (numsubsounds == 0 ? 1 : codec->numsubsounds)); count++)
        {
            FMOD_MODE sample_mode = mode_in;
            FMOD_CODEC_WAVEFORMAT waveformat2;

            result = codec->mDescription.getwaveformat(codec, count, &waveformat2);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (!numsubsounds && (sample_mode & FMOD_NONBLOCKING))
            {
                sample = SAFE_CAST(Sample, *sound);
            }
            else
            {
                sample = 0;
            }

            /*
                Check the user passed in inclusion list if this sample should be loaded or not.
            */
            if (numsubsounds > 0 && exinfo && exinfo->inclusionlist)
            {
                bool found = false;
                int count2;

                for (count2 = 0; count2 < exinfo->inclusionlistnum; count2++)
                {
                    if (exinfo->inclusionlist[count2] == count)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    continue;
                }
            }

            if (exinfo && exinfo->speakermap)
            {
                if (exinfo->speakermap == FMOD_SPEAKERMAPTYPE_ALLMONO)
                {
                    waveformat2.channelmask = SPEAKER_ALLMONO;
                }
                else if (exinfo->speakermap == FMOD_SPEAKERMAPTYPE_ALLSTEREO)
                {
                    waveformat2.channelmask = SPEAKER_ALLSTEREO;
                }
                else if (exinfo->speakermap == FMOD_SPEAKERMAPTYPE_51_PROTOOLS)
                {
                    waveformat2.channelmask = SPEAKER_PROTOOLS;
                }
            }

            if (sample_mode & FMOD_CREATECOMPRESSEDSAMPLE)
            {
                if (waveformat2.format == FMOD_SOUND_FORMAT_MPEG || 
                    waveformat2.format == FMOD_SOUND_FORMAT_IMAADPCM || 
                    waveformat2.format == FMOD_SOUND_FORMAT_XMA ||
                    waveformat2.format == FMOD_SOUND_FORMAT_CELT)       /* Only allow these formats through as realtime samples. */
                {
                    realtimesample = true;
                }
                else
                {
                    sample_mode &= ~FMOD_CREATECOMPRESSEDSAMPLE;

                    if (samplecontainer)
                    {
                        samplecontainer->mMode &= ~FMOD_CREATECOMPRESSEDSAMPLE;
                    }
                }
            }

            if (!realtimesample && mode_in & FMOD_OPENMEMORY_POINT)
            {               
                result = codec->canPointTo();
                if (result != FMOD_OK)
                {              
                    return result;
                }
            }

            /*
                Create the sample
            */
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "creating subsound %d/%d\n", count, numsubsounds));

            result = createSample(sample_mode, &waveformat2, &sample);
            if (result != FMOD_OK)
            {
                return result;
            }

            /*
                Put in any post sample create info from the user. 
            */
            codec_cleanup.releasePtr();
            sample->mSubSoundIndex  = count;
            sample->mSubSoundParent = samplecontainer;
            sample->mType           = codec->mType;
            sample->mCodec          = codec;            
            sample->mMinDistance    = 1.0f     * mDistanceScale;
            sample->mMaxDistance    = 10000.0f * mDistanceScale;
            sample->mUserData       = exinfo ? exinfo->userdata : 0;

            if (sample->mNumSubSamples)
            {
                for (int count = 0; count < sample->mNumSubSamples; count++)
                {
                    sample->mSubSample[count]->mCodec         = codec;
                    sample->mSubSample[count]->mSubSoundIndex = sample->mSubSoundIndex;
                }
            }

            if (!sample->mSoundGroupNode.getData())
            {
                result = sample->setSoundGroup(mSoundGroup);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
            
            /*
                Now load it.
            */
            if (!numsubsounds && count == 0)
            {
                sample->mPostReadCallback        = exinfo ? exinfo->pcmreadcallback : 0;
                sample->mPostSetPositionCallback = exinfo ? exinfo->pcmsetposcallback : 0;
                sample->mPostCallbackSound       = (FMOD_SOUND *)sample;
            }

            if (codec->mDescription.soundcreate)
            {
                SoundI *s = SAFE_CAST(SoundI, sample);

                result = codec->mDescription.soundcreate(codec, count, (FMOD_SOUND *)s);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }

            result = codec->reset();    /* Reset the codec if it needs resetting, and clear PCM Buffer and reset it. */
            if (result != FMOD_OK)
            {
                return result;
            }

            if (waveformat2.mode & FMOD_CREATECOMPRESSEDSAMPLE || sample_mode & FMOD_CREATECOMPRESSEDSAMPLE)
            {
                result = codec->setPosition(count, 0, FMOD_TIMEUNIT_RAWBYTES);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
            else
            {
                result = codec->setPosition(count, 0, FMOD_TIMEUNIT_PCM);
                if (result != FMOD_OK)
                {
                    return result;
                }            
            }

            if (sample->mPostSetPositionCallback)
            {
                if (numsubsounds > 0)
                {
                    samplecontainer->mPostSetPositionCallback((FMOD_SOUND *)samplecontainer, count, 0, FMOD_TIMEUNIT_PCM);
                }
                else
                {
                    sample->mPostSetPositionCallback((FMOD_SOUND *)sample, 0, 0, FMOD_TIMEUNIT_PCM);
                }
            }

            /*
                Get the data for the sample (from the file or just point to it).
            */
            if (sample_mode & FMOD_OPENMEMORY_POINT)   /* If we are simply pointing to the user's memory address. */
            {
                if (codec->mNonInterleaved && sample->mNumSubSamples)
                {
                    unsigned int onechanlengthbytes = 0;

                    sample->getBytesFromSamples(sample->mLength / sample->mNumSubSamples, &onechanlengthbytes);

                    for (int count = 0; count < sample->mNumSubSamples; count++)
                    {
                        result = ((Sample *)(sample->mSubSample[count]))->setBufferData((char *)name_or_data + codec->mFile->mCurrentPosition + (onechanlengthbytes * count));
                        if (result != FMOD_OK)
                        {
                            if (samplecontainer || codec)
                            {
                                sample->release();              /* Release it here because it hasnt been made a subsound yet. */
                            }
                            return result;
                        }
                    }
                }
                else
                {
                    result = sample->setBufferData((char *)name_or_data + codec->mFile->mCurrentPosition);
                    if (result != FMOD_OK)
                    {
                        if (samplecontainer || codec)
                        {
                            sample->release();              /* Release it here because it hasnt been made a subsound yet. */
                        }
                        return result;
                    }
                }
            }
            else if (!(sample_mode & FMOD_OPENONLY))
            {
                unsigned int samplelength, read;

                if (waveformat2.mode & FMOD_CREATECOMPRESSEDSAMPLE || sample_mode & FMOD_CREATECOMPRESSEDSAMPLE)
                {
                    samplelength = waveformat2.lengthbytes;

                    if (codec->mType != FMOD_SOUND_TYPE_FSB && codec->mType != FMOD_SOUND_TYPE_XMA)
                    {
                        samplelength -= codec->mSrcDataOffset;
                    }
                }
                else
                {
                    samplelength = waveformat2.lengthpcm;
                }

                result = sample->read(0, samplelength, &read);
   	            if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
                {
                    if (!(sample->mMode & FMOD_NONBLOCKING) && (samplecontainer || codec))
                    {
                        sample->release();              /* Release it here because it hasnt been made a subsound yet. */
                    }
                    return result;
                }
               
                if (samplelength != read && sample->mLoopLength == sample->mLength)
                {
                    sample->mLoopLength = read;
                }

                /*
                    Now after the read, set the position back to 0 again.
                */            
                result = sample->setPositionInternal(0);
                if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
                {
                    return result;
                }
            }

            if (numsubsounds > 0)
            {
                subsound[count] = sample;
                
                samplecontainer->mNumActiveSubSounds++;
            }
        }

        // Don't close the file handle for DLS files with inclusion lists, MIDI will
        // need to specify which sub sounds to load after the DLS is created.
        if (!(mode_in & FMOD_OPENONLY) && !(codec->mType == FMOD_SOUND_TYPE_DLS && numsubsounds > 0 && exinfo && exinfo->inclusionlist))
        {
            if (file)
            {
                file->close();
                FMOD_Memory_Free(file);
            }
            codec->mFile = 0;
#if 0
            if (!(mode_in & FMOD_CREATECOMPRESSEDSAMPLE))
            {
                codec->release();
                codec = 0;
                sample->mCodec = 0;
            }
#endif
        }

        if (numsubsounds > 0)
        {            
            samplecontainer_cleanup.releasePtr();
            *sound = samplecontainer;
        }
        else
        {
            *sound = sample;
        }
    }

    soundi = SAFE_CAST(SoundI, *sound);
    FMOD_OS_CriticalSection_Enter(gSoundListCrit);
    {
        soundi->addAfter(&mSoundListHead);
    }
    FMOD_OS_CriticalSection_Leave(gSoundListCrit);    
    
	if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "System::createSoundInternal", "sound open failed with error %d\n", result));
    
        if (!(mode_in & FMOD_NONBLOCKING))
        {
            *sound = 0;
        }

        return result;
    }

    /*
        Put the name of the sound into the name field.
    */
    if (soundi->mName && ((mode_in & FMOD_UNICODE && !((short *)soundi->mName)[0]) || !soundi->mName[0]))
    {
        bool usefilename = true;

        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "No name found in file, use filename.\n"));

        /*
            First look for the name in a tag.
        */
        if (codec->mMetadata)
        {
            FMOD_TAG tag;

            result = codec->mMetadata->getTag("TITLE", 0, &tag);
            if (result == FMOD_OK)
            {
                FMOD_strncpy(soundi->mName, (char *)tag.data, FMOD_STRING_MAXNAMELEN);
                usefilename = false;

                /*
                    Update tag so it is set back to "updated" status
                */
                codec->mMetadata->addTag(tag.type, "TITLE", tag.data, tag.datalen, tag.datatype, true);
            }
            else
            {
                result = codec->mMetadata->getTag("TT2", 0, &tag);
                if (result == FMOD_OK)
                {
                    FMOD_strncpy(soundi->mName, (char *)tag.data, FMOD_STRING_MAXNAMELEN);
                    usefilename = false;

                    /*
                        Update tag so it is set back to "updated" status
                    */
                    codec->mMetadata->addTag(tag.type, "TT2", tag.data, tag.datalen, tag.datatype, true); 
                }
            }
        }

        if (usefilename && !(mode_in & (FMOD_OPENMEMORY | FMOD_OPENMEMORY_POINT)) && name_or_data)
        {
            if (mode_in & FMOD_UNICODE)
            {
                for (count = FMOD_strlenW((short *)name_or_data); count >= 0; count--)
                {
                    if (((short *)name_or_data)[count] == L'\\' || ((short *)name_or_data)[count] == L'/')
                    {
                        count++;
                        break;
                    }
                }
                if (count < 0)
                {
                    count = 0;
                }

                FMOD_strncpyW((short *)soundi->mName, (short *)name_or_data + count, FMOD_STRING_MAXNAMELEN / 2); 
            }
            else
            {
                for (count = FMOD_strlen(name_or_data); count >= 0; count--)
                {
                    if (name_or_data[count] == '\\' || name_or_data[count] == '/')
                    {
                        count++;
                        break;
                    }
                }
                if (count < 0)
                {
                    count = 0;
                }

                FMOD_strncpy(soundi->mName, (char *)name_or_data + count, FMOD_STRING_MAXNAMELEN); 
            }
        }
    }

    if (!calledfromasync)
    {
        soundi->mOpenState   = FMOD_OPENSTATE_READY;
    }

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSoundInternal", "done.  OpenState now = FMOD_OPENSTATE_READY.\n\n"));

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
FMOD_RESULT SystemI::checkDriverList(bool fromsystemupdate)
{
    const unsigned int  UPDATEFREQUENCYMS   = 1000;
    FMOD_RESULT         result              = FMOD_OK;
    bool                devicelistchanged   = false;
    unsigned int        timestamp           = 0;

    if (fromsystemupdate && !mCallback)
    {
        return FMOD_OK;
    }    
    
    FMOD_OS_Time_GetMs(&timestamp);
    if (!fromsystemupdate || ((timestamp - mDeviceListLastCheckedTime) >= UPDATEFREQUENCYMS))
    {
        mDeviceListLastCheckedTime = timestamp;

        result = FMOD_OS_CheckDriverList(&devicelistchanged);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (devicelistchanged)
        {
            mDeviceListChanged = true;

            /*
                Force the output mode to re-enumerate on next driver list related call.
            */
            mOutput->mEnumerated        = false;
            #ifdef FMOD_SUPPORT_RECORDING            
		    mOutput->mRecordEnumerated  = false;
            #endif
        }
    }

    if (fromsystemupdate && mDeviceListChanged)
    {
        mCallback((FMOD_SYSTEM *)this, FMOD_SYSTEM_CALLBACKTYPE_DEVICELISTCHANGED, 0, 0);
        mDeviceListChanged = false;
    }

    return FMOD_OK;
}

/*
    ====================================================================================

    PUBLIC MEMBER FUNCTIONS

    ====================================================================================
*/

#ifndef FMOD_STATICFORPLUGINS

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
// OFFSET: 0x008D35C0
FMOD_RESULT SystemI::setUpPlugins()
{
    FMOD_RESULT result;
    AutoReleaseClear<PluginFactory*> pluginFactoryCleanup;

    /*
        Create the plugin factory here, because it is needed pre-init for output plugins!
    */
    mPluginFactory = FMOD_Object_Alloc(FMOD::PluginFactory);
    if (!mPluginFactory)
    {
        return FMOD_ERR_MEMORY;
    }

    pluginFactoryCleanup = &mPluginFactory;

    result = mPluginFactory->setSystem(this);
    if (result != FMOD_OK)
    {
        return result;
    }

    /*
        Register plugins for fmod to use
    */
    mPluginFactory->setPluginPath(mPluginPath);

    /* 
        Register platform specific outputs 
    */
    CHECK_RESULT(FMOD_OS_Output_Register(mPluginFactory));

    /* 
        Register cross platform outputs 
    */
#ifdef FMOD_USE_PLUGINS

    CHECK_RESULT(mPluginFactory->tryLoadPlugin("output_wavwriter"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("output_wavwriter_nrt"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("output_nosound")); 
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("output_nosound_nrt")); 

#else

    #ifdef FMOD_SUPPORT_WAVWRITER
    CHECK_RESULT(mPluginFactory->registerOutput(FMOD::OutputWavWriter::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_WAVWRITER_NRT
    CHECK_RESULT(mPluginFactory->registerOutput(FMOD::OutputWavWriter_NRT::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_NOSOUND
    CHECK_RESULT(mPluginFactory->registerOutput(FMOD::OutputNoSound::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_NOSOUND_NRT
    CHECK_RESULT(mPluginFactory->registerOutput(FMOD::OutputNoSound_NRT::getDescriptionEx()));
    #endif

#endif
   




#ifdef FMOD_USE_PLUGINS
    /*
        Register file format codec plugins
    */
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_tag", 0, true, 100));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_cdda", 0, true, 200));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_fsb", &mFSBPluginHandle, true, 300));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_vag", 0, true, 500));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_wav", &mWAVPluginHandle, true, 600));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_oggvorbis", 0, true, 800));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_tremor", 0, true, 900));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_aiff", 0, true, 1000));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_flac", 0, true, 1100));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_mod", 0, true, 1200));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_s3m", 0, true, 1300));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_xm", 0, true, 1400));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_it", 0, true, 1500));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_midi", 0, true, 1600));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_dls", 0, true, 1700));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_sf2", 0, true, 1800));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_asf", 0, true, 1900));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_mpeg", &mMPEGPluginHandle, true, 2400));   /* Second last, due to slow 4k bytescan if mpeg format header isn't found */
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_playlist", 0, true, 2450));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("codec_celt", &mCELTPluginHandle, true, 2500));

    #ifdef FMOD_SUPPORT_RAW
    CHECK_RESULT(mPluginFactory->registerCodec(CodecRaw::getDescriptionEx()));
    #endif

    /*
        Register DSP plugins.
    */
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_oscillator"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_fft"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_lowpass"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_lowpass2"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_lowpass_simple"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_highpass"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_echo"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_delay"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_flange"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_tremolo"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_distortion"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_normalize"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_parameq"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_pitchshift"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_chorus"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_reverb"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_sfxreverb"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_itecho"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_compressor"));
    CHECK_RESULT(mPluginFactory->tryLoadPlugin("dsp_dolbyheadphones"));

#else
    
    /*
        Register file format codecs
    */   
    #ifdef FMOD_SUPPORT_TAGS
    CHECK_RESULT(mPluginFactory->registerCodec(CodecTag::getDescriptionEx(), 0, 100));
    #endif
    #ifdef FMOD_SUPPORT_CDDA
    CHECK_RESULT(mPluginFactory->registerCodec(CodecCDDA::getDescriptionEx(), 0, 200));
    #endif  
    #ifdef FMOD_SUPPORT_FSB
        CHECK_RESULT(mPluginFactory->registerCodec(CodecFSB::getDescriptionEx(), &mFSBPluginHandle, 300));
        #ifdef FMOD_FSB_USEHEADERCACHE
        CodecFSB::gCacheHead.initNode();
        #endif
    #endif
    #ifdef FMOD_SUPPORT_GCADPCM_DSP
    CHECK_RESULT(mPluginFactory->registerCodec(CodecDSP::getDescriptionEx(), 0, 400));
    #endif
    #ifdef FMOD_SUPPORT_VAG
    CHECK_RESULT(mPluginFactory->registerCodec(CodecVAG::getDescriptionEx(), 0, 500));
    #endif
    #ifdef FMOD_SUPPORT_WAV
    CHECK_RESULT(mPluginFactory->registerCodec(CodecWav::getDescriptionEx(), &mWAVPluginHandle, 600)); 
    #endif
    #ifdef FMOD_SUPPORT_AT3
    CHECK_RESULT(mPluginFactory->registerCodec(CodecAT3::getDescriptionEx(), 0, 700));
    #endif
    #ifdef FMOD_SUPPORT_OGGVORBIS
    CHECK_RESULT(mPluginFactory->registerCodec(CodecOggVorbis::getDescriptionEx(), 0, 800));
    #endif
    #ifdef FMOD_SUPPORT_TREMOR
    CHECK_RESULT(mPluginFactory->registerCodec(CodecTremor::getDescriptionEx(), 0, 900));
    #endif
    #ifdef FMOD_SUPPORT_AIFF
    CHECK_RESULT(mPluginFactory->registerCodec(CodecAIFF::getDescriptionEx(), 0, 1000));
    #endif
    #ifdef FMOD_SUPPORT_FLAC
    CHECK_RESULT(mPluginFactory->registerCodec(CodecFLAC::getDescriptionEx(), 0, 1100));
    #endif
    #ifdef FMOD_SUPPORT_MOD
    CHECK_RESULT(mPluginFactory->registerCodec(CodecMOD::getDescriptionEx(), 0, 1200));
    #endif
    #ifdef FMOD_SUPPORT_S3M
    CHECK_RESULT(mPluginFactory->registerCodec(CodecS3M::getDescriptionEx(), 0, 1300));
    #endif
    #ifdef FMOD_SUPPORT_XM
    CHECK_RESULT(mPluginFactory->registerCodec(CodecXM::getDescriptionEx(), 0, 1400));
    #endif
    #ifdef FMOD_SUPPORT_IT
    CHECK_RESULT(mPluginFactory->registerCodec(CodecIT::getDescriptionEx(), 0, 1500));
    #endif
    #ifdef FMOD_SUPPORT_MIDI
    CHECK_RESULT(mPluginFactory->registerCodec(CodecMIDI::getDescriptionEx(), 0, 1600));
    #endif
    #ifdef FMOD_SUPPORT_DLS
    CHECK_RESULT(mPluginFactory->registerCodec(CodecDLS::getDescriptionEx(), 0, 1700));
    #endif
    #ifdef FMOD_SUPPORT_SF2
    CHECK_RESULT(mPluginFactory->registerCodec(CodecSF2::getDescriptionEx(), 0, 1800));
    #endif
    #ifdef FMOD_SUPPORT_ASF
    CHECK_RESULT(mPluginFactory->registerCodec(CodecASF::getDescriptionEx(), 0, 1900));
    #endif
    #ifdef FMOD_SUPPORT_XMA
    CHECK_RESULT(mPluginFactory->registerCodec(CodecXMA::getDescriptionEx(), 0, 2000));
    #endif
    #ifdef FMOD_SUPPORT_XWMA
    CHECK_RESULT(mPluginFactory->registerCodec(CodecXWMA::getDescriptionEx(), 0, 2100));
    #endif
    #ifdef FMOD_SUPPORT_MPEGPSP
    CHECK_RESULT(mPluginFactory->registerCodec(CodecMPEGPSP::getDescriptionEx(), 0, 2300));
    #endif
    #ifdef FMOD_SUPPORT_MPEG
    CHECK_RESULT(mPluginFactory->registerCodec(CodecMPEG::getDescriptionEx(), &mMPEGPluginHandle, 2400));   /* Last, due to slow 4k bytescan if mpeg format header isn't found */
    #endif
    #ifdef FMOD_SUPPORT_PLAYLIST
    CHECK_RESULT(mPluginFactory->registerCodec(CodecPlaylist::getDescriptionEx(), 0, 2450));
    #endif
    #ifdef FMOD_SUPPORT_RAW
    CHECK_RESULT(mPluginFactory->registerCodec(CodecRaw::getDescriptionEx(), 0, 2500));
    #endif
    #ifdef FMOD_SUPPORT_CELT
    CHECK_RESULT(mPluginFactory->registerCodec(CodecCELT::getDescriptionEx(), &mCELTPluginHandle, 2600));
    #endif


    /*
        Register DSP plugins.
    */
    #ifdef FMOD_SUPPORT_OSCILLATOR
    CHECK_RESULT(mPluginFactory->registerDSP(DSPOscillator::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_LOWPASS
    CHECK_RESULT(mPluginFactory->registerDSP(DSPLowPass::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_LOWPASS2
    CHECK_RESULT(mPluginFactory->registerDSP(DSPLowPass2::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_LOWPASS_SIMPLE
    CHECK_RESULT(mPluginFactory->registerDSP(DSPLowPassSimple::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_HIGHPASS
    CHECK_RESULT(mPluginFactory->registerDSP(DSPHighPass::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_ECHO
    CHECK_RESULT(mPluginFactory->registerDSP(DSPEcho::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_DELAY
    CHECK_RESULT(mPluginFactory->registerDSP(DSPDelay::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_FLANGE
    CHECK_RESULT(mPluginFactory->registerDSP(DSPFlange::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_TREMOLO
    CHECK_RESULT(mPluginFactory->registerDSP(DSPTremolo::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_DISTORTION
    CHECK_RESULT(mPluginFactory->registerDSP(DSPDistortion::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_NORMALIZE
    CHECK_RESULT(mPluginFactory->registerDSP(DSPNormalize::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_PARAMEQ
    CHECK_RESULT(mPluginFactory->registerDSP(DSPParamEq::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_PITCHSHIFT
    CHECK_RESULT(mPluginFactory->registerDSP(DSPPitchShift::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_CHORUS
    CHECK_RESULT(mPluginFactory->registerDSP(DSPChorus::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_FREEVERB
    CHECK_RESULT(mPluginFactory->registerDSP(DSPReverb::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_ITECHO
    CHECK_RESULT(mPluginFactory->registerDSP(DSPITEcho::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_COMPRESSOR
    CHECK_RESULT(mPluginFactory->registerDSP(DSPCompressor::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_SFXREVERB
    CHECK_RESULT(mPluginFactory->registerDSP(DSPSfxReverb::getDescriptionEx()));
    #endif

#endif

    /*
        Create the built in 'user' codec.  This is for when people create a sound with FMOD_CREATEUSER
    */
    #ifdef FMOD_SUPPORT_USERCODEC
    CHECK_RESULT(mPluginFactory->registerCodec(CodecUser::getDescriptionEx(), 0, 2600));
    #endif

    pluginFactoryCleanup.releasePtr();

    mPluginsLoaded = true;

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
// OFFSET: 0x008D3980
FMOD_RESULT SystemI::sortSpeakerList()
{
    int count, count2;
    bool used[FMOD_SPEAKER_MAX];
    int outputchannels;

    if (mSpeakerMode == FMOD_SPEAKERMODE_RAW)
    {
        return FMOD_OK;
    }

    /*
        Make a sorted speaker list based on the angles.
    */
    for (count = 0; count < FMOD_SPEAKER_MAX; count++)
    {
        mSpeakerList[count] =0;
    }

    FMOD_memset(used, 0, sizeof(bool) * FMOD_SPEAKER_MAX);

    outputchannels = mMaxOutputChannels;
    if (mSpeakerMode == FMOD_SPEAKERMODE_QUAD || mSpeakerMode == FMOD_SPEAKERMODE_PROLOGIC)
    {
        outputchannels = 6; /* 3d pretends it has rear right/rear left for SetSpeakerMix, so 5 instead of 4. */
    }

    for (count = 0; count < outputchannels; count++)
    {
        /* initialise to some high value */
        float lowest = 2 * FMOD_ANGLESORT_REVOLUTION;

        for (count2 = 0; count2 < outputchannels; count2++)
        {
            if (mSpeaker[count2].mSpeaker == FMOD_SPEAKER_LOW_FREQUENCY)
            {
                continue;
            }
            if (!mSpeaker[count2].mActive)
            {
                continue;
            }
            if (mSpeakerMode == FMOD_SPEAKERMODE_QUAD && mSpeaker[count2].mSpeaker == FMOD_SPEAKER_FRONT_CENTER)
            {
                continue;
            }

            if (mSpeaker[count2].mXZAngle < lowest && !used[count2])
            {
                lowest = mSpeaker[count2].mXZAngle;
                mSpeakerList[count] = &mSpeaker[count2];
            }
        }
        if (mSpeakerList[count])
        {
            used[mSpeakerList[count]->mSpeaker] = true;
        }
    }

    return prepareSpeakerPairs();
}


FMOD_RESULT SystemI::prepareSpeakerPairs()
{
    int speaker = 0;
    FMOD_SPEAKERCONFIG *spkA, *spkB;

    /*
        Step 1:
        - Calculate speaker angles and normals
    */
    while (mSpeakerList[speaker])
    {
        mSpeakerList[speaker]->mXZNormal = mSpeakerList[speaker]->mPosition;
        mSpeakerList[speaker]->mXZNormal.y = 0;
        FMOD_Vector_Normalize(&mSpeakerList[speaker]->mXZNormal);

        mSpeakerList[speaker]->mXZAngle = FMOD_AngleSort_GetValue(mSpeakerList[speaker]->mXZNormal.x, mSpeakerList[speaker]->mXZNormal.z);

        ++speaker;
    }

    /*
        Step 2:
        - Limit speaker separation to a maximum of 180 degrees so that sources at poles
          are rendered hard left or hard right.
    */
    speaker = 0;
    while (mSpeakerList[speaker])
    {
        spkA = mSpeakerList[speaker++];
        if (mSpeakerList[speaker])
        {
            spkB = mSpeakerList[speaker];
        }
        else
        {
            spkB = mSpeakerList[0];
        }
    
        if (spkA->mXZAngle == spkB->mXZAngle)   /* 2 speakers are at the same location so skip to the next pair */
        {
            continue;
        }

        if (FMOD_AngleSort_IsReflex(spkA->mXZAngle, spkB->mXZAngle))
        {
            FMOD_VECTOR lateral;
            FMOD_Vector_Subtract(&spkA->mXZNormal, &spkB->mXZNormal, &lateral);
            FMOD_Vector_Normalize(&lateral);
            spkA->mXZNormal = lateral;
            FMOD_Vector_Scale(&lateral, -1.0f, &spkB->mXZNormal);
            spkA->mXZAngle = FMOD_AngleSort_GetValue(spkA->mXZNormal.x, spkA->mXZNormal.z);
            spkB->mXZAngle = FMOD_AngleSort_GetValue(spkB->mXZNormal.x, spkB->mXZNormal.z);

            // only possible to have one pair of reflex speakers
            break;
        }
    }


    /*
        Step 3:
        - Determine whether pairs will use VBAP or lateral panning only
        - Precalculate some constants for panning
    */
    speaker = 0;
    while (mSpeakerList[speaker])
    {
        spkA = mSpeakerList[speaker++];
        if (mSpeakerList[speaker])
        {
            spkB = mSpeakerList[speaker];
        }
        else
        {
            spkB = mSpeakerList[0];
        }

        if (spkA->mXZAngle == spkB->mXZAngle)   /* 2 speakers are at the same location so skip to the next pair */
        {
            continue;
        }
        
        spkA->mPairUseVBAP = !FMOD_AngleSort_IsStraight(spkA->mXZAngle, spkB->mXZAngle);
        if(spkA->mPairUseVBAP)
        {
            /*
                we would normally use the determinant to solve the speaker gains however this is a waste
                cpu as the gains are normalised for power. We need to keep its sign however so that gains
                are always positive.
            */
            float determinant = spkA->mXZNormal.x * spkB->mXZNormal.z - spkB->mXZNormal.x * spkA->mXZNormal.z;
            spkA->mPairVBAPSign = determinant > 0.0f ? 1.0f : -1.0f;
        }
    }

    return FMOD_OK;
}


#ifdef FMOD_SUPPORT_DSPCODEC
/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT SystemI::allocateDSPCodec(FMOD_SOUND_FORMAT format, DSPCodec **dsp)
{
    if (0)
    {
    }
#ifdef FMOD_SUPPORT_MPEG
    else if (format == FMOD_SOUND_FORMAT_MPEG)
    {
        return mDSPCodecPool_MPEG.alloc(dsp);
    }
#endif
#ifdef FMOD_SUPPORT_XMA
    else if (format == FMOD_SOUND_FORMAT_XMA)
    {
        return mDSPCodecPool_XMA.alloc(dsp);
    }
#endif
#ifdef FMOD_SUPPORT_IMAADPCM
    else if (format == FMOD_SOUND_FORMAT_IMAADPCM)
    {
        return mDSPCodecPool_ADPCM.alloc(dsp);
    }
#endif
#ifdef FMOD_SUPPORT_CELT
    else if (format == FMOD_SOUND_FORMAT_CELT)
    {
        return mDSPCodecPool_CELT.alloc(dsp);
    }
#endif
#ifdef FMOD_SUPPORT_RAWCODEC
    else if (format == FMOD_SOUND_FORMAT_PCM16)
    {
        return mDSPCodecPool_RAW.alloc(dsp);
    }
#endif

    return FMOD_ERR_FORMAT;
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
// OFFSET: 0x008D8A10
SystemI::SystemI()
{
    mInitialized                = false;
    mPluginsLoaded              = false;
    mOutputType                 = FMOD_OUTPUTTYPE_AUTODETECT;
    mOutput                     = 0;
    mEmulated                   = 0;
    mMainThreadID               = 0;
    mChannel                    = 0;
    mPluginFactory              = 0;
    mLastTimeStamp              = 0;
    mStreamFileBufferSize       = 16*1024;
    mStreamFileBufferSizeType   = FMOD_TIMEUNIT_RAWBYTES;
    mDeviceListLastCheckedTime  = 0;
    mDeviceListChanged          = false;
    mUserData                   = 0;

    mNumSoftwareChannels        = 32;
    mMinHardwareChannels2D      = 0;
    mMaxHardwareChannels2D      = 1000;
    mMinHardwareChannels3D      = 0;
    mMaxHardwareChannels3D      = 1000;
    mCreatedHardwareSample      = false;
    mBufferSize                 = FILE_SECTORSIZE;
    mUsesUserCallbacks          = false;
                                
#if defined(PLATFORM_XENON) || defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
    mDSPBlockSize               = 256;
    mDSPBufferSize              = mDSPBlockSize * 4;
#else
    mDSPBlockSize               = 1024;                  
    mDSPBufferSize              = mDSPBlockSize * 4;     
#endif

#ifdef FMOD_SUPPORT_SOFTWARE
    mSoftware                   = 0;

    mDSPTempBuff                = 0;
    mDSPTempBuffMem             = 0;

    for (int count = 0; count < FMOD_DSP_MAXTREEDEPTH; count++)
    {
        mDSPMixBuff[count] = 0;
    }
     
    mDSPClock.mHi = 0;
    mDSPClock.mLo = 0;
#endif

    mMaxInputChannels           = DSP_DEFAULTLEVELS_IN;
    mMaxOutputChannels          = 0;

    set3DSpeakerPosition(FMOD_SPEAKER_FRONT_LEFT,    -1.0f,  1.0f);
    set3DSpeakerPosition(FMOD_SPEAKER_FRONT_RIGHT,    1.0f,  1.0f);
    set3DSpeakerPosition(FMOD_SPEAKER_FRONT_CENTER,   0.0f,  1.0f);
    set3DSpeakerPosition(FMOD_SPEAKER_LOW_FREQUENCY,  0.0f,  0.0f);
    set3DSpeakerPosition(FMOD_SPEAKER_BACK_LEFT,     -1.0f, -1.0f);
    set3DSpeakerPosition(FMOD_SPEAKER_BACK_RIGHT,     1.0f, -1.0f);

#ifdef PLATFORM_PS3
    set3DSpeakerPosition(FMOD_SPEAKER_SIDE_LEFT,     -0.4f,  -1.0f);
    set3DSpeakerPosition(FMOD_SPEAKER_SIDE_RIGHT,     0.4f,  -1.0f);
#else
    set3DSpeakerPosition(FMOD_SPEAKER_SIDE_LEFT,     -1.0f,  0.0f);
    set3DSpeakerPosition(FMOD_SPEAKER_SIDE_RIGHT,     1.0f,  0.0f);
#endif

#if defined(PLATFORM_XENON)
    mOutputFormat               = FMOD_SOUND_FORMAT_PCMFLOAT;

    setSpeakerMode(FMOD_SPEAKERMODE_5POINT1);
#elif defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
    mOutputFormat               = FMOD_SOUND_FORMAT_PCMFLOAT;

    setSpeakerMode(FMOD_SPEAKERMODE_7POINT1);
#else    
    mOutputFormat               = FMOD_SOUND_FORMAT_PCM16;

    setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
#endif    
#if defined(PLATFORM_IPHONE)
    mOutputRate                 = 24000;
#else
    mOutputRate                 = 48000;
#endif
    mOutputHandle               = 0;
    mSelectedDriver             = 0;
    mResampleMethod             = FMOD_DSP_RESAMPLER_LINEAR;
                                
    mNumListeners               = 1;
	mDistanceScale              = 1;
	mRolloffScale               = 1;
	mDopplerScale               = 1;
#ifdef FMOD_SUPPORT_MULTIREVERB
    mReverb3DActive           = false;
    FMOD_REVERB_PROPERTIES prop = FMOD_PRESET_OFF;
    setReverbAmbientProperties(&prop);
#endif

#ifdef FMOD_SUPPORT_GEOMETRY
 	mGeometryList         = 0;	
    mGeometryMgr.mSystem = this;
#endif

    FMOD_memset(mPluginPath, 0, FMOD_STRING_MAXPATHLEN);

    mAdvancedSettings.maxXMAcodecs            = 0;
    mAdvancedSettings.maxADPCMcodecs          = 0;
    mAdvancedSettings.maxMPEGcodecs           = 0;
    mAdvancedSettings.maxCELTcodecs            = 0;
    mAdvancedSettings.maxPCMcodecs            = 0;
    mAdvancedSettings.max3DReverbDSPs         = FMOD_ADVANCEDSETTINGS_MAX3DREVERBDSPS;
    mAdvancedSettings.HRTFMinAngle            = 180;
    mAdvancedSettings.HRTFMaxAngle            = 360;
    mAdvancedSettings.HRTFFreq                = 4000;
    mAdvancedSettings.vol0virtualvol          = 0.0f;
    mAdvancedSettings.eventqueuesize          = 32;
    mAdvancedSettings.defaultDecodeBufferSize = FMOD_STREAMDECODEBUFFERSIZE_DEFAULT;
    mAdvancedSettings.geometryMaxFadeTime     = 0;

    mMPEGPluginHandle = 0xFFFFFFFF;
    mFSBPluginHandle = 0xFFFFFFFF;
    mWAVPluginHandle = 0xFFFFFFFF;

#ifdef FMOD_SUPPORT_DSPCODEC
    #ifdef FMOD_SUPPORT_MPEG
    mDSPCodecPool_MPEG.mSystem = this;
    #endif
    #ifdef FMOD_SUPPORT_IMAADPCM
    mDSPCodecPool_ADPCM.mSystem = this;
    #endif
    #ifdef FMOD_SUPPORT_XMA
    mDSPCodecPool_XMA.mSystem = this;
    #endif
    #ifdef FMOD_SUPPORT_CELT
    mDSPCodecPool_CELT.mSystem = this;
    #endif
    #ifdef FMOD_SUPPORT_RAWCODEC
    mDSPCodecPool_RAW.mSystem = this;
    #endif
#endif

#ifdef FMOD_SUPPORT_ASIO
    mASIOSpeakerList[0]  = FMOD_SPEAKER_FRONT_LEFT;
    mASIOSpeakerList[1]  = FMOD_SPEAKER_FRONT_RIGHT;
    #if (DSP_MAXLEVELS_OUT > 2)
    mASIOSpeakerList[2]  = FMOD_SPEAKER_FRONT_CENTER;
    mASIOSpeakerList[3]  = FMOD_SPEAKER_LOW_FREQUENCY;
    mASIOSpeakerList[4]  = FMOD_SPEAKER_BACK_LEFT;
    mASIOSpeakerList[5]  = FMOD_SPEAKER_BACK_RIGHT;
    #endif
    #if (DSP_MAXLEVELS_OUT > 6)
    mASIOSpeakerList[6]  = FMOD_SPEAKER_SIDE_LEFT;
    mASIOSpeakerList[7]  = FMOD_SPEAKER_SIDE_RIGHT;
    #endif

    #if (DSP_MAXLEVELS_OUT > 8)
    mASIOSpeakerList[8]  = FMOD_SPEAKER_FRONT_LEFT;
    mASIOSpeakerList[9]  = FMOD_SPEAKER_FRONT_RIGHT;
    mASIOSpeakerList[10] = FMOD_SPEAKER_FRONT_LEFT;
    mASIOSpeakerList[11] = FMOD_SPEAKER_FRONT_RIGHT;
    mASIOSpeakerList[12] = FMOD_SPEAKER_FRONT_LEFT;
    mASIOSpeakerList[13] = FMOD_SPEAKER_FRONT_RIGHT;
    mASIOSpeakerList[14] = FMOD_SPEAKER_FRONT_LEFT;
    mASIOSpeakerList[15] = FMOD_SPEAKER_FRONT_RIGHT;
    #endif
#endif

    mDownmix = 0;
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
// OFFSET: 0x008D9A30
FMOD_RESULT SystemI::release()
{
    FMOD_RESULT result;

    if (mInitialized)
    {
        result = close();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (mOutput)
    {
        mOutput->release();
        mOutput = 0;
    }
    
    #if defined(FMOD_SUPPORT_GEOMETRY) && defined(FMOD_SUPPORT_GEOMETRY_THREADED)
    mGeometryMgr.releaseOcclusionThread();
    #endif

    removeNode();

    FMOD_Memory_Free(this);
    
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
// OFFSET: 0x008D3AB0
FMOD_RESULT SystemI::setOutput(FMOD_OUTPUTTYPE outputtype)
{
    FMOD_RESULT result = FMOD_OK;
    int count, numoutputs;

    if (mInitialized)
	{
		return FMOD_ERR_INITIALIZED;
	}

    if (mOutput)
    {
        if (outputtype == mOutputType)
        {
            return FMOD_OK;
        }

        mOutput->release();
        mOutput = 0;
    }

    if (!mPluginsLoaded)
    {
        result = setUpPlugins();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        Scan plugin list and find an equal type.
    */
    result = mPluginFactory->getNumOutputs(&numoutputs);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (outputtype == FMOD_OUTPUTTYPE_AUTODETECT)
    {
        FMOD_OS_Output_GetDefault(&outputtype);
    }

    for (count = 0; count < numoutputs; count++)
    {
        FMOD_OUTPUT_DESCRIPTION_EX *outputdesc = 0;
        unsigned int handle;

        result = mPluginFactory->getOutputHandle(count, &handle);
        if (result != FMOD_OK)
        {
            continue;
        }

        result = mPluginFactory->getOutput(handle, &outputdesc);
        if (result != FMOD_OK)
        {
            continue;
        }

        if (outputdesc->mType == outputtype)
        {
            result = mPluginFactory->createOutput(outputdesc, &mOutput);
            if (result != FMOD_OK)
            {
                return result;
            }

            mOutputType = mOutput->mDescription.mType;
            mOutputHandle = mOutput->mDescription.mHandle;

            return FMOD_OK;
        }
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
// OFFSET: 0x008D3BC0
FMOD_RESULT SystemI::getOutput(FMOD_OUTPUTTYPE *output)
{
    if (!output)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *output = mOutputType;

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
// OFFSET: 0x008D3BF0
FMOD_RESULT SystemI::setSoftwareFormat(int samplerate, FMOD_SOUND_FORMAT format, int numoutputchannels, int maxinputchannels, FMOD_DSP_RESAMPLER resamplermethod)
{
	if (mInitialized)
	{
		return FMOD_ERR_INITIALIZED;
	}

    if (samplerate < 8000 || samplerate > 192000)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (maxinputchannels > DSP_MAXLEVELS_IN || numoutputchannels > DSP_MAXLEVELS_OUT)
    {
        return FMOD_ERR_TOOMANYCHANNELS;
    }

    mOutputRate      = samplerate;
    mOutputFormat    = format;
    mResampleMethod  = resamplermethod;

    #ifndef PLATFORM_PS3
    if (numoutputchannels)
    {
        mMaxOutputChannels  = numoutputchannels;

        mSpeakerMode = FMOD_SPEAKERMODE_RAW;
    }
    #endif
    if (maxinputchannels > 0)
    {
        mMaxInputChannels = maxinputchannels;
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
// OFFSET: 0x008D3C60
FMOD_RESULT SystemI::getNumDrivers(int *numdrivers)
{
    FMOD_RESULT result;

    if (!numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    if (!mInitialized)
    {
        result = setOutput(mOutputType);
        if (result != FMOD_OK)
        {
            *numdrivers = 0;
            return result;
        }
    }

    result = checkDriverList(false);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mOutput->mDescription.getnumdrivers)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif

        return mOutput->mDescription.getnumdrivers(mOutput, numdrivers);
    }

    *numdrivers = 0;

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
FMOD_RESULT SystemI::getDriverInfo(int id, char *name, int namelen, FMOD_GUID *guid)
{
    FMOD_RESULT result;
    int numdrivers;

    result = getNumDrivers(&numdrivers);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (id < 0 || id >= numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mInitialized)
    {
        result = setOutput(mOutputType);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (mOutput->mDescription.getdriverinfo)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif

        return mOutput->mDescription.getdriverinfo(mOutput, id, name, namelen, guid);
    }

    if (mOutput->mDescription.getdrivername)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif

        return mOutput->mDescription.getdrivername(mOutput, id, name, namelen);
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
FMOD_RESULT SystemI::getDriverInfoW(int id, short *name, int namelen, FMOD_GUID *guid)
{
    FMOD_RESULT result;
    int numdrivers;

    result = getNumDrivers(&numdrivers);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (id < 0 || id >= numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mInitialized)
    {
        result = setOutput(mOutputType);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (mOutput->mDescription.getdriverinfow)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif

        return mOutput->mDescription.getdriverinfow(mOutput, id, name, namelen, guid);
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
// OFFSET: 0x008D3D70
FMOD_RESULT SystemI::getDriverCaps(int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency, FMOD_SPEAKERMODE *controlpanelspeakermode)
{
    FMOD_RESULT      result;
    FMOD_CAPS        lcaps;
    FMOD_SPEAKERMODE lcontrolpanelspeakermode;
    int              lminfrequency;
    int              lmaxfrequency;
    int              lnum2dchannels;
    int              lnum3dchannels;
    int              ltotalchannels;
    int              numdrivers;

    if (mInitialized)
    {
        return FMOD_ERR_INITIALIZED;
    }

    result = getNumDrivers(&numdrivers);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (id < 0 || id >= numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mInitialized)
    {
        result = setOutput(mOutputType);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    lcaps = FMOD_CAPS_NONE;
    lminfrequency = 0;
    lmaxfrequency = 0;
    lnum2dchannels = 0;
    lnum3dchannels = 0;
    ltotalchannels = 0;
    lcontrolpanelspeakermode = FMOD_SPEAKERMODE_STEREO;

    if (mOutput->mDescription.getdrivercapsex2)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif
        result = mOutput->mDescription.getdrivercapsex2(mOutput, id, &lcaps, &lminfrequency, &lmaxfrequency, &lcontrolpanelspeakermode, &lnum2dchannels, &lnum3dchannels, &ltotalchannels);
        if (result != FMOD_OK)
        {
            return result;
        }
    }
    if (mOutput->mDescription.getdrivercapsex)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif
        result = mOutput->mDescription.getdrivercapsex(mOutput, id, &lcaps, &lminfrequency, &lmaxfrequency, &lcontrolpanelspeakermode);
        if (result != FMOD_OK)
        {
            return result;
        }
    }
    else if (mOutput->mDescription.getdrivercaps)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif
        result = mOutput->mDescription.getdrivercaps(mOutput, id, &lcaps);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    mOutput->mNum2DChannelsFromCaps = lnum2dchannels;
    mOutput->mNum3DChannelsFromCaps = lnum3dchannels;
    mOutput->mTotalChannelsFromCaps = ltotalchannels;

    if (caps)
    {
        *caps = lcaps;
    }
    if (minfrequency)
    {
        *minfrequency = lminfrequency;
    }
    if (maxfrequency)
    {
        *maxfrequency = lmaxfrequency;
    }
    if (controlpanelspeakermode)
    {
        *controlpanelspeakermode = lcontrolpanelspeakermode;
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
// OFFSET: 0x008D3F30
FMOD_RESULT SystemI::setDriver(int driver)
{
    FMOD_RESULT        result       = FMOD_OK;
    int                numdrivers   = 0;

    result = getNumDrivers(&numdrivers);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (driver < -1 || driver >= numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (driver == -1)
    {
        driver = 0;
    }

    /*
        If the driver is already initialised, we need to reset it
    */
    if (mInitialized)
	{
#ifdef FMOD_SUPPORT_SOFTWARE
        int                samplerate   = 0;
        FMOD_SOUND_FORMAT  soundformat  = FMOD_SOUND_FORMAT_NONE;
        FMOD_SPEAKERMODE   speakermode  = FMOD_SPEAKERMODE_STEREO;

        if (mCreatedHardwareSample)
#endif
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::setDriver", "Cannot change driver when hardware samples have been created.\n"));
            return FMOD_ERR_NEEDSSOFTWARE;
        }

#ifdef FMOD_SUPPORT_RECORDING
        if (mOutput->mRecordNumActive)
        {
            result = mOutput->recordStopAll(false);
            CHECK_RESULT(result);
        }
#endif

#ifdef FMOD_SUPPORT_SOFTWARE
        /*
            Stop the current driver
        */
        if (mOutput->mDescription.stop)
        {
            mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */

            mOutput->mDescription.stop(mOutput);
        }
        else if (mOutput->mDescription.polling)
        {
            OutputPolled *outputpolled = SAFE_CAST(OutputPolled, mOutput);
        
            outputpolled->stop();
        }

        /*
            Close the current driver
        */
        if (mOutput->mDescription.close)
        {
            mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */

            mOutput->mDescription.close(mOutput);
        }
     
        /*
            Initialise the new driver
        */
        speakermode = mSpeakerMode;
        samplerate  = mOutputRate;
        soundformat = mOutputFormat;

        if (mOutput->mDescription.initex)
        {
            mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
            
            result = mOutput->mDescription.initex(mOutput, driver, mFlags, &samplerate, mMaxOutputChannels, &soundformat, &speakermode, mDSPBlockSize, mDSPBufferSize / mDSPBlockSize, 0, 0, NULL);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else if (mOutput->mDescription.init)
        {
            mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */

            result = mOutput->mDescription.init(mOutput, driver, mFlags, &samplerate, mMaxOutputChannels, &soundformat, mDSPBlockSize, mDSPBufferSize / mDSPBlockSize, NULL);
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        /*
            Check if output mode changed the sample rate, format or speaker mode
        */
        if (speakermode != mSpeakerMode || samplerate != mOutputRate || soundformat != mOutputFormat)
        {
            /*
                Stop the new driver
            */
            if (mOutput->mDescription.stop)
            {
                mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */

                mOutput->mDescription.stop(mOutput);
            }
            else if (mOutput->mDescription.polling)
            {
                OutputPolled *outputpolled = SAFE_CAST(OutputPolled, mOutput);
            
                outputpolled->stop();
            }

            /*
                Close the new driver
            */
            if (mOutput->mDescription.close)
            {
                mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */

                mOutput->mDescription.close(mOutput);
            }

            // The new driver is not compatible with the current FMOD state, user must now choose a driver that is compatible (possibly the original one if it is still connected)
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::setDriver", "Selected driver does not support current output format, sample rate or number of channels.\n"));
            return FMOD_ERR_OUTPUT_INIT;
        }
        
        /*
            Start the new driver
        */
        if (mOutput->mDescription.start)
        {
            mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */

            result = mOutput->mDescription.start(mOutput);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else if (mOutput->mDescription.polling)
        {
            OutputPolled *outputpolled = SAFE_CAST(OutputPolled, mOutput);
        
            result = outputpolled->start();
            if (result != FMOD_OK)
            {
                return result;
            }
        }
#endif
	}

    mSelectedDriver = driver;
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
FMOD_RESULT SystemI::getDriver(int *driver)
{
    if (!driver)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *driver = mSelectedDriver;

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
// OFFSET: 0x008D3F90
FMOD_RESULT SystemI::setHardwareChannels(int min2d, int max2d, int min3d, int max3d)
{
	if (mInitialized)
	{
		return FMOD_ERR_INITIALIZED;
	}

    if (min2d < 0 || max2d < 0 || min3d < 0 || max3d < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mMinHardwareChannels2D = min2d;
    mMaxHardwareChannels2D = max2d;
    mMinHardwareChannels3D = min3d;
    mMaxHardwareChannels3D = max3d;

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
// OFFSET: 0x008D3FF0
FMOD_RESULT SystemI::getHardwareChannels(int *num2d, int *num3d, int *total)
{
    int numhw3d = 0, numhw2d = 0, numhwtotal = 0;

#ifndef FMOD_STATICFORPLUGINS

    if (mInitialized)
    {
        if (mOutput)
        {
            FMOD_RESULT result;
        
            if (mOutput->mChannelPool)
            {
                result = mOutput->mChannelPool->getNumChannels(&numhw2d);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }

            if (mOutput->mChannelPool3D)
            {
                result = mOutput->mChannelPool3D->getNumChannels(&numhw3d);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
        }
        numhwtotal = numhw3d + numhw2d;
    }
    else
    {
        FMOD_RESULT result;
        result = setOutput(mOutputType);
        if (result != FMOD_OK)
        {
            return result;
        }

        numhw2d    = mOutput->mNum2DChannelsFromCaps;
        numhw3d    = mOutput->mNum3DChannelsFromCaps;
        numhwtotal = mOutput->mTotalChannelsFromCaps;
    }
#endif

    if (num3d)
    {
        *num3d = numhw3d;
    }

    if (num2d)
    {
        *num2d = numhw2d;
    }

    if (total)
    {
        *total = numhwtotal;
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
// OFFSET: 0x008D40A0
FMOD_RESULT SystemI::setSoftwareChannels(int numsoftwarechannels)
{
    if (numsoftwarechannels < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mInitialized)
    {
        return FMOD_ERR_INITIALIZED;
    }

    mNumSoftwareChannels = numsoftwarechannels;

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
FMOD_RESULT SystemI::getSoftwareChannels(int *numsoftwarechannels)
{
    if (!numsoftwarechannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numsoftwarechannels = mNumSoftwareChannels;

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
// OFFSET: 0x008D40D0
FMOD_RESULT SystemI::setDSPBufferSize(unsigned int bufferlength, int numbuffers)
{
	if (mInitialized)
	{
		return FMOD_ERR_INITIALIZED;
	}

    #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)

    return FMOD_ERR_UNSUPPORTED;

    #else

    if (bufferlength < 1 || numbuffers < 2)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mDSPBlockSize  = bufferlength;
    mDSPBufferSize = bufferlength * numbuffers;

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
// OFFSET: 0x008D4110
FMOD_RESULT SystemI::getDSPBufferSize(unsigned int *bufferlength, int *numbuffers)
{
    if (bufferlength)
    {
        *bufferlength = mDSPBlockSize;
    }
    if (numbuffers)
    {
        *numbuffers = mDSPBufferSize / mDSPBlockSize;
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
FMOD_RESULT SystemI::setFileSystem(FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek, int blockalign)
{
    setGlobalUserCallbacks(useropen, userclose, userread, userseek);

    if (blockalign >= 0)
    {
        mBufferSize = blockalign;
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
FMOD_RESULT SystemI::attachFileSystem(FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek)
{
    mOpenRiderCallback  = useropen;
    mCloseRiderCallback = userclose;
    mReadRiderCallback  = userread;
    mSeekRiderCallback  = userseek;

    return FMOD_OK;
}

#ifndef FMOD_STATICFORPLUGINS


/*
[
	[DESCRIPTION]
    Sets advanced features like configuring memory and cpu usage for FMOD_CREATEREALTIMESAMPLE usage.

	[PARAMETERS]
    'settings'      Pointer to FMOD_ADVANCEDSETTINGS structure.
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
    FMOD_ADVANCEDSETTINGS
    SystemI::getAdvancedSettings
]
*/
FMOD_RESULT SystemI::setAdvancedSettings(FMOD_ADVANCEDSETTINGS *settings)
{
    int memberoffset;

    if (!settings)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    memberoffset = (int)((FMOD_UINT_NATIVE)&settings->maxXMAcodecs - (FMOD_UINT_NATIVE)settings);
    
    if (settings->cbsize <= memberoffset)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (settings->maxADPCMcodecs < 0 || settings->maxADPCMcodecs > 65535)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (settings->maxMPEGcodecs < 0 || settings->maxMPEGcodecs > 65535)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (settings->maxXMAcodecs < 0 || settings->maxXMAcodecs > 65535)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (settings->maxCELTcodecs < 0 || settings->maxCELTcodecs > 65535)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (settings->maxPCMcodecs < 0 || settings->maxPCMcodecs > 65535)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    {
        FMOD_RESULT result;

        result = FMOD_CHECKFLOAT(settings->HRTFMinAngle);
        if (result != FMOD_OK)
        {
            return result;
        }
        result = FMOD_CHECKFLOAT(settings->HRTFMaxAngle);
        if (result != FMOD_OK)
        {
            return result;
        }
        result = FMOD_CHECKFLOAT(settings->HRTFFreq);
        if (result != FMOD_OK)
        {
            return result;
        }
        result = FMOD_CHECKFLOAT(settings->vol0virtualvol);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (settings->HRTFMinAngle < 0 || settings->HRTFMinAngle > 360)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (settings->HRTFMaxAngle < settings->HRTFMinAngle || settings->HRTFMaxAngle > 360.0f)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (settings->HRTFFreq < 0.0001f)
    {
        settings->HRTFFreq = mAdvancedSettings.HRTFFreq;
    }
    else if (settings->HRTFFreq < 10.0f || settings->HRTFFreq > 22050.0f)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (settings->vol0virtualvol < 0.0f)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (settings->eventqueuesize < 0 || settings->eventqueuesize > 65535)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (settings->ASIONumChannels > DSP_MAXLEVELS_OUT || settings->ASIONumChannels < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (settings->debugLogFilename && FMOD_strlen(settings->debugLogFilename) >= 255)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

#ifdef FMOD_DEBUG
    if (settings->debugLogFilename)
    {
        FMOD_strcpy(FMOD::gGlobal->gDebugFilename, settings->debugLogFilename);
    }
#endif

    if ((settings->defaultDecodeBufferSize == 0) || (settings->defaultDecodeBufferSize > 30000))
    {
        settings->defaultDecodeBufferSize = mAdvancedSettings.defaultDecodeBufferSize;
    }

    FMOD_memcpy(&mAdvancedSettings, settings, settings->cbsize);

#ifdef FMOD_DEBUG
    if (settings->debugLogFilename) /* Dont store a pointer to the user name, set it back to mAdvancedSettings debuglog filename here. */
    {
        mAdvancedSettings.debugLogFilename = FMOD::gGlobal->gDebugFilename;
    }
#endif

#ifdef FMOD_SUPPORT_ASIO
    if (settings->ASIOSpeakerList)
    {
        int count;

        for (count = 0; count < settings->ASIONumChannels; count++)
        {
            mASIOSpeakerList[count] = settings->ASIOSpeakerList[count];
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
    FMOD_ADVANCEDSETTINGS
    SystemI::setAdvancedSettings
]
*/
FMOD_RESULT SystemI::getAdvancedSettings(FMOD_ADVANCEDSETTINGS *settings)
{
    int           usercbsize;
    int           asioNumChannels;
    FMOD_SPEAKER *speakerlist;
    char         *debuglogfilename; 

    if (!settings)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (settings->ASIONumChannels > DSP_MAXLEVELS_OUT || settings->ASIONumChannels < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    usercbsize = settings->cbsize;
    speakerlist = settings->ASIOSpeakerList;
    asioNumChannels = settings->ASIONumChannels;
    debuglogfilename = settings->debugLogFilename; 

    FMOD_memcpy(settings, &mAdvancedSettings, usercbsize);

    settings->cbsize = usercbsize;
    settings->ASIOSpeakerList = speakerlist;
    settings->debugLogFilename = debuglogfilename;

#ifdef FMOD_DEBUG
    if (settings->debugLogFilename)
    {
        FMOD_strcpy(settings->debugLogFilename, FMOD::gGlobal->gDebugFilename);
    }
#endif

	#ifdef FMOD_SUPPORT_ASIO
    if (settings->ASIONumChannels > DSP_MAXLEVELS_OUT)
    {
        settings->ASIONumChannels = DSP_MAXLEVELS_OUT;
    }
    if (settings->ASIOSpeakerList)
    {
        int count;

        for (count = 0; count < asioNumChannels; count++)
        {
            settings->ASIOSpeakerList[count] = mASIOSpeakerList[count];
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
// OFFSET: 0x008D5A50
FMOD_RESULT SystemI::setSpeakerMode(FMOD_SPEAKERMODE speakermode)
{
	if (mInitialized)
	{
		return FMOD_ERR_INITIALIZED;
	}
    
    mSpeakerMode = speakermode;

    switch (mSpeakerMode)
    {
        case FMOD_SPEAKERMODE_RAW:
        {
            /* Dont change mMaxOutputChannels */
            return FMOD_OK;
        }
        case FMOD_SPEAKERMODE_MONO:
        {
            mMaxOutputChannels = 1;
            break;
        }
        case FMOD_SPEAKERMODE_STEREO:
        {
            mMaxOutputChannels = 2;
            
            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_LEFT,    -1.0f,  0.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_RIGHT,    1.0f,  0.0f);
            break;
        }
        case FMOD_SPEAKERMODE_QUAD:
        {
            mMaxOutputChannels = 4;

            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_LEFT,    -1.0f,  1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_RIGHT,    1.0f,  1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_BACK_LEFT,     -1.0f, -1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_BACK_RIGHT,     1.0f, -1.0f);
            break;
        }
        case FMOD_SPEAKERMODE_SURROUND:
        {
            mMaxOutputChannels = 5;

            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_LEFT,    -1.0f,  1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_RIGHT,    1.0f,  1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_CENTER,   0.0f,  1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_BACK_LEFT,     -1.0f, -1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_BACK_RIGHT,     1.0f, -1.0f);
            break;
        }
        case FMOD_SPEAKERMODE_5POINT1:
        {
            mMaxOutputChannels = 6;

            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_LEFT,    -1.0f,  1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_RIGHT,    1.0f,  1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_CENTER,   0.0f,  1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_BACK_LEFT,     -1.0f, -1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_BACK_RIGHT,     1.0f, -1.0f);
            break;

        }
        case FMOD_SPEAKERMODE_7POINT1:
        {
            mMaxOutputChannels = 8;
            
            #if defined(FMOD_SUPPORT_MYEARS) && !defined(PLATFORM_PS3)
            /* MyEars uses standard 7.1 speaker positioning whereas FMOD dos not */

            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_LEFT,    -0.5f,  0.866f);
            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_RIGHT,    0.5f,  0.866f);
            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_CENTER,   0.0f,  1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_BACK_LEFT,     -0.707f, -0.707f);
            set3DSpeakerPosition(FMOD_SPEAKER_BACK_RIGHT,     0.707f, -0.707f);
            set3DSpeakerPosition(FMOD_SPEAKER_SIDE_LEFT,     -1.0f,  0.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_SIDE_RIGHT,     1.0f,  0.0f);

            #else

            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_LEFT,    -1.0f,  1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_RIGHT,    1.0f,  1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_CENTER,   0.0f,  1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_BACK_LEFT,     -1.0f, -1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_BACK_RIGHT,     1.0f, -1.0f);
            
            #ifdef PLATFORM_PS3
            set3DSpeakerPosition(FMOD_SPEAKER_SIDE_LEFT,     -0.4f,  -1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_SIDE_RIGHT,     0.4f,  -1.0f);
            #else
            set3DSpeakerPosition(FMOD_SPEAKER_SIDE_LEFT,     -1.0f,  0.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_SIDE_RIGHT,     1.0f,  0.0f);
            #endif

            #endif

            break;
        }
        case FMOD_SPEAKERMODE_PROLOGIC:
        {
            mMaxOutputChannels = 2;

            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_LEFT,    -1.0f,  1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_RIGHT,    1.0f,  1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_FRONT_CENTER,   0.0f,  1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_BACK_LEFT,     -1.0f, -1.0f);
            set3DSpeakerPosition(FMOD_SPEAKER_BACK_RIGHT,     1.0f, -1.0f);
            break;
        }
        default:
        {
            break;
        }
    }

    return sortSpeakerList();
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
// OFFSET: 0x008D4150
FMOD_RESULT SystemI::setCallback(FMOD_SYSTEM_CALLBACK callback)
{
    FMOD::gGlobal->gSystemCallback = callback;
    mCallback = callback;

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
FMOD_RESULT SystemI::setPluginPath(const char *path)
{
	if (mInitialized)
	{
		return FMOD_ERR_INITIALIZED;
	}

    if (FMOD_strlen(path) >= FMOD_STRING_MAXPATHLEN)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_strncpy(mPluginPath, path, FMOD_STRING_MAXPATHLEN);

    if (mPluginFactory)
    {
        mPluginFactory->setPluginPath(mPluginPath);
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
FMOD_RESULT SystemI::loadPlugin(const char *filename, unsigned int *handle, unsigned int priority)
{
#ifdef FMOD_SUPPORT_DLLS
    FMOD_RESULT result;

    if (!mPluginsLoaded)
    {
        result = setUpPlugins();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    return mPluginFactory->loadPlugin(filename, handle, false, priority);

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
FMOD_RESULT SystemI::getNumPlugins(FMOD_PLUGINTYPE type, int *numplugins)
{
    FMOD_RESULT result;

    if (!numplugins)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mPluginsLoaded)
    {
        result = setUpPlugins();
        if (result != FMOD_OK)
        {
            return result;
        }
    }
            
    switch (type)
    {
        case FMOD_PLUGINTYPE_OUTPUT:
        {
            mPluginFactory->getNumOutputs(numplugins);
            break;
        }
        case FMOD_PLUGINTYPE_CODEC:
        {
            mPluginFactory->getNumCodecs(numplugins);
            break;
        }
        case FMOD_PLUGINTYPE_DSP:
        {
            mPluginFactory->getNumDSPs(numplugins);
            break;
        }
        default:
        {
            return FMOD_ERR_INVALID_PARAM;
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
FMOD_RESULT SystemI::getPluginHandle(FMOD_PLUGINTYPE plugintype, int index, unsigned int *handle)
{
    switch (plugintype)
    {
        case FMOD_PLUGINTYPE_CODEC:
        {
            return mPluginFactory->getCodecHandle(index, handle);
        }
        case FMOD_PLUGINTYPE_DSP:
        {
            return mPluginFactory->getDSPHandle(index, handle);
        }
        case FMOD_PLUGINTYPE_OUTPUT:
        {
            return mPluginFactory->getOutputHandle(index, handle);
        }
        default:
        {
            return FMOD_ERR_INVALID_PARAM;
        }
    }
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
FMOD_RESULT SystemI::getPluginInfo(unsigned int handle, FMOD_PLUGINTYPE *plugintype, char *name, int namelen, unsigned int *version)
{
    FMOD_RESULT result;
    FMOD_OUTPUT_DESCRIPTION_EX *outputdesc;
    FMOD_CODEC_DESCRIPTION_EX *codecdesc;
    FMOD_DSP_DESCRIPTION_EX *dspdesc;

    if (!mPluginsLoaded)
    {
        result = setUpPlugins();
        if (result != FMOD_OK)
        {
            return result;
        }
    }       
    
    result = mPluginFactory->getOutput(handle, &outputdesc);
    if (result == FMOD_OK)
    {
        if (name)
        {
            FMOD_strncpy(name, outputdesc->name, namelen);
        }
        if (version)
        {
            *version = outputdesc->version;
        }
        if (plugintype)
        {
            *plugintype = FMOD_PLUGINTYPE_OUTPUT;
        }
    }
    else if (result == FMOD_ERR_PLUGIN_MISSING)
    {
        result = mPluginFactory->getCodec(handle, &codecdesc);
        if (result == FMOD_OK)
        {
            if (name)
            {
                FMOD_strncpy(name, codecdesc->name, namelen);
            }
            if (version)
            {
                *version = codecdesc->version;
            }
            if (plugintype)
            {
                *plugintype = FMOD_PLUGINTYPE_CODEC;
            }
        }
        else if (result == FMOD_ERR_PLUGIN_MISSING)
        {
            result = mPluginFactory->getDSP(handle, &dspdesc);
            if (result == FMOD_OK)
            {
                if (name)
                {
                    FMOD_strncpy(name, dspdesc->name, namelen);
                }
                if (version)
                {
                    *version = dspdesc->version;
                }
                if (plugintype)
                {
                    *plugintype = FMOD_PLUGINTYPE_DSP;
                }
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
FMOD_RESULT SystemI::unloadPlugin(unsigned int handle)
{
    FMOD_RESULT result;

    if (!mPluginsLoaded)
    {
        result = setUpPlugins();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    return mPluginFactory->unloadPlugin(handle);
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
FMOD_RESULT SystemI::setOutputByPlugin(unsigned int handle)
{
    FMOD_RESULT                 result = FMOD_OK;
    FMOD_OUTPUT_DESCRIPTION_EX *outputdesc = 0;

    if (mInitialized)
	{
		return FMOD_ERR_INITIALIZED;
	}

    if (mOutput)
    {
        mOutput->release();
        mOutput = 0;
    }

    if (!mPluginsLoaded)
    {
        result = setUpPlugins();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    result = mPluginFactory->getOutput(handle, &outputdesc);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mPluginFactory->createOutput(outputdesc, &mOutput);
    if (result != FMOD_OK)
    {
        return result;
    }

    mOutputType = mOutput->mDescription.mType;
    mOutputHandle = mOutput->mDescription.mHandle;

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
FMOD_RESULT SystemI::getOutputByPlugin(unsigned int *handle)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *handle = mOutputHandle;

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

class SystemInitCleanup
{
public:
    SystemInitCleanup()
        : mSystem(0), mSpeakerMode(FMOD_SPEAKERMODE_MAX), 
          mOutputFormat(FMOD_SOUND_FORMAT_MAX), mOutputRate(0),
          mSoundListCrit(0), mFileCrit(0), mAsyncCrit(0),
          mProfilerInitialized(false)
    { }

    void setSystem(SystemI *system) 
    { 
        mSystem = system; 
        mSpeakerMode  = system->mSpeakerMode;
        mOutputFormat = system->mOutputFormat;
        mOutputRate   = system->mOutputRate;
    }
    
    void profilerInitialized()                            { mProfilerInitialized = true; }
    void setSoundListCrit(FMOD_OS_CRITICALSECTION **crit) { mSoundListCrit = crit; }
    void setFileCrit(FMOD_OS_CRITICALSECTION **crit)      { mFileCrit = crit; }
    void setAsyncCrit(FMOD_OS_CRITICALSECTION **crit)     { mAsyncCrit = crit; }

    void cleanup(bool)
    {
        if(mSystem)
        {
            if(mSpeakerMode != FMOD_SPEAKERMODE_MAX)
            {
                mSystem->setSpeakerMode(mSpeakerMode);
            }
            if (mOutputFormat != FMOD_SOUND_FORMAT_MAX)
            {
                mSystem->mOutputFormat = mOutputFormat;
            }
            if (mOutputRate != 0)
            {
                mSystem->mOutputRate = mOutputRate;
            }
            mSystem->close();
            
#ifdef FMOD_SUPPORT_PROFILE
            if (FMOD::gGlobal->gSystemInitCount == 0 && mProfilerInitialized)
            {
                FMOD_Profile_Release();
            }
#endif
        }
        if(mAsyncCrit)
        {
            FMOD_OS_CriticalSection_Free(*mAsyncCrit);
            *mAsyncCrit = 0;
        }
        if(mFileCrit)
        {
            FMOD_OS_CriticalSection_Free(*mFileCrit);
            *mFileCrit = 0;
        }
        if(mSoundListCrit)
        {
            FMOD_OS_CriticalSection_Free(*mSoundListCrit);
            *mSoundListCrit = 0;
        }
    }    

private:
    SystemI *mSystem;
    FMOD_SPEAKERMODE mSpeakerMode;
    FMOD_SOUND_FORMAT mOutputFormat;
    int mOutputRate;
    FMOD_OS_CRITICALSECTION **mSoundListCrit;
    FMOD_OS_CRITICALSECTION **mFileCrit;
    FMOD_OS_CRITICALSECTION **mAsyncCrit;
    bool mProfilerInitialized;
};


// OFFSET: 0x008D9AA0
FMOD_RESULT SystemI::init(int maxchannels, FMOD_INITFLAGS flags, void *extradriverdata)
{
    int count;
    FMOD_RESULT result;
    int channellimit = (int)(1 << CHANINDEX_BITS);
    FMOD_SPEAKERMODE speakerMode;
    AutoCleanup<bool, SystemInitCleanup> initCleanup(true);

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::init", "FMOD Ex Version: %08x\n", FMOD_VERSION));
    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::init", "maxchannels = %d, flags = %08x, extradriverdata = %p\n", maxchannels, flags, extradriverdata));

    if (maxchannels < 0 || maxchannels >= channellimit)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

	if (mInitialized)
	{
		return FMOD_ERR_INITIALIZED;
	}

    result = closeEx(true);
    if (result != FMOD_OK)
    {
        return result;
    }

    initCleanup.setSystem(this);

    mFlags = flags;
    FMOD_OS_Thread_GetCurrentID(&mMainThreadID);

    /*
        Set up output driver for this system
    */
    result = setOutput(mOutputType);
    if (result != FMOD_OK)
    {
        return result;
    }

#if defined(FMOD_SUPPORT_MYEARS) && !defined(PLATFORM_PS3)
    bool enablemyears = !(mFlags & FMOD_INIT_DISABLE_MYEARS) && !mDownmix && DownmixMyEars::dataExists();
    if (enablemyears)
    {
        mOutputRate = DownmixMyEars::getSampleRate();
    }
#endif

    if (mOutput->mDescription.initex)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif
        speakerMode = mSpeakerMode;
        result = mOutput->mDescription.initex(mOutput, mSelectedDriver, flags, &mOutputRate, mMaxOutputChannels, &mOutputFormat, &speakerMode, mDSPBlockSize, mDSPBufferSize / mDSPBlockSize, 0, 0, extradriverdata);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (speakerMode != mSpeakerMode)
        {
            setSpeakerMode(speakerMode);
        }
    }
    else if (mOutput->mDescription.init)
    {
        int outputchannels = mMaxOutputChannels;

#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif
        result = mOutput->mDescription.init(mOutput, mSelectedDriver, flags, &mOutputRate, outputchannels, &mOutputFormat, mDSPBlockSize, mDSPBufferSize / mDSPBlockSize, extradriverdata);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

#if defined(FMOD_SUPPORT_NEURAL) && !defined(PLATFORM_PS3)
    if ((flags & FMOD_INIT_DTS_NEURALSURROUND) && (mSpeakerMode == FMOD_SPEAKERMODE_STEREO || mSpeakerMode == FMOD_SPEAKERMODE_5POINT1) && !mDownmix)
    {
        mDownmix = FMOD_Object_Calloc(DownmixNeural);
        if (!mDownmix)
        {
            return FMOD_ERR_MEMORY;
        }

        result = mDownmix->init(mDSPBufferSize, mOutputRate, mSpeakerMode);
        if (result != FMOD_OK)
        {
            return result;
        }

        /*
            Software mixer should always be mixing in 7.1 if we are using Neural
        */
        setSpeakerMode(FMOD_SPEAKERMODE_7POINT1);
    }
#endif
    
#if defined(FMOD_SUPPORT_MYEARS) && !defined(PLATFORM_PS3)
    /*
        MyEars will be enabled by default as long as the data exists
        and another downmixer such as NEURALSURROUND has not been set.
    */
    if (enablemyears && !mDownmix)
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::init", "Load MyEars data\n"));

        mDownmix = FMOD_Object_Calloc(DownmixMyEars);
        if (!mDownmix)
        {
            return FMOD_ERR_MEMORY;
        }

        result = mDownmix->init(mDSPBufferSize, mOutputRate, mSpeakerMode, 0);
        if (result != FMOD_OK)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::init", "Failed to initialise MyEars. Returned error (%d)\n", result));

            mDownmix->shutdown();

            FMOD_Memory_Free(mDownmix);
            mDownmix = 0;
            enablemyears = false;
        }
        else
        {
            /*
                Software mixer should always be mixing in 7.1 if we are using MyEars (or 5.1 for testing)
            */
            int inputchannels;
            mDownmix->getInputChannels(&inputchannels);
            switch(inputchannels)
            {
            case 8:
                setSpeakerMode(FMOD_SPEAKERMODE_7POINT1);
                break;
            case 6:
                setSpeakerMode(FMOD_SPEAKERMODE_5POINT1);
                break;
            default:
                result = mDownmix->shutdown();
                if (result != FMOD_OK)
                {
                    return result;
                }
                FMOD_Memory_Free(mDownmix);
                mDownmix = 0;
                return FMOD_ERR_INTERNAL;
            }

            /*
                Override software HRTF to false when using MyEars
            */
            mFlags &= ~FMOD_INIT_SOFTWARE_HRTF;
        }
    }
#endif
  
#ifdef FMOD_SUPPORT_SOFTWARE
    if (!(mFlags & FMOD_INIT_SOFTWARE_DISABLE))
    {
        FMOD_DSP_DESCRIPTION    description;
        FMOD_DSP_DESCRIPTION_EX descriptionex;
        FMOD_SOUND_FORMAT       bufferformat;
        int                     bufferchannels;

        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::init", "Set up software engine\n"));

        /*
            Set up the 'software' output object.
        */
        mSoftware = FMOD_Object_Alloc(OutputSoftware);
        if (!mSoftware)
        {
            return FMOD_ERR_MEMORY;
        }   
        mSoftware->mSystem = this;

        if (!mDSPCrit)
        {
            result = FMOD_OS_CriticalSection_Create(&mDSPCrit);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (!mDSPLockCrit)
        {
            result = FMOD_OS_CriticalSection_Create(&mDSPLockCrit);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (!mDSPConnectionCrit)
        {
            result = FMOD_OS_CriticalSection_Create(&mDSPConnectionCrit);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
#ifdef FMOD_NEED_DSPCODECPOOLINITCRIT
        if(!mDSPCodecPoolInitCrit)
        {
            result = FMOD_OS_CriticalSection_Create(&mDSPCodecPoolInitCrit);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
#endif

        mConnectionRequestUsedHead.initNode();
        mConnectionRequestFreeHead.initNode();

        for (count = 0; count < FMOD_DSP_CONNECTION_REQUEST_MAX; count++)
        {
            DSPConnectionRequest *request = &mConnectionRequest[count];

            request->initNode();
            request->addBefore(&mConnectionRequestFreeHead);

        }

        result = getSoftwareFormat(0, &bufferformat, &bufferchannels, 0, 0, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (mMaxInputChannels < bufferchannels)
        {
            mMaxInputChannels = bufferchannels;
        }

        /*
            Create a global intermediate pair of buffers for the DSP engine to use.  
        */
        mDSPTempBuffMem = (float *)FMOD_Memory_Calloc((mDSPBlockSize * (bufferchannels < mMaxInputChannels ? mMaxInputChannels : bufferchannels) * sizeof(float)) + 16);
        if (!mDSPTempBuffMem)
        {
            return FMOD_ERR_MEMORY;
        }
        mDSPTempBuff = (float *)FMOD_ALIGNPOINTER(mDSPTempBuffMem, 16);

        /*
            Create a pool of connections so we dont have to use malloc/free each time a connection/disconnection happens.
            Usually with the default software engine it is:
            1 for soundcard to mixer.
            'mNumSoftwareChannels' for mixer to channel head(s)
            'mNumSoftwareChannels' for channelhead(s) to wavetable(s)/generator(s).
        */
        result = mDSPConnectionPool.init(this, 1 + (mNumSoftwareChannels * 2), bufferchannels < 2 ? 2 : bufferchannels, mMaxInputChannels);
        if (result != FMOD_OK)
        {
            return result;
        }

        /*
            Create the soundcard unit and main mixer unit then connect them together.
        */
        FMOD_memset(&descriptionex, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));
        FMOD_strcpy(descriptionex.name, "FMOD SoundCard Unit");
        descriptionex.version      = 0x00010100;
        descriptionex.channels     = bufferchannels;
        descriptionex.create       = 0;
        descriptionex.release      = 0;
        descriptionex.read         = 0;
        descriptionex.setposition  = 0;
        descriptionex.mCategory    = FMOD_DSP_CATEGORY_SOUNDCARD;
        descriptionex.mFormat      = bufferformat;
        result = createDSP(&descriptionex, &mDSPSoundCard);
        if (result != FMOD_OK)
        {
            return result;
        }

        #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
        mDSPSoundCard->updateTreeLevel(0);  /* Set root to 0 to get initial tree level for rest of network to base off. */
        #endif
        mDSPSoundCard->setActive(true);

        FMOD_memset(&description, 0, sizeof(FMOD_DSP_DESCRIPTION));
        FMOD_strcpy(description.name, "FMOD ChannelGroup Target Unit");
        description.version      = 0x00010100;
        description.channels     = 0;
        description.create       = 0;
        description.release      = 0;
        description.read         = 0;
        description.setposition  = 0;
        result = createDSP(&description, &mDSPChannelGroupTarget);
        if (result != FMOD_OK)
        {
            return result;
        }

        mDSPChannelGroupTarget->setDefaults((float)mOutputRate, -1, -1, -1);
        mDSPChannelGroupTarget->setActive(true);

        result = mDSPSoundCard->addInput(mDSPChannelGroupTarget);
        if (result != FMOD_OK)
        {
            return result;
        }
    }
#endif

    result = createChannelGroup("FMOD master group", &mChannelGroup);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = createSoundGroup("FMOD master group", &mSoundGroup);
    if (result != FMOD_OK)
    {
        return result;
    }

#ifdef FMOD_SUPPORT_SOFTWARE
    if (!(mFlags & FMOD_INIT_SOFTWARE_DISABLE))
    {
        result = mSoftware->init(mNumSoftwareChannels);
        if (result != FMOD_OK)
        {
            return result;
        }

        /*
            If the user wants to add extra code around the polled start, 
            check here for overriden start function.
            Plugin writer must call OutputPolled::start.
        */
        if (mOutput->mDescription.start)
        {
            mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */

            result = mOutput->mDescription.start(mOutput);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else if (mOutput->mDescription.polling)
        {
            OutputPolled *outputpolled = SAFE_CAST(OutputPolled, mOutput);
        
            result = outputpolled->start();
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }
#endif

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::init", "Set up emulated output\n"));

    /*
        Set up emulated output for virtual channels
    */
    if (maxchannels > 0)
    {       
        mEmulated = FMOD_Object_Alloc(OutputEmulated);
        if (!mEmulated)
        {
            return FMOD_ERR_MEMORY;
        }
        
        mEmulated->mSystem = this;

        result = mEmulated->init(maxchannels);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::init", "create the channel pool\n"));

    /*
        Create a pool of channels the user will interface with, that will contain pointers to real or virtual channels
    */
    if (maxchannels)
    {
        mChannel = (ChannelI *)FMOD_Memory_Calloc(sizeof(ChannelI) * maxchannels);
        if (!mChannel)
        {
            return FMOD_ERR_MEMORY;
        }

        mNumChannels = maxchannels;

        for (count = 0; count < mNumChannels; count++)
        {
            new (&mChannel[count]) ChannelI(count, this);
            
            mChannel[count].addAfter(&mChannelFreeListHead);
            mChannel[count].setChannelGroup(mChannelGroup);                 /* Put channels into primary channel group */
        }
    }

    /*
        Global list of sounds critical section
    */
    if (!gSoundListCrit)
    {
        result = FMOD_OS_CriticalSection_Create(&gSoundListCrit);
        if (result != FMOD_OK)
        {
            return result;
        }

        initCleanup.setSoundListCrit(&gSoundListCrit);
    }

    /*
        Sound lock critical section
    */
    if (!mMultiSubSampleLockBufferCrit)
    {
        result = FMOD_OS_CriticalSection_Create(&mMultiSubSampleLockBufferCrit);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

#ifdef FMOD_SUPPORT_STREAMING

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::init", "Set up streamer\n"));

    /*
        Create stream thread.
    */
    if (!mStreamThreadActive && !(mFlags & FMOD_INIT_STREAM_FROM_UPDATE))  /* Single buffered is not nonblocking! */
    {
        AutoFreeCrit realchanCritCleanup;
        AutoFreeCrit updateCritCleanup;
        AutoFreeCrit listCritCleanup;

        result = FMOD_OS_CriticalSection_Create(&mStreamRealchanCrit);
        if (result != FMOD_OK)
        {
            return result;
        }
        realchanCritCleanup = mStreamRealchanCrit;
        result = FMOD_OS_CriticalSection_Create(&mStreamUpdateCrit);
        if (result != FMOD_OK)
        {
            return result;
        }
        updateCritCleanup = mStreamUpdateCrit;
        result = FMOD_OS_CriticalSection_Create(&mStreamListCrit);
        if (result != FMOD_OK)
        {
            return result;
        }
        listCritCleanup = mStreamListCrit;

        result = mStreamThread.initThread("FMOD stream thread", streamThread, this, STREAM_THREADPRIORITY, 0, STREAM_STACKSIZE, false, 10, this);
        if (result != FMOD_OK)
        {
            return result;
        }
        mStreamThreadActive = true;
        realchanCritCleanup.releasePtr();
        updateCritCleanup.releasePtr();
        listCritCleanup.releasePtr();
    }

    if (!FMOD::gGlobal->gFileCrit)
    {
        result = FMOD_OS_CriticalSection_Create(&FMOD::gGlobal->gFileCrit);
        if (result != FMOD_OK)
        {
            return result;
        }
        initCleanup.setFileCrit(&FMOD::gGlobal->gFileCrit);
    }

    if (!FMOD::gGlobal->gAsyncCrit)
    {
        result = FMOD_OS_CriticalSection_Create(&FMOD::gGlobal->gAsyncCrit);
        if (result != FMOD_OK)
        {
            return result;
        }
        initCleanup.setAsyncCrit(&FMOD::gGlobal->gAsyncCrit);
    }

#endif

    /*
        Initialize codecs up front if specified.
    */
#ifdef FMOD_SUPPORT_DSPCODEC
    if (!(mFlags & FMOD_INIT_SOFTWARE_DISABLE))
    {
        #ifdef FMOD_SUPPORT_IMAADPCM
        if (mAdvancedSettings.maxADPCMcodecs)
        {
            int count;

            result = mDSPCodecPool_ADPCM.init(FMOD_DSP_CATEGORY_DSPCODECADPCM, 64, mAdvancedSettings.maxADPCMcodecs);
            if (result != FMOD_OK)
            {
                return result;
            }

            for (count = 0; count < mDSPCodecPool_ADPCM.mNumDSPCodecs; count++)
            {
                DSPCodec *dspcodec = SAFE_CAST(DSPCodec, mDSPCodecPool_ADPCM.mPool[count]);
                CodecWav *wav      = (CodecWav *)dspcodec->mCodec;

                wav->mSrcFormat  = &wav->mSrcFormatMemory;
                wav->mReadBuffer = mDSPCodecPool_ADPCM.mReadBuffer;
                wav->mSrcFormat->Format.wFormatTag = WAVE_FORMAT_IMA_ADPCM;
            }       
        }
        #endif
        #ifdef FMOD_SUPPORT_MPEG
        if (mAdvancedSettings.maxMPEGcodecs)
        {
            int count;

            result = mDSPCodecPool_MPEG.init(FMOD_DSP_CATEGORY_DSPCODECMPEG, 1152, mAdvancedSettings.maxMPEGcodecs);
            if (result != FMOD_OK)
            {
                return result;
            }

            for (count = 0; count < mDSPCodecPool_MPEG.mNumDSPCodecs; count++)
            {
                DSPCodec *dspcodec = SAFE_CAST(DSPCodec, mDSPCodecPool_MPEG.mPool[count]);
                CodecMPEG *mpeg    = (CodecMPEG *)dspcodec->mCodec;

                mpeg->mSrcDataOffset    = 0;    /* Raw data will start at 0. */
                mpeg->mWaveFormatMemory = 0;    /* This will be set up upon play. */
                mpeg->reset();            
            }       
        }
        #endif
        #ifdef FMOD_SUPPORT_XMA
        if (mAdvancedSettings.maxXMAcodecs)
        {
            int count;

            result = mDSPCodecPool_XMA.init(FMOD_DSP_CATEGORY_DSPCODECXMA, 512, mAdvancedSettings.maxXMAcodecs);
            if (result != FMOD_OK)
            {
                return result;
            }

            for (count = 0; count < mDSPCodecPool_XMA.mNumDSPCodecs; count++)
            {
                DSPCodec *dspcodec = SAFE_CAST(DSPCodec, mDSPCodecPool_XMA.mPool[count]);
                CodecXMA *xma      = (CodecXMA *)dspcodec->mCodec;

                xma->mSrcDataOffset = 0;    /* Raw data will start at 0. */
                xma->mWaveFormatMemory = 0;    /* This will be set up upon play. */
                
                result = xma->XMAInit(1);           
                if (result != FMOD_OK)
                {
                    return result;
                }
            }       
        }
        #endif
        #ifdef FMOD_SUPPORT_CELT
        if (mAdvancedSettings.maxCELTcodecs)
        {
            int count;
            FMOD_CODEC_DESCRIPTION_EX *desc;

            result = mPluginFactory->getCodec(mCELTPluginHandle, &desc);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mDSPCodecPool_CELT.init(FMOD_DSP_CATEGORY_DSPCODECCELT, FMOD_CELT_FRAMESIZESAMPLES, mAdvancedSettings.maxCELTcodecs);
            if (result != FMOD_OK)
            {
                close();
                return result;
            }

            for (count = 0; count < mDSPCodecPool_CELT.mNumDSPCodecs; count++)
            {
                DSPCodec  *dspcodec = SAFE_CAST(DSPCodec, mDSPCodecPool_CELT.mPool[count]);
                CodecCELT *celt     = (CodecCELT *)dspcodec->mCodec;

                celt->mSrcDataOffset = 0;       /* Raw data will start at 0. */
                celt->mWaveFormatMemory = 0;    /* This will be set up upon play. */

                result = desc->init(celt, 1);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }     
        }
        #endif
        #ifdef FMOD_SUPPORT_RAWCODEC
        if (mAdvancedSettings.maxPCMcodecs)
        {
            result = mDSPCodecPool_RAW.init(FMOD_DSP_CATEGORY_DSPCODECRAW, 256, mAdvancedSettings.maxPCMcodecs);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        #endif
    }
#endif

    /*
        Initialize 2D and 3D reverb masters
    */
    {
        const FMOD_REVERB_PROPERTIES prop = FMOD_PRESET_OFF;

        result = mReverbGlobal.init(this, false);
        if (result != FMOD_OK)
        {
            return result;
        }

#ifdef FMOD_SUPPORT_REVERB
        result = setReverbProperties(&prop);
        if (result != FMOD_OK)
        {
            return result;
        }
#endif

#ifdef FMOD_SUPPORT_MULTIREVERB
        result = mReverb3D.init(this, false);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = set3DReverbProperties(&prop);
        if (result != FMOD_OK)
        {
            return result;
        }
#endif
    }

    if (mFlags & FMOD_INIT_ENABLE_PROFILE)
    {    
#ifdef FMOD_SUPPORT_PROFILE
        result = FMOD_Profile_Create(mAdvancedSettings.profileport);
        if (result != FMOD_OK)
        {
            return result;
        }
        initCleanup.profilerInitialized();
#endif

#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_DSP)
        result = FMOD_ProfileDsp_Create();
        if (result != FMOD_OK)
        {
            return result;
        }
#endif
#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_MEMORY)
        result = FMOD_ProfileMemory_Create();
        if (result != FMOD_OK)
        {
            return result;
        }
#endif
#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_CPU)
        result = FMOD_ProfileCpu_Create();
        if (result != FMOD_OK)
        {
            return result;
        }
#endif
#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_CHANNEL)
        result = FMOD_ProfileChannel_Create();
        if (result != FMOD_OK)
        {
            return result;
        }
#endif
#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_CODEC)
        result = FMOD_ProfileCodec_Create();
        if (result != FMOD_OK)
        {
            return result;
        }
#endif
    }


#ifdef FMOD_SUPPORT_VSTPLUGIN
    mVSTPluginsListHead.initNode();
#endif

    mSpeakerLevelsPool.mSystem = this;
    result = mHistoryBufferPool.init(mAdvancedSettings.maxSpectrumWaveDataBuffers, FMOD_MAX(mMaxOutputChannels, mMaxInputChannels) );
    if (result != FMOD_OK)
    {
        return result;
    }

    mInitialized = true;
    
    gGlobal->gSystemInitCount++;

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::init", "done\n\n"));

    initCleanup.releasePtr();

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
// OFFSET: 0x008D9A20
FMOD_RESULT SystemI::close()
{
    return closeEx(false);
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
// OFFSET: 0x008D9550
FMOD_RESULT SystemI::closeEx(bool calledfrominit)
{
    FMOD_RESULT result;
    int count;

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::close", "\n"));

    update();   /* Execute an update to make sure everything is in sync. */

#ifdef FMOD_SUPPORT_RECORDING
    if (mOutput && mOutput->mRecordNumActive)
    {
        result = mOutput->recordStopAll(false);
        CHECK_RESULT(result);
    }
#endif

    /*
        Stop all sounds
    */
    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::close", "Stop all sounds\n"));
    for (count = 0; count < mNumChannels; count++)
    {
        mChannel[count].stopEx(CHANNELI_STOPFLAG_REFSTAMP | CHANNELI_STOPFLAG_UPDATELIST | CHANNELI_STOPFLAG_RESETCALLBACKS | CHANNELI_STOPFLAG_RESETCHANNELGROUP |CHANNELI_STOPFLAG_UPDATESYNCPOINTS);
    }

    update();   /* For ps2 it needs to execute an update to get the channel commands to execute. */

    #ifdef FMOD_SUPPORT_STREAMING
    if (mStreamThreadActive)
    {
        mStreamThread.closeThread();
        mStreamThreadActive = false;

        FMOD_OS_CriticalSection_Free(mStreamRealchanCrit);
        mStreamRealchanCrit = 0;

        FMOD_OS_CriticalSection_Free(mStreamUpdateCrit);
        mStreamUpdateCrit = 0;

        FMOD_OS_CriticalSection_Free(mStreamListCrit);
        mStreamListCrit = 0;

        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::close", "Stream thread destroyed\n"));
    }
    #endif

    /*
        Shut down streamer and async thread, only if this is the last instance of a system object.
    */
    if (gGlobal->gSystemInitCount == 1 && mInitialized)
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::close", "Shut down streamer and FMOD_NONBLOCKING and FileSystem thread.\n"));

        #ifdef FMOD_SUPPORT_NONBLOCKING
        result = AsyncThread::shutDown();
        if (result!= FMOD_OK)
        {
            return result;
        }
        #endif

        if (FMOD::gGlobal->gAsyncCrit)
        {
            FMOD_OS_CriticalSection_Free(FMOD::gGlobal->gAsyncCrit);
            FMOD::gGlobal->gAsyncCrit = 0;
        }

        if (gSoundListCrit)
        {
            FMOD_OS_CriticalSection_Free(gSoundListCrit);
            gSoundListCrit = 0;
        }
    
        if (mFlags & FMOD_INIT_ENABLE_PROFILE)
        {
#ifdef FMOD_SUPPORT_PROFILE
            result = FMOD_Profile_Release();
            if (result != FMOD_OK)
            {
                return result;
            }
#endif
        }
        

        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::close", "Shut down file system.\n"));
        result = File::shutDown();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        Recursively remove all channelgroups.
    */    
    if (mChannelGroup)
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::close", "Free master channel group.\n"));

        result = mChannelGroup->releaseInternal(true);
        if (result != FMOD_OK)
        {
            return result;
        }
        mChannelGroup = 0;
    }

    /*
        Remove 'main' sound group.
    */    
    if (mSoundGroup)
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::close", "Remove 'master' sound group.\n"));

        result = mSoundGroup->releaseInternal();
        if (result != FMOD_OK)
        {
            return result;
        }
        mSoundGroup = 0;
    }

    if (mOutput)
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::close", "Shut down output.\n"));

        if (mOutput->mDescription.stop)
        {
#ifdef FMOD_SUPPORT_SOFTWARE
            mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
            mOutput->readfrommixer = 0;
#endif

            mOutput->mDescription.stop(mOutput);
        }
        else if (mOutput->mDescription.polling)
        {
            OutputPolled *outputpolled = SAFE_CAST(OutputPolled, mOutput);
        
            outputpolled->stop();
        }
	}

#ifdef FMOD_SUPPORT_SOFTWARE
    if (mDSPChannelGroupTarget)
    {
        result = mDSPChannelGroupTarget->release();
        mDSPChannelGroupTarget = 0;
    }
#endif

#ifdef FMOD_SUPPORT_MULTIREVERB
    set3DReverbActive(false);  /* Stop it from creating new reverbs inside update functions. */

    ReverbI *reverb_c = SAFE_CAST(ReverbI, mReverb3DHead.getNext());
    ReverbI *reverb_tmp;
    
    while (reverb_c != &mReverb3DHead)
    {
        reverb_tmp = reverb_c;
        reverb_c = SAFE_CAST(ReverbI, reverb_c->getNext());
        reverb_tmp->release(true);
    }
    mReverb3D.release(false);
#endif

    mReverbGlobal.release(false);

#ifdef FMOD_SUPPORT_SOFTWARE
    if (mDSPTempBuffMem)
    {
        FMOD_Memory_Free(mDSPTempBuffMem);
        mDSPTempBuff = mDSPTempBuffMem = 0;
    }
#endif
	if (mOutput)
	{
	    /*
		    Dont clean up the output if it is coming from init, because the user set up the output pre-init.
	    */
        if (calledfrominit)
        {
            if (mOutput->mDescription.close)
            {
#ifdef FMOD_SUPPORT_SOFTWARE
                mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
                mOutput->readfrommixer = 0;
#endif

                mOutput->mDescription.close(mOutput);
            }
        }
        else
        {

            result = mOutput->release();
			mOutput = 0;
		}
    }
   
#ifdef FMOD_SUPPORT_SOFTWARE
    if (mSoftware)
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::close", "Free software output.\n"));

        result = mSoftware->release();
        mSoftware = 0;
    }
#endif

    if (mEmulated)
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::close", "Free emulated output.\n"));

        result = mEmulated->release();
        mEmulated = 0;
    }


    /*
        Free codec pools for FMOD_CREATECOMPRESSEDSAMPLE.
    */    
#ifdef FMOD_SUPPORT_DSPCODEC
    #ifdef FMOD_SUPPORT_MPEG
    result = mDSPCodecPool_MPEG.close();
    if (result != FMOD_OK)
    {
        return result;
    }
    #endif
    #ifdef FMOD_SUPPORT_XMA
    result = mDSPCodecPool_XMA.close();
    if (result != FMOD_OK)
    {
        return result;
    }
    #endif
    #ifdef FMOD_SUPPORT_IMAADPCM
    result = mDSPCodecPool_ADPCM.close();
    if (result != FMOD_OK)
    {
        return result;
    }
    #endif
    #ifdef FMOD_SUPPORT_RAWCODEC
    result = mDSPCodecPool_RAW.close();
    if (result != FMOD_OK)
    {
        return result;
    }
    #endif
#endif

    if (mChannel)
    {
        int count;

        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::close", "Free channel pool.\n"));

        for (count = 0; count < mNumChannels; count++)
        {
            if (mChannel[count].mLevels)
            {
                FMOD_Memory_Free(mChannel[count].mLevels);
                mChannel[count].mLevels = 0;
            }
        }

        FMOD_Memory_Free(mChannel);
        mChannel = 0;
        mNumChannels = 0;
    }

    mChannelFreeListHead.initNode();

#ifdef FMOD_SUPPORT_SOFTWARE

    if (mDSPSoundCard)
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::close", "Remove DSP Soundcard unit.\n"));

        result = mDSPSoundCard->release();  /* Release last because some things above point to it. */
        mDSPSoundCard = 0;
    }

    /*
        Remove miscllaneous DSP stuff
    */

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::close", "Remove miscllaneous DSP stuff.\n"));

    result = mDSPConnectionPool.close();
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < FMOD_DSP_MAXTREEDEPTH; count++)
    {
        if (mDSPMixBuff[count])
        {
            FMOD_Memory_Free(mDSPMixBuff[count]);
            mDSPMixBuff[count] = 0;
        }
    }

    if (mDSPCrit)
    {
        result = FMOD_OS_CriticalSection_Free(mDSPCrit);
        if (result != FMOD_OK)
        {
            return result;
        }
        mDSPCrit = 0;
    }
    if (mDSPLockCrit)
    {
        result = FMOD_OS_CriticalSection_Free(mDSPLockCrit);
        if (result != FMOD_OK)
        {
            return result;
        }
        mDSPLockCrit = 0;
    }
    if (mDSPConnectionCrit)
    {
        result = FMOD_OS_CriticalSection_Free(mDSPConnectionCrit);
        if (result != FMOD_OK)
        {
            return result;
        }
        mDSPConnectionCrit = 0;
    }
#ifdef FMOD_NEED_DSPCODECPOOLINITCRIT
    if (mDSPCodecPoolInitCrit)
    {
        result = FMOD_OS_CriticalSection_Free(mDSPCodecPoolInitCrit);
        if (result != FMOD_OK)
        {
            return result;
        }
        mDSPCodecPoolInitCrit = 0;
    }
#endif

#endif

#ifdef FMOD_SUPPORT_MPEG_SPU
    CodecMPEG::closeAll();
#endif

    /*
        Sound lock critical section
    */
    if (mMultiSubSampleLockBufferCrit)
    {
        FMOD_OS_CriticalSection_Free(mMultiSubSampleLockBufferCrit);
        mMultiSubSampleLockBufferCrit = 0;
    }
    
    mSpeakerLevelsPool.release();
    mHistoryBufferPool.release();

    if (mDownmix)
    {
        result = mDownmix->shutdown();
        if (result != FMOD_OK)
        {
            return result;
        }
        FMOD_Memory_Free(mDownmix);
        mDownmix = 0;
    }

    if (mPluginFactory && !calledfrominit)
    {
        CHECK_RESULT(mPluginFactory->release());
        mPluginFactory = 0;
        mPluginsLoaded = false;
    }

    if (mInitialized)
    {    
        gGlobal->gSystemInitCount--;
    }

    mInitialized = false;
    
    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::close", "done.\n\n"));

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
// OFFSET: 0x008D8FD0
FMOD_RESULT SystemI::update()
{
    FMOD_RESULT result;
    unsigned int currenttimestamp;
    int delta, count;

    if (!mInitialized)
    {
        return FMOD_ERR_UNINITIALIZED;
    }
   
    #ifdef FMOD_DEBUG
    {
        FMOD_UINT_NATIVE id;

        FMOD_OS_Thread_GetCurrentID(&id);
        
        if (id != mMainThreadID)
        {
            FLOG((FMOD_DEBUG_TYPE_THREAD, __FILE__, __LINE__, "SystemI::update", "Warning!  You are calling FMOD from different threads! This is not safe!\n"));
        }
    }
    #endif

    mUpdateTimeStamp.stampIn();

//    unsigned int a, b;
//    FMOD_OS_Time_GetNs(&a);

    /*
        Calculate the time delta since this was last called.
    */
    if (!mLastTimeStamp)
    {
        FMOD_OS_Time_GetMs(&mLastTimeStamp);
    }

    FMOD_OS_Time_GetMs(&currenttimestamp);

    if (currenttimestamp >= mLastTimeStamp)
    {
        delta = currenttimestamp - mLastTimeStamp;
    }
    else
    {
        /*
            Timestamp has wrapped around?
        */
        delta = currenttimestamp;
    }

    mLastTimeStamp = currenttimestamp;

    /*
        Update emulated output object.
    */
    if (mEmulated)
    {
        result = mEmulated->update();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

#ifdef FMOD_SUPPORT_MULTIREVERB
    /*
        Update reverb-specific details
    */
    result = update3DReverbs();
    if (result != FMOD_OK)
    {
        return result;
    }
#endif

    /*
        Update per channel stuff including virtual voice swapping.
    */
    result = updateChannels(delta);
    if (result != FMOD_OK)
    {
        return result;
    }

    /*
        Update any fading or volume logic by sound groups.
    */
    result = updateSoundGroups(delta);
    if (result != FMOD_OK)
    {
        return result;
    }

#ifdef FMOD_SUPPORT_SOFTWARE
    if (!mSoftware)
#endif
    {
        FMOD_OS_Time_GetMs(&FMOD::gGlobal->gDSPClockTimeStamp);
        FMOD::gGlobal->gDSPClock.mHi += delta;
    
        mDSPClock.mValue += ((FMOD_UINT64)delta * (FMOD_UINT64)mOutputRate / (FMOD_UINT64)1000);
    }

    /*
        Update hardware output object.
    */
    if (mOutput && mOutput->mDescription.update)
    {
        mUpdateTimeStamp.setPaused(true);
    
#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif

        result = mOutput->mDescription.update(mOutput);
        if (result != FMOD_OK)
        {
            return result;
        }
    
        mUpdateTimeStamp.setPaused(false);
    }

    /*
        Determine if the device list has changed
    */
    result = checkDriverList(true);
    if (result != FMOD_OK)
    {
        return result;
    }

#ifdef FMOD_SUPPORT_RECORDING
    /*
        Clean up recording device list, stop finished devices
    */
    if (mOutput && mOutput->mRecordNumActive)
    {
        result = mOutput->recordStopAll(true);
        CHECK_RESULT(result);
    }
#endif

    /*
        Clear moved flag from listeners.
    */
    for (count = 0; count < mNumListeners; count++)
    {
        mListener[count].mMoved = false;
        mListener[count].mRotated = false;
    }

#ifdef FMOD_SUPPORT_GEOMETRY
	mGeometryMgr.mMoved = false;
#endif

#ifdef FMOD_SUPPORT_NONBLOCKING
    result = AsyncThread::update();
    if (result != FMOD_OK)
    {
        return result;
    }
#endif

    mUpdateTimeStamp.stampOut(95);    

#ifdef FMOD_SUPPORT_STREAMING
    if (mFlags & FMOD_INIT_STREAM_FROM_UPDATE)
    {
        updateStreams();
    }
#endif

    if (mFlags & FMOD_INIT_SYNCMIXERWITHUPDATE)
    {
        if(mOutput->mDescription.polling)
        {
            OutputPolled *outputpolled = (OutputPolled *)mOutput;

            outputpolled->wakeupThread();
        }
    }

#ifdef FMOD_SUPPORT_PROFILE
    if (mFlags & FMOD_INIT_ENABLE_PROFILE)
    {
        result = FMOD_Profile_Update(this, delta);
        if (result != FMOD_OK)
        {
            return result;
        }
    }
#endif

#ifdef FMOD_SUPPORT_VSTPLUGIN
    updateVSTPlugins();
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
// OFFSET: 0x008D4180
FMOD_RESULT SystemI::set3DSettings(float dopplerscale, float distancescale, float rolloffscale)
{
    if (dopplerscale < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (distancescale <= 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (rolloffscale < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mDopplerScale  = dopplerscale;
    mDistanceScale = distancescale;
    mRolloffScale  = rolloffscale;

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
FMOD_RESULT SystemI::set3DNumListeners(int numlisteners)
{
    if (numlisteners < 1 || numlisteners > LISTENER_MAX)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    mNumListeners = numlisteners;

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
// OFFSET: 0x008D41E0
FMOD_RESULT SystemI::set3DListenerAttributes(int listener, const FMOD_VECTOR *pos, const FMOD_VECTOR *vel, const FMOD_VECTOR *forward, const FMOD_VECTOR *up)
{
    if (listener < 0 || listener >= LISTENER_MAX)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (forward)
    {
        #ifdef FMOD_DEBUG
        {
            FMOD_RESULT result;
            float length;

            result = FMOD_CHECKFLOAT(forward->x);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = FMOD_CHECKFLOAT(forward->y);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = FMOD_CHECKFLOAT(forward->z);
            if (result != FMOD_OK)
            {
                return result;
            }

            length = FMOD_Vector_DotProduct(forward, forward);
            if (length < 0.9f || length > 1.1f)
            {
                return FMOD_ERR_INVALID_VECTOR;
            }
        }
        #endif

        if (mListener[listener].mLastFront.x != forward->x ||
            mListener[listener].mLastFront.y != forward->y ||
            mListener[listener].mLastFront.z != forward->z)
        {
            mListener[listener].mRotated = true;
        }
 
        mListener[listener].mLastFront = mListener[listener].mFront;
        mListener[listener].mFront     = *forward;
    }

    if (up)
    {
        #ifdef FMOD_DEBUG
        {
            FMOD_RESULT result;
            float length;

            result = FMOD_CHECKFLOAT(up->x);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = FMOD_CHECKFLOAT(up->y);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = FMOD_CHECKFLOAT(up->z);
            if (result != FMOD_OK)
            {
                return result;
            }

            length = FMOD_Vector_DotProduct(up, up);
            if (length < 0.9f || length > 1.1f)
            {
                return FMOD_ERR_INVALID_VECTOR;
            }
        }
        #endif

        if (mListener[listener].mLastUp.x != up->x ||
            mListener[listener].mLastUp.y != up->y ||
            mListener[listener].mLastUp.z != up->z)
        {
            mListener[listener].mRotated = true;
        }

        mListener[listener].mLastUp = mListener[listener].mUp;
        mListener[listener].mUp     = *up;
    }

    if (pos)
    {
        #ifdef FMOD_DEBUG
        {
            FMOD_RESULT result;

            result = FMOD_CHECKFLOAT(pos->x);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = FMOD_CHECKFLOAT(pos->y);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = FMOD_CHECKFLOAT(pos->z);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        #endif

        if (mListener[listener].mLastPosition.x != pos->x ||
            mListener[listener].mLastPosition.y != pos->y ||
            mListener[listener].mLastPosition.z != pos->z)
        {
            mListener[listener].mMoved = true;
        }

        mListener[listener].mPosition     = *pos;
        mListener[listener].mLastPosition = mListener[listener].mPosition;
    }

    if (vel)
    {
        #ifdef FMOD_DEBUG
        {
            FMOD_RESULT result;

            result = FMOD_CHECKFLOAT(vel->x);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = FMOD_CHECKFLOAT(vel->y);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = FMOD_CHECKFLOAT(vel->z);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        #endif

        if (mListener[listener].mLastVelocity.x != vel->x ||
            mListener[listener].mLastVelocity.y != vel->y ||
            mListener[listener].mLastVelocity.z != vel->z)
        {
            mListener[listener].mMoved = true;
        }

        mListener[listener].mLastVelocity = mListener[listener].mVelocity;
        mListener[listener].mVelocity = *vel;
    }

    {
        FMOD_VECTOR l_front, l_up;
        
        l_up    = mListener[listener].mUp;
        l_front = mListener[listener].mFront;

        if (mFlags & FMOD_INIT_3D_RIGHTHANDED)
        {
            l_up.z    = -l_up.z;
            l_front.z = -l_front.z;
        }

        #ifdef FMOD_DEBUG
        {
            float dot = FMOD_Vector_DotProduct(&l_up, &l_front);

            if (dot < -0.01f || dot > 0.01f)
            {
                return FMOD_ERR_INVALID_VECTOR;
            }
        }
        #endif

        FMOD_Vector_CrossProduct(&l_up, &l_front, &mListener[listener].mRight);
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
// OFFSET: 0x008D44A0
FMOD_RESULT SystemI::set3DRolloffCallback(FMOD_3D_ROLLOFFCALLBACK callback)
{
    mRolloffCallback = callback;

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
// OFFSET: 0x008D44C0
FMOD_RESULT SystemI::set3DSpeakerPosition(FMOD_SPEAKER speaker, float x, float y, bool active)
{
    if (speaker < 0 || speaker >= FMOD_SPEAKER_MAX)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mSpeaker[speaker].mSpeaker    = speaker;
    mSpeaker[speaker].mPosition.x = x;
    mSpeaker[speaker].mPosition.y = 0;
    mSpeaker[speaker].mPosition.z = y;
    mSpeaker[speaker].mActive     = active;

    /* calculate XZ angle for sorting */
    mSpeaker[speaker].mXZAngle = FMOD_AngleSort_GetValue(mSpeaker[speaker].mPosition.x, mSpeaker[speaker].mPosition.z);

    return sortSpeakerList();
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
FMOD_RESULT SystemI::get3DSpeakerPosition(FMOD_SPEAKER speaker, float *x, float *y, bool *active)
{
    if (speaker < 0 || speaker >= FMOD_SPEAKER_MAX)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (x)
    {
        *x = mSpeaker[speaker].mPosition.x;
    }
    if (y)
    {
        *y = mSpeaker[speaker].mPosition.z;
    }
    if (active)
    {
        *active = mSpeaker[speaker].mActive;
    }
    
    return FMOD_OK;
}

#endif   // #ifndef FMOD_STATICFORPLUGINS


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
// OFFSET: 0x008D4600
FMOD_RESULT SystemI::get3DNumListeners(int *numlisteners)
{
    if (!numlisteners)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numlisteners = mNumListeners;

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
// OFFSET: 0x008D4630
FMOD_RESULT SystemI::get3DListenerAttributes(int listener, FMOD_VECTOR *pos, FMOD_VECTOR *vel, FMOD_VECTOR *forward, FMOD_VECTOR *up)
{
    if (listener < 0 || listener >= LISTENER_MAX)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (pos)
    {
        *pos = mListener[listener].mPosition;
    }

    if (vel)
    {
        *vel = mListener[listener].mVelocity;
    }

    if (forward)
    {
        *forward = mListener[listener].mFront;
    }

    if (up)
    {
        *up = mListener[listener].mUp;
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
// OFFSET: 0x008D46F0
FMOD_RESULT SystemI::get3DSettings(float *dopplerscale, float *distancescale, float *rolloffscale)
{
    if (dopplerscale)
    {
        *dopplerscale = mDopplerScale;
    }
    if (distancescale)
    {
        *distancescale = mDistanceScale;
    }
    if (rolloffscale)
    {
        *rolloffscale = mRolloffScale;
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
// OFFSET: 0x008D4730
FMOD_RESULT SystemI::getVersion(unsigned int *version)
{
    if (!version)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *version = FMOD_VERSION;

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
FMOD_RESULT SystemI::getOutputHandle(void **handle)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mOutput)
    {
        return FMOD_ERR_UNINITIALIZED;
    }

    if (mOutput->mDescription.gethandle)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif

        return mOutput->mDescription.gethandle(mOutput, handle);
    }
    
    return FMOD_OK;
}


#ifndef FMOD_STATICFORPLUGINS

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
FMOD_RESULT SystemI::getChannelsPlaying(int *channels)
{
    if (!channels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    *channels = mChannelUsedListHead.count();

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
FMOD_RESULT SystemI::getCPUUsage(float *dsp, float *stream, float *geometry, float *update, float *total)
{
    FMOD_RESULT result;
    float       totalcpu, usage;

    totalcpu = 0;

#ifdef FMOD_SUPPORT_SOFTWARE
    result = mDSPTimeStamp.getCPUUsage(&usage);
    if (result == FMOD_OK)
    {
        totalcpu += usage;

        if (dsp)
        {
            *dsp = usage;
        }
    }
#else
    if (dsp)
    {
        *dsp = 0;
    }
#endif

#ifdef FMOD_SUPPORT_STREAMING
    result = mStreamTimeStamp.getCPUUsage(&usage);
    if (result == FMOD_OK)
    {
        totalcpu += usage;

        if (stream)
        {
            *stream = usage;
        }
    }
#else
    if (stream)
    {
        *stream = 0;
    }
#endif

#ifdef FMOD_SUPPORT_GEOMETRY_THREADED
    result = mGeometryTimeStamp.getCPUUsage(&usage);
    if (result == FMOD_OK)
    {
        totalcpu += usage;

        if (geometry)
        {
            *geometry = usage;
        }
    }
#else
    if (geometry)
    {
        *geometry = 0;
    }
#endif

    result = mUpdateTimeStamp.getCPUUsage(&usage);
    if (result == FMOD_OK)
    {
        totalcpu += usage;

        if (update)
        {
            *update = usage;
        }
    }

    if (total)
    {
        *total = totalcpu;
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
FMOD_RESULT SystemI::getSoundRAM(int *currentalloced, int *maxalloced, int *total)
{
    if (mOutput && mOutput->mDescription.getsoundram)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif

        mOutput->mDescription.getsoundram(mOutput, currentalloced, maxalloced, total);
    }
    else
    {
        if (currentalloced)
        {
            *currentalloced = 0;
        }
        if (maxalloced)
        {
            *maxalloced = 0;
        }
        if (total)
        {
            *total = 0;
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
FMOD_RESULT SystemI::getNumCDROMDrives(int *numdrives)
{
#ifdef FMOD_SUPPORT_CDDA
    return FMOD_OS_CDDA_GetNumDevices(numdrives);

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
FMOD_RESULT SystemI::getCDROMDriveName(int drive, char *drivename, int drivenamelen, char *scsiname, int scsinamelen, char *devicename, int devicenamelen)
{
#ifdef FMOD_SUPPORT_CDDA
    return FMOD_OS_CDDA_GetDeviceName(drive, drivename, drivenamelen, scsiname, scsinamelen, devicename, devicenamelen);
#else
    return FMOD_ERR_UNSUPPORTED;
#endif
}



/*
[
	[DESCRIPTION]
    Retrieves the spectrum from the currently playing output signal.

	[PARAMETERS]
    'spectrumarray'     Pointer to an array of floats to receive spectrum data.  Data range is 0-1.  Decibels = 10.0f * (float)log10(val) * 2.0f;
    'numvalues'         Size of array in floating point values being passed to the function.  Must be a power of 2. (ie 128/256/512 etc).  Min = 64.  Max = 8192.
    'channeloffset'     Channel to analyze.  If the signal is multichannel (such as a stereo output), then this value represents which channel to analyze.  On a stereo signal 0 = left, 1 = right.
    'windowtype'        Pre-FFT window method.  This filters the PCM data before entering the spectrum analyzer to reduce transient frequency error for more accurate results.  See FMOD_DSP_FFT_WINDOW for different types of fft window techniques possible and for a more detailed explanation.
     
	[RETURN_VALUE]

	[REMARKS]
    The larger the windowsize, the more CPU the FFT will take.  Choose the right value to trade off between accuracy / speed.<br>
    The larger the windowsize, the more 'lag' the spectrum will seem to inherit.  This is because the window size stretches the analysis back in time to what was already played.  For example if the window size happened to be 44100 and the output rate was 44100 it would be analyzing the past second of data, and giving you the average spectrum over that time period.<br>
    If you are not displaying the result in dB, then the data may seem smaller than it should be.  To display it you may want to normalize the data - that is, find the maximum value in the resulting spectrum, and scale all values in the array by 1 / max.  (ie if the max was 0.5f, then it would become 1).<br>
    To get the spectrum for both channels of a stereo signal, call this function twice, once with channeloffset = 0, and again with channeloffset = 1.  Then add the spectrums together and divide by 2 to get the average spectrum for both channels.<br>

    [PLATFORMS]

	[SEE_ALSO]
    FMOD_DSP_FFT_WINDOW
]
*/
FMOD_RESULT SystemI::getSpectrum(float *spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
#ifdef FMOD_SUPPORT_GETSPECTRUM
    FMOD_RESULT     result = FMOD_OK;
    #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
    DSPI           *soundcard;
    #else
    DSPSoundCard   *soundcard;
    #endif
    float          *buffer;
    unsigned int    position, length;
    int             numchannels, windowsize;
    static DSPFFT   fft;

    if (!mDSPSoundCard)
    {
        return FMOD_ERR_INITIALIZATION;
    }
    
    #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
    soundcard = SAFE_CAST(DSPI, mDSPSoundCard);
    #else
    soundcard = SAFE_CAST(DSPSoundCard, mDSPSoundCard);
    #endif
    if (!soundcard)
    {
        return FMOD_ERR_INITIALIZATION;
    }

    windowsize = numvalues * 2;

    if (windowsize != (1 <<  7) &&
        windowsize != (1 <<  8) &&
        windowsize != (1 <<  9) &&
        windowsize != (1 << 10) &&
        windowsize != (1 << 11) &&
        windowsize != (1 << 12) &&
        windowsize != (1 << 13) &&
        windowsize != (1 << 14))
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = getSoftwareFormat(0, 0, &numchannels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (channeloffset >= numchannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = soundcard->startBuffering();
    if (result != FMOD_OK)
    {
        return result;
    }

    result = soundcard->getHistoryBuffer(&buffer, &position, &length);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (windowsize > (int)length)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
   
    position -= windowsize;
    if ((int)position < 0)
    {
        position += length;
    }

    mUpdateTimeStamp.stampIn();

    result = fft.getSpectrum(buffer, position, length, spectrumarray, windowsize, channeloffset, numchannels, windowtype);

    mUpdateTimeStamp.stampOut(95);

    return result;
#else
    return FMOD_ERR_NEEDSSOFTWARE;
#endif // FMOD_SUPPORT_GETSPECTRUM
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
    FMOD_DSP_FFT_WINDOW
]
*/
FMOD_RESULT SystemI::getWaveData(float *wavearray, int numvalues, int channeloffset)
{
#ifdef FMOD_SUPPORT_SOFTWARE
    FMOD_RESULT     result = FMOD_OK;
    #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
    DSPI           *soundcard;
    #else
    DSPSoundCard   *soundcard;
    #endif
    float          *buffer;
    unsigned int    position, length;
    int             numchannels, count;

    if (!mDSPSoundCard)
    {
        return FMOD_ERR_INITIALIZATION;
    }

    #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
    soundcard = SAFE_CAST(DSPI, mDSPSoundCard);
    #else
    soundcard = SAFE_CAST(DSPSoundCard, mDSPSoundCard);
    #endif
    if (!soundcard)
    {
        return FMOD_ERR_INITIALIZATION;
    }

    result = getSoftwareFormat(0, 0, &numchannels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (channeloffset >= numchannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = soundcard->startBuffering();
    if (result != FMOD_OK)
    {
        return result;
    }

    result = soundcard->getHistoryBuffer(&buffer, &position, &length);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (numvalues > (int)length)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    position -= numvalues;
    if ((int)position < 0)
    {
        position += length;
    }

    for (count = 0; count < numvalues; count++)
    {
        wavearray[count] = buffer[position*numchannels+channeloffset];
        position++;
        if (position >= length)
        {
            position = 0;
        }
    }

    return result;
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
// OFFSET: 0x008D91D0
FMOD_RESULT SystemI::createSound(const char *name_or_data, FMOD_MODE mode_in, FMOD_CREATESOUNDEXINFO *exinfo, SoundI **sound)
{
    FMOD_RESULT result;

	if (!mInitialized)
	{
		return FMOD_ERR_UNINITIALIZED;
	}

    if (!sound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (!name_or_data && !(mode_in & FMOD_OPENUSER))
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (mode_in & FMOD_NONBLOCKING && !(mode_in & (FMOD_SOFTWARE | FMOD_HARDWARE)))
    {
        /*
            When using nonblocking, we can't rely on the codec to determine hardware or software if
            neither is specified. Since the sample is created of the type of the output mode and
            returned to the caller, we can't change it later if the codec requires it.
        */
        mode_in |= FMOD_HARDWARE;
    }
    if (!(mode_in & FMOD_SOFTWARE))
    {
        if (!mOutput)
        {
            return FMOD_ERR_OUTPUT_NOHARDWARE;
        }
    }

    #if !defined(PLATFORM_WII) && !defined(PLATFORM_PSP)  // We can allow this on the Wii, and PSP
    if (mode_in & FMOD_HARDWARE && mode_in & FMOD_OPENMEMORY_POINT && !(mode_in & FMOD_CREATESTREAM))
    {
        return FMOD_ERR_NEEDSSOFTWARE;
    }
    #endif

    *sound = 0;

#ifdef FMOD_SUPPORT_NONBLOCKING

    if (mode_in & FMOD_NONBLOCKING)
    {
        SoundI            *soundi;

        #ifdef FMOD_DEBUG
        if (mode_in & (FMOD_OPENMEMORY | FMOD_OPENMEMORY_POINT))
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSound", "memory = %p : mode %08x\n", name_or_data, mode_in));
        }
        else
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSound", "filename = %s : mode %08x\n", name_or_data, mode_in));
        }
        #endif

        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSound", "FMOD_NONBLOCKING specified.  Putting into queue to be opened asynchronously!\n"));

#ifdef FMOD_SUPPORT_STREAMING
        if (mode_in & FMOD_CREATESTREAM)
        {
            Stream *stream = FMOD_Object_Calloc(Stream);
            if (!stream)
            {
                return FMOD_ERR_MEMORY;
            }

            *sound = stream;
        }
        else
#endif
        {
            Sample *sample = 0;
            int     hwchannels = 0;

            if (!(mode_in & FMOD_SOFTWARE))
            {   
                getHardwareChannels(0, 0, &hwchannels);
            }

            if (hwchannels && mOutput->mDescription.createsample)
            {
#ifdef FMOD_SUPPORT_SOFTWARE
                mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
                mOutput->readfrommixer = 0;
#endif

                result = mOutput->mDescription.createsample(mOutput, (FMOD_MODE)0, 0, &sample);
                mCreatedHardwareSample = true;
            }
            else
            {
#ifdef FMOD_SUPPORT_SOFTWARE
                result = mSoftware->createSample((FMOD_MODE)0, 0, &sample);
#else
                result = FMOD_ERR_NEEDSSOFTWARE;
                FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSound", "ERROR - Software not initialized\n"));
#endif
            }
            if (result != FMOD_OK)
            {
                return result;
            }

            *sound = sample;
        }

        soundi = SAFE_CAST(SoundI, *sound);

        /*
            Calculate the size of the AsyncData plus any data that's pointed to by the exinfo struct.
            We need to do a deep copy of this exinfo struct soon.
        */
        int sizeof_asyncdata = sizeof(AsyncData);

        if (exinfo)
        {
            sizeof_asyncdata += (exinfo->inclusionlistnum * sizeof(int));

            if (exinfo->dlsname)
            {
                sizeof_asyncdata += (FMOD_strlen(exinfo->dlsname) + 1);
            }

            if (exinfo->encryptionkey)
            {
                sizeof_asyncdata += (FMOD_strlen(exinfo->encryptionkey) + 1);
            }
        }

        soundi->mAsyncData = (AsyncData *)FMOD_Memory_Calloc(sizeof_asyncdata);
        if (!soundi->mAsyncData)
        {
            return FMOD_ERR_MEMORY;
        }

        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSound", "allocated async data mem\n"));

        if (mode_in & (FMOD_OPENMEMORY | FMOD_OPENMEMORY_POINT))
        {
            soundi->mAsyncData->mNameData = (void *)name_or_data;
        }
        else if (name_or_data)
        {
            if (mode_in & FMOD_UNICODE)
            {
                // don't need to divide FMOD_STRING_MAXNAMELEN by 2 here, as AsyncData::mName is big enough
                FMOD_strncpyW((short *)soundi->mAsyncData->mName, (short *)name_or_data, FMOD_STRING_MAXNAMELEN); 
            }
            else
            {
                FMOD_strncpy(soundi->mAsyncData->mName, name_or_data, FMOD_STRING_MAXNAMELEN); 
            }
        }

        soundi->mAsyncData->mBufferSize = mStreamFileBufferSize;
        soundi->mAsyncData->mBufferSizeType = mStreamFileBufferSizeType;

        soundi->mMode       = mode_in;
        soundi->mSystem     = this;
        soundi->mOpenState  = FMOD_OPENSTATE_LOADING;

        if (exinfo)
        {
            FMOD_memcpy(&soundi->mAsyncData->mExInfo, exinfo, sizeof(FMOD_CREATESOUNDEXINFO));
            soundi->mAsyncData->mExInfoExists = true;

            if (exinfo->initialsoundgroup)
            {
                soundi->setSoundGroup((FMOD::SoundGroupI *)exinfo->initialsoundgroup);
            }

            /*
                Now do a deep copy of things that are pointed to by the exinfo struct by tacking them
                on the end of the soundi->mAsyncData memory block that we previously allocated and diddling
                the pointers to them. We calculated the size of this block to take into account this extra 
                stuff so we're not going to stomp on memory.
            */
            char *p = (char *)(soundi->mAsyncData + 1); // Pointer arithmetic. This puts us at the first byte after the end of the mAsyncData struct

            if (soundi->mAsyncData->mExInfo.inclusionlistnum)
            {
                FMOD_memcpy(p, soundi->mAsyncData->mExInfo.inclusionlist, soundi->mAsyncData->mExInfo.inclusionlistnum * sizeof(int));
                soundi->mAsyncData->mExInfo.inclusionlist = (int *)p;
                p += (soundi->mAsyncData->mExInfo.inclusionlistnum * sizeof(int));
            }

            if (soundi->mAsyncData->mExInfo.dlsname)
            {
                FMOD_strcpy(p, soundi->mAsyncData->mExInfo.dlsname);
                soundi->mAsyncData->mExInfo.dlsname = p;
                p += (FMOD_strlen(soundi->mAsyncData->mExInfo.dlsname) + 1);
            }

            if (soundi->mAsyncData->mExInfo.encryptionkey)
            {
                FMOD_strcpy(p, soundi->mAsyncData->mExInfo.encryptionkey);
                soundi->mAsyncData->mExInfo.encryptionkey = p;
                p += (FMOD_strlen(soundi->mAsyncData->mExInfo.encryptionkey) + 1);
            }
        }
        else
        {
            soundi->mAsyncData->mExInfoExists = false;
        }

        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSound", "getasyncthread\n"));

        result = AsyncThread::getAsyncThread(soundi);
        if (result != FMOD_OK)
        {
            soundi->mOpenState = FMOD_OPENSTATE_ERROR;  /* Stop it from hanging in release because state is 'loading'. */
            soundi->release();
            *sound = 0;
            return result;
        }

        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSound", "setdata soundi = %p : node = %p\n", soundi, &soundi->mAsyncData->mNode));

        FMOD_OS_CriticalSection_Enter(soundi->mAsyncData->mThread->mCrit);
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSound", "add node to async list : head = %p.  list count = %d\n", &soundi->mAsyncData->mThread->mHead, soundi->mAsyncData->mThread->mHead.count()));
            soundi->mAsyncData->mNode.setData(soundi);
            soundi->mAsyncData->mNode.addBefore(&soundi->mAsyncData->mThread->mHead);
        }
        FMOD_OS_CriticalSection_Leave(soundi->mAsyncData->mThread->mCrit);

        soundi->mAsyncData->mThread->mThread.wakeupThread();
    
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "SystemI::createSound", "done\n"));
    }
    else
#endif
    {
        if (exinfo)
        {
            FMOD_CREATESOUNDEXINFO exinfo_copy;
        
            memcpy(&exinfo_copy, exinfo, sizeof(FMOD_CREATESOUNDEXINFO));   /* Just in case the createSoundInternal modifies the exinfo struct internally somewhere. */
            
            result = createSoundInternal(name_or_data, mode_in, mStreamFileBufferSize, mStreamFileBufferSizeType, &exinfo_copy, false, sound);
        }
        else
        {
            result = createSoundInternal(name_or_data, mode_in, mStreamFileBufferSize, mStreamFileBufferSizeType, exinfo, false, sound);
        }
        
        if (*sound && exinfo && exinfo->initialsoundgroup)
        {
            (*sound)->setSoundGroup((FMOD::SoundGroupI *)exinfo->initialsoundgroup);
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
// OFFSET: 0x008D9520
FMOD_RESULT SystemI::createStream(const char *name_or_data, FMOD_MODE mode_in, FMOD_CREATESOUNDEXINFO *exinfo, SoundI **sound)
{
#ifdef FMOD_SUPPORT_STREAMING
    return createSound(name_or_data, mode_in | FMOD_CREATESTREAM, exinfo, sound);
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
FMOD_RESULT SystemI::createCodec(FMOD_CODEC_DESCRIPTION *description, unsigned int priority)
{   
    if (!description)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

#ifdef FMOD_SUPPORT_SOFTWARE
    if (mFlags & FMOD_INIT_SOFTWARE_DISABLE)
#endif
    {
        return FMOD_ERR_NEEDSSOFTWARE;
    }

#ifdef FMOD_SUPPORT_SOFTWARE
    {
        FMOD_RESULT result;
        FMOD_CODEC_DESCRIPTION_EX descriptionex;

        descriptionex.name            = description->name;
        descriptionex.version         = description->version;
        descriptionex.timeunits       = description->timeunits;
        descriptionex.defaultasstream = description->defaultasstream;
        descriptionex.open            = description->open;
        descriptionex.close           = description->close;
        descriptionex.read            = description->read;
        descriptionex.getlength       = description->getlength;
        descriptionex.setposition     = description->setposition;
        descriptionex.getposition     = description->getposition;
        descriptionex.soundcreate     = description->soundcreate;
        descriptionex.getwaveformat   = description->getwaveformat;
        descriptionex.mType           = FMOD_SOUND_TYPE_UNKNOWN;
        descriptionex.mSize           = sizeof(Codec);
        descriptionex.mModule         = 0;
        descriptionex.reset           = 0;

        result = mPluginFactory->registerCodec(&descriptionex, 0, priority);
        if (result != FMOD_OK)
        {
            return result;
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
// OFFSET: 0x008D5CD0
FMOD_RESULT SystemI::createDSP(FMOD_DSP_DESCRIPTION *description, DSPI **dsp)
{   
    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *dsp = 0;

    if (!description)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

#ifdef FMOD_SUPPORT_SOFTWARE
    if (mFlags & FMOD_INIT_SOFTWARE_DISABLE)
#endif
    {
        return FMOD_ERR_NEEDSSOFTWARE;
    }

#ifdef FMOD_SUPPORT_SOFTWARE
    {
        FMOD_RESULT result;
        FMOD_DSP_DESCRIPTION_EX descriptionex;

        FMOD_strcpy(descriptionex.name, description->name);
        descriptionex.version               = description->version;
        descriptionex.channels              = description->channels;
        descriptionex.create                = description->create;
        descriptionex.release               = description->release;
        descriptionex.reset                 = description->reset;
        descriptionex.read                  = description->read;
        descriptionex.setposition           = description->setposition;
                                            
        descriptionex.numparameters         = description->numparameters;
        descriptionex.paramdesc             = description->paramdesc;
        descriptionex.setparameter          = description->setparameter;
        descriptionex.getparameter          = description->getparameter;
        descriptionex.config                = description->config;
        descriptionex.configwidth           = description->configwidth;
        descriptionex.configheight          = description->configheight;
        descriptionex.userdata              = description->userdata;
        descriptionex.getmemoryused         = 0;

        descriptionex.mSize                 = 0;
        descriptionex.mCategory             = FMOD_DSP_CATEGORY_FILTER;
        descriptionex.mFormat               = FMOD_SOUND_FORMAT_PCMFLOAT;
#ifdef FMOD_SUPPORT_DLLS
        descriptionex.mModule               = 0;
        descriptionex.mAEffect              = 0;
#endif
        descriptionex.mResamplerBlockLength = 0;
        descriptionex.mType                 = FMOD_DSP_TYPE_UNKNOWN;
        descriptionex.mDSPSoundCard         = mDSPSoundCard;

        result = mPluginFactory->createDSP(&descriptionex, dsp);
        if (result != FMOD_OK)
        {
            return result;
        }

        (*dsp)->mSystem = this;
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
// OFFSET: 0x008D5DE0
FMOD_RESULT SystemI::createDSP(FMOD_DSP_DESCRIPTION_EX *description, DSPI **dsp, bool allocate)
{   
    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (allocate)
    {
        *dsp = 0;
    }

    if (!description)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

#ifdef FMOD_SUPPORT_SOFTWARE
    if (mFlags & FMOD_INIT_SOFTWARE_DISABLE)
#endif
    {
        return FMOD_ERR_NEEDSSOFTWARE;
    }

#ifdef FMOD_SUPPORT_SOFTWARE
    {
        FMOD_RESULT result;
        FMOD_DSP_DESCRIPTION_EX descriptionex;

        FMOD_strcpy(descriptionex.name, description->name);
        descriptionex.version               = description->version;
        descriptionex.channels              = description->channels;
        descriptionex.create                = description->create;
        descriptionex.release               = description->release;
        descriptionex.reset                 = description->reset;
        descriptionex.read                  = description->read;
        descriptionex.setposition           = description->setposition;
                                            
        descriptionex.numparameters         = description->numparameters;
        descriptionex.paramdesc             = description->paramdesc;
        descriptionex.setparameter          = description->setparameter;
        descriptionex.getparameter          = description->getparameter;
        descriptionex.config                = description->config;
        descriptionex.configwidth           = description->configwidth;
        descriptionex.configheight          = description->configheight;
        descriptionex.userdata              = description->userdata;
        descriptionex.getmemoryused         = description->getmemoryused;
                                            
        descriptionex.mSize                 = description->mSize;
        descriptionex.mCategory             = description->mCategory;
        descriptionex.mFormat               = description->mFormat;
#ifdef FMOD_SUPPORT_DLLS
        descriptionex.mModule               = 0;
        descriptionex.mAEffect              = description->mAEffect;
#endif
        descriptionex.mResamplerBlockLength = description->mResamplerBlockLength;
        descriptionex.mType                 = description->mType;
        descriptionex.mDSPSoundCard         = description->mDSPSoundCard;

        result = mPluginFactory->createDSP(&descriptionex, dsp);
        if (result != FMOD_OK)
        {
            return result;
        }

        (*dsp)->mSystem = this;
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
// OFFSET: 0x008D5F10
FMOD_RESULT SystemI::createDSPByType(FMOD_DSP_TYPE type, DSPI **dsp)
{
#ifdef FMOD_SUPPORT_SOFTWARE
    FMOD_RESULT result;
    
    if (!mPluginFactory)
    {
        return FMOD_ERR_UNINITIALIZED;
    }

    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *dsp = 0;

    if (type == FMOD_DSP_TYPE_MIXER)
    {
        FMOD_DSP_DESCRIPTION desc;

        FMOD_memset(&desc, 0, sizeof(FMOD_DSP_DESCRIPTION));

        FMOD_strcpy(desc.name, "FMOD Mixer unit");

        result = createDSP(&desc, dsp);
        if (result != FMOD_OK)
        {
            return result;
        }

        (*dsp)->mDescription.mType = FMOD_DSP_TYPE_MIXER;
    }
    else
    {
        int count, numdsps;

        result = mPluginFactory->getNumDSPs(&numdsps);
        if (result != FMOD_OK)
        {
            return result;
        }

        for (count = 0; count < numdsps; count++)
        {
            FMOD_DSP_DESCRIPTION_EX *descriptionex = 0;
            unsigned int handle;

            result = mPluginFactory->getDSPHandle(count, &handle);
            if (result != FMOD_OK)
            {
                continue;
            }

            result = mPluginFactory->getDSP(handle, &descriptionex);
            if (result != FMOD_OK)
            {
                continue;
            }

            if (descriptionex->mType == type)
            {
                result = mPluginFactory->createDSP(descriptionex, dsp);
                if (result != FMOD_OK)
                {
                    return result;
                }

                return FMOD_OK;
            }
        }
        
        return FMOD_ERR_PLUGIN_MISSING;
    }

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
FMOD_RESULT SystemI::createDSPByPlugin(unsigned int handle, DSPI **dsp)
{
#ifdef FMOD_SUPPORT_SOFTWARE
    FMOD_DSP_DESCRIPTION_EX *descriptionex = 0;
    FMOD_RESULT result;
    
    if (!mPluginFactory)
    {
        return FMOD_ERR_UNINITIALIZED;
    }

    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *dsp = 0;

    result = mPluginFactory->getDSP(handle, &descriptionex);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mPluginFactory->createDSP(descriptionex, dsp);
    if (result != FMOD_OK)
    {
        return result;
    }

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
FMOD_RESULT SystemI::createChannelGroupInternal(const char *name, ChannelGroupI **channelgroup, bool createdsp, bool storenameinchannelgroup)
{
    ChannelGroupI *newchannelgroup;
    AutoRelease<ChannelGroupI> newchannelgroupauto;

    if (!channelgroup)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
#ifdef FMOD_SUPPORT_SOFTWARE
    if (mSoftware && createdsp)
    {
        newchannelgroup = FMOD_Object_Calloc(ChannelGroupSoftware);
    }
    else
#endif
    {
        newchannelgroup = FMOD_Object_Calloc(ChannelGroupI);
    }

    if (!newchannelgroup)
    {
        return FMOD_ERR_MEMORY;
    }

    newchannelgroupauto = newchannelgroup;

    newchannelgroup->addAfter(&mChannelGroupHead);
    newchannelgroup->mSystem = this;

    if (name && storenameinchannelgroup)
    {
        newchannelgroup->mName = FMOD_strdup(name);
        if (!newchannelgroup->mName)
        {
            return FMOD_ERR_MEMORY;
        }
    }
    else
    {
        newchannelgroup->mName = 0;
    }

    /*
        Create a DSP unit for this channelgroup then add it to the soundcard unit.
    */
#ifdef FMOD_SUPPORT_SOFTWARE
    if (mSoftware)
    {
        if (createdsp)
        {
            FMOD_RESULT             result;
            FMOD_DSP_DESCRIPTION_EX description;

            FMOD_memset(&description, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

            FMOD_strcpy(description.name, "ChannelGroup");
            if (name)
            {
                FMOD_strcat(description.name, ":");
                /*
                    description.name is hardwired to 32 so don't overflow it
                */
                FMOD_strncat(description.name, name, 18);
            }
            description.version      = 0x00010100;

            newchannelgroup->mDSPHead = &((ChannelGroupSoftware *)newchannelgroup)->mDSPHeadMemory;
    #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
            newchannelgroup->mDSPHead = (DSPI *)FMOD_ALIGNPOINTER(newchannelgroup->mDSPHead, 128); 
            new (newchannelgroup->mDSPHead) DSPFilter;
    #endif

            result = createDSP(&description, (DSPI **)&newchannelgroup->mDSPHead, false);
            if (result != FMOD_OK)
            {
                return result;
            }
            newchannelgroup->mDSPHead->setDefaults((float)mOutputRate, -1, -1, -1);
            newchannelgroup->mDSPHead->setActive(true);

            result = mDSPChannelGroupTarget->addInputQueued(newchannelgroup->mDSPHead, false, 0, 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            newchannelgroup->mDSPMixTarget = newchannelgroup->mDSPHead;
        }
        else
        {
            newchannelgroup->mDSPMixTarget = mDSPChannelGroupTarget;
        }
    }
#endif

    newchannelgroupauto.releasePtr();

    if (name && !FMOD_stricmp("music", name))
    {
        /*
            Store pointer to "music" channelgroup.
        */

        mOutput->mMusicChannelGroup = newchannelgroup;
    }

    *channelgroup = newchannelgroup;

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
// OFFSET: 0x008D6000
FMOD_RESULT SystemI::createChannelGroup(const char *name, ChannelGroupI **channelgroup)
{
    if (!channelgroup)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
#ifdef FMOD_SUPPORT_SOFTWARE
    if (mSoftware)
    {
        return createChannelGroupInternal(name, channelgroup, true, true);
    }
    else
#endif

    return createChannelGroupInternal(name, channelgroup, false, true);
}


#ifndef FMOD_STATICFORPLUGINS
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
// OFFSET: 0x008D4750
FMOD_RESULT SystemI::createSoundGroup(const char *name, SoundGroupI **soundgroup)
{
    SoundGroupI *newsoundgroup;
    AutoCritRelease<SoundGroupI> autonewsoundgroup(gSoundListCrit);

    if (!soundgroup)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    newsoundgroup = FMOD_Object_Calloc(SoundGroupI);
    if (!newsoundgroup)
    {
        return FMOD_ERR_MEMORY;
    }

    FMOD_OS_CriticalSection_Enter(gSoundListCrit);
    {
        newsoundgroup->addAfter(&mSoundGroupFreeHead);
        newsoundgroup->mSystem = this;
    }
    FMOD_OS_CriticalSection_Leave(gSoundListCrit);

    autonewsoundgroup = newsoundgroup;

    if (name)
    {
        newsoundgroup->mName = FMOD_strdup(name);
        if (!newsoundgroup->mName)
        {
            return FMOD_ERR_MEMORY;
        }
    }
    else
    {
        newsoundgroup->mName = 0;
    }

    autonewsoundgroup.releasePtr();

    *soundgroup = newsoundgroup;

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
// OFFSET: 0x008D61A0
FMOD_RESULT SystemI::playSound(FMOD_CHANNELINDEX channelid, SoundI *sound, bool paused, ChannelI **channel)
{
    FMOD_RESULT  result;
    ChannelI    *chan = 0;
    bool         mute = false;

    if (channel)
    {
        if (channelid == FMOD_CHANNEL_REUSE)
        {
            result = ChannelI::validate((Channel *)*channel, &chan);
        }
    }

    if (!sound)
    {
        if (channel)
        {
            *channel = 0;
        }
        return FMOD_ERR_INVALID_PARAM;
    }
   
    if (sound->mOpenState != FMOD_OPENSTATE_READY)
    {
        if (channel)
        {
            *channel = 0;
        }
        return FMOD_ERR_NOTREADY;
    }

    if (sound->mType == FMOD_SOUND_TYPE_PLAYLIST)
    {
        return FMOD_ERR_FORMAT;
    }

#ifndef FMOD_STATICFORPLUGINS
    if (sound->mSoundGroup && sound->mSoundGroup->mMaxAudible >= 0)
    {
        int numplaying;

        result = sound->mSoundGroup->getNumPlaying(&numplaying);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (numplaying >= sound->mSoundGroup->mMaxAudible)
        {
            switch (sound->mSoundGroup->mMaxAudibleBehavior)
            {
                case FMOD_SOUNDGROUP_BEHAVIOR_FAIL:
                {
                    return FMOD_ERR_MAXAUDIBLE;
                }
                case FMOD_SOUNDGROUP_BEHAVIOR_MUTE:
                {
                    mute = true;
                    break;
                }
                case FMOD_SOUNDGROUP_BEHAVIOR_STEALLOWEST:
                {
                    LinkedListNode *current;
                    float lowest = 9999.0f;

                    /*
                        Find least important sound and replace it.
                    */
                    current = mChannelUsedListHead.getNext();
                    while (current != &mChannelUsedListHead)
                    {
                        ChannelI *channel = (ChannelI *)current; 

                        if (channel->mRealChannel[0] && channel->mRealChannel[0]->mSound)
                        {
                            SoundGroupI *soundgroup = channel->mRealChannel[0]->mSound->mSoundGroup;

                            if (soundgroup == sound->mSoundGroup)
                            {
                                float audibility;

                                channel->getAudibility(&audibility);

                                if (audibility < lowest)
                                {
                                    chan = channel;
                                    lowest = audibility;
                                    channelid = (FMOD_CHANNELINDEX)chan->mIndex;
                                }
                            }
                        }

                        current = current->getNext();
                    }
                    break;
                }
                default:
                {
                    break;
                }
            };
        }
    }
#endif

    /*
        First find a channel
    */
    result = findChannel(channelid, sound, &chan);
    if (result != FMOD_OK)
    {
        if (channel)
        {
            *channel = 0;
        }
        return result;
    }

    result = chan->play(sound, paused, true, mute);
    if (result != FMOD_OK)
    {       
        if (channel)
        {
            *channel = 0;
        }
        chan->stopEx(CHANNELI_STOPFLAG_UPDATELIST | CHANNELI_STOPFLAG_RESETCALLBACKS | CHANNELI_STOPFLAG_RESETCHANNELGROUP | CHANNELI_STOPFLAG_UPDATESYNCPOINTS);
        return result;
    }

    result = chan->updatePosition();
    if (result != FMOD_OK)
    {
        if (channel)
        {
            *channel = 0;
        }
        return result;
    }

    if (channelid == FMOD_CHANNEL_REUSE && *channel)
    {
        chan->mHandleCurrent = chan->mHandleOriginal;
    }
    else
    {
        result = chan->referenceStamp(true);
        if (result != FMOD_OK)
        {
            if (channel)
            {
                *channel = 0;
            }
            return result;
        }
    }

    if (channel)
    {
        *channel = (ChannelI *)((FMOD_UINT_NATIVE)chan->mHandleCurrent);
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
// OFFSET: 0x008D47F0
FMOD_RESULT SystemI::playDSP(FMOD_CHANNELINDEX channelid, DSPI *dsp, bool paused, ChannelI **channel)
{
    FMOD_RESULT  result;
    ChannelI    *chan = 0;

    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (channel)
    {
        if (channelid == FMOD_CHANNEL_REUSE)
        {
            ChannelI::validate((Channel *)*channel, &chan);
        }
        if (channel)
        {
            *channel = 0;
        }
    }

    /*
        First find a channel
    */
    result = findChannel(channelid, dsp, &chan);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = chan->play(dsp, paused, true, false);
    if (result != FMOD_OK)
    {
        chan->stopEx(CHANNELI_STOPFLAG_UPDATELIST | CHANNELI_STOPFLAG_RESETCALLBACKS | CHANNELI_STOPFLAG_RESETCHANNELGROUP | CHANNELI_STOPFLAG_UPDATESYNCPOINTS);
        return result;
    }

    result = chan->updatePosition();
    if (result != FMOD_OK)
    {
        return result;
    }

    if (channelid == FMOD_CHANNEL_REUSE && *channel)
    {
        chan->mHandleCurrent = chan->mHandleOriginal;
    }
    else
    {
        result = chan->referenceStamp(true);
        if (result != FMOD_OK)
        {
            if (channel)
            {
                *channel = 0;
            }
            return result;
        }
    }

    if (channel)
    {
        *channel = (ChannelI *)((FMOD_UINT_NATIVE)chan->mHandleCurrent);
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
// OFFSET: 0x008D48E0
FMOD_RESULT SystemI::getChannel(int id, ChannelI **channel)
{
    FMOD_UINT_NATIVE handle;

    if (!channel)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (id < 0 || id >= mNumChannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    handle = (mIndex << SYSTEMID_SHIFT) | ((id << CHANINDEX_SHIFT) & (CHANINDEX_MASK << CHANINDEX_SHIFT));
    *channel = (ChannelI *)handle;

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
// OFFSET: 0x008D4920
FMOD_RESULT SystemI::getMasterChannelGroup(ChannelGroupI **channelgroup)
{
    if (!channelgroup)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mChannelGroup)
    {
        *channelgroup = 0;
        return FMOD_ERR_UNINITIALIZED;
    }

    *channelgroup = mChannelGroup;

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
FMOD_RESULT SystemI::getMasterSoundGroup(SoundGroupI **soundgroup)
{
    if (!soundgroup)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mSoundGroup)
    {
        *soundgroup = 0;
        return FMOD_ERR_UNINITIALIZED;
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
FMOD_RESULT SystemI::createReverb(ReverbI **reverb)
{
#ifdef FMOD_SUPPORT_MULTIREVERB
    FMOD_RESULT result;
    ReverbI *newreverb;
    
    /*
        initialise a new reverb instance
    */
    newreverb = (ReverbI*)FMOD_Object_Alloc(ReverbI);
    if (!newreverb)
    {
        return FMOD_ERR_MEMORY;
    }

    /*
        Initialise the reverb object.
    */
    result = newreverb->init(this, true, FMOD_REVERB_VIRTUAL);
    if (result != FMOD_OK)
    {
        FMOD_Memory_Free(newreverb);
        return result;
    }

    /*
        Add the reverb object to the system's channel reverb list
    */
    newreverb->addBefore(&mReverb3DHead);

    if (reverb)
    {
        *reverb = newreverb;
    }

    mReverbGlobal.setDisableIfNoEnvironment(false);

    mReverb3D.setDisableIfNoEnvironment(false);

    set3DReverbActive(true);

    return FMOD_OK;
#else
    return FMOD_ERR_UNSUPPORTED;
#endif
}


#ifdef FMOD_SUPPORT_MULTIREVERB
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
// OFFSET: 0x008D4950
unsigned int SystemI::count3DPhysicalReverbs()
{
    unsigned int count = 0;

    ReverbI* chanreverb_c = SAFE_CAST(ReverbI, mReverb3DHead.getNext());
    while (chanreverb_c != &mReverb3DHead)
    {
        if (chanreverb_c->getMode() == FMOD_REVERB_PHYSICAL)
        {
            ++count;
        }
        chanreverb_c = SAFE_CAST(ReverbI, chanreverb_c->getNext());
    }
    return count;
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
// OFFSET: 0x008D4980
unsigned int SystemI::count3DVirtualReverbs()
{
    unsigned int count = 0;

    ReverbI* chanreverb_c = SAFE_CAST(ReverbI, mReverb3DHead.getNext());
    while (chanreverb_c != &mReverb3DHead)
    {
        if (chanreverb_c->getMode() == FMOD_REVERB_VIRTUAL)
        {
            ++count;
        }
        chanreverb_c = SAFE_CAST(ReverbI, chanreverb_c->getNext());
    }
    return count;
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
// OFFSET: 0x008D49B0
FMOD_RESULT SystemI::set3DReverbActive(bool state)
{
    mReverb3DActive = state;

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
// OFFSET: 0x008D49D0
FMOD_RESULT SystemI::get3DReverbActive(bool *state)
{
    if (state)
    {
        *state = mReverb3DActive;
    }
    return FMOD_OK;
}

#endif

#ifndef FMOD_STATICFORPLUGINS

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
// OFFSET: 0x008D49F0
FMOD_RESULT SystemI::setReverbProperties(const FMOD_REVERB_PROPERTIES *prop, bool force_create)
{
#ifdef FMOD_SUPPORT_REVERB
    FMOD_RESULT result;

    if (!prop)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (prop->Instance < 0 || prop->Instance >= FMOD_REVERB_MAXINSTANCES)
    {
        return FMOD_ERR_REVERB_INSTANCE;
    }

    int instance = prop->Instance;

#ifdef FMOD_SUPPORT_SOFTWARE
    /*
        Create DSP if necessary
    */
    if ((!mReverbGlobal.mInstance[instance].mDSP) && (force_create || (prop->Environment!=-1)) && mSoftware)
    {
        /*
            Create
        */
        result = mReverbGlobal.createDSP(instance);
        if (result == FMOD_OK)
        {
            /*
                Link into network
            */
            if (mDSPChannelGroupTarget)
            {
                result = mDSPChannelGroupTarget->addInput(mReverbGlobal.mInstance[instance].mDSP);
                if (result != FMOD_OK)
                {
                    return result;
                }
                mReverbGlobal.setGain(1.0f);
            }
            else 
            {
                return FMOD_ERR_UNINITIALIZED;
            }

            /*
                Connect all channels to the reverb input
            */
            ChannelI *channel_c = SAFE_CAST(ChannelI, mChannelUsedListHead.getNext());

            while (channel_c != &mChannelUsedListHead)
            {
                FMOD_RESULT  result;
                FMOD_REVERB_CHANNELPROPERTIES cprops;
                
                FMOD_memset(&cprops, 0, sizeof(FMOD_REVERB_CHANNELPROPERTIES));

                cprops.Flags |= (instance == 0 ? FMOD_REVERB_CHANNELFLAGS_INSTANCE0 : 
                                instance == 1 ? FMOD_REVERB_CHANNELFLAGS_INSTANCE1 : 
                                instance == 2 ? FMOD_REVERB_CHANNELFLAGS_INSTANCE2 : 
                                instance == 3 ? FMOD_REVERB_CHANNELFLAGS_INSTANCE3 : 0);

                /*
                    Flush the channel properties through
                */
                result = channel_c->getReverbProperties(&cprops);
                if (result != FMOD_OK)
                {
                    return result;
                }
                result = channel_c->setReverbProperties( &cprops);
                if (result != FMOD_OK)
                {
                    return result;
                }
               
                channel_c = SAFE_CAST(ChannelI, channel_c->getNext());            
            }

            mReverbGlobal.mInstance[instance].mDSP->setActive(true);
        }
    }
#endif

    /*
        Set DSP (sw/hw handled by ReverbI)
    */
    result = mReverbGlobal.setProperties(prop);
    if (result != FMOD_OK)
    {
        return result;
    }

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
FMOD_RESULT SystemI::getReverbProperties(FMOD_REVERB_PROPERTIES *prop)
{
    if (!prop)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (prop->Instance < 0 || prop->Instance >= FMOD_REVERB_MAXINSTANCES)
    {
        return FMOD_ERR_REVERB_INSTANCE;
    }

    return mReverbGlobal.getProperties(prop);
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
#ifdef FMOD_SUPPORT_MULTIREVERB
// OFFSET: 0x008D4AD0
FMOD_RESULT SystemI::set3DReverbProperties(const FMOD_REVERB_PROPERTIES *prop, bool force_create)
{
    FMOD_RESULT result;
    int instance = 0;

    if (!prop)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    /*
        Create DSP if necessary
    */
    if ((!mReverb3D.mInstance[instance].mDSP) && (force_create || (prop->Environment!=-1)))
    {
        /*
            Create
        */
        result = mReverb3D.createDSP(instance);
        if (result != FMOD_OK)
        {
            return result;
        }

        /*
            Link into network
        */
        if (mDSPChannelGroupTarget)
        {
            result = mDSPChannelGroupTarget->addInput(mReverb3D.mInstance[instance].mDSP);
            if (result != FMOD_OK)
            {
                return result;
            }
            mReverb3D.setGain(1.0f);
        }
        else 
        {
            return FMOD_ERR_UNINITIALIZED;
        }

        /*
            Connect all channels to the reverb input
        */
        ChannelI* channel_c = SAFE_CAST(ChannelI, mChannelUsedListHead.getNext());
        while (channel_c != &mChannelUsedListHead)
        {           
            /*
                Flush the channel properties through
            */
            FMOD_REVERB_CHANNELPROPERTIES cprops;

            FMOD_memset(&cprops, 0, sizeof(FMOD_REVERB_CHANNELPROPERTIES));

            result = channel_c->getReverbProperties(&cprops);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = channel_c->setReverbProperties( &cprops);
            if (result != FMOD_OK)
            {
                return result;
            }
                       
            channel_c = SAFE_CAST(ChannelI, channel_c->getNext());
        }

        mReverb3D.mInstance[instance].mDSP->setActive(true);
    }

    /*
        Set DSP (sw/hw handled by ReverbI)
    */
    result = mReverb3D.setProperties(prop);
    if (result != FMOD_OK)
    {
        return result;
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

	[SEE_ALSO]
]
*/
#ifdef FMOD_SUPPORT_MULTIREVERB
FMOD_RESULT SystemI::get3DReverbProperties(FMOD_REVERB_PROPERTIES *prop)
{
    return mReverb3D.getProperties(prop);
}
#endif

#endif

/*
[
	[DESCRIPTION]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
// OFFSET: 0x008D4BB0
FMOD_RESULT SystemI::getDSPHead(DSPI **dsp)
{
#ifdef FMOD_SUPPORT_SOFTWARE
    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mDSPSoundCard)
    {
        return FMOD_ERR_INTERNAL;
    }

    *dsp = mDSPSoundCard;

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
FMOD_RESULT SystemI::addDSP(DSPI *dsp, DSPConnectionI **dspconnection)
{
    FMOD_RESULT result;
    DSPI *dsphead = 0;
 
    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
   
    result = getDSPHead(&dsphead);
    if (result != FMOD_OK)
    {
        return result;
    }
 
    result = dsphead->insertInputBetween(dsp, 0, false, dspconnection);
    if (result != FMOD_OK)
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
// OFFSET: 0x008D4BE0
FMOD_RESULT SystemI::lockDSP()
{
#ifdef FMOD_SUPPORT_SOFTWARE
    FMOD_OS_CriticalSection_Enter(mDSPLockCrit);

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
// OFFSET: 0x008D4C00
FMOD_RESULT SystemI::unlockDSP()
{
#ifdef FMOD_SUPPORT_SOFTWARE
    FMOD_OS_CriticalSection_Leave(mDSPLockCrit);

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
FMOD_RESULT SystemI::getDSPClock(unsigned int *hi, unsigned int *lo)
{
    if (hi)
    {
        *hi = mDSPClock.mHi;
    }
    if (lo)
    {
        *lo = mDSPClock.mLo;
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
// OFFSET: 0x008D4C20
FMOD_RESULT SystemI::getRecordNumDrivers(int *numdrivers)
{
#ifdef FMOD_SUPPORT_RECORDING
    FMOD_RESULT result = FMOD_OK;

	if (!mOutput)
	{
		return FMOD_ERR_UNINITIALIZED;
	}

    if (!numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    result = checkDriverList(false);
    CHECK_RESULT(result);

    if (mOutput->mDescription.record_getnumdrivers)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif

        return mOutput->mDescription.record_getnumdrivers(mOutput, numdrivers);
    }

    *numdrivers = 0;
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
FMOD_RESULT SystemI::getRecordDriverInfo(int id, char *name, int namelen, FMOD_GUID *guid)
{
#ifdef FMOD_SUPPORT_RECORDING
    FMOD_RESULT result      = FMOD_OK;
    int         numdrivers  = 0;

	if (!mOutput)
	{
		return FMOD_ERR_UNINITIALIZED;
	}
    
    result = getRecordNumDrivers(&numdrivers);
    CHECK_RESULT(result);

    if (id < 0 || id >= numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mOutput->mDescription.record_getdriverinfo)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif

        return mOutput->mDescription.record_getdriverinfo(mOutput, id, name, namelen, guid);
    }

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
FMOD_RESULT SystemI::getRecordDriverInfoW(int id, short *name, int namelen, FMOD_GUID *guid)
{
#ifdef FMOD_SUPPORT_RECORDING
    FMOD_RESULT result      = FMOD_OK;
    int         numdrivers  = 0;

	if (!mOutput)
	{
		return FMOD_ERR_UNINITIALIZED;
	}
    
    result = getRecordNumDrivers(&numdrivers);
    CHECK_RESULT(result);

    if (id < 0 || id >= numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mOutput->mDescription.record_getdriverinfow)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif

        return mOutput->mDescription.record_getdriverinfow(mOutput, id, name, namelen, guid);
    }

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
FMOD_RESULT SystemI::getRecordDriverCaps(int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency)
{
#ifdef FMOD_SUPPORT_RECORDING    
    FMOD_RESULT result          = FMOD_OK;
    FMOD_CAPS   lcaps           = FMOD_CAPS_NONE;
    int         lminfrequency   = 0;
    int         lmaxfrequency   = 0;
    int         numdrivers      = 0;
    
	if (!mOutput)
	{
		return FMOD_ERR_UNINITIALIZED;
	}
    
    result = getRecordNumDrivers(&numdrivers);
    CHECK_RESULT(result);
    
    if (id < 0 || id >= numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    if (mOutput->mDescription.record_getdrivercaps)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif
        result = mOutput->mDescription.record_getdrivercaps(mOutput, id, &lcaps, &lminfrequency, &lmaxfrequency);
        CHECK_RESULT(result);
    }
    
    if (caps)
    {
        *caps = lcaps;
    }
    if (minfrequency)
    {
        *minfrequency = lminfrequency;
    }
    if (maxfrequency)
    {
        *maxfrequency = lmaxfrequency;
    }
    
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
// OFFSET: 0x008D63D0
FMOD_RESULT SystemI::recordStart(int id, SoundI *sound, bool loop)
{
#ifdef FMOD_SUPPORT_RECORDING
    int                     numdrivers  = 0;
    FMOD_RESULT             result      = FMOD_OK;
    FMOD_RECORDING_INFO    *recordinfo  = NULL;

	if (!mInitialized)
	{
		return FMOD_ERR_UNINITIALIZED;
	}

	if (!sound || (sound->mMode & FMOD_CREATESTREAM))
	{
		return FMOD_ERR_INVALID_PARAM;
	}

    result = getRecordNumDrivers(&numdrivers);
    CHECK_RESULT(result);

    if (id < 0 || id >= numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

	recordStop(id);

    /*
        Create recording info for this record instance
    */
    recordinfo = FMOD_Object_Calloc(FMOD_RECORDING_INFO);
    if (!recordinfo)
    {
        return FMOD_ERR_MEMORY;
    }

    recordinfo->mRecordId       = id;
    recordinfo->mRecordDriver   = -1;
    recordinfo->mRecordLoop     = loop;
    recordinfo->mRecordSound    = sound;
    recordinfo->mRecordRate     = (int)sound->mDefaultFrequency;  /* This will get set to what the hardware can do in the driver */

    /*
        Multi-mic support requires GUIDs to be implemented for the platform
    */
    result = getRecordDriverInfo(id, NULL, 0, &recordinfo->mRecordGUID);
    CHECK_RESULT(result);

    if (mOutput->mDescription.record_start)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#else
        mOutput->readfrommixer = 0;
#endif

        result = mOutput->mDescription.record_start(mOutput, recordinfo, (FMOD_SOUND *)sound, loop);
        CHECK_RESULT(result);
    }

    /*
        Create a temp buffer to assist in moving data from hardware to sound sample
    */
    {
        unsigned int bufferlengthbytes = 0;

        SoundI::getBytesFromSamples(FMOD_RECORD_TEMPBUFFERSIZE, &bufferlengthbytes, sound->mChannels, FMOD_SOUND_FORMAT_PCMFLOAT);
        recordinfo->mRecordTempBufferLength = FMOD_RECORD_TEMPBUFFERSIZE;
        
        recordinfo->mRecordTempBuffer = (float*)FMOD_Memory_Calloc(bufferlengthbytes);
        if (!recordinfo->mRecordTempBuffer)
        {
            return FMOD_ERR_MEMORY;
        }
    }
    
    /*
        Setup a DSPResampler to handle record resampling if required
    */
    if (recordinfo->mRecordRate != sound->mDefaultFrequency)
    {
#ifdef FMOD_SUPPORT_SOFTWARE
        unsigned int             blocklength = 0;
        FMOD_DSP_DESCRIPTION_EX  description;

        /*
            Total latency for resampling is ~30ms, we need 3 blocks for the resampler,
            so each block will be ~10ms.
        */
        blocklength = (int)(0.01f * recordinfo->mRecordRate);
        blocklength /= 16;  /* Round down to nearest 16 bytes. */
        blocklength *= 16;
        
        FMOD_memset(&description, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));
        description.channels               = sound->mChannels;
        description.mFormat                = FMOD_SOUND_FORMAT_PCMFLOAT;    /* Record read callback will always give floats. */
        description.userdata               = mOutput;
        description.read                   = Output::recordResamplerReadCallback;
        description.mResamplerBlockLength  = blocklength;

        recordinfo->mRecordResamplerDSP = FMOD_Object_Calloc(DSPResampler);
        if (!recordinfo->mRecordResamplerDSP)
        {
            return FMOD_ERR_MEMORY;
        }

        recordinfo->mRecordResamplerDSP->mSystem = this;
        recordinfo->mRecordResamplerDSP->mBuffer = recordinfo->mRecordTempBuffer;
        recordinfo->mRecordResamplerDSP->alloc(&description);
        recordinfo->mRecordResamplerDSP->mTargetFrequency = (int)sound->mDefaultFrequency;
        recordinfo->mRecordResamplerDSP->setFrequency((float)recordinfo->mRecordRate);
        recordinfo->mRecordResamplerDSP->setFinished(false);
#else
        return FMOD_ERR_NEEDSSOFTWARE;
#endif
    }

    /*
        Make the new record device "live" by putting in the list
    */
    FMOD_OS_CriticalSection_Enter(mOutput->mRecordInfoCrit);
    {
        recordinfo->addAfter(&mOutput->mRecordInfoHead);
        mOutput->mRecordNumActive++;
    }
    FMOD_OS_CriticalSection_Leave(mOutput->mRecordInfoCrit);

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
// OFFSET: 0x008D4D20
FMOD_RESULT SystemI::recordStop(int id)
{
#ifdef FMOD_SUPPORT_RECORDING
    FMOD_RESULT             result              = FMOD_OK;
    int                     numdrivers          = 0;
    FMOD_RECORDING_INFO    *currentrecordinfo   = 0;

	if (!mInitialized)
	{
		return FMOD_ERR_UNINITIALIZED;
	}

    result = getRecordNumDrivers(&numdrivers);
    CHECK_RESULT(result);

    if (id < 0 || id >= numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = mOutput->recordGetInfo(id, &currentrecordinfo);
    CHECK_RESULT(result);

    if (currentrecordinfo)
    {
        result = mOutput->recordStop(currentrecordinfo);
        CHECK_RESULT(result);
    }

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
// OFFSET: 0x008D4DE0
FMOD_RESULT SystemI::getRecordPosition(int id, unsigned int *position)
{
#ifdef FMOD_SUPPORT_RECORDING
    FMOD_RESULT             result      = FMOD_OK;
    FMOD_RECORDING_INFO    *info        = NULL;
    int                     numdrivers  = 0;

    if (!mInitialized)
	{
		return FMOD_ERR_UNINITIALIZED;
	}

    if (!position)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = getRecordNumDrivers(&numdrivers);
    CHECK_RESULT(result);

    if (id < 0 || id >= numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mOutput->recordGetInfo(id, &info);
    if (info)
    {
        *position = info->mRecordOffset;
    }
    else
    {
        *position = 0;
    }

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
// OFFSET: 0x008D4E10
FMOD_RESULT SystemI::isRecording(int id, bool *recording)
{
#ifdef FMOD_SUPPORT_RECORDING
    FMOD_RESULT             result      = FMOD_OK;
    FMOD_RECORDING_INFO    *info        = NULL;
    int                     numdrivers  = 0;

    if (!mInitialized)
	{
		return FMOD_ERR_UNINITIALIZED;
	}
    
    if (!recording)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = getRecordNumDrivers(&numdrivers);
    CHECK_RESULT(result);

    if (id < 0 || id >= numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mOutput->recordGetInfo(id, &info);
    if (info)
    {
        *recording = true;
    }
    else
    {
        *recording = false;
    }

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
FMOD_RESULT SystemI::createGeometry(int maxNumPolygons, int maxNumVertices, GeometryI **geometry)
{
#ifdef FMOD_SUPPORT_GEOMETRY
	FMOD_RESULT result;

    if (!geometry)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (maxNumPolygons <= 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (maxNumVertices <= 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (geometry == 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

 	*geometry = (GeometryI *)FMOD_Memory_Alloc(sizeof (GeometryI));
    if (!*geometry)
    {
        return FMOD_ERR_MEMORY;	
    }
    
    new (*geometry) GeometryI(&mGeometryMgr);
 	
	result = (*geometry)->alloc(maxNumPolygons, maxNumVertices);
	if (result != FMOD_OK)
    {
		return result;
    }
 
 	if (mGeometryList)
    {
 		(*geometry)->addBefore(mGeometryList);
    }

 	mGeometryList = *geometry;
 
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
FMOD_RESULT SystemI::setGeometrySettings(float maxWorldSize)
{
#ifdef FMOD_SUPPORT_GEOMETRY
	FMOD_RESULT result;

	if (maxWorldSize <= 0.0f)
    {
		return FMOD_ERR_INVALID_PARAM;
    }

 	result = mGeometryMgr.setWorldSize(maxWorldSize);
	if (result != FMOD_OK)
    {
		return result;
    }
	
	if (mGeometryList)
	{
		// we have to remove everything from the tree and re-add
		// it when the world size changes
		GeometryI* current = mGeometryList;
		do
		{ 
			current->removeFromTree();
			current = SAFE_CAST(GeometryI, current->getNext());
		}
		while (current != mGeometryList);

		current = mGeometryList;
		do
		{ 
			result = current->setWorldSize(maxWorldSize); // re-adds automatically
			if (result != FMOD_OK)
            {
				return result;
            }

			current = SAFE_CAST(GeometryI, current->getNext());
		}
		while (current != mGeometryList);
	}

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
FMOD_RESULT SystemI::getGeometrySettings(float *maxWorldSize)
{
#ifdef FMOD_SUPPORT_GEOMETRY
	if (!maxWorldSize)
    {
		return FMOD_ERR_INVALID_PARAM;
    }

 	*maxWorldSize = mGeometryMgr.getWorldSize(); 
	
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
FMOD_RESULT SystemI::loadGeometry(const void *data, int dataSize, GeometryI **geometry)
{ 
#ifdef FMOD_SUPPORT_GEOMETRY
	FMOD_RESULT result;

    if (!data)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (!geometry)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    *geometry = (GeometryI *)FMOD_Memory_Alloc(sizeof (GeometryI));
    if (!*geometry)
    {
        return FMOD_ERR_MEMORY;	
    }
    
    new (*geometry) GeometryI(&mGeometryMgr);
 	
	result = (*geometry)->load(data, dataSize);
	if (result != FMOD_OK)
    {
		return result;
    }
 
 	if (mGeometryList)
    {
 		(*geometry)->addBefore(mGeometryList);
    }

 	mGeometryList = *geometry;
    
    return FMOD_OK;

#else
	return FMOD_ERR_UNSUPPORTED;
#endif
}

FMOD_RESULT SystemI::getGeometryOcclusion(const FMOD_VECTOR *listener,
                                          const FMOD_VECTOR *source,
                                          float *direct, float *reverb)
{
    if(!source || !listener)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    float direct_occlusion = 0.0f, reverb_occlusion = 0.0f;

#ifdef FMOD_SUPPORT_GEOMETRY
    CHECK_RESULT(mGeometryMgr.lineTestAll(listener, source,
                                          &direct_occlusion, &reverb_occlusion));
#endif

    if(direct)
    {
        *direct = direct_occlusion;
    }

    if(reverb)
    {
        *reverb = reverb_occlusion;
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
FMOD_RESULT SystemI::setNetworkProxy(const char *proxy)
{
#ifdef FMOD_SUPPORT_NET
    return FMOD_Net_SetProxy(proxy);
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
FMOD_RESULT SystemI::getNetworkProxy(char *proxy, int proxylen)
{
#ifdef FMOD_SUPPORT_NET
    return FMOD_Net_GetProxy(proxy, proxylen);
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
FMOD_RESULT SystemI::setNetworkTimeout(int timeout)
{
#ifdef FMOD_SUPPORT_NET
    return FMOD_Net_SetTimeout(timeout);
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
FMOD_RESULT SystemI::getNetworkTimeout(int *timeout)
{
#ifdef FMOD_SUPPORT_NET
    return FMOD_Net_GetTimeout(timeout);
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
FMOD_RESULT SystemI::setStreamBufferSize(unsigned int filebuffersize, FMOD_TIMEUNIT filebuffersizetype)
{
    if (filebuffersize <= 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (filebuffersizetype != FMOD_TIMEUNIT_MS && 
        filebuffersizetype != FMOD_TIMEUNIT_PCM && 
        filebuffersizetype != FMOD_TIMEUNIT_PCMBYTES && 
        filebuffersizetype != FMOD_TIMEUNIT_RAWBYTES)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mStreamFileBufferSize       = filebuffersize;
    mStreamFileBufferSizeType   = filebuffersizetype;

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
FMOD_RESULT SystemI::getStreamBufferSize(unsigned int *filebuffersize, FMOD_TIMEUNIT *filebuffersizetype)
{
    if (filebuffersize)
    {
        *filebuffersize = mStreamFileBufferSize;
    }
    if (filebuffersizetype)
    {
        *filebuffersizetype = mStreamFileBufferSizeType;
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
FMOD_RESULT SystemI::setUserData(void *userdata)
{
    mUserData = userdata;
    
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
FMOD_RESULT SystemI::getUserData(void **userdata)
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
FMOD_RESULT SystemI::getSoundList(SoundI **sound)
{
    if (!sound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *sound = &mSoundListHead;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Stops all playing channels using this sound.

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
// OFFSET: 0x008D4E50
FMOD_RESULT SystemI::stopSound(SoundI *sound)
{
    ChannelI *current;
    bool      streamstopped = false;

#ifdef FMOD_SUPPORT_RECORDING
    /*
        If this sound is currently recording, stop the recording
    */    
    if (mOutput && mOutput->mRecordNumActive)
    {
        FMOD_RECORDING_INFO *currentrecordinfo = NULL;

        currentrecordinfo = SAFE_CAST(FMOD_RECORDING_INFO, mOutput->mRecordInfoHead.getNext());
        while (currentrecordinfo != &mOutput->mRecordInfoHead)
        {
            FMOD_RECORDING_INFO *next = SAFE_CAST(FMOD_RECORDING_INFO, currentrecordinfo->getNext());
            
            if (currentrecordinfo->mRecordSound == sound)
            {
                mOutput->recordStop(currentrecordinfo);
                break;
            }
            currentrecordinfo = next;
        }
    }
#endif

#ifdef FMOD_SUPPORT_STREAMING
    if (sound->isStream())
    {
        FMOD_OS_CriticalSection_Enter(mStreamListCrit);

        if (!mStreamListChannelHead.isEmpty())
        {
            LinkedListNode *current;
            Stream *stream = SAFE_CAST(Stream, sound);
            
            current = mStreamListChannelHead.getNext();
            while (current != &mStreamListChannelHead)
            {
                ChannelStream  *channelstream = (ChannelStream *)current->getData();
                Stream         *currentstream = SAFE_CAST(Stream, channelstream->mSound);
                LinkedListNode *next = current->getNext();
                
                streamstopped = false;
                
                /*
                    Might not have been played yet.
                */
                if (!currentstream)
                {
                    current = next;
                    continue;
                }
        
                /*
                    If this channel is playing this stream, stop the channel.
                */
                if (currentstream == stream)
                {
                    FMOD_OS_CriticalSection_Leave(mStreamListCrit);
                                
                    channelstream->mParent->stop();
                    streamstopped = true;
                    
                    FMOD_OS_CriticalSection_Enter(mStreamListCrit);
                }

                /*
                    If this channel is playing a sentence and it is the sound that is currently playing, stop the channel.
                */
#ifdef FMOD_SUPPORT_SENTENCING
                else if (currentstream->mSubSoundList)
                {
                    if (currentstream->mSubSound[currentstream->mSubSoundList[channelstream->mSubSoundListCurrent].mIndex] == sound)
                    {
                        FMOD_OS_CriticalSection_Leave(mStreamListCrit);
                                    
                        channelstream->mParent->stop();
                        streamstopped = true;
                        
                        FMOD_OS_CriticalSection_Enter(mStreamListCrit);                        
                    }
                    else
                    {
                        int count;

                        for (count = 0; count < currentstream->mNumSubSounds; count++)
                        {
                            if (currentstream->mSubSound[count] == sound)
                            {
                                if (!(currentstream->mCodec && currentstream->mCodec->mFlags & FMOD_CODEC_USERLENGTH))
                                {
                                    currentstream->mLength -= sound->mLength;
                                }
                                currentstream->mSubSound[count] = 0;
                                break;              /* found it, so break out of subsound search. */
                            }
                        }
                    }
                }
#endif

                /*
                    If this channel is playing a substream, stop the channel.
                */
                else if (currentstream->mSubSound)
                {
                    int count;

                    for (count = 0; count < currentstream->mNumSubSounds; count++)
                    {
                        if (currentstream->mSubSound[count] == sound)
                        {
                            FMOD_OS_CriticalSection_Leave(mStreamListCrit);
                                        
                            channelstream->mParent->stop();
                            streamstopped = true;
                            
                            FMOD_OS_CriticalSection_Enter(mStreamListCrit);                            
                            break;              /* found it, so break out of subsound search. */
                        }
                    }
                }

                /* 
                    If this channel is using this stream's sample:  Stop that channel.
                */
                else if (currentstream->mSample == stream->mSample)
                {
                    FMOD_OS_CriticalSection_Leave(mStreamListCrit);
                    
                    channelstream->mParent->stop();
                    streamstopped = true;
                    
                    FMOD_OS_CriticalSection_Enter(mStreamListCrit);
                }

                current = next;
            }
        }
        
        FMOD_OS_CriticalSection_Leave(mStreamListCrit);
    }
#endif

    /*
        Note that release/stopSound() can be called from the event system, on sounds that have never played before.  
        So don't let these sounds execute an async channel loop, it will crash.  The 'Played' flag is never set on these types of sound, they're just temporary storage.
    */
    if (sound->mFlags & FMOD_SOUND_FLAG_PLAYED && !streamstopped)
    {
        if (sound->mNumAudible)                 /* Channel::stop hasn't been called on all instances of this sound yet.  Let's do it for them. */
        {
            if (sound->mMode & FMOD_SOFTWARE)   /* This is slow, but they didn't stop the sound.  Make sure the mixer is not continuing to do things with the channel after we stop. */
            {
                lockDSP();
            }

            current = SAFE_CAST(ChannelI, mChannelUsedListHead.getNext());
            while (current != &mChannelUsedListHead)
            {
                ChannelI *next = SAFE_CAST(ChannelI, current->getNext());

                if (current->mRealChannel[0])
                {
                    SoundI *channelsound = 0;

                    current->getCurrentSound(&channelsound);
                    if (channelsound == sound)
                    {
                        /*
                            Same call as current->stop, except we dont want end callbacks when releasing.
                        */
                        current->stopEx(CHANNELI_STOPFLAG_REFSTAMP | CHANNELI_STOPFLAG_UPDATELIST | CHANNELI_STOPFLAG_RESETCALLBACKS | CHANNELI_STOPFLAG_RESETCHANNELGROUP | CHANNELI_STOPFLAG_UPDATESYNCPOINTS);
                    }
                }

                current = next;
            }

            if (sound->mMode & FMOD_SOFTWARE)
            {
                unlockDSP();
            }

        }
        else if (sound->mMode & FMOD_SOFTWARE)                    /* Apparently all instances of this voice have been stopped, BUT!  Are they still active in the mixer? */
        {
            FMOD_OPENSTATE openstate;

            sound->getOpenState(&openstate, 0,0);
            
            if (openstate == FMOD_OPENSTATE_STREAMING)
            {
                lockDSP();      /* Just stall for 1 DSP tick. */
                unlockDSP();
            }   
        }

        if (!sound->isStream())
        {
            sound->mFlags &= ~FMOD_SOUND_FLAG_PLAYED;   /* For sample banks, avoid constant dsp locks over and over again for nothing. */
        }
    }

    #ifdef PLATFORM_PS2
    update();
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
// OFFSET: 0x008D5020
FMOD_RESULT SystemI::stopDSP(DSPI *dsp)
{   
    ChannelI *current;

    current = SAFE_CAST(ChannelI, mChannelUsedListHead.getNext());
    while (current != &mChannelUsedListHead)
    {
        ChannelI *next = SAFE_CAST(ChannelI, current->getNext());
        DSPI *channeldsp;

        current->getCurrentDSP(&channeldsp);
        if (channeldsp == dsp)
        {
            current->stop();
        }

        current = next;
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    On success, FMOD_OK is returned.
    On failure, an error code is returned.

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
// OFFSET: 0x008D5060
FMOD_RESULT SystemI::getListenerObject(int listener, Listener **listenerobject)
{   
    if (!listenerobject || (listener < 0) || (listener >= mNumListeners))
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *listenerobject = &mListener[listener];

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    On success, FMOD_OK is returned.
    On failure, an error code is returned.

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT SystemI::setGlobalUserCallbacks(FMOD_FILE_OPENCALLBACK open, FMOD_FILE_CLOSECALLBACK close, FMOD_FILE_READCALLBACK read, FMOD_FILE_SEEKCALLBACK seek)
{
    if (!open || !close || !read || !seek)
    {
        open = 0;
        close = 0;
        read = 0;
        seek = 0;

        mUsesUserCallbacks = false;
    }
    else
    {
        mUsesUserCallbacks = true;
    }

    mOpenCallback  = open;
    mCloseCallback = close;
    mReadCallback  = read;
    mSeekCallback  = seek;

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
// OFFSET: 0x008D50A0
FMOD_RESULT SystemI::flushDSPConnectionRequests(bool calledfrommainthread)
{
    FMOD_OS_CriticalSection_Enter(mDSPConnectionCrit);
    {
        DSPConnectionRequest *request;

        if (mConnectionRequestFlushing)
        {
            FMOD_OS_CriticalSection_Leave(mDSPConnectionCrit);
            return FMOD_OK;             /* addinput and disconnect can trigger this function to recursively execute. */
        }

        if (!mConnectionRequestUsedHead.isEmpty())
        {               
            mConnectionRequestFlushing = true;

            if (calledfrommainthread)
            {
                FMOD_OS_CriticalSection_Enter(mDSPCrit);
            }

            request = (DSPConnectionRequest *)mConnectionRequestUsedHead.getNext();
            while (request != &mConnectionRequestUsedHead)
            {   
                switch (request->mRequest)
                {
                    case DSPCONNECTION_REQUEST_ADDINPUT: 
                    {
                        request->mThis->addInputInternal(request->mTarget, false, request->mConnection, 0, false);
                        break;
                    }
                    case DSPCONNECTION_REQUEST_ADDINPUT_ERRCHECK: 
                    {
                        request->mThis->addInputInternal(request->mTarget, true, request->mConnection, 0, false);
                        break;
                    }
                    case DSPCONNECTION_REQUEST_DISCONNECTFROM: 
                    {
                        request->mThis->disconnectFromInternal(request->mTarget, request->mConnection, false);
                        if (request->mTarget)
                        {
                            request->mTarget->mFlags &= ~FMOD_DSP_FLAG_QUEUEDFORDISCONNECT;
                        }
                        else
                        {
                            request->mThis->mFlags &= ~FMOD_DSP_FLAG_QUEUEDFORDISCONNECT;
                        }
                        break;
                    }
                    case DSPCONNECTION_REQUEST_DISCONNECTALLINPUTS:
                    {
                        request->mThis->disconnectAllInternal(true, false, false);
                        break;
                    }
                    case DSPCONNECTION_REQUEST_DISCONNECTALLOUTPUTS:
                    {
                        request->mThis->disconnectAllInternal(false, true, false);
                        request->mThis->mFlags &= ~FMOD_DSP_FLAG_QUEUEDFORDISCONNECT;
                        break;
                    }
                    case DSPCONNECTION_REQUEST_DISCONNECTALL:
                    {
                        request->mThis->disconnectAllInternal(true, true, false);
                        request->mThis->mFlags &= ~FMOD_DSP_FLAG_QUEUEDFORDISCONNECT;
                        break;
                    }
                    case DSPCONNECTION_REQUEST_INSERTINBETWEEN:
                    case DSPCONNECTION_REQUEST_INSERTINBETWEEN_SEARCH:
                    {
                        request->mThis->insertInputBetweenInternal(request->mTarget, request->mInputIndex, request->mRequest == DSPCONNECTION_REQUEST_INSERTINBETWEEN_SEARCH ? true : false, request->mConnection, false);
                        request->mTarget->mFlags |= FMOD_DSP_FLAG_USEDADDDSP;
                        request->mTarget->reset();
                        request->mTarget->setActive(true);
                        break;
                    }
                    case DSPCONNECTION_REQUEST_REVERBUPDATEPARAMETERS:
                    {
    #ifdef FMOD_SUPPORT_SFXREVERB 
                        DSPI *reverb = (DSPI *)request->mThis;
                        if (reverb->mDescription.update)
                        {                    
                            reverb->mDescription.update((FMOD_DSP_STATE *)reverb);
                        }
    #endif
                        break;
                    }
    #ifdef FMOD_DSP_NONBLOCKING_REMOVE_RELEASE
                    case DSPCONNECTION_REQUEST_REMOVE:
                    {
                        request->mThis->removeInternal(false);
                        break;
                    }
                    case DSPCONNECTION_REQUEST_RELEASE:
                    {
                        request->mThis->releaseInternal(request->mTarget ? true : false, false);
                        break;
                    }
    #endif
                }        

                request->removeNode();
                request->addBefore(&mConnectionRequestFreeHead);   /* Put back on free list. */
    
                request = (DSPConnectionRequest *)mConnectionRequestUsedHead.getNext();
            }
            mConnectionRequestFlushing = false;

            if (calledfrommainthread)
            {
                FMOD_OS_CriticalSection_Leave(mDSPCrit);
            }
        }
    }
    FMOD_OS_CriticalSection_Leave(mDSPConnectionCrit);

    return FMOD_OK;
}
#endif


#ifndef FMOD_STATICFORPLUGINS
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
FMOD_RESULT SystemI::getGlobals(Global **global)
{
    if (!global)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *global = gGlobal;

    return FMOD_OK;
}

FMOD_RESULT F_API System_SetDebugLevel(unsigned int level)
{
#ifdef FMOD_DEBUG
    gGlobal->gDebugLevel = (FMOD_DEBUGLEVEL)level;
#endif
    return FMOD_OK;
}

unsigned int F_API System_GetDebugLevel()
{
#ifdef FMOD_DEBUG
    return (unsigned int )gGlobal->gDebugLevel;
#else
    return 0;
#endif
}

FMOD_RESULT F_API System_SetDebugMode(unsigned int mode)
{
#ifdef FMOD_DEBUG
    gGlobal->gDebugMode = (FMOD_DEBUGMODE)mode;
#endif
    return FMOD_OK;
}
#endif

#ifdef FMOD_SUPPORT_MULTIREVERB
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
// OFFSET: 0x008D65E0
FMOD_RESULT SystemI::update3DReverbs()
{
    float        t = 0.0f;
    bool         reverb3d_active;
    ReverbI     *reverb_c;
      
    //
    // Standardised properties are all in floating point form for correct arithmetic
    //
    FMOD_REVERB_STDPROPERTIES stdprops;
    FMOD_memset(&stdprops, 0, sizeof(FMOD_REVERB_STDPROPERTIES));

    //
    // For all General mode reverbs, set the level of the listener gain
    //
    for (reverb_c = SAFE_CAST(ReverbI, mReverb3DHead.getNext()); reverb_c != &mReverb3DHead; reverb_c = SAFE_CAST(ReverbI, reverb_c->getNext()))
    {
        float distance_gain, distance_coeff;
        bool active;

        reverb_c->getActive(&active);

        if (active)
        {
            /*
                Find listener-reverb distance gain
            */
            reverb_c->calculateDistanceGain(&mListener[0].mPosition, &distance_gain, &distance_coeff);

#ifdef FMOD_SUPPORT_GEOMETRY
            /*
                Find reverb occlusion gain of the listener-reverb path
            */
            if (distance_gain > 0.0f)
            {
                float direct_occlusion, reverb_occlusion;
                FMOD_RESULT  result;
                FMOD_VECTOR  pos;

                reverb_c->get3DAttributes(&pos, 0, 0);

                result = mGeometryMgr.lineTestAll(&mListener[0].mPosition, &pos, &direct_occlusion, &reverb_occlusion); 
                if (result != FMOD_OK)
                {
                    return result;
                }
                distance_gain *= (1.0f - reverb_occlusion);
                distance_coeff *= (1.0f - reverb_occlusion);
            }
#endif

            /*
               Reverb modules - ones which have DSP : set the presence gain
            */
            if (reverb_c->getMode() == FMOD_REVERB_PHYSICAL)
            {
                if (distance_gain != reverb_c->getGain())
                {
                    reverb_c->setGain(distance_gain);
                }
            }
            //
            // Virtual reverbs - ones without DSP and just have properties and geometry : add to running average
            //
            else if (reverb_c->getMode() == FMOD_REVERB_VIRTUAL)
            {
                if (distance_coeff >= 0.001f)
                {
                    FMOD_REVERB_PROPERTIES reverbprops;

                    FMOD_memset(&reverbprops, 0, sizeof(FMOD_REVERB_PROPERTIES));

                    reverb_c->getProperties(&reverbprops);

                    ReverbI::sumProps(&stdprops, &reverbprops, distance_coeff);
                    t += distance_coeff;
                }
            }
        }
    }

    get3DReverbActive(&reverb3d_active);

    if (reverb3d_active)
    {
        FMOD_REVERB_PROPERTIES qprops;

        /*
            Complement with ambient setting if there's not enough reverb presence
        */
        if (t < 1.0f)
        {
            FMOD_REVERB_PROPERTIES ambience;

            getReverbAmbientProperties(&ambience);

            if (ambience.Environment == -1)
            {
                ambience.Room = -10000; 
                ReverbI::sumRoomProps(&stdprops, &ambience, 1.0f - t);
            }
            else
            {
                ReverbI::sumProps(&stdprops, &ambience, 1.0f - t);
            }

            t = 1.0f;
        }

        /*
            Normalise the properties with the accumulated t value
        */
        ReverbI::factorProps(&qprops, &stdprops, 1.0f/t); 
        set3DReverbProperties(&qprops, true);
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
FMOD_RESULT SystemI::setReverbAmbientProperties(FMOD_REVERB_PROPERTIES* prop)
{
    if (!prop)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (prop->Environment != -1)
    {
        set3DReverbActive(true);
    }

    FMOD_memcpy(&mReverb3DAmbientProperties, prop, sizeof(FMOD_REVERB_PROPERTIES));

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
FMOD_RESULT SystemI::getReverbAmbientProperties(FMOD_REVERB_PROPERTIES* prop)
{
    if (!prop)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_memcpy(prop, &mReverb3DAmbientProperties, sizeof(FMOD_REVERB_PROPERTIES));

    return FMOD_OK;
}
#endif

unsigned int F_API System_GetDebugMode()
{
#ifdef FMOD_DEBUG
    return (unsigned int )gGlobal->gDebugMode;
#else
    return 0;
#endif
}


/*
[API]
[
	[DESCRIPTION]
    Callback for system events.

	[PARAMETERS]
	'system'        Pointer to a system handle.   Note this could be null if FMOD_SYSTEM_CALLBACKTYPE_THREADCREATED is triggered from the EventSystem.
    'type'          The type of callback.  Refer to FMOD_SYSTEM_CALLBACKTYPE.
    'commanddata1'  The first callback type specific data generated by the callback.  See remarks for meaning.
    'commanddata2'  The second callback type specific data generated by the callback.  See remarks for meaning.

	[RETURN_VALUE]

	[REMARKS]
    <u>C++ Users</u>.  Cast <b>FMOD_SYSTEM *</b> to <b>FMOD::System *</b> inside the callback and use as normal.<br>
    <br>
    <u>'commanddata1' and 'commanddata2' meanings.</u><br>
    These 2 values are set by the callback depending on what is happening in the callback and the type of callback.<br>
    <li><b>FMOD_SYSTEM_CALLBACKTYPE_DEVICELISTCHANGED</b><br>
        <i>commanddata1</i>: Always 0.<br>
        <i>commanddata2</i>: Always 0.<br>
    <br>
    <li><b>FMOD_SYSTEM_CALLBACKTYPE_MEMORYALLOCATIONFAILED</b><br>
        <i>commanddata1</i>: A string (char*) which represents the file and line number of the allocation inside FMOD.<br>
        <i>commanddata2</i>: The size (int) of the requested allocation.<br>
    <br>
    <li><b>FMOD_SYSTEM_CALLBACKTYPE_THREADCREATED</b><br>
        <i>commanddata1</i>: The handle of the created thread.  See notes below for thread handle types<br>
        <i>commanddata2</i>: A string (char*) which represents the name of the thread.<br>
    <br>
    <li><b>FMOD_SYSTEM_CALLBACKTYPE_BADDSPCONNECTION</b><br>
        <i>commanddata1</i>: Pointer to a FMOD::DSP object that was the target of the DSP connection.<br>
        <i>commanddata2</i>: Pointer to a FMOD::DSP object that was the source of the DSP connection.<br>
    <br>
    <li><b>FMOD_SYSTEM_CALLBACKTYPE_BADDSPLEVEL</b><br>
        <i>commanddata1</i>: Pointer to a FMOD::DSP object that was trying to exceed the DSP tree level maximum.<br>
        <i>commanddata2</i>: 0.<br>
    <br>
    
    <b>Note!</b>  For FMOD_SYSTEM_CALLBACKTYPE_DEVICELISTCHANGED, the user must call System::update for the callback to trigger! See FMOD_SYSTEM_CALLBACKTYPE for details.<br>
    <br>
    <b>Note!</b>  For FMOD_SYSTEM_CALLBACKTYPE_THREADCREATED, the handle that is returned (via commanddata1) is different on each platform.  The types to cast to are as follows.<br>
    <li>iPhone, Linux, Mac, Solaris : pthread_t
    <li>PS2 : int
    <li>PS3 : sys_ppu_thread_t
    <li>PSP : PSPThreadWrapper.   This is a custom struct you can define as typedef struct PSPThreadWrapper { SceUID    id; int     (*func)(void *param); void     *param; };
    <li>Wii : OSThread
    <li>Win32, Win64, Xbox360 : HANDLE
    <br>
    <br>
    <br>
    Here is an example of a system callback.<br>
    <br>
    <PRE>
    FMOD_RESULT F_CALLBACK systemcallback(FMOD_SYSTEM *system, FMOD_SYSTEM_CALLBACKTYPE type, void *commanddata1, void *commanddata2)
    {
    <ul>FMOD::System *sys = (FMOD::System *)system;
        <br>
        switch (type)
        {
        <ul>case FMOD_SYSTEM_CALLBACKTYPE_DEVICELISTCHANGED:
            {
            <ul>int numdrivers;
                <br>
                printf("NOTE : FMOD_SYSTEM_CALLBACKTYPE_DEVICELISTCHANGED occured.\n");
                <br>
                sys->getNumDrivers(&numdrivers);
                <br>
                printf("Numdevices = %d\n", numdrivers);
                break;
            </ul>}
            case FMOD_SYSTEM_CALLBACKTYPE_MEMORYALLOCATIONFAILED:
            {
            <ul>printf("ERROR : FMOD_SYSTEM_CALLBACKTYPE_MEMORYALLOCATIONFAILED occured.\n");
                printf("%s.\n", commanddata1);
                printf("%d bytes.\n", commanddata2);
                break;
            </ul>}
            case FMOD_SYSTEM_CALLBACKTYPE_THREADCREATED:
            {
            <ul>printf("NOTE : FMOD_SYSTEM_CALLBACKTYPE_THREADCREATED occured.\n");
                printf("Thread ID = %d\n", (int)commanddata1);
                printf("Thread Name = %s\n", (char *)commanddata2);
                break;
            </ul>}
            case FMOD_SYSTEM_CALLBACKTYPE_BADDSPCONNECTION:
            {
            <ul>FMOD::DSP *source = (FMOD::DSP *)commanddata1;
                FMOD::DSP *dest = (FMOD::DSP *)commanddata2;
                <br>
                printf("ERROR : FMOD_SYSTEM_CALLBACKTYPE_BADDSPCONNECTION occured.\n");
                if (source)
                {
                <ul>char name[256];
                    source->getInfo(name, 0,0,0,0);
                    printf("SOURCE = %s\n", name);
                </ul>}
                if (dest)
                {
                <ul>char name[256];
                    dest->getInfo(name, 0,0,0,0);
                    printf("DEST = %s\n", name);
                </ul>}
                break;
            </ul>}
            case FMOD_SYSTEM_CALLBACKTYPE_BADDSPLEVEL:
            {
            <ul>FMOD::DSP *source = (FMOD::DSP *)commanddata1;
                <br>
                printf("ERROR : FMOD_SYSTEM_CALLBACKTYPE_BADDSPLEVEL occured.\n");
                if (source)
                {
                <ul>char name[256];
                    source->getInfo(name, 0,0,0,0);
                    printf("SOURCE = %s\n", name);
                </ul>}
                break;
            </ul>}
        </ul>}
        <br>
        return FMOD_OK;    
    </ul>}
    </PRE>
    <br>
    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox, Xbox360, PlayStation 2, GameCube, PlayStation Portable, PlayStation 3, Wii, Solaris

	[SEE_ALSO]
    System::setCallback
    FMOD_SYSTEM_CALLBACKTYPE
    System::update
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_SYSTEM_CALLBACK(FMOD_SYSTEM *system, FMOD_SYSTEM_CALLBACKTYPE type, void* commanddata1, void* commanddata2)
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
FMOD_RESULT SystemI::getMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, FMOD_MEMORY_USAGE_DETAILS *memoryused_details)
{
#ifdef FMOD_SUPPORT_MEMORYTRACKER
    GETMEMORYINFO_IMPL
#else
    return FMOD_ERR_UNIMPLEMENTED;
#endif
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
FMOD_RESULT SystemI::getMemoryUsedImpl(MemoryTracker *tracker)
{
    FMOD_RESULT     result = FMOD_OK;

#ifndef FMOD_STATICFORPLUGINS
    LinkedListNode *node;
    int             count;

    tracker->add(false, FMOD_MEMBITS_SYSTEM, sizeof(*this));

    for (node = mSoundListHead.getNext(); node != &mSoundListHead; node = node->getNext())
    {
        SoundI *soundi = (SoundI *)node;
        CHECK_RESULT(soundi->getMemoryUsed(tracker));
    }

    if (gSoundListCrit)
    {
        tracker->add(false, FMOD_MEMBITS_SYSTEM, gSizeofCriticalSection);
    }

    if (mChannel)
    {
        for (count=0; count < mNumChannels; count++)
        {
            CHECK_RESULT(mChannel[count].getMemoryUsed(tracker));
        }
    }

    if (mOutput && mOutput->mDescription.getmemoryused)
    {
        CHECK_RESULT(mOutput->mDescription.getmemoryused(mOutput, tracker));
    }

    if (mEmulated)
    {
        CHECK_RESULT(mEmulated->getMemoryUsed(tracker));
    }

#ifdef FMOD_SUPPORT_SOFTWARE
    if (mDSPTempBuffMem)
    {
        int bufferchannels;
        result = getSoftwareFormat(0, 0, &bufferchannels, 0, 0, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        tracker->add(false, FMOD_MEMBITS_SYSTEM, (mDSPBlockSize * (bufferchannels < mMaxInputChannels ? mMaxInputChannels : bufferchannels) * sizeof(float)) + 16);
    }

    for (count = 0; count < FMOD_DSP_MAXTREEDEPTH; count++)
    {
        if (mDSPMixBuff[count])
        {
            tracker->add(false, FMOD_MEMBITS_SYSTEM, (mDSPBlockSize * (mMaxOutputChannels < mMaxInputChannels ? mMaxInputChannels : mMaxOutputChannels) * sizeof(float)) + 16);
        }
    }

    CHECK_RESULT(mDSPConnectionPool.getMemoryUsed(tracker));

    if (mDSPCrit)
    {
        tracker->add(false, FMOD_MEMBITS_SYSTEM, gSizeofCriticalSection);
    }

    if (mDSPLockCrit)
    {
        tracker->add(false, FMOD_MEMBITS_SYSTEM, gSizeofCriticalSection);
    }

    if (mDSPConnectionCrit)
    {
        tracker->add(false, FMOD_MEMBITS_SYSTEM, gSizeofCriticalSection);
    }

#ifdef FMOD_NEED_DSPCODECPOOLINITCRIT
    if (mDSPCodecPoolInitCrit)
    {
        tracker->add(false, FMOD_MEMBITS_SYSTEM, gSizeofCriticalSection);
    }
#endif

//    FMOD_OS_GetMemoryUsed(tracker);

    {
        LinkedListNode *current;

        current = FMOD::gGlobal->gFileThreadHead.getNext();
        while (current != &FMOD::gGlobal->gFileThreadHead)
        {
            tracker->add(false, FMOD_MEMBITS_FILE, sizeof(FileThread));
            tracker->add(false, FMOD_MEMBITS_FILE, gSizeofSemaphore);
            tracker->add(false, FMOD_MEMBITS_FILE, gSizeofCriticalSection);
            current = current->getNext();
        }
    }

    if (mDSPSoundCard)
    {
#ifdef PLATFORM_PS3
        CHECK_RESULT(((DSPI *)mDSPSoundCard)->getMemoryUsed(tracker));
#else
        CHECK_RESULT(((DSPSoundCard *)mDSPSoundCard)->getMemoryUsed(tracker));
#endif
    }

    if (mDSPChannelGroupTarget)
    {
        CHECK_RESULT(mDSPChannelGroupTarget->getMemoryUsed(tracker));
    }

    if (mSoftware)
    {
        CHECK_RESULT(mSoftware->getMemoryUsed(tracker));
    }
#endif

    if (mPluginFactory)
    {
        CHECK_RESULT(mPluginFactory->getMemoryUsed(tracker));
    }

    for (node = mChannelGroupHead.getNext(); node != &mChannelGroupHead; node = node->getNext())
    {
        ChannelGroupI *channelgroupi = (ChannelGroupI *)node;
        CHECK_RESULT(channelgroupi->getMemoryUsed(tracker));
    }

    if (mSoundGroup)
    {
        CHECK_RESULT(mSoundGroup->getMemoryUsed(tracker));
    }

//        MemSingleton                    mMultiSubSampleLockBuffer;

    if (mMultiSubSampleLockBufferCrit)
    {
        tracker->add(false, FMOD_MEMBITS_SYSTEM, gSizeofCriticalSection);
    }

    if (FMOD::gGlobal->gFileCrit)
    {
        tracker->add(false, FMOD_MEMBITS_SYSTEM, gSizeofCriticalSection);
    }

    if (FMOD::gGlobal->gAsyncCrit)
    {
        tracker->add(false, FMOD_MEMBITS_SYSTEM, gSizeofCriticalSection);
    }

    CHECK_RESULT(mSpeakerLevelsPool.getMemoryUsed(tracker));
    CHECK_RESULT(mHistoryBufferPool.getMemoryUsed(tracker));

#ifdef FMOD_SUPPORT_STREAMING
//        LinkedListNode                  mStreamListSoundHead;
    CHECK_RESULT(mStreamThread.getMemoryUsed(tracker));

    if (mStreamRealchanCrit)
    {
        tracker->add(false, FMOD_MEMBITS_SYSTEM, gSizeofCriticalSection);
    }

    if (mStreamUpdateCrit)
    {
        tracker->add(false, FMOD_MEMBITS_SYSTEM, gSizeofCriticalSection);
    }

    if (mStreamListCrit)
    {
        tracker->add(false, FMOD_MEMBITS_SYSTEM, gSizeofCriticalSection);
    }
#endif

#ifdef FMOD_SUPPORT_DSPCODEC
    #ifdef FMOD_SUPPORT_MPEG
    CHECK_RESULT(mDSPCodecPool_MPEG.getMemoryUsed(tracker));
    #endif
    #ifdef FMOD_SUPPORT_IMAADPCM
    CHECK_RESULT(mDSPCodecPool_ADPCM.getMemoryUsed(tracker));
    #endif
    #ifdef FMOD_SUPPORT_XMA
    CHECK_RESULT(mDSPCodecPool_XMA.getMemoryUsed(tracker));
    #endif
    #ifdef FMOD_SUPPORT_CELT
    CHECK_RESULT(mDSPCodecPool_CELT.getMemoryUsed(tracker));
    #endif
    #ifdef FMOD_SUPPORT_RAWCODEC
    CHECK_RESULT(mDSPCodecPool_RAW.getMemoryUsed(tracker));
    #endif
#endif

#ifdef FMOD_SUPPORT_GEOMETRY
//		GeometryI					   *mGeometryList;
//		GeometryMgr                     mGeometryMgr;
#endif

    CHECK_RESULT(mReverbGlobal.getMemoryUsed(tracker));

#ifdef FMOD_SUPPORT_MULTIREVERB
    CHECK_RESULT(mReverb3D.getMemoryUsed(tracker));

    for (node = mReverb3DHead.getNext(); node != &mReverb3DHead; node = node->getNext())
    {
        CHECK_RESULT(((ReverbI *)node)->getMemoryUsed(tracker));
    }
#endif
  
    #ifdef FMOD_SUPPORT_PROFILE
    if (gGlobal->gProfile)
    {
        CHECK_RESULT(gGlobal->gProfile->getMemoryUsed(tracker));
    }
    #endif


    tracker->add(false, FMOD_MEMBITS_SYSTEM, FMOD_OS_GetMemoryUsed());
    
    #ifdef FMOD_SUPPORT_PROFILE
    if (gGlobal->gProfile)
    {
        CHECK_RESULT(gGlobal->gProfile->getMemoryUsed(tracker));
    }
    #endif

#endif  // #ifndef FMOD_STATICFORPLUGINS

    return FMOD_OK;
}

#endif

}
