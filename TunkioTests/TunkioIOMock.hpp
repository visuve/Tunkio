#pragma once

#include <iostream>
#include <string>

namespace Tunkio::IO
{
    class PathMock
    {
    public:
        PathMock(const wchar_t* str);
        PathMock(const std::wstring& str);

        friend std::wostream& operator << (std::wostream& os, const PathMock& p);

        const wchar_t* c_str() const;
    private:
        std::wstring m_str;
    };

    class FileSystemMock
    {
    public:
        static bool exists(const PathMock&);
        static uint64_t file_size(const PathMock&);
        static bool remove(const PathMock&);
    };

    class StreamBufferMock
    {
    public:
        StreamBufferMock() = default;
        uint64_t sputn(const char * data, uint64_t size);
    };

    class FileStreamMock
    {
    public:
        FileStreamMock(const PathMock& path, int modes);
        operator bool();
        uint64_t tellp();
        StreamBufferMock* rdbuf();

    private:
        const PathMock m_path;
        const int m_modes;
        StreamBufferMock m_buffer;
    };

     HANDLE Win32OpenMock(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
     BOOL Win32DeviceIoControlMock(HANDLE, DWORD, LPVOID, DWORD, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
     BOOL Win32WriteMock(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
}