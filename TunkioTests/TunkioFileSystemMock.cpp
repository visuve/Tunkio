#include "PCH.hpp"
#include "TunkioFileSystemMock.hpp"

namespace Tunkio::FileSystemMock
{
    PathMock::PathMock(const std::wstring& str) :
        m_str(str)
    {
    }

    const wchar_t * PathMock::c_str() const
    {
        return m_str.c_str();
    }

    std::wostream& operator << (std::wostream & os, const PathMock& path)
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