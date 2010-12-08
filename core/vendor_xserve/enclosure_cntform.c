#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/timeutil.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>
#include <lithium/snmp.h>

#include "enclosure.h"

int v_enclosure_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Enclosure Container Main Form
   */
  i_form_item *item = NULL;
  i_container *cnt = (i_container *) ent;
  
  /* Start Frame */
  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  /* 
   * Temperature Table 
   */

  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    int row;
    char *labels[5];
    v_enclosure_item *enc;
    
    item = i_form_table_create (reqdata->form_out, "templist", NULL, 5);
    if (!item) { i_printf (1, "v_fan_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    labels[0] = "Int. Ambient";
    labels[1] = "Sys.Ctrl Ambient";
    labels[2] = "Sys.Ctrl Internal";
    labels[3] = "RAM Slots";
    labels[4] = "PCI Slots";
    i_form_table_add_row (item, labels);
    labels[0] = NULL;
    labels[1] = NULL;
    labels[2] = NULL;
    labels[3] = NULL;
    labels[4] = NULL;

    enc = v_enclosure_static_item ();

    if (enc->int_amb_temp)
    { labels[0] = i_metric_valstr (enc->int_amb_temp, NULL); }    
    if (enc->sc_amb_temp)
    { labels[1] = i_metric_valstr (enc->sc_amb_temp, NULL); }    
    if (enc->sc_int_temp)
    { labels[2] = i_metric_valstr (enc->sc_int_temp, NULL); }    
    if (enc->memory_temp)
    { labels[3] = i_metric_valstr (enc->memory_temp, NULL); }    
    if (enc->pci_temp)
    { labels[4] = i_metric_valstr (enc->pci_temp, NULL); }    

    row = i_form_table_add_row (item, labels);
    if (enc->int_amb_temp) i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(enc->int_amb_temp), NULL, 0, NULL, 0);
    if (enc->sc_amb_temp) i_form_table_add_link (item, 1, row, RES_ADDR(self), ENT_ADDR(enc->sc_amb_temp), NULL, 0, NULL, 0);
    if (enc->sc_int_temp) i_form_table_add_link (item, 2, row, RES_ADDR(self), ENT_ADDR(enc->sc_int_temp), NULL, 0, NULL, 0);
    if (enc->memory_temp) i_form_table_add_link (item, 3, row, RES_ADDR(self), ENT_ADDR(enc->memory_temp), NULL, 0, NULL, 0);
    if (enc->pci_temp) i_form_table_add_link (item, 4, row, RES_ADDR(self), ENT_ADDR(enc->pci_temp), NULL, 0, NULL, 0);
    
    if (labels[0]) free (labels[0]);
    if (labels[1]) free (labels[1]);
    if (labels[2]) free (labels[2]);
    if (labels[3]) free (labels[3]);
    if (labels[4]) free (labels[4]);

    i_form_string_add (reqdata->form_out, "templist_note", "Note", "'*' next to a value indicates the data is not current");
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
