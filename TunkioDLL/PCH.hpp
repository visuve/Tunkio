#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <SDKDDKVer.h>
#include <Windows.h>
#else

#include <fcntl.h>
#include <linux/fs.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <bitset>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <limits>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
#define LastError GetLastError()
#else
#define LastError errno
#endif