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
#include <iomanip>
#include <memory>
#include <queue>
#include <random>
#include <regex>
#include <span>
#include <sstream>
#include <string>
#include <vector>

#include "../KuuraAPI/KuuraAPI.h"
#include "../KuuraLIB/KuuraDataUnits.hpp"
#include "../KuuraLIB/KuuraTime.hpp"
#include "../KuuraCLI/KuuraArgument.hpp"
