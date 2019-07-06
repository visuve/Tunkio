#include "PCH.hpp"
#include "TunkioAPI.h"
#include "TunkioExitCodes.hpp"

namespace Tunkio
{
    TEST(TunkioAPITest, WipeFileSuccess)
    {
        const auto progress = [](uint64_t bytesWritten, uint64_t secondsElapsed) -> void
        {
            assert(bytesWritten && secondsElapsed && "Progress triggered with zero(s)");
            const uint64_t megabytesWritten = bytesWritten / 1024;
            std::cout << megabytesWritten << " megabytes written. Speed " << megabytesWritten / secondsElapsed << " MB/s" << std::endl;
        };

        const TunkioOptions options
        {
            TunkioTarget::File,
            TunkioMode::Zeroes,
            false,
            progress,
            {
                4,
                const_cast<char*>("xyz")
            }
        };

        const unsigned long actual = TunkioExecute(&options);
        const unsigned long expected = Tunkio::ExitCode::Success;
        EXPECT_EQ(actual, expected);
    }
}