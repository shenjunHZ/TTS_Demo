//
// Created by shenjun on 18-12-2.
//

#pragma once

namespace player
{
    class IPlayer
    {
    public:
        virtual ~IPlayer() = default;
        virtual void playAudioFile() = 0;
    };
}
