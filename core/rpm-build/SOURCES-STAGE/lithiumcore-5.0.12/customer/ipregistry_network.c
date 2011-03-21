#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/ip.h>
#include <induction/message.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/socket.h>
#include <induction/msgproc.h>
#include <induction/auth.h>
#include <induction/entity.h>
#include <induction/metric.h>
#include <induction/object.h>
#include <induction/timer.h>
#include <induction/ipregistry.h>

#include "ipregistry.h"

l_ipregistry_network* l_ipregistry_network_create (i_resource *self, char *network_str, char *mask_str)
{
  l_ipregistry_network *network;

  network = (l_ipregistry_network *) malloc (sizeof(l_ipregistry_network));
  if (!network)
  { i_printf (1, "l_ipregistry_network_create failed to malloc network"); return NULL; }
  memset (network, 0, sizeof(l_ipregistry_network));
  network->entry_list = i_list_create ();

  if (network_str) 
  {
    network->network_str = strdup (network_str);
    inet_aton (network->network_str, &network->networkaddr);
  }
  if (mask_str) 
  {
    network->mask_str = strdup (mask_str);
    inet_aton (network->mask_str, &network->maskaddr);
  }

  return network;
}

void l_ipregistry_network_free (void *networkptr)
{
  l_ipregistry_network *network = networkptr;

  if (network->network_str) free (network->network_str);
  if (network->mask_str) free (network->mask_str);
  if (network->rescan_timer) i_timer_remove (network->rescan_timer);

  free (network);
}

