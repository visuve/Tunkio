#include "PCH.hpp"
#include "TunkioTime.hpp"

namespace Tunkio::Time
{
    std::ostream& operator << (std::ostream& os, Days s) { return os << s.count() << 'd'; }
    std::ostream& operator << (std::ostream& os, Hours s) { return os << s.count() << 'h'; }
    std::ostream& operator << (std::ostream& os, Minutes s) { return os << s.count() << 'm'; }
    std::ostream& operator << (std::ostream& os, Seconds s) { return os << s.count() << 's'; }
    std::ostream& operator << (std::ostream& os, MilliSeconds ms) { return os << ms.count() << "ms"; }
    std::ostream& operator << (std::ostream& os, MicroSeconds us) { return os << us.count() << "us"; }
    std::ostream& operator << (std::ostream& os, const Duration& x) { return os << x.H << ' ' << x.M << ' ' << x.S << ' ' << x.Ms << ' ' << x.Us; }

    std::wostream& operator << (std::wostream& os, Days s) { return os << s.count() << L'd'; }
    std::wostream& operator << (std::wostream& os, Hours s) { return os << s.count() << L'h'; }
    std::wostream& operator << (std::wostream& os, Minutes s) { return os << s.count() << L'm'; }
    std::wostream& operator << (std::wostream& os, Seconds s) { return os << s.count() << L's'; }
    std::wostream& operator << (std::wostream& os, MilliSeconds ms) { return os << ms.count() << L"ms"; }
    std::wostream& operator << (std::wostream& os, MicroSeconds us) { return os << us.count() << L"us"; }
    std::wostream& operator << (std::wostream& os, const Duration& x) { return os << x.H << L' ' << x.M << L' ' << x.S << L' ' << x.Ms << L' ' << x.Us; }

    std::string HumanReadable(const Duration& duration)
    {
        std::stringstream os;

        if (duration.H >= Hours(24))
        {
            os << std::chrono::duration_cast<Days>(duration.H) << ' ';
            os << duration.H % 24 << ' ';
            os << duration.M << ' ';
        }
        else if (duration.H.count() % 24)
        {
            os << duration.H << ' ';
            os << duration.M << ' ';
        }
        else if (duration.M > Minutes(0))
        {
            os << duration.M << ' ';
        }

        os << duration.S;

        return os.str();
    }

    Duration Timer::Elapsed() const
    {
        return Duration(Elapsed<MicroSeconds>());
    }
}