/*$ preserve start $*/

#include "fmod_settings.h"
#include "fmod_systemi.h"
#include "fmod_cmdlog.h"

namespace FMOD
{


//AJS
/*AJS
#ifdef FMOD_SUPPORT_CMDLOG
    result = FMOD_CmdLog_Release();
    if (result != FMOD_OK)
    {
        return result;
    }
#endif
AJS*/

/*$ preserve end $*/


// OFFSET: 0x008D1150
FMOD_RESULT System::release()
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->release();
    }
}


// OFFSET: 0x008D1180
FMOD_RESULT System::setOutput(FMOD_OUTPUTTYPE output)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setOutput(output);
    }
}


// OFFSET: 0x008D11B0
FMOD_RESULT System::getOutput(FMOD_OUTPUTTYPE *output)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getOutput(output);
    }
}


// OFFSET: 0x008D11E0
FMOD_RESULT System::getNumDrivers(int *numdrivers)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getNumDrivers(numdrivers);
    }
}


FMOD_RESULT System::getDriverInfo(int id, char *name, int namelen, FMOD_GUID *guid)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getDriverInfo(id, name, namelen, guid);
    }
}


FMOD_RESULT System::getDriverInfoW(int id, short *name, int namelen, FMOD_GUID *guid)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getDriverInfoW(id, name, namelen, guid);
    }
}


// OFFSET: 0x008D1240
FMOD_RESULT System::getDriverCaps(int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency, FMOD_SPEAKERMODE *controlpanelspeakermode)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getDriverCaps(id, caps, minfrequency, maxfrequency, controlpanelspeakermode);
    }
}


// OFFSET: 0x008D1280
FMOD_RESULT System::setDriver(int driver)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setDriver(driver);
    }
}


FMOD_RESULT System::getDriver(int *driver)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getDriver(driver);
    }
}


// OFFSET: 0x008D12B0
FMOD_RESULT System::setHardwareChannels(int min2d, int max2d, int min3d, int max3d)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setHardwareChannels(min2d, max2d, min3d, max3d);
    }
}


// OFFSET: 0x008D12F0
FMOD_RESULT System::setSoftwareChannels(int numsoftwarechannels)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setSoftwareChannels(numsoftwarechannels);
    }
}


FMOD_RESULT System::getSoftwareChannels(int *numsoftwarechannels)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getSoftwareChannels(numsoftwarechannels);
    }
}


// OFFSET: 0x008D1320
FMOD_RESULT System::setSoftwareFormat(int samplerate, FMOD_SOUND_FORMAT format, int numoutputchannels, int maxinputchannels, FMOD_DSP_RESAMPLER resamplemethod)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setSoftwareFormat(samplerate, format, numoutputchannels, maxinputchannels, resamplemethod);
    }
}


FMOD_RESULT System::getSoftwareFormat(int *samplerate, FMOD_SOUND_FORMAT *format, int *numoutputchannels, int *maxinputchannels, FMOD_DSP_RESAMPLER *resamplemethod, int *bits)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getSoftwareFormat(samplerate, format, numoutputchannels, maxinputchannels, resamplemethod, bits);
    }
}


// OFFSET: 0x008D1360
FMOD_RESULT System::setDSPBufferSize(unsigned int bufferlength, int numbuffers)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setDSPBufferSize(bufferlength, numbuffers);
    }
}


FMOD_RESULT System::getDSPBufferSize(unsigned int *bufferlength, int *numbuffers)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getDSPBufferSize(bufferlength, numbuffers);
    }
}


FMOD_RESULT System::setFileSystem(FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek, int blockalign)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setFileSystem(useropen, userclose, userread, userseek, blockalign);
    }
}


FMOD_RESULT System::attachFileSystem(FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->attachFileSystem(useropen, userclose, userread, userseek);
    }
}


FMOD_RESULT System::setAdvancedSettings(FMOD_ADVANCEDSETTINGS *settings)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setAdvancedSettings(settings);
    }
}


FMOD_RESULT System::getAdvancedSettings(FMOD_ADVANCEDSETTINGS *settings)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getAdvancedSettings(settings);
    }
}


// OFFSET: 0x008D1390
FMOD_RESULT System::setSpeakerMode(FMOD_SPEAKERMODE speakermode)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setSpeakerMode(speakermode);
    }
}


FMOD_RESULT System::getSpeakerMode(FMOD_SPEAKERMODE *speakermode)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getSpeakerMode(speakermode);
    }
}


