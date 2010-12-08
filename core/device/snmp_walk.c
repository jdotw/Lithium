#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <netdb.h>
#include <string.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/object.h>
#include <induction/path.h>
#include <induction/hierarchy.h>

#include "snmp.h"
#include "avail.h"

static i_list* static_walk_list = NULL;

/* SNMP Walk */

int l_snmp_walk (i_resource *self, l_snmp_session *session, char *root_str, int flags, int (*callback_func) (i_resource *self, l_snmp_session *session, struct snmp_pdu *pdu, void *data), void *passdata)
{
  /* Create and fillout the reqdata and perform the 
   * root GETNEXT SNMP request
   */

  int num;
  int reqid;
  oid *oidptr;
  l_snmp_walk_reqdata *reqdata;

  if (!self || !session || !root_str || !callback_func) return -1;

  /* Check marking */
  if (self->hierarchy->dev->mark == ENTSTATE_OUTOFSERVICE) return -1; 

  /* Create and setup the reqdata */
  
  reqdata = l_snmp_walk_reqdata_create ();
  if (!reqdata)
  { i_printf (1, "l_snmp_walk failed to create reqdata"); return -1; }

  reqdata->session = session;
  reqdata->flags = flags;
  reqdata->callback_func = callback_func;
  reqdata->passdata = passdata;

  /* Parse the OID string */

  oidptr = snmp_parse_oid (root_str, reqdata->root, &reqdata->root_length);
  if (!oidptr)
  { i_printf (1, "l_snmp_walk failed to parse OID %s", root_str); l_snmp_walk_reqdata_free (reqdata); return -1; }
  memmove (reqdata->name, reqdata->root, (reqdata->root_length * sizeof(oid)));
  reqdata->name_length = reqdata->root_length;

  /* Start the process */

  reqid = l_snmp_walk_get_next (self, reqdata);
  reqdata->id = reqid;   /* The walk-id is the initial reqid */
  if (reqid == -1)
  { i_printf (2, "l_snmp_walk failed to perform initial SNMP GETNEXT request"); l_snmp_walk_reqdata_free (reqdata); return -1; }

  /* Enqueue the walk into the walk-list */

  num = l_snmp_walk_reqlist_add (reqdata);
  if (num != 0)
  { i_printf (1, "l_snmp_walk failed to add walk reqdata to walk_list"); l_snmp_walk_reqdata_free (reqdata); return -1; }

  /* Finished for now */

  return reqdata->id;
}

int l_snmp_walk_get_next (i_resource *self, l_snmp_walk_reqdata *reqdata)
{
  int num;
  int reqid;
  int *reqidptr;
  char *errstr;
  struct snmp_pdu *request = NULL;

  /* create PDU for GETNEXT request and add object name to request */

  request = snmp_pdu_create(SNMP_MSG_GETNEXT);
  snmp_add_null_var (request, reqdata->name, reqdata->name_length);

  /* do the request */
  reqid = snmp_send (reqdata->session->ss, request);
  if (reqid == 0)     /* snmp_send returns 0 on error */
  { 
    i_object *availobj;
    
    /* Error sending */
    snmp_error (reqdata->session->ss, NULL, NULL, &errstr);
    i_printf (1, "l_snmp_walk_get_next failed to send GETNEXT pdu (%s)", errstr); 
    free (errstr);
    snmp_free_pdu (request); 

    /* Record avail */
    availobj = l_snmp_availobj ();
    l_avail_record_fail (availobj);

    return -1; 
  }

  reqidptr = (int *) malloc (sizeof(int));
  if (!reqidptr)
  { i_printf (1, "l_snmp_walk_get_next sent request but failed to malloc reqidptr"); return -1; }

  memcpy (reqidptr, &reqid, sizeof(int));
  num = i_list_enqueue (reqdata->reqid_list, reqidptr);
  if (num != 0)
  { i_printf (1, "l_snmp_walk_get_next failed to enqueue reqidptr into reqdata->reqid_list"); free (reqidptr); return -1; }

  /* install the pdu_callback */

  num = l_snmp_pducallback_add (self, reqdata->session, reqid, l_snmp_walk_get_next_callback, reqdata);
  if (num != 0)
  { i_printf (1, "l_snmp_walk_get_next sent request but failed to install PDU callback"); return -1; }

  /* finished */

  return reqid;
}

