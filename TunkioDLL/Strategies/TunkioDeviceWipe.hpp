#pragma once

#include "TunkioAPI.h"
#include "ITunkioOperation.hpp"

namespace Tunkio
{
    class DeviceWipeImpl;

    class DeviceWipe : public IOperation
    {
    public:
        DeviceWipe(const TunkioOptions* options);
        ~DeviceWipe();

        bool Run() override;
    protected:
        bool Fill() override;
        bool Remove() override;
    private:
        DeviceWipeImpl* m_impl = nullptr;
    };
}