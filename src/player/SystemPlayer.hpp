#pragma once

#include "IPlayer.hpp"
#include "logger/Logger.hpp"

namespace player
{
   class SystemPlayer : public IPlayer
   {
   public:
       SystemPlayer(Logger& logger, const std::string& filePath);

       void playAudioFile() override;

   private:
       std::string m_filePath;
       Logger& m_logger;
   };
}