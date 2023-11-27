/**
 * @file
 * @author Vinícius Gabriel Linden
 * @date 2022-04-07
 * @copyright nubix Software-Design GmbH 2022
 * @brief Debugging macros.
 */

#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <cstdio>
#include <cstring>

#define STRING2(x) #x
#define STRING(x) STRING2(x)

//! File name macro.
#define __DEBUGFILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef DEBUG

#define DEBUG_INFO(str, args...) printf("\e[1m[INFO] %s:%u@%s(): " str "\e[0m\n", __DEBUGFILENAME__, __LINE__, __FUNCTION__, ##args)
#define DEBUG_INFO_INT(var) printf("\e[1m[INFO INT] %s:%u@%s(): " #var " = %ld\e[0m\n", __DEBUGFILENAME__, __LINE__, __FUNCTION__, var)
#define DEBUG_INFO_FLOAT(var) printf("\e[1m[INFO FLOAT] %s:%u@%s(): " #var " = %f\e[0m\n", __DEBUGFILENAME__, __LINE__, __FUNCTION__, var)
#define DEBUG_INFO_HEX(var) printf("\e[1m[INFO HEX] %s:%u@%s(): " #var " = 0x%0x\e[0m\n", __DEBUGFILENAME__, __LINE__, __FUNCTION__, var)

#define DEBUG_WARN(str, args...) printf("\e[1;33m[WARN] %s:%u@%s(): " str "\e[0m\n", __DEBUGFILENAME__, __LINE__, __FUNCTION__, ##args)
#define DEBUG_WARN_INT(var) printf("\e[1;33m[WARN INT] %s:%u@%s(): " #var " = %ld\e[0m\n", __DEBUGFILENAME__, __LINE__, __FUNCTION__, var)
#define DEBUG_WARN_FLOAT(var) printf("\e[1;33m[WARN FLOAT] %s:%u:%s: " #var " = %f\e[0m\n", __DEBUGFILENAME__, __LINE__, __FUNCTION__, var)
#define DEBUG_WARN_HEX(var) printf("\e[1;33m[WARN HEX] %s:%u:%s: " #var " = 0x%0x\e[0m\n", __DEBUGFILENAME__, __LINE__, __FUNCTION__, var)

#define DEBUG_ERROR(str, args...) printf("\e[1;31m[ERROR] %s:%u@%s(): " str "\e[0m\n", __DEBUGFILENAME__, __LINE__, __FUNCTION__, ##args)
#define DEBUG_ERROR_INT(var) printf("\e[1;31m[ERROR INT] %s:%u:%s: " #var " = %ld\e[0m\n", __DEBUGFILENAME__, __LINE__, __FUNCTION__, var)
#define DEBUG_ERROR_FLOAT(var) printf("\e[1;31m[ERROR FLOAT] %s:%u:%s: " #var " = %f\e[0m\n", __DEBUGFILENAME__, __LINE__, __FUNCTION__, var)
#define DEBUG_ERROR_HEX(var) printf("\e[1;31m[ERROR HEX] %s:%u:%s: " #var " = 0x%0x\e[0m\n", __DEBUGFILENAME__, __LINE__, __FUNCTION__, var)

#else /* DEBUG */

#define DEBUG_INFO(str, args...)
#define DEBUG_INFO_INT(var)
#define DEBUG_INFO_FLOAT(var)
#define DEBUG_INFO_HEX(var)

#define DEBUG_WARN(str, args...)
#define DEBUG_WARN_INT(var)
#define DEBUG_WARN_FLOAT(var)
#define DEBUG_WARN_HEX(var)

#define DEBUG_ERROR(str, args...)
#define DEBUG_ERROR_INT(var)
#define DEBUG_ERROR_FLOAT(var)
#define DEBUG_ERROR_HEX(var)

#endif /* not DEBUG */

/**
 * @brief Clear debug screen.
 */
void debug_clear_screen();

#endif /* DEBUG_HPP */
