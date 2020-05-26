#pragma once

#include "TunkioAPI.h"

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
        virtual bool Open() = 0;
        virtual bool Fill() = 0;

        const TunkioOptions* m_options;
    };
}