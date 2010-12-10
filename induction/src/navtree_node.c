#include <stdlib.h>
#include <string.h>

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
 * Node manipulation
 */

i_navtree_node* i_navtree_node_create (char *name_str, char *desc_str, i_resource_address *resaddr, i_entity_address *entaddr, char *form_str, char *passdata, int passdata_size, i_navtree_node *parent)
{
  /* This function creates a new navtree
   * node using the specified name_str, 
   * desc_str, resource, entity and form_str. 
   * If a parent node is specified, the node is
   * enqueued into the parents child_list
   */
  int num;
  i_navtree_node *node;

  /* Malloc node */
  node = (i_navtree_node *) malloc (sizeof(i_navtree_node));
  if (!node)
  { i_printf (1, "i_navtree_node_create failed to malloc i_navtree_node struct"); return NULL; }
  memset (node, 0, sizeof(i_navtree_node));

  /* Configure node */
  if (name_str) node->name_str = strdup (name_str);
  if (desc_str) node->desc_str = strdup (desc_str);
  if (resaddr) node->resaddr = i_resource_address_duplicate (resaddr);
  if (entaddr) node->entaddr = i_entity_address_duplicate (entaddr);
  if (form_str) node->form_str = strdup (form_str);
  else node->form_str = strdup ("main");
  if (passdata && passdata_size > 0)
  {
    node->passdata = malloc (passdata_size);
    if (!node->passdata)
    { 
      i_printf (1, "i_navtree_node_create failed to malloc node->passdata %i bytes", passdata_size); 
      i_navtree_node_free (node); 
      return NULL; 
    }
    memcpy (node->passdata, passdata, passdata_size);
    node->passdata_size = passdata_size;
  }

  /* Create child_list */
  node->child_list = i_list_create ();
  if (!node->child_list)
  {
    i_printf (1, "i_navtree_node_create failed to create node->child_list");
    i_navtree_node_free (node);
    return NULL;
  }
  i_list_set_destructor (node->child_list, i_navtree_node_free); 

  /* Enqueue to parent */
  if (parent) 
  { 
    num = i_list_enqueue (parent->child_list, node); 
    if (num != 0)
    { 
      i_printf (1, "i_navtree_node_create failed to enqueue node %s into parent %s", name_str, parent->name_str); 
      i_navtree_node_free (node);
      return NULL;
    }
    node->parent = parent;
  }

  return node;
}

void i_navtree_node_free (void *nodeptr)
{
  i_navtree_node *node = nodeptr;

  if (!node) return;

  if (node->child_list) i_list_free (node->child_list);
  if (node->name_str) free (node->name_str);
  if (node->desc_str) free (node->desc_str);
  if (node->resaddr) i_resource_address_free (node->resaddr);
  if (node->entaddr) i_entity_address_free (node->entaddr);
  if (node->form_str) free (node->form_str); 
  if (node->passdata) free (node->passdata); 

  free (node);
}

/* @} */
