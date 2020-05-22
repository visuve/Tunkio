#pragma once

/*#pragma warning(push)
#pragma warning(disable: 4244)
#pragma warning(disable: 4458)
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.h>
#include <FL/Fl_Button.h>
#include <FL/Fl_Progress.h>
#pragma warning(pop)*/

#define WIN32_LEAN_AND_MEAN
#include <SDKDDKVer.h>
#include <Windows.h>


#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/progress.hpp>
#include <nana/gui/widgets/button.hpp>

#include <iostream>
#include <map>

#include "TunkioAPI.h"
#include "TunkioArgs.hpp"
#include "TunkioMemory.hpp"
#include "TunkioDataUnits.hpp"