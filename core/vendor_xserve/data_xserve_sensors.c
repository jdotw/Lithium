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
#include <lithium/snmp.h>

#include "cpu.h"
#include "enclosure.h"
#include "power.h"
#include "plist.h"
#include "data.h"

/*
 * Xserve Sensors Processing
 */

int v_data_xserve_process_sensors (i_resource *self, v_plist_req *req, xmlNodePtr sensorArrayNode)
{
  /* Process each sensor */
  xmlNodePtr node;
  for (node = sensorArrayNode->children; node; node = node->next)
  {
    /* Look through each sensor dict */
    if (strcmp((char *)node->name, "dict") == 0)
    {
      /* Get sensor type */
      char *type_str = v_plist_data_from_dict (req->plist, node, "type");
      if (!type_str) continue;
      if (strstr(type_str, "temperature"))
      {
        /* Process a fan sensor */
        v_data_xserve_process_temp_sensor (self, req, node);
      }
      else if (strstr(type_str, "power"))
      {
        /* Process power sensors */
        v_data_xserve_process_power_sensor (self, req, node);
      }
      else if (strstr(type_str, "voltage"))
      {
        /* Process voltage sensors */
        v_data_xserve_process_voltage_sensor (self, req, node);
      }
      else if (strstr(type_str, "current"))
      {
        /* Process current sensors */
        v_data_xserve_process_current_sensor (self, req, node);
      }
      free (type_str);
      type_str = NULL;
    }
  }

  return 0;
}

int v_data_xserve_process_temp_sensor (i_resource *self, v_plist_req *req, xmlNodePtr dictNode)
{
  i_metric *temp_met = NULL;
  
  /* Get group */
  char *group = v_plist_data_from_dict (req->plist, dictNode, "group");
  if (!group) return -1;

  /* Get location */
  char *location = v_plist_data_from_dict (req->plist, dictNode, "location");
  if (!location) { free (group); return -1; }

  /* Location */
  if (strstr(group, "Processor"))
  {
    /* 
     * CPU Temperature 
     */

    /* Get CPU */
    int cpu_id;
    v_cpu_item *cpu;
    cpu_id = atoi ((location+4));
    cpu = v_cpu_x (cpu_id - 1);
    if (!cpu) 
    { i_printf (2, "v_data_xserve_process_temp_sensor failed to get cpu %i", cpu_id - 1); free (group); free (location); return -1; }

    /* Set the metric */
    if (strstr(location, "Internal"))
    { temp_met = cpu->temp_internal; }
    else if (strstr(location, "Inlet"))
    { temp_met = cpu->temp_inlet; }
    else if (strstr(location, "Ambient"))
    { temp_met = cpu->temp_ambient; }
  }
  else if (strstr(group, "Enclosure"))
  {
    /* 
     * Enclosure Temperature
     */

    /* Get enclosure item */
    v_enclosure_item *enc = v_enclosure_static_item ();

    /* Set the metric */
    if (strstr(location, "PCI Slots"))
    { temp_met = enc->pci_temp; }
    else if (strstr(location, "DIMM"))
    { temp_met = enc->memory_temp; }
    else if (strstr(location, "Between the Processors"))
    { temp_met = enc->int_amb_temp; }
    else if (strstr(location, "System Controller Internal"))
    { temp_met = enc->sc_int_temp; }
    else if (strstr(location, "System Controller Ambient"))
    { temp_met = enc->sc_amb_temp; }
  }
  
  /* Set Value */
  if (temp_met)
  {
    char *str = v_plist_data_from_dict (req->plist, dictNode, "current-value");
    if (str)
    {
      i_metric_value *val;
      val = i_metric_value_create ();
      val->flt = (atof (str)) / 65536;
      i_metric_value_enqueue (self, temp_met, val);
      temp_met->refresh_result = REFRESULT_OK;
      free (str);
    }
    else
    { temp_met->refresh_result = REFRESULT_TOTAL_FAIL; }
  }

  free (group);
  free (location);

  return 0;
}

int v_data_xserve_process_power_sensor (i_resource *self, v_plist_req *req, xmlNodePtr dictNode)
{
  /* Get location */
  char *location = v_plist_data_from_dict (req->plist, dictNode, "location");
  if (!location) return -1;

  /* Get CPU */
  int cpu_id;
  v_cpu_item *cpu;
  cpu_id = atoi ((location+4));
  cpu = v_cpu_x (cpu_id - 1);
  if (!cpu)
  { i_printf (1, "v_data_xserve_process_power_sensor failed to get cpu %i", cpu_id - 1); return -1; }

  char *str = v_plist_data_from_dict (req->plist, dictNode, "current-value");
  if (str)
  {
    i_metric_value *val;
    val = i_metric_value_create ();
    val->flt = (atof (str)) / 65536;
    i_metric_value_enqueue (self, cpu->power_watts, val);
    cpu->power_watts->refresh_result = REFRESULT_OK;
    free (str);
  }
  else
  { cpu->power_watts->refresh_result = REFRESULT_TOTAL_FAIL; }

  free (location);

  return 0;
}

