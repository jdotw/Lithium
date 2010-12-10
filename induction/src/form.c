#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>

#include "induction.h"
#include "entity.h"
#include "form.h"
#include "callback.h"
#include "list.h"
#include "data.h"
#include "message.h"
#include "respond.h"

/** \addtogroup form Web Forms
 * @{
 */

extern i_resource *global_self;

/* Forms, inductions interface independant data display / gather method */

void i_form_free_item_option (void *data)
{
	i_form_item_option *option = (i_form_item_option *) data;

	if (!data) return;

	if (option->data) free (option->data);
	free (option);	
}

void i_form_free_option (void *data)
{
  i_form_option *opt = (i_form_option *) data;

  if (!data) return;

  if (opt->data) free (opt->data);
  free (opt);
}

void i_form_free_item (void *data)
{
	i_form_item *item = (i_form_item *) data;

	if (!data) return;

    if (item->name) free (item->name);
	if (item->options) i_list_free (item->options);
	free (item);
}

void i_form_free (void *formptr)
{
  i_form *form = formptr;

  if (!form) return;

  if (form->items) i_list_free (form->items);
  if (form->options) i_list_free (form->options);

  free (form);	 
}

i_form* i_form_create (i_form_reqdata *reqdata, int submit)
{
  i_form *form;

  form = (i_form *) malloc (sizeof(i_form));
  if (!form) { i_printf (1, "i_form_create unable to malloc form"); return NULL; }
  memset (form, 0, sizeof(i_form));

  form->reqdata = reqdata;
  form->submit = submit;
	
  form->items = i_list_create ();
  if (!form->items) { i_printf (1, "i_form_create unable to create items list"); i_form_free (form); return NULL; }
  i_list_set_destructor (form->items, i_form_free_item);                

  form->options = i_list_create ();
  if (!form->options) { i_printf (1, "i_form_create unable to create options list"); i_form_free (form); return NULL; }
  i_list_set_destructor (form->options, i_form_free_option);

  return form;
}

int i_form_set_title (i_form *form, char *title)
{
  i_form_option *opt;

  if (!form || !title) return -1;

  i_form_option_remove (form, FORM_OPTION_TITLE);     /* Remove old title if any */
  
  opt = i_form_option_add (form, FORM_OPTION_TITLE, title, strlen(title)+1);
  if (!opt) { i_printf (1, "i_form_set_title failed to set title"); return -1; }

  return 0;
}

int i_form_set_submit (i_form *form, int submit)
{
  if (!form) return -1;

  form->submit = submit;

  return 0;
}

i_form_option* i_form_option_create (int type, void *data, int datasize)
{
  i_form_option *opt;

  opt = (i_form_option *) malloc (sizeof(i_form_option));
  if (!opt) { i_printf (1, "i_form_option_create unable to malloc opt"); return NULL; }
  memset (opt, 0, sizeof(i_form_option));

  opt->type = type;
  opt->datasize = datasize;
  if (datasize > 0 && data) 
  { 
    opt->data = (char *) malloc (datasize); 
    if (!opt->data) { i_printf (1, "i_form_option_create unable to malloc opt->data"); i_form_free_option (opt); return NULL; }
    memcpy (opt->data, data, datasize); 
  }

  return opt;
}

i_form_option* i_form_option_add (i_form *form, int type, char *data, int datasize)
{
  i_form_option *opt;

  opt = i_form_option_create (type, data, datasize);
  if (!opt) { i_printf (1, "i_form_option_add unable to create opt"); return NULL; }

  i_list_enqueue (form->options, opt);

  return opt;
}

i_form_option* i_form_option_find (i_form *form, int type)
{
  i_form_option *opt;

  for (i_list_move_head(form->options); (opt = i_list_restore(form->options)) != NULL; i_list_move_next(form->options))
  {
    if (opt->type == type) return opt;
  }

  return NULL;
}

int i_form_option_remove (i_form *form, int type)
{
  int delete_count = 0;
  i_form_option *opt;
  
  for (i_list_move_head(form->options); (opt = i_list_restore(form->options)) != NULL; i_list_move_next(form->options))
  {
    if (opt->type == type)
    { i_list_delete (form->options); delete_count++; }
  }

  if (delete_count < 1) return -1; 

  return 0;
}

