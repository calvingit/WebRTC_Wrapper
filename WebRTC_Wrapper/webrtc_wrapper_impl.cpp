/*
**Copyright (c) 2013 Calvin Cheung. All Rights Reserved.
**
**Use of this source code is governed by a BSD-style license.
**
**This is a simple wrapper for WebRTC. I used it in the Video Conference project.
**
**URL:https://github.com/calvingit/WebRTC_Wrapper/
**
**File: LoganVoiceEngineImpl.cpp
**
**Author: Calvin Cheung, Chinna.
**
**Update Date: 2013-5-21
*/

#include "webrtc_wrapper_impl.h"

#ifdef _DEBUG
#define VALIDATE                                       \
    if (res != 0){                                     \
    printf("##%s(%i) ERROR:\n",__FILE__, __LINE__);          \
    printf("       %s  error, code = %i\n",__FUNCTION__, base->LastError());  \
    }

#else
#define VALIDATE 
#endif

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
    int channel = base->CreateChannel();

    //¹¹½¨VoiceChanTrans
    VoiceChannelTransport *voice_channel_transport(new VoiceChannelTransport(netw, channel));
    VoiceChanTrans vct;
    vct.channel = channel;
    vct.pTrans = voice_channel_transport;

    vec_vct.push_back(vct);

    return channel;
}
int LoganVoiceEngineImpl::LoganVoe_DeleteChannel(int channel)
{
    int index = LoganVoe_FindIndexOfTrans(channel);
    if (index < 0)
    {
        return index;
    }
    if (vec_vct[index].pTrans)
    {
        delete vec_vct[index].pTrans;
    } 
    res = base->DeleteChannel(vec_vct[index].channel);
    VALIDATE;
    vec_vct[index].SetEmpty();
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_DeleteAllChannel()
{
    for(unsigned int i = 0; i < vec_vct.size(); i++)
    {
        if (vec_vct[i].channel != -1)
        {
            if (vec_vct[i].pTrans)
            {
                delete vec_vct[i].pTrans;
            }
            res = base->DeleteChannel(vec_vct[i].channel);
            VALIDATE;
            vec_vct[i].SetEmpty();
        }
    }
    vec_vct.clear();
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_FindIndexOfTrans(int channelid)
{
    unsigned int i = 0;
    for(; i < vec_vct.size(); i++)
    {
        if (vec_vct[i].channel == channelid)
        {
            return i;
        }
    }
    return  -1;
}

int LoganVoiceEngineImpl::LoganVoe_SetSendDestination(int channelId,const char * ip, int port)
{

    int index = LoganVoe_FindIndexOfTrans(channelId);
    if (index < 0)
    {
        return index;
    }

    res = vec_vct[index].pTrans->SetSendDestination(ip, port);
    VALIDATE;
 
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_SetLocalReceiver(int channelId,int port)
{
    int index = LoganVoe_FindIndexOfTrans(channelId);
    if (index < 0)
    {
        return index;
    }

    res = vec_vct[index].pTrans->SetLocalReceiver(port);
    VALIDATE;
    return res;
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
    return  codec->NumOfCodecs();
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

int LoganVoiceEngineImpl::LoganVoe_StartRecordingMicrophone(LoganWriteStreamCallBack lwscb)
{
    _outStream.pWrite = lwscb;
    res = file->StartRecordingMicrophone(&_outStream);
    VALIDATE;
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_StartRecordingMicrophone(const char* fileNameUTF8, 
                                                            LoganCodec* codec)
{
    if (codec == NULL)
    {
        res = file->StartRecordingMicrophone(fileNameUTF8, NULL);
    } 
    else
    {
        CodecInst ci;
        ci.channels = codec->channels;
        ci.pacsize = codec->pacsize;
        ci.plfreq = codec->plfreq;
        ci.pltype = codec->pltype;
        ci.rate = codec->rate;
        memcpy(ci.plname, codec->plname, RTP_PAYLOAD_NAME_LEN);
        res = file->StartRecordingMicrophone(fileNameUTF8, &ci);
    }
    VALIDATE;
    return res;

}

int LoganVoiceEngineImpl::LoganVoe_StartPlayingFileLocally(int channelid,
                                                           LoganReadStreamCallBack lrscb)
{
    _inStream.pRead = lrscb;
    res = file->StartPlayingFileLocally(channelid, &_inStream);
    VALIDATE;
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_StartPlayingFileLocally(int channelid,
                                                           const char fileNameUTF8[1024],
                                                           int samplerate,//(8000, 16000, 32000)
                                                           bool loop)
{
    FileFormats format;
    switch(samplerate)
    {
    case 8000:format = kFileFormatPcm8kHzFile;break;
    case 16000:format = kFileFormatPcm16kHzFile;break;
    case 32000:format = kFileFormatPcm32kHzFile;break;
    default:format = kFileFormatPcm16kHzFile;break;
    }
    res = file->StartPlayingFileLocally(channelid, fileNameUTF8, loop, format);
    VALIDATE;
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_IsPlayingFileLocally(int channel)
{
    return file->IsPlayingFileLocally(channel);
}

int LoganVoiceEngineImpl::LoganVoe_StartRecordingPlayout(int channel, const char* fileNameUTF8,
                                                         LoganCodec *codec)
{
    if (codec == NULL)
    {
        res = file->StartRecordingPlayout(channel,fileNameUTF8);
    } 
    else
    {
        CodecInst ci;
        ci.channels = codec->channels;
        ci.pacsize = codec->pacsize;
        ci.plfreq = codec->plfreq;
        ci.pltype = codec->pltype;
        ci.rate = codec->rate;
        memcpy(ci.plname, codec->plname, RTP_PAYLOAD_NAME_LEN);
        res = file->StartRecordingPlayout(channel,fileNameUTF8, &ci);
    }
    VALIDATE;
    return res;    
}

int LoganVoiceEngineImpl::LoganVoe_StopRecordingPlayout(int channel)
{
    res = file->StopRecordingPlayout(channel);
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

int LoganVoiceEngineImpl::LoganVoe_GetSpeechInputLevel(unsigned int& level)
{
    res = volume->GetSpeechInputLevel(level);
    VALIDATE;
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_GetSpeechOutputLevel(int channel, unsigned int& level)
{
    res = volume->GetSpeechOutputLevel(channel, level);
    VALIDATE;
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_StartPlayingFileAsMicrophone(
    int channel,
    const char fileNameUTF8[1024],
    int sampleRate,
    bool loop,
    bool mixWithMicrophone)
{
    FileFormats format;
    switch(sampleRate)
    {
    case 8000:format = kFileFormatPcm8kHzFile;break;
    case 16000:format = kFileFormatPcm16kHzFile;break;
    case 32000:format = kFileFormatPcm32kHzFile;break;
    default:format = kFileFormatPcm16kHzFile;break;
    }
    res = file->StartPlayingFileAsMicrophone(channel, fileNameUTF8, loop,
        mixWithMicrophone,format);
    VALIDATE;
    return res;
}


int LoganVoiceEngineImpl::LoganVoe_StartPlayingFileAsMicrophone(
    int channel,
    LoganReadStreamCallBack read,
    int sampleRate,
    bool mixWithMicrophone)
{
    _inStream.pRead = read;
    FileFormats format;
    switch(sampleRate)
    {
    case 8000:format = kFileFormatPcm8kHzFile;break;
    case 16000:format = kFileFormatPcm16kHzFile;break;
    case 32000:format = kFileFormatPcm32kHzFile;break;
    default:format = kFileFormatPcm16kHzFile;break;
    }
    res = file->StartPlayingFileAsMicrophone(channel, &_inStream, mixWithMicrophone,
        format);
    VALIDATE;
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_StopPlayingFileAsMicrophone(int channel)
{
    res = file->StopPlayingFileAsMicrophone(channel);
    VALIDATE;
    return res;
}

int LoganVoiceEngineImpl::LoganVoe_IsPlayingFileAsMicrophone(int channel)
{
    return file->IsPlayingFileAsMicrophone(channel);
}