#include "PCH.hpp"
#include "TunkioIOMock.hpp"
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

        const unsigned long actual = TunkioExecuteA(argc, argv);
        const unsigned long expected = ERROR_SUCCESS;
        EXPECT_EQ(actual, expected);
    }
}