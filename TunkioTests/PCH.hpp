#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <SDKDDKVer.h>
#include <Windows.h>
#endif

#include <gtest/gtest.h>

#include <vector>
#include <functional>
#include <string>
#include <filesystem>
#include <regex>
#include <random>
#include <iomanip>
#include <sstream>
#include <array>