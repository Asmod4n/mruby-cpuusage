/*
** cpuusage.c - Platform-specific CPU usage implementation
*/

#include "../include/cpuusage.h"

/* Platform detection */
#if defined(_WIN32) || defined(_WIN64)
  #define PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
  #define PLATFORM_MACOS
#elif defined(__linux__)
  #define PLATFORM_LINUX
#elif defined(__unix__) || defined(__unix)
  #define PLATFORM_UNIX
#else
  #define PLATFORM_UNKNOWN
#endif

/* Platform-specific includes */
#ifdef PLATFORM_WINDOWS
  #include <windows.h>
#else
  #include <sys/time.h>
  #include <sys/resource.h>
  #include <unistd.h>
#endif

/*
** Get CPU usage snapshot for current process
** Returns 0 on success, -1 on failure
*/
int
cpuusage_snapshot(cpu_snapshot_t *snapshot)
{
  if (!snapshot) {
    return -1;
  }

#ifdef PLATFORM_WINDOWS
  /* Windows implementation using GetProcessTimes */
  FILETIME createTime, exitTime, kernelTime, userTime;
  ULARGE_INTEGER kernelInt, userInt;
  
  if (!GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, 
                       &kernelTime, &userTime)) {
    return -1;
  }
  
  /* Convert FILETIME (100-nanosecond intervals) to seconds */
  kernelInt.LowPart = kernelTime.dwLowDateTime;
  kernelInt.HighPart = kernelTime.dwHighDateTime;
  userInt.LowPart = userTime.dwLowDateTime;
  userInt.HighPart = userTime.dwHighDateTime;
  
  snapshot->system_time = (double)kernelInt.QuadPart / 10000000.0;
  snapshot->user_time = (double)userInt.QuadPart / 10000000.0;
  
  return 0;

#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS) || defined(PLATFORM_UNIX)
  /* Unix-like systems using getrusage */
  struct rusage usage;
  
  if (getrusage(RUSAGE_SELF, &usage) != 0) {
    return -1;
  }
  
  /* Convert timeval to seconds */
  snapshot->user_time = (double)usage.ru_utime.tv_sec + 
                        (double)usage.ru_utime.tv_usec / 1000000.0;
  snapshot->system_time = (double)usage.ru_stime.tv_sec + 
                          (double)usage.ru_stime.tv_usec / 1000000.0;
  
  return 0;

#else
  /* Fallback for unknown platforms - return zeros */
  snapshot->user_time = 0.0;
  snapshot->system_time = 0.0;
  return 0;
#endif
}
