// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

// Windows headers
#include "framework.h"

// C++ Standard Library
#include <cassert>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>


// Project-local libraries
#include "detours/detours.h"

// Project type aliases
using address = uintptr_t;

// Project
#include "invasive_hooks/invasive_hooks.hpp"

#endif //PCH_H
