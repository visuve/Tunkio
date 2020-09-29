#pragma once

#include "TunkioAPI.h"

namespace Tunkio
{
	class IOperation
	{
	public:
		IOperation(const std::string& path) :
			m_path(path)
		{
		}

		virtual ~IOperation() = default;
		virtual bool Run() = 0;

		TunkioFillMode m_fillMode = TunkioFillMode::Zeroes;
		TunkioStartedCallback* m_startedCallback = nullptr;
		TunkioProgressCallback* m_progressCallback = nullptr;
		TunkioErrorCallback* m_errorCallback = nullptr;
		TunkioCompletedCallback* m_completedCallback = nullptr;		
		bool m_removeAfterFill = false;

	protected:
		const std::string m_path;
	};
}