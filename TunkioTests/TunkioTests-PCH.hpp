#pragma once

#if defined(_WIN32)
#include <Windows.h>
#include <debugapi.h>
#undef min
#undef max
#endif 


#include <gtest/gtest.h>

#include <array>
#include <filesystem>
#include <functional>
#include <chrono>
#include <cuchar>
#include <iomanip>
#include <memory>
#include <queue>
#include <random>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "../TunkioAPI/TunkioAPI.h"
#include "../TunkioLIB/TunkioDataUnits.hpp"
#include "../TunkioLIB/TunkioTime.hpp"
#include "../TunkioCLI/TunkioArgument.hpp"
