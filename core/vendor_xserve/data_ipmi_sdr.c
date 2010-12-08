#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/socket.h>
#include <induction/entity.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>
#include "device/snmp.h"

#include "fans.h"
#include "cpu.h"
#include "pci.h"
#include "psu.h"
#include "ram.h"
#include "mainboard.h"
#include "ipmitool.h"
#include "csv.h"
#include "data.h"

/*
 * IPMI 'sdr'
 */

static v_ipmitool_cmd *static_cmd = NULL;

/* Data Source Refrsesh */

int v_data_ipmi_sdr_refresh (i_resource *self, i_metric *met, int opcode)
{
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh processa */
      static_cmd = v_ipmitool_exec (self, "sdr", NULL, 0, v_data_ipmi_sdr_ipmicb, met);
      break;

    case REFOP_COLLISION:   /* Handle collision */
      break;

    case REFOP_TERMINATE:   /* Terminate the refresh */
      if (static_cmd)
      {
        /* FIX must cancel request */
        static_cmd = NULL;
      }
      break;

    case REFOP_CLEANDATA:   /* Cleanup persistent refresh data */
      break;
  }

  return 0;
}

int v_data_ipmi_sdr_ipmicb (i_resource *self, v_ipmitool_cmd *cmd, int result, void *passdata)
{
  i_metric *datamet = passdata;

  if (result != IPMIRESULT_OK || !cmd || !cmd->output_str || strlen(cmd->output_str) < 1)
  {
    datamet->refresh_result = REFRESULT_TOTAL_FAIL;
    i_entity_refresh_terminate (ENTITY(datamet));
    return -1;
  }

  /* Interpret data */
  i_list *row;
  i_list *rows = v_csv_process_data (cmd->output_str);
  for (i_list_move_head(rows); (row=i_list_restore(rows))!=NULL; i_list_move_next(rows))
  {
    /* Process row */
    i_list_move_head(row);
    char *sensor = i_list_restore(row);
    if (!sensor) continue;

    /* Check type */
    if (strstr(sensor, "CPU"))
    { v_data_ipmi_sdr_process_cpu (self, row); }
    else if (strstr(sensor, "FBDIMM"))
    { v_data_ipmi_sdr_process_fbdimm (self, row); }
    else if (strstr(sensor, "DIMM"))
    { v_data_ipmi_sdr_process_dimm (self, row); }
    else if (strstr(sensor, "PCI"))
    { v_data_ipmi_sdr_process_pci (self, row); }
    else if (strstr(sensor, "PSU"))
    { v_data_ipmi_sdr_process_psu (self, row); }
    else if (strstr(sensor, "Fan"))
    { v_data_ipmi_sdr_process_fan (self, row); }
    else 
    { v_data_ipmi_sdr_process_mainboard (self, row); }
  }
  i_list_free (rows);
  
  /* Set result and terminate */
  datamet->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(datamet));
  
  return 0;
}

int v_data_ipmi_sdr_process_cpu (i_resource *self, i_list *row)
{
  char *values[2];

  /* Get values */
  i_list_move_head (row);
  values[0] = i_list_restore (row);
  if (!values[0]) return -1;
  i_list_move_next (row);
  values[1] = i_list_restore (row);
  if (!values[1]) return -1;

  /* Get metric */
  i_metric *met = NULL;
  if (strstr(values[0], "Fan"))
  {
    /* Get/Create Fan */
    char *fan_desc = (char *) malloc (10);
    memset (fan_desc, 0, 10);
    strncpy (fan_desc, values[0], 9);
    v_fans_item *fan = v_fans_get (fan_desc);
    if (!fan)
    {
      fan = v_fans_create (self, fan_desc);
      if (!fan) return -1; 
    }
    free (fan_desc);

    /* Get metric */
    if (strstr(values[0], "Inlet")) met = fan->in_rpm;
    else if (strstr(values[0], "Outlet")) met = fan->out_rpm;
  }
  else if (strlen(values[0]) > 6)
  {
    /* Get CPU */
    v_cpu_item *cpu = v_cpu_x ((int) values[0][4] - 'A');
    if (!cpu)
    {
      cpu = v_cpu_create (self);
      if (!cpu) return -1;
    }

    /* Get metric */
    if (strstr(values[0], "Core")) met = cpu->power_vcore;
    else if (strstr(values[0], "VRM Input")) met = cpu->v_12v;
    else if (strstr(values[0], "Power")) met = cpu->power_watts;
    else if (strstr(values[0], " HS")) met = cpu->temp_heatsink;
  }

  /* Set value */
  if (met)
  {
    i_metric_value *val;
    
    val = i_metric_value_create ();
    val->flt = atof (values[1]); 
    i_metric_value_enqueue (self, met, val);
    met->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(met));
  }

  return 0;
}

