#include <stdio.h>
#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/navtree.h>
#include <induction/postgresql.h>
#include <induction/customer.h>

#include "dhcp.h"
#include "navtree.h"

/* Customer-level navtree */

int l_navtree_func_cust (i_resource *self, i_customer *cust, i_navtree_node *root)
{
  i_navtree_node *node;
  i_site *site;

  node = i_navtree_node_create ("incident", "Incidents", NULL, NULL, "incident_list", NULL, 0, root);
  node = i_navtree_node_create ("users", "Users", NULL, NULL, "user_list", NULL, 0, root);
//  node = i_navtree_node_create ("inventory", "Inventory", NULL, NULL, "inventory_vendorlist", NULL, 0, root);
//  node = i_navtree_node_create ("ipregistry", "IP Registry", NULL, NULL, "ipregistry_networklist", NULL, 0, root);

  node = i_navtree_node_create ("sitelist", "Sites", NULL, NULL, "site_list", NULL, 0, root);
  for (i_list_move_head(cust->site_list); (site=i_list_restore(cust->site_list))!=NULL; i_list_move_next(cust->site_list))
  { 
    if (site->licensed)
    {
      i_navtree_node *sitenode;
      sitenode = i_navtree_node_create (site->name_str, site->desc_str, NULL, ENT_ADDR(site), "device_list", NULL, 0, node);
      sitenode->opstate = site->opstate;
      sitenode->adminstate = site->adminstate;
    }
  }
  
  node = i_navtree_node_create ("licenses", "Licenses", NULL, NULL, "lic_main", NULL, 0, root);
  node = i_navtree_node_create ("restart", "Restart", NULL, NULL, "restart", NULL, 0, root);

  return 0;
}

/* Site-level navtree */

int l_navtree_func_site (i_resource *self, i_site *site, i_navtree_node *root)
{
  i_device *dev;
  i_navtree_node *node;

  node = i_navtree_node_create ("incident", "Incidents", NULL, ENT_ADDR(site), "incident_list", NULL, 0, root);
  node = i_navtree_node_create ("users", "Users", NULL, ENT_ADDR(site), "user_list", NULL, 0, root);
  node = i_navtree_node_create ("devicelist", "Devices", NULL, ENT_ADDR(site), "device_list", NULL, 0, root);
  for (i_list_move_head(site->dev_list); (dev=i_list_restore(site->dev_list))!=NULL; i_list_move_next(site->dev_list))
  { 
    i_navtree_node *devnode;
    if (dev->licensed && dev->resaddr)
    { 
      devnode = i_navtree_node_create (dev->name_str, dev->desc_str, RES_ADDR(dev->resaddr), NULL, NULL, NULL, 0, node); 
      devnode->opstate = dev->opstate;
      devnode->adminstate = dev->adminstate;
    }
  }
  
  return 0;
}
