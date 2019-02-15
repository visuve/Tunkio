#pragma once

#include <string>
#include <any>
#include <typeindex>
#include <array>
#include <vector>

namespace Tunkio::Args
{
    enum class Target : char
    {
        AutoDetect = 'a',
        File = 'f',
        Directory = 'd',
        MassMedia = 'm'
    };

    enum class Mode : char
    {
        Zeroes = '0',
        Ones = '1',
        LessRandom = 'R',
        MoreRandom = 'r'
    };

    template <typename C>
    constexpr bool TargetFromChar(C c, std::any& m)
    {
        switch (c)
        {
            case static_cast<C>(Target::AutoDetect) :
                m = Target::AutoDetect;
                return true;
            case static_cast<C>(Target::File) :
                m = Target::File;
                return true;
            case static_cast<C>(Target::Directory) :
                m = Target::Directory;
                return true;
            case  static_cast<C>(Target::MassMedia) :
                m = Target::MassMedia;
                return true;
        }

        return false;
    }

    template <typename C>
    constexpr bool ModeFromChar(C c, std::any& m)
    {
        switch (c)
        {
            case static_cast<C>(Mode::Zeroes) :
                m = Mode::Zeroes;
                return true;
            case static_cast<C>(Mode::Ones) :
                m = Mode::Ones;
                return true;
            case static_cast<C>(Mode::LessRandom) :
                m = Mode::LessRandom;
                return true;
            case static_cast<C>(Mode::MoreRandom) :
                m = Mode::MoreRandom;
                return true;
        }

        return false;
    }

    template <typename C>
    constexpr bool BoolFromChar(C c, std::any& m)
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

    template <typename C>
    class Argument
    {
    public:
        Argument(bool required, const std::basic_string<C>& key, std::any value) :
            Required(required),
            Key(key),
            Type(value.type()),
            m_value(value)
        {
        }

        bool Argument::operator == (const std::basic_string<C>& key) const
        {
            return Key == key;
        }

        bool Argument::operator != (const std::basic_string<C>& key) const
        {
            return Key != key;
        }

        bool Parse(const std::basic_string<C>& value)
        {
            if (Type == typeid(std::basic_string<C>))
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

        template <class T>
        const T Value() const
        {
            return std::any_cast<T>(m_value);
        }

        const bool Required;
        const std::basic_string<C> Key;
        const std::type_index Type;

    private:
        std::any m_value;
    };

    using NarrowArgument = Argument<char>;
    using WideArgument = Argument<wchar_t>;
    
    template <typename C>
    bool Parse(std::array<Args::Argument<C>, 4>& arguments, const std::vector<std::basic_string<C>>& rawArgs)
    {
        for (auto& arg : arguments)
        {
            bool found = false;

            for (auto& rawArg : rawArgs)
            {
                const auto mm = std::mismatch(rawArg.cbegin(), rawArg.cend(), arg.Key.cbegin(), arg.Key.cend());
                const std::basic_string<C> key = { rawArg.cbegin(), mm.first };
                const std::basic_string<C> value = { mm.first, rawArg.cend() };

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