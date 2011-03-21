#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include <induction.h>
#include <induction/entity.h>
#include <induction/form.h>

#include "form_submit.h"
#include "form_render.h"
#include "resource_address.h"
#include "shutdown.h"
#include "errorcodes.h"

/* cbdata struct manipulation */

a_form_submit_cbdata* a_form_submit_cbdata_create ()
{
  a_form_submit_cbdata *cbdata;

  cbdata = (a_form_submit_cbdata *) malloc (sizeof(a_form_submit_cbdata));
  if (!cbdata)
  { i_printf (1, "a_form_submit_cbdata_create failed to malloc cbdata"); return NULL; }
  memset (cbdata, 0, sizeof(a_form_submit_cbdata));

  return cbdata;
}

void a_form_submit_cbdata_free (void *cbdataptr)
{
  a_form_submit_cbdata *cbdata = cbdataptr;

  if (!cbdataptr) return;
  free (cbdata);
}

/* form_submit functions */

int a_form_submit (i_resource *self, int argc, char *argv[], int optind)
{
  /* New way of doing things. First we get the original form, then just
   * change the values. Thus preserving the various options 
   */

  i_resource_address *resaddr;
  i_entity_address *entaddr;
  a_form_submit_cbdata *cbdata;

  /* args/state checking */
  
  if (argc < optind+11)
  { 
    printf ("<BR>Insufficient arguments to a_form_submit (argc is %i, optind+11 is %i)\n", argc, optind+11); 
    exit (ERROR_BADARGS); 
  }
  
  if (!self->core_socket) 
  { 
    printf ("<BR><BR>a_form_submit failed because there is no connection to the client handler<BR>\n");
    exit (ERROR_NOCONN); 
  }

  /* Create/Setup cbdata */
  cbdata = a_form_submit_cbdata_create ();
  if (!cbdata)
  { printf ("<BR>Failed to create cbdata struct"); exit (ERROR_INTERNAL); }
  cbdata->src_resaddr = argv[optind + 2];
  cbdata->src_entaddr = argv[optind + 3];
  cbdata->src_form = argv[optind + 4];
  cbdata->src_ref_sec = atol (argv[optind + 5]);
  if (strlen(argv[optind+6]) > 0)
  { cbdata->src_passdata = argv[optind + 6]; cbdata->src_passdatasize = strlen (cbdata->src_passdata) + 1; }
  cbdata->dst_resaddr = argv[optind + 7];
  cbdata->dst_entaddr = argv[optind + 8];
  cbdata->dst_form = argv[optind + 9];
  cbdata->dst_ref_sec = atol(argv[optind + 10]);
  if (strlen(argv[optind+11]) > 0)
  { cbdata->dst_passdata = argv[optind + 11]; cbdata->dst_passdatasize = strlen (cbdata->dst_passdata) + 1; }
  cbdata->argc = argc;
  cbdata->argv = argv;
  cbdata->optind = optind;

  /* First, get the original source form */

  resaddr = a_resource_address_string_to_struct (cbdata->src_resaddr);
  if (!resaddr) 
  { 
    printf ("<BR>Failed to convert src_resaddr string into resource address\n"); 
    a_form_submit_cbdata_free (cbdata); 
    exit (ERROR_BADADDRESS);
  }
  if (cbdata->src_entaddr && strlen(cbdata->src_entaddr) > 0)
  { entaddr = i_entity_address_struct (cbdata->src_entaddr); } 
  else
  { entaddr = NULL; }
  if (!resaddr) 
  { 
    printf ("<BR>Failed to convert src_entaddr string into entity address\n"); 
    a_form_submit_cbdata_free (cbdata); 
    exit (ERROR_BADADDRESS);
  }


  cbdata->formget_op = i_form_get (self, resaddr, entaddr, cbdata->src_form, cbdata->src_ref_sec, cbdata->src_passdata, cbdata->src_passdatasize, a_form_submit_formget_callback, cbdata);
  i_resource_free_address (resaddr);
  i_entity_address_free (entaddr);
  if (!cbdata->formget_op) 
  { 
    printf ("<BR>Failed to send request for original form (%s %s %s)<BR>\n", cbdata->src_resaddr, cbdata->src_entaddr, cbdata->src_form); 
    a_form_submit_cbdata_free (cbdata); 
    exit (ERROR_SENDFAILED);
  }

  return 1;   /* Return 1 to enter i_loop */
}

