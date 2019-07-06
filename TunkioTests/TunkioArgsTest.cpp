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
        EXPECT_TRUE(Parse(Arguments, { "--path=x", "--target=v", "--mode=1" }));
        EXPECT_TRUE(Parse(Arguments, { "--path=x", "--target=v", "--mode=1", "--remove=y" }));

        EXPECT_STREQ(Arguments.at("path").Value<std::filesystem::path>().c_str(), L"x");
        EXPECT_EQ(Arguments.at("target").Value<TunkioTarget>(), TunkioTarget::Volume);
        EXPECT_EQ(Arguments.at("mode").Value<TunkioMode>(), TunkioMode::Ones);
        EXPECT_EQ(Arguments.at("remove").Value<bool>(), true);
    }

    TEST(TunkioArgsTest, ParseOptionalFailure)
    {
        EXPECT_FALSE(Parse(Arguments, { "--path=xyz", "--target=x", "--mode=0" }));
        EXPECT_FALSE(Parse(Arguments, { "--path=x", "--target=m", "--mode=z" }));
        EXPECT_FALSE(Parse(Arguments, { "--path=xyz", "--target=m", "--mode=rofl" }));
    }

    TEST(TunkioArgsTest, ParseAbnormalOrderSuccess)
    {
        EXPECT_TRUE(Parse(Arguments, { "--mode=0",  "--target=a", "--path=xyz" }));
        EXPECT_TRUE(Parse(Arguments, { "--mode=1", "--path=x", "--target=v", }));

        EXPECT_STREQ(Arguments.at("path").Value<std::filesystem::path>().c_str(), L"x");
        EXPECT_EQ(Arguments.at("target").Value<TunkioTarget>(), TunkioTarget::Volume);
        EXPECT_EQ(Arguments.at("mode").Value<TunkioMode>(), TunkioMode::Ones);
        EXPECT_EQ(Arguments.at("remove").Value<bool>(), true);
    }

    TEST(TunkioArgsTest, ParseAbnormalOrderFailure)
    {
        EXPECT_FALSE(Parse(Arguments, { "--mode=0",  "--target=a", "--path=" }));
        EXPECT_FALSE(Parse(Arguments, { "--mode=a", "--path=x", "--target=0", }));
    }

    TEST(TunkioArgsTest, ParseUtterNonsense)
    {
        EXPECT_FALSE(Parse(Arguments, { "--path=\0", " ", "\0" }));
        EXPECT_FALSE(Parse(Arguments, { "a", "b", "c" }));
        EXPECT_FALSE(Parse(Arguments, { "a b c", "d e f" }));
    }
}