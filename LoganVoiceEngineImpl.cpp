
#include "LoganVoiceEngineImpl.h"


#define VALIDATE                                       \
    if (res != 0){                                     \
    printf("##%s  %i:\n",__FILE__, __LINE__);          \
    printf("       %s  Called Error, ",__FUNCTION__);  \
    printf("code: %i\n", base->LastError());      \
    }


static int res = 0;


LoganVoiceEngine* LoganVoiceEngine::Create() {
    LoganVoiceEngineImpl* lap = new LoganVoiceEngineImpl();
    if (lap->LoganVoe_Init() != 0) {
        delete lap;
        lap = NULL;
    }

    return lap;
}

void LoganVoiceEngine::Destroy(LoganVoiceEngine* lve) {
    delete static_cast<LoganVoiceEngineImpl*>(lve);
}


LoganVoiceEngineImpl::LoganVoiceEngineImpl():
    m_voe(NULL),
    base(NULL),
    codec(NULL),
    volume(NULL),
    apm(NULL),
    netw(NULL),
    hardware(NULL)
{

}

LoganVoiceEngineImpl::~LoganVoiceEngineImpl()
{
    LoganVoe_UnInit();
}


int LoganVoiceEngineImpl::LoganVoe_Init()
{
    m_voe = VoiceEngine::Create();

    base = VoEBase::GetInterface(m_voe);
    codec = VoECodec::GetInterface(m_voe);
    apm = VoEAudioProcessing::GetInterface(m_voe);
    volume = VoEVolumeControl::GetInterface(m_voe);
    netw = VoENetwork::GetInterface(m_voe);
    file = VoEFile::GetInterface(m_voe);
    hardware = VoEHardware::GetInterface(m_voe);

    //for no trace
    VoiceEngine::SetTraceFilter(kTraceNone);

    return base->Init();
}

void LoganVoiceEngineImpl::LoganVoe_UnInit()
{
    base->Terminate();

    if (base)
        base->Release();

    if (codec)
        codec->Release();

    if (volume)
        volume->Release();

    if (apm)
        apm->Release();

    if (netw)
        netw->Release();

    if (file)
        file->Release();


    if (hardware)
        hardware->Release();


    VoiceEngine::Delete(m_voe);
}

int LoganVoiceEngineImpl::LoganVoe_CreateChannel()
{
    return base->CreateChannel();
}
int LoganVoiceEngineImpl::LoganVoe_DeleteChannel(int channel)
{
    res = base->DeleteChannel(channel);
    VALIDATE;
    return res;
}
int LoganVoiceEngineImpl::LoganVoe_SetSendDestination(int channelId, char * ip, int port)
{/*
    if (channelId < 0)
    {
        return -1;
    }
    if (ip.empty())
    {
        return -2;
    }

    VoiceChannelTransport *voice_channel_transport(new VoiceChannelTransport(netw, channelId));

    res = voice_channel_transport->SetSendDestination(ip, port);
    VALIDATE;
    //构建一对通道id和transport的关系
    VoiceChanTrans vct;
    vct.channelid = channelId;
    vct.trans = (void *)voice_channel_transport;
    vec_vct.push_back(vct);
*/
    return -1;
}

int LoganVoiceEngineImpl::LoganVoe_SetLocalReceiver(int channelId,int port)
{
   /* VoiceChannelTransport* p= (VoiceChannelTransport*)FindTransFromChannel(channelId);
    if (p == NULL)
    {
        return -1;
    }

    res = p->SetLocalReceiver(port);
    VALIDATE;*/
    return -1;
}

