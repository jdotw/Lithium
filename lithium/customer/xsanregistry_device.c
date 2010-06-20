#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
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

#include "xsanregistry.h"

l_xsanregistry_device* l_xsanregistry_device_create (i_resource_address *addr, i_entity_descriptor *ent, int type)
{
  l_xsanregistry_device *device;

  device = (l_xsanregistry_device *) malloc (sizeof(l_xsanregistry_device));
  if (!device)
  { i_printf (1, "l_xsanregistry_device_create failed to malloc device"); return NULL; }
  memset (device, 0, sizeof(l_xsanregistry_device));
  device->entry_list = i_list_create ();
  if (addr)
  { device->devaddr = i_resource_address_duplicate (addr); }
  if (ent)
  { device->devent = i_entity_descriptor_duplicate (ent); }
  device->type = type;

  return device;
}

void l_xsanregistry_device_free (void *deviceptr)
{
  l_xsanregistry_device *device = deviceptr;

  if (device->entry_list) i_list_free (device->entry_list);
  if (device->devent) i_entity_descriptor_free (device->devent);
  if (device->devaddr) i_resource_address_free (device->devaddr);

  free (device);
}
