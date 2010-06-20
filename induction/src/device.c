#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include "induction.h"
#include "cement.h"
#include "entity.h"
#include "device.h"
#include "form.h"
#include "customer.h"
#include "site.h"
#include "data.h"
#include "list.h"
#include "timer.h"

/** \addtogroup device Devices
 * @ingroup entity
 * @{
 */

extern i_resource *global_self;

/* Struct Manipulation */

i_device* i_device_create (char *name_str, char *desc_str, char *ip_str, char *lom_ip_str, char *snmpcomm_str, char *username_str, char *password_str, char *lom_username_str, char *lom_password_str, char *vendor_str, char *profile_str, long refresh_interval)
{
  i_device *dev;

  dev = (i_device *) i_entity_create (name_str, desc_str, ENT_DEVICE, sizeof(i_device));
  if (!dev)
  { i_printf (1, "i_device_create failed to create device entity"); return NULL; }
  dev->snmpversion = 1;
  if (ip_str) dev->ip_str = strdup (ip_str);
  if (lom_ip_str) dev->lom_ip_str = strdup (lom_ip_str);
  if (snmpcomm_str) dev->snmpcomm_str = strdup (snmpcomm_str);
  if (username_str) dev->username_str = strdup (username_str);
  if (password_str) dev->password_str = strdup (password_str);
  if (lom_username_str) dev->lom_username_str = strdup (lom_username_str);
  if (lom_password_str) dev->lom_password_str = strdup (lom_password_str);
  if (vendor_str) dev->vendor_str = strdup (vendor_str);
  else dev->vendor_str = strdup ("snmpbasic");
  if (profile_str) dev->profile_str = strdup (profile_str);
  if (refresh_interval > 0) dev->refresh_interval = refresh_interval;
  else dev->refresh_interval = DEVICE_DEFAULT_REFRESH_INTERVAL;
  dev->refresh_func = i_entity_refresh_children;  /* Default refresh func */
  dev->authorative = 1;

  return dev;
}

void i_device_free (void *devptr)
{
  /* This func should only be called
   * by i_entity_free. 
   * Just free the device-specific 
   * portions of the struct. The struct
   * and everything else will be freed
   * by i_entity_free
   */
  i_device *dev = devptr;

  if (!dev) return;

  if (dev->ip_str) free (dev->ip_str);
  if (dev->lom_ip_str) free (dev->lom_ip_str);
  if (dev->snmpcomm_str) free (dev->snmpcomm_str);
  if (dev->snmpauthpass_str) free (dev->snmpauthpass_str);
  if (dev->snmpprivpass_str) free (dev->snmpprivpass_str);
  if (dev->username_str) free (dev->username_str);
  if (dev->password_str) free (dev->password_str);
  if (dev->lom_username_str) free (dev->lom_username_str);
  if (dev->lom_password_str) free (dev->lom_password_str);
  if (dev->vendor_str) free (dev->vendor_str);
  if (dev->profile_str) free (dev->profile_str);
  if (dev->modb_xml_str) free (dev->modb_xml_str);
}

i_device* i_device_duplicate (i_device *dev)
{
  i_device *dup;

  dup = i_device_create (dev->name_str, dev->desc_str, dev->ip_str, dev->lom_ip_str, dev->snmpcomm_str, dev->username_str, dev->password_str, dev->vendor_str, dev->lom_username_str, dev->lom_password_str, dev->profile_str, dev->refresh_interval);
  if (!dup)
  { i_printf (1, "i_device_duplicate failed to create dup device"); return NULL; }

  /* Duplicate Variables */
  if (dev->fsroot_str) dup->fsroot_str = strdup (dev->fsroot_str); 
  if (dev->resaddr) dup->resaddr = i_resource_address_duplicate (dev->resaddr);
  dup->snmpversion = dev->snmpversion;
  dup->snmpauthmethod = dev->snmpauthmethod;
  dup->snmpprivenc = dev->snmpprivenc;
  if (dev->snmpauthpass_str) dup->snmpauthpass_str = strdup (dev->snmpauthpass_str);
  if (dev->snmpprivpass_str) dup->snmpprivpass_str = strdup (dev->snmpprivpass_str);
  dup->protocol = dev->protocol;
  dup->icmp = dev->icmp;
  dup->lithiumsnmp = dev->lithiumsnmp;
  dup->swrun = dev->swrun;
  dup->nagios = dev->nagios;
  dup->lom = dev->lom;
  dup->xsan = dev->xsan;
  uuid_copy (dup->uuid, dev->uuid);

  return dup;
}

