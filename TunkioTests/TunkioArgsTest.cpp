#include "PCH.hpp"
#include "TunkioArgs.hpp"

namespace Tunkio
{
    std::array<Args::Argument, 3> Args::Arguments
    {
        Args::Argument(true, L"--path=", std::wstring()),
        Args::Argument(false, L"--target=", Args::Target::AutoDetect),
        Args::Argument(false, L"--mode=", Args::Mode::Zeroes)
    };;

    class TunkioArgsTest : public ::testing::Test
    {
    public:
        ~TunkioArgsTest()
        {
            // This is a bit annoyingly flaky
            Args::Arguments[0].Value = std::wstring();
            Args::Arguments[1].Value = Args::Target::AutoDetect;
            Args::Arguments[2].Value = Args::Mode::Zeroes;
        }
    };

    TEST_F(TunkioArgsTest, ParseRequiredSuccess)
    {
        EXPECT_TRUE(Args::Parse({ L"--path=xyz" }));
        EXPECT_TRUE(Args::Parse({ L"--path=x" }));
    }

    TEST_F(TunkioArgsTest, ParseRequiredFailure)
    {
        EXPECT_FALSE(Args::Parse({ L"--path=" }));
        EXPECT_FALSE(Args::Parse({ L"--path" }));
    }

    TEST_F(TunkioArgsTest, ParseOptionalSuccess)
    {
        EXPECT_TRUE(Args::Parse({ L"--path=xyz", L"--target=a", L"--mode=0" }));
        EXPECT_TRUE(Args::Parse({ L"--path=x", L"--target=v", L"--mode=1" }));
    }

    TEST_F(TunkioArgsTest, ParseOptionalFailure)
    {
        EXPECT_FALSE(Args::Parse({ L"--path=xyz", L"--target=x", L"--mode=0" }));
        EXPECT_FALSE(Args::Parse({ L"--path=x", L"--target=v", L"--mode=z" }));
    }

    TEST_F(TunkioArgsTest, ParseAbnormalOrderSuccess)
    {
        EXPECT_TRUE(Args::Parse({ L"--mode=0",  L"--target=a", L"--path=xyz" }));
        EXPECT_TRUE(Args::Parse({ L"--mode=1", L"--path=x", L"--target=v",  }));
    }

    TEST_F(TunkioArgsTest, ParseAbnormalOrderFailure)
    {
        EXPECT_FALSE(Args::Parse({ L"--mode=0",  L"--target=a", L"--path=" }));
        EXPECT_FALSE(Args::Parse({ L"--mode=a", L"--path=x", L"--target=0", }));
    }
}