int v_data_xserve_process_voltage_sensor (i_resource *self, v_plist_req *req, xmlNodePtr dictNode)
{
  i_metric *volt_met = NULL;
  
  /* Get location */
  char *location = v_plist_data_from_dict (req->plist, dictNode, "location");
  if (!location) return -1;

  /* Checl */
  if (strstr(location, "CPU"))
  {
    /* 
     * CPU Vcore
     */

    /* Get CPU */
    int cpu_id;
    v_cpu_item *cpu;
    cpu_id = atoi ((location+4));
    cpu = v_cpu_x (cpu_id - 1);
    if (!cpu) 
    { i_printf (1, "v_data_xserve_process_temp_sensor failed to get cpu %i", cpu_id - 1); free (location); return -1; }

    /* Set the metric */
    volt_met = cpu->power_vcore;
  }
  else
  {
    /* 
     * General Power Sensor
     */

    /* Get enclosure item */
    v_power_item *power = v_power_static_item ();

    /* 
     * Set the metric 
     */

    if (!strcmp(location, "12v"))
    { volt_met = power->v_12v; }
    
    if (!strcmp(location, "1.2v Sleep"))
    { volt_met = power->v_1_2v_sleep; }
    
    if (!strcmp(location, "System Controller Vcore"))
    { volt_met = power->sc_vcore; }
    
    if (!strcmp(location, "1.5v Sleep"))
    { volt_met = power->v_1_5v_sleep; }
    
    if (!strcmp(location, "3.3v"))
    { volt_met = power->v_3_3v; }
    
    if (!strcmp(location, "5v Sleep"))
    { volt_met = power->v_5v_sleep; }
    
    if (!strcmp(location, "3.3v Sleep"))
    { volt_met = power->v_3_3v_sleep; }
    
    if (!strcmp(location, "DDR IO Sleep"))
    { volt_met = power->ddr_io_sleep; }
    
    if (!strcmp(location, "1.5v"))
    { volt_met = power->v_1_5v; }
    
    if (!strcmp(location, "3.3v Trickle"))
    { volt_met = power->v_3_3v_trickle; }
    
    if (!strcmp(location, "DDR IO"))
    { volt_met = power->ddr_io; }
    
    if (!strcmp(location, "1.2v"))
    { volt_met = power->v_1_2v; }
    
    if (!strcmp(location, "12v Trickle"))
    { volt_met = power->v_12v_trickle; }
    
    if (!strcmp(location, "System Bus IO VDD"))
    { volt_met = power->io_vdd; }
    
    if (!strcmp(location, "5v"))
    { volt_met = power->v_5v; }
    
    if (!strcmp(location, "1.8v"))
    { volt_met = power->v_1_8v; }

  }
  
  /* Set Value */
  if (volt_met)
  {
    char *str = v_plist_data_from_dict (req->plist, dictNode, "current-value");
    if (str)
    {
      i_metric_value *val;
      val = i_metric_value_create ();
      val->flt = (atof (str)) / 65536;
      i_metric_value_enqueue (self, volt_met, val);
      volt_met->refresh_result = REFRESULT_OK;
      free (str);
    }
    else
    { volt_met->refresh_result = REFRESULT_TOTAL_FAIL; }
  }

  free (location);

  return 0;
}

int v_data_xserve_process_current_sensor (i_resource *self, v_plist_req *req, xmlNodePtr dictNode)
{
  i_metric *cur_met = NULL;
  
  /* Get location */
  char *location = v_plist_data_from_dict (req->plist, dictNode, "location");
  if (!location) return -1;

  /* Get CPU */
  int cpu_id;
  v_cpu_item *cpu;
  cpu_id = atoi ((location+4));
  cpu = v_cpu_x (cpu_id - 1);
  if (!cpu)
  { i_printf (1, "v_data_xserve_process_power_sensor failed to get cpu %i", cpu_id - 1); free (location); return -1; }

  /*
   * Set metric
   */

  if (strstr(location, "Core"))
  { cur_met = cpu->current_core; }

  if (strstr(location, "12v"))
  { cur_met = cpu->current_12v; }
  
  /* Set value */
  if (cur_met)
  {  
    char *str = v_plist_data_from_dict (req->plist, dictNode, "current-value");
    if (str)
    {
      i_metric_value *val;
      val = i_metric_value_create ();
      val->flt = (atof (str)) / 65536;
      i_metric_value_enqueue (self, cur_met, val);
      cur_met->refresh_result = REFRESULT_OK;
      free (str);
    }
    else
    { cur_met->refresh_result = REFRESULT_TOTAL_FAIL; }
  }

  free (location);

  return 0;
}


