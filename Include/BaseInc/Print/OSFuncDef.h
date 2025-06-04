#ifndef __FRAMEWORK_INCLUDE_PRINT_OSFUNCDEF_H__
#define __FRAMEWORK_INCLUDE_PRINT_OSFUNCDEF_H__

#ifdef WIN32
#define vsn_printf _vsnprintf
#else
#define vsn_printf vsnprintf
#endif
// color def
// support crt vt100
#undef COLOR_NONE
#undef COLOR_BLUE
#undef COLOR_GREEN
#undef COLOR_YELLOW
#undef COLOR_RED
#undef COLOR_LIGHT_RED
#ifdef WIN32
#define COLOR_NONE      7
#define COLOR_BLUE      1
#define COLOR_GREEN     2
#define COLOR_YELLOW    6
#define COLOR_RED       4
#define COLOR_LIGHT_RED 5
#else
#define COLOR_NONE         "\033[m"
#define COLOR_BLUE         "\033[0;34m"
#define COLOR_PURPLE       "\033[0;35m"
#define COLOR_SKYBLUE      "\033[0;36m"
#define COLOR_GREEN        "\033[1;32m"
#define COLOR_YELLOW       "\033[1;33m"
#define COLOR_RED          "\033[0;31m"
#define COLOR_LIGHT_RED    "\033[1;31m"
#endif

typedef unsigned int uint;

#endif /* ifndef __FRAMEWORK_INCLUDE_PRINT_OSFUNCDEF_H__ */
