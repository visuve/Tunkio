#include "PCH.hpp"
#include "TunkioArgs.hpp"

namespace Tunkio
{
    std::array<Args::Argument, 3> Args::Arguments
    {
        Args::Argument(true, L"--path=", std::wstring()),
        Args::Argument(false, L"--target=", Args::Target::AutoDetect),
        Args::Argument(false, L"--mode=", Args::Mode::Zeroes)
    };

    TEST(TunkioArgsTest, ParseRequiredSuccess)
    {
        EXPECT_TRUE(Args::Parse({ L"--path=xyz" }));
        EXPECT_TRUE(Args::Parse({ L"--path=x" }));
    }

    TEST(TunkioArgsTest, ParseRequiredFailure)
    {
        EXPECT_FALSE(Args::Parse({ L"--path=" }));
        EXPECT_FALSE(Args::Parse({ L"--path" }));
    }

    TEST(TunkioArgsTest, ParseOptionalSuccess)
    {
        EXPECT_TRUE(Args::Parse({ L"--path=xyz", L"--target=a", L"--mode=0" }));
        EXPECT_TRUE(Args::Parse({ L"--path=x", L"--target=v", L"--mode=1" }));
    }

    TEST(TunkioArgsTest, ParseOptionalFailure)
    {
        EXPECT_FALSE(Args::Parse({ L"--path=xyz", L"--target=x", L"--mode=0" }));
        EXPECT_FALSE(Args::Parse({ L"--path=x", L"--target=v", L"--mode=z" }));
        EXPECT_FALSE(Args::Parse({ L"--path=xyz", L"--target=v", L"--mode=rofl" }));
    }

    TEST(TunkioArgsTest, ParseAbnormalOrderSuccess)
    {
        EXPECT_TRUE(Args::Parse({ L"--mode=0",  L"--target=a", L"--path=xyz" }));
        EXPECT_TRUE(Args::Parse({ L"--mode=1", L"--path=x", L"--target=v",  }));
    }

    TEST(TunkioArgsTest, ParseAbnormalOrderFailure)
    {
        EXPECT_FALSE(Args::Parse({ L"--mode=0",  L"--target=a", L"--path=" }));
        EXPECT_FALSE(Args::Parse({ L"--mode=a", L"--path=x", L"--target=0", }));
    }

    TEST(TunkioArgsTest, ParseUtterNonsense)
    {
        EXPECT_FALSE(Args::Parse({ L"--path=\0", L" ", L"\0" }));
        EXPECT_FALSE(Args::Parse({ L"a", L"b", L"c" }));
        EXPECT_FALSE(Args::Parse({ L"a b c", L"d e f" }));
    }
}