int a_form_submit_formget_callback (i_resource *self, i_form *form, i_resource_address *srcresaddr, i_entity_address *srcentaddr, char *form_name, int result, void *passdata)
{
  /* Called when the original form is received */

  int i;
  int num;
  i_resource_address *resaddr;
  i_entity_address *entaddr;
  a_form_submit_cbdata *cbdata = passdata;

  if (!form)
  {
    if (result == FORM_RESULT_DENIED)
    { exit (ERROR_DENIED); }
    else
    {
      printf ("<BR>Failed to retrieve original form\n");
      exit (ERROR_OPERROR);
    }
  }
  
  /* Loop through each supplied name/value pair and update as needed */

  for (i=cbdata->optind+12; i < (cbdata->argc-2); i+=2)                      /* i+1/argc-1 to skip the submit variable, +=2 because the args are in pairs */
  {
    i_form_item *item;

    item = i_form_find_item (form, cbdata->argv[i]);                /* Find the item */
    if (!item) 
    {
      /* The item doesnt exist in the form, so we add it as a hidden */
      i_form_hidden_add (form, cbdata->argv[i], cbdata->argv[i+1]);
      continue;
    }

    switch (item->type)
    {
      case FORM_ITEM_DROPDOWN: 
        num = i_form_dropdown_set_selected (item, cbdata->argv[i+1]);
        break;
      default:
        num = i_form_generic_item_set_value (item, cbdata->argv[i+1], strlen(cbdata->argv[i+1])+1);       /* Generic value change */
    }

    if (num != 0) printf ("<BR>Warning: Failed to update item \"%s\" while preparing submission form\n", cbdata->argv[i]);
  }

  resaddr = a_resource_address_string_to_struct (cbdata->dst_resaddr);
  if (!resaddr)
  { 
    printf ("<BR>Failed to convert dst_resaddr address string to address struct\n");
    a_form_submit_cbdata_free (cbdata);
    a_shutdown (self);
    exit (ERROR_BADADDRESS);
  }
  if (cbdata->dst_entaddr && strlen (cbdata->dst_entaddr) > 0)
  { 
    entaddr = i_entity_address_struct (cbdata->dst_entaddr); 
    if (!entaddr)
    { 
      printf ("<BR>Failed to convert dst_entaddr address string to address struct\n");
      a_form_submit_cbdata_free (cbdata);
      a_shutdown (self);
      exit (ERROR_BADADDRESS);
    }
  }
  else 
  { entaddr = NULL; }

  cbdata->formsend_op = i_form_send (self, resaddr, entaddr, cbdata->dst_form, cbdata->dst_ref_sec, form, a_form_submit_formsend_callback, cbdata);
  i_resource_free_address (resaddr);
  i_entity_address_free (entaddr);
  if (!cbdata->formsend_op)
  {
    printf ("<BR>Failed to send form submission\n");
    a_form_submit_cbdata_free (cbdata);
    a_shutdown (self);
    exit (ERROR_SENDFAILED);
  }

  return 0;
}

int a_form_submit_formsend_callback (i_resource *self, i_form *form, void *passdata)
{
  a_form_submit_cbdata *cbdata = passdata;

  /* state check */
  if (!form)
  {
    printf ("<BR>Failed to send form submission (NULL form sent to formsend_callnack)\n");
    a_form_submit_cbdata_free (cbdata);
    a_shutdown (self);
    exit (ERROR_OPERROR);
  }

  /* Render form or report result */
  a_form_render (self, form, cbdata->dst_resaddr, cbdata->dst_entaddr, cbdata->dst_form, cbdata->dst_ref_sec, cbdata->dst_passdata);

  /* Terminate */
  a_form_submit_cbdata_free (cbdata);
  a_shutdown (self);
  exit (0);

  return 0;
}

