/*
** mrb_cpuusage.c - CPUUsage class
*/
/*
** cpuusage.c - Platform-specific CPU usage implementation
*/

#define _GNU_SOURCE  /* for RUSAGE_THREAD on Linux */

typedef struct {
  double user_time;    /* User CPU time in seconds */
  double system_time;  /* System CPU time in seconds */
  double child_user_time;
  double child_sys_time;
} cpu_snapshot_t;

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
  #include <sys/times.h>
  #include <unistd.h>
#endif

#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <mruby/presym.h>
#include <mruby/error.h>

#ifdef PLATFORM_LINUX
static long cpuusage_hz = 0;
static volatile int cpuusage_hz_init = 0;

static void
cpuusage_init(mrb_state *mrb)
{
  long hz = sysconf(_SC_CLK_TCK);
  if (hz <= 0) mrb_sys_fail(mrb, "sysconf(_SC_CLK_TCK);");
  cpuusage_hz = hz;
}
#endif
/*
** Get CPU usage snapshot
** Returns 0 on success, -1 on failure
**
** cpu_snapshot_t is expected to look like:
**
**   typedef struct {
**     double user_time;
**     double system_time;
**     double child_user_time;
**     double child_sys_time;
**   } cpu_snapshot_t;
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
  kernelInt.LowPart  = kernelTime.dwLowDateTime;
  kernelInt.HighPart = kernelTime.dwHighDateTime;
  userInt.LowPart    = userTime.dwLowDateTime;
  userInt.HighPart   = userTime.dwHighDateTime;

  snapshot->system_time     = (double)kernelInt.QuadPart / 10000000.0;
  snapshot->user_time       = (double)userInt.QuadPart   / 10000000.0;
  snapshot->child_user_time = 0.0;
  snapshot->child_sys_time  = 0.0;

  return 0;

#elif defined(PLATFORM_LINUX)
  /* Linux: per-thread CPU usage via getrusage(RUSAGE_THREAD) */
  struct rusage usage;

  if (getrusage(RUSAGE_THREAD, &usage) != 0) {
    return -1;
  }

  snapshot->user_time =
      (double)usage.ru_utime.tv_sec +
      (double)usage.ru_utime.tv_usec / 1000000.0;

  snapshot->system_time =
      (double)usage.ru_stime.tv_sec +
      (double)usage.ru_stime.tv_usec / 1000000.0;

  /* RUSAGE_THREAD has no child times */
  snapshot->child_user_time = 0.0;
  snapshot->child_sys_time  = 0.0;

  return 0;

#elif defined(PLATFORM_MACOS) || defined(PLATFORM_UNIX)
  /* Other POSIX: use times(2) and fill utime/stime/cutime/cstime */
  struct tms t;
  clock_t ticks = times(&t);

  if (ticks == (clock_t)-1 || cpuusage_hz <= 0) {
    return -1;
  }

  snapshot->user_time       = (double)t.tms_utime  / (double)cpuusage_hz;
  snapshot->system_time     = (double)t.tms_stime  / (double)cpuusage_hz;
  snapshot->child_user_time = (double)t.tms_cutime / (double)cpuusage_hz;
  snapshot->child_sys_time  = (double)t.tms_cstime / (double)cpuusage_hz;

  return 0;

#else
  errno = ENOSYS;
  /* Fallback for unknown platforms - return zeros */
  snapshot->user_time       = 0.0;
  snapshot->system_time     = 0.0;
  snapshot->child_user_time = 0.0;
  snapshot->child_sys_time  = 0.0;
  return -1;
#endif
}

static mrb_value
mrb_cpuusage_snapshot(mrb_state *mrb, mrb_value self)
{
  cpu_snapshot_t snapshot;
  mrb_value hash;

  if (cpuusage_snapshot(&snapshot) != 0) {
    mrb_sys_fail(mrb, "cpuusage_snapshot");
  }

  hash = mrb_hash_new_capa(mrb, 5);

  mrb_hash_set(mrb, hash, mrb_symbol_value(MRB_SYM(user_time)),
               mrb_float_value(mrb, snapshot.user_time));
  mrb_hash_set(mrb, hash, mrb_symbol_value(MRB_SYM(system_time)),
               mrb_float_value(mrb, snapshot.system_time));
  mrb_hash_set(mrb, hash, mrb_symbol_value(MRB_SYM(child_user_time)),
               mrb_float_value(mrb, snapshot.child_user_time));
  mrb_hash_set(mrb, hash, mrb_symbol_value(MRB_SYM(child_system_time)),
               mrb_float_value(mrb, snapshot.child_sys_time));
  mrb_hash_set(mrb, hash, mrb_symbol_value(MRB_SYM(total_time)),
               mrb_float_value(mrb,
                 snapshot.user_time +
                 snapshot.system_time +
                 snapshot.child_user_time +
                 snapshot.child_sys_time));

  return hash;
}

void
mrb_mruby_cpuusage_gem_init(mrb_state *mrb)
{
#ifdef PLATFORM_LINUX
  if (!cpuusage_hz_init) {
    cpuusage_init(mrb);
    cpuusage_hz_init = 1;
  }
#endif
  struct RClass *cpuusage = mrb_define_module_id(mrb, MRB_SYM(CPUUsage));
  mrb_define_module_function_id(mrb, cpuusage, MRB_SYM(snapshot),
                                mrb_cpuusage_snapshot, MRB_ARGS_NONE());
}

void
mrb_mruby_cpuusage_gem_final(mrb_state *mrb)
{
  /* nothing to do */
}
