#include "TunkioTests-PCH.hpp"

namespace Tunkio
{
	class TunkioArgsTest : public ::testing::Test
	{
	public:
		std::map<std::string, Argument> Arguments;

		virtual void SetUp() override
		{
			Arguments =
			{
				{ "path", Argument(true, std::filesystem::path()) },
				{ "target", Argument(true, TunkioTargetType::FileWipe) },
				{ "mode", Argument(false, TunkioFillType::ZeroFill) },
				{ "filler", Argument(false, std::string()) },
				{ "verify", Argument(false, false) },
				{ "remove", Argument(false, false) }
			};
		}
	};

	TEST_F(TunkioArgsTest, ParseRequiredSuccess)
	{
		EXPECT_TRUE(ParseVector(Arguments, { "--path=xyz", "--target=d" }));

		EXPECT_EQ(
			Arguments.at("path").Value<std::filesystem::path>(),
			std::filesystem::path("xyz"));

		EXPECT_EQ(
			Arguments.at("target").Value<TunkioTargetType>(),
			TunkioTargetType::DirectoryWipe);
	}

	TEST_F(TunkioArgsTest, ParseRequiredFailure)
	{
		EXPECT_FALSE(ParseVector(Arguments, { "--path=" }));
		EXPECT_FALSE(ParseVector(Arguments, { "--path" }));
		EXPECT_FALSE(ParseVector(Arguments, { "--target" }));
		EXPECT_FALSE(ParseVector(Arguments, { "--target=" }));
		EXPECT_FALSE(ParseVector(Arguments, { "--path=x", "--target" }));
		EXPECT_FALSE(ParseVector(Arguments, { "--path=x", "--target=" }));
		EXPECT_FALSE(ParseVector(Arguments, { "--path=x", "--target=x" }));
	}

	TEST_F(TunkioArgsTest, ParseOptionalSuccess)
	{
		EXPECT_TRUE(ParseVector(Arguments,
		{
			"--path=xyz",
			"--target=d",
			"--mode=1",
			"--filler=foobar",
			"--verify=y",
			"--remove=y"
		}));

		EXPECT_EQ(
			Arguments.at("path").Value<std::filesystem::path>(),
			std::filesystem::path("xyz"));

		EXPECT_EQ(
			Arguments.at("target").Value<TunkioTargetType>(),
			TunkioTargetType::DirectoryWipe);

		EXPECT_EQ(Arguments.at("mode").Value<TunkioFillType>(), TunkioFillType::OneFill);
		EXPECT_STREQ(Arguments.at("filler").Value<std::string>().c_str(), "foobar");
		EXPECT_EQ(Arguments.at("verify").Value<bool>(), true);
		EXPECT_EQ(Arguments.at("remove").Value<bool>(), true);
	}

	TEST_F(TunkioArgsTest, ParseAbnormalOrderSuccess)
	{
		EXPECT_TRUE(ParseVector(Arguments,
		{
			"--target=D",
			"--mode=r",
			"--remove=n",
			"--filler=bar foo\nfoo bar",
			"--verify=y",
			"--path=zyx"
		}));

		EXPECT_EQ(
			Arguments.at("path").Value<std::filesystem::path>(),
			std::filesystem::path("zyx"));

		EXPECT_EQ(
			Arguments.at("target").Value<TunkioTargetType>(),
			TunkioTargetType::DriveWipe);

		EXPECT_EQ(Arguments.at("mode").Value<TunkioFillType>(), TunkioFillType::RandomFill);
		EXPECT_STREQ(Arguments.at("filler").Value<std::string>().c_str(), "bar foo\nfoo bar");
		EXPECT_EQ(Arguments.at("verify").Value<bool>(), true);
		EXPECT_EQ(Arguments.at("remove").Value<bool>(), false);
	}

	TEST_F(TunkioArgsTest, ParseUtterNonsense)
	{
		EXPECT_FALSE(ParseVector(Arguments,
		{
			"--target=barfoo",
			"--mode=foobar",
			"--remove=z",
			"--filler=\0\0\0",
			"--verify=",
			"--path=\0"
		}));

		EXPECT_EQ(
			Arguments.at("path").Value<std::filesystem::path>(),
			std::filesystem::path());

		EXPECT_EQ(
			Arguments.at("target").Value<TunkioTargetType>(),
			TunkioTargetType::FileWipe);

		EXPECT_EQ(Arguments.at("mode").Value<TunkioFillType>(), TunkioFillType::ZeroFill);
		EXPECT_TRUE(Arguments.at("filler").Value<std::string>().empty());
		EXPECT_EQ(Arguments.at("verify").Value<bool>(), false);
		EXPECT_EQ(Arguments.at("remove").Value<bool>(), false);
	}
}
