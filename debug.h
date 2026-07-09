#ifndef DEBUG_H
#define DEBUG_H

/**
 * DESCRIPTION:
 * signal handling and debug printing are optional features,
 * not required by the subject, that simplify debugging
 */

#include <signal.h>

// Define a global variable for a clean exit strategy by signal-handling
extern volatile sig_atomic_t g_signalnum;

// Define the debug print mode as a C preprocessor macro.
// If DEBUG is defined, the DEBUG_PRINT macro will print the provided message (variadic macro). Otherwise, it will do nothing.
#ifdef DEBUG
#define DEBUG_PRINT(...)                    \
	do                                      \
	{                                       \
		printf("DEBUG_INFO: " __VA_ARGS__); \
	} while (0)
#else
#define DEBUG_PRINT(...) \
	do                   \
	{                    \
	} while (0)
#endif

#endif
