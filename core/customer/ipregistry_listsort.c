#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/ip.h>
#include <induction/message.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/socket.h>
#include <induction/msgproc.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/ipregistry.h>

#include "ipregistry.h"

int l_ipregistry_listsort_ip_func (void *curptr, void *nextptr)
{
  unsigned long int cur_addr_h;
  unsigned long int next_addr_h;
  l_ipregistry_entry *cur = curptr;
  l_ipregistry_entry *next = nextptr;

  cur_addr_h = ntohl (cur->ipaddr.s_addr);
  next_addr_h = ntohl (next->ipaddr.s_addr);

  if (cur_addr_h > next_addr_h) return 1;

  return 0;
}