i_form_item* i_form_create_item (int type, char *name)
{
	i_form_item *item;

	if (!name) return NULL;

	item = (i_form_item *) malloc (sizeof(i_form_item));
	if (!item) { i_printf (1, "i_form_create_item unable to malloc item"); 
			return NULL; }
	memset (item, 0, sizeof(i_form_item));

	item->type = type;
	item->name = strdup (name);
    item->options = i_list_create ();
    i_list_set_destructor (item->options, i_form_free_item_option);

	return item;
}

i_form_item_option* i_form_create_item_option (int type, int i, int x, int y, void *data, int datasize)
{
	i_form_item_option *option;
    
	option = (i_form_item_option *) malloc (sizeof(i_form_item_option));
	if (!option) { i_printf (1, "i_form_create_item_option unable to malloc option");
			return NULL; }
	memset (option, 0, sizeof(i_form_item_option));

	option->type = type;
	option->i = i;
	option->x = x;
	option->y = y;
	option->datasize = datasize;

	if (data && datasize > 0)
	{
		option->data = malloc (datasize);
		if (!option->data) { i_printf (1, "i_form_create_item_option unable to malloc data");
					i_form_free_item_option (option); return NULL; }
		memcpy (option->data, data, datasize);
	}

	return option;
}

i_form* i_form_add_item (i_form *form, i_form_item *item)			/* Adds the item to a form. Returns the form ptr on success, NULL on fail */
{
	if (!form || !item) return NULL;

    i_list_enqueue (form->items, item);
    item->form = form;

	return form;
}

i_form_item* i_form_find_item (i_form *form, char *name)
{
  i_form_item *item;

  if (!form || !name) return NULL;

  for (i_list_move_head(form->items); (item = i_list_restore(form->items)) != NULL; i_list_move_next(form->items))
  {
    /* Loop through each item */

    if (!strcmp(item->name, name))
    {
      return item;
    }

  }

  return NULL;                    /* Not found */            
}

i_form* i_form_delete_item (i_form *form, char *name)				/* Deletes first item called 'name'. Returns the form on success, NULL on fail */
{
	i_form_item *item;

	for (i_list_move_head(form->items); (item = i_list_restore(form->items)) != NULL; i_list_move_next(form->items))
	{
		/* Loop through each item */

		if (!strcmp(item->name, name)) 
		{
			i_list_delete (form->items);	/* Match */
			return form;
		}
	}

	return NULL;					/* Not found */
}

i_form_item_option* i_form_item_add_option (i_form_item *item, int type, int i, int x, int y, void *data, int datasize)
{
	i_form_item_option *option;

	if (!item || !item->options) return NULL;

	option = i_form_create_item_option (type, i, x, y, data, datasize);
	if (!option) { i_printf (1, "i_form_item_add_option unable to create option"); 
			return NULL; }

	i_list_enqueue (item->options, option);

	return option;	
}

void i_form_item_add_link (i_form_item *item, int i, int x, int y, i_resource_address *resaddr, i_entity_address *entaddr, char *form_name, time_t ref_sec, char *passdata, int passdata_size)
{
  char *refsecstr;
  char *resaddrstr = NULL;
  char *entaddrstr = NULL;

  /* Convert strings */
  if (resaddr) resaddrstr = i_resource_address_struct_to_string (resaddr);
  else resaddrstr = i_resource_address_struct_to_string (RES_ADDR(global_self));
  if (entaddr) entaddrstr = i_entity_address_string (NULL, entaddr);

  /* Add link options */
  if (!form_name)
  { form_name = "main"; }
  i_form_item_add_option (item, ITEM_OPTION_LINK_NAME, i, x, y, form_name, strlen(form_name)+1);
  asprintf (&refsecstr, "%li", ref_sec);
  i_form_item_add_option (item, ITEM_OPTION_LINK_REFSEC, i, x, y, refsecstr, strlen(refsecstr)+1);
  free (refsecstr);
  if (resaddrstr) 
  { i_form_item_add_option (item, ITEM_OPTION_LINK_RESADDR, i, x, y, resaddrstr, strlen(resaddrstr)+1); free (resaddrstr); }
  if (entaddrstr) 
  { i_form_item_add_option (item, ITEM_OPTION_LINK_ENTADDR, i, x, y, entaddrstr, strlen(entaddrstr)+1); free (entaddrstr); }
  if (passdata && passdata_size > 0) 
  { i_form_item_add_option (item, ITEM_OPTION_LINK_PASSDATA, i, x, y, passdata, passdata_size); }
}

