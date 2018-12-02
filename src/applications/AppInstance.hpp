//
// Created by shenjun on 18-9-28.
//

#pragma once

#include <memory>
#include <mutex>
#include "logger/Logger.hpp"
#include "configurations/AppConfiguration.hpp"
#include "ClientReceiver.hpp"
#include "KDTTSService.hpp"
#include "IOService.hpp"
#include "TimerService.hpp"
#include "player/AlsaPlayer.hpp"

namespace applications
{
    class AppInstance final
    {
    public:
        AppInstance(Logger& logger, const configuration::AppConfiguration& config, const configuration::AppAddresses& appAddress);
        ~AppInstance();

        void loopFuction();

    private:
        void initService();
        void threadTTSService();
        void threadAudioPlay();

    private:
        bool m_bLoginSDK{false};
        bool m_bConnectServer{false};
        bool m_bAudioData{false};
        std::unique_ptr<timerservice::IOService> m_ioService;
        std::unique_ptr<timerservice::TimerService> m_timerService;
        std::unique_ptr<ITTSService> m_kdTTSService{nullptr};
        std::unique_ptr<player::IPlayer> m_alsaPlayer{nullptr};
        std::mutex m_mutex;

        ClientReceiver m_clientReceiver;
        std::thread m_threadTTSService;
        std::thread m_threadAudioPLay;
    };
}