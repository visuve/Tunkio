#pragma once

#include <functional>

namespace Tunkio::IO::Mock
{
    void SetFakeWrite(std::function<bool(unsigned long* writtenBytes)> fakeWrite);
}