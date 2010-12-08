#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "induction.h"
#include "cement.h"
#include "entity.h"
#include "object.h"
#include "metric.h"
#include "rrdtool.h"
#include "rrd.h"
#include "form.h"

/** \addtogroup form_item Form Items
 * @ingroup form
 * @{
 */

/*
 * Functions to add metric graphs to forms
 */

i_form_item* i_form_metgraph_add (i_resource *self, i_form *form, i_metric *met, int graphsize, time_t ref_sec, int period)
{
  char *name_str;
  char *label_str;
  char *period_str;
  char *error_str;
  char *pass_str;
  i_form_item *item;
  i_rrdtool_cmd *cmd;

  /* Check/Set ref_sec */
  if (ref_sec < 1)
  { ref_sec = time (NULL); }
 
  /* Add image with NULL image file initially */
  period_str = i_metric_graph_periodstr (ref_sec, period);
  asprintf (&name_str, "%s_%i_%li", met->name_str, period, ref_sec);
  asprintf (&label_str, "%s %s (%s)", met->obj->desc_str, met->desc_str, period_str);
  item = i_form_image_add (form, name_str, label_str, NULL, NULL);
  free (name_str);
  free (period_str);
  if (!item)
  { 
    asprintf (&error_str, "Failed to add image for graph %s", label_str);
    free (label_str);
    item = i_form_string_add (form, "error", "Error", error_str);
    free (error_str);
    return NULL;
  }

  /* Add Large Graph Link */
  asprintf (&pass_str, "%i", period);
  i_form_item_add_link (item, 0, 0, 0, RES_ADDR(self), ENT_ADDR(met), "metgraph_large", ref_sec, pass_str, strlen(pass_str)+1);
  if (pass_str) free (pass_str);

  /* Render the graph */
  cmd = i_metric_graph_render (self, met, graphsize, ref_sec, period, i_form_metgraph_rrdcb, item);
  if (!cmd)
  { 
    asprintf (&error_str, "Failed to render graph %s", label_str);
    free (label_str);
    item = i_form_string_add (form, "error", "Error", error_str);
    free (error_str);
    return NULL;
  }
  free (label_str);

  /* Set item state to NOTREADY */
  i_form_item_notready (self, item);
  
  return item;
}

i_form_item* i_form_metcgraph_add (i_resource *self, i_form *form, i_metric_cgraph *cgraph, int graphsize, time_t ref_sec, int period)
{
  char *name_str;
  char *label_str;
  char *period_str;
  char *error_str;
  char *pass_str;
  i_form_item *item;
  i_rrdtool_cmd *cmd;

  /* Check/Set ref_sec */
  if (ref_sec < 1)
  { ref_sec = time (NULL); }
 
  /* Add image with NULL image file initially */
  period_str = i_metric_graph_periodstr (ref_sec, period);
  asprintf (&name_str, "%s_%i_%li", cgraph->name_str, period, ref_sec);
  asprintf (&label_str, "%s (%s)", cgraph->title_str, period_str);
  item = i_form_image_add (form, name_str, label_str, NULL, NULL);
  free (name_str);
  free (label_str);
  free (period_str);
  if (!item)
  {
    asprintf (&error_str, "Failed to add image for graph %s", cgraph->title_str);
    item = i_form_string_add (form, "error", "Error", error_str);
    free (error_str);
    return NULL;
  }

  /* Add Large Graph Link */
  asprintf (&pass_str, "%s:%i", cgraph->name_str, period);
  i_form_item_add_link (item, 0, 0, 0, RES_ADDR(self), ENT_ADDR(cgraph->obj), "metcgraph_large", ref_sec, pass_str, strlen(pass_str)+1);
  if (pass_str) free (pass_str); 
  
  /* Render the graph */
  cmd = i_metric_cgraph_render (self, cgraph, graphsize, ref_sec, period, i_form_metgraph_rrdcb, item);
  if (!cmd)
  {
    asprintf (&error_str, "Failed to render graph %s", cgraph->title_str);
    item = i_form_string_add (form, "error", "Error", error_str);
    free (error_str);
    return NULL;
  }

  /* Set item state to NOTREADY */
  i_form_item_notready (self, item);
  
  return item;

}

int i_form_metgraph_rrdcb (i_resource *self, i_rrdtool_cmd *cmd, int result, void *passdata)
{
  int num;
  i_form_item *item = passdata;

  /* Set image file for item */
  num = i_form_image_setimage (item, cmd->fullpath_str); 
  if (num != 0)
  { 
    i_printf (1, "i_form_metgraph_rrdcb warning, failed to call i_form_image_setimage for %s", 
      cmd->fullpath_str);
  }
  
  /* Set item state to ready */
  i_form_item_ready (self, item);

  return 0;
}

/* @} */
