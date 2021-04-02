#pragma once

#include "../KuuraAPI.h"
#include "../KuuraComposer.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"
#include "../FillConsumers/KuuraFillConsumer.hpp"

namespace Kuura
{
	class IWorkload
	{
	public:
		IWorkload(
			const Composer* parent,
			const std::filesystem::path& path,
			bool removeAfterOverwrite);

		virtual ~IWorkload();

		virtual bool Run() = 0;

		bool VerifyPass = false;

	protected:
		const std::filesystem::path _path;
		const bool _removeAfterOverwrite = false;

		bool Fill(
			uint16_t pass,
			uint64_t bytesLeft,
			uint64_t& bytesWritten,
			std::shared_ptr<IFillProvider> filler,
			std::shared_ptr<IFillConsumer> fillable);

		const Composer* _parent;

	private:
		std::queue<std::shared_ptr<IFillProvider>> _fillers;
	};
}
