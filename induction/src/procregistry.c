#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "interface.h"
#include "data.h"
#include "message.h"
#include "hierarchy.h"
#include "cement.h"
#include "entity.h"
#include "object.h"
#include "metric.h"

#include "procregistry.h"

/* Registering a process */

long i_procregistry_register (i_resource *self, i_object *process)
{
  /* Send a message to the customer resource 
   * to register the given IP address. 
   * 
   * The msg_id is returned.
   *
   * Message data structure is:
   *
   * int objentdesc_datasize
   * char *objentdesc_data                     * Service object entity descriptor
   * 
   */

  int datasize;
  char *procentdesc_data;
  int procentdesc_datasize;
  char *data;
  char *dataptr;
  long msgid;

  /* Obtain ip entity descriptor */
  procentdesc_data = i_entity_descriptor_data (NULL, ENTITY(process), &procentdesc_datasize);
  if (!procentdesc_data || procentdesc_datasize < 1)
  { i_printf (1, "i_procregistry_register failed to obtain entity descriptor for IP entity"); return -1; }

  /* Calculate datasize and malloc */
  datasize = sizeof(int) + procentdesc_datasize;
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_procregistry_register failed to malloc data"); free (procentdesc_data); return -1; }
  dataptr = data;

  /* Add chunk */
  dataptr = i_data_add_chunk (data, dataptr, datasize, procentdesc_data, procentdesc_datasize);
  free (procentdesc_data);
  if (!dataptr)
  { i_printf (1, "i_procregistry_register failed to add procentdesc_data to data"); free (data); return -1; }

  /* Send message */
  msgid = i_message_send (self, MSG_PROC_REGISTER, data, datasize, self->hierarchy->cust_addr, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid < 0)
  { i_printf (1, "i_procregistry_register failed to send MSG_PROC_REGISTER message"); return -1; }

  return msgid;
}

long i_procregistry_deregister (i_resource *self, i_resource_address *custaddr, i_resource_address *devaddr, i_object *process)
{
  /* Send a request to the customer resource to 
   * de-register the specified process.
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
   * int procentdesc_datasize;
   * char *procentdesc_data;
   * 
   */

  long msgid;
  int datasize;
  char *data;
  char *dataptr;
  int procentdesc_datasize;
  char *procentdesc_data;
  char *devaddr_str;

  /* Convert device_addr to address struct */
  if (!devaddr) devaddr = self->hierarchy->dev_addr;
  devaddr_str = i_resource_address_struct_to_string (devaddr);
  if (!devaddr_str)
  { i_printf (1, "i_procregistry_deregister failed to convert devaddr to devaddr_str"); return -1; }

  /* Obtain entity descriptor for ip entity */
  if (process)
  {
    procentdesc_data = i_entity_descriptor_data (NULL, ENTITY(process), &procentdesc_datasize);
    if (!procentdesc_data || procentdesc_datasize < 1)
    { i_printf (1, "i_procregistry_register failed to obtain entity descriptor for process entity"); free (devaddr_str); return -1; }
  }
  else
  { procentdesc_data = NULL; procentdesc_datasize = 0; }

  /* Create data */ 
  datasize = (2*sizeof(int)) + strlen(devaddr_str)+1 + procentdesc_datasize;
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_procregistry_deregister failed to malloc data"); free (devaddr_str); free (procentdesc_data); return -1; }
  dataptr = data;

  /*
   * Add data variables
   */
  
  dataptr = i_data_add_string (data, dataptr, datasize, devaddr_str);
  free (devaddr_str);
  if (!data)
  { i_printf (1, "i_procregistry_deregister failed to add dev_addr_str to data"); free (procentdesc_data); free (data); return -1; }

  dataptr = i_data_add_chunk (data, dataptr, datasize, procentdesc_data, procentdesc_datasize);
  free (procentdesc_data);
  if (!dataptr)
  { i_printf (1, "i_procregistry_deregister failed to add procentdesc_data to data"); free (data); return -1; }

  /* Send message */
  if (!custaddr) custaddr = self->hierarchy->cust_addr;
  msgid = i_message_send (self, MSG_PROC_DEREGISTER, data, datasize, custaddr, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid < 0)
  { i_printf (1, "i_procregistry_deregister failed to send MSG_PROC_DEREGISTER message"); return -1; }

  return msgid;
}
