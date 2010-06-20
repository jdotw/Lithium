#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/task.h>
#include <mach/mach_init.h>

#include "induction.h"
#include "timer.h"
#include "memcheck.h"

void getres(task_t task, unsigned int *rss, unsigned int *vs)
{
    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

    task_info(task, TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count);
    *rss = t_info.resident_size;
    *vs = t_info.virtual_size;
}

float i_memcheck_rss ()
{
  unsigned int rss, vs, psize;
  task_t task = MACH_PORT_NULL;

  if (task_for_pid(current_task(), getpid(), &task) != KERN_SUCCESS)
      abort();
  getres(task, &rss, &vs);
  psize = getpagesize();

  return (float) rss / 1024.0;
}

float i_memcheck_vss ()
{
    unsigned int rss, vs, psize;
    task_t task = MACH_PORT_NULL;

    if (task_for_pid(current_task(), getpid(), &task) != KERN_SUCCESS)
        abort();
    getres(task, &rss, &vs);
    psize = getpagesize();

    return (float) vs / 1024.0;
}

int i_memcheck_perflog_timer (i_resource *self, i_timer *timer, void *passdata)
{
  if (self->perflog)
  {
    i_printf (0, "PERF: RSS Memory Usage is %.2fMByte", i_memcheck_vss() / (1024 * 1024));
  }
  return 0;
}
