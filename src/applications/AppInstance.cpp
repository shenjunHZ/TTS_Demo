//
// Created by shenjun on 18-9-28.
//
#include <string>
#include <thread>
#include <unistd.h>
#include "AppInstance.hpp"
#include "DefaultTimerService.hpp"

namespace
{
    std::string getKDAppId(const configuration::AppConfiguration& config)
    {
        if (config.find(configuration::kdAppId) != config.end())
        {
            return config[configuration::kdAppId].as<std::string>();
        }
        return "5ba5af09";
    }

    std::string getTTSWorkDir(const configuration::AppConfiguration& config)
    {
        if(config.find(configuration::ttsWorkDir) != config.end())
        {
            return config[configuration::ttsWorkDir].as<std::string>();
        }
        return ".";
    }

    std::string getWavFileName(const configuration::AppConfiguration& config)
    {
        if(config.find(configuration::wavFileName) != config.end())
        {
            return config[configuration::wavFileName].as<std::string>();
        }
        return "ttsAudio.wav";
    }

    std::string getWavFilePath(const configuration::AppConfiguration& config)
    {
        if(config.find(configuration::wavFilePath) != config.end())
        {
            return config[configuration::wavFilePath].as<std::string>();
        }
        return ".";
    }

    std::atomic_bool keep_running(true);
}

namespace applications
{
    AppInstance::AppInstance(Logger& logger, const configuration::AppConfiguration& config, const configuration::AppAddresses& appAddress)
        : m_ioService{std::make_unique<timerservice::IOService>()}
        , m_timerService{std::make_unique<timerservice::DefaultTimerService>(*m_ioService)}
        , m_clientReceiver{logger, config, appAddress, *m_timerService}

    {
        std::string strDir = getTTSWorkDir(config);
        std::string strId = getKDAppId(config);
        configuration::TTSLoginParams loginParams {strId, strDir};
        configuration::TTSSessionParams sessionParams;
        std::string strFileName = getWavFileName(config);
        m_kdTTSService = std::make_unique<KDTTSService>(logger, loginParams, sessionParams, strFileName);
        configuration::wave_pcm_hdr wavHdr;
        std::string strFilePath = getWavFilePath(config);
        m_alsaPlayer = std::make_unique<player::AlsaPlayer>(logger, wavHdr, strFilePath + strFileName);//loginParams.work_dir + "/ttsAudio.wav");

        initService();
    }

    AppInstance::~AppInstance()
    {
        if(m_threadTTSService.joinable())
        {
            m_threadTTSService.join();
        }
        if(m_threadAudioPLay.joinable())
        {
            m_threadAudioPLay.join();
        }

        if(nullptr != m_ioService)
        {
            m_ioService->stop();
        }
    }

    void AppInstance::initService()
    {
        if(m_kdTTSService)
        {
            m_bLoginSDK = m_kdTTSService->loginSDK();
        }
    }

    void AppInstance::threadTTSService()
    {
        // send data to TTS
        while(keep_running)
        {
            std::string dataMessage = m_clientReceiver.getDataMessage();

            if(0 < dataMessage.length())
            {
                //LOG_DEBUG_MSG("tts data : {} length: {}", dataMessage.c_str(), dataMessage.length());
                if(m_kdTTSService->TTSCompound(dataMessage))
                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    m_bAudioData = true;
                    m_alsaPlayer->playAudioFile();
                }
            }
            else
            {
                usleep(1000*10);
            }
        }
    }

    void AppInstance::threadAudioPlay()
    {
        while(keep_running)
        {
            if (m_bAudioData and m_alsaPlayer)
            {
                m_alsaPlayer->playAudioFile();
                std::lock_guard<std::mutex> lock(m_mutex);
                m_bAudioData = false;
            }
            else
            {
                usleep(1000 * 10);
            }
        }
    }

    void AppInstance::loopFuction()
    {
        if(m_bLoginSDK)
        {
            m_threadTTSService = std::thread(&AppInstance::threadTTSService, this);
        }

        m_clientReceiver.receiveLoop();

        //m_threadAudioPLay = std::thread(&AppInstance::threadAudioPlay, this);
    }
} // namespace applications