#include "PCH.hpp"
#include "TunkioArgs.hpp"

namespace Tunkio
{
    TEST(TunkioArgsTest, ParseRequiredSuccess)
    {
        EXPECT_TRUE(Args::Parse({ L"--path=xyz" }));
        EXPECT_TRUE(Args::Parse({ L"--path=x" }));

        EXPECT_STREQ(Args::Arguments[0].Value<std::wstring>().c_str(), L"x");
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
        EXPECT_TRUE(Args::Parse({ L"--path=x", L"--target=v", L"--mode=1", L"--remove=y" }));

        EXPECT_STREQ(Args::Arguments[0].Value<std::wstring>().c_str(), L"x");
        EXPECT_EQ(Args::Arguments[1].Value<Args::Target>(), Args::Target::Volume);
        EXPECT_EQ(Args::Arguments[2].Value<Args::Mode>(), Args::Mode::Ones);
        EXPECT_EQ(Args::Arguments[3].Value<bool>(), true);
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

        EXPECT_STREQ(Args::Arguments[0].Value<std::wstring>().c_str(), L"x");
        EXPECT_EQ(Args::Arguments[1].Value<Args::Target>(), Args::Target::Volume);
        EXPECT_EQ(Args::Arguments[2].Value<Args::Mode>(), Args::Mode::Ones);
        EXPECT_EQ(Args::Arguments[3].Value<bool>(), true);
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