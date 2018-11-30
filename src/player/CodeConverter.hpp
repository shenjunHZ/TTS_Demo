//
// Created by shenjun on 18-10-11.
//

#pragma once

#include <memory>

extern "C"{
#include <iconv.h>
}

namespace player
{
    class CodeConverter
    {
    public:
        CodeConverter(const char* fromCharset, const char* toCharset);

        ~CodeConverter();

        int decodeCoverter(char* input, unsigned int inLen,
                           char* output, unsigned int outLen);

        std::wstring stringToWstring(const std::string& strData);
        std::string wstringToString(const std::wstring& wstrData);

    private:
        iconv_t m_iconv;
    };
}