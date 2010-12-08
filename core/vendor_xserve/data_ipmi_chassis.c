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

#include "chassis.h"
#include "cpu.h"
#include "pci.h"
#include "psu.h"
#include "mainboard.h"
#include "ipmitool.h"
#include "csv.h"
#include "data.h"
#include "xserve.h"

/*
 * IPMI 'chassis'
 */

static v_ipmitool_cmd *static_cmd = NULL;

/* Data Source Refrsesh */

int v_data_ipmi_chassis_refresh (i_resource *self, i_metric *met, int opcode)
{
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh processa */
      static_cmd = v_ipmitool_exec (self, "raw", "0x00 0x01", 0, v_data_ipmi_chassis_ipmicb, met);
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

int v_data_ipmi_chassis_ipmicb (i_resource *self, v_ipmitool_cmd *cmd, int result, void *passdata)
{
  i_metric *datamet = passdata;

  if (result != IPMIRESULT_OK || !cmd || !cmd->raw_data || cmd->raw_datasize < 3)
  {
    datamet->refresh_result = REFRESULT_TOTAL_FAIL;
    i_entity_refresh_terminate (ENTITY(datamet));
    return -1;
  }

  /* Get chassis */
  v_chassis_item *chassis = v_chassis_static_item ();

  /* Interpret data */
  i_metric_value *val;
  uint8_t octet_1;
  uint8_t octet_2;
  uint8_t octet_3;
  memcpy (&octet_1, cmd->raw_data, 1);
  memcpy (&octet_2, cmd->raw_data + 1, 1);
  memcpy (&octet_3, cmd->raw_data + 2, 1);

  /* 
   * Current Power State - 1st Byte 
   */

  /* System Power */
  val = i_metric_value_create ();
  if (octet_1 & CH_SYSTEMPOWER) 
  { 
    /* System is on */
    val->integer = 1;

    /* Set flag */
    v_powerstate_set (1);
  }
  else 
  { 
    /* System is off */
    val->integer = 0; 

    /* Set flag */
    v_powerstate_set (0);
  }
  i_metric_value_enqueue (self, chassis->systempower, val);
  chassis->systempower->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(chassis->systempower));

  /* Power Overload */
  val = i_metric_value_create ();
  if (octet_1 & CH_POWEROVERLOAD) { val->integer = 1; }
  else { val->integer = 0; }
  i_metric_value_enqueue (self, chassis->poweroverload, val);
  chassis->poweroverload->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(chassis->poweroverload));

  /* Power Fault */
  val = i_metric_value_create ();
  if (octet_1 & CH_POWERFAULT) { val->integer = 1; }
  else { val->integer = 0; }
  i_metric_value_enqueue (self, chassis->mainspowerfault, val);
  chassis->mainspowerfault->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(chassis->mainspowerfault));

  /* Power Control Fault */
  val = i_metric_value_create ();
  if (octet_1 & CH_POWERCONTROLFAULT) { val->integer = 1; }
  else { val->integer = 0; }
  i_metric_value_enqueue (self, chassis->powercontrolfault, val);
  chassis->powercontrolfault->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(chassis->powercontrolfault));

  /*
   * Misc Chassis State - 2nd Byte
   */
  
  /* Power-on Cause */
  val = i_metric_value_create ();
  if (octet_2 & CH_PWR_IPMION) { val->integer = 1; }
  else { val->integer = 0; }
  i_metric_value_enqueue (self, chassis->poweroncause, val);
  chassis->poweroncause->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(chassis->poweroncause));

  /* Power-off Cause */
  val = i_metric_value_create ();
  if (octet_2 & CH_PWR_POWERFAULT) { val->integer = 4; }
  else if (octet_2 & CH_PWR_POWEROVERLOAD) { val->integer = 2; }
  else if (octet_2 & CH_PWR_ACFAILED) { val->integer = 1; }
  i_metric_value_enqueue (self, chassis->poweroffcause, val);
  chassis->poweroffcause->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(chassis->poweroffcause));

  /*
   * Faults - 3rd Byte 
   */

  /* Keylock */
  val = i_metric_value_create ();
  if (octet_3 & CH_KEYLOCK) { val->integer = 1; }
  else { val->integer = 0; }
  i_metric_value_enqueue (self, chassis->keylock, val);
  chassis->keylock->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(chassis->keylock));

  /* Drive Fault */
//  val = i_metric_value_create ();
//  if (octet_3 & CH_DRIVEFAULT) { val->integer = 1; }
//  else { val->integer = 0; }
//  i_metric_value_enqueue (self, chassis->drivefault, val);
//  chassis->drivefault->refresh_result = REFRESULT_OK;
//  i_entity_refresh_terminate (ENTITY(chassis->drivefault));
  
  /* Cooling Fault */
//  val = i_metric_value_create ();
//  if (octet_3 & CH_COOLINGFAULT) { val->integer = 1; }
//  else { val->integer = 0; }
//  i_metric_value_enqueue (self, chassis->coolingfault, val);
//  chassis->coolingfault->refresh_result = REFRESULT_OK;
//  i_entity_refresh_terminate (ENTITY(chassis->coolingfault));
  
  /* ID Light */
  val = i_metric_value_create ();
  if (octet_3 & CH_IDLIGHT) { val->integer = 1; }
  else { val->integer = 0; }
  i_metric_value_enqueue (self, chassis->idlight, val);
  chassis->idlight->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(chassis->idlight));

  /* Set result and terminate */
  datamet->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(datamet));
  
  return 0;
}



