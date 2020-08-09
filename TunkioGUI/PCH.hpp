#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <SDKDDKVer.h>
#include <Windows.h>
#endif

#include <nana/gui.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/progress.hpp>
#include <nana/gui/widgets/button.hpp>

#include <iostream>
#include <sstream>
#include <future>
#include <array>

#include "TunkioAPI.h"
#include "TunkioArgs.hpp"
#include "TunkioMemory.hpp"
#include "TunkioDataUnits.hpp"