int v_data_ipmi_sdr_process_fbdimm (i_resource *self, i_list *row)
{
  char *values[2];

  /* Get values */
  i_list_move_head (row);
  values[0] = i_list_restore (row);
  if (!values[0]) return -1;
  i_list_move_next (row);
  values[1] = i_list_restore (row);
  if (!values[1]) return -1;

  /* Get metric */
  i_metric *met = NULL;
  if (strstr(values[0], "Fan"))
  {
    /* Get/Create Fan */
    char *fan_desc = (char *) malloc (13);
    memset (fan_desc, 0, 13);
    strncpy (fan_desc, values[0], 12);
    v_fans_item *fan = v_fans_get (fan_desc);
    if (!fan)
    {
      fan = v_fans_create (self, fan_desc);
      if (!fan) return -1; 
    }
    free (fan_desc);

    /* Get metric */
    if (strstr(values[0], "In")) met = fan->in_rpm;
    else if (strstr(values[0], "Out")) met = fan->out_rpm;
  }
  else if (strstr(values[0], "Slot"))
  {
    /* FIX -- Add handling or FBDIMM temps (non-nehalem) */
  }
  else
  {
    /* Get mainboard item */
    v_mainboard_item *mb = v_mainboard_static_item ();

    /* Get Metric */
    if (strstr(values[0], "VRM")) met = mb->fbdimm_vrm_12v;
  }

  /* Set value */
  if (met)
  {
    i_metric_value *val;
    
    val = i_metric_value_create ();
    val->flt = atof (values[1]); 
    i_metric_value_enqueue (self, met, val);
    met->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(met));
  }

  return 0;
}

int v_data_ipmi_sdr_process_dimm (i_resource *self, i_list *row)
{
  /* Nehalem Xserve DIMM handling */
  char *values[2];

  /* Get values */
  i_list_move_head (row);
  values[0] = i_list_restore (row);
  if (!values[0]) return -1;
  i_list_move_next (row);
  values[1] = i_list_restore (row);
  if (!values[1]) return -1;

  /* Get metric */
  i_metric *met = NULL;
  if (strstr(values[0], "Slot"))
  {
    char location_str[3];
    memset (location_str, 0, 3);
    strncpy (location_str, values[0]+10, 2);
    char *ram_slot;
    if (location_str[0] == 'A')
    { asprintf (&ram_slot, "CPUA/%s", location_str); }
    else if (location_str[0] == 'B')
    { asprintf (&ram_slot, "CPUB/%s", location_str); }
    else
    { ram_slot = NULL; }
    v_ram_item *ram = v_ram_get (ram_slot);
    if (!ram)
    {
      ram = v_ram_create (self, ram_slot);
    }
    if (ram)
    { met = ram->temp; }
  }
  else
  {
    /* Get mainboard item */
    v_mainboard_item *mb = v_mainboard_static_item ();

    /* Get Metric */
    if (strstr(values[0], "VRM Input")) met = mb->fbdimm_vrm_12v;
  }

  /* Set value */
  if (met)
  {
    i_metric_value *val;
    
    val = i_metric_value_create ();
    val->flt = atof (values[1]); 
    i_metric_value_enqueue (self, met, val);
    met->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(met));
  }

  return 0;
}

int v_data_ipmi_sdr_process_fan (i_resource *self, i_list *row)
{
  /* Nehalem-style Fan Info 
   * 
   * Fan 1 In,5888,RPM,ok
   * Fan 1 Out,5760,RPM,ok
   * Fan 2 In,5888,RPM,ok
   * Fan 2 Out,5632,RPM,ok
   * Fan 3 In,5888,RPM,ok
   * Fan 3 Out,5760,RPM,ok
   * Fan 4 In,5888,RPM,ok
   * ...
   */

  char *values[2];

  /* Get values */
  i_list_move_head (row);
  values[0] = i_list_restore (row);
  if (!values[0]) return -1;
  i_list_move_next (row);
  values[1] = i_list_restore (row);
  if (!values[1]) return -1;

  /* Get metric */
  i_metric *met = NULL;
  if (strstr(values[0], "Fan"))
  {
    /* Truncate fan description (leaves out In/Out) */
    char fan_desc[6];
    memset (fan_desc, 0, 6);
    strncpy (fan_desc, values[0], 5);     

    /* Get/Create Fan */
    v_fans_item *fan = v_fans_get (fan_desc);
    if (!fan)
    {
      fan = v_fans_create (self, fan_desc);      
      if (!fan) return -1;
    }

    /* Get metric */
    if (strstr(values[0], "In")) met = fan->in_rpm;
    else if (strstr(values[0], "Out")) met = fan->out_rpm;
  }

  /* Set value */
  if (met)
  {
    i_metric_value *val;

    val = i_metric_value_create ();
    val->flt = atof (values[1]);
    i_metric_value_enqueue (self, met, val);
    met->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(met));
  }

  return 0;
}

