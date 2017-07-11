#pragma once

#ifdef EXPMODULE
#define EXPORT_SYMBOL __declspec(dllexport)
#else
#define EXPORT_SYMBOL __declspec(dllimport)
#endif
