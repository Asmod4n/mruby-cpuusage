/*
** mrb_cpuusage.c - CPUUsage class
*/

#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/hash.h"
#include "mruby/string.h"
#include "../include/cpuusage.h"

static mrb_value
mrb_cpuusage_snapshot(mrb_state *mrb, mrb_value self)
{
  cpu_snapshot_t snapshot;
  mrb_value hash;
  
  if (cpuusage_snapshot(&snapshot) != 0) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "Failed to get CPU usage snapshot");
  }
  
  hash = mrb_hash_new(mrb);
  mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern_lit(mrb, "user_time")), 
               mrb_float_value(mrb, snapshot.user_time));
  mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern_lit(mrb, "system_time")), 
               mrb_float_value(mrb, snapshot.system_time));
  mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern_lit(mrb, "total_time")), 
               mrb_float_value(mrb, snapshot.user_time + snapshot.system_time));
  
  return hash;
}

void
mrb_mruby_cpuusage_gem_init(mrb_state *mrb)
{
  struct RClass *cpuusage;
  cpuusage = mrb_define_module(mrb, "CPUUsage");
  mrb_define_module_function(mrb, cpuusage, "snapshot", mrb_cpuusage_snapshot, MRB_ARGS_NONE());
}

void
mrb_mruby_cpuusage_gem_final(mrb_state *mrb)
{
  /* nothing to do */
}
