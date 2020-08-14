#pragma once

#include "../TunkioOperation.hpp"
#include "TunkioPosixAutoHandle.hpp"

namespace Tunkio
{
	class PosixWipe : public IOperation
	{
	public:
		PosixWipe(const TunkioOptions* options);
		~PosixWipe();

		virtual uint64_t Size() = 0;
		virtual bool Run() = 0;
		virtual bool Remove() = 0;

		bool Open() override;
		bool Fill() override;

		void ReportStarted() const;
		bool ReportProgress() const;
		void ReportComplete() const;
		void ReportError(uint32_t error) const;

	protected:
		uint64_t m_size = 0;
		uint64_t m_totalBytesWritten = 0;
		PosixAutoHandle m_handle;
	};
}