#include "PCH.hpp"
#include "TunkioDataUnits.hpp"

namespace Tunkio
{
    TEST(TunkioDataUnitTest, Bytes)
    {
        EXPECT_EQ(DataUnit::Byte(1).Bytes(), 1);
        EXPECT_EQ(DataUnit::Kibibyte(1).Bytes(), 0x400u);
        EXPECT_EQ(DataUnit::Mebibyte(1).Bytes(), 0x100000u);
        EXPECT_EQ(DataUnit::Gibibyte(1).Bytes(), 0x40000000u);
        EXPECT_EQ(DataUnit::Tebibyte(1).Bytes(), 0x10000000000u);
        EXPECT_EQ(DataUnit::Pebibyte(1).Bytes(), 0x4000000000000u);

        EXPECT_EQ(DataUnit::Byte(0).Bytes(), 0);
        EXPECT_EQ(DataUnit::Kibibyte(0).Bytes(), 0);
        EXPECT_EQ(DataUnit::Mebibyte(0).Bytes(), 0);
        EXPECT_EQ(DataUnit::Gibibyte(0).Bytes(), 0);
        EXPECT_EQ(DataUnit::Tebibyte(0).Bytes(), 0);
        EXPECT_EQ(DataUnit::Pebibyte(0).Bytes(), 0);
    }
    TEST(TunkioDataUnitTest, Equals)
    {
        EXPECT_EQ(DataUnit::Byte(1024), DataUnit::Kibibyte(1));
        EXPECT_EQ(DataUnit::Kibibyte(1024), DataUnit::Mebibyte(1));
    }

    TEST(TunkioDataUnitTest, Sum)
    {
        EXPECT_EQ(DataUnit::Mebibyte(3) + DataUnit::Mebibyte(1), DataUnit::Kibibyte(4096));
        EXPECT_EQ(DataUnit::Mebibyte(4) + DataUnit::Kibibyte(1), DataUnit::Kibibyte(4097));
    }

    TEST(TunkioDataUnitTest, Diff)
    {
        EXPECT_EQ(DataUnit::Mebibyte(3) - DataUnit::Mebibyte(1), DataUnit::Kibibyte(2048));
        EXPECT_EQ(DataUnit::Mebibyte(4) - DataUnit::Kibibyte(1), DataUnit::Kibibyte(4095));
    }
    TEST(TunkioDataUnitTest, ToString)
    {
        {
            std::stringstream stream;
            stream <<
                DataUnit::Pebibyte(1) << ' ' <<
                DataUnit::Tebibyte(1) << ' ' <<
                DataUnit::Gibibyte(1) << ' ' <<
                DataUnit::Mebibyte(1) << ' ' <<
                DataUnit::Kibibyte(1) << ' ' <<
                DataUnit::Byte(1);

            EXPECT_STREQ(stream.str().c_str(), "1 pebibyte 1 tebibyte 1 gibibyte 1 mebibyte 1 kibibyte 1 byte");
        }
        {
            std::stringstream stream;
            stream <<
                DataUnit::Pebibyte(2) << ' ' <<
                DataUnit::Tebibyte(2) << ' ' <<
                DataUnit::Gibibyte(2) << ' ' <<
                DataUnit::Mebibyte(2) << ' ' <<
                DataUnit::Kibibyte(2) << ' ' <<
                DataUnit::Byte(2);

            EXPECT_STREQ(stream.str().c_str(), "2 pebibytes 2 tebibytes 2 gibibytes 2 mebibytes 2 kibibytes 2 bytes");
        }
        {
            std::stringstream stream;
            stream << DataUnit::Byte(1024);
            EXPECT_STREQ(stream.str().c_str(), "1024 bytes");
        }
    }

    TEST(TunkioDataUnitTest, HumanReadable)
    {
        EXPECT_STREQ(DataUnit::HumanReadable(DataUnit::Pebibyte(1)).c_str(), "1.000 pebibyte");
        EXPECT_STREQ(DataUnit::HumanReadable(DataUnit::Tebibyte(1024)).c_str(), "1.000 pebibyte");
        EXPECT_STREQ(DataUnit::HumanReadable(DataUnit::Kibibyte(1536)).c_str(), "1.500 mebibytes");
    }
}