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

#include "induction.h"
#include "construct.h"
#include "hashtable.h"
#include "socket.h"
#include "form.h"
#include "auth.h"
#include "hierarchy.h"
#include "message.h"
#include "entity.h"

i_resource *global_self = NULL;

/*
 * General resource-related functions
 */

i_resource* i_resource_self ()
{ return global_self; }

/* Struct Manipulation */

void i_resource_free (void *resptr)
{
  i_resource *res = resptr;
  if (!res) return;

  if (res->node) free (res->node);
  if (res->plexus) free (res->plexus);
  if (res->ident_str) free (res->ident_str);

  if (res->core_socket) i_socket_free (res->core_socket);

  if (res->construct) i_construct_free (res->construct);
  if (res->config_form) i_form_free (res->config_form);
  if (res->module_name) free (res->module_name);
  
  if (res->owner) i_resource_free_address (res->owner);
  if (res->root) free (res->root);
  if (res->customer_id) free (res->customer_id);
  if (res->auth) i_authentication_free (res->auth);

  /* Free the hierarchy, during this process, the 
   * entity refered to be res->ent will be freed. 
   * If the hierarchy is not present, free res->ent
   */
  if (res->hierarchy) 
  { i_hierarchy_free (res->hierarchy); res->ent = NULL; }
  else if (res->ent) 
  {
    /* Recursively find parent (root) entity */
    while (res->ent->parent)
    { res->ent = res->ent->parent; }

    /* Free root parent entity */
    i_entity_free (res->ent);
  }

  if (res->state == RES_STATE_WAITING_REGISTER)
  { i_resource_local_incoming_list_remove (res); }

  if (res == global_self) global_self = NULL;     /* Just so global_self doesnt point to a free'd res */

  if (res->hosted)
  { i_hashtable_free (res->hosted); }

  free (res);
}

/* Struct->Data Conversion Functions */

char* i_resource_struct_to_data (i_resource *res, int *datasizeptr)             /* Take a resource struct and turn the _defining elements_ into a data block */
{
  /* Takes the i_resource struct and converts the following in order
   * into a data chunk :
   *
   * res->plexus    (null term str)
   * res->node      (null term str)
   * res->type      (int)
   * res->ident_int (int)
   * res->ident_str (null term str)
   * res->module_name   (null term str)
   *
   */

  int datasize;
  char *data;
  char *dataptr;

  if (!res || !datasizeptr || !res->module_name) { i_printf (1, "i_resource_struct_to_data insufficient params. failed"); return NULL; }
 
  /* Calculate data size */

  datasize = (2*(sizeof (int))) + strlen(res->module_name) + 4;
  if (res->plexus) datasize += strlen (res->plexus);
  if (res->node) datasize += strlen (res->node);
  if (res->ident_str) datasize += strlen (res->ident_str);
  
  /* Create the data */

  data = (char *) malloc (datasize);
  if (!data) { i_printf (1, "i_resource_struct_to_data unable to malloc data"); return NULL; }
  dataptr = data;

  if (res->plexus) { strcpy (dataptr, res->plexus); dataptr += strlen (res->plexus) + 1; }
  else { *dataptr = '\0'; dataptr++; }
  if (res->node) { strcpy (dataptr, res->node); dataptr += strlen (res->node) + 1; }
  else { *dataptr = '\0'; dataptr++; }
  
  memcpy (dataptr, &res->type, sizeof(int));
  dataptr += sizeof (int);
  memcpy (dataptr, &res->ident_int, sizeof(int));
  dataptr += sizeof (int);

  if (res->ident_str) { strcpy (dataptr, res->ident_str); dataptr += strlen (res->ident_str) + 1; }
  else { *dataptr = '\0'; dataptr++; }

  strcpy (dataptr, res->module_name); 
  dataptr += strlen (res->module_name) + 1;

  memcpy (datasizeptr, &datasize, sizeof(int));

  return data;
}

i_resource* i_resource_data_to_struct (char *data, unsigned int datasize)               /* Take a data block and form a resource struct */
{
  /* 
   * Takes a data chunk formed by i_resource_struct_to_data and 
   * turns it back into a resource struct 
   * 
   */

  /* FIX: Really should be redone to be segfault safe, and the above too */
  
  i_resource *res;
  char *dataptr;

  if (datasize < ((2 * sizeof(int)) + 4)) { i_printf (1, "i_resource_data_to_struct failed because datasize is below minimum size"); return NULL; }

  res = (i_resource *) malloc (sizeof(i_resource));
  if (!res) { i_printf (1, "i_resource_data_to_struct unable to malloc res"); return NULL; }
  memset (res, 0, sizeof(i_resource));

  dataptr = data;

  if (*dataptr == '\0') { res->plexus = NULL; dataptr++; }
  else { res->plexus = strdup (dataptr); dataptr += strlen (res->plexus) + 1; }
  if (*dataptr == '\0') { res->node = NULL; dataptr++; }
  else { res->node = strdup (dataptr); dataptr += strlen (res->node) + 1; }
  
  memcpy (&res->type, dataptr, sizeof(int));
  dataptr += sizeof(int);
  memcpy (&res->ident_int, dataptr, sizeof(int));
  dataptr += sizeof(int);

  if (*dataptr == '\0') { res->ident_str = NULL; dataptr++; }
  else { res->ident_str = strdup (dataptr); dataptr += strlen (res->ident_str) + 1; }

  res->module_name = strdup (dataptr);
  dataptr += strlen (res->module_name) + 1;

  return res;
}

/* Hosted (Resource-in-a-Resource) Functions */

i_resource *i_resource_hosted_get (i_resource *self, i_resource_address *addr)
{
  /* Return the hosted resource structed for
   * the given resource address
   */

  if (!self->hosted) return NULL;

  i_hashtable_key *key = i_hashtable_key_resource (addr, self->hosted->size);
  i_resource *res = i_hashtable_get (self->hosted, key);
  i_hashtable_key_free (key);

  return res;
}

/* Misc functions */

long i_resource_send_ready (i_resource *self)
{
  /* Send a MSG_RES_READY resource to the parent */

  long msgid;
  
  if (!self->owner)
  { i_printf (1, "i_resource_send_ready called with NULL res->owner. failed"); return -1; }

  msgid = i_message_send (self, MSG_RES_READY, NULL, 0, self->owner, MSG_FLAG_REQ, 0);
  if (msgid == -1)
  { i_printf (1, "i_resource_send_ready failed to send MSG_RES_READY"); return -1; }

  return msgid;
}

char* i_resource_state_str (i_resource *res)
{
  switch (res->state)
  {
    case 0: return "Unknown";
    case 1: return "Running";
    case 2: return "Sleeping";
    case 3: return "Waiting Registration";
    default: return "Other";
  }

  return NULL;
}

char* i_resource_typestr (int type)
{
  switch (type)
  {
    case RES_CORE: return "Core";
    case RES_EXEC: return "Exec";
    case RES_CLIENT_HANDLER_CORE: return "Client Handler Core";
    case RES_CLIENT_HANDLER_CHILD: return "Client Handler Child";
    case RES_ADMIN: return "Administration";
    case RES_CUSTOMER: return "Customer";
    case RES_SITE: return "Site";
    case RES_DEVICE: return "Device";
    case RES_SMS: return "SMS";
    default: return "Unknown";
  }

  return NULL;
}
                  
