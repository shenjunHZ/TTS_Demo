//
// Created by junshen on 10/1/18.
//

#pragma once

namespace applications
{
    class ITTSService
    {
    public:
        virtual ~ITTSService() = default;

        virtual bool loginSDK() = 0;
        virtual bool logoutSDK() = 0;
        virtual bool TTSCompound(const std::string& srcTextContext) = 0;
    };
}
