#pragma once

#include "../KuuraCallbackContainer.hpp"

namespace Kuura
{
	class IFillProvider;
	class IFillConsumer;

	class IWorkload
	{
	public:
		IWorkload(
			const CallbackContainer* callbacks,
			const std::filesystem::path& path,
			bool removeAfterOverwrite);

		virtual ~IWorkload();

		virtual bool Run(const std::vector<std::shared_ptr<IFillProvider>>& fillers) = 0;

		bool VerifyPass = false;

	protected:
		const CallbackContainer* _callbacks;
		const std::filesystem::path _path;
		const bool _removeAfterOverwrite = false;

		bool Overwrite(
			uint16_t pass,
			uint64_t bytesLeft,
			uint64_t& bytesWritten,
			std::shared_ptr<IFillProvider> provider,
			std::shared_ptr<IFillConsumer> consumer);
	};
}
