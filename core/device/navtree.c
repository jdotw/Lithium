#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/navtree.h>

#include "navtree.h"

int l_navtree_func (i_resource *self, i_entity *ent, i_navtree_node *parent)
{
  i_entity *child;
  i_navtree_node *root;

  /* Expand flag : 
   *  0 = NAVTREE_EXP_ALWAYS   - Always expand the tree
   *  1 = NAVTREE_EXP_RESTRICT - Only expand the tree if the root
   *                             node of the tree is the same type 
   *                             of entity as the current entity.
   *  2 = NAVTREE_EXP_NEVER    - Never expand
   */

  /* Find root node */
  if (ent->navtree_expand == NAVTREE_EXP_RESTRICT)
  { 
    root = parent;
    while (root->parent)
    { root = root->parent; }
  }
  else
  { root = NULL; }

  /* Expand if necessary */
  if (ent->navtree_expand == NAVTREE_EXP_ALWAYS || (ent->navtree_expand == NAVTREE_EXP_RESTRICT && root && root->entaddr && root->entaddr->type == ent->ent_type))
  {
    for (i_list_move_head(ent->child_list); (child=i_list_restore(ent->child_list))!=NULL; i_list_move_next(ent->child_list))
    { 
      i_navtree_node *childnode;
      if (child->hidden) continue;
      childnode = i_navtree_generate (self, child, parent);
    }
    
    if (ent->ent_type == ENT_OBJECT)
    {
      /* Object Level */
      i_navtree_node *node;

      node = i_navtree_node_create ("triggerset", "Trigger Sets", RES_ADDR(self->hierarchy->dev_addr), i_entity_address_struct_static (ent), "triggerset_list", NULL, 0, parent);
      node = i_navtree_node_create ("record", "Recording", RES_ADDR(self->hierarchy->dev_addr), i_entity_address_struct_static (ent), "record_metlist", NULL, 0, parent);
    }
  }

  if (ent->ent_type == ENT_DEVICE)
  {
    /* Device level */
    char *devaddr_str;
    i_navtree_node *node;

    devaddr_str = i_resource_address_struct_to_string (RES_ADDR(self));

    node = i_navtree_node_create ("inventory", "Inventory", RES_ADDR(self->hierarchy->cust_addr), NULL, "inventory_devicemain", devaddr_str, strlen(devaddr_str), parent);
     // node = i_navtree_node_create ("ipregistry", "IP Registry", RES_ADDR(self->hierarchy->cust_addr), NULL, "ipregistry_devicemain", devaddr_str, strlen(devaddr_str), parent);

    free (devaddr_str);
  }

  return 0;
}


