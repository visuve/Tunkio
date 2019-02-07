#pragma once

#include <functional>

namespace WDW::Mock
{
    void SetFakeWrite(std::function<bool(unsigned long*)> fakeWrite);
}