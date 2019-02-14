#include "PCH.hpp"
#include "TunkioEncoding.hpp"

namespace Tunkio::Encoding
{
    TEST(TunkioEncodingTest, AnsiToUnicode)
    {
        {
            const std::wstring unicode = AnsiToUnicode("abcdefgihjklmnopqstruvwxyzABCDEFGIHJKLMNOPQSTRUVWXYZ");
            EXPECT_STREQ(unicode.c_str(), L"abcdefgihjklmnopqstruvwxyzABCDEFGIHJKLMNOPQSTRUVWXYZ");
            EXPECT_NE(*unicode.crbegin(), '\0');
        }
        {
            const std::wstring unicode = AnsiToUnicode(std::string("abcdefgihjklmnopqstruvwxyzABCDEFGIHJKLMNOPQSTRUVWXYZ"));
            EXPECT_STREQ(unicode.c_str(), L"abcdefgihjklmnopqstruvwxyzABCDEFGIHJKLMNOPQSTRUVWXYZ");
            EXPECT_NE(*unicode.crbegin(), '\0');
        }
        {
            const auto unicode = AnsiToUnicode(std::vector<std::string>({ "abcdefgihjklmnopqstruvwxyz", "ABCDEFGIHJKLMNOPQSTRUVWXYZ" }));
            EXPECT_STREQ(unicode[0].c_str(), L"abcdefgihjklmnopqstruvwxyz");
            EXPECT_STREQ(unicode[1].c_str(), L"ABCDEFGIHJKLMNOPQSTRUVWXYZ");
        }
    }

    TEST(TunkioEncodingTest, UnicodeToAnsi)
    {
        {
            const std::string ansi = UnicodeToAnsi(L"abcdefgihjklmnopqstruvwxyzABCDEFGIHJKLMNOPQSTRUVWXYZ");
            EXPECT_STREQ(ansi.c_str(), "abcdefgihjklmnopqstruvwxyzABCDEFGIHJKLMNOPQSTRUVWXYZ");
            EXPECT_NE(*ansi.crbegin(), '\0');
        }
        {
            const std::string ansi = UnicodeToAnsi(std::wstring(L"abcdefgihjklmnopqstruvwxyzABCDEFGIHJKLMNOPQSTRUVWXYZ"));
            EXPECT_STREQ(ansi.c_str(), "abcdefgihjklmnopqstruvwxyzABCDEFGIHJKLMNOPQSTRUVWXYZ");
            EXPECT_NE(*ansi.crbegin(), '\0');
        }
        {
            const auto unicode = UnicodeToAnsi(std::vector<std::wstring>({ L"abcdefgihjklmnopqstruvwxyz", L"ABCDEFGIHJKLMNOPQSTRUVWXYZ" }));
            EXPECT_STREQ(unicode[0].c_str(), "abcdefgihjklmnopqstruvwxyz");
            EXPECT_STREQ(unicode[1].c_str(), "ABCDEFGIHJKLMNOPQSTRUVWXYZ");
        }
    }
}