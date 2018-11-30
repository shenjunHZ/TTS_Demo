//
// Created by junshen on 9/30/18.
//

#pragma once

#include <queue>
#include "logger/LoggerFwd.hpp"
#include "configurations/AppConfiguration.hpp"
#include "socket/ClientSocket.hpp"
#include "socket/ISocketSysCall.hpp"
#include "socket/IDataListener.hpp"
#include "TimerService.hpp"

namespace applications
{
    class ClientReceiver : public IDataListener
    {
    public:
        ClientReceiver(Logger& logger, const configuration::AppConfiguration& config,
                       const configuration::AppAddresses& appAddress, timerservice::TimerService& timerService);

        void onDataMessage(const std::string& dataMessage);
        std::string getDataMessage();
        void receiveLoop();

    private:
        std::unique_ptr<endpoints::ISocketSysCall> m_socketSysCall;
        std::unique_ptr<endpoints::ClientSocket> m_clientSocket;
        Logger& m_logger;
        std::queue<std::string> m_receiveDatas;

        mutable std::mutex m_mutex;
        std::condition_variable m_condition;
    };
}
