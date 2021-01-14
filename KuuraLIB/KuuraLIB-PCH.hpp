#pragma once

#if defined(_WIN32)
#include <Windows.h>
#undef min
#undef max
#else
#include <errno.h>
#endif

#include <any>
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cinttypes>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <random>
#include <utility>
#include <vector>
