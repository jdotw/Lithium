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

#include "form_get.h"
#include "resource_address.h"
#include "shutdown.h"
#include "form_render.h"
#include "errorcodes.h"

typedef struct
{
  i_form_get_cbdata *form_op;
  char *resaddr_str;
  char *entaddr_str;
  char *form_name;
  time_t ref_sec;
  char *form_passdata;
  int form_passdatasize;
} a_form_get_cbdata;

/* cbdata struct manipulation */

a_form_get_cbdata* a_form_get_cbdata_create ()
{
  a_form_get_cbdata *cbdata;

  cbdata = (a_form_get_cbdata *) malloc (sizeof(a_form_get_cbdata));
  if (!cbdata)
  { printf ("<BR>Failed to malloc a_form_get_cbdata struct\n"); return NULL; }
  memset (cbdata, 0, sizeof(a_form_get_cbdata));

  return cbdata;
}

void a_form_get_cbdata_free (void *cbdataptr)
{
  a_form_get_cbdata *cbdata = cbdataptr;

  if (!cbdata) return;

  free (cbdata);
}

/* form_get functions */

int a_form_get (i_resource *self, int argc, char *argv[], int optind)
{
  i_resource_address *resaddr;
  i_entity_address *entaddr;
  a_form_get_cbdata *cbdata;

  /* Args/State check */
  
  if (argc < optind+5) 
  { 
    printf ("Not enough arguments sent to a_form_get<BR>\n"); 
    exit (ERROR_BADARGS); 
  }

  if (!self->core_socket) 
  { 
    printf ("<BR>Failed to retrieve form. No connection to Lithium Client Handler present\n"); 
    exit (ERROR_NOCONN);
  }

  /* Create/Setup cbdata */

  cbdata = a_form_get_cbdata_create ();
  if (!cbdata)
  { 
    printf ("<BR>Failed to create cbdata\n");
    exit (ERROR_INTERNAL);
  }
  cbdata->resaddr_str = argv[optind + 2];
  cbdata->entaddr_str = argv[optind + 3];
  cbdata->form_name = argv[optind + 4];
  cbdata->ref_sec = (time_t) atol (argv[optind + 5]);
  if (argc > optind+6 && strlen (argv[optind+6]) > 0) 
  { 
    cbdata->form_passdata = argv[optind+6]; cbdata->form_passdatasize = strlen (cbdata->form_passdata)+6; 
  }

  resaddr = a_resource_address_string_to_struct (cbdata->resaddr_str);
  if (!resaddr)
  { 
    printf ("<BR>Failed to convert resource address string to address struct\n"); 
    a_form_get_cbdata_free (cbdata); 
    exit (ERROR_BADADDRESS);
  }

  if (cbdata->entaddr_str && strlen(cbdata->entaddr_str) > 0)
  { entaddr = i_entity_address_struct (cbdata->entaddr_str); }
  else
  { entaddr = NULL; }

  cbdata->form_op = i_form_get (self, resaddr, entaddr, cbdata->form_name, cbdata->ref_sec, cbdata->form_passdata, cbdata->form_passdatasize, a_form_get_form_callback, cbdata);
  i_resource_address_free (resaddr);
  i_entity_address_free (entaddr);
  if (!cbdata->form_op)
  { 
    printf ("<BR>Failed to send request for form\n"); 
    a_form_get_cbdata_free (cbdata); 
    exit (ERROR_SENDFAILED);
  }

  return 1;
}

int a_form_get_form_callback (i_resource *self, i_form *form, i_resource_address *resaddr, i_entity_address *entaddr, char *form_name, int result, void *passdata)
{
  a_form_get_cbdata *cbdata = passdata;

  if (!form)
  { 
    if (result == FORM_RESULT_DENIED)
    {
      exit (ERROR_DENIED); 
    }
    else 
    { 
      printf ("<BR>Failed to retrieve form\n"); 
      exit (ERROR_OPERROR);
    }
  }

  /* Now, on to rendering the form */

  a_form_render (self, form, cbdata->resaddr_str, cbdata->entaddr_str, cbdata->form_name, cbdata->ref_sec, cbdata->form_passdata);
  i_form_free (form);
  a_form_get_cbdata_free (cbdata);

  /* Terminate process */
  
  a_shutdown (self);
  exit (0);

  /* Finished */

  return 0;
}
