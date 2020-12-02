#include "TunkioCLI-PCH.hpp"
#include "TunkioArgument.hpp"

namespace Tunkio
{
	bool Argument::Parse(std::string_view value)
	{
		if (value.empty())
		{
			return false;
		}

		if (Type == typeid(std::filesystem::path))
		{
			m_value = std::filesystem::path(value);
			return true;
		}

		if (Type == typeid(std::string))
		{
			m_value = std::string(value);
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

		if (Type == typeid(TunkioFillType))
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
			case static_cast<uint8_t>(TunkioTargetType::FileWipe) :
			{
				result = TunkioTargetType::FileWipe;
				return true;
			}
			case static_cast<uint8_t>(TunkioTargetType::DirectoryWipe) :
			{
				result = TunkioTargetType::DirectoryWipe;
				return true;
			}
			case  static_cast<uint8_t>(TunkioTargetType::DriveWipe) :
			{
				result = TunkioTargetType::DriveWipe;
				return true;
			}
		}

		return false;
	}

	bool ModeFromChar(char c, Argument::Variant& result)
	{
		switch (c)
		{
			case static_cast<uint8_t>(TunkioFillType::ZeroFill) :
			{
				result = TunkioFillType::ZeroFill;
				return true;
			}
			case static_cast<uint8_t>(TunkioFillType::OneFill) :
			{
				result = TunkioFillType::OneFill;
				return true;
			}
			case static_cast<uint8_t>(TunkioFillType::ByteFill) :
			{
				result = TunkioFillType::ByteFill;
				return true;
			}
			case static_cast<uint8_t>(TunkioFillType::SequenceFill) :
			{
				result = TunkioFillType::SequenceFill;
				return true;
			}
			case static_cast<uint8_t>(TunkioFillType::FileFill) :
			{
				result = TunkioFillType::FileFill;
				return true;
			}
			case static_cast<uint8_t>(TunkioFillType::RandomFill) :
			{
				result = TunkioFillType::RandomFill;
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

	bool ParseVector(
		std::map<std::string, Argument>& arguments,
		const std::vector<std::string>& rawArguments)
	{
		for (auto& kvp : arguments)
		{
			const std::string argumentKey = "--" + kvp.first + '=';
			bool found = false;

			for (std::string_view rawArgument : rawArguments)
			{
				if (!rawArgument.starts_with(argumentKey))
				{
					continue;
				}

				std::string_view rawArgumentValue = rawArgument.substr(argumentKey.size());

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
}
