#pragma once

#include "TunkioAPI.h"
#include "ITunkioOperation.hpp"

namespace Tunkio
{
    class FileWipeImpl;

    class FileWipe : public IOperation
    {
    public:
        FileWipe(const TunkioOptions* options);
        ~FileWipe();

        uint32_t Run() override;
    protected:
        bool Exists() override;
        uint64_t Size() override;
        bool Fill() override;
        bool Remove() override;
    private:
        FileWipeImpl* m_impl = nullptr;
    };
}