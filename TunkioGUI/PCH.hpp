#pragma once

#define WIN32_LEAN_AND_MEAN
#include <SDKDDKVer.h>
#include <Windows.h>


#include <nana/gui.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/progress.hpp>
#include <nana/gui/widgets/button.hpp>

#include <iostream>
#include <sstream>
#include <map>
#include <future>

#include "TunkioAPI.h"
#include "TunkioArgs.hpp"
#include "TunkioMemory.hpp"
#include "TunkioDataUnits.hpp"