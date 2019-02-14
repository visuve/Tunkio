#include "PCH.hpp"
#include "TunkioAPI.h"

namespace Tunkio
{
    TEST(TunkioAPITest, WipeFileSuccess)
    {
        const int argc = 5;
        char* argv[argc] =
        { 
            "Path\\To\\This\\Exe",
            "--path=xyz", 
            "--target=f", 
            "--mode=0",
            "--remove=y"
        };

        const auto progress = [](uint64_t bytesWritten, uint64_t secondsElapsed) -> void
        {
            const uint64_t megabytesWritten = bytesWritten / 1024;
            std::wcout << megabytesWritten << L" megabytes written. Speed " << megabytesWritten / secondsElapsed << " MB/s" << std::endl;
        };

        const unsigned long actual = TunkioExecuteA(argc, argv, progress);
        const unsigned long expected = ERROR_SUCCESS;
        EXPECT_EQ(actual, expected);
    }
}