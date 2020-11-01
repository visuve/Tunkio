#pragma once

#include "../FillProviders/TunkioFillProvider.hpp"

namespace Tunkio
{
	class IWorkload
	{
	public:
		IWorkload(const std::string& path) :
			m_path(path)
		{
		}

		virtual ~IWorkload()
		{
			assert(m_fillers.empty());
		}

		virtual bool Run() = 0;

		std::queue<std::shared_ptr<IFillProvider>> m_fillers;
		TunkioStartedCallback* m_startedCallback = nullptr;
		TunkioProgressCallback* m_progressCallback = nullptr;
		TunkioErrorCallback* m_errorCallback = nullptr;
		TunkioCompletedCallback* m_completedCallback = nullptr;
		bool m_verifyAfterWipe = false;
		bool m_removeAfterFill = false;

	protected:
		const std::string m_path;
	};
}