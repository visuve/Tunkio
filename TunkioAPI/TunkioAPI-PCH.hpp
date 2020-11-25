#pragma once

#if defined(_WIN32)
#include <Windows.h>
#undef min
#undef max
#elif defined(__linux__)
#include <linux/fs.h>
#else
#include <sys/disk.h>
#endif

#if !defined(_WIN32)
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <cassert>
#include <cstring>
#include <filesystem>
#include <iomanip>
#include <limits>
#include <numeric>
#include <memory>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#if defined(_WIN32)
#define LastError GetLastError()
#else
#define LastError errno
#endif
