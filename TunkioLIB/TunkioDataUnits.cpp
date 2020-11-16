#include "PCH.hpp"
#include "TunkioDataUnits.hpp"

namespace Tunkio::DataUnit
{
	std::ostream& operator << (std::ostream& os, Bytes bs)
	{
		return os << bs.Value() << (bs.Value() > 1 ? " bytes" : " byte");
	}

	std::ostream& operator << (std::ostream& os, Kibibytes bs)
	{
		return os << bs.Value() << (bs.Value() > 1 ? " kibibytes" : " kibibyte");
	}

	std::ostream& operator << (std::ostream& os, Mebibytes bs)
	{
		return os << bs.Value() << (bs.Value() > 1 ? " mebibytes" : " mebibyte");
	}

	std::ostream& operator << (std::ostream& os, Gibibytes bs)
	{
		return os << bs.Value() << (bs.Value() > 1 ? " gibibytes" : " gibibyte");
	}

	std::ostream& operator << (std::ostream& os, Tebibytes bs)
	{
		return os << bs.Value() << (bs.Value() > 1 ? " tebibytes" : " tebibyte");
	}

	std::ostream& operator << (std::ostream& os, Pebibytes bs)
	{
		return os << bs.Value() << (bs.Value() > 1 ? " pebibytes" : " pebibyte");
	}

	std::ostream& operator << (std::ostream& os, Exbibytes bs)
	{
		return os << bs.Value() << (bs.Value() > 1 ? " exbibytes" : " exbibyte");
	}
}