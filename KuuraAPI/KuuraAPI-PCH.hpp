#pragma once

#if !defined(_WIN32)
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#if defined(_WIN32)
#include <Windows.h>
#undef min
#undef max
#elif defined(__linux__)
#include <linux/fs.h>
#else
#include <sys/disk.h>
#endif

#include <cassert>
#include <cstring>
#include <cstddef>
#include <cuchar>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <numeric>
#include <memory>
#include <optional>
#include <queue>
#include <random>
#include <span>
#include <sstream>
#include <string>
#include <vector>

#if defined(_WIN32)
#define LastError GetLastError()
#else
#define LastError errno
#endif
