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
#include "ip.h"

/* Registering an IP Address */

long i_ipregistry_register (i_resource *self, i_object *ip)
{
  /* Send a message to the customer resource 
   * to register the given IP address. 
   * 
   * The msg_id is returned.
   *
   * Message data structure is:
   *
   * int ipentdesc_datasize
   * char *ipentdesc_data                     * IP object entity descriptor
   * 
   */

  int datasize;
  char *ipentdesc_data;
  int ipentdesc_datasize;
  char *data;
  char *dataptr;
  long msgid;

  /* Check for 127.0.0.1 */
  if (ip->desc_str && strcmp(ip->desc_str, "127.0.0.1") == 0)
  { return -1; }

  /* Obtain ip entity descriptor */
  ipentdesc_data = i_entity_descriptor_data (NULL, ENTITY(ip), &ipentdesc_datasize);
  if (!ipentdesc_data || ipentdesc_datasize < 1)
  { i_printf (1, "i_ipregistry_register failed to obtain entity descriptor for IP entity"); return -1; }

  /* Calculate datasize and malloc */
  datasize = sizeof(int) + ipentdesc_datasize;
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_ipregistry_register failed to malloc data"); free (ipentdesc_data); return -1; }
  dataptr = data;

  /* Add chunk */
  dataptr = i_data_add_chunk (data, dataptr, datasize, ipentdesc_data, ipentdesc_datasize);
  free (ipentdesc_data);
  if (!dataptr)
  { i_printf (1, "i_ipregistry_register failed to add ipentdesc_data to data"); free (data); return -1; }

  /* Send message */
  msgid = i_message_send (self, MSG_IP_REGISTER, data, datasize, self->hierarchy->cust_addr, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid < 0)
  { i_printf (1, "i_ipregistry_register failed to send MSG_IP_REGISTER message"); return -1; }

  return msgid;
}

long i_ipregistry_setnetmask (i_resource *self, i_object *ip, i_metric *netmask)
{
  /* Send a message to the customer resource 
   * to set the netmask for the specified IP.
   * If the func is called with no current value for netmask
   * then a NULL string is sent in the message causing 
   * the customer resource to remove the mask_str.
   * 
   * The msg_id is returned.
   *
   * Message data structure is:
   *
   * int ipentdesc_datasize
   * char *ipentdesc_data                     * IP object entity descriptor
   * int mask_str_datasize                    
   * char *mask_str;                          * Netmask in dot decimal string form
   * 
   */

  int datasize;
  char *ipentdesc_data;
  int ipentdesc_datasize;
  char *data;
  char *dataptr;
  char *mask_str;
  long msgid;

  /* Check for 127.0.0.1 */
  if (ip->desc_str && strcmp(ip->desc_str, "127.0.0.1") == 0)
  { return -1; }

  /* Obtain ip entity descriptor */
  ipentdesc_data = i_entity_descriptor_data (NULL, ENTITY(ip), &ipentdesc_datasize);
  if (!ipentdesc_data || ipentdesc_datasize < 1)
  { i_printf (1, "i_ipregistry_setnetmask failed to obtain entity descriptor for IP entity"); return -1; }

  /* Retrieve current value for netmask */
  mask_str = i_metric_valstr (netmask, NULL);
  
  /* Calculate datasize */
  datasize = (2*sizeof(int)) + ipentdesc_datasize;
  if (mask_str) datasize += strlen(mask_str) + 1;

  /* Malloc data */
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_ipregistry_setnetmask failed to malloc data"); free (ipentdesc_data); if (mask_str) free (mask_str); return -1; }
  dataptr = data;

  /* Add entity descriptor chunk */
  dataptr = i_data_add_chunk (data, dataptr, datasize, ipentdesc_data, ipentdesc_datasize);
  free (ipentdesc_data);
  if (!dataptr)
  { i_printf (1, "i_ipregistry_setnetmask failed to add ipentdesc_data to data"); free (data); if (mask_str) free (mask_str); return -1; }

  /* Add mask_str */
  dataptr = i_data_add_string (data, dataptr, datasize, mask_str);
  if (mask_str) free (mask_str);
  if (!dataptr)
  { i_printf (1, "i_ipregistry_setnetmask failed to add mask_str to data"); free (data); return -1; }

  /* Send message */
  msgid = i_message_send (self, MSG_IP_SETNETMASK, data, datasize, self->hierarchy->cust_addr, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid < 0)
  { i_printf (1, "i_ipregistry_setnetmask failed to send MSG_IP_SETNETMASK message"); return -1; }

  return msgid;
}