// OFFSET: 0x008D13C0
FMOD_RESULT System::setCallback(FMOD_SYSTEM_CALLBACK callback)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setCallback(callback);
    }
}


FMOD_RESULT System::setPluginPath(const char *path)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setPluginPath(path);
    }
}


FMOD_RESULT System::loadPlugin(const char *filename, unsigned int *handle, unsigned int priority)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->loadPlugin(filename, handle, priority);
    }
}


FMOD_RESULT System::unloadPlugin(unsigned int handle)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->unloadPlugin(handle);
    }
}


FMOD_RESULT System::getNumPlugins(FMOD_PLUGINTYPE plugintype, int *numplugins)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getNumPlugins(plugintype, numplugins);
    }
}


FMOD_RESULT System::getPluginHandle(FMOD_PLUGINTYPE plugintype, int index, unsigned int *handle)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getPluginHandle(plugintype, index, handle);
    }
}


FMOD_RESULT System::getPluginInfo(unsigned int handle, FMOD_PLUGINTYPE *plugintype, char *name, int namelen, unsigned int *version)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getPluginInfo(handle, plugintype, name, namelen, version);
    }
}


FMOD_RESULT System::setOutputByPlugin(unsigned int handle)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setOutputByPlugin(handle);
    }
}


FMOD_RESULT System::getOutputByPlugin(unsigned int *handle)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getOutputByPlugin(handle);
    }
}


FMOD_RESULT System::createDSPByPlugin(unsigned int handle, DSP **dsp)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->createDSPByPlugin(handle, (DSPI **)dsp);
    }
}


FMOD_RESULT System::createCodec(FMOD_CODEC_DESCRIPTION *description, unsigned int priority)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->createCodec(description, priority);
    }
}


// OFFSET: 0x008D13F0
FMOD_RESULT System::init(int maxchannels, FMOD_INITFLAGS flags, void *extradriverdata)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->init(maxchannels, flags, extradriverdata);
    }
}


// OFFSET: 0x008D1420
FMOD_RESULT System::close()
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->close();
    }
}


// OFFSET: 0x008D1450
FMOD_RESULT System::update()
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->update();
    }
}


// OFFSET: 0x008D1480
FMOD_RESULT System::set3DSettings(float dopplerscale, float distancefactor, float rolloffscale)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->set3DSettings(dopplerscale, distancefactor, rolloffscale);
    }
}


FMOD_RESULT System::get3DSettings(float *dopplerscale, float *distancefactor, float *rolloffscale)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->get3DSettings(dopplerscale, distancefactor, rolloffscale);
    }
}


FMOD_RESULT System::set3DNumListeners(int numlisteners)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->set3DNumListeners(numlisteners);
    }
}


FMOD_RESULT System::get3DNumListeners(int *numlisteners)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->get3DNumListeners(numlisteners);
    }
}


// OFFSET: 0x008D14C0
FMOD_RESULT System::set3DListenerAttributes(int listener, const FMOD_VECTOR *pos, const FMOD_VECTOR *vel, const FMOD_VECTOR *forward, const FMOD_VECTOR *up)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->set3DListenerAttributes(listener, pos, vel, forward, up);
    }
}


// OFFSET: 0x008D1500
FMOD_RESULT System::get3DListenerAttributes(int listener, FMOD_VECTOR *pos, FMOD_VECTOR *vel, FMOD_VECTOR *forward, FMOD_VECTOR *up)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->get3DListenerAttributes(listener, pos, vel, forward, up);
    }
}


// OFFSET: 0x008D1540
FMOD_RESULT System::set3DRolloffCallback(FMOD_3D_ROLLOFFCALLBACK callback)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->set3DRolloffCallback(callback);
    }
}


FMOD_RESULT System::set3DSpeakerPosition(FMOD_SPEAKER speaker, float x, float y, bool active)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->set3DSpeakerPosition(speaker, x, y, active);
    }
}


FMOD_RESULT System::get3DSpeakerPosition(FMOD_SPEAKER speaker, float *x, float *y, bool *active)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->get3DSpeakerPosition(speaker, x, y, active);
    }
}


FMOD_RESULT System::setStreamBufferSize(unsigned int filebuffersize, FMOD_TIMEUNIT filebuffersizetype)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setStreamBufferSize(filebuffersize, filebuffersizetype);
    }
}


