#include "PCH.hpp"
#include "TunkioFileSystemMock.hpp"
#include "TunkioEncoding.hpp"

namespace Tunkio::FileSystemMock
{
    PathMock::PathMock(const char* str) :
        m_str(Encoding::ToWide(str))
    {
    }

    PathMock::PathMock(const wchar_t* str) :
        m_str(str)
    {
    }

    PathMock::PathMock(const std::string& str) :
        m_str(Encoding::ToWide(str))
    {
    }

     PathMock::PathMock(const std::wstring& str) :
        m_str(str)
     {
     }

    const wchar_t* PathMock::c_str() const
    {
        return m_str.c_str();
    }

    std::ostream& operator << (std::ostream& os, const PathMock& path)
    {
        return os << Encoding::ToNarrow(path.m_str);
    }

    std::wostream& operator << (std::wostream& os, const PathMock& path)
    {
        return os << path.m_str;
    }

    bool exists(const PathMock&)
    {
        return true;
    }

    uint64_t file_size(const PathMock&)
    {
        return 1;
    }

    bool remove(const PathMock&)
    {
        return true;
    }
}