#include "PCH.hpp"
#include "TunkioIOMock.hpp"
#include "TunkioAPI.h"

namespace Tunkio
{
    TEST(TunkioAPITest, WipeFileSuccess)
    {
        IO::Mock::SetFakeWrite([](LPDWORD bytesWritten)
        {
            *bytesWritten = 1;
            return true;
        });

        wchar_t* argv[4] = 
        { 
            L"Path\\To\\This\\Exe",
            L"--path=xyz", 
            L"--target=f", 
            L"--mode=0" 
        };

        const unsigned long actual = TunkioExecuteW(3, argv);
        const unsigned long expected = ERROR_SUCCESS;
        EXPECT_EQ(actual, expected);
    }
}