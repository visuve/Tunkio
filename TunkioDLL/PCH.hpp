#pragma once

#if defined(_WIN32)
#include <SDKDDKVer.h>
#include <Windows.h>
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

#include <filesystem>
#include <iostream>
#include <iomanip>
#include <limits>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#if defined(_WIN32)
#define LastError GetLastError()
#else
#define LastError errno
#endif
