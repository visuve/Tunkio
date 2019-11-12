#include "PCH.hpp"
#include "TunkioEncoding.hpp"

namespace Tunkio::Encoding
{
    std::wstring ToWide(const char* ansi)
    {
        std::wstring unicode;

        int required = MultiByteToWideChar(CP_UTF8, 0, ansi, -1, nullptr, 0) - 1;

        if (required > 0)
        {
            unicode.resize(static_cast<size_t>(required));
            required = MultiByteToWideChar(CP_UTF8, 0, ansi, -1, &unicode.front(), required);
        }

        return unicode;
    }

    std::wstring ToWide(const std::string& ansi)
    {
        std::wstring unicode;

        int required = MultiByteToWideChar(CP_UTF8, 0, ansi.c_str(), static_cast<int>(ansi.length()), nullptr, 0);

        if (required > 0)
        {
            unicode.resize(static_cast<size_t>(required));
            required = MultiByteToWideChar(CP_UTF8, 0, ansi.c_str(), static_cast<int>(ansi.length()), &unicode.front(), required);
        }

        return unicode;
    }

    /*std::vector<std::wstring> ToWide(const std::vector<std::string>& ansi)
    {
        std::vector<std::wstring> unicode;

        std::transform(ansi.cbegin(), ansi.cend(), std::back_inserter(unicode), [](const std::string& iter)
        {
            return ToWide(iter);
        });

        return unicode;
    }*/

    std::string ToNarrow(const wchar_t* unicode)
    {
        std::string ansi;

        int required = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, nullptr, 0, nullptr, nullptr) - 1;

        if (required > 0)
        {
            ansi.resize(static_cast<size_t>(required));
            required = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, &ansi.front(), required, nullptr, nullptr);
        }

        return ansi;
    }

    std::string ToNarrow(const std::wstring& unicode)
    {
        std::string ansi;

        int required = WideCharToMultiByte(CP_UTF8, 0, unicode.c_str(), static_cast<int>(unicode.length()), nullptr, 0, nullptr, nullptr);

        if (required > 0)
        {
            ansi.resize(static_cast<size_t>(required));
            required = WideCharToMultiByte(CP_UTF8, 0, unicode.c_str(), static_cast<int>(unicode.length()), &ansi.front(), required, nullptr, nullptr);
        }

        return ansi;
    }

    /* std::vector<std::string> ToAnsi(const std::vector<std::wstring>& unicode)
     {
         std::vector<std::string> ansi;
         std::transform(unicode.cbegin(), unicode.cend(), std::back_inserter(ansi), [](const std::wstring& iter)
         {
             return ToAnsi(iter);
         });

         return ansi;
     }*/
}