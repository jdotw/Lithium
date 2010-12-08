#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>

#include "snmp.h"
#include "avail.h"
#include "icmp.h"

/* ICMP Object Refresh */

int l_icmp_refresh (i_resource *self, i_metric *met, int opcode)
{
  /* ICMP Reachable metric */

  l_icmp_item *icmp = met->obj->itemptr;
  l_icmp_echo_callback *echocb = met->refresh_data;

  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:       /* Begin the refresh process */
      if (met == icmp->reachable)
      {
        /* Send echo */
        echocb = l_icmp_echo_send (self, met->obj->desc_str, 56, (met->obj->cnt->refresh_int_sec - 1), l_icmp_refresh_echocb, met);
        if (!echocb)
        { 
          met->refresh_result = REFRESULT_TOTAL_FAIL;
        }
      }
      else
      { 
        /* Response Metric, always set to OK, nothing more to do */
        met->refresh_result = REFRESULT_OK; 
        return 1;
      }
      break;
    case REFOP_COLLISION:     /* Handle a refresh collision */
      break;
    case REFOP_TERMINATE:     /* Terminate an existing refresh */
      if (echocb)
      { met->refresh_result = REFRESULT_TOTAL_FAIL; }
      break;
    case REFOP_CLEANDATA:     /* Cleanup the refresh_data struct */
      if (echocb)
      { l_icmp_echo_callback_free (echocb); met->refresh_data = NULL; }
      break;
    default:
      i_printf (1, "l_icmp_refresh unknown opcode received (%i)", opcode);
  }

  return 0;
}

int l_icmp_refresh_echocb (i_resource *self, int result, l_icmp_echo_callback *callback, void *passdata)
{
  /* Called when an ICMP response is received or a timeout occurs */
  i_metric_value *reachval;
  i_metric_value *respval;
  i_metric *met = passdata;
  l_icmp_item *icmp = met->obj->itemptr;

  /* Set Reresh State 
   *
   * Note: State is always OK because the true 
   *        outcome of the refresh will be reflected
   *        in the metric's value
   */
  met->refresh_result = REFRESULT_OK; 
  icmp->response->refresh_result = REFRESULT_OK;

  /* NULLify refresh data */
  met->refresh_data = NULL;

  /* Create Values */
  reachval = i_metric_value_create ();
  respval = i_metric_value_create ();
  switch (result)
  {
    case ECHORES_OK:
      /* Success */
      reachval->integer = 1;
      respval->integer = result;
      break;
    default:
      /* Failure */
      reachval->integer = 0;
      respval->integer = result;
  }
  
  /* Enqueue Values */
  i_metric_value_enqueue (self, met, reachval);
  i_metric_value_enqueue (self, icmp->response, respval);

  /* Record avail&rt */
  if (icmp->avail_flag == 1)
  {
    if (result == ECHORES_OK)
    { l_avail_record_ok (l_icmp_availobj(), &callback->req_tv, &callback->resp_tv); }
    else
    { l_avail_record_fail (l_icmp_availobj()); }
  }

  /* Terminate the refresh */
  i_entity_refresh_terminate (ENTITY(met));

  return 0;
}