i_form_item_option* i_form_item_append_option (i_form_item *item, int type, int x, int y, void *data, int datasize)
{
  int highest_i = 0;
  i_form_item_option *opt;

  if (!item) return NULL;

  for (i_list_move_head(item->options); (opt=i_list_restore(item->options)) != NULL; i_list_move_next(item->options))
  {
    if (opt->type == type) 
      if (opt->i > highest_i) highest_i = opt->i;
  }

  return i_form_item_add_option (item, type, highest_i+1, x, y, data, datasize);  
}

i_form_item_option* i_form_item_find_option (i_form_item *item, int type, int i, int x, int y)			/* Find an option */
{
	i_form_item_option *opt;

	for (i_list_move_head(item->options); (opt=i_list_restore(item->options)) != NULL; i_list_move_next(item->options))
	{
		if (opt->type == type && opt->i == i && opt->x == x && opt->y == y) return opt; 		/* Found */
	}

	return NULL;					/* Not found */
}

int i_form_item_delete_option (i_form_item *item, int type, int i, int x, int y)
{
	i_form_item_option *opt;

	for (i_list_move_head(item->options); (opt=i_list_restore(item->options)) != NULL; i_list_move_next(item->options))
	{
		if (opt->type == type && opt->i == i && opt->x == x && opt->y == y) { i_list_delete (item->options); return 0; } /* Found & removed */
	}

	return -1;
}

int i_form_item_ready (i_resource *self, i_form_item *item)
{
  /* Decrement the form->notready_count counter
   * and sets the items state to READY. If the forms
   * notready_count is 0 after the counter is 
   * decremented, and the forms state is DELIVERABLE
   * then call i_form_deliver
   */

  /* Decrement counter */
  if (item->state != ITEMSTATE_READY)
  { item->form->notready_count--; }

  /* Set state */
  item->state = ITEMSTATE_READY;

  /* Check notready count */
  if (item->form->notready_count == 0 && item->form->state == FORMSTATE_DELIVERABLE)
  {
    /* Deliver the form */
    return i_form_deliver (self, item->form->reqdata);
  }

  return 0;
}

int i_form_item_notready (i_resource *self, i_form_item *item)
{
  /* Increment the form->notready_count counter
   * and sets the items state to NOTREADY
   */
  if (item->state != ITEMSTATE_NOTREADY)
  { item->form->notready_count++; }
  item->state = ITEMSTATE_NOTREADY;

  return 0;
}

/* Form struct to data conversions */

