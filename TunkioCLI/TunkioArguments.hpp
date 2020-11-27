#pragma once

#include "../TunkioAPI/TunkioAPI.h"

#include <typeindex>
#include <variant>

namespace Tunkio
{
	class Argument
	{
	public:
		using Variant = std::variant<
			std::string,
			std::filesystem::path,
			TunkioTargetType,
			TunkioFillType,
			bool>;

		template <typename T>
		Argument(bool required, T&& value) :
			Required(required),
			Type(typeid(value)),
			m_value(value)
		{
		}

		bool Parse(std::string_view value);

		template <class T>
		const T& Value() const
		{
			return std::get<T>(m_value);
		}

		const bool Required;
		const std::type_index Type;

	private:
		Variant m_value;
	};

	bool TargetFromChar(char c, Argument::Variant& result);
	bool ModeFromChar(char c, Argument::Variant& result);
	bool BoolFromChar(char c, Argument::Variant& result);

	bool ParseVector(
		std::map<std::string, Argument>& arguments,
		const std::vector<std::string>& rawArgs);
}