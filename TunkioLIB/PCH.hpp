#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <random>
#include <sstream>
#include <chrono>
#include <array>