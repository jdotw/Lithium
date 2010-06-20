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

/*
 * Add new entry 
 */

l_ipregistry_entry* l_ipregistry_add (i_resource *self, char *ip_str, char *mask_str)
{
  /* Add a new entry to the ipregistry tables */

  i_hashtable_key *key;
  l_ipregistry_entry *entry;

  /* Retrieve tables */
  i_hashtable *ip_table = l_ipregistry_table_ip ();

  /* Check table */
  if (!ip_table)
  { i_printf (1, "l_ipregistry_add failed to add entry, ip_table not present"); return NULL; }

  /* 127.0.0.1/0.0.0.0 Check and discard*/
  if (!strcmp(ip_str, "127.0.0.1") || !strcmp(ip_str, "0.0.0.0"))
  { return NULL; }     /* ignore/discard a 0.0.0.0 or 127.0.0.1 address */

  /* Check mask, use /32 as default */
  if (!mask_str) 
  { mask_str = "255.255.255.255"; }

  /* Create entry */
  entry = l_ipregistry_entry_create ();
  entry->ip_str = strdup (ip_str);
  inet_aton (ip_str, &entry->ipaddr);
  entry->mask_str = strdup (mask_str);
  inet_aton (mask_str, &entry->maskaddr);

  /* 
   * Add entry to IP table.
   *
   * If by chance/error a previous entry is found, 
   * the new entry will NOT be added based on the assumption
   * that the existing entry will be 'wiser' 
   */

  key = i_hashtable_key_ip (entry->ipaddr.s_addr, 0, 0, 0, ip_table->size);
  if (!key)
  { i_printf (1, "l_ipregistry_add failed to create key for ip_table"); return NULL; }

  l_ipregistry_entry *existing_entry;
  existing_entry = i_hashtable_get (ip_table, key);
  if (existing_entry)
  { 
    i_printf (1, "l_ipregistry_add warning; entry already exists for %s/%s", ip_str, mask_str); 
    l_ipregistry_entry_free (entry);
    entry = existing_entry;
  }
  else
  { i_hashtable_put (ip_table, key, entry); }
  i_hashtable_free_key (key);

  /*
   * Add to network table 
   */

  l_ipregistry_set_netmask (self, ip_str, mask_str);

  return entry;
}

