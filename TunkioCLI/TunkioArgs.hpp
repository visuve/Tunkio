#pragma once

#include <string>
#include <any>
#include <typeindex>
#include <map>
#include <vector>
#include <sstream>

namespace Tunkio::Args
{
    class Argument
    {
    public:
        template <typename T>
        Argument(bool required, T&& value) :
            Required(required),
            Type(typeid(value)),
            m_value(std::make_any<T>(value))
        {
        }

        bool Parse(const std::string& value);

        template <class T>
        const T& Value() const
        {
            return std::any_cast<const T&>(m_value);
        }

        const bool Required;
        const std::type_index Type;

    private:
        std::any m_value;
    };

    bool TargetFromChar(char c, std::any& result);
    bool ModeFromChar(char c, std::any& result);
    bool BoolFromChar(char c, std::any& result);

    bool Parse(std::map<std::string, Argument>& arguments, const std::vector<std::string>& rawArgs);
    bool Parse(std::map<std::string, Argument>& arguments, const std::string& rawArgs);
}