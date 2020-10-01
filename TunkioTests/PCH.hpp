#pragma once

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <SDKDDKVer.h>
#include <Windows.h>
#endif

#include <gtest/gtest.h>

#include <array>
#include <functional>
#include <chrono>
#include <iomanip>
#include <random>
#include <regex>
#include <sstream>
#include <string>
#include <vector>