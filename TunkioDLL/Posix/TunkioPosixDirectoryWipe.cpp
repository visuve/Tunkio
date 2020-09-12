#include "../PCH.hpp"
#include "../TunkioErrorCodes.hpp"
#include "../TunkioDirectoryWipe.hpp"
#include "../TunkioFillStrategy.hpp"
#include "TunkioPosixWipe.hpp"

namespace Tunkio
{
	struct PosixFile
	{
		std::filesystem::path Path;
		int Descriptor = 0;
		uint64_t Size = 0;

		PosixFile(const std::filesystem::path& path) :
			Path(path),
			Descriptor(PosixWipe::Open(path))
		{
			if (Descriptor == -1)
			{
				return;
			}

			struct stat64 buffer = { 0 };

			if (fstat64(Descriptor, &buffer) != 0)
			{
				return;
			}

			Size = buffer.st_size;
		}

		~PosixFile()
		{
			if (Descriptor)
			{
				close(Descriptor);
			}
		}

		bool IsValid() const
		{
			return Descriptor > 0 && Size > 0;
		}

		bool Delete()
		{
			if (close(Descriptor))
			{
				Descriptor = 0;
				return remove(Path.c_str()) == 0;
			}

			return false;
		}
	};

	class PosixDirectoryWipe : public IOperation
	{
	public:
		PosixDirectoryWipe(const TunkioOptions* options) :
			IOperation(options)
		{
		}

		~PosixDirectoryWipe()
		{
		}

		bool Run() override
		{
			if (!Open())
			{
				ReportError(errno);
				return false;
			}

			m_totalSize = Size();

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

				const PosixFile file(entry.path());

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
			const auto sum = [](uint64_t sum, const PosixFile& a)
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

			for (const PosixFile& file : m_files)
			{
				while (bytesLeft)
				{
					if (fakeData.Size<uint64_t>() > bytesLeft)
					{
						fakeData.Resize(bytesLeft);
					}

					ssize_t bytesWritten =
						write(file.Descriptor, fakeData.Front(), fakeData.Size<size_t>());

					if (!bytesWritten)
					{
						ReportError(errno);
						return false;
					}

					m_totalBytesWritten += bytesWritten;
					bytesLeft -= bytesWritten;

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
			return false;
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
		std::vector<PosixFile> m_files;
	};

	DirectoryWipe::DirectoryWipe(const TunkioOptions* options) :
		IOperation(options),
		m_impl(new PosixDirectoryWipe(options))
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