#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "interface.h"
#include "data.h"
#include "message.h"
#include "hierarchy.h"
#include "cement.h"
#include "entity.h"
#include "device.h"
#include "container.h"
#include "object.h"
#include "metric.h"

/* Registering a volume */

long i_xsanregistry_register (i_resource *self, int type, i_object *vol)
{
  /* Send a message to the customer resource 
   * to register the given Xsan vol/computer. 
   * 
   * The msg_id is returned.
   *
   * Message data structure is:
   *
   * int volentdesc_datasize
   * char *volentdesc_data                     * Volume object entity descriptor
   * int deventdesc_datasize
   * char *deventdesc_data                     * Device object entity descriptor
   * int type_datasize;
   * char *type_data;
   * 
   */

  int datasize;
  char *volentdesc_data;
  int volentdesc_datasize;
  char *deventdesc_data;
  int deventdesc_datasize;
  char *data;
  char *dataptr;
  long msgid;

  /* Obtain volume entity descriptor */
  if (vol)
  {
    volentdesc_data = i_entity_descriptor_data (NULL, ENTITY(vol), &volentdesc_datasize);
    if (!volentdesc_data || volentdesc_datasize < 1)
    { i_printf (1, "i_xsanregistry_register failed to obtain entity descriptor for volume entity"); return -1; }
  }
  else
  {
    volentdesc_data = NULL;
    volentdesc_datasize = 0;
  }

  /* Obtain device entity descriptor */
  deventdesc_data = i_entity_descriptor_data (NULL, ENTITY(self->hierarchy->dev), &deventdesc_datasize);
  if (!deventdesc_data || deventdesc_datasize < 1)
  { i_printf (1, "i_xsanregistry_register failed to obtain entity descriptor for device entity"); return -1; }

  /* Calculate datasize and malloc */
  datasize = (sizeof(int) * 4) + volentdesc_datasize + deventdesc_datasize;
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_xsanregistry_register failed to malloc data"); free (volentdesc_data); free (deventdesc_data); return -1; }
  dataptr = data;

  /* Add chunk */
  dataptr = i_data_add_chunk (data, dataptr, datasize, volentdesc_data, volentdesc_datasize);
  free (volentdesc_data);
  if (!dataptr)
  { i_printf (1, "i_xsanregistry_register failed to add volentdesc_data to data"); free (data); return -1; }

  /* Add chunk */
  dataptr = i_data_add_chunk (data, dataptr, datasize, deventdesc_data, deventdesc_datasize);
  free (deventdesc_data);
  if (!dataptr)
  { i_printf (1, "i_xsanregistry_register failed to add deventdesc_data to data"); free (data); return -1; }

  /* Add type */
  dataptr = i_data_add_int (data, dataptr, datasize, &type);
  if (!dataptr)
  { i_printf (1, "i_xsanregistry_register failed to add type to data"); free (data); return -1; }

  /* Send message */
  msgid = i_message_send (self, MSG_XSAN_REGISTER, data, datasize, self->hierarchy->cust_addr, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid < 0)
  { i_printf (1, "i_xsanregistry_register failed to send MSG_XSAN_REGISTER message"); return -1; }

  return msgid;
}

long i_xsanregistry_deregister (i_resource *self, i_resource_address *custaddr, i_resource_address *devaddr, i_object *service)
{
  /* Send a request to the customer resource to 
   * de-register the specified service.
   * If custaddr is NULL, the local customer
   * from the self->hierarchy struct will be used.
   * If devaddr is NULL, the local device
   * from the self->hierarchy struct will be used.
   * If no Service is specified, all Services registered 
   * to the relevant device will be deregistered.
   *
   * Data format :
   *
   * int devaddr_datasize;
   * char *devaddr_data;
   * int xsanentdesc_datasize;
   * char *xsanentdesc_data;
   * 
   */

  long msgid;
  int datasize;
  char *data;
  char *dataptr;
  int xsanentdesc_datasize;
  char *xsanentdesc_data;
  char *devaddr_str;

  /* Convert device_addr to address struct */
  if (!devaddr) devaddr = self->hierarchy->dev_addr;
  devaddr_str = i_resource_address_struct_to_string (devaddr);
  if (!devaddr_str)
  { i_printf (1, "i_xsanregistry_deregister failed to convert devaddr to devaddr_str"); return -1; }

  /* Obtain entity descriptor for ip entity */
  if (service)
  {
    xsanentdesc_data = i_entity_descriptor_data (NULL, ENTITY(service), &xsanentdesc_datasize);
    if (!xsanentdesc_data || xsanentdesc_datasize < 1)
    { i_printf (1, "i_xsanregistry_register failed to obtain entity descriptor for service entity"); free (devaddr_str); return -1; }
  }
  else
  { xsanentdesc_data = NULL; xsanentdesc_datasize = 0; }

  /* Create data */ 
  datasize = (2*sizeof(int)) + strlen(devaddr_str)+1 + xsanentdesc_datasize;
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_xsanregistry_deregister failed to malloc data"); free (devaddr_str); free (xsanentdesc_data); return -1; }
  dataptr = data;

  /*
   * Add data variables
   */
  
  dataptr = i_data_add_string (data, dataptr, datasize, devaddr_str);
  free (devaddr_str);
  if (!data)
  { i_printf (1, "i_xsanregistry_deregister failed to add dev_addr_str to data"); free (xsanentdesc_data); free (data); return -1; }

  dataptr = i_data_add_chunk (data, dataptr, datasize, xsanentdesc_data, xsanentdesc_datasize);
  free (xsanentdesc_data);
  if (!dataptr)
  { i_printf (1, "i_xsanregistry_deregister failed to add xsanentdesc_data to data"); free (data); return -1; }

  /* Send message */
  if (!custaddr) custaddr = self->hierarchy->cust_addr;
  msgid = i_message_send (self, MSG_XSAN_DEREGISTER, data, datasize, custaddr, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid < 0)
  { i_printf (1, "i_xsanregistry_deregister failed to send MSG_SVC_DEREGISTER message"); return -1; }

  return msgid;
}
