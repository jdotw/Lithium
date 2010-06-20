#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "hierarchy.h"
#include "list.h"
#include "colour.h"
#include "cement.h"
#include "entity.h"
#include "form.h"
#include "auth.h"
#include "navtree.h"
#include "navform.h"

/** \addtogroup form_nav Navigation Menu
 * @ingroup form
 * @{
 */

/* Form Generation */

int form_navigation_menu (i_resource *self, i_form_reqdata *reqdata)
{
  i_entity *ent = NULL;
  i_navtree_node *root;

  if (!reqdata || !reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }

  /* Get local entity */
  if (reqdata->entaddr)
  {
    /* Entity Specified */
    ent = i_entity_local_get (self, reqdata->entaddr);
  }
  if (!ent)
  {
    /* No Entity Specified or Entity Not Found */
    switch (self->type)
    {
      case RES_CUSTOMER:
        ent = ENTITY(self->hierarchy->cust);
        break;
      case RES_SITE:
        ent = ENTITY(self->hierarchy->site);
        break;
      case RES_DEVICE:
        ent = ENTITY(self->hierarchy->dev);
        break;
    }
  }

  if (!ent) return -1;
  
  /* Generate navtree */
  root = i_navtree_generate (self, ent, NULL);
  if (!root) 
  { 
    i_printf (1, "form_navigation failed to generate tree for entity %s %s", 
      i_entity_typestr (ent->ent_type), ent->name_str);
    return -1;
  }
  
  /* Create form */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_navigation failed to create form"); return -1; }

  /* Start Frame */
  i_form_frame_start (reqdata->form_out, "navform_frame", "Navigation Menu");
//  i_form_frame_start (reqdata->form_out, "navform_frame", i_entity_typestr(ent->ent_type));

  /* Render Tree */
  i_navform_render_navtree (self, reqdata->form_out, root, 0);

  /* Free Tree */
  i_navtree_node_free (root);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, "navform_frame");

  return 1;
}

int i_navform_render_navtree (i_resource *self, i_form *form, i_navtree_node *node, unsigned short level)
{
  /* This func recursively renders the child nodes
   * of the specified navtree node. It is assumed that
   * the actual node itself has already been rendered
   */

  int i;
  int table_count = 0;
  char *labels[level+1];
  i_navtree_node *child;
  i_form_item *table = NULL;

  /* NULLify labels */
  for (i=0; i < level; i++)
  { labels[i] = NULL; }

  for (i_list_move_head(node->child_list); (child=i_list_restore(node->child_list))!=NULL; i_list_move_next(node->child_list))
  {
    int row;
    char *shortdesc_str = NULL;
    char *table_name;

    /* Create Table */      
    asprintf (&table_name, "navform_%s_%i_%i", node->name_str, level, table_count);
    table = i_form_table_create (form, table_name, NULL, level+1);
    free (table_name);
    i_form_item_add_option (table, ITEM_OPTION_NOHEADERS, 0, 0, 0, NULL, 0);
    
    /* Render Child Node */
    if (child->desc_str && strlen(child->desc_str) > 30)
    { 
      /* String too long */
      char buf[25];

      snprintf (buf, 25, "%s", child->desc_str);
      asprintf (&shortdesc_str, "%s...", buf);
      labels[level] = shortdesc_str;
    }
    else
    { labels[level] = child->desc_str; }

    row = i_form_table_add_row (table, labels); 
    if (shortdesc_str) free (shortdesc_str);

    if (child->form_str)
    { i_form_table_add_link (table, level, row, child->resaddr, child->entaddr, child->form_str, 0, child->passdata, child->passdata_size); }
    i_form_item_add_option (table, ITEM_OPTION_ALIGN, 1, level, row, "LEFT", 5);
    if (child->opstate > ENTSTATE_NORMAL)
    { i_form_item_add_option (table, ITEM_OPTION_FGCOLOR, 1, level, row, i_colour_fg_str(child->opstate), 8); }

    /* Set spacer width */
    for (i=0; i < level; i++)
    { i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, i, row, "5", 2); }                      

    /* Render Children */
    if (child->child_list && child->child_list->size > 0)
    {
      /* Recursively call i_navform_render_navtree for the child node */
      i_navform_render_navtree (self, form, child, level+1);
    }
  }

  return 0;
}

/* @} */
