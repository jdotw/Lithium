#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "entity.h"
#include "form.h"
#include "message.h"
#include "auth.h"

/** \addtogroup form Web Forms
 * @{
 */

/* Reqdata struct manipulation */

i_form_reqdata* i_form_reqdata_create ()
{
  i_form_reqdata *reqdata;

  reqdata = (i_form_reqdata *) malloc (sizeof(i_form_reqdata));
  if (!reqdata)
  { i_printf (1, "i_form_reqdata_create failed to malloc reqdata struct"); return NULL; }
  memset (reqdata, 0, sizeof(i_form_reqdata));

  return reqdata;
}

void i_form_reqdata_free (void *reqdataptr)
{
  i_form_reqdata *reqdata = reqdataptr;

  if (!reqdata) return;

  if (reqdata->entaddr) i_entity_address_free (reqdata->entaddr);
  if (reqdata->msg_in) i_message_free (reqdata->msg_in);
  if (reqdata->auth) i_authentication_free (reqdata->auth);
  if (reqdata->form_passdata) free (reqdata->form_passdata);
  
  if (reqdata->form_name) free (reqdata->form_name);
  if (reqdata->form_in) i_form_free (reqdata->form_in);
  if (reqdata->form_out) i_form_free (reqdata->form_out);

  free (reqdata);
}

/* @} */
