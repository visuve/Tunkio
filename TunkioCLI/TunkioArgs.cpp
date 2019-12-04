#include "PCH.hpp"
#include "TunkioArgs.hpp"
#include "TunkioAPI.h"

namespace Tunkio::Args
{
    bool Argument::Parse(const std::string& value)
    {
        if (value.empty())
        {
            return false;
        }

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

            return TargetFromChar(value.front(), m_value);
        }

        if (Type == typeid(TunkioMode))
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

    bool TargetFromChar(char c, std::any& result)
    {
        switch (c)
        {
            case static_cast<uint8_t>(TunkioTarget::File) :
            {
                result = TunkioTarget::File;
                return true;
            }
            case static_cast<uint8_t>(TunkioTarget::Directory) :
            {
                result = TunkioTarget::Directory;
                return true;
            }
            case  static_cast<uint8_t>(TunkioTarget::Device) :
            {
                result = TunkioTarget::Device;
                return true;
            }
        }

        return false;
    }

    bool ModeFromChar(char c, std::any& result)
    {
        switch (c)
        {
            case static_cast<uint8_t>(TunkioMode::Zeroes) :
            {
                result = TunkioMode::Zeroes;
                return true;
            }
            case static_cast<uint8_t>(TunkioMode::Ones) :
            {
                result = TunkioMode::Ones;
                return true;
            }
            case static_cast<uint8_t>(TunkioMode::LessRandom) :
            {
                result = TunkioMode::LessRandom;
                return true;
            }
            case static_cast<uint8_t>(TunkioMode::MoreRandom) :
            {
                result = TunkioMode::MoreRandom;
                return true;
            }
        }

        return false;
    }

    bool BoolFromChar(char c, std::any& result)
    {
        switch (c)
        {
            case 'y':
                result = true;
                return true;
            case 'n':
                result = false;
                return true;
        }

        return false;
    }

    bool Parse(std::map<std::string, Argument>& arguments, const std::vector<std::string>& rawArguments)
    {
        for (auto& kvp : arguments)
        {
            const std::string argumentKey = "--" + kvp.first + '=';
            const std::regex argumentRegex(argumentKey + "(\"[^\"]+\"|[^\\s\"]+)");
            bool found = false;

            for (const std::string& rawArgument : rawArguments)
            {
                std::smatch matches;

                if (!std::regex_match(rawArgument, matches, argumentRegex))
                {
                    continue;
                }

                const std::string rawArgumentValue = matches.str(1);

                if (!kvp.second.Parse(rawArgumentValue))
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

    bool ParsePotko(std::map<std::string, Argument>& arguments, const std::string& rawArguments)
    {
        for (auto& kvp : arguments)
        {
            const std::string argumentKey = "--" + kvp.first + '=';
            const std::regex argumentRegex(argumentKey + "(\"[^\"]+\"|[^\\s\"]+)");
            std::smatch matches;

            if (!std::regex_search(rawArguments, matches, argumentRegex))
            {
                if (kvp.second.Required)
                {
                    return false;
                }

                continue;
            }

            const std::string rawArgumentValue = matches.str(1);

            if (!kvp.second.Parse(rawArgumentValue))
            {
                return false;
            }
        }

        return true;
    }
}