int LoganVoiceEngineImpl::LoganVoe_GetSpecCodec(int index, LoganCodec &lc)
{
    if (index < 0 || index >= codec->NumOfCodecs())
    {
        res = -1;
        VALIDATE;
        return res;
    }
    CodecInst c;
    res = codec->GetCodec(index, c);
    VALIDATE;
    lc.channels = c.channels;
    lc.pacsize = c.pacsize;
    memcpy(lc.plname,c.plname, RTP_PAYLOAD_NAME_LEN);
    lc.plfreq = c.plfreq;
    lc.pltype = c.pltype;
    lc.rate = c.rate;
    
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_SetSendCodec(int channelid, LoganCodec lc)
{
    CodecInst c;
    c.channels = lc.channels;
    c.pacsize  = lc.pacsize ;
    c.plfreq   = lc.plfreq  ;
    c.pltype   = lc.pltype  ;
    c.rate     = lc.rate    ;
    memcpy(c.plname, lc.plname, RTP_PAYLOAD_NAME_LEN);
    res = codec->SetSendCodec(channelid, c);
    VALIDATE;
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_GetNumsOfCodec()
{
    res = codec->NumOfCodecs();
    VALIDATE;
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_GetNumOfRecordingDevices()
{
    int rd(-1);
    res = hardware->GetNumOfRecordingDevices(rd);
    VALIDATE;
    return rd;
}
int LoganVoiceEngineImpl::LoganVoe_GetNumOfPlayoutDevices()
{
    int rd(-1);
    res = hardware->GetNumOfPlayoutDevices(rd);
    VALIDATE;
    return rd;
}

int LoganVoiceEngineImpl::LoganVoe_GetPlayoutDeviceName(int index, char nameUTF8[128], char guidUTF8[128])
{
    char dn[DEVICE_MAX_LEN] = { 0 };
    char guid[DEVICE_MAX_LEN] = { 0 };
    res = hardware->GetPlayoutDeviceName(index, nameUTF8, guidUTF8);
    VALIDATE;

    return res;
}
int LoganVoiceEngineImpl::LoganVoe_GetRecordingDeviceName(int index, char nameUTF8[128], char guidUTF8[128])
{
    char dn[DEVICE_MAX_LEN] = { 0 };
    char guid[DEVICE_MAX_LEN] = { 0 };
    res = hardware->GetRecordingDeviceName(index, nameUTF8, guidUTF8);
    VALIDATE;
    
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_SetPlayoutDevice(int idx)
{
    if(idx >= LoganVoe_GetNumOfPlayoutDevices())
    {
        return -1;
    }
    res = hardware->SetPlayoutDevice(idx);
    VALIDATE;
    return res;
}
int LoganVoiceEngineImpl::LoganVoe_SetRecordingDevice(int idx)
{
    if (idx >= LoganVoe_GetNumOfRecordingDevices())
    {
        return -1;
    }
    res = hardware->SetRecordingDevice(idx);
    VALIDATE;
    return res;
}
//for vad
int LoganVoiceEngineImpl::LoganVoe_SetVADStatus(int channelsid, bool b, int mode)
{
    res = codec->SetVADStatus(channelsid, b, (VadModes)mode);
    VALIDATE;
    return res;
}
int LoganVoiceEngineImpl::LoganVoe_GetVADStatus(int channelsid,bool &b, int &mode)
{
    bool disabledDTX;
    VadModes m;
    res = codec->GetVADStatus(channelsid,b,(VadModes)m, disabledDTX);
    VALIDATE;
    mode = (int)m;
    return res;
}
//for agc
int LoganVoiceEngineImpl::LoganVoe_SetAgcStatus(bool b, int mode)
{
    res = apm->SetAgcStatus(b, (AgcModes)mode);
    VALIDATE;
    return res;
}
int LoganVoiceEngineImpl::LoganVoe_GetAgcStatus(bool &b, int &mode)
{
    AgcModes m;
    res = apm->GetAgcStatus(b,(AgcModes)m);
    VALIDATE;
    mode = (int)m;
    return res;
}

//for ec
int LoganVoiceEngineImpl::LoganVoe_SetEcStatus(bool b, int mode)
{
    res = apm->SetEcStatus(b, (EcModes)mode);
    VALIDATE;
    return res;
}
int LoganVoiceEngineImpl::LoganVoe_GetEcStatus(bool &b, int &mode)
{
    EcModes m;
    res = apm->GetEcStatus(b,(EcModes)m);
    VALIDATE;
    mode = (int)m;
    return res;
}

//for ns
int LoganVoiceEngineImpl::LoganVoe_SetNsStatus(bool b, int mode)
{
    res = apm->SetNsStatus(b, (NsModes)mode);
    VALIDATE;
    return res;
}
int LoganVoiceEngineImpl::LoganVoe_GetNsStatus(bool &b, int &mode)
{
    NsModes m;
    res = apm->GetNsStatus(b,(NsModes)m);
    VALIDATE;
    mode = int(m);
    return res;
}


int LoganVoiceEngineImpl::LoganVoe_StartReceive(int channelid)
{
    res = base->StartReceive(channelid);
    VALIDATE;
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_StartPlayout(int channelid)
{
    res = base->StartPlayout(channelid);
    VALIDATE;
    return res;
}
int LoganVoiceEngineImpl::LoganVoe_StartSend(int channelid)
{
    res = base->StartSend(channelid);
    VALIDATE;
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_StopReceive(int channelid)
{
    res = base->StopReceive(channelid);
    VALIDATE;
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_StopPlayout(int channelid)
{
    res = base->StopPlayout(channelid);
    VALIDATE;
    return res;
}
int LoganVoiceEngineImpl::LoganVoe_StopSend(int channelid)
{
    res = base->StopSend(channelid);
    VALIDATE;
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_StartRecordingMicrophone()
{
    res = file->StartRecordingMicrophone(&_outStream);
    VALIDATE;
    return res;
}
int LoganVoiceEngineImpl::LoganVoe_StartPlayingFileLocally(int channelid)
{
    res = file->StartPlayingFileLocally(channelid, &_inStream);
    VALIDATE;
    return res;
}
int LoganVoiceEngineImpl::LoganVoe_StopPlayingFileLocally(int channelid)
{
    res = file->StopPlayingFileLocally(channelid);
    VALIDATE;
    return res;
}
int LoganVoiceEngineImpl::LoganVoe_StopRecordingMicrophone()
{
    res = file->StopRecordingMicrophone();
    VALIDATE;
    return res;
}


unsigned int LoganVoiceEngineImpl::LoganVoe_GetMicVolume()
{
    unsigned int vol = 999;
    res = volume->GetMicVolume(vol);
    VALIDATE;
    return vol;
}

int LoganVoiceEngineImpl::LoganVoe_SetMicVolume(unsigned int vol)
{
    res = volume->SetMicVolume(vol);
    VALIDATE;
    return vol;
}

unsigned int LoganVoiceEngineImpl::LoganVoe_GetSpeakerVolume()
{
    unsigned int vol = 999;
    res = volume->GetSpeakerVolume(vol);
    VALIDATE;
    return vol;
}
int LoganVoiceEngineImpl::LoganVoe_SetInputMute(int channel, bool enable)
{
    res = volume->SetInputMute(channel, enable);
    VALIDATE;
    return res;
}


int LoganVoiceEngineImpl::LoganVoe_SetSpeakerVolume(unsigned int vol)
{
    res = volume->SetSpeakerVolume(vol);
    VALIDATE;
    return vol;
}

int LoganVoiceEngineImpl::LoganVoe_SetCallBackFuncs(LoganReadStreamCallBack lrscb,
                              LoganWriteStreamCallBack lwscb)
{
    if (lrscb == NULL || lwscb == NULL)
    {
        return -1;
    }
    _inStream.pRead = lrscb;
    _outStream.pWrite = lwscb;
    return 0;

}

int LoganVoiceEngineImpl::LoganVoe_GetInputMute(int channel, bool& enabled)
{
    res = volume->GetInputMute(channel, enabled);
    VALIDATE;
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_SetSystemOutputMute(bool enable)
{
    res = volume->SetSystemInputMute(enable);
    VALIDATE;
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_GetSystemOutputMute(bool &enabled)
{
    res = volume->GetSystemOutputMute(enabled);
    VALIDATE;
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_SetSystemInputMute(bool enable)
{
    res = volume->SetSystemInputMute(enable);
    VALIDATE;
    return res;
}
int LoganVoiceEngineImpl::LoganVoe_GetSystemInputMute(bool& enabled)
{
    res = volume->GetSystemInputMute(enabled);
    VALIDATE;
    return res;
}
