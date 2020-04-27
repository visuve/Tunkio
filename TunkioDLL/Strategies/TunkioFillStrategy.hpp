#pragma once

#include <vector>
#include "TunkioAPI.h"

namespace Tunkio
{
    // TODO: turn into a generator class
    // std::vector<uint8_t> InitByMode(const TunkioMode mode, const size_t size);
    class FillStrategy
    {
    public:
        FillStrategy(const TunkioMode mode, const size_t size);

        template<typename T>
        T Size() const
        {
            return static_cast<T>(m_data.size());
        }

        template<typename T>
        void Resize(T size)
        {
            m_data.resize(static_cast<size_t>(size));
        }

        const uint8_t* Front();

    private:
        const TunkioMode m_mode;
        std::vector<uint8_t> m_data;
    };
}