/* Struct/Data Conversion */

char* i_device_data (i_device *dev, int *datasizeptr)
{
  /* Data format: 
   *
   * unsigned short type;
   * int name_len
   * char *name_str
   * int desc_len
   * char *desc_len
   * int ip_len;
   * char *ip_str;
   * int lom_ip_len;
   * char *lom_ip_str;
   * int snmpcomm_len;
   * char *snmpcomm_str;
   *
   * int snmpversion;
   * int snmpauthmethod;
   * int snmpprivenc;
   * int snmpauthpass_len;
   * char *snmpauthpass_str;
   * int snmpprivpass_len;
   * char *snmpprivpass_str;
   * 
   * int username_len;
   * char *username_str;
   * int password_len;
   * char *password_str;
   * int lom_username_len;
   * char *lom_username_str;
   * int lom_password_len;
   * char *lom_password_str;
   * int vendor_len;
   * char *vendor_str;
   * int profile_len;
   * char *profile_str;
   * long refresh_interval;
   * int protocol;
   * int icmp;
   * int lithiumsnmp;
   * int swrun;
   * int nagios;
   * int lom;
   * int xsan;
   * int uuid_len;
   * char *uuid_str;
   * int mark;
   * int limited;
   * int sitedata_size;
   * char *sitedata;
   *
   *
   */
  int datasize;
  int sitedata_size;
  char *data;
  char *dataptr;
  char *sitedata;
  char uuid_str[37];

  /* Parse UUID */
  uuid_unparse_lower (dev->uuid, uuid_str);

  /* Create site data */
  if (dev->site)
  {
    sitedata = i_site_data (dev->site, &sitedata_size);
    if (!sitedata)
    { i_printf (1, "i_device_data failed to convert dev->site to data"); return NULL; }
  }
  else
  { sitedata = NULL; sitedata_size = 0; }

  /* Calc data size */
  datasize = (27 * sizeof(int)) + sizeof(unsigned short) + sizeof(long) + sitedata_size;
  if (dev->name_str) datasize += strlen (dev->name_str)+1;
  if (dev->desc_str) datasize += strlen (dev->desc_str)+1;
  if (dev->ip_str) datasize += strlen (dev->ip_str)+1;
  if (dev->lom_ip_str) datasize += strlen (dev->lom_ip_str)+1;
  if (dev->snmpcomm_str) datasize += strlen (dev->snmpcomm_str)+1;
  if (dev->snmpauthpass_str) datasize += strlen (dev->snmpauthpass_str)+1;
  if (dev->snmpprivpass_str) datasize += strlen (dev->snmpprivpass_str)+1;
  if (dev->username_str) datasize += strlen (dev->username_str)+1;
  if (dev->password_str) datasize += strlen (dev->password_str)+1;
  if (dev->lom_username_str) datasize += strlen (dev->lom_username_str)+1;
  if (dev->lom_password_str) datasize += strlen (dev->lom_password_str)+1;
  if (dev->vendor_str) datasize += strlen (dev->vendor_str)+1;
  if (dev->profile_str) datasize += strlen (dev->profile_str)+1;
  datasize += strlen(uuid_str)+1;

  /* Malloc */
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_device_data failed to malloc data (%i bytes)", datasize); if (sitedata) free (sitedata); return NULL; }
  dataptr = data;
  
  /* Add type */
  dataptr = i_data_add_ushort (data, dataptr, datasize, &dev->ent_type);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add ent_type to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* Name */
  dataptr = i_data_add_string (data, dataptr, datasize, dev->name_str);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add name_str to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* Desc */
  dataptr = i_data_add_string (data, dataptr, datasize, dev->desc_str);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add desc_str to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* IP */
  dataptr = i_data_add_string (data, dataptr, datasize, dev->ip_str);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add ip_str to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* LOM IP */
  dataptr = i_data_add_string (data, dataptr, datasize, dev->lom_ip_str);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add lom_ip_str to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* SNMP Community */
  dataptr = i_data_add_string (data, dataptr, datasize, dev->snmpcomm_str);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add snmpcomm_str to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* SNMP Version */
  dataptr = i_data_add_int (data, dataptr, datasize, &dev->snmpversion);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add snmpversion to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* SNMP Auth Method */
  dataptr = i_data_add_int (data, dataptr, datasize, &dev->snmpauthmethod);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add snmpauthmethod to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* SNMP Privacy Encoding */
  dataptr = i_data_add_int (data, dataptr, datasize, &dev->snmpprivenc);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add snmpprivenc to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* SNMP Auth Password */
  dataptr = i_data_add_string (data, dataptr, datasize, dev->snmpauthpass_str);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add snmpauthpass_str to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* SNMP Privacy Password */
  dataptr = i_data_add_string (data, dataptr, datasize, dev->snmpprivpass_str);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add snmpprivpass_str to data"); free (data); if (sitedata) free (sitedata); return NULL; }
  
  /* Username */
  dataptr = i_data_add_string (data, dataptr, datasize, dev->username_str);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add username_str to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* Password */
  dataptr = i_data_add_string (data, dataptr, datasize, dev->password_str);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add password_str to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* LOM Username */
  dataptr = i_data_add_string (data, dataptr, datasize, dev->lom_username_str);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add lom_username_str to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* LOM Password */
  dataptr = i_data_add_string (data, dataptr, datasize, dev->lom_password_str);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add lom_password_str to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* Vendor */
  dataptr = i_data_add_string (data, dataptr, datasize, dev->vendor_str);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add vendor_str to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* Profile */
  dataptr = i_data_add_string (data, dataptr, datasize, dev->profile_str);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add profile_str to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* Refresh interval */
  dataptr = i_data_add_long (data, dataptr, datasize, &dev->refresh_interval);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add refresh_interval to data"); free (data); return NULL; }

  /* Protocol */
  dataptr = i_data_add_int (data, dataptr, datasize, &dev->protocol);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add protocol to data"); free (data); return NULL; }

  /* ICMP */
  dataptr = i_data_add_int (data, dataptr, datasize, &dev->icmp);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add icmp to data"); free (data); return NULL; }

  /* Lithium SNMP Extensions */
  dataptr = i_data_add_int (data, dataptr, datasize, &dev->lithiumsnmp);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add lithiumsnmp to data"); free (data); return NULL; }

  /* Process List Monitoring */
  dataptr = i_data_add_int (data, dataptr, datasize, &dev->swrun);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add swrun to data"); free (data); return NULL; }

  /* Nagios */
  dataptr = i_data_add_int (data, dataptr, datasize, &dev->nagios);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add nagios to data"); free (data); return NULL; }

  /* LOM */
  dataptr = i_data_add_int (data, dataptr, datasize, &dev->lom);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add lom to data"); free (data); return NULL; }

  /* Xsan */
  dataptr = i_data_add_int (data, dataptr, datasize, &dev->xsan);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add xsan to data"); free (data); return NULL; }

  /* UUID */
  dataptr = i_data_add_string (data, dataptr, datasize, uuid_str);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add uuid_str to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* Mark */
  dataptr = i_data_add_int (data, dataptr, datasize, &dev->mark);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add mark to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* Limited */
  dataptr = i_data_add_int (data, dataptr, datasize, &dev->limited);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add mark to data"); free (data); if (sitedata) free (sitedata); return NULL; }

  /* Site data */
  dataptr = i_data_add_chunk (data, dataptr, datasize, sitedata, sitedata_size);
  if (sitedata) free (sitedata);
  if (!dataptr)
  { i_printf (1, "i_device_data failed to add sitedata chunk"); free (data); return NULL; }

  /* Set datasizeptr */
  memcpy (datasizeptr, &datasize, sizeof(int));

  return data;
}