char* i_form_form_to_data (i_form *form, int *datasizeptr)				/* Take a form and 'compile' it into a data block */
{
  /* Data format is as follows
   *
   * int submit
   * 
   * unsigned long option_count
   * ---(repeated for each option)---
   * int opt_type
   * int opt_datasize
   * void* opt_data
   * ---(end repeated section)---
   *
   * unsigned long item_count
   * ---(repeated for each item)---
   * int item_datasize
   * void *item_data
   * ---(end repeated section)---
   * 
   */

  int datasize;
  int olddatasize;
  char *data;
  char *dataptr;
  char *x;
  i_form_item *item;
  i_form_option *opt;

  if (!datasizeptr) return NULL;
  memset (datasizeptr, 0, sizeof(int));
  if (!form || !form->options || !form->items) return NULL;

  datasize = sizeof(int) + sizeof(long);

  data = (char *) malloc (datasize);
  if (!data) { i_printf (1, "i_form_form_to_data unable to malloc data (%i bytes)", datasize); return NULL; }
  dataptr = data;

  dataptr = i_data_add_int (data, dataptr, datasize, &form->submit);
  if (!dataptr) 
  { i_printf (1, "i_form_form_to_data unable to copy form->submit into data"); free (data); return NULL; }

  dataptr = i_data_add_long (data, dataptr, datasize, (long *) &form->options->size);
  if (!dataptr) 
  { i_printf (1, "i_form_form_to_data unable to copy form->options->size into data"); free (data); return NULL; }

  for (i_list_move_head(form->options); (opt=i_list_restore(form->options)) != NULL; i_list_move_next(form->options))
  {
    olddatasize = datasize;
    datasize += (2 * sizeof(int)) + opt->datasize;
    x = realloc (data, datasize);
    if (x) data = x;
    else 
    { 
      i_printf (1, "i_form_form_to_data unable to re-alloc data (%i bytes - current opt size is %i", datasize, opt->datasize); 
      free (data); 
      return NULL; 
    }
    dataptr = data + olddatasize;
    
    dataptr = i_data_add_int (data, dataptr, datasize, &opt->type);
    if (!dataptr) 
    { i_printf (1, "i_form_form_to_data unable to copy opt->type into data"); free (data); return NULL; }

    dataptr = i_data_add_chunk (data, dataptr, datasize, opt->data, opt->datasize);
    if (!dataptr) 
    { i_printf (1, "i_form_form_to_data unable to copy opt->data into data"); free (data); return NULL; }
  }
  
  olddatasize = datasize;                /* Done the long way around because calculating for 3 ints above makes for incorrect alignement */
  datasize += sizeof(long);
  x = realloc (data, datasize);
  if (!x)
  {
    i_printf (1, "i_form_form_to_data unable to realloc data to fit item_count");
    free (data); 
    return NULL;
  }
  data = x;
  dataptr = data + olddatasize;
  memcpy (dataptr, &form->items->size, sizeof(long));
  dataptr += sizeof(long);

  for (i_list_move_head(form->items); (item = i_list_restore(form->items)) != NULL; i_list_move_next(form->items))
  {
    /* Compile each item to a data block */

    int item_datasize;
    char *item_data;

    item_data = i_form_item_to_data (item, &item_datasize);					/* Compile the item to data */
    if (!item_data) { i_printf (1, "i_form_form_to_data item %s did not compile to data", item->name); continue; }

    olddatasize = datasize;										        /* Recalculate and allocate mem */
    datasize += sizeof(int) + item_datasize;
    x = realloc (data, datasize);
    if (x) data = x;
    else { i_printf (1, "i_form_form_to_data failed to realloc mem"); free (data); if (item_data) free (item_data); return NULL; }
    dataptr = data + olddatasize;

    dataptr = i_data_add_chunk (data, dataptr, datasize, item_data, item_datasize);
    if (item_data) free (item_data);
    if (!dataptr) 
    { i_printf (1, "i_form_form_to_data unable to add item into data"); free (data); return NULL; }
  }

  memcpy (datasizeptr, &datasize, sizeof(int));

  return data;

}

