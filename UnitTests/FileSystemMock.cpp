#include "PCH.hpp"
#include "FileSystemMock.hpp"
#include "../WinDiskWipe/WDW.hpp"
#include "../WinDiskWipe/Units.hpp"

namespace WDW
{
    namespace Mock
    {
        std::function<bool(unsigned long*)> FakeWrite;

        void SetFakeWrite(std::function<bool(unsigned long*)> fn)
        {
            FakeWrite = fn;
        }
    }

    BOOL Win32DeviceIoControl(HANDLE, DWORD, LPVOID, DWORD, LPVOID diskGeometry, DWORD, LPDWORD bytesReturned, LPOVERLAPPED)
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

    BOOL Win32Write(HANDLE, LPCVOID, DWORD, LPDWORD writtenBytes, LPOVERLAPPED)
    {
        return WDW::Mock::FakeWrite(writtenBytes);
    }
}