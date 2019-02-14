#pragma once

#include "TunkioAPI.h"
#include "TunkioFileSystem.hpp"

#ifndef TESTING
#include <Windows.h>
constexpr auto Win32FormatMessage = FormatMessageW;
constexpr auto Win32CreateFile = CreateFileW;
constexpr auto Win32DeviceIoControl = DeviceIoControl;
constexpr auto Win32WriteFile = WriteFile;
#else
#include "TunkioOperatingSystemMock.hpp"
constexpr auto Win32FormatMessage = Tunkio::FormatMessageMock;
constexpr auto Win32CreateFile = Tunkio::CreateFileMock;
constexpr auto Win32DeviceIoControl = Tunkio::DeviceIoControlMock;
constexpr auto Win32WriteFile = Tunkio::WriteFileMock;
#endif


namespace Tunkio::Native
{
    namespace Win32
    {
        using RawHandle = void*;

        class AutoHandle
        {
        public:
            AutoHandle(const RawHandle handle);
            AutoHandle(const AutoHandle&) = delete;
            AutoHandle(AutoHandle&&) = delete;
            AutoHandle& operator = (const AutoHandle&) = delete;
            AutoHandle& operator = (AutoHandle&&) = delete;
            ~AutoHandle();

            bool IsValid() const;
            operator const RawHandle() const;
        private:
            const RawHandle m_handle = reinterpret_cast<RawHandle*>(-1);
        };
        
        std::wstring ErrorToString(const uint32_t error);

        RawHandle Open(const Path& path);
        uint64_t VolumeSize(const AutoHandle& volume);
        bool Fill(const AutoHandle& handle, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, TunkioProgressCallback progress);
    }
}