i_form* i_form_data_to_form (char *data, int datasize)
{  
  /* Data format is as follows (REMOVE, same as above)
   *
   * int submit
   * 
   * unsigned long option_count
   * ---(repeated for each option)---
   * int opt_type
   * int opt_datasize
   * void* opt_data
   * ---(end repeated section)---
   *
   * unsigned long item_count
   * ---(repeated for each item)---
   * int item_datasize
   * void *item_data
   * ---(end repeated section)---
   * 
   */

  unsigned long i;
  unsigned long opt_count;
  unsigned long item_count;
  int submit;
  int offset;
  i_form *form;
  char *dataptr = data;

  if (!data || datasize < 1) { i_printf (1, "i_form_data_to_form data is too small"); return NULL; }

  submit = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_form_data_to_form failed to get submit flag from data"); return NULL; }
  dataptr += offset;

  form = i_form_create (NULL, submit);
  if (!form) { i_printf (1, "i_form_data_to_form unable to create form"); return NULL; }

  opt_count = (unsigned long) i_data_get_long (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_form_data_to_form failed to get opt_count from data"); return NULL; }
  dataptr += offset;

  for (i=0; i < opt_count; i++)
  {
    int opt_type;
    int opt_datasize;
    char *opt_data = NULL;
  
    opt_type = i_data_get_int (data, dataptr, datasize, &offset);
    if (offset < 1)
    { i_printf (1, "i_form_data_to_form failed to get opt_type from data"); i_form_free (form); return NULL; }
    dataptr += offset;

    opt_data = i_data_get_chunk (data, dataptr, datasize, &opt_datasize, &offset);
    if (offset < 1)
    { i_printf (1, "i_form_data_to_form failed to get opt_data from data"); i_form_free (form); return NULL; }
    dataptr += offset;

    i_form_option_add (form, opt_type, opt_data, opt_datasize);
    free (opt_data);
  }

  item_count = (unsigned long) i_data_get_long (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_form_data_to_form failed to get item_count from data"); i_form_free (form); return NULL; }
  dataptr += offset;

  for (i=0; i < item_count; i++)
  {
    int item_datasize;
    char *item_data;
    i_form_item *item;

    item_data = i_data_get_chunk (data, dataptr, datasize, &item_datasize, &offset);
    if (offset < 1)
    { i_printf (1, "i_form_data_to_form failed to get item_data from data"); i_form_free (form); return NULL; }
    dataptr += offset;
    
    item = i_form_data_to_item (item_data, item_datasize);	              /* Decode the data to an item */
    if (item_data) free (item_data);
    if (!item) 
    { i_printf (1, "i_form_data_to_form failed to convert item_data to item"); i_form_free (form); return NULL; }
    i_form_add_item (form, item);
  }

  return form;
}

/* Form item data/struct conversions */

char* i_form_item_to_data (i_form_item *item, int *datasizeptr)					/* Compile an item to data */
{
  /* Data format is as follows:
   *
   * int type
   * int name_size
   * char *name
   * unsigned long option_count
   *
   * ---(repeat for each option)---
   * int option_type
   * int i
   * int x
   * int y
   * int datasize
   * void *data
   * ---(end repeated section)---
   */
  
  int olddatasize;
  int datasize;
  char *data;
  char *dataptr;
  i_form_item_option *option;

  if (!datasizeptr) return NULL;
  memset (datasizeptr, 0, sizeof(int));
  
  if (!item || !item->options) return NULL;

  datasize = (2 * sizeof(int)) + sizeof(long);
  if (item->name) datasize += strlen(item->name)+1;
  
  data = (char *) malloc (datasize);
  if (!data) { i_printf (1, "i_form_item_to_data unable to malloc data"); return NULL; }
  dataptr = data;

  dataptr = i_data_add_int (data, dataptr, datasize, &item->type);
  if (!dataptr) { i_printf (1, "i_form_item_to_data failed to add item->type to data"); free (data); return NULL; }

  dataptr = i_data_add_string (data, dataptr, datasize, item->name);
  if (!dataptr) { i_printf (1, "i_form_item_to_data failed to add item->name to data"); free (data); return NULL; }

  dataptr = i_data_add_long (data, dataptr, datasize, (long *) &item->options->size);
  if (!dataptr) { i_printf (1, "i_form_item_to_data failed to add item->options->size to data"); free (data); return NULL; }

  for (i_list_move_head(item->options); (option = i_list_restore(item->options)) != NULL; i_list_move_next(item->options))
  {
    /* Loop through each option */
    char *x;

    olddatasize = datasize;
    datasize += (5 * sizeof(int)) + option->datasize;
    x = realloc (data, datasize);
    if (!x) 
    { i_printf (1, "i_form_item_to_data failed to realloc data to add an item option"); return NULL; }
    data = x;
    dataptr = data + olddatasize;
  
    dataptr = i_data_add_int (data, dataptr, datasize, &option->type);
    if (!dataptr) { i_printf (1, "i_form_item_to_data failed to add option->type to data"); free (data); return NULL; }

    dataptr = i_data_add_int (data, dataptr, datasize, &option->i);
    if (!dataptr) { i_printf (1, "i_form_item_to_data failed to add option->type to data"); free (data); return NULL; }

    dataptr = i_data_add_int (data, dataptr, datasize, &option->x);
    if (!dataptr) { i_printf (1, "i_form_item_to_data failed to add option->type to data"); free (data); return NULL; }

    dataptr = i_data_add_int (data, dataptr, datasize, &option->y);
    if (!dataptr) { i_printf (1, "i_form_item_to_data failed to add option->type to data"); free (data); return NULL; }

    dataptr = i_data_add_chunk (data, dataptr, datasize, option->data, option->datasize);
    if (!dataptr) { i_printf (1, "i_form_item_to_data failed to add option->type to data"); free (data); return NULL; }
  }

  memcpy (datasizeptr, &datasize, sizeof(int));
    
  return data;
}

i_form_item* i_form_data_to_item (char *data, int datasize)	  /* Decode data to an item */
{
  unsigned long i;
  unsigned long option_count;
  int item_type;
  int offset;
  char *item_name;
  char *dataptr = data;
  i_form_item *item;

  if (!data || datasize < 1) 
  { i_printf (1, "i_form_data_to_item called with NULL data or datasize < 1"); return NULL; }

  item_type = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1) 
  { i_printf (1, "i_form_data_to_item failed to get item_type from data"); return NULL; }
  dataptr += offset;
  
  item_name = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) 
  { i_printf (1, "i_form_data_to_item failed to get item_name from data"); return NULL; }
  dataptr += offset;

  option_count = (unsigned long) i_data_get_long (data, dataptr, datasize, &offset);
  if (offset < 1) 
  { i_printf (1, "i_form_data_to_item failed to get option_count from data"); return NULL; }
  dataptr += offset;
  
  item = i_form_create_item (item_type, item_name);
  if (item_name) free (item_name);
  if (!item) { i_printf (1, "i_form_item_to_data unable to create item"); return NULL; }
  
  for (i=0; i < option_count; i++)
  {
    int option_type;
    int option_i;
    int option_x;
    int option_y;
    int option_datasize;
    char *option_data;
  
    option_type = i_data_get_int (data, dataptr, datasize, &offset);
    if (offset < 1) 
    { i_printf (1, "i_form_data_to_item failed to get option_type from data"); return NULL; }
    dataptr += offset;  
    
    option_i = i_data_get_int (data, dataptr, datasize, &offset);
    if (offset < 1) 
    { i_printf (1, "i_form_data_to_item failed to get option_i from data"); return NULL; }
    dataptr += offset;  
    
    option_x = i_data_get_int (data, dataptr, datasize, &offset);
    if (offset < 1) 
    { i_printf (1, "i_form_data_to_item failed to get option_x from data"); return NULL; }
    dataptr += offset;  
    
    option_y = i_data_get_int (data, dataptr, datasize, &offset);
    if (offset < 1) 
    { i_printf (1, "i_form_data_to_item failed to get option_y from data"); return NULL; }
    dataptr += offset;

    option_data = i_data_get_chunk (data, dataptr, datasize, &option_datasize, &offset);
    if (offset < 1) 
    { i_printf (1, "i_form_data_to_item failed to get option_data from data"); return NULL; }
    dataptr += offset;
    
    i_form_item_add_option (item, option_type, option_i, option_x, option_y, option_data, option_datasize);		/* Add the decoded option */				
    if (option_data) free (option_data);
  }

	return item;
}

