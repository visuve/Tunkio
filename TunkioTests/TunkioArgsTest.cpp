#include "TunkioTests-PCH.hpp"

namespace Tunkio::Args
{
	std::map<std::string, Argument> Arguments =
	{
		{ "path", Argument(true, std::string()) },
		{ "target", Argument(false, TunkioTargetType::FileWipe) },
		{ "mode", Argument(false, TunkioFillType::ZeroFill) },
		{ "remove", Argument(false, false) },
	};

	TEST(TunkioArgsTest, ParseRequiredSuccess)
	{
		EXPECT_TRUE(ParseVector(Arguments, { "--path=xyz" }));
		EXPECT_TRUE(ParseVector(Arguments, { "--path=x" }));

		EXPECT_STREQ(Arguments.at("path").Value<std::string>().c_str(), "x");
	}

	TEST(TunkioArgsTest, ParseRequiredFailure)
	{
		EXPECT_FALSE(ParseVector(Arguments, { "--path=" }));
		EXPECT_FALSE(ParseVector(Arguments, { "--path" }));
	}

	TEST(TunkioArgsTest, ParseOptionalSuccess)
	{
		EXPECT_TRUE(ParseVector(Arguments, { "--path=xyz", "--target=d", "--mode=0" }));
		EXPECT_TRUE(ParseVector(Arguments, { "--path=x", "--target=D", "--mode=1" }));
		EXPECT_TRUE(ParseVector(Arguments, { "--path=x", "--target=D", "--mode=1", "--remove=y" }));

		EXPECT_STREQ(Arguments.at("path").Value<std::string>().c_str(), "x");
		EXPECT_EQ(Arguments.at("target").Value<TunkioTargetType>(), TunkioTargetType::DriveWipe);
		EXPECT_EQ(Arguments.at("mode").Value<TunkioFillType>(), TunkioFillType::OneFill);
		EXPECT_EQ(Arguments.at("remove").Value<bool>(), true);
	}

	TEST(TunkioArgsTest, ParseOptionalFailure)
	{
		EXPECT_FALSE(ParseVector(Arguments, { "--path=xyz", "--target=x", "--mode=0" }));
		EXPECT_FALSE(ParseVector(Arguments, { "--path=x", "--target=m", "--mode=z" }));
		EXPECT_FALSE(ParseVector(Arguments, { "--path=xyz", "--target=m", "--mode=rofl" }));
	}

	TEST(TunkioArgsTest, ParseAbnormalOrderSuccess)
	{
		EXPECT_TRUE(ParseVector(Arguments, { "--mode=0",  "--target=d", "--path=xyz" }));
		EXPECT_TRUE(ParseVector(Arguments, { "--mode=1", "--path=x", "--target=D", }));

		EXPECT_STREQ(Arguments.at("path").Value<std::string>().c_str(), "x");
		EXPECT_EQ(Arguments.at("target").Value<TunkioTargetType>(), TunkioTargetType::DriveWipe);
		EXPECT_EQ(Arguments.at("mode").Value<TunkioFillType>(), TunkioFillType::OneFill);
		EXPECT_EQ(Arguments.at("remove").Value<bool>(), true);
	}

	TEST(TunkioArgsTest, ParseAbnormalOrderFailure)
	{
		EXPECT_FALSE(ParseVector(Arguments, { "--mode=0",  "--target=a", "--path=" }));
		EXPECT_FALSE(ParseVector(Arguments, { "--mode=a", "--path=x", "--target=0", }));
	}

	TEST(TunkioArgsTest, ParseUtterNonsense)
	{
		EXPECT_FALSE(ParseVector(Arguments, { "--path=\0", " ", "\0" }));
		EXPECT_FALSE(ParseVector(Arguments, { "a", "b", "c" }));
		EXPECT_FALSE(ParseVector(Arguments, { "a b c", "d e f" }));
	}

#ifndef __clang__
	TEST(TunkioArgsTest, ParseUnsupportedTypes)
	{
		std::map<std::string, Argument> unsupported =
		{
			{ "x", Argument(0, uint64_t()) },
		};

		EXPECT_FALSE(ParseVector(unsupported, { "--x=555" }));
	}
#endif
}
