#include "PCH.hpp"
#include "TunkioArgs.hpp"

namespace Tunkio::Args
{
    bool Argument::Parse(const std::string& value)
    {
        if (Type == typeid(std::string))
        {
            m_value = std::make_any<std::string>(value);
            return true;
        }

        if (Type == typeid(std::filesystem::path))
        {
            m_value = std::make_any<std::filesystem::path>(value);
            return true;
        }

        if (Type == typeid(TunkioTarget))
        {
            if (value.size() != 1)
            {
                // Argument has incorrect length
                return false;
            }

            return TargetFromChar(value.front());
        }

        if (Type == typeid(TunkioMode))
        {
            if (value.size() != 1)
            {
                // Argument has incorrect length
                return false;
            }

            return ModeFromChar(value.front());
        }

        if (Type == typeid(bool))
        {
            if (value.size() != 1)
            {
                // Argument has incorrect length
                return false;
            }

            return BoolFromChar(value.front());
        }

        // Uknown type
        return false;
    }

    bool Argument::TargetFromChar(char c)
    {
        switch (c)
        {
            case static_cast<uint8_t>(TunkioTarget::AutoDetect) :
                m_value = TunkioTarget::AutoDetect;
                return true;
            case static_cast<uint8_t>(TunkioTarget::File) :
                m_value = TunkioTarget::File;
                return true;
            case static_cast<uint8_t>(TunkioTarget::Directory) :
                m_value = TunkioTarget::Directory;
                return true;
            case  static_cast<uint8_t>(TunkioTarget::Volume) :
                m_value = TunkioTarget::Volume;
                return true;
        }

        return false;
    }

    bool Argument::ModeFromChar(char c)
    {
        switch (c)
        {
            case static_cast<uint8_t>(TunkioMode::Zeroes) :
                m_value = TunkioMode::Zeroes;
                return true;
            case static_cast<uint8_t>(TunkioMode::Ones) :
                m_value = TunkioMode::Ones;
                return true;
            case static_cast<uint8_t>(TunkioMode::LessRandom) :
                m_value = TunkioMode::LessRandom;
                return true;
            case static_cast<uint8_t>(TunkioMode::MoreRandom) :
                m_value = TunkioMode::MoreRandom;
                return true;
        }

        return false;
    }

    bool Argument::BoolFromChar(char c)
    {
        switch (c)
        {
            case 'y':
                m_value = true;
                return true;
            case 'n':
                m_value = false;
                return true;
        }

        return false;
    }

    bool Parse(std::map<std::string, Argument>& arguments, const std::vector<std::string>& rawArgs)
    {
        for (auto& kvp : arguments)
        {
            const std::string paramKey = "--" + kvp.first + '=';
            bool found = false;

            for (const std::string& rawArg : rawArgs)
            {
                const auto mm = std::mismatch(rawArg.cbegin(), rawArg.cend(), paramKey.cbegin(), paramKey.cend());
                const std::string argKey = { rawArg.cbegin(), mm.first };
                const std::string argValue = { mm.first, rawArg.cend() };

                if (paramKey != argKey)
                {
                    continue;
                }

                if (argValue.empty())
                {
                    return false;
                }

                if (!kvp.second.Parse(argValue))
                {
                    return false;
                }

                found = true;
            }

            if (kvp.second.Required && !found)
            {
                return false;
            }
        }

        return true;
    }
}