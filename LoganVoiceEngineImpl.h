/*
**Copyright (c) 2013 Calvin Cheung. All Rights Reserved.
**
**Use of this source code is governed by a BSD-style license.
**
**This is a simple wrapper for WebRTC. I used it in the Video Conference project.
**
**URL:https://github.com/calvingit/WebRTC_Wrapper/
**
**File: LoganVoiceEngineImpl.h
**
**Author: Calvin Cheung, Chinna.
**
**Update Date: 2013-5-21
*/


#ifndef __LOGAN_VOICE_ENGINE_IMPL_H__
#define __LOGAN_VOICE_ENGINE_IMPL_H__


#include <cstdlib>
#include <vector>
#include "common_types.h"
#include "LoganVoiceEngine.h"
#include "voe_errors.h"
#include "voe_base.h"
#include "voe_codec.h"
#include "voe_volume_control.h"
#include "voe_audio_processing.h"
#include "voe_file.h"
#include "voe_hardware.h"
#include "voe_network.h"
#include "engine_configurations.h"


using namespace webrtc;
//using namespace test;

const int VOE_API_MAXCHANNELS = 8;
const int RTP_PAYLOAD_NAME_LEN = 32;
const int DEVICE_MAX_LEN = 128;

class LoganInStream:public InStream
{
public:
    virtual int Read(void *buf,int len);
    LoganReadStreamCallBack pRead;
    virtual ~LoganInStream() {}
    LoganInStream() {} 
};

int LoganInStream::Read(void *buf,int len)
{
    return pRead(buf, len);
}

class LoganOutStream:public OutStream
{
public:
    virtual bool Write(const void *buf,int len);
    LoganWriteStreamCallBack pWrite;

    virtual ~LoganOutStream() {}
    LoganOutStream() {}
};

bool LoganOutStream::Write(const void *buf,int len)
{
    return pWrite(buf, len);
}


class LoganVoiceEngineImpl:public LoganVoiceEngine
{
private:
    VoiceEngine* m_voe;
    VoEBase* base;
    VoECodec* codec;
    VoEVolumeControl* volume;
    VoEAudioProcessing* apm;
    VoENetwork *netw;
    VoEFile *file;
    VoEHardware *hardware;

    LoganInStream _inStream;
    LoganOutStream _outStream;

public:

    LoganVoiceEngineImpl();
    virtual ~LoganVoiceEngineImpl();

    virtual int LoganVoe_Init();
    virtual void LoganVoe_UnInit();
    virtual int LoganVoe_CreateChannel();
    virtual int LoganVoe_DeleteChannel(int channel);
    virtual int LoganVoe_SetSendDestination(int channelId, char* ip, int port);
    virtual int LoganVoe_SetLocalReceiver(int channelId, int port);

    //获取编解码器的数目
    virtual int LoganVoe_GetNumsOfCodec();
    //获取指定索引的编解码器
    virtual int LoganVoe_GetSpecCodec(int index, LoganCodec &lc);
    //设置指定通道的编码器
    virtual int LoganVoe_SetSendCodec(int channelid, LoganCodec lc);

    //获取录音设备的数目
    virtual int LoganVoe_GetNumOfRecordingDevices();
    //获取播放设备的数目
    virtual int LoganVoe_GetNumOfPlayoutDevices();
    //获取指定索引的播放设备的名字
    virtual int LoganVoe_GetPlayoutDeviceName(int index,char nameUTF8[128], char guidUTF8[128]);
    //获取指定索引的录音设备的名字
    virtual int LoganVoe_GetRecordingDeviceName(int index, char nameUTF8[128], char guidUTF8[128]);
    //设置播放设备
    virtual int LoganVoe_SetPlayoutDevice(int idx);
    //设置录音设备
    virtual int LoganVoe_SetRecordingDevice(int idx);

    //语音处理设置
    virtual int LoganVoe_SetVADStatus(int channelsid, bool b, int mode);
    virtual int LoganVoe_SetAgcStatus(bool b, int mode);
    virtual int LoganVoe_SetEcStatus(bool b, int mode);
    virtual int LoganVoe_SetNsStatus(bool b, int mode);
    virtual int LoganVoe_GetVADStatus(int channelsid, bool &b, int &mode);
    virtual int LoganVoe_GetAgcStatus(bool &b, int &mode);
    virtual int LoganVoe_GetEcStatus(bool &b, int &mode);
    virtual int LoganVoe_GetNsStatus(bool &b, int &mode);

    virtual int LoganVoe_StartReceive(int channelid);
    virtual int LoganVoe_StartPlayout(int channelid);
    virtual int LoganVoe_StartSend(int channelid);
    virtual int LoganVoe_StartRecordingMicrophone(LoganWriteStreamCallBack lwscb);

    virtual int LoganVoe_StartRecordingMicrophone(const char* fileNameUTF8, 
        LoganCodec* codec = NULL);

    virtual int LoganVoe_StartPlayingFileLocally(int channelid,
        LoganReadStreamCallBack lrscb);

    virtual int LoganVoe_StartPlayingFileLocally(int channelid,
        const char fileNameUTF8[1024],
        int samplerate,//(8000, 16000, 32000)
        bool loop = false);

    virtual int LoganVoe_IsPlayingFileLocally(int channel);

    virtual int LoganVoe_StartRecordingPlayout(int channel, const char* fileNameUTF8,
        LoganCodec *codec = NULL);

    virtual int LoganVoe_StartPlayingFileAsMicrophone(
        int channel,
        const char fileNameUTF8[1024],
        int sampleRate,
        bool loop = false ,
        bool mixWithMicrophone = false);

    virtual int LoganVoe_StartPlayingFileAsMicrophone(
        int channel,
        LoganReadStreamCallBack read,
        int sampleRate,
        bool mixWithMicrophone = false);

    virtual int LoganVoe_StopPlayingFileAsMicrophone(int channel);

    virtual int LoganVoe_IsPlayingFileAsMicrophone(int channel);

    virtual int LoganVoe_StopReceive(int channelid);
    virtual int LoganVoe_StopPlayout(int channelid);
    virtual int LoganVoe_StopSend(int channelid);
    virtual int LoganVoe_StopRecordingPlayout(int channel);
    virtual int LoganVoe_StopPlayingFileLocally(int channelid);
    virtual int LoganVoe_StopRecordingMicrophone();

    virtual unsigned int LoganVoe_GetMicVolume();
    virtual unsigned int LoganVoe_GetSpeakerVolume();
    virtual int LoganVoe_SetMicVolume(unsigned int vol);
    virtual int LoganVoe_SetSpeakerVolume(unsigned int vol);
    virtual int LoganVoe_SetInputMute(int channel, bool enable);
    virtual int LoganVoe_GetInputMute(int channel, bool& enabled);
    virtual int LoganVoe_SetSystemOutputMute(bool enable);
    virtual int LoganVoe_GetSystemOutputMute(bool &enabled);
    virtual int LoganVoe_SetSystemInputMute(bool enable);
    virtual int LoganVoe_GetSystemInputMute(bool& enabled);
    virtual int LoganVoe_GetSpeechInputLevel(unsigned int& level);
    virtual int LoganVoe_GetSpeechOutputLevel(int channel, unsigned int& level);

};
#endif //__LOGAN_VOICE_ENGINE_IMPL_H__