FMOD_RESULT System::getStreamBufferSize(unsigned int *filebuffersize, FMOD_TIMEUNIT *filebuffersizetype)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getStreamBufferSize(filebuffersize, filebuffersizetype);
    }
}


// OFFSET: 0x008D1570
FMOD_RESULT System::getVersion(unsigned int *version)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getVersion(version);
    }
}


FMOD_RESULT System::getOutputHandle(void **handle)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getOutputHandle(handle);
    }
}


FMOD_RESULT System::getChannelsPlaying(int *channels)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getChannelsPlaying(channels);
    }
}


// OFFSET: 0x008D15A0
FMOD_RESULT System::getHardwareChannels(int *num2d, int *num3d, int *total)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getHardwareChannels(num2d, num3d, total);
    }
}


FMOD_RESULT System::getCPUUsage(float *dsp, float *stream, float *geometry, float *update, float *total)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getCPUUsage(dsp, stream, geometry, update, total);
    }
}


FMOD_RESULT System::getSoundRAM(int *currentalloced, int *maxalloced, int *total)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getSoundRAM(currentalloced, maxalloced, total);
    }
}


FMOD_RESULT System::getNumCDROMDrives(int *numdrives)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getNumCDROMDrives(numdrives);
    }
}


FMOD_RESULT System::getCDROMDriveName(int drive, char *drivename, int drivenamelen, char *scsiname, int scsinamelen, char *devicename, int devicenamelen)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getCDROMDriveName(drive, drivename, drivenamelen, scsiname, scsinamelen, devicename, devicenamelen);
    }
}


FMOD_RESULT System::getSpectrum(float *spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getSpectrum(spectrumarray, numvalues, channeloffset, windowtype);
    }
}


FMOD_RESULT System::getWaveData(float *wavearray, int numvalues, int channeloffset)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getWaveData(wavearray, numvalues, channeloffset);
    }
}


// OFFSET: 0x008D15D0
FMOD_RESULT System::createSound(const char *name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *exinfo, Sound **sound)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->createSound(name_or_data, mode, exinfo, (SoundI **)sound);
    }
}


// OFFSET: 0x008D1610
FMOD_RESULT System::createStream(const char *name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *exinfo, Sound **sound)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->createStream(name_or_data, mode, exinfo, (SoundI **)sound);
    }
}


// OFFSET: 0x008D1650
FMOD_RESULT System::createDSP(FMOD_DSP_DESCRIPTION *description, DSP **dsp)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->createDSP(description, (DSPI **)dsp);
    }
}


// OFFSET: 0x008D1680
FMOD_RESULT System::createDSPByType(FMOD_DSP_TYPE type, DSP **dsp)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->createDSPByType(type, (DSPI **)dsp);
    }
}


// OFFSET: 0x008D16B0
FMOD_RESULT System::createChannelGroup(const char *name, ChannelGroup **channelgroup)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->createChannelGroup(name, (ChannelGroupI **)channelgroup);
    }
}


// OFFSET: 0x008D16E0
FMOD_RESULT System::createSoundGroup(const char *name, SoundGroup **soundgroup)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->createSoundGroup(name, (SoundGroupI **)soundgroup);
    }
}


FMOD_RESULT System::createReverb(Reverb **reverb)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->createReverb((ReverbI **)reverb);
    }
}


// OFFSET: 0x008D1710
FMOD_RESULT System::playSound(FMOD_CHANNELINDEX channelid, Sound *sound, bool paused, Channel **channel)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->playSound(channelid, (SoundI *)sound, paused, (ChannelI **)channel);
    }
}


// OFFSET: 0x008D1750
FMOD_RESULT System::playDSP(FMOD_CHANNELINDEX channelid, DSP *dsp, bool paused, Channel **channel)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->playDSP(channelid, (DSPI *)dsp, paused, (ChannelI **)channel);
    }
}


// OFFSET: 0x008D1790
FMOD_RESULT System::getChannel(int channelid, Channel **channel)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getChannel(channelid, (ChannelI **)channel);
    }
}


FMOD_RESULT System::getMasterChannelGroup(ChannelGroup **channelgroup)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getMasterChannelGroup((ChannelGroupI **)channelgroup);
    }
}


FMOD_RESULT System::getMasterSoundGroup(SoundGroup **soundgroup)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getMasterSoundGroup((SoundGroupI **)soundgroup);
    }
}


// OFFSET: 0x008D17C0
FMOD_RESULT System::setReverbProperties(const FMOD_REVERB_PROPERTIES *prop)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setReverbProperties(prop);
    }
}


