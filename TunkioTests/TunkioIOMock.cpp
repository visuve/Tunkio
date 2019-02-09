#include "PCH.hpp"
#include "TunkioIOMock.hpp"
#include "TunkioIO.hpp"
#include "TunkioUnits.hpp"

namespace Tunkio::IO
{
    namespace Mock
    {
        std::function<bool(unsigned long* writtenBytes)> FakeWrite;

        void SetFakeWrite(std::function<bool(unsigned long*)> fn)
        {
            FakeWrite = fn;
        }
    }

    HANDLE Win32Open(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE)
    {
        return nullptr;
    }

    BOOL Win32FileSize(HANDLE, PLARGE_INTEGER size)
    {
        size->QuadPart = 123;
        return TRUE;
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
        return Mock::FakeWrite(writtenBytes);
    }
}