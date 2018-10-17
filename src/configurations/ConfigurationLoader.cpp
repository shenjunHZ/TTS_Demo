//
// Created by junshen on 10/8/18.
//

#include "ConfigurationLoader.hpp"
#include "AppConfiguration.hpp"

namespace
{
    boost::program_options::options_description createProgramOptionsDescription()
    {
        auto description = boost::program_options::options_description();
        using boost::program_options::value;

        description.add_options()
            (configuration::serverPort, value<unsigned>()->default_value(8080), "tcp server port")
            (configuration::clientPort, value<unsigned>()->default_value(8080), "tcp local port")
            (configuration::serverAddress, value<std::string>()->default_value("192.168.1.10"), "Server Address")
            (configuration::clientAddress, value<std::string>()->default_value("192.168.1.11"), "Client Address")
            (configuration::kdAppId, value<std::string>()->default_value("5ba5af09"), "KD TTS Register Id")
            (configuration::connectTimer, value<unsigned>()->default_value(10), "connect timer period")
            (configuration::ttsWorkDir, value<std::string>()->default_value("."), "tts work dir");

        return description;
    }

    boost::program_options::variables_map parseProgramOptions(std::ifstream& boostOptionsStream)
    {
        namespace po = boost::program_options;

        po::variables_map appConfig;
        po::store(po::parse_config_file(boostOptionsStream, createProgramOptionsDescription()), appConfig);
        try
        {
            po::notify(appConfig);
        }
        catch (const po::error& e)
        {
            LOG_ERROR_MSG("Parsing the config file failed: {}", e.what());
            throw;
        }
        return appConfig;
    }
} // namespace

namespace configuration
{
    AppConfiguration loadFromIniFile(const std::string& configFilename)
    {
        std::ifstream configFileStream{configFilename};
        AppConfiguration configuration{{parseProgramOptions(configFileStream)}};

        return configuration;
    }

    AppAddresses getAppAddresses(const boost::program_options::variables_map& cmdParams)
    {
        return AppAddresses{cmdParams[serverAddress].as<std::string>(),
                                           cmdParams[clientAddress].as<std::string>(),
                                           cmdParams[serverPort].as<unsigned>(),
                                           cmdParams[clientPort].as<unsigned>()};

    }

} // namespace configuration
