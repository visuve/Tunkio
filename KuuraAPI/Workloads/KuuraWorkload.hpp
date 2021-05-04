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

		virtual uint64_t Size() = 0;
		virtual std::pair<bool, uint64_t> Run(const std::vector<std::shared_ptr<IFillProvider>>& fillers) = 0;

		const std::filesystem::path Path;

	protected:
		const CallbackContainer* _callbacks;
		const bool _removeAfterOverwrite = false;

		bool Overwrite(
			uint16_t pass,
			uint64_t bytesLeft,
			uint64_t& bytesWritten,
			std::shared_ptr<IFillProvider> provider,
			std::shared_ptr<IFillConsumer> consumer);
	};
}
