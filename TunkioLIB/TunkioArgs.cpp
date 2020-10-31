#include "PCH.hpp"
#include "TunkioArgs.hpp"

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
			m_value = value;
			return true;
		}

		if (Type == typeid(TunkioTargetType))
		{
			if (value.size() != 1)
			{
				// Argument has incorrect length
				return false;
			}

			return TargetFromChar(value.front(), m_value);
		}

		if (Type == typeid(TunkioFillMode))
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

	bool TargetFromChar(char c, Argument::Variant& result)
	{
		switch (c)
		{
			case static_cast<uint8_t>(TunkioTargetType::File) :
			{
				result = TunkioTargetType::File;
				return true;
			}
			case static_cast<uint8_t>(TunkioTargetType::Directory) :
			{
				result = TunkioTargetType::Directory;
				return true;
			}
			case  static_cast<uint8_t>(TunkioTargetType::Drive) :
			{
				result = TunkioTargetType::Drive;
				return true;
			}
		}

		return false;
	}

	bool ModeFromChar(char c, Argument::Variant& result)
	{
		switch (c)
		{
			case static_cast<uint8_t>(TunkioFillMode::Zeroes) :
			{
				result = TunkioFillMode::Zeroes;
				return true;
			}
			case static_cast<uint8_t>(TunkioFillMode::Ones) :
			{
				result = TunkioFillMode::Ones;
				return true;
			}
			case static_cast<uint8_t>(TunkioFillMode::Random) :
			{
				result = TunkioFillMode::Random;
				return true;
			}
		}

		return false;
	}

	bool BoolFromChar(char c, Argument::Variant& result)
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

	bool ParseVector(std::map<std::string, Argument>& arguments, const std::vector<std::string>& rawArguments)
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

	bool ParseString(std::map<std::string, Argument>& arguments, const std::string& rawArguments)
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