FMOD_RESULT System::getReverbProperties(FMOD_REVERB_PROPERTIES *prop)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getReverbProperties(prop);
    }
}


FMOD_RESULT System::setReverbAmbientProperties(FMOD_REVERB_PROPERTIES *prop)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setReverbAmbientProperties(prop);
    }
}


FMOD_RESULT System::getReverbAmbientProperties(FMOD_REVERB_PROPERTIES *prop)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getReverbAmbientProperties(prop);
    }
}


FMOD_RESULT System::getDSPHead(DSP **dsp)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getDSPHead((DSPI **)dsp);
    }
}


FMOD_RESULT System::addDSP(DSP *dsp, DSPConnection **connection)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->addDSP((DSPI *)dsp, (DSPConnectionI **)connection);
    }
}


FMOD_RESULT System::lockDSP()
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->lockDSP();
    }
}


FMOD_RESULT System::unlockDSP()
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->unlockDSP();
    }
}


FMOD_RESULT System::getDSPClock(unsigned int *hi, unsigned int *lo)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getDSPClock(hi, lo);
    }
}


// OFFSET: 0x008D1820
FMOD_RESULT System::getRecordNumDrivers(int *numdrivers)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getRecordNumDrivers(numdrivers);
    }
}


FMOD_RESULT System::getRecordDriverInfo(int id, char *name, int namelen, FMOD_GUID *guid)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getRecordDriverInfo(id, name, namelen, guid);
    }
}


FMOD_RESULT System::getRecordDriverInfoW(int id, short *name, int namelen, FMOD_GUID *guid)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getRecordDriverInfoW(id, name, namelen, guid);
    }
}


FMOD_RESULT System::getRecordDriverCaps(int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getRecordDriverCaps(id, caps, minfrequency, maxfrequency);
    }
}


// OFFSET: 0x008D1880
FMOD_RESULT System::getRecordPosition(int id, unsigned int *position)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getRecordPosition(id, position);
    }
}


// OFFSET: 0x008D18B0
FMOD_RESULT System::recordStart(int id, Sound *sound, bool loop)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->recordStart(id, (SoundI *)sound, loop);
    }
}


// OFFSET: 0x008D18E0
FMOD_RESULT System::recordStop(int id)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->recordStop(id);
    }
}


// OFFSET: 0x008D1910
FMOD_RESULT System::isRecording(int id, bool *recording)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->isRecording(id, recording);
    }
}


FMOD_RESULT System::createGeometry(int maxpolygons, int maxvertices, Geometry **geometry)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->createGeometry(maxpolygons, maxvertices, (GeometryI **)geometry);
    }
}


FMOD_RESULT System::setGeometrySettings(float maxworldsize)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setGeometrySettings(maxworldsize);
    }
}


FMOD_RESULT System::getGeometrySettings(float *maxworldsize)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getGeometrySettings(maxworldsize);
    }
}


FMOD_RESULT System::loadGeometry(const void *data, int datasize, Geometry **geometry)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->loadGeometry(data, datasize, (GeometryI **)geometry);
    }
}


FMOD_RESULT System::getGeometryOcclusion(const FMOD_VECTOR *listener, const FMOD_VECTOR *source, float *direct, float *reverb)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getGeometryOcclusion(listener, source, direct, reverb);
    }
}


FMOD_RESULT System::setNetworkProxy(const char *proxy)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setNetworkProxy(proxy);
    }
}


FMOD_RESULT System::getNetworkProxy(char *proxy, int proxylen)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getNetworkProxy(proxy, proxylen);
    }
}


FMOD_RESULT System::setNetworkTimeout(int timeout)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setNetworkTimeout(timeout);
    }
}


FMOD_RESULT System::getNetworkTimeout(int *timeout)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getNetworkTimeout(timeout);
    }
}


FMOD_RESULT System::setUserData(void *_userdata)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->setUserData(_userdata);
    }
}


FMOD_RESULT System::getUserData(void **_userdata)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getUserData(_userdata);
    }
}


FMOD_RESULT System::getMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, FMOD_MEMORY_USAGE_DETAILS *memoryused_details)
{
    FMOD_RESULT result;
    SystemI *systemi;

    result = SystemI::validate(this, &systemi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return systemi->getMemoryInfo(memorybits, event_memorybits, memoryused, memoryused_details);
    }
}


/*$ preserve start $*/
}
/*$ preserve end $*/
