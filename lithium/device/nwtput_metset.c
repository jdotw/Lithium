#include <stdlib.h>

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

#include "nwtput.h"

/* Network Throughput Metric Sets 
 *
 * A set of metrics is a pair of input/output 
 * packet per second metrics and a pair of input/output
 * bits per second metrics. A single function to add/remove
 * four metrics at a time is provided for simplicity. The 
 * metrics specified need not be related in any way. 
 * 
 */

int l_nwtput_metset_add (i_resource *self, i_metric *input_bps, i_metric *output_bps, i_metric *input_pps, i_metric *output_pps)
{
  /* Add the specified metrics to the relevant objects/acsum metrics */
  i_object *bpsobj = l_nwtput_bpsobj ();
  i_object *ppsobj = l_nwtput_ppsobj ();
  l_nwtput_item *bpsitem;
  l_nwtput_item *ppsitem;

  if (!ppsobj || !bpsobj) return -1;

  bpsitem = (l_nwtput_item *) bpsobj->itemptr;
  ppsitem = (l_nwtput_item *) ppsobj->itemptr;

  /* Input Bps */
  if (input_bps)
  { i_metric_acsum_addmet (bpsitem->input, input_bps, 0); }
  
  /* Output Bps */
  if (output_bps)
  { i_metric_acsum_addmet (bpsitem->output, output_bps, 0); }
  
  /* Input Pps */
  if (input_pps)
  { i_metric_acsum_addmet (ppsitem->input, input_pps, 0); }
  
  /* Output Pps */
  if (output_pps)
  { i_metric_acsum_addmet (ppsitem->output, output_pps, 0); }

  return 0;
}

int l_nwtput_metset_remove (i_resource *self, i_metric *input_bps, i_metric *output_bps, i_metric *input_pps, i_metric *output_pps)
{
  /* Remove the specified metrics to the relevant objects/acsum metrics */
  i_object *bpsobj = l_nwtput_bpsobj ();
  i_object *ppsobj = l_nwtput_ppsobj ();
  l_nwtput_item *bpsitem;
  l_nwtput_item *ppsitem;

  if (!ppsobj || !bpsobj) return -1;

  bpsitem = (l_nwtput_item *) bpsobj->itemptr;
  ppsitem = (l_nwtput_item *) ppsobj->itemptr;

  /* Input Bps */
  if (input_bps)
  { i_metric_acsum_removemet (bpsitem->input, input_bps); }

  /* Output Bps */
  if (output_bps)
  { i_metric_acsum_removemet (bpsitem->output, output_bps); }

  /* Input Pps */
  if (input_pps)
  { i_metric_acsum_removemet (ppsitem->input, input_pps); }

  /* Output Pps */
  if (output_pps)
  { i_metric_acsum_removemet (ppsitem->output, output_pps); }

  return 0;

}
