#include "PCH.hpp"
#include "TunkioAPI.h"
#include "TunkioErrorCodes.hpp"

namespace Tunkio
{
    // TODO: check call counts

    const auto progress = [](uint64_t bytesWritten) -> void
    {
        const uint64_t megabytesWritten = bytesWritten / 1024;
        std::cout << megabytesWritten << " megabytes written." << std::endl;
    };

    const auto errors = [](uint32_t error, uint64_t bytesWritten) -> void
    {
        std::cout << "Error " << error << "  occurred. Bytes written: " << bytesWritten << std::endl;
    };

    const auto completed = [](uint64_t bytesWritten) -> void
    {
        std::cout << "Finished. Bytes written: " << bytesWritten << std::endl;
    };

    TEST(TunkioAPITest, CreateHandleFail)
    {
        TunkioHandle* handle = TunkioCreate(nullptr);
        EXPECT_EQ(handle, nullptr);
        TunkioFree(handle);
    }

    TEST(TunkioAPITest, CreateHandleSuccess)
    {
        const TunkioOptions options
        {
            TunkioTarget::Device,
            TunkioMode::Random,
            false,
            TunkioCallbacks { progress, errors, completed },
            TunkioString{ 7, "foobar" }
        };

        TunkioHandle* handle = TunkioCreate(&options);
        EXPECT_NE(handle, nullptr);
        TunkioFree(handle);
    }


    TEST(TunkioAPITest, WipeFileSuccess)
    {
        const TunkioOptions options
        {
            TunkioTarget::File,
            TunkioMode::Random,
            false,
            TunkioCallbacks { progress, errors, completed },
            TunkioString{ 7, "foobar" }
        };

        TunkioHandle* handle = TunkioCreate(&options);
        EXPECT_EQ(Tunkio::ErrorCode::Success, int(TunkioRun(handle)));
        EXPECT_NE(handle, nullptr);
        TunkioFree(handle);
    }

    TEST(TunkioAPITest, WipeDeviceSuccess)
    {
        const TunkioOptions options
        {
            TunkioTarget::Device,
            TunkioMode::Random,
            false,
            TunkioCallbacks { progress, errors, completed },
            TunkioString{ 7, "foobar" }
        };

        TunkioHandle* handle = TunkioCreate(&options);
        EXPECT_EQ(Tunkio::ErrorCode::Success, int(TunkioRun(handle)));
        EXPECT_NE(handle, nullptr);
        TunkioFree(handle);
    }
}