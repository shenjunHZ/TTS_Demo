//
// Created by junshen on 10/1/18.
//
#include "KDTTSService.hpp"

namespace applications
{
    KDTTSService::KDTTSService(Logger& logger, configuration::TTSLoginParams& loginParams,
                               configuration::TTSSessionParams& sessionParams)
        : m_loginParams{loginParams}
        , m_sessionParams{sessionParams}
        , m_logger{logger}
        , m_bLoggerSuccess{false}
    {

    };

    KDTTSService::~KDTTSService()
    {
        if(m_bLoggerSuccess)
        {
            logoutSDK();
        }
    }

    bool KDTTSService::loginSDK()
    {
        int ret = MSP_SUCCESS;
        std::string loginParams = "appid = " + m_loginParams.appid + ", work_dir = " + m_loginParams.work_dir;

        /* 用户登录 */
        ret = MSPLogin(NULL, NULL, loginParams.c_str()); //第一个参数是用户名，第二个参数是密码，第三个参数是登录参数，用户名和密码可在http://www.xfyun.cn注册获取
        if (MSP_SUCCESS != ret)
        {
            LOG_ERROR_MSG("MSPLogin failed, error code: {}.", ret);
            m_bLoggerSuccess = false;
            return false;
        }
        LOG_INFO_MSG(m_logger, "MSPLogin success. login params: {}", loginParams.c_str());
        m_bLoggerSuccess = true;
        return true;
    }

    bool KDTTSService::logoutSDK()
    {
        int ret = MSPLogout();
        if(MSP_SUCCESS != ret) //退出登录
        {
            LOG_ERROR_MSG("MSPLogin failed, error code: %d.", ret);
        }

        m_bLoggerSuccess = false;
        LOG_INFO_MSG(m_logger, "MSPLogout success.");
        return true;
    }

    bool KDTTSService::TTSCompound(const std::string& srcTextContext, const std::string& desFileName)
    {
        int          ret          = -1;
        FILE*        fp           = nullptr;
        const char*  sessionID    = nullptr;
        unsigned int audio_len    = 0;

        configuration::wave_pcm_hdr wav_hdr;

        int synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;

        LOG_INFO_MSG(m_logger, "Start to compound.");

        if (0 == srcTextContext.length() || 0 == desFileName.size())
        {
            LOG_ERROR_MSG("params is error!");
            return false;
        }

        fp = fopen(desFileName.c_str(), "wb");
        if (nullptr == fp)
        {
            LOG_ERROR_MSG("open {} error.", desFileName.c_str());
            return false;
        }
        /* 开始合成 */
        std::string sessionParams = "engine_type = " + m_sessionParams.engine_type +
                                    ", voice_name = " + m_sessionParams.voice_name +
                                    ", text_encoding = " + m_sessionParams.text_encoding +
                                    ", tts_res_path = " + m_sessionParams.tts_res_path +
                                    ", sample_rate = " + std::to_string(m_sessionParams.sample_rate) +
                                    ", speed = " + std::to_string(m_sessionParams.speed) +
                                    ", volume = " + std::to_string(m_sessionParams.volume) +
                                    ", pitch = " + std::to_string(m_sessionParams.pitch) +
                                    ", rdn = " + std::to_string(m_sessionParams.rdn);

        sessionID = QTTSSessionBegin(sessionParams.c_str(), &ret);
        if (MSP_SUCCESS != ret)
        {
            LOG_ERROR_MSG("QTTSSessionBegin failed, error code: {}.", ret);
            fclose(fp);
            return false;
        }

        ret = QTTSTextPut(sessionID, srcTextContext.c_str(), strlen(srcTextContext.c_str()), NULL);
        if (MSP_SUCCESS != ret)
        {
            LOG_ERROR_MSG("QTTSTextPut failed, error code: {}. id: {} Context: {} len: {}",
                          ret, sessionID, srcTextContext.c_str(), strlen(srcTextContext.c_str()));
            QTTSSessionEnd(sessionID, "TextPutError");
            fclose(fp);
            return false;
        }
        //LOG_INFO_MSG(m_logger, "compounding now ...");
        fwrite(&wav_hdr, sizeof(wav_hdr) ,1, fp); //添加wav音频头，使用采样率为16000
        while (true)
        {
            /* 获取合成音频 */
            const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
            if (MSP_SUCCESS != ret)
                break;
            if (NULL != data)
            {
                fwrite(data, audio_len, 1, fp);
                wav_hdr.data_size += audio_len; //计算data_size大小
            }
            if (MSP_TTS_FLAG_DATA_END == synth_status)
                break;
        }

        if (MSP_SUCCESS != ret)
        {
            LOG_ERROR_MSG("QTTSAudioGet failed, error code: {} id: {} Context: {}", ret, sessionID, srcTextContext.c_str());
            QTTSSessionEnd(sessionID, "AudioGetError");
            fclose(fp);
            return false;
        }
        /* 修正wav文件头数据的大小 */
        wav_hdr.size_8 += wav_hdr.data_size + (sizeof(wav_hdr) - 8);

        /* 将修正过的数据写回文件头部,音频文件为wav格式 */
        fseek(fp, 4, 0);
        fwrite(&wav_hdr.size_8, sizeof(wav_hdr.size_8), 1, fp); //写入size_8的值
        fseek(fp, 40, 0); //将文件指针偏移到存储data_size值的位置
        fwrite(&wav_hdr.data_size, sizeof(wav_hdr.data_size), 1, fp); //写入data_size的值

        fclose(fp);
        fp = nullptr;
        /* 合成完毕 */
        ret = QTTSSessionEnd(sessionID, "Normal");
        if (MSP_SUCCESS != ret)
        {
            LOG_INFO_MSG(m_logger, "QTTSSessionEnd failed, error code: {}.",ret);
        }

        LOG_INFO_MSG(m_logger, "Compound finish.");
        return true;
    }
}