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

        uint32_t Run() override;
    protected:
        bool Exists() override;
        uint64_t Size() override;
        bool Fill() override;
        bool Remove() override;
    private:
        DeviceWipeImpl* m_impl = nullptr;
    };
}