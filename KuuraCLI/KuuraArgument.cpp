#include "KuuraCLI-PCH.hpp"
#include "KuuraArgument.hpp"

namespace Kuura
{
	bool Argument::Parse(std::string_view value)
	{
		if (value.empty())
		{
			return false;
		}

		if (Type == typeid(std::filesystem::path))
		{
			_value = std::filesystem::path(value);
			return true;
		}

		if (Type == typeid(std::string))
		{
			_value = std::string(value);
			return true;
		}

		if (Type == typeid(KuuraTargetType))
		{
			if (value.size() != 1)
			{
				// Argument has incorrect length
				return false;
			}

			return TargetFromChar(value.front(), _value);
		}

		if (Type == typeid(KuuraFillType))
		{
			if (value.size() != 1)
			{
				// Argument has incorrect length
				return false;
			}

			return ModeFromChar(value.front(), _value);
		}

		if (Type == typeid(bool))
		{
			if (value.size() != 1)
			{
				// Argument has incorrect length
				return false;
			}

			return BoolFromChar(value.front(), _value);
		}

		// Uknown type
		return false;
	}

	bool TargetFromChar(char c, Argument::Variant& result)
	{
		switch (c)
		{
			case static_cast<uint8_t>(KuuraTargetType::FileOverwrite) :
			{
				result = KuuraTargetType::FileOverwrite;
				return true;
			}
			case static_cast<uint8_t>(KuuraTargetType::DirectoryOverwrite) :
			{
				result = KuuraTargetType::DirectoryOverwrite;
				return true;
			}
			case  static_cast<uint8_t>(KuuraTargetType::DriveOverwrite) :
			{
				result = KuuraTargetType::DriveOverwrite;
				return true;
			}
		}

		return false;
	}

	bool ModeFromChar(char c, Argument::Variant& result)
	{
		switch (c)
		{
			case static_cast<uint8_t>(KuuraFillType::ZeroFill) :
			{
				result = KuuraFillType::ZeroFill;
				return true;
			}
			case static_cast<uint8_t>(KuuraFillType::OneFill) :
			{
				result = KuuraFillType::OneFill;
				return true;
			}
			case static_cast<uint8_t>(KuuraFillType::ByteFill) :
			{
				result = KuuraFillType::ByteFill;
				return true;
			}
			case static_cast<uint8_t>(KuuraFillType::SequenceFill) :
			{
				result = KuuraFillType::SequenceFill;
				return true;
			}
			case static_cast<uint8_t>(KuuraFillType::FileFill) :
			{
				result = KuuraFillType::FileFill;
				return true;
			}
			case static_cast<uint8_t>(KuuraFillType::RandomFill) :
			{
				result = KuuraFillType::RandomFill;
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
