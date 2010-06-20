#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/name.h>
#include <induction/desc.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>

#include "record.h"
#include "snmp.h"

/*
 * SNMP Object Factory 
 *
 * The refresh function is called to walk the name_oid_str
 * and add any new objects as well as removing any obsolete 
 * objects from the container to which the factory pertains.
 *
 */

/* Factory Refresh */

int l_snmp_objfact_refresh (i_resource *self, i_object *obj, int opcode)
{
  l_snmp_objfact *fact = obj->refresh_data;

  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:                     /* Begin a refresh operation */

      /* Move to start of factory object list */
      i_list_move_head (fact->obj_list); 

      /* Open SNMP session */
      fact->session = l_snmp_session_open_device (self, fact->dev);
      if (!fact->session)
      { i_printf (1, "l_snmp_objfact_refresh failed to open SNMP session to device"); return -1; }

      /* Begin SNMP walk */
      fact->walkid = l_snmp_walk (self, fact->session, fact->name_oid_str, 0, l_snmp_objfact_refresh_walkcb, fact);
      if (fact->walkid == -1)
      { 
        i_printf (1, "l_snmp_objfact_refresh failed to begin walk of %s oid tree for container %s", fact->name_oid_str, fact->cnt->name_str);
        l_snmp_session_close (fact->session);
        fact->session = NULL;
        return -1; 
      }

      break;

    case REFOP_COLLISION:
      break;
      
    case REFOP_TERMINATE:
      if (fact->walkid > 0)
      { l_snmp_walk_terminate (fact->walkid); fact->walkid = 0; }
      if (fact->session)
      { l_snmp_session_close (fact->session); fact->session = NULL; }
      break;

    case REFOP_CLEANDATA:
      /* DO NOT free the factory. The object is freed 
       * from the factories objfact_free funct
       */
      break;

    default:
      i_printf (1, "l_snmp_objfact_refresh unknown op code received (%i)", opcode & REFOP_ALL);
  }

  return 0;
}

