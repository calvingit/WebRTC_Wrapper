WebRTC_Wrapper
==============

自己封装的WebRTC, 使用的库编译自webrtc的2013-05-23版本,开发环境为Visual Studio 2008。

其实很简单的东西，只是把Voe_Base, Voe_Hardware， Voe_File之类集合在一起。

目前只有语音部分，采集、播放、音量控制、音频编码器已经集成了，网络部分绑定了ip和channel，但没有进行详细测试，扩展自己传输得写那两个回调函数。