int v_data_ipmi_sdr_process_psu (i_resource *self, i_list *row)
{
  char *values[2];

  /* Get values */
  i_list_move_head (row);
  values[0] = i_list_restore (row);
  if (!values[0]) return -1;
  i_list_move_next (row);
  values[1] = i_list_restore (row);
  if (!values[1]) return -1;

  /* Get metric */
  i_metric *met = NULL;
  if (strlen(values[0]) > 3)
  {
    /* Get PSU Slot */
    char *psu_desc = (char *) malloc (5);
    memset (psu_desc, 0, 5);
    strncpy (psu_desc, values[0], 4);
    v_psu_item *psu = v_psu_get (psu_desc);
    if (!psu)
    {
      psu = v_psu_create (self, psu_desc);
      if (!psu) return -1;
    }
    free (psu_desc);

    /* Get metric */
    if (strstr(values[0], "Exhaust")) met = psu->exhaust_temp;
    else if (strstr(values[0], "Fan Out")) met = psu->fan_out_rpm;
    else if (strstr(values[0], "Fan In")) met = psu->fan_in_rpm;
    else if (strstr(values[0], "5V STBY")) met = psu->v_5v_standby;
    else if (strstr(values[0], "12V Pwr")) met = psu->power;
    else if (strstr(values[0], "12V")) met = psu->v_12v;
    else if (strlen(values[0]) == 4) met = psu->power;
  }

  /* Set value */
  if (met)
  {
    i_metric_value *val;
    
    val = i_metric_value_create ();
    val->flt = atof (values[1]); 
    i_metric_value_enqueue (self, met, val);
    met->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(met));
  }

  return 0;
}

int v_data_ipmi_sdr_process_pci (i_resource *self, i_list *row)
{
  char *values[2];

  /* Get values */
  i_list_move_head (row);
  values[0] = i_list_restore (row);
  if (!values[0]) return -1;
  i_list_move_next (row);
  values[1] = i_list_restore (row);
  if (!values[1]) return -1;

  /* Get metric */
  i_metric *met = NULL;
  if (strlen(values[0]) > 9)
  {
    /* Get PCI Slot */
    char *pci_desc = (char *) malloc (11);
    memset (pci_desc, 0, 11);
    strncpy (pci_desc, values[0], 10);
    v_pci_item *pci = v_pci_get (pci_desc);
    if (!pci)
    {
      pci = v_pci_create (self, pci_desc);
      if (!pci) return -1;
    }
    free (pci_desc);

    /* Get metric */
    if (strstr(values[0], "Pwr")) met = pci->power;
    else met = pci->temp;
  }

  /* Set value */
  if (met)
  {
    i_metric_value *val;
    
    val = i_metric_value_create ();
    val->flt = atof (values[1]); 
    i_metric_value_enqueue (self, met, val);
    met->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(met));
  }

  return 0;
}

int v_data_ipmi_sdr_process_mainboard (i_resource *self, i_list *row)
{
  char *values[2];

  /* Get values */
  i_list_move_head (row);
  values[0] = i_list_restore (row);
  if (!values[0]) return -1;
  i_list_move_next (row);
  values[1] = i_list_restore (row);
  if (!values[1]) return -1;

  /* Get metric */
  i_metric *met = NULL;
  v_mainboard_item *mb = v_mainboard_static_item ();
  if (strstr(values[0], "NorthBridge HS")) met = mb->nbridge_temp;
  else if (strstr(values[0], "IOH HS")) met = mb->nbridge_temp;
  else if (strstr(values[0], "STBY 5V")) met = mb->standby;
  else if (strstr(values[0], "STBY 3.3V")) met = mb->standby;
  else if (strstr(values[0], "Main 3.3V")) met = mb->main_3_3v;
  else if (strstr(values[0], "Main 12V")) met = mb->main_12v;
  else if (strstr(values[0], "N & S Bridge")) met = mb->ns_bridge_power;
  else if (strstr(values[0], "IOH Power")) met = mb->ns_bridge_power;
  else if (strstr(values[0], "Chunnel"))
  {
    v_cpu_item *cpu = NULL;
    if (strstr(values[0], "L Chunnel")) 
    { cpu = v_cpu_x (0); }
    else
    { cpu = v_cpu_x (1); }
    if (cpu)
    {
      met = cpu->temp_inlet;
    }
  }  
  
  /* Set value */
  if (met)
  { 
    i_metric_value *val;
    
    val = i_metric_value_create ();
    val->flt = atof (values[1]);  
    i_metric_value_enqueue (self, met, val);
    met->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(met));
  }
  
  return 0;
}



