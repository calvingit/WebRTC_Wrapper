//#ifdef _DEBUG

#include <iostream>


#include "webrtc_wrapper.h"

using namespace std;

LoganVoiceEngine *lve;
FILE *rfile = NULL;
FILE *wfile = NULL;

void PrintCodecs();//打印编码器信息
void PrintDevices();//打印设备信息
void ChanTransTest();//走webrtc的网络
void RunMyNetwork();//自己接管发送接收，或者读写文件

int _stdcall read(void *buf,int len)
{
    /*如果是网络的话，这里就是从缓冲区获取数据，
    每次len个字节，如果缓冲区没有数据，设为NULL，
    但是返回值不要return 0,还是return len.
    */
    fread(buf,1, len, rfile);

    return len;
}
//采集的数据写到网络或文件
bool _stdcall write(const void *buf,int len)
{
    /*
    如果是网络，这里直接发送就可以了，编码的话另外处理
    */
    fwrite(buf, 1, len, wfile);

    return true;
}

void PrintOptions()
{
    cout << "webrtc wrapper test" <<endl;
    cout << "1. 打印编码器信息"<<endl;
    cout << "2. 打印音频设备信息"<<endl;
    cout << "3. 网络测试"<<endl;
    cout << "4. 文件测试(或跑自己的网络)"<<endl;
    cout << "0. 结束"<<endl;
    cout << "    请选择测试项目:"<<endl;
}

int main(int argc, char **argv)
{

    PrintOptions();
    int i = -1;
    cin >> i;
    while (i != 0)
    {
        switch(i)
        {
        case 1:PrintCodecs();break;
        case 2:PrintDevices();break;
        case 3:ChanTransTest();break;
        case 4:RunMyNetwork();break;
        default:break;
        }
        
        PrintOptions();
        cin >> i;
    }

    return 0;
}

void PrintCodecs()
{
    lve = LoganVoiceEngine::Create();
    LoganCodec cinst;
    for (int i = 0; i < lve->LoganVoe_GetNumsOfCodec(); i++) {
        lve->LoganVoe_GetSpecCodec(i, cinst);

        if (strncmp(cinst.plname, "ISAC", 4) == 0 && cinst.plfreq == 32000) {
            printf("%i. ISAC-swb pltype:%i plfreq:%i channels:%i\n", i, cinst.pltype,
                cinst.plfreq, cinst.channels);
        } else if (strncmp(cinst.plname, "ISAC", 4) == 0 && cinst.plfreq == 48000) {
            printf("%i. ISAC-fb pltype:%i plfreq:%i channels:%i\n", i, cinst.pltype,
                cinst.plfreq, cinst.channels);
        } else {
            printf("%i. %s pltype:%i plfreq:%i channels:%i\n", i, cinst.plname,
                cinst.pltype, cinst.plfreq, cinst.channels);
        }
    }

    cout << "输入“0”结束:";
    while(true)
    {
        int i;
        cin >>i;
        if (i == 0)
        {
            break;
        }
    }
    LoganVoiceEngine::Destroy(lve);
}

void ChanTransTest()
{
    lve = LoganVoiceEngine::Create();

    //PrintCodecs();
    int channelid = lve->LoganVoe_CreateChannel();

    //设置编码器
    LoganCodec codec;
    lve->LoganVoe_GetSpecCodec(0, codec);
    lve->LoganVoe_SetSendCodec(channelid,codec);//设置编码器
    cout << "输入本地端口:";
    int localport, remoteport;
    char strip[128] = {0};
    cin >>localport;
    cout <<"输入对方IP:";
    cin >> strip;
    cout <<"输入对方端口:";
    cin>> remoteport;


    lve->LoganVoe_SetSendDestination(channelid, strip, remoteport);
    lve->LoganVoe_SetLocalReceiver(channelid, localport);

    lve->LoganVoe_SetEcStatus(true);//开启回音消除
    lve->LoganVoe_SetNsStatus(true);//开启噪音抑制

    lve->LoganVoe_StartPlayout(channelid);
    lve->LoganVoe_StartSend(channelid);
    lve->LoganVoe_StartReceive(channelid);

    cout << "输入“0”结束:";
    while(true)
    {
        int i;
        cin >>i;
        if (i == 0) //输入0就结束测试
        {
            break;
        }
    }

    lve->LoganVoe_StopReceive(channelid);
    lve->LoganVoe_StopSend(channelid);
    lve->LoganVoe_StopPlayout(channelid);
    lve->LoganVoe_DeleteChannel(channelid);

    LoganVoiceEngine::Destroy(lve);
}

//一律用16000采样率，16位，单声道
void RunMyNetwork()
{   
    string strFileIn = "audio_short16.pcm";
    string strFileOut = "my_recorded.pcm";
    rfile = fopen((strFileIn).c_str(), "rb");
    wfile = fopen((strFileOut).c_str(),"wb");

    lve = LoganVoiceEngine::Create();
    int id = lve->LoganVoe_CreateChannel();
    lve->LoganVoe_StartPlayout(id);

    lve->LoganVoe_StartPlayingFileLocally(id,read);

    lve->LoganVoe_StartRecordingMicrophone(write);
    cout << "输入“0”结束:";
    while(true)
    {
        int i;
        cin >>i;
        if (i == 0) 
        {
            break;
        }
    }

    LoganVoiceEngine::Destroy(lve);
}

void PrintDevices()
{
    lve = LoganVoiceEngine::Create();
    char name[128] = {0};
    char guid[128] = {0};
    for (int i = 0; i < lve->LoganVoe_GetNumOfPlayoutDevices(); i++)
    {
        lve->LoganVoe_GetPlayoutDeviceName(i, name, guid);
        cout <<"Play Devices: "<<name<<endl;
    }

    for(int j = 0; j < lve->LoganVoe_GetNumOfRecordingDevices(); j++)
    {
        lve->LoganVoe_GetRecordingDeviceName(j, name, guid);
        cout <<"Record Devices:"<<name<<endl;
    }
    cout << "输入“0”结束:";
    while(true)
    {
        int i;
        cin >>i;
        if (i == 0) 
        {
            break;
        }
    }

    LoganVoiceEngine::Destroy(lve);
}


//#endif