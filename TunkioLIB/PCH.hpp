#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#endif

#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <random>
#include <vector>