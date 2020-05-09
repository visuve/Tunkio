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

        virtual bool Run() = 0;
        virtual bool Remove() = 0;

    protected:
        virtual bool Fill() = 0;

        const TunkioOptions* m_options;
    };
}