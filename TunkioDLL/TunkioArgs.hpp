#pragma once

#include <string>
#include <any>
#include <typeindex>
#include <array>
#include <vector>

#include "TunkioFileSystem.hpp"

namespace Tunkio::Args
{
    enum class Target : char
    {
        AutoDetect = 'a',
        File = 'f',
        Directory = 'd',
        Volume = 'v'
    };

    enum class Mode : char
    {
        Zeroes = '0',
        Ones = '1',
        LessRandom = 'R',
        MoreRandom = 'r'
    };

    template <typename T>
    constexpr bool TargetFromChar(T c, std::any& m)
    {
        switch (c)
        {
            case static_cast<wchar_t>(Target::AutoDetect) :
                m = Target::AutoDetect;
                return true;
            case static_cast<wchar_t>(Target::File) :
                m = Target::File;
                return true;
            case static_cast<wchar_t>(Target::Directory) :
                m = Target::Directory;
                return true;
            case  static_cast<wchar_t>(Target::Volume) :
                m = Target::Volume;
                return true;
        }

        return false;
    }

    template <typename T>
    constexpr bool ModeFromChar(T c, std::any& m)
    {
        switch (c)
        {
            case static_cast<T>(Mode::Zeroes) :
                m = Mode::Zeroes;
                return true;
            case static_cast<T>(Mode::Ones) :
                m = Mode::Ones;
                return true;
            case static_cast<T>(Mode::LessRandom) :
                m = Mode::LessRandom;
                return true;
            case static_cast<T>(Mode::MoreRandom) :
                m = Mode::MoreRandom;
                return true;
        }

        return false;
    }

    template <typename T>
    constexpr bool BoolFromChar(T c, std::any& m)
    {
        switch (c)
        {
        case 'y':
            m = true;
            return true;
        case 'n':
            m = false;
            return true;
        }

        return false;
    }

    template <typename T>
    class Argument
    {
    public:
        Argument(bool required, const std::basic_string<T>& key, std::any value) :
            Required(required),
            Key(key),
            Type(value.type()),
            m_value(value)
        {
        }

        bool Argument::operator == (const std::basic_string<T>& key) const
        {
            return Key == key;
        }

        bool Argument::operator != (const std::basic_string<T>& key) const
        {
            return Key != key;
        }

        bool Parse(const std::basic_string<T>& value)
        {
            if (Type == typeid(std::basic_string<T>) || Type == typeid(Path))
            {
                m_value = value;
                return true;
            }

            if (Type == typeid(Target))
            {
                if (value.size() != 1)
                {
                    // Argument has incorrect length
                    return false;
                }

                return TargetFromChar(value.front(), m_value);
            }

            if (Type == typeid(Mode))
            {
                if (value.size() != 1)
                {
                    // Argument has incorrect length
                    return false;
                }

                return ModeFromChar(value.front(), m_value);
            }

            if (Type == typeid(bool))
            {
                if (value.size() != 1)
                {
                    // Argument has incorrect length
                    return false;
                }

                return BoolFromChar(value.front(), m_value);
            }

            // Uknown type
            return false;
        }

        template <typename X>
        const X Value() const
        {
            return std::any_cast<X>(m_value);
        }

        const bool Required;
        const std::basic_string<T> Key;
        const std::type_index Type;

    private:
        std::any m_value;
    };
    
    template <typename T>
    bool Parse(std::array<Args::Argument<T>, 4>& arguments, const std::vector<std::basic_string<T>>& rawArgs)
    {
        for (auto& arg : arguments)
        {
            bool found = false;

            for (auto& rawArg : rawArgs)
            {
                const auto mm = std::mismatch(rawArg.cbegin(), rawArg.cend(), arg.Key.cbegin(), arg.Key.cend());
                const std::basic_string<T> key = { rawArg.cbegin(), mm.first };
                const std::basic_string<T> value = { mm.first, rawArg.cend() };

                if (arg != key)
                {
                    continue;
                }

                if (value.empty())
                {
                    return false;
                }

                if (!arg.Parse(value))
                {
                    return false;
                }

                found = true;
            }

            if (arg.Required && !found)
            {
                return false;
            }
        }

        return true;
    }
}