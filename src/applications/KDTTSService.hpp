//
// Created by junshen on 10/1/18.
//

#pragma once

#include <string>
#include "configurations/AppConfiguration.hpp"
#include "logger/Logger.hpp"
#include "configurations/ConfigurationLoader.hpp"
#include "ITTSService.hpp"

extern "C" {
#include "qtts.h"
#include "msp_cmn.h"
#include "msp_errors.h"
}

namespace applications
{
    class KDTTSService : public ITTSService
    {
    public:
        KDTTSService(Logger& logger, configuration::TTSLoginParams& loginParams,
                     configuration::TTSSessionParams& sessionParams, const std::string& fileName);
        ~KDTTSService();

        bool loginSDK();
        bool logoutSDK();
        bool TTSCompound(const std::string& srcTextContext);

    private:
        std::string m_sessionID{};
        configuration::TTSLoginParams m_loginParams;
        configuration::TTSSessionParams m_sessionParams;

        Logger& m_logger;
        bool m_bLoggerSuccess;
        std::string m_fileName;
    };
}
