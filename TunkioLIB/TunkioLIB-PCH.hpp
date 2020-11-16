#pragma once

#if defined(_WIN32)
#include <Windows.h>
#include <WbemCli.h>
#include <wrl/client.h>
#undef max
#elif defined(__linux__)
#include <linux/fs.h>
#include <linux/hdreg.h>
#else
#include <sys/disk.h>
#include <libgeom.h>
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

#include <algorithm>
#include <any>
#include <array>
#include <cassert>
#include <chrono>
#include <cinttypes>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <typeindex>
#include <random>
#include <utility>
#include <variant>
#include <vector>