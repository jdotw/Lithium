#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#ifdef OS_DARWIN
#include <mach/task.h>
#include <mach/mach_init.h>
#endif

#include "induction.h"
#include "timer.h"
#include "memcheck.h"

#ifdef OS_DARWIN
void getres(task_t task, unsigned int *rss, unsigned int *vs)
{
    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

    task_info(task, TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count);
    *rss = t_info.resident_size;
    *vs = t_info.virtual_size;
}
#endif

float i_memcheck_rss ()
{
#ifdef OS_DARWIN
  unsigned int rss, vs, psize;
  task_t task = MACH_PORT_NULL;

  if (task_for_pid(current_task(), getpid(), &task) != KERN_SUCCESS)
      abort();
  getres(task, &rss, &vs);
  psize = getpagesize();

  return (float) rss / 1024.0;
#else
  return 0.;
#endif
}

float i_memcheck_vss ()
{
#ifdef OS_DARWIN
    unsigned int rss, vs, psize;
    task_t task = MACH_PORT_NULL;

    if (task_for_pid(current_task(), getpid(), &task) != KERN_SUCCESS)
        abort();
    getres(task, &rss, &vs);
    psize = getpagesize();

    return (float) vs / 1024.0;
#else
    return 0.;
#endif
}

int i_memcheck_perflog_timer (i_resource *self, i_timer *timer, void *passdata)
{
#ifdef OS_DARWIN
  if (self->perflog)
  {
    i_printf (0, "PERF: RSS Memory Usage is %.2fMByte", i_memcheck_vss() / (1024 * 1024));
  }
#endif
  return 0;
}
