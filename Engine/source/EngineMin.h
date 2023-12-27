#pragma once

#ifdef ENGINE_EXPORTS

#define YT_API __declspec(dllexport)

#else

#define YT_API __declspec(dllimport)

#endif 
