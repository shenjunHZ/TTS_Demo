//
// Created by shenjun on 18-10-11.
//
#include <codecvt>
#include "CodeConverter.hpp"
#include "logger/Logger.hpp"

namespace player
{
    CodeConverter::CodeConverter(const char* fromCharset, const char* toCharset)
    {
        m_iconv = iconv_open(toCharset, fromCharset);
        if(0 == m_iconv)
        {
            LOG_ERROR_MSG("Iconv open filed.");
        }
    }

    CodeConverter::~CodeConverter()
    {
        iconv_close(m_iconv);
    }

    int CodeConverter::decodeCoverter(char* input, long unsigned int inLen,
                      char* output, long unsigned int outLen)
    {
        return iconv(m_iconv, &input, static_cast<size_t*>(&inLen),
        &output, static_cast<size_t*>(&outLen));
    }

    std::wstring CodeConverter::stringToWstring(const std::string& strData)
    {
        using convert_typeX = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_typeX, wchar_t> converterX;

        return converterX.from_bytes(strData);

    }

    std::string CodeConverter::wstringToString(const std::wstring& wstrData)
    {
        using convert_typeX = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_typeX, wchar_t> converterX;

        return converterX.to_bytes(wstrData);

    }
}