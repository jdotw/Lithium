#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

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

#include "netscan.h"
#include "ipregistry.h"

int l_ipregistry_set_netmask (i_resource *self, char *ip_str, char *mask_str)
{
  /* Add entry to network table */
  i_hashtable *network_table;

  /* Get network table */
  network_table = l_ipregistry_table_network ();

  /* Get entry */
  l_ipregistry_entry *entry = l_ipregistry_get_ip_str (self, ip_str);
  if (!entry)
  { i_printf (1, "l_ipregistry_set_netmask failed to find entry for %s", ip_str); return -1; }

  /* Unset current */
  if (entry->network_str)
  {
    char *str;
    asprintf (&str, "%s/%s", entry->network_str, entry->mask_str); 
    i_hashtable_key *key = i_hashtable_create_key_string (str, network_table->size);
    free (str);
    if (key)
    {
      /* Retrieve network entry */
      l_ipregistry_network *network = i_hashtable_get (network_table, key);

      /* Find and remove entry from old network entry */
      if (network && i_list_search(network->entry_list, entry) == 0)
      { i_list_delete (network->entry_list); }

      /* Check for empty/unused network entry */
      if (network && network->entry_list->size == 0)
      { 
        i_hashtable_remove (network_table, key); 
        l_ipregistry_network_free (network);
      }
      i_hashtable_free_key (key);
    }

    /* Unset string */
    free (entry->network_str);
    entry->network_str = NULL;
  }
  if (entry->mask_str)
  {
    free (entry->mask_str);
    entry->mask_str = NULL;
  }

  /* Set new mask */
  if (mask_str)
  {
    entry->mask_str = strdup (mask_str);
    inet_aton (entry->mask_str, &entry->maskaddr);

    /* Set new network string/addr */
    entry->networkaddr.s_addr = entry->maskaddr.s_addr & entry->ipaddr.s_addr;
    asprintf (&entry->network_str, "%s", inet_ntoa(entry->networkaddr));

    /* Find new network entry */
    char *str;
    asprintf (&str, "%s/%s", entry->network_str, entry->mask_str);
    i_hashtable_key *key = i_hashtable_create_key_string (str, network_table->size);   /* Key by network and mask string */
    free (str);
    if (key)
    { 
      l_ipregistry_network *network = i_hashtable_get (network_table, key);
      if (!network)
      {
        /* New network */
        network = l_ipregistry_network_create (self, entry->network_str, entry->mask_str);
        i_hashtable_put (network_table, key, network);

        /* Check for non-/32 networks */
        if (strcmp(entry->mask_str, "255.255.255.255") != 0 && strcmp(entry->network_str, "169.254.0.0") != 0)
        {
          /* Perform initial scan */
          //l_netscan_scan (self, entry->network_str, entry->mask_str, NULL, NULL);
        }
      }
      i_hashtable_free_key (key);
    
      i_list_enqueue (network->entry_list, entry);
      i_list_sort (network->entry_list, l_ipregistry_listsort_ip_func);
    }
  }
//  else
//  {
//    /* No mask set, free entry */
//    l_ipregistry_entry_free (entry);
//  }

  return 0;
}

int l_ipregistry_set_dns (i_resource *self, l_ipregistry_entry *entry, char *dns_str)
{
  /* Clear old */
  if (entry->dns_str) free (entry->dns_str);
  entry->dns_str = NULL;

  /* Set new */
  if (dns_str)
  { entry->dns_str = strdup (dns_str); }
  
  return 0;
}
