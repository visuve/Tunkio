#pragma once

#include <string>
#include <any>
#include <typeindex>
#include <array>
#include <vector>

namespace Tunkio::Args
{
    enum class Target : wchar_t
    {
        AutoDetect = 'a',
        File = 'f',
        Directory = 'd',
        Volume = 'v'
    };

    enum class Mode : wchar_t
    {
        Zeroes = '0',
        Ones = '1',
        LessRandom = 'R',
        MoreRandom = 'r'
    };

    class Argument
    {
    public:
        Argument(bool required, const std::wstring& key, std::any value);
        bool Parse(const std::wstring& from);

        bool operator == (const std::wstring& key) const;
        bool operator != (const std::wstring& key) const;

        template <typename T>
        const T Value() const
        {
            return std::any_cast<T>(m_value);
        }

        const bool Required;
        const std::wstring Key;
        const std::type_index Type;

    private:
        std::any m_value;
    };

    extern std::array<Args::Argument, 3> Arguments;

    bool Parse(const std::vector<std::wstring>& rawArgs);
}