int l_snmp_objfact_refresh_walkcb (i_resource *self, l_snmp_session *session, struct snmp_pdu *pdu, void *passdata)
{
  int num;
  i_object *obj;
  l_snmp_objfact *fact = passdata; 

  if (pdu && pdu->variables)
  {
    /* PDU Received */
    char *name_str;
    void *p;

    /* Obtain unique name for object */
    switch (fact->namesource)
    {
      case OBJFACT_NAME_PDU:
        name_str = l_snmp_var_to_str (pdu->variables);
        if (!name_str || strlen(name_str) < 1)
        { 
          i_printf (1, "l_snmp_objfact_walkcb failed to convert received PDU variable into a unique name_str for object in container %s. Falling back to OID index for name %d", fact->cnt->name_str, pdu->variables->name[pdu->variables->name_length-1]);
          if (name_str) free (name_str);
          asprintf (&name_str, "%d", pdu->variables->name[pdu->variables->name_length-1]);
        }
        break;
      case OBJFACT_NAME_OID:
        asprintf (&name_str, "%d", pdu->variables->name[pdu->variables->name_length-1]);
        break;
      case OBJFACT_NAME_HEXSTRING:
        name_str = l_snmp_var_to_hexstr (pdu->variables);
        if (!name_str || strlen(name_str) < 1)
        { 
          i_printf (1, "l_snmp_objfact_walkcb failed to convert received PDU variable into a unique name_str (hex string) for object in container %s. Falling back to OID index for name %d", fact->cnt->name_str, pdu->variables->name[pdu->variables->name_length-1]);
          if (name_str) free (name_str);
          asprintf (&name_str, "%d", pdu->variables->name[pdu->variables->name_length-1]);
        }
        break;
      default:
        i_printf (1, "l_snmp_objfact_walkcb name source not specified");
        return 0;
    }

    /* Perform duplicate name handling if enabled */
    if (fact->enable_dupname_handling && fact->first_refresh)
    {
      /* Dup name handling has been enabled and this is still
       * the first refresh. Ignore the PDU 
       */
      free (name_str);
      return 0;
    }
    else if (fact->enable_dupname_handling)
    {
      /* Build the index_oidstr */
      size_t i;
      char *index_oidstr = NULL;
      char *new_name;
      for (i=0; i < (pdu->variables->name_length - fact->name_oid_len); i++)
      {
        char *temp_str;
        if (index_oidstr)
        {
          asprintf (&temp_str, "%s.%i", index_oidstr, pdu->variables->name[fact->name_oid_len+i]);
          free (index_oidstr);
          index_oidstr = temp_str;
        }
        else
        { asprintf (&index_oidstr, "%i", pdu->variables->name[fact->name_oid_len+i]); }
      }

      /* Append to the name and desc */
      asprintf (&new_name, "%s_%s", name_str, index_oidstr);
      free (name_str);
      name_str = new_name;
      if (index_oidstr) free (index_oidstr);
    }

    /* Parse the name_str to remove non-alphanumeric chars */
    i_name_parse (name_str);

    /* Retrieve object from current object_list position */
    obj = i_list_restore (fact->obj_list);
    if (!obj || strcmp(obj->name_str, name_str) != 0)
    {
      /* The object at the current list position does not 
       * match the name_str retrieved from the current PDU.
       * Because the obj_list is ALWAYS kept in index-order, this
       * means that either the current obj_list object is not long
       * valid, OR, a this PDU is a new object
       */
      i_object *exist_obj;
      i_list *obsol_list;

      /* Assume at this point that the current obj_list object
       * is obsolete. Traverse the rest of the obj_list in an attempt
       * to find the object to which this PDU refers to. Queue all objects
       * from the obj_list that are encountered BEFORE the object to which 
       * the PDU refers into the obsol_list (obsolete list)
       */

      /* Create list */
      obsol_list = i_list_create ();
      if (!obsol_list)
      { i_printf (1, "l_snmp_objfact_walkcb failed to create obsol_list"); free (name_str); return 0; }

      /* Fill the obsol_list with obsolete objects */
      p = fact->obj_list->p;
      for (; (exist_obj=i_list_restore(fact->obj_list))!=NULL; i_list_move_next(fact->obj_list))
      {
        if (!strcmp(exist_obj->name_str, name_str))
        { break; }
        else
        {
          i_list_enqueue (obsol_list, exist_obj); 
        }
      }
      fact->obj_list->p = p;

      /* Look to see if there'san object in the new/unregistered list
       * that matches this one. This is to test for duplicate name identifiers
       */
      if (!fact->enable_dupname_handling)
      {
        i_object *new_obj;
        
        for (i_list_move_head(fact->unreg_list); (new_obj=i_list_restore(fact->unreg_list))!=NULL; i_list_move_next(fact->unreg_list))
        {
          if (!strcmp(new_obj->name_str, name_str))
          { break; }
        }

        if (new_obj)
        {
          /* We have encountered a duplicate identifier on the first
           * refresh. This means in the MIB this factory is walking, 
           * not everyone has a unique identifier. This has been found 
           * to happen in the ifMib. Some (dopey) vendors have all 
           * their interfaces ifDescr set to the same value. Dell
           * and Linksys are guilty of this. 
           *
           * So, seeing as this is the first refresh and we've hit 
           * duplicate identifiers, we must now engage duplicate
           * name processing mode which will automatically append
           * the index_oidstr to the name_str (and then parse it)
           * to ensure unique name_strs are the result
           *
           * Remove all the new/unregistered objects, and set the
           * enable_dupname_handling flag. The rest of the PDUs
           * received in this refresh will be ignored because
           * of the dupname_enabled flag and first_refresh both
           * being 1
           * 
           */

          /* Free new (unregistered) objects and remove from obj_list */
          for (i_list_move_head(fact->unreg_list); (new_obj=i_list_restore(fact->unreg_list))!=NULL; i_list_move_next(fact->unreg_list))
          {
            /* Remove from obj_list */
            num = i_list_search (fact->obj_list, new_obj);
            if (num == 0)
            {
              /* Clean the object */
              if (fact->cleanfunc)
              { fact->cleanfunc (self, fact->cnt, new_obj); }
              /* Remove from obj_list */
              i_list_delete (fact->obj_list);
            }

            /* Free and remove from unreg_list */
            i_entity_free (ENTITY(new_obj));
            i_list_delete (fact->unreg_list);
          }

          /* Reconfigure fact */
          i_printf (0, "l_snmp_objfact_walkcb has enabled dupname_handling for %s", fact->obj->name_str);
          fact->enable_dupname_handling = 1;

          return 0;
        }
      }

      /* Now check to see if exist_obj != NULL. 
       * 
       * If exist_obj is NOT NULL then the existing object
       * to which this PDU pertains was found and hence all 
       * objects in the obsol_list are obsolete and should be
       * cleaned, deregistered and freed
       *
       * If exist_obj is NULL, then there is infact NO existing object for 
       * the received PDU. As such, no objects are obsoleted at this 
       * stage. Instead, a new object is added and fabricated.
       */

      if (exist_obj)
      {
        /* Obsolete objects */
        for (i_list_move_head(obsol_list); (exist_obj=i_list_restore(obsol_list))!=NULL; i_list_move_next(obsol_list))
        {
          int dereg = 1;

          /* Clean the object */
          if (fact->cleanfunc)
          { 
            
            num = fact->cleanfunc (self, fact->cnt, exist_obj);
            if (num == 1)
            {
              /* If the cleanfunc returns 1, do not dereg/free the object */
              dereg = 0;
            }
          }

          /* Deregister/Free */
          if (dereg == 1)
          {
            i_printf (2, "l_snmp_objfact_refresh_walkcb deregistering object %s", exist_obj->name_str);
            num = i_entity_deregister (self, ENTITY(exist_obj));
            if (num != 0)
            { 
              i_printf (1, "l_snmp_objfact_refresh_walkcb warning, failed to deregister obsolete %s %s",
                i_entity_typestr (exist_obj->ent_type), exist_obj->name_str);
            }

            i_entity_free (ENTITY(exist_obj));

            /* fact->obj_list->p MUST be at the first obsolete obj */
            i_list_delete (fact->obj_list);
          }

          /* After all the obsolete objects have been removed, the list->p
           * will be at the object to which this received PDU is for. Hence,
           * we move to the next item which is the item that should 
           * pertain to the next pdu received.
           */
          i_list_move_next (fact->obj_list);
        }

        /* The obj_list->p currently points to the
         * item to which this PDU refers to (as it matched
         * the name_str). As such, we now move to the next item
         * in anticipation that it will be the next PDU recvd
         */
        i_list_move_next (fact->obj_list);

        /* End of obsolete object handling */
      }
      else
      {
        /* New Object */
        obj = i_object_create (name_str, NULL);
        if (!obj)
        { 
          i_printf (1, "l_snmp_objfact_walkcb failed to create the i_object struct for an object in container %s", 
            fact->cnt->name_str);
          free (name_str);
          i_list_free (obsol_list);
          return 0;
        }
        obj->cnt = fact->cnt;
        obj->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

        /* Fabricate the object */
        if (fact->fabfunc)
        {
          size_t i;
          char *index_oidstr = NULL;

          /* Build the index_oidstr */
          for (i=0; i < (pdu->variables->name_length - fact->name_oid_len); i++)
          { 
            char *temp_str;
            if (index_oidstr)
            { 
              asprintf (&temp_str, "%s.%i", index_oidstr, pdu->variables->name[fact->name_oid_len+i]); 
              free (index_oidstr);
              index_oidstr = temp_str;
            }
            else
            { asprintf (&index_oidstr, "%i", pdu->variables->name[fact->name_oid_len+i]); }
          }

          /* Call the fabfunc to fabricate */
          num = fact->fabfunc (self, fact->cnt, obj, pdu, index_oidstr, fact->passdata);
          if (index_oidstr) free (index_oidstr);
          if (num == 0)
          {
            /* Object fabricated. Store the object into factory
             * object list such that it is infront of the current
             * obj_list->p item. The obj_list->p MUST be left at 
             * the current position. The assumption is, that next time 
             * this walkcb is called, it will likely be for that item
             *
             * Also enqueue it into the unreg_list
             * of items to be registered.
             */
            p = fact->obj_list->p;
            i_list_move_prev (fact->obj_list);
            i_list_store (fact->obj_list, obj);
            fact->obj_list->p = p;
            i_list_enqueue (fact->unreg_list, obj);

            /* Parse desc_str */
            if (obj->desc_str)
            { i_desc_parse (obj->desc_str); }
          }
          else
          { 
            /* Fabricate function returned -1
             * Object is discarded 
             */
            i_entity_free (ENTITY(obj));
            obj = NULL;
          }
        }

        /* WARNING: At this point, obj may be NULL
         *          if it has been discarded by
         *          the fabrication function.
         */

        if (obj)
        {
          /* Evaluate apprules for all triggersets */
          i_triggerset_evalapprules_allsets (self, obj);

          /* Evaluate recrules for all metrics */
          l_record_eval_recrules_obj (self, obj);
        }

        /* End of New Object processing */
      }

      /* Free obsol_list */
      i_list_free (obsol_list);

      /* End of processing a new or obsoleted objects */
    }
    else
    {
      /* Current obj_list->p matches PDU. Move to next obj */
      i_list_move_next (fact->obj_list);
    }

    free (name_str);

    /* End of PDU processing */
  }
  else
  {
    /* No PDU - End of Walk */
    fact->walkid = 0;

    /* Check/Report error */
    if (session->error != SNMP_ERROR_NOERROR)
    {
      i_printf (2, "l_snmp_objfact_walkcb %s occurred whilst walking the %s OID for container %s", 
        l_snmp_session_error_str (session), fact->name_oid_str, fact->cnt->name_str);

      /* Set the factory's object refresh to retry interval 
       * if it is not already set accordingly
       */
      if (fact->obj->refresh_int_sec != fact->retry_int_sec)
      {
        num = l_snmp_objfact_retryrefcfg (self, fact);
        if (num != 0)
        {
          i_printf (1, "l_snmp_objfact_walkcb warning, failed to set factory refresh timer to retry interval for container %s",
            fact->cnt->name_str);
        }
      }
      
      /* Free new (unregistered) objects and remove from obj_list */
      for (i_list_move_head(fact->unreg_list); (obj=i_list_restore(fact->unreg_list))!=NULL; i_list_move_next(fact->unreg_list))
      { 
        /* Remove from obj_list */
        num = i_list_search (fact->obj_list, obj);
        if (num == 0)
        {
          /* Clean the object */
          if (fact->cleanfunc)
          { fact->cleanfunc (self, fact->cnt, obj); }
          /* Remove from obj_list */
          i_list_delete (fact->obj_list); 
        }

        /* Free and remove from unreg_list */
        i_entity_free (ENTITY(obj)); 
        i_list_delete (fact->unreg_list); 
      }
    }
    else
    {
      /* Perform end-of-list obsolecence 
       *
       * Any objects at, or after, the current list
       * position are in fact obsolete because no further
       * data was received regarding them. Normally, at
       * this point the list should be at the null list end
       * pointer. Otherwise, all remaining objects should deregistered
       * and free
       */
      i_object *exist_obj;
      for (; (exist_obj=i_list_restore(fact->obj_list))!=NULL; i_list_move_next(fact->obj_list))
      {
        int dereg = 1;
        
        /* Clean the object */
        if (fact->cleanfunc)
        {
          num = fact->cleanfunc (self, fact->cnt, exist_obj);
          if (num == 1)
          {
            /* If the cleanfunc returns 1, do not dereg/free the object */
            dereg = 0;
          }
        }

        /* Deregister/Free */
        if (dereg == 1)
        {
          num = i_entity_deregister (self, ENTITY(exist_obj));
          if (num != 0)
          {
            i_printf (1, "l_snmp_objfact_refresh_walkcb warning, failed to deregister obsolete %s %s",
              i_entity_typestr (exist_obj->ent_type), exist_obj->name_str);
          }

          i_entity_free (ENTITY(exist_obj));

          /* fact->obj_list->p MUST be at the first obsolete obj */
          i_list_delete (fact->obj_list);
        }
      }
      
      /* Register new (unregistered) objects */
      for (i_list_move_head(fact->unreg_list); (obj=i_list_restore(fact->unreg_list))!=NULL; i_list_move_next(fact->unreg_list))
      { i_entity_register (self, ENTITY(fact->cnt), ENTITY(obj)); i_list_delete (fact->unreg_list); }

      /* Set the factory's object refresh to the normal interval 
       * if it is not already set accordingly
       */
      if (fact->obj->refresh_int_sec != fact->refresh_int_sec)
      {
        num = l_snmp_objfact_normalrefcfg (self, fact);
        if (num != 0)
        {
          i_printf (1, "l_snmp_objfact_walkcb warning, failed to set factory refresh timer to normal interval for container %s",
            fact->cnt->name_str);
        }
      } 
    }

    /* Call the control */
    if (fact->ctrlfunc)
    { fact->ctrlfunc (self, fact->cnt, session->error, fact->passdata); }

    /* Terminate the refresh operation */
    fact->obj->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(fact->obj));

    if (fact->first_refresh && fact->enable_dupname_handling)
    {
      /* This was the first refresh and dupname handling had to be enabled
       * We must now kick off another refresh because this one would not
       * have yielded any objects
       */
       fact->first_refresh = 0;
       i_entity_refresh (self, ENTITY(fact->obj), REFFLAG_AUTO, NULL, NULL);
    }
 
    /* Set the first refresh flag */
    fact->first_refresh = 0;
  }

  return 0;
}

