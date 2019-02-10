#include "PCH.hpp"
#include "TunkioIOMock.hpp"
#include "TunkioUnits.hpp"

namespace Tunkio::IO
{
    PathMock::PathMock(const wchar_t* str) :
        m_str(str)
    {
    }

    PathMock::PathMock(const std::wstring& str) :
        m_str(str)
    {
    }

    std::wostream& operator << (std::wostream & os, const PathMock& path)
    {
        return os << path.m_str;
    }

    bool FileSystemMock::exists(const PathMock &)
    {
        return true;
    }

    uint64_t FileSystemMock::file_size(const PathMock &)
    {
        return 1;
    }

    bool FileSystemMock::remove(const PathMock &)
    {
        return true;
    }


    FileStreamMock::FileStreamMock(const PathMock& path, std::ios_base::openmode modes) :
        m_path(path),
        m_modes(modes)
    {
    }

    FileStreamMock::operator bool()
    {
        return true;
    }

    uint64_t FileStreamMock::tellp()
    {
        return 1;
    }

    StreamBufferMock* FileStreamMock::rdbuf()
    {
        return &m_buffer;
    }

    uint64_t StreamBufferMock::sputn(const char * data, uint64_t size)
    {
        return data ? size : 0;
    }

    HANDLE Win32OpenMock(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE)
    {
        return reinterpret_cast<HANDLE>(1);
    }

    BOOL Win32DeviceIoControlMock(HANDLE, DWORD, LPVOID, DWORD, LPVOID diskGeometry, DWORD, LPDWORD bytesReturned, LPOVERLAPPED)
    {
        auto diskGeo = reinterpret_cast<DISK_GEOMETRY*>(diskGeometry);
        diskGeo->Cylinders = { 2 };
        diskGeo->MediaType = MEDIA_TYPE::F3_1Pt44_512;
        diskGeo->TracksPerCylinder = 3;
        diskGeo->SectorsPerTrack = 5;
        diskGeo->BytesPerSector = 7;

        *bytesReturned = sizeof(DISK_GEOMETRY);
        return TRUE;
    }

    BOOL Win32WriteMock(HANDLE, LPCVOID, DWORD, LPDWORD written, LPOVERLAPPED)
    {
        *written = 0x1000;
        return TRUE;
    }
}