/* Form Duplication */

i_form* i_form_duplicate (i_form *form)
{
  int datasize;
  char *data;

  data = i_form_form_to_data (form, &datasize);
  if (!data) { i_printf (1, "i_form_duplicate unable to compile form to data"); return NULL; }

  form = i_form_data_to_form (data, datasize);
  free (data);
  if (!form) { i_printf (1, "i_form_duplicate unable to decode data to form"); return NULL; }

  return form;
}

/* Generic value fetching */

i_form_item_option* i_form_get_value_for_item (i_form *form, char *name)
{
  i_form_item *item;
  i_form_item_option *opt;

  if (!form || !name) return NULL;

  item = i_form_find_item (form, name);
  if (!item) return NULL;

  switch (item->type)
  {
    case FORM_ITEM_DROPDOWN: opt = i_form_dropdown_get_selected_value (item);
                             break;
    default: opt = i_form_item_find_option (item, ITEM_OPTION_VALUE, 0, 0, 0);
  }

  return opt;
}

/* Generic failure handling function for form-related handlers */

int i_form_handler_failed (i_resource *self, i_message *msg, i_form_reqdata *reqdata)
{
  if (msg) i_respond_failed (self, msg, 0);
  if (reqdata) i_form_reqdata_free (reqdata);
  return 0;
}

/* @} */
