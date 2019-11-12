#pragma once

#include "TunkioAPI.h"
#include <cstdint>

namespace Tunkio
{
    class IOperation
    {
    public:
        inline IOperation(const TunkioOptions* options) :
            m_options(options)
        {
        }

        virtual ~IOperation() = default;

        virtual uint32_t Run() = 0;

    protected:
        virtual bool Exists() = 0;
        virtual uint64_t Size() = 0;
        virtual bool Fill() = 0;
        virtual bool Remove() = 0;
        const TunkioOptions* m_options;
    };
}