i_device* i_device_struct (char *data, int datasize)
{
  int offset;
  int sitedata_size;
  unsigned short type;
  char *name_str;
  char *desc_str;
  char *dataptr = data;
  char *sitedata;
  i_device *dev;

  /* Type */
  type = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get type from data"); return NULL; }
  dataptr += offset;

  /* Name */
  name_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct warning, failed to get name_str from data"); }
  dataptr += offset;

  /* Desc */
  desc_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct warning, failed to get desc_str from data"); }
  dataptr += offset;

  /* Create device struct */
  dev = i_device_create (name_str, desc_str, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0);
  free (name_str);
  free (desc_str);
  if (!dev)
  { i_printf (1, "i_device_struct failed to create device struct"); return NULL; }

  /* IP */
  dev->ip_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get ip_str from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* LOM IP */
  dev->lom_ip_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get lom_ip_str from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* SNMP Community */
  dev->snmpcomm_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get snmpcomm_str from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* SNMP Version */
  dev->snmpversion = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get snmpversion from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* SNMP Auth method */
  dev->snmpauthmethod = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get snmpauthmethod from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* SNMP Privacy Encoding */
  dev->snmpprivenc = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get snmpprivenc from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* SNMP Auth Password */
  dev->snmpauthpass_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get snmpauthpass_str from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* SNMP priv Password */
  dev->snmpprivpass_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get snmpprivpass_str from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* Username */
  dev->username_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get username_str from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* Password */
  dev->password_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get password_str from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* LOM Username */
  dev->lom_username_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get lom_username_str from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* Password */
  dev->lom_password_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get lom_password_str from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* Vendor */
  if (dev->vendor_str) { free (dev->vendor_str); dev->vendor_str = NULL; }
  dev->vendor_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get vendor_str from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* Profile */
  dev->profile_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get profile_str from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* Refresh interval */
  dev->refresh_interval = i_data_get_long (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get refresh_interval from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* Protocol */
  dev->protocol = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get protocol from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* ICMP */
  dev->icmp = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get icmp from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* Lithium SNMP */
  dev->lithiumsnmp = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get lithiumsnmp from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* Process List */
  dev->swrun = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get swrun from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* Nagios */
  dev->nagios = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get nagios from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* LOM */
  dev->lom = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get lom from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* Xsan */
  dev->xsan = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get xsan from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* UUID */
  char *uuid_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get uuid_str from data"); i_device_free (dev); return NULL; }
  dataptr += offset;
  uuid_parse (uuid_str, dev->uuid);
  if (uuid_str) free (uuid_str);

  /* Mark */
  dev->mark = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get mark from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* Limited */
  dev->limited = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get mark from data"); i_device_free (dev); return NULL; }
  dataptr += offset;

  /* Site */
  sitedata = i_data_get_chunk (data, dataptr, datasize, &sitedata_size, &offset);
  if (offset < 1)
  { i_printf (1, "i_device_struct failed to get sitedata from data"); i_device_free (dev); return NULL; }
  if (sitedata)
  {
    i_site *site;
    site = i_site_struct (sitedata, sitedata_size);
    free (sitedata);
    if (site)
    {
      /* Register device to site */
      i_entity_register (global_self, ENTITY(site), ENTITY(dev));
    }
  }

  return dev;
}

/* Device Specific Registration / Deregistration 
 * (Should only be called by i_entity_register and i_entity_deregister)
 */

int i_device_register (i_resource *self, i_site *site, i_device *dev)
{
  return 0;
}

int i_device_deregister (i_resource *self, i_device *dev)
{
  return 0;
}

