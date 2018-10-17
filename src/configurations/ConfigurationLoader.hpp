//
// Created by junshen on 10/8/18.
//

#pragma once

#include <string>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include "AppConfiguration.hpp"

namespace configuration
{
    using AppConfiguration = boost::program_options::variables_map;

    AppConfiguration loadFromIniFile(const std::string& configFilename);

    AppAddresses getAppAddresses(const boost::program_options::variables_map& cmdParams);
}
