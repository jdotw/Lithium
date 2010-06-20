#include <stdlib.h>

#include "induction.h"
#include "list.h"
#include "cement.h"
#include "entity.h"
#include "navtree.h"

/** \addtogroup form_nav Navigation Menu
 * @ingroup form
 * @{
 */

/*
 * Navtree - Generic entity-based navigation system 
 *
 * Each entity has a navtree_func assigned to it which,
 * when called, will return a root node for its tree of
 * navigation items (i.e links to forms/entities) which is 
 * used to create a hierarchical navigation system.
 *
 */

static int (*static_default_func) () = NULL;

i_navtree_node* i_navtree_generate (i_resource *self, i_entity *ent, i_navtree_node *parent)
{
  /* This function creates a new root node
   * that will be attached to the parent
   * node if specified. The new root node
   * is then passed to the entities navtree
   * func
   */

  int num;
  i_navtree_node *root;
  i_navtree_node *node;
  i_list *hier_list;
  i_entity *parent_ent;

  /* Create root node */
  root = i_navtree_node_create (ent->name_str, ent->desc_str, RES_ADDR(self), ENT_ADDR(ent), NULL, NULL, 0, parent);
  if (!root)
  { 
    i_printf (1, "i_navtree_generate failed to create root node for %s %s", 
      i_entity_typestr(ent->ent_type), ent->name_str);
  }
  root->opstate = ent->opstate;
  root->adminstate = ent->adminstate;

  /* Create parent hierarchy */
  node = root;
  if (!parent)
  {
    hier_list = i_list_create ();
    parent_ent = ent;
    while (parent_ent)
    { 
      i_list_push (hier_list, parent_ent); parent_ent = parent_ent->parent; 
    }
    for (i_list_move_head(hier_list); (parent_ent=i_list_restore(hier_list))!=NULL; i_list_move_next(hier_list))
    { node = i_navtree_node_create (parent_ent->name_str, parent_ent->desc_str, RES_ADDR(parent_ent->resaddr), ENT_ADDR(parent_ent), NULL, NULL, 0, node); }
    i_list_free (hier_list);
  }

  /* Call ent->navtree_func */
  if (ent->navtree_func)
  { 
    num = ent->navtree_func (self, ent, node);
    if (num != 0)
    {
      i_printf (1, "i_navtree_generate failed to call ent->navtree_func for %s %s",
        i_entity_typestr(ent->ent_type), ent->name_str);
      i_navtree_node_free (root);
      return NULL;
    }
  }
  else if (static_default_func)
  {
    num = static_default_func (self, ent, node);
    if (num != 0)
    {
      i_printf (1, "i_navtree_generate failed to call static_default_func for %s %s",
        i_entity_typestr(ent->ent_type), ent->name_str);
      i_navtree_node_free (root);
      return NULL;
    }
  }

  return root;
}

/* Default Function Manipulation */

int i_navtree_defaultfunc_set (int (*defaultfunc) ())
{ static_default_func = defaultfunc; return 0; }

/* @} */
