#include "PCH.hpp"
#include "TunkioFillStrategy.hpp"

namespace Tunkio
{
    void Random(std::vector<uint8_t>& data)
    {
        thread_local std::random_device device;
        thread_local std::default_random_engine engine(device());
        thread_local std::uniform_int_distribution<uint16_t> distribution(0, 255); // uint8_t is not supported

        for (size_t i = 0; i < data.size(); ++i)
        {
            data[i] = static_cast<uint8_t>(distribution(engine));
        }
    }

    FillStrategy::FillStrategy(const TunkioMode mode, const size_t size) :
        m_mode(mode)
    {
        switch (mode)
        {
            case TunkioMode::Zeroes:
                m_data = std::vector<uint8_t>(size, 0);
                break;
            case TunkioMode::Ones:
                m_data = std::vector<uint8_t>(size, 1);
                break;
            case TunkioMode::Random:
                m_data = std::vector<uint8_t>(size);
                Random(m_data);
                break;
        }
    }

    const uint8_t* FillStrategy::Front()
    {
        switch (m_mode)
        {
            case TunkioMode::Random:
                Random(m_data);
                break;
        }

        return m_data.data();
    }
}