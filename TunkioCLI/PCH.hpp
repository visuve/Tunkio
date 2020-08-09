#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <SDKDDKVer.h>
#include <Windows.h>
#endif

#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <cassert>
#include <sstream>
#include <atomic>
#include <csignal>
#include <numeric>
#include <array>

#include "TunkioAPI.h"