int l_snmp_walk_get_next_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata)
{
  int num;
  int *reqidptr;
  struct variable_list *vars;
  l_snmp_walk_reqdata *reqdata = passdata;

  if (!self || !reqdata) return -1;

  /* Ensure this walk is still valid and in the reqlist */

  num = i_list_search (static_walk_list, reqdata);
  if (num != 0)
  { i_printf (1, "l_snmp_walk_get_next_callback called for an invalid/non-current walk. discarded PDU"); return -1; }
  
  /* Remove the reqid from the reqdata->reqid_list */

  for (i_list_move_head(reqdata->reqid_list); (reqidptr=(int *)i_list_restore(reqdata->reqid_list))!=NULL; i_list_move_next(reqdata->reqid_list))
  {
    if (*reqidptr == reqid)
    { 
      num = i_list_delete (reqdata->reqid_list); 
      if (num != 0)
      { i_printf (1, "l_snmp_walk_get_next_callback failed to remove reqid %i from reqdata->reqid_list", reqid); }
    }
  }

  /* Process the PDU */
  
  if (!pdu)
  { 
    i_printf (2, "l_snmp_walk_get_next_callback recieved NULL pdu (%s)", l_snmp_session_error_str (reqdata->session));
    reqdata->callback_func (self, reqdata->session, NULL, reqdata->passdata);
    l_snmp_walk_reqlist_remove (reqdata->id);
    return -1;
  }
  
  if (pdu->errstat == SNMP_ERR_NOERROR)
  {
    /* check resulting variables */

    for(vars = pdu->variables; vars; vars = vars->next_variable)
    {
      /* Loop through the variables recvd */
      if ((vars->name_length < reqdata->root_length) || (memcmp(reqdata->root, vars->name, reqdata->root_length * sizeof(oid))!=0))
      {
        /* not part of this subtree, walking of tree successfully completed */
        reqdata->session->error = SNMP_ERROR_NOERROR;
        reqdata->callback_func (self, reqdata->session, NULL, reqdata->passdata);
        l_snmp_walk_reqlist_remove (reqdata->id);
        return 0;
      }

      /* Run the callback */
      
      reqdata->session->error = SNMP_ERROR_NOERROR;
      num = reqdata->callback_func (self, reqdata->session, pdu, reqdata->passdata);
      if (num != 0)
      {
        i_printf (2, "l_snmp_walk_get_next_callback callback did not return 0, terminating walk");
        l_snmp_walk_reqlist_remove (reqdata->id);  /* Also frees reqdata */
        return 0;
      }

      /* Adjust the name and name_length for the next OID */

      if ((vars->type != SNMP_ENDOFMIBVIEW) && (vars->type != SNMP_NOSUCHOBJECT) && (vars->type != SNMP_NOSUCHINSTANCE))
      {
        /* not an exception value */
        if (reqdata->flags & CHECK_OID_INCREASE && snmp_oid_compare(reqdata->name, reqdata->name_length, vars->name, vars->name_length) >= 0)
        {
          /* CHECK_OID_INCREASING flag is set and the OID is
           * not increasing. Report the error and terminate
           * the walk
           */

          char name_buf[SPRINT_MAX_LEN];
          char var_buf[SPRINT_MAX_LEN];
              
          snprint_objid (name_buf, SPRINT_MAX_LEN, reqdata->name, reqdata->name_length);
          snprint_objid (var_buf, SPRINT_MAX_LEN, vars->name, pdu->variables->name_length);
          i_printf (2, "l_snmp_walk_get_next_callback walk failed, OID not increasing: %s >= %s\n", name_buf, var_buf);

          /* Call back has already been called, so we cautiously
           * call it again with a NULL pdu and the error set
           * to SNMP_ERROR_OIDNOTINCREASING
           */

          reqdata->session->error = SNMP_ERROR_OIDNOTINCREASING;
          reqdata->callback_func (self, reqdata->session, NULL, reqdata->passdata);

          /* Remove walk from reqlist */
          
          l_snmp_walk_reqlist_remove (reqdata->id);  /* Also frees reqdata */

          return -1;
        }
        memmove((char *)reqdata->name, (char *)vars->name, vars->name_length * sizeof(oid));
        reqdata->name_length = vars->name_length;
      }
      else /* an exception value, so stop */
      {
        /* This is the end of the walk,
         * but the callback has already been called,
         * no need to call it again
         */

        l_snmp_walk_reqlist_remove (reqdata->id);  /* Also frees reqdata */

        return -1;
      }

    }

    /* Send the next request */
    num = l_snmp_walk_get_next (self, reqdata);
    if (num == -1)
    {
      i_printf (1, "l_snmp_walk_get_next_callback failed to call l_snmp_walk_get_next");
      /* This is the end of the walk,
       * but the callback has already been called, 
       * no need to call it again
       */

      l_snmp_walk_reqlist_remove (reqdata->id);  /* Also frees reqdata */
      return -1; 
    }
  }
  else
  {
    /* error in response. first, print it */

    if (pdu->errstat == SNMP_ERR_NOSUCHNAME)
    { i_printf (2, "l_snmp_walk_get_next_callback reached end of MIB"); }
    else
    { 
      i_printf (1, "l_snmp_walk_get_next_callback found error in packet (%s)", snmp_errstring(pdu->errstat));
      if (pdu->errindex != 0)
      { i_printf (1, "l_snmp_walk_get_next_callback failed object"); }
    }

    /* the signal the end of the walk */

    reqdata->session->error = SNMP_ERROR_NOERROR;
    reqdata->callback_func (self, reqdata->session, NULL, reqdata->passdata);
    l_snmp_walk_reqlist_remove (reqdata->id); 

    return -1;
  }

  return 0;
}

