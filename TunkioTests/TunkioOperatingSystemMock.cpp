#include "PCH.hpp"
#include "TunkioOperatingSystemMock.hpp"

namespace Tunkio
{
    DWORD FormatMessageMock(DWORD, LPCVOID, DWORD, DWORD, LPSTR, DWORD, va_list *)
    {
        return 0;
    }

    HANDLE CreateFileMock(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE)
    {
        return reinterpret_cast<HANDLE>(1);
    }

    BOOL DeviceIoControlMock(HANDLE, DWORD, LPVOID, DWORD, LPVOID diskGeometry, DWORD, LPDWORD bytesReturned, LPOVERLAPPED)
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

    BOOL WriteFileMock(HANDLE, LPCVOID, DWORD, LPDWORD written, LPOVERLAPPED)
    {
        *written = 0x1000;
        return TRUE;
    }
}