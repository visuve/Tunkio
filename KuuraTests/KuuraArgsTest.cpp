#include "KuuraTests-PCH.hpp"

namespace Kuura
{
	class KuuraArgsTest : public ::testing::Test
	{
	public:
		std::map<std::string, Argument> Arguments;

		virtual void SetUp() override
		{
			Arguments =
			{
				{ "path", Argument(true, std::filesystem::path()) },
				{ "target", Argument(true, KuuraTargetType::FileOverwrite) },
				{ "mode", Argument(false, KuuraFillType::ZeroFill) },
				{ "filler", Argument(false, std::string()) },
				{ "verify", Argument(false, false) },
				{ "remove", Argument(false, false) }
			};
		}
	};

	TEST_F(KuuraArgsTest, ParseRequiredSuccess)
	{
		EXPECT_TRUE(ParseVector(Arguments, { "--path=xyz", "--target=d" }));

		EXPECT_EQ(
			Arguments.at("path").Value<std::filesystem::path>(),
			std::filesystem::path("xyz"));

		EXPECT_EQ(
			Arguments.at("target").Value<KuuraTargetType>(),
			KuuraTargetType::DirectoryOverwrite);
	}

	TEST_F(KuuraArgsTest, ParseRequiredFailure)
	{
		EXPECT_FALSE(ParseVector(Arguments, { "--path=" }));
		EXPECT_FALSE(ParseVector(Arguments, { "--path" }));
		EXPECT_FALSE(ParseVector(Arguments, { "--target" }));
		EXPECT_FALSE(ParseVector(Arguments, { "--target=" }));
		EXPECT_FALSE(ParseVector(Arguments, { "--path=x", "--target" }));
		EXPECT_FALSE(ParseVector(Arguments, { "--path=x", "--target=" }));
		EXPECT_FALSE(ParseVector(Arguments, { "--path=x", "--target=x" }));
	}

	TEST_F(KuuraArgsTest, ParseOptionalSuccess)
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
			Arguments.at("target").Value<KuuraTargetType>(),
			KuuraTargetType::DirectoryOverwrite);

		EXPECT_EQ(Arguments.at("mode").Value<KuuraFillType>(), KuuraFillType::OneFill);
		EXPECT_STREQ(Arguments.at("filler").Value<std::string>().c_str(), "foobar");
		EXPECT_EQ(Arguments.at("verify").Value<bool>(), true);
		EXPECT_EQ(Arguments.at("remove").Value<bool>(), true);
	}

	TEST_F(KuuraArgsTest, ParseAbnormalOrderSuccess)
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
			Arguments.at("target").Value<KuuraTargetType>(),
			KuuraTargetType::DriveOverwrite);

		EXPECT_EQ(Arguments.at("mode").Value<KuuraFillType>(), KuuraFillType::RandomFill);
		EXPECT_STREQ(Arguments.at("filler").Value<std::string>().c_str(), "bar foo\nfoo bar");
		EXPECT_EQ(Arguments.at("verify").Value<bool>(), true);
		EXPECT_EQ(Arguments.at("remove").Value<bool>(), false);
	}

	TEST_F(KuuraArgsTest, ParseUtterNonsense)
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
			Arguments.at("target").Value<KuuraTargetType>(),
			KuuraTargetType::FileOverwrite);

		EXPECT_EQ(Arguments.at("mode").Value<KuuraFillType>(), KuuraFillType::ZeroFill);
		EXPECT_TRUE(Arguments.at("filler").Value<std::string>().empty());
		EXPECT_EQ(Arguments.at("verify").Value<bool>(), false);
		EXPECT_EQ(Arguments.at("remove").Value<bool>(), false);
	}
}
