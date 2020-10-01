#pragma once

#if defined(_WIN32)
#include <SDKDDKVer.h>
#include <Windows.h>
#else
#include <sys/wait.h>
#endif

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <csignal>
#include <filesystem>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "TunkioAPI.h"