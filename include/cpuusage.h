#ifndef CPUUSAGE_H
#define CPUUSAGE_H

typedef struct {
  double user_time;    /* User CPU time in seconds */
  double system_time;  /* System CPU time in seconds */
} cpu_snapshot_t;

/* Platform-specific implementations */
int cpuusage_snapshot(cpu_snapshot_t *snapshot);

#endif /* CPUUSAGE_H */