int l_snmp_walk_terminate (int walk_id)
{
  int num;

  num = l_snmp_walk_reqlist_remove (walk_id);
  if (num != 0)
  { i_printf (1, "l_snmp_walk_terminate failed to remove walk id %i from list", walk_id); return -1; }

  return 0;
}

/* SNMP WALK Request Data */

l_snmp_walk_reqdata* l_snmp_walk_reqdata_create ()
{
  l_snmp_walk_reqdata *reqdata;

  reqdata = (l_snmp_walk_reqdata *) malloc (sizeof(l_snmp_walk_reqdata));
  if (!reqdata)
  { i_printf (1, "l_snmp_walk_reqdata_create failed to malloc reqdata"); return NULL; }
  memset (reqdata, 0, sizeof(l_snmp_walk_reqdata));

  reqdata->name_length = MAX_OID_LEN;
  reqdata->root_length = MAX_OID_LEN;
  reqdata->reqid_list = i_list_create ();
  if (!reqdata->reqid_list)
  { i_printf (1, "l_snmp_walk_reqdata_create failed to create reqdata->reqid_list"); l_snmp_walk_reqdata_free (reqdata); return NULL; }
  i_list_set_destructor (reqdata->reqid_list, free);

  return reqdata;
}

void l_snmp_walk_reqdata_free (void *reqdataptr)
{
  int *reqidptr;
  l_snmp_walk_reqdata *reqdata = reqdataptr;

  /* DO NOT FREE THE SESSION */

  if (!reqdata) return;

  if (reqdata->reqid_list)
  {
    for (i_list_move_head(reqdata->reqid_list); (reqidptr=(int *)i_list_restore(reqdata->reqid_list))!=NULL; i_list_move_next(reqdata->reqid_list))
    {
      int reqid;
      memcpy (&reqid, reqidptr, sizeof(int));
      l_snmp_pducallback_remove_by_reqid (reqdata->session, reqid);
    }
    i_list_free (reqdata->reqid_list);
  }

  free (reqdata);
}

/* SNMP Walk Request List */

int l_snmp_walk_reqlist_add (l_snmp_walk_reqdata *reqdata)
{
  int num;

  if (!reqdata) return -1;

  if (!static_walk_list)
  { 
    static_walk_list = i_list_create ();
    if (!static_walk_list)
    { i_printf (1, "l_snmp_walk_reqlist_add failed to create static_walk_list"); return -1; }
    i_list_set_destructor (static_walk_list, l_snmp_walk_reqdata_free);
  }

  num = i_list_enqueue (static_walk_list, reqdata);
  if (num != 0)
  { i_printf (1, "l_snmp_walk_reqlist_add failed to enqueue reqdata into static_walk_list"); return -1; }

  return 0;
}

int l_snmp_walk_reqlist_remove (int id)
{
  int removed_count = 0;
  l_snmp_walk_reqdata *reqdata;

  for (i_list_move_head(static_walk_list); (reqdata=i_list_restore(static_walk_list))!=NULL; i_list_move_next(static_walk_list))
  {
    if (reqdata->id == id)
    { i_list_delete (static_walk_list); removed_count++; }
  }

  if (static_walk_list && static_walk_list->size < 1)
  { i_list_free (static_walk_list); static_walk_list = NULL; }

  if (removed_count == 0) return -1;

  return 0;
}

