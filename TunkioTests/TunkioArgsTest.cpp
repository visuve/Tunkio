#include "PCH.hpp"
#include "TunkioArgs.hpp"

namespace Tunkio::Args
{
    std::array<Argument<wchar_t>, 4> Arguments =
    {
        Argument<wchar_t>(true, L"--path=", std::wstring()),
        Argument<wchar_t>(false, L"--target=", Target::AutoDetect),
        Argument<wchar_t>(false, L"--mode=", Mode::Zeroes),
        Argument<wchar_t>(false, L"--remove=", false),
    };

    TEST(TunkioArgsTest, ParseRequiredSuccess)
    {
        EXPECT_TRUE(Parse(Arguments, { L"--path=xyz" }));
        EXPECT_TRUE(Parse(Arguments, { L"--path=x" }));

        EXPECT_STREQ(Arguments[0].Value<std::wstring>().c_str(), L"x");
    }

    TEST(TunkioArgsTest, ParseRequiredFailure)
    {
        EXPECT_FALSE(Parse(Arguments, { L"--path=" }));
        EXPECT_FALSE(Parse(Arguments, { L"--path" }));
    }

     TEST(TunkioArgsTest, ParseOptionalSuccess)
    {
        EXPECT_TRUE(Parse(Arguments, { L"--path=xyz", L"--target=a", L"--mode=0" }));
        EXPECT_TRUE(Parse(Arguments, { L"--path=x", L"--target=m", L"--mode=1" }));
        EXPECT_TRUE(Parse(Arguments, { L"--path=x", L"--target=m", L"--mode=1", L"--remove=y" }));

        EXPECT_STREQ(Arguments[0].Value<std::wstring>().c_str(), L"x");
        EXPECT_EQ(Arguments[1].Value<Target>(), Target::MassMedia);
        EXPECT_EQ(Arguments[2].Value<Mode>(), Mode::Ones);
        EXPECT_EQ(Arguments[3].Value<bool>(), true);
    }

    TEST(TunkioArgsTest, ParseOptionalFailure)
    {
        EXPECT_FALSE(Parse(Arguments, { L"--path=xyz", L"--target=x", L"--mode=0" }));
        EXPECT_FALSE(Parse(Arguments, { L"--path=x", L"--target=m", L"--mode=z" }));
        EXPECT_FALSE(Parse(Arguments, { L"--path=xyz", L"--target=m", L"--mode=rofl" }));
    }

    TEST(TunkioArgsTest, ParseAbnormalOrderSuccess)
    {
        EXPECT_TRUE(Parse(Arguments, { L"--mode=0",  L"--target=a", L"--path=xyz" }));
        EXPECT_TRUE(Parse(Arguments, { L"--mode=1", L"--path=x", L"--target=m",  }));

        EXPECT_STREQ(Arguments[0].Value<std::wstring>().c_str(), L"x");
        EXPECT_EQ(Arguments[1].Value<Target>(), Target::MassMedia);
        EXPECT_EQ(Arguments[2].Value<Mode>(), Mode::Ones);
        EXPECT_EQ(Arguments[3].Value<bool>(), true);
    }

    TEST(TunkioArgsTest, ParseAbnormalOrderFailure)
    {
        EXPECT_FALSE(Parse(Arguments, { L"--mode=0",  L"--target=a", L"--path=" }));
        EXPECT_FALSE(Parse(Arguments, { L"--mode=a", L"--path=x", L"--target=0", }));
    }

    TEST(TunkioArgsTest, ParseUtterNonsense)
    {
        EXPECT_FALSE(Parse(Arguments, { L"--path=\0", L" ", L"\0" }));
        EXPECT_FALSE(Parse(Arguments, { L"a", L"b", L"c" }));
        EXPECT_FALSE(Parse(Arguments, { L"a b c", L"d e f" }));
    }
}