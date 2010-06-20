#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "cement.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "form.h"
#include "list.h"
#include "auth.h"
#include "timeutil.h"
#include "device.h"
#include "hierarchy.h"

/** \addtogroup form_item Form Items
 * @ingroup form
 * @{
 */

int form_metgraph_large (i_resource *self, i_form_reqdata *reqdata)
{
  int period;
  i_metric *met = NULL;
  i_form_item *item;

  /* Check auth */
  if (!reqdata || !reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }

  /* Create form */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_metgraph_large failed to create form"); return -1; }

  /* Get local entity */
  if (reqdata->entaddr)
  { met = (i_metric *) i_entity_local_get (self, reqdata->entaddr); }
  else
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "No entity specified");
    return 1;
  }

  /* Check entity type */
  if (met->ent_type != ENT_METRIC)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified entity is not a metric"); return 1; }

  /* Determine period from form_passdata */
  if (reqdata->form_passdata && reqdata->form_passdata_size > 0)
  { period = atoi (reqdata->form_passdata); }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No period specified"); return 1; }
  
  /* Frame Start */
  i_form_frame_start (reqdata->form_out, met->name_str, met->desc_str); 

  /* Render Graph */
  item = i_form_metgraph_add (self, reqdata->form_out, met, GRAPHSIZE_LARGE, reqdata->ref_sec, period);

  /* Frame End */
  i_form_frame_end (reqdata->form_out, met->name_str); 

  return 1;
}

int form_metcgraph_large (i_resource *self, i_form_reqdata *reqdata)
{
  int period;
  char *name_str;
  i_object *obj = NULL;
  i_metric_cgraph *cgraph;
  i_form_item *item;
  
  /* Check auth */
  if (!reqdata || !reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }
  
  /* Create form */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_metgraph_large failed to create form"); return -1; }
  
  /* Get local entity */
  if (reqdata->entaddr)
  { obj = (i_object *) i_entity_local_get (self, reqdata->entaddr); }
  else
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "No entity specified");
    return 1;
  } 
  
  /* Check entity type */
  if (obj->ent_type != ENT_OBJECT)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified entity is not an object"); return 1; }
  
  /* Determine period and cgraph name_str from form_passdata */
  if (reqdata->form_passdata && reqdata->form_passdata_size > 0)
  { 
    char *periodptr;
    name_str = strdup ((char *) reqdata->form_passdata);
    periodptr = strchr (name_str, ':');
    if (!periodptr)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Corrupt passdata"); free (name_str); return 1; }
    *periodptr = '\0';
    periodptr++;
    period = atoi (periodptr); 
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No period specified"); return 1; }

  /* Get cgraph */
  cgraph = i_metric_cgraph_get (obj, name_str);
  free (name_str);
  if (!cgraph)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified Custom/Combined Graph not found"); return 1; }
  
  /* Frame Start */
  i_form_frame_start (reqdata->form_out, cgraph->name_str, cgraph->title_str);
  
  /* Render Graph */
  item = i_form_metcgraph_add (self, reqdata->form_out, cgraph, GRAPHSIZE_LARGE, reqdata->ref_sec, period);
  
  /* Frame End */
  i_form_frame_end (reqdata->form_out, cgraph->name_str);

  return 1;
}

/* @} */
