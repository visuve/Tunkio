#pragma once

#include <algorithm>

namespace Tunkio::Encoding
{
    std::wstring ToWide(const char* narrow);
    std::wstring ToWide(const std::string& narrow);
    // std::vector<std::wstring> ToWide(const std::vector<std::string>& ansi);

    std::string ToNarrow(const wchar_t* wide);
    std::string ToNarrow(const std::wstring& wide);
    // std::vector<std::string> ToNarrow(const std::vector<std::wstring>& unicode);
};