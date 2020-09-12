#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <SDKDDKVer.h>
#include <Windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <filesystem>
#include <random>
#include <sstream>
#include <limits>
#include <numeric>

#if defined(_WIN32) || defined(_WIN64)
#define LastError GetLastError()
#else
#define LastError errno
#endif