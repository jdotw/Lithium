#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/timer.h>

#include "spawn.h"

extern i_list *global_spawn_schedule_list;

typedef struct 
{
  i_hashtable *res_table;
  int type;
  int ident_int;
  char *ident_str;
  char *mod_str;
  char *root;
  i_form *config_form;
  int no_remove;
} l_spawn_callback_data;

void l_spawn_schedule_free_callbackdata (void *dataptr)
{
  l_spawn_callback_data *data = dataptr;

  if (!data) return;

  if (data->ident_str) free (data->ident_str);
  if (data->mod_str) free (data->mod_str);
  if (data->root) free (data->root);
  if (data->config_form) i_form_free (data->config_form);

  free (data);
}

int l_spawn_scheduled_callback (i_resource *self, i_timer *timer, void *dataptr) 
{
  /* Called ever x seconds to spawn the next scheduled resource */

  i_resource *res;
  l_spawn_callback_data *data;
  
  if (!global_spawn_schedule_list) 
  { i_printf (1, "l_spawn_scheduled_callback failed because global_spawn_schedule=NULL"); return 0; }

  if (global_spawn_schedule_list->size < 1)
  {
    /* Nothing further scheduled */
    i_timer_remove (timer);
    i_list_free (global_spawn_schedule_list);
    global_spawn_schedule_list = NULL;
    return -11;
  }

  i_list_move_head (global_spawn_schedule_list);
  data = i_list_restore (global_spawn_schedule_list);
  if (!data)
  { i_printf (1, "l_spawn_scheduled_callback failed to restore callback data from list"); i_list_delete (global_spawn_schedule_list); return 0; }

  res = i_resource_local_create (self, RES_ADDR(self), data->res_table, data->type, data->ident_int, data->ident_str, data->mod_str, data->root, self->customer_id, NULL);
  if (!res)
  { i_printf (1, "l_spawn_scheduled_callback failed to create resource"); i_list_delete (global_spawn_schedule_list); return 0; }
  
  i_list_delete (global_spawn_schedule_list);
  return 0;
}

int l_spawn_scheduled (i_resource *self, i_hashtable *res_table, int type, int ident_int, char *ident_str, char *mod_str, char *root, i_form *config_form, int no_remove)
{
  int num;
  l_spawn_callback_data *data;

  data = (l_spawn_callback_data *) malloc (sizeof(l_spawn_callback_data));
  if (!data) { i_printf (1, "l_spawn_scheduled unable to malloc data"); return -1; }
  memset (data, 0, sizeof(l_spawn_callback_data));

  data->res_table = res_table;
  data->type = type;
  if (ident_str) data->ident_str = strdup (ident_str);
  if (mod_str) data->mod_str = strdup (mod_str);
  if (root) data->root = strdup (root);
  else data->root = strdup (self->root);
  if (config_form) data->config_form = i_form_duplicate (config_form);
  data->no_remove = no_remove;

  num = i_list_enqueue (global_spawn_schedule_list, data);

  return num;
}


