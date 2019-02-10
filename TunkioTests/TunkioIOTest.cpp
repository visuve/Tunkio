#include "PCH.hpp"
#include "TunkioIOMock.hpp"
#include "TunkioIO.hpp"

namespace Tunkio
{
    TEST(TunkioIOTest, FileOpenSuccess)
    {
        EXPECT_TRUE(IO::File::Open(L"foo/bar") == true);
    }

    TEST(TunkioIOTest, FileSizeSuccess)
    {
        EXPECT_EQ(IO::File::Size(L"foo/bar"), 1);
    }

    TEST(TunkioIOTest, FileFillLessThanMegaSuccess)
    {
        IO::FileStream file = IO::File::Open(L"foo/bar");

        uint64_t bytesLeft = 0x1000;
        uint64_t bytesWritten = 0;

        EXPECT_TRUE(IO::File::Fill(file, bytesLeft, bytesWritten, nullptr));
    }

    TEST(TunkioIOTest, FileFillMoreThanMegaSuccess)
    {
        IO::FileStream file = IO::File::Open(L"foo/bar");

        uint64_t bytesLeft = 0x20'0000;
        uint64_t bytesWritten = 0;

        EXPECT_TRUE(IO::File::Fill(file, bytesLeft, bytesWritten, nullptr));
    }

    TEST(TunkioIOTest, FileRemoveSuccess)
    {
        EXPECT_TRUE(IO::File::Remove(L"foo/bar"));
    }

    TEST(TunkioIOTest, VolumeOpenSuccess)
    {
        EXPECT_NE(IO::Volume::Open(L"foo/bar"), INVALID_HANDLE_VALUE);
    }

    TEST(TunkioIOTest, VolumeSizeSuccess)
    {
        IO::RawHandle volume = IO::Volume::Open(L"foo/bar");
        EXPECT_EQ(IO::Volume::Size(volume), 210);
    }

   TEST(TunkioIOTest, VolumeFillLessThanMegaSuccess)
    {
        uint64_t bytesLeft = 0x1000;
        uint64_t bytesWritten = 0;

        IO::RawHandle volume = IO::Volume::Open(L"foo/bar");
        EXPECT_TRUE(IO::Volume::Fill(volume, bytesLeft, bytesWritten, nullptr));
    }

   TEST(TunkioIOTest, VolumeFillMoreThanMegaSuccess)
   {
       uint64_t bytesLeft = 0x20'0000;
       uint64_t bytesWritten = 0;

       IO::RawHandle volume = IO::Volume::Open(L"foo/bar");
       EXPECT_TRUE(IO::Volume::Fill(volume, bytesLeft, bytesWritten, nullptr));
   }
}