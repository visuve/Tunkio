#include "PCH.hpp"
#include "TunkioFileSystemMock.hpp"

namespace Tunkio::FileSystemMock
{
    PathMock::PathMock(const wchar_t* str) :
        m_str(str)
    {
    }

    PathMock::PathMock(const char* str)
    {
        // NOTE: this is a mock so an ugly solution will do
        size_t required = 0;
        m_str.resize(1024);
        if (mbstowcs_s(&required, &m_str.front(), 1024, str, 1024) == ERROR_SUCCESS && required > 0)
        {
            m_str.resize(required - 1);
        }
        else
        {
            m_str.clear();
        }
    }

    PathMock::PathMock(const std::wstring& str) :
        m_str(str)
    {
    }

    PathMock::PathMock(const std::string& str) :
        PathMock(str.c_str())
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