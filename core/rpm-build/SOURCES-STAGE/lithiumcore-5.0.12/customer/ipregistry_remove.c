#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
    
#include <induction.h>
#include <induction/hashtable.h>
#include <induction/ip.h>
#include <induction/message.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/socket.h>
#include <induction/msgproc.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/ipregistry.h>
      
#include "ipregistry.h"
        
int l_ipregistry_remove (i_resource *self, char *ip_str)
{       
  /* 
   * Remove an IP address entry from the registry 
   *
   * NOTE: This DOES NOT remove device table entries
   *
   * This is designed to only ever be called if an IP
   * entry is to be discarded... i.e has not been visible
   * for a long time and has no devices/entities bound
   */
  i_hashtable_key *key;

  /* Get entry */
  l_ipregistry_entry *entry = l_ipregistry_get_ip_str (self, ip_str);
  if (!entry)
  { i_printf (1, "l_ipregistry_remove failed, entry not found for %s", ip_str); return -1; }
  
  /* Set NULL netmask to remove from network table */ 
  l_ipregistry_set_netmask (self, ip_str, NULL);

  /* Remove from ip_table */
  struct in_addr ipaddr;
  i_hashtable *ip_table = l_ipregistry_table_ip ();
  inet_aton (ip_str, &ipaddr);
  key = i_hashtable_key_ip (ipaddr.s_addr, 0, 0, 0, ip_table->size);
  if (key)
  {
    i_hashtable_remove (ip_table, key);
  }
  i_hashtable_free_key (key);

  return 0;
}
