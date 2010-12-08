#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <induction.h>

i_resource_address* a_resource_address_string_to_struct (char *str)
{
  /* The i_resource_address_string_to_struct attempts to set node and plexus 
   * to the currently configured values. This doesnt work for the actuator as 
   * it doesnt and shouldnt have access to those config files. Hence we create 
   * a blank address from a "::::" string and if the node/plexus matches 
   * we free/NULL it
   */

  i_resource_address *addr;
  i_resource_address *local_address;

  addr = i_resource_address_string_to_struct (str);
  if (!addr) return NULL;
  
  local_address = i_resource_address_string_to_struct ("::::");
  if (!local_address)
  { 
    printf ("<BR><BR>a_resource_address_string_to_struct failed to create local_address"); 
    i_resource_free_address (addr); 
    return NULL; 
  }
                  
  if (!strcmp(addr->plexus, local_address->plexus)) { free (addr->plexus); addr->plexus = NULL; }       
  if (!strcmp(addr->node, local_address->node)) { free (addr->node); addr->node = NULL; }

  i_resource_free_address (local_address);

  return addr;
}                          
