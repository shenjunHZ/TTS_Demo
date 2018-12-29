//
// Created by shenjun on 18-9-25.
//

#pragma once

#include <string>
#include <fstream>
#include "logger/Logger.hpp"

#define ADDRESSES_CONFIG_PREFIX "address"
#define REGISTER_CONFIG_PREFIX "register"
#define TIMER_CONFIG_PREFIX "timer"
#define ALSA_CONFIG_PREFIX "alsa"

namespace configuration
{
    constexpr auto serverAddress = ADDRESSES_CONFIG_PREFIX ".serverAddress";
    constexpr auto clientAddress = ADDRESSES_CONFIG_PREFIX ".clientAddress";
    constexpr auto serverPort    = ADDRESSES_CONFIG_PREFIX ".serverPort";
    constexpr auto clientPort    = ADDRESSES_CONFIG_PREFIX ".clientPort";
    constexpr auto kdAppId       = REGISTER_CONFIG_PREFIX ".kdAppId";
    constexpr auto ttsWorkDir    = REGISTER_CONFIG_PREFIX ".ttsWorkDir";
    constexpr auto connectTimer  = TIMER_CONFIG_PREFIX ".connectTimer";
    constexpr auto wavFilePath   = ALSA_CONFIG_PREFIX ".wavFilePath";
    constexpr auto wavFileName   = ALSA_CONFIG_PREFIX ".wavFileName";
    constexpr auto enablePlayTool = ALSA_CONFIG_PREFIX ".enablePlayTool";
}


namespace configuration
{
    struct AppAddresses
    {
        std::string serverAddress;
        std::string clientAddress;
        unsigned int serverPort;
        unsigned int clientPort;
    };

    struct TcpConfiguration
    {

    };

    // session params according to the sdk
    struct TTSLoginParams
    {
        std::string appid;
        std::string work_dir;

        TTSLoginParams()
        {
            appid = "5ba5af09";
            work_dir = ".";
        }

        TTSLoginParams(std::string& id, std::string& dir)
        {
            appid = id;
            work_dir = dir;
        }
    };

/*
* rdn:           合成音频数字发音方式
* volume:        合成音频的音量
* pitch:         合成音频的音调
* speed:         合成音频对应的语速
* voice_name:    合成发音人
* sample_rate:   合成音频采样率
* text_encoding: 合成文本编码格式
*
*/
    struct TTSSessionParams
    {
        std::string engine_type;
        std::string voice_name;
        std::string text_encoding;
        std::string tts_res_path;
        int sample_rate;
        int speed;
        int volume;
        int pitch;
        int rdn;

        TTSSessionParams()
        {
            engine_type = "local";
            voice_name = "xiaoyan";
            text_encoding = "UTF8";
            tts_res_path = "fo|res/tts/xiaoyan.jet;fo|res/tts/common.jet";
            sample_rate = 16000;
            speed = 50;
            volume = 50;
            pitch = 50;
            rdn = 2;
        }
    };

    /* wav音频头部格式 */
    typedef struct _wave_pcm_hdr
    {
        char            riff[4];                // = "RIFF"
        int             size_8;                 // = FileSize - 8
        char            wave[4];                // = "WAVE"
        char            fmt[4];                 // = "fmt "
        int             fmt_size;               // = 下一个结构体的大小 : 16

        short int       format_tag;             // = PCM : 1
        short int       channels;               // = 通道数 : 1
        int             samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
        int             avg_bytes_per_sec;      // = 每秒字节数 : samples_per_sec * bits_per_sample / 8
        short int       block_align;            // = 每采样点字节数 : wBitsPerSample / 8
        short int       bits_per_sample;        // = 量化比特数: 8 | 16

        char            data[4];                // = "data";
        int             data_size;              // = 纯数据长度 : FileSize - 44

        _wave_pcm_hdr()
        : riff{'R', 'I', 'F', 'F' }
        , wave{'W', 'A', 'V', 'E'}
        , fmt{'f', 'm', 't', ' '}
        , data{'d', 'a', 't', 'a'}
        {
            size_8 = 0;
            fmt_size = 16;
            format_tag = 1;
            channels = 1;
            samples_per_sec = 16000;
            avg_bytes_per_sec = 32000;
            block_align = 2;
            bits_per_sample = 16;
            data_size = 0;
        }
    } wave_pcm_hdr;

    typedef struct _wave_format
    {
        int dwSize;
        short int wFormatTag;
        short int wChannels;
        int dwSamplesPerSec;
        int	dwAvgBytesPerSec;
        short int wBlockAlign;
        short int wBitsPerSample;
    } wave_format;

}
