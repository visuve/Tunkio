#pragma once

#include "../TunkioOperation.hpp"
#include "TunkioWin32AutoHandle.hpp"

namespace Tunkio
{
	class Win32Wipe : public IOperation
	{
	public:
		Win32Wipe(const TunkioOptions* options);
		~Win32Wipe();

		virtual uint64_t Size() = 0;
		virtual bool Run() = 0;
		virtual bool Remove() = 0;

		bool Open() override;
		bool Fill() override;

		void ReportStarted() const;
		bool ReportProgress() const;
		void ReportComplete() const;
		void ReportError(uint32_t error) const;

		static HANDLE Open(const std::string& path);

	protected:
		uint64_t m_size = 0;
		uint64_t m_totalBytesWritten = 0;
		Win32AutoHandle m_handle;
	};
}