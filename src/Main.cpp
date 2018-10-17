#include <stdlib.h>
#include <string>
#include <boost/program_options.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include "logger/Logger.hpp"
#include "applications/AppInstance.hpp"
#include "configurations/ConfigurationLoader.hpp"

 void parseProgramOptions(int argc, char** argv, boost::program_options::variables_map& cmdParams)
 {
     namespace po = boost::program_options;
     {
         po::options_description optDescr("General");
         {
             try
             {
                 auto defaultConfigFilePath = po::value<std::string>()->default_value("configuration.ini");

                 optDescr.add_options()("config-file,c", defaultConfigFilePath, "Configuration file path")(
                     "help,h", "Prints this help message");
             }
             catch (boost::bad_lexical_cast& e)
             {
                 LOG_ERROR_MSG("parseProgramOptions:lexical_cast Failed in  default_value");
                 std::cerr << e.what() << std::endl;
             }
         }

         try
         {
             po::store(po::parse_command_line(argc, argv, optDescr), cmdParams);
             po::notify(cmdParams);
         }
         catch (const po::error& e)
         {
             std::cerr << e.what() << std::endl;
             std::exit(EXIT_FAILURE);
         }

         if (cmdParams.count("help") != 0u)
         {
             std::cout << optDescr << std::endl;
             std::exit(EXIT_SUCCESS);
         }
     } // namespace po=boost::program_options;
 }

void runApp(const configuration::AppConfiguration& configParams, const configuration::AppAddresses& address)
{
    auto& logger = logger::getLogger();
    LOG_INFO_MSG(logger, "Start to run app");

    applications::AppInstance appInstance(logger, configParams, address);

    //std::atomic_bool keepRunning(true);
    appInstance.loopFuction();
}

int main(int argc, char* argv[])
{
    boost::program_options::variables_map cmdParams;
    parseProgramOptions(argc, argv, cmdParams);

    try
    {
       configuration::AppConfiguration configParams =
           configuration::loadFromIniFile(cmdParams["config-file"].as<std::string>());
        const auto addresses = configuration::getAppAddresses(configParams);
        runApp(configParams, addresses);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR_MSG(boost::diagnostic_information(e));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

