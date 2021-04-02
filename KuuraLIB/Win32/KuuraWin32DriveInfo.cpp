#include "../KuuraLIB-PCH.hpp"
#include "../KuuraDriveInfo.hpp"

#include <Windows.h>
#include <WbemCli.h>
#include <wrl/client.h>

namespace Kuura
{
	using Microsoft::WRL::ComPtr;

	class ComVariant
	{
	public:
		ComVariant()
		{
			VariantInit(&_value);
		}

		~ComVariant()
		{
			VariantClear(&_value);
		}

		operator VARIANT* ()
		{
			return &_value;
		}

		VARIANT* operator -> ()
		{
			return &_value;
		}

	private:
		VARIANT _value;
	};

	std::string ToUtf8(const std::wstring& unicode)
	{
		std::string utf8;

		int required = WideCharToMultiByte(
			CP_UTF8,
			0, unicode.c_str(),
			static_cast<int>(unicode.length()),
			nullptr, 0,
			nullptr,
			nullptr);

		if (required > 0)
		{
			utf8.resize(static_cast<size_t>(required));
			int result = WideCharToMultiByte(
				CP_UTF8,
				0,
				unicode.c_str(),
				static_cast<int>(unicode.length()),
				&utf8.front(),
				required,
				nullptr,
				nullptr);

			if (result != required)
			{
				utf8.resize(static_cast<size_t>(result));
			}
		}

		return utf8;
	}

	class DiskDriveInfo
	{
	public:
		DiskDriveInfo()
		{
			// https://docs.microsoft.com/en-us/windows/win32/wmisdk/creating-a-wmi-application-using-c-
			_result = CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY | COINIT_MULTITHREADED);
			_uninitializeRequired = SUCCEEDED(_result);

			if (FAILED(_result) && _result != RPC_E_CHANGED_MODE)
			{
				std::cerr << "CoInitializeEx failed: 0x"
					<< std::hex << _result << std::endl;
				return;
			}

			_result = CoInitializeSecurity(
				nullptr,
				-1,
				nullptr,
				nullptr,
				RPC_C_AUTHN_LEVEL_DEFAULT,
				RPC_C_IMP_LEVEL_IMPERSONATE,
				nullptr,
				EOAC_NONE,
				0);

			if (FAILED(_result) && _result != RPC_E_TOO_LATE)
			{
				std::cerr << "CoInitializeSecurity failed: 0x"
					<< std::hex << _result << std::endl;
				return;
			}

			_result = CoCreateInstance(
				CLSID_WbemLocator,
				nullptr,
				CLSCTX_INPROC_SERVER,
				IID_PPV_ARGS(&_locator));

			if (FAILED(_result))
			{
				std::cerr << "CoCreateInstance failed: 0x"
					<< std::hex << _result << std::endl;
				return;
			}

			_result = _locator->ConnectServer(
				BSTR(L"root\\CIMV2"),
				nullptr,
				nullptr,
				nullptr,
				WBEM_FLAG_CONNECT_USE_MAX_WAIT,
				nullptr,
				nullptr,
				&_service);

			if (FAILED(_result))
			{
				std::cerr << "IWbemLocator::ConnectServer failed: 0x"
					<< std::hex << _result << std::endl;
				return;
			}

			_result = CoSetProxyBlanket(
				_service.Get(),
				RPC_C_AUTHN_WINNT,
				RPC_C_AUTHZ_NONE,
				nullptr,
				RPC_C_AUTHN_LEVEL_CALL,
				RPC_C_IMP_LEVEL_IMPERSONATE,
				nullptr,
				EOAC_NONE
			);

			if (FAILED(_result))
			{
				std::cerr << "CoSetProxyBlanket failed: 0x"
					<< std::hex << _result << std::endl;
			}
		}

		~DiskDriveInfo()
		{
			_service.Reset();
			_locator.Reset();

			if (_uninitializeRequired)
			{
				CoUninitialize();
			}
		}

		// https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/win32-diskdrive
		std::vector<Drive> DiskDrives()
		{
			if (FAILED(_result))
			{
				return {};
			}

			ComPtr<IEnumWbemClassObject> enumerator;

			_result = _service->ExecQuery(
				BSTR(L"WQL"),
				BSTR(L"SELECT Caption, DeviceID, Partitions, Size FROM Win32_DiskDrive"),
				WBEM_FLAG_FORWARD_ONLY,
				nullptr,
				&enumerator);

			if (FAILED(_result))
			{
				std::cerr << "IWbemServices::ExecQuery failed: 0x"
					<< std::hex << _result << std::endl;
				return {};
			}

			std::vector<Drive> drives;

			while (SUCCEEDED(_result))
			{
				ComPtr<IWbemClassObject> classObject;
				ULONG numElems;
				_result = enumerator->Next(WBEM_INFINITE, 1, &classObject, &numElems);

				if (FAILED(_result))
				{
					std::cerr << "IEnumWbemClassObject::Next failed: 0x"
						<< std::hex << _result << std::endl;
					return drives;
				}

				if (_result == WBEM_S_FALSE)
				{
					return drives;
				}

				Drive drive;

				drive.Description = Value<std::string>(classObject.Get(), L"Caption", VT_BSTR);
				drive.Path = Value<std::string>(classObject.Get(), L"DeviceID", VT_BSTR);
				drive.Partitions = Value<uint32_t>(classObject.Get(), L"Partitions", VT_I4);
				drive.Capacity = Value<uint64_t>(classObject.Get(), L"Size", VT_BSTR);

				drives.emplace_back(drive);
			}

			return drives;
		}

	private:
		template <typename T, size_t N>
		T Value(
			IWbemClassObject* classObject,
			const wchar_t(&name)[N],
			VARTYPE type)
		{
			std::any result;
			ComVariant comVariant;

			_result = classObject->Get(name, 0, comVariant, nullptr, nullptr);

			if (FAILED(_result) || comVariant->vt == VT_NULL)
			{
				std::wcerr << L"Fecthing " << name << L" failed. HRESULT: 0x"
					<< std::hex << _result << std::endl;

				return T();
			}

			assert(type == comVariant->vt);

			switch (type)
			{
				case VT_BOOL:
					result = comVariant->boolVal ? true : false;
					break;
				case VT_I4:
					result = comVariant->uintVal;
					break;
				case VT_BSTR:
					if (typeid(T) == typeid(std::string))
					{
						result = ToUtf8(comVariant->bstrVal);
					}
					else
					{
						// Sometimes large integer values are actually returned as strings.
						// Thanks again Microsoft ,,|,
						result = wcstoumax(comVariant->bstrVal, nullptr, 10);
					}
					break;
				default:
					std::cerr << type << " not handled!" << std::endl;
					break;
			}

			return std::any_cast<T>(result);
		}

		bool _uninitializeRequired = false;
		HRESULT _result;
		ComPtr<IWbemLocator> _locator;
		ComPtr<IWbemServices> _service;
	};

	std::vector<Drive> DriveInfo()
	{
		return DiskDriveInfo().DiskDrives();
	}
}

