#include "PCH.hpp"
#include "TunkioAPI.h"

namespace Tunkio
{
    TEST(TunkioAPITest, WipeFileSuccess)
    {
        const auto progress = [](uint64_t bytesWritten, uint64_t secondsElapsed) -> void
        {
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
                "xyz"
            }
        };

        const unsigned long actual = TunkioExecute(&options);
        const unsigned long expected = ERROR_SUCCESS;
        EXPECT_EQ(actual, expected);
    }
}