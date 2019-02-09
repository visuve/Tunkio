#include "PCH.hpp"
#include "TunkioEncoding.hpp"

namespace Tunkio
{
    TEST(TunkioEncodingTest, AnsiToUnicode)
    {
        {
            const std::wstring unicode = Encoding::AnsiToUnicode("abcdefgihjklmnopqstruvwxyzABCDEFGIHJKLMNOPQSTRUVWXYZ");
            EXPECT_STREQ(unicode.c_str(), L"abcdefgihjklmnopqstruvwxyzABCDEFGIHJKLMNOPQSTRUVWXYZ");
            EXPECT_NE(*unicode.crbegin(), '\0');
        }
        {
            const std::wstring unicode = Encoding::AnsiToUnicode(std::string("abcdefgihjklmnopqstruvwxyzABCDEFGIHJKLMNOPQSTRUVWXYZ"));
            EXPECT_STREQ(unicode.c_str(), L"abcdefgihjklmnopqstruvwxyzABCDEFGIHJKLMNOPQSTRUVWXYZ");
            EXPECT_NE(*unicode.crbegin(), '\0');
        }
        {
            const auto unicode = Encoding::AnsiToUnicode(std::vector<std::string>({ "abcdefgihjklmnopqstruvwxyz", "ABCDEFGIHJKLMNOPQSTRUVWXYZ" }));
            EXPECT_STREQ(unicode[0].c_str(), L"abcdefgihjklmnopqstruvwxyz");
            EXPECT_STREQ(unicode[1].c_str(), L"ABCDEFGIHJKLMNOPQSTRUVWXYZ");
        }
    }

    TEST(TunkioEncodingTest, UnicodeToAnsi)
    {
        {
            const std::string ansi = Encoding::UnicodeToAnsi(L"abcdefgihjklmnopqstruvwxyzABCDEFGIHJKLMNOPQSTRUVWXYZ");
            EXPECT_STREQ(ansi.c_str(), "abcdefgihjklmnopqstruvwxyzABCDEFGIHJKLMNOPQSTRUVWXYZ");
            EXPECT_NE(*ansi.crbegin(), '\0');
        }
        {
            const std::string ansi = Encoding::UnicodeToAnsi(std::wstring(L"abcdefgihjklmnopqstruvwxyzABCDEFGIHJKLMNOPQSTRUVWXYZ"));
            EXPECT_STREQ(ansi.c_str(), "abcdefgihjklmnopqstruvwxyzABCDEFGIHJKLMNOPQSTRUVWXYZ");
            EXPECT_NE(*ansi.crbegin(), '\0');
        }
        {
            const auto unicode = Encoding::UnicodeToAnsi(std::vector<std::wstring>({ L"abcdefgihjklmnopqstruvwxyz", L"ABCDEFGIHJKLMNOPQSTRUVWXYZ" }));
            EXPECT_STREQ(unicode[0].c_str(), "abcdefgihjklmnopqstruvwxyz");
            EXPECT_STREQ(unicode[1].c_str(), "ABCDEFGIHJKLMNOPQSTRUVWXYZ");
        }
    }
}