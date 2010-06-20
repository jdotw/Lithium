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

/* Registering a LUN */

long i_lunregistry_register (i_resource *self, int type, i_object *obj, char *wwn_str, int lun)
{
  /* Send a message to the customer resource 
   * to register the given IP address. 
   * 
   * The msg_id is returned.
   *
   * Message data structure is:
   *
   * int type;
   * int objentdesc_datasize
   * char *objentdesc_data                     * LUN object entity descriptor
   * int wwn_datasize;
   * char *wwn_str;
   * int lun_datasize;
   * char *lun;
   */

  int datasize;
  char *lunentdesc_data;
  int lunentdesc_datasize;
  char *data;
  char *dataptr;
  long msgid;

  /* Obtain ip entity descriptor */
  lunentdesc_data = i_entity_descriptor_data (NULL, ENTITY(obj), &lunentdesc_datasize);
  if (!lunentdesc_data || lunentdesc_datasize < 1)
  { i_printf (1, "i_lunregistry_register failed to obtain entity descriptor for IP entity"); return -1; }

  /* Calculate datasize and malloc */
  datasize = (5 * sizeof(int)) + lunentdesc_datasize + (strlen(wwn_str) + 1);
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_lunregistry_register failed to malloc data"); free (lunentdesc_data); return -1; }
  dataptr = data;

  /* Add lun */
  dataptr = i_data_add_int (data, dataptr, datasize, &type);
  if (!dataptr)
  { i_printf (1, "i_lunregistry_register failed to add type to data"); free (data); return -1; }

  /* Add chunk */
  dataptr = i_data_add_chunk (data, dataptr, datasize, lunentdesc_data, lunentdesc_datasize);
  free (lunentdesc_data);
  if (!dataptr)
  { i_printf (1, "i_lunregistry_register failed to add lunentdesc_data to data"); free (data); return -1; }

  /* Add wwn */
  dataptr = i_data_add_string (data, dataptr, datasize, wwn_str);
  if (!dataptr)
  { i_printf (1, "i_lunregistry_register failed to add wwn to data"); free (data); return -1; }

  /* Add lun */
  dataptr = i_data_add_int (data, dataptr, datasize, &lun);
  if (!dataptr)
  { i_printf (1, "i_lunregistry_register failed to add lun to data"); free (data); return -1; }

  /* Send message */
  msgid = i_message_send (self, MSG_LUN_REGISTER, data, datasize, self->hierarchy->cust_addr, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid < 0)
  { i_printf (1, "i_lunregistry_register failed to send MSG_LUN_REGISTER message"); return -1; }

  return msgid;
}

long i_lunregistry_deregister (i_resource *self, i_resource_address *custaddr, i_resource_address *devaddr, i_object *obj)
{
  /* Send a request to the customer resource to 
   * de-register the specified LUN.
   * If custaddr is NULL, the local customer
   * from the self->hierarchy struct will be used.
   * If devaddr is NULL, the local device
   * from the self->hierarchy struct will be used.
   * If no LUN is specified, all LUN registered 
   * to the relevant device will be deregistered.
   *
   * Data format :
   *
   * int devaddr_datasize;
   * char *devaddr_data;
   * int lunentdesc_datasize;
   * char *lunentdesc_data;
   * 
   */

  long msgid;
  int datasize;
  char *data;
  char *dataptr;
  int lunentdesc_datasize;
  char *lunentdesc_data;
  char *devaddr_str;

  /* Convert device_addr to address struct */
  if (!devaddr) devaddr = self->hierarchy->dev_addr;
  devaddr_str = i_resource_address_struct_to_string (devaddr);
  if (!devaddr_str)
  { i_printf (1, "i_lunregistry_deregister failed to convert devaddr to devaddr_str"); return -1; }

  /* Obtain entity descriptor for ip entity */
  if (obj)
  {
    lunentdesc_data = i_entity_descriptor_data (NULL, ENTITY(obj), &lunentdesc_datasize);
    if (!lunentdesc_data || lunentdesc_datasize < 1)
    { i_printf (1, "i_lunregistry_register failed to obtain entity descriptor for obj entity"); free (devaddr_str); return -1; }
  }
  else
  { lunentdesc_data = NULL; lunentdesc_datasize = 0; }

  /* Create data */ 
  datasize = (2*sizeof(int)) + strlen(devaddr_str)+1 + lunentdesc_datasize;
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_lunregistry_deregister failed to malloc data"); free (devaddr_str); free (lunentdesc_data); return -1; }
  dataptr = data;

  /*
   * Add data variables
   */
  
  dataptr = i_data_add_string (data, dataptr, datasize, devaddr_str);
  free (devaddr_str);
  if (!data)
  { i_printf (1, "i_lunregistry_deregister failed to add dev_addr_str to data"); free (lunentdesc_data); free (data); return -1; }

  dataptr = i_data_add_chunk (data, dataptr, datasize, lunentdesc_data, lunentdesc_datasize);
  free (lunentdesc_data);
  if (!dataptr)
  { i_printf (1, "i_lunregistry_deregister failed to add lunentdesc_data to data"); free (data); return -1; }

  /* Send message */
  if (!custaddr) custaddr = self->hierarchy->cust_addr;
  msgid = i_message_send (self, MSG_LUN_DEREGISTER, data, datasize, custaddr, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid < 0)
  { i_printf (1, "i_lunregistry_deregister failed to send MSG_LUN_DEREGISTER message"); return -1; }

  return msgid;
}
