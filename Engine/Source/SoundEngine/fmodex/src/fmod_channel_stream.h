#ifndef _FMOD_CHANNEL_STREAM_H
#define _FMOD_CHANNEL_STREAM_H

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_STREAMING

#include "fmod_channel_real.h"

namespace FMOD
{   
    class ChannelStream : public ChannelReal, public LinkedListNode
    {
      public:
        
        volatile bool   mFinished;
        unsigned int    mLastPCM;
        unsigned int    mDecodeOffset;
        unsigned int    mSamplesPlayed;
        unsigned int    mSamplesWritten;
        LinkedListNode  mStreamNode;
        int             mNumRealChannels;
        ChannelReal    *mRealChannel[FMOD_CHANNEL_MAXREALSUBCHANNELS];       /* Even a real channel may contain a pointer to another real channel! (ie stream channel points to dsound channel) */

      public:
        
        ChannelStream();

        FMOD_RESULT setRealChannel(ChannelReal *realchan);
        bool        isStream() { return true; }
       
        FMOD_RESULT  set2DFreqVolumePanFor3D();
        FMOD_RESULT  moveChannelGroup(ChannelGroupI *oldchannelgroup, ChannelGroupI *newchannelgroup, bool forcedspreconnect);
 
        FMOD_RESULT  alloc               ();
        FMOD_RESULT  start               ();
        FMOD_RESULT  update              (int delta);
        FMOD_RESULT  updateStream        ();
        FMOD_RESULT  setMode             (FMOD_MODE mode);                                  
                                                                                            
        FMOD_RESULT  stop                ();
        FMOD_RESULT  setPaused           (bool paused);
        FMOD_RESULT  setVolume           (float volume);
        FMOD_RESULT  setFrequency        (float frequency);
        FMOD_RESULT  setPan              (float pan, float fbpan = 1);
        FMOD_RESULT  setDSPClockDelay    ();
        FMOD_RESULT  setSpeakerMix       (float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright);
        FMOD_RESULT  setSpeakerLevels    (int speaker, float *levels, int numlevels);
        // OFFSET: 0x0091F6F0
        FMOD_RESULT  setPosition         (unsigned int position, FMOD_TIMEUNIT postype) { return setPositionEx(position, postype); }
        FMOD_RESULT  setPositionEx       (unsigned int position, FMOD_TIMEUNIT postype, bool fromasync = false);
        FMOD_RESULT  getPosition         (unsigned int *position, FMOD_TIMEUNIT postype);
        FMOD_RESULT  setLoopPoints       (unsigned int loopstart, unsigned int looplength);
        FMOD_RESULT  setLoopCount        (int loopcount);
        FMOD_RESULT  setLowPassGain      (float gain);
        FMOD_RESULT  set3DAttributes     ();
        FMOD_RESULT  set3DMinMaxDistance ();
        FMOD_RESULT  set3DConeSettings   (float iconeangle, float oconeangle, float ovolume);
        FMOD_RESULT  set3DConeOrientation(FMOD_VECTOR *orientation);
        FMOD_RESULT  set3DOcclusion      (float directocclusion, float reverbocclusion);
        FMOD_RESULT  setReverbProperties (const FMOD_REVERB_CHANNELPROPERTIES *prop);
        FMOD_RESULT  getReverbProperties (FMOD_REVERB_CHANNELPROPERTIES *prop);
        FMOD_RESULT  isPlaying           (bool *isplaying, bool includethreadlatency = false);
        FMOD_RESULT  getSpectrum         (float *spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
        FMOD_RESULT  getWaveData         (float *wavearray, int numvalues, int channeloffset);
        FMOD_RESULT  getDSPHead          (DSPI **dsp);

#ifdef PLATFORM_WII
        FMOD_RESULT  setLowPassFilter    (int cutoff);
        FMOD_RESULT  getLowPassFilter    (int *cutoff);
        FMOD_RESULT  setBiquadFilter     (bool active, unsigned short b0, unsigned short b1, unsigned short b2, unsigned short a1, unsigned short a2);
        FMOD_RESULT  getBiquadFilter     (bool *active, unsigned short *b0, unsigned short *b1, unsigned short *b2, unsigned short *a1, unsigned short *a2);
        FMOD_RESULT  setControllerSpeaker(unsigned int controllerspeaker, int subchannel = -1);
        FMOD_RESULT  getControllerSpeaker(unsigned int *controllerspeaker);
#endif
    };
}

#endif

#endif

