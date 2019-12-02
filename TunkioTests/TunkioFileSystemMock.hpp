#pragma once

namespace Tunkio::FileSystemMock
{
    class PathMock
    {
    public:
        PathMock() = default;
        PathMock(const char* str);
        PathMock(const wchar_t* str);
        PathMock(const std::string& str);
        PathMock(const std::wstring& str);

        friend std::ostream& operator << (std::ostream& os, const PathMock& p);
        friend std::wostream& operator << (std::wostream& os, const PathMock& p);

        const wchar_t* c_str() const;
    private:
        std::wstring m_str;
    };

    bool exists(const PathMock &);
    uint64_t file_size(const PathMock &);
    bool remove(const PathMock &);
}