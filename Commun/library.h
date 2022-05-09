#pragma once

#ifdef _MSC_VER

#include <Windows.h>
#define HLIB HINSTANCE
#define LOADLIB(file) LoadLibraryA(file)
#define GETPROC(handler, func) GetProcAddress(handler, func)
#define CLOSELIB(handler) FreeLibrary(handler)
#define API_EXPORT __declspec(dllexport)

#else

#include <dlfcn.h>
#define HLIB void*
#define LOADLIB(file) dlopen(file, RTLD_LAZY)
#define GETPROC(handler, func) dlsym(handler, func)
#define CLOSELIB(handler) dlclose(handler)
#define API_EXPORT

#endif
