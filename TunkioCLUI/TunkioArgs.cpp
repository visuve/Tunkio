#include "PCH.hpp"
#include "TunkioArgs.hpp"

namespace Tunkio::Args
{
    constexpr bool TargetFromChar(wchar_t c, std::any& m)
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

    constexpr bool ModeFromChar(wchar_t c, std::any& m)
    {
        switch (c)
        {
            case static_cast<wchar_t>(Mode::Zeroes) :
                m = Mode::Zeroes;
                return true;
            case static_cast<wchar_t>(Mode::Ones) :
                m = Mode::Ones;
                return true;
            case static_cast<wchar_t>(Mode::LessRandom) :
                m = Mode::LessRandom;
                return true;
            case static_cast<wchar_t>(Mode::MoreRandom) :
                m = Mode::MoreRandom;
                return true;
        }

        return false;
    }

    Argument::Argument(bool required, const std::wstring& key, std::any value) :
        Required(required),
        Key(key),
        Value(value)
    {
    }

    bool Argument::Parse(const std::wstring& from)
    {
        if (Value.type() == typeid(std::wstring))
        {
            Value = from;
            return true;
        }

        if (Value.type() == typeid(Target))
        {
            if (from.size() != 1)
            {
                // Argument has incorrect length
                return false;
            }

            return TargetFromChar(from.front(), Value);
        }

        if (Value.type() == typeid(Mode))
        {
            if (from.size() != 1)
            {
                // Argument has incorrect length
                return false;
            }

            return ModeFromChar(from.front(), Value);
        }

        // Uknown type
        return false;
    }

    bool Argument::operator == (const std::wstring& key) const
    {
        return Key == key;
    }

    bool Argument::operator != (const std::wstring& key) const
    {
        return Key != key;
    }
    
    bool Parse(const std::vector<std::wstring>& rawArgs)
    {
        for (Argument& arg : Arguments)
        {
            bool found = false;

            for (auto& rawArg : rawArgs)
            {
                const auto mm = std::mismatch(rawArg.cbegin(), rawArg.cend(), arg.Key.cbegin(), arg.Key.cend());

                const std::wstring key = { rawArg.cbegin(), mm.first };
                const std::wstring value = { mm.first, rawArg.cend() };

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