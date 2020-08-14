#include "../PCH.hpp"
#include "../TunkioErrorCodes.hpp"
#include "../TunkioDirectoryWipe.hpp"
#include "../TunkioFillStrategy.hpp"
#include "TunkioWin32AutoHandle.hpp"
#include "TunkioWin32Wipe.hpp"

namespace Tunkio
{
	struct Win32File
	{
		std::filesystem::path Path;
		HANDLE Handle = nullptr;
		uint64_t Size = 0;

		Win32File(const std::filesystem::path& path) :
			Path(path),
			Handle(Win32Wipe::Open(path.string()))
		{
			if (Handle == INVALID_HANDLE_VALUE)
			{
				return;
			}

			LARGE_INTEGER fileSize = { 0 };

			if (!GetFileSizeEx(Handle, &fileSize))
			{
				return;
			}

			Size = fileSize.QuadPart;
		}

		~Win32File()
		{
			if (Handle) 
			{
				CloseHandle(Handle);
			}
		}

		bool IsValid() const
		{
			return Handle != INVALID_HANDLE_VALUE && Size > 0;
		}

		bool Delete()
		{
			if (CloseHandle(Handle))
			{
				Handle = nullptr;
				return DeleteFileW(Path.c_str());
			}

			return false;
		}
	};

	class Win32DirectoryWipe : public IOperation
	{
	public:
		Win32DirectoryWipe(const TunkioOptions* options) :
			IOperation(options)
		{
		}

		~Win32DirectoryWipe()
		{
		}

		bool Run() override
		{
			if (!Open())
			{
				ReportError(GetLastError());
				return false;
			}

			m_totalSize  = Size();

			if (!m_totalSize)
			{
				return false;
			}

			return Fill();
		}

		bool Open() override
		{
			const std::string path(m_options->Path.Data, m_options->Path.Length);

			for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
			{
				if (!entry.is_regular_file())
				{
					continue;
				}

				const Win32File file(entry.path());

				if (file.IsValid())
				{
					return false;
				}

				m_files.emplace_back(file);
			}

			return !m_files.empty();
		}

		uint64_t Size() override
		{
			const auto sum = [](uint64_t sum, const Win32File& a)
			{
				return sum + a.Size;
			};

			const uint64_t combinedSize = 
				std::accumulate(m_files.cbegin(), m_files.cend(), uint64_t(0), sum);

			if (!combinedSize)
			{
				ReportError(ErrorCode::NoData);
			}

			return combinedSize;
		}

		bool Fill() override
		{
			uint64_t bytesLeft = m_totalSize;
			FillStrategy fakeData(m_options->Mode, DataUnit::Mebibyte(1));

			ReportStarted();

			for (const Win32File& file : m_files)
			{
				while (bytesLeft)
				{
					if (fakeData.Size<uint64_t>() > bytesLeft)
					{
						fakeData.Resize(bytesLeft);
					}

					DWORD bytesWritten = 0u;

					const bool result = WriteFile(file.Handle,
						fakeData.Front(),
						fakeData.Size<uint32_t>(),
						&bytesWritten,
						nullptr);

					m_totalBytesWritten += bytesWritten;
					bytesLeft -= bytesWritten;

					if (!result)
					{
						ReportError(GetLastError());
						return false;
					}

					if (!ReportProgress())
					{
						return true;
					}
				}
			}

			ReportComplete();
			return true;
		}

		bool Remove() override
		{
			for (Win32File& file : m_files)
			{
				if (!file.Delete())
				{
					return false;
				}
			}

			return true;
		}

		void ReportStarted() const
		{
			m_options->Callbacks.StartedCallback(m_totalSize);
		}

		bool ReportProgress() const
		{
			return m_options->Callbacks.ProgressCallback(m_totalBytesWritten);
		}

		void ReportComplete() const
		{
			m_options->Callbacks.CompletedCallback(m_totalBytesWritten);
		}

		void ReportError(uint32_t error) const
		{
			m_options->Callbacks.ErrorCallback(error, m_totalBytesWritten);
		}

	private:
		uint64_t m_totalSize = 0;
		uint64_t m_totalBytesWritten = 0;
		std::vector<Win32File> m_files;
	};

	DirectoryWipe::DirectoryWipe(const TunkioOptions* options) :
		IOperation(options),
		m_impl(new Win32DirectoryWipe(options))
	{
	}

	DirectoryWipe::~DirectoryWipe()
	{
		delete m_impl;
	}

	bool DirectoryWipe::Run()
	{
		return m_impl->Run();
	}

	bool DirectoryWipe::Open()
	{
		return m_impl->Open();
	}

	uint64_t DirectoryWipe::Size()
	{
		return m_impl->Size();
	}

	bool DirectoryWipe::Fill()
	{
		return m_impl->Fill();
	}

	bool DirectoryWipe::Remove()
	{
		return m_impl->Remove();
	}
}