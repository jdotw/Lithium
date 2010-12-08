#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <dlfcn.h>
#include <libxml/parser.h>

#include "induction.h"
#include "xml.h"

extern i_resource *global_self;

/* i_resource_address related functions */

void i_resource_address_free (void *addrptr)
{
  i_resource_address *addr = (i_resource_address *) addrptr;
  
  if (!addr) return;

  if (addr->plexus) free (addr->plexus);
  if (addr->node) free (addr->node);
  if (addr->ident_str) free (addr->ident_str);

  free (addr);
}

void i_resource_free_address (void *addrptr)  /* Legacy */
{ i_resource_address_free (addrptr); }

i_resource_address* i_resource_address_create ()
{
  i_resource_address *addr;

  addr = (i_resource_address *) malloc (sizeof(i_resource_address));
  if (!addr)
  { i_printf (1, "i_resource_address_create unable to malloc addr"); return NULL; }
  memset (addr, 0, sizeof(i_resource_address));

  return addr;
}

char* i_resource_address_struct_to_string (i_resource_address *addr)
{
	char *data;
	char *node;
	char *plexus;
	char *ident_str;

    if (!addr) return NULL;
    
	if (!addr->plexus) plexus = "";
	else plexus = addr->plexus;
	if (!addr->node) node = "";
	else node = addr->node;
	if (!addr->ident_str) ident_str = "";
	else ident_str = addr->ident_str;

	asprintf (&data, "%s:%s:%i:%i:%s", plexus, node, addr->type, addr->ident_int, ident_str);

	return data;
}

i_resource_address* i_resource_address_string_to_struct (char *str)
{
  char *working_str;
  char *plexus;
  char *node;
  char *type;
  char *ident_int;
  char *ident_str;
  i_resource_address *addr;

  if (!str) return NULL;

  /* Create working copy */
  working_str = strdup (str);			/* So that we dont mess with the original */

  /* Retrieve identifiers */
  
  plexus = working_str;

  node = strchr (plexus, ':');
  if (!node) { i_printf (1, "i_resource_address_string_to_struct unable to locate node in string"); free (working_str); return NULL; }
  *node = '\0';
  node++;

  type = strchr (node, ':');
  if (!type) { i_printf (1, "i_resource_address_string_to_struct unable to locate type in string"); free (working_str); return NULL; }
  *type = '\0';
  type++;

  ident_int = strchr (type, ':');
  if (!ident_int) { i_printf (1, "i_resource_address_string_to_struct unable to locate ident_int in string"); free (working_str); return NULL; }
  *ident_int = '\0';
  ident_int++;

  ident_str = strchr (ident_int, ':');
  if (!ident_str) { i_printf (1, "i_resource_address_string_to_struct unable to locate ident_str in string"); free (working_str); return NULL; }
  *ident_str = '\0';
  ident_str++;	

  /* Compile address struct */
  addr = i_resource_address_create ();
  if (!addr) 
  { 
    i_printf (1, "i_resource_address_string_to_struct failed to malloc addr");
    free (working_str);
    return NULL; 
  }

  if (*plexus == '\0')
  {
    if (global_self && global_self->plexus) addr->plexus = strdup(global_self->plexus);
    else addr->plexus = NULL;
  }
  else addr->plexus = strdup (plexus);

  if (*node == '\0')
  {
    if (global_self && global_self->node) addr->node = strdup(global_self->node);
    else addr->node = NULL;
  }
  else addr->node = strdup (node);

  if (*type == '\0') addr->type = 0;
  else addr->type = atoi (type);

  if (*ident_int == '\0') addr->ident_int = 0;
  else addr->ident_int = atoi (ident_int);

  if (*ident_str == '\0') addr->ident_str = NULL;
  else addr->ident_str = strdup (ident_str);

  free (working_str); 
        
  return addr;
}

int i_resource_address_compare (i_resource_address *first, i_resource_address *second)
{
  /* Compares the two resource addresses by converting them 
   * to strings and running a strcmp
   *
   * Returns -1 on error
   * Returns 0 on match
   * Returns 1 on no match
   */

  int num;
  char *first_str;
  char *second_str;

  if (!first || !second) return -1;
  
  first_str = i_resource_address_struct_to_string (first);
  if (!first_str) 
  { i_printf (1, "i_resource_address_compare unable to convert first address to a string"); return -1; }
  second_str = i_resource_address_struct_to_string (second);
  if (!second_str)
  { i_printf (1, "i_resource_address_compare unable to convert second address to a string"); return -1; }

  num = strcmp (first_str, second_str);
  free (first_str);
  free (second_str);

  if (num != 0) return 1;

  return 0;
}

i_resource_address* i_resource_address_duplicate (i_resource_address *original)
{
	i_resource_address *dup;

    if (!original) { i_printf (1, "i_resource_address_duplicate called with NULL original"); return NULL; }
    
	dup = i_resource_address_create ();
	if (!dup) { i_printf (1, "i_resource_address_duplicate unable to malloc dup");
			return NULL; }

	if (original->plexus) dup->plexus = strdup (original->plexus);
	if (original->node) dup->node = strdup (original->node);
	dup->type = original->type;
	dup->ident_int = original->ident_int;
	if (original->ident_str) dup->ident_str = strdup (original->ident_str);

	return dup;
}

char* i_resource_address_entity_string (i_resource_address *addr)
{
  char *str;
  i_resource_info *info;
  
  switch (addr->type)
  {
    case RES_CUSTOMER:
      str = strdup ("Customer");
      break;
    case RES_SITE:
      str = strdup ("Site");
      break;
    case RES_DEVICE:
      str = strdup ("Device");
      break;
    default:
      info = i_resource_info_local (addr->type);
      if (info && info->type_str)
      { str = strdup (info->type_str); i_resource_info_free (info); }
      else
      { str = strdup ("Unknown Resource Type"); }
  }

  return str;
}

/* XML Function */

xmlNodePtr i_resource_address_xml (i_resource_address *resaddr)
{
  char *str;
  xmlNodePtr addr_node;

  /* Create resource address node */
  addr_node = xmlNewNode (NULL, BAD_CAST "resource_address");

  xmlNewChild (addr_node, NULL, BAD_CAST "cluster", BAD_CAST resaddr->plexus);
  xmlNewChild (addr_node, NULL, BAD_CAST "node", BAD_CAST resaddr->node);
  asprintf (&str, "%i", resaddr->type);
  xmlNewChild (addr_node, NULL, BAD_CAST "type_int", BAD_CAST str);
  free (str);
//  xmlNewChild (addr_node, NULL, BAD_CAST "res_type", BAD_CAST i_resource_typestr(resaddr->type));
  xmlNewChild (addr_node, NULL, BAD_CAST "ident_str", BAD_CAST resaddr->ident_str);
  asprintf (&str, "%i", resaddr->ident_int);
  xmlNewChild (addr_node, NULL, BAD_CAST "ident_int", BAD_CAST str);
  free (str);

  return addr_node;
}
