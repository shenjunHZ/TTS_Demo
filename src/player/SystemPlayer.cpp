#include "SystemPlayer.hpp"

namespace
{
    constexpr int bashError = 127;
}

namespace player
{
    SystemPlayer::SystemPlayer(Logger& logger, const std::string& filePath)
        : m_filePath{filePath}
        , m_logger{logger}
    {

    }

    void SystemPlayer::playAudioFile()
    {
        if(not m_filePath.empty())
        {
            std::string strCommend = "play " + m_filePath;
            int ret = system(strCommend.c_str());
            if(ret < 0)
            {
                LOG_ERROR_MSG("system call play failed : {}", std::strerror(ret));
            }
            else if(bashError == ret)
            {
                LOG_ERROR_MSG("bash run failed : {}", std::strerror(ret));
            }
        }
    }
}

