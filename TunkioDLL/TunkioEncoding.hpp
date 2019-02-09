#pragma once

#include <algorithm>

namespace Tunkio::Encoding
{
    std::wstring AnsiToUnicode(const char* ansi);
    std::wstring AnsiToUnicode(const std::string& ansi);
    std::vector<std::wstring> AnsiToUnicode(const std::vector<std::string>& ansi);

    std::string UnicodeToAnsi(const wchar_t* unicode);
    std::string UnicodeToAnsi(const std::wstring& unicode);
    std::vector<std::string> UnicodeToAnsi(const std::vector<std::wstring>& unicode);
};