#pragma once
#include <string> 
#include <vector>

// -- Debug assert --
#ifdef _DEBUG

#ifdef BUILD_PLATFORM_WINDOWS
#include <windows.h>

#define LB_ASSERT(condition, msg)                                               \
    do                                                                          \
    {                                                                           \
        if (!(condition))                                                       \
        {                                                                       \
            std::cout << "Error in '" << __FILE__ << "' at line #" << __LINE__; \
            std::cout << "\n  --> " << msg << "\n";                             \
            MessageBox(                                                         \
                NULL,                                                           \
                msg,                                                            \
                "Error",                                                        \
                MB_OK | MB_ICONERROR);                                          \
            __debugbreak();                                                     \
        }                                                                       \
    } while (0)
#else
#define LB_ASSERT(condition, msg)                                               \
    do                                                                          \
    {                                                                           \
        if (!(condition))                                                       \
        {                                                                       \
            std::cout << "Error in '" << __FILE__ << "' at line #" << __LINE__; \
            std::cout << "\n  --> " << msg << "\n";                             \
            __debugbreak();                                                     \
        }                                                                       \
    } while (0)
#endif

#else
#define LB_ASSERT(condition, msg)                   \
    do                                              \
    {                                               \
        if (!(condition))                           \
        {                                           \
            std::cout << "\n  --> " << msg << "\n"; \
        }                                           \
    } while (0)

#endif // _DEBUG

// Return a lowercase version of the string
std::string to_lower_case(const std::string &str);

// Split a string along a delimiter char
std::vector<std::string> split(const std::string &str, char delim);

// Can this string be converted into a float?
bool is_float(const std::string &str);

// Convert float to string with 2 decimal point formatting
std::string float_to_fmtstring(float f);