long i_ipregistry_setiface (i_resource *self, i_object *ip, i_object *iface)
{
  /* Send a message to the customer resource 
   * to set the netmask for the specified IP.
   * If the iface is NULL, a NULL (size 0) data 
   * block is sent for the ifentdesc causing the
   * customer resource to remove the iface reference.
   * 
   * The msg_id is returned.
   *
   * Message data structure is:
   *
   * int ipentdesc_datasize
   * char *ipentdesc_data                     * IP object entity descriptor
   * int ifentdesc_datasize                    
   * char *ifentdesc_data;                    * Interface object entity desriptor
   * 
   */

  int datasize;
  char *ipentdesc_data;
  int ipentdesc_datasize;
  char *ifentdesc_data;
  int ifentdesc_datasize;
  char *data;
  char *dataptr;
  long msgid;

  /* Check for 127.0.0.1 */
  if (ip->desc_str && strcmp(ip->desc_str, "127.0.0.1") == 0)
  { return -1; }

  /* Obtain ip entity descriptor */
  ipentdesc_data = i_entity_descriptor_data (NULL, ENTITY(ip), &ipentdesc_datasize);
  if (!ipentdesc_data || ipentdesc_datasize < 1)
  { i_printf (1, "i_ipregistry_setiface failed to obtain entity descriptor for IP entity"); return -1; }

  /* Obtain ip entity descriptor */
  if (iface)
  {
    ifentdesc_data = i_entity_descriptor_data (NULL, ENTITY(iface), &ifentdesc_datasize);
    if (!ifentdesc_data || ifentdesc_datasize < 1)
    { i_printf (1, "i_ipregistry_setiface failed to obtain entity descriptor for the interface entity"); return -1; }
  }
  else
  { ifentdesc_data = NULL; ifentdesc_datasize = 0; }

  /* Calculate datasize and malloc */
  datasize = (2*sizeof(int)) + ipentdesc_datasize + ifentdesc_datasize;
  data = (char *) malloc (datasize);
  if (!data)
  { 
    i_printf (1, "i_ipregistry_setiface failed to malloc data"); 
    free (ipentdesc_data); 
    if (ifentdesc_data) free (ifentdesc_data);
    return -1; 
  }
  dataptr = data;

  /* Add IP entity descriptor chunk */
  dataptr = i_data_add_chunk (data, dataptr, datasize, ipentdesc_data, ipentdesc_datasize);
  free (ipentdesc_data);
  if (!dataptr)
  { 
    i_printf (1, "i_ipregistry_setiface failed to add ipentdesc_data to data"); 
    if (ifentdesc_data) free (ifentdesc_data);
    free (data); 
    return -1; 
  }
  
  /* Add interface entity descriptor chunk */
  dataptr = i_data_add_chunk (data, dataptr, datasize, ifentdesc_data, ifentdesc_datasize);
  if (ifentdesc_data) free (ifentdesc_data);
  if (!dataptr)
  { 
    i_printf (1, "i_ipregistry_setiface failed to add ifentdesc_data to data"); 
    free (data); 
    return -1; 
  }

  /* Send message */
  msgid = i_message_send (self, MSG_IP_SETIFACE, data, datasize, self->hierarchy->cust_addr, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid < 0)
  { i_printf (1, "i_ipregistry_setiface failed to send MSG_IP_SETIFACE message"); return -1; }

  return msgid;
}

long i_ipregistry_deregister (i_resource *self, i_resource_address *custaddr, i_resource_address *devaddr, i_object *ip)
{
  /* Send a request to the customer resource to 
   * de-register the specified IP.
   * If custaddr is NULL, the local customer
   * from the self->hierarchy struct will be used.
   * If devaddr is NULL, the local device
   * from the self->hierarchy struct will be used.
   * If no IP is specified, all IPs registered 
   * to the relevant device will be deregistered.
   *
   * Data format :
   *
   * int devaddr_datasize;
   * char *devaddr_data;
   * int ipentdesc_datasize;
   * char *ipentdesc_data;
   * 
   */

  long msgid;
  int datasize;
  char *data;
  char *dataptr;
  int ipentdesc_datasize;
  char *ipentdesc_data;
  char *devaddr_str;

  /* Convert device_addr to address struct */
  if (!devaddr) devaddr = self->hierarchy->dev_addr;
  devaddr_str = i_resource_address_struct_to_string (devaddr);
  if (!devaddr_str)
  { i_printf (1, "i_ipregistry_deregister failed to convert devaddr to devaddr_str"); return -1; }

  /* Obtain entity descriptor for ip entity */
  if (ip)
  {
    ipentdesc_data = i_entity_descriptor_data (NULL, ENTITY(ip), &ipentdesc_datasize);
    if (!ipentdesc_data || ipentdesc_datasize < 1)
    { i_printf (1, "i_ipregistry_register failed to obtain entity descriptor for IP entity"); free (devaddr_str); return -1; }
  }
  else
  { ipentdesc_data = NULL; ipentdesc_datasize = 0; }

  /* Create data */ 
  datasize = (2*sizeof(int)) + strlen(devaddr_str)+1 + ipentdesc_datasize;
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_ipregistry_deregister failed to malloc data"); free (devaddr_str); free (ipentdesc_data); return -1; }
  dataptr = data;

  /*
   * Add data variables
   */
  
  dataptr = i_data_add_string (data, dataptr, datasize, devaddr_str);
  free (devaddr_str);
  if (!data)
  { i_printf (1, "i_ipregistry_deregister failed to add dev_addr_str to data"); free (ipentdesc_data); free (data); return -1; }

  dataptr = i_data_add_chunk (data, dataptr, datasize, ipentdesc_data, ipentdesc_datasize);
  free (ipentdesc_data);
  if (!dataptr)
  { i_printf (1, "i_ipregistry_deregister failed to add ipentdesc_data to data"); free (data); return -1; }

  /* Send message */
  if (!custaddr) custaddr = self->hierarchy->cust_addr;
  msgid = i_message_send (self, MSG_IP_DEREGISTER, data, datasize, custaddr, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid < 0)
  { i_printf (1, "i_ipregistry_deregister failed to send MSG_IP_DEREGISTER message"); return -1; }

  return msgid;
}
