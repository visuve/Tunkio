#include "../PCH.hpp"
#include "../TunkioDriveInfo.hpp"

namespace Tunkio
{
	using Microsoft::WRL::ComPtr;

	class ComVariant
	{
	public:
		ComVariant()
		{
			VariantInit(&m_value);
		}

		~ComVariant()
		{
			VariantClear(&m_value);
		}

		operator VARIANT*()
		{
			return &m_value;
		}

		VARIANT* operator -> ()
		{
			return &m_value;
		}

	private:
		VARIANT m_value;
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
			m_result = CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY | COINIT_MULTITHREADED);
			m_uninitializeRequired = SUCCEEDED(m_result);

			if (FAILED(m_result) && m_result != RPC_E_CHANGED_MODE)
			{
				std::cerr << "CoInitializeEx failed: 0x"
					<< std::hex << m_result << std::endl;
				return;
			}

			m_result = CoInitializeSecurity(
				nullptr,
				-1,
				nullptr,
				nullptr,
				RPC_C_AUTHN_LEVEL_DEFAULT,
				RPC_C_IMP_LEVEL_IMPERSONATE,
				nullptr,
				EOAC_NONE,
				0);

			if (FAILED(m_result) && m_result != RPC_E_TOO_LATE)
			{
				std::cerr << "CoInitializeSecurity failed: 0x"
					<< std::hex << m_result << std::endl;
				return ;
			}

			m_result = CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_locator));

			if (FAILED(m_result))
			{
				std::cerr << "CoCreateInstance failed: 0x"
					<< std::hex << m_result << std::endl;
				return;
			}

			m_result = m_locator->ConnectServer(
				BSTR(L"root\\CIMV2"),
				nullptr,
				nullptr,
				nullptr,
				WBEM_FLAG_CONNECT_USE_MAX_WAIT,
				nullptr,
				nullptr,
				&m_service);

			if (FAILED(m_result))
			{
				std::cerr << "IWbemLocator::ConnectServer failed: 0x"
					<< std::hex << m_result << std::endl;
				return;
			}

			m_result = CoSetProxyBlanket(
				m_service.Get(),
				RPC_C_AUTHN_WINNT,
				RPC_C_AUTHZ_NONE,
				nullptr,
				RPC_C_AUTHN_LEVEL_CALL,
				RPC_C_IMP_LEVEL_IMPERSONATE,
				nullptr,
				EOAC_NONE
			);

			if (FAILED(m_result))
			{
				std::cerr << "CoSetProxyBlanket failed: 0x"
					<< std::hex << m_result << std::endl;
			}
		}

		~DiskDriveInfo()
		{
			m_service->Release();
			m_locator->Release();

			if (m_uninitializeRequired)
			{
				CoUninitialize();
			}
		}

		// https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/win32-diskdrive
		std::vector<Drive> DiskDrives()
		{
			if (FAILED(m_result))
			{
				return {};
			}

			ComPtr<IEnumWbemClassObject> enumerator;

			m_result = m_service->ExecQuery(
				BSTR(L"WQL"),
				BSTR(L"SELECT Caption, DeviceID, Partitions, Size FROM Win32_DiskDrive"),
				WBEM_FLAG_FORWARD_ONLY,
				nullptr,
				&enumerator);

			if (FAILED(m_result))
			{
				std::cerr << "IWbemServices::ExecQuery failed: 0x"
					<< std::hex << m_result << std::endl;
				return {};
			}

			std::vector<Drive> drives;

			while (SUCCEEDED(m_result))
			{
				ComPtr<IWbemClassObject> classObject;
				ULONG numElems;
				m_result = enumerator->Next(WBEM_INFINITE, 1, &classObject, &numElems);

				if (FAILED(m_result))
				{
					std::cerr << "IEnumWbemClassObject::Next failed: 0x"
						<< std::hex << m_result << std::endl;
					return drives;
				}

				if (m_result == WBEM_S_FALSE)
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

			m_result = classObject->Get(name, 0, comVariant, nullptr, nullptr);

			if (FAILED(m_result) || comVariant->vt == VT_NULL)
			{
				std::wcerr << L"Fecthing " << name << L" failed. HRESULT: 0x"
					<< std::hex << m_result << std::endl;

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

		bool m_uninitializeRequired = false;
		HRESULT m_result;
		ComPtr<IWbemLocator> m_locator;
		ComPtr<IWbemServices> m_service;
	};

	std::vector<Drive> DriveInfo()
	{
		return DiskDriveInfo().DiskDrives();
	}
}

