#include "PCH.hpp"
#include "TunkioArgs.hpp"

namespace Tunkio::Args
{
    std::map<std::string, Argument> Arguments =
    {
        { "path", Argument(true, std::filesystem::path()) },
        { "target", Argument(false, TunkioTarget::AutoDetect) },
        { "mode", Argument(false, TunkioMode::Zeroes) },
        { "remove", Argument(false, false) },
    };

    TEST(TunkioArgsTest, ParseRequiredSuccess)
    {
        EXPECT_TRUE(Parse(Arguments, { "--path=xyz" }));
        EXPECT_TRUE(Parse(Arguments, { "--path=x" }));

        EXPECT_STREQ(Arguments.at("path").Value<std::filesystem::path>().c_str(), L"x");
    }

    TEST(TunkioArgsTest, ParseRequiredFailure)
    {
        EXPECT_FALSE(Parse(Arguments, { "--path=" }));
        EXPECT_FALSE(Parse(Arguments, { "--path" }));
    }

     TEST(TunkioArgsTest, ParseOptionalSuccess)
    {
        EXPECT_TRUE(Parse(Arguments, { "--path=xyz", "--target=a", "--mode=0" }));
        EXPECT_TRUE(Parse(Arguments, { "--path=x", "--target=m", "--mode=1" }));
        EXPECT_TRUE(Parse(Arguments, { "--path=x", "--target=m", "--mode=1", "--remove=y" }));

        EXPECT_STREQ(Arguments.at("path").Value<std::filesystem::path>().c_str(), L"x");
        EXPECT_EQ(Arguments.at("target").Value<TunkioTarget>(), TunkioTarget::MassMedia);
        EXPECT_EQ(Arguments.at("mode").Value<TunkioMode>(), TunkioMode::Ones);
        EXPECT_EQ(Arguments.at("remove").Value<bool>(), true);
    }

    TEST(TunkioArgsTest, ParseOptionalFailure)
    {
        EXPECT_FALSE(Parse(Arguments, { "--path=xyz", "--target=x", "--mode=0" }));
        EXPECT_FALSE(Parse(Arguments, { "--path=x", "--target=m", "--mode=z" }));
        EXPECT_FALSE(Parse(Arguments, { "--path=xyz", "--target=m", "--mode=rofl" }));
    }

    /*TEST(TunkioArgsTest, ParseAbnormalOrderSuccess)
    {
        EXPECT_TRUE(Parse(Arguments, { L"--mode=0",  L"--target=a", L"--path=xyz" }));
        EXPECT_TRUE(Parse(Arguments, { L"--mode=1", L"--path=x", L"--target=m",  }));

        EXPECT_STREQ(Arguments[0].Value<std::u16string>().c_str(), L"x");
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
    }*/
}