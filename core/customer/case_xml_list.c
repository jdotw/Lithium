#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/entity_xml.h>
#include <induction/postgresql.h>
#include <induction/auth.h>
#include <induction/xml.h>
#include <induction/callback.h>

#include "case.h"
#include "case_xml.h"

int xml_case_list (i_resource *self, i_xml_request *req)
{
  char *id = NULL;
  char *state = NULL;
  char *startupper = NULL;
  char *startlower = NULL;
  char *endupper = NULL;
  char *endlower = NULL;
  char *hline = NULL;
  char *owner = NULL;
  char *requester = NULL;
  i_entity_descriptor *entdesc = NULL;
  i_callback *cb;
  l_case_xml_list_reqdata *reqdata;
  
  if (req->xml_in)
  {
    /* Search criteria specified */
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;

    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;

      /* Check for entity descriptor */
      if (!strcmp((char *)node->name, "entity_descriptor"))
      {
        entdesc = i_entity_descriptor_fromxml (req->xml_in, node);
        continue;
      }

      /* Retrieve and check str */
      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
      if (!str) continue;

      /* Interpret item */
      if (!strcmp((char *)node->name, "id") && str) id = strdup(str);
      else if (!strcmp((char *)node->name, "state") && str) state = strdup(str);
      else if (!strcmp((char *)node->name, "startupper") && str) startupper = strdup(str);
      else if (!strcmp((char *)node->name, "startlower") && str) startlower = strdup(str);
      else if (!strcmp((char *)node->name, "endupper") && str) endupper = strdup(str);
      else if (!strcmp((char *)node->name, "endlower") && str) endlower = strdup(str);
      else if (!strcmp((char *)node->name, "hline") && str) hline = strdup(str);
      else if (!strcmp((char *)node->name, "owner") && str) owner = strdup(str);
      else if (!strcmp((char *)node->name, "requester") && str) requester = strdup(str);

      /* Free str */
      if (str) xmlFree (str);
    }
  }
  else
  {
    /* Default to searching for open cases */
    state = strdup(CASE_STATE_OPEN_NUMSTR);
  }

  /* Create reqdata */
  reqdata = (l_case_xml_list_reqdata *) malloc (sizeof(l_case_xml_list_reqdata));
  memset (reqdata, 0, sizeof(l_case_xml_list_reqdata));
  reqdata->req = req;
  
  if (entdesc)
  {
    /* Entity criteria specified, load entities */
    char *enttype_str;
    asprintf (&enttype_str, "%i", entdesc->type);
    cb = l_case_entity_sql_list (self, id, state, enttype_str, entdesc->site_name, entdesc->dev_name, entdesc->cnt_name, entdesc->obj_name, entdesc->met_name, entdesc->trg_name, startlower, endupper, l_case_xml_list_entitycb, reqdata);
    free (enttype_str);
    i_entity_descriptor_free (entdesc);
  }
  else
  {
    /* No entity specified, load cases */
    cb = l_case_sql_list (self, id, state, startupper, startlower, endupper, endlower, hline, owner, requester, l_case_xml_list_casecb, reqdata);
  }

  /* Free criteria */
  if (id) free (id);
  if (state) free (state);
  if (startupper) free (startupper);
  if (startlower) free (startlower);
  if (endupper) free (endupper);
  if (endlower) free (endlower);
  if (hline) free (hline);
  if (owner) free (owner);
  if (requester) free (requester);

  /* Check that a callback was returned */
  if (!cb) return -1;

  return 0;
}

int l_case_xml_list_entitycb (i_resource *self, i_list *list, void *passdata)
{
  l_case *cas;
  i_list *idlist;
  i_callback *cb;
  l_case_xml_list_reqdata *reqdata = passdata;

  /* Create idlist */
  idlist = i_list_create ();
  i_list_set_destructor (idlist, free);
  
  /* Loop through cases */
  for (i_list_move_head(list); (cas=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    /* Add caseid to idlist */
    char *idstr;

    asprintf (&idstr, "%li", cas->id);
    i_list_enqueue (idlist, idstr);
  }

  /* Get cases by idlist from SQL */
  if (idlist->size > 0)
  {
    /* Store case list */
    reqdata->caselist = list;

    /* Retrieve ID list */
    cb = l_case_sql_list_idlist (self, idlist, l_case_xml_list_casecb, reqdata);
    i_list_free (idlist);
    if (!cb) 
    { i_printf (1, "l_case_xml_list_entitycb failed to call l_case_sql_list_idlist"); return -1; }

    return 0;   /* KEEP the list */
  }
  else
  {
    i_list_free (idlist);
    l_case_xml_list_casecb (self, NULL, reqdata);
    return -1;
  }

}

int l_case_xml_list_casecb (i_resource *self, i_list *list, void *passdata)
{
  i_xml *xml;
  l_case *cas;
  l_case *entitycase;
  xmlNodePtr root_node;
  l_case_xml_list_reqdata *reqdata = passdata;
  i_xml_request *req = reqdata->req;

  /* Add entities where applicable */
  if (reqdata->caselist && reqdata->caselist->size > 0)
  {
    /* Loop through cases */
    for (i_list_move_head(list); (cas=i_list_restore(list))!=NULL; i_list_move_next(list))
    {
      /* Loop through entity sets */
      for (i_list_move_head(reqdata->caselist); (entitycase=i_list_restore(reqdata->caselist))!=NULL; i_list_move_next(reqdata->caselist))
      {
        if (cas->id == entitycase->id)
        {
          /* Matching cases found. Transfer entities over, 
           * and then remove the case from the entity case list
           */
          cas->ent_list = entitycase->ent_list;
          entitycase->ent_list = NULL;
          i_list_delete (reqdata->caselist);
        }
      }
    }
  }
  
  /* Create XML */
  xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "case_list");
  xmlDocSetRootElement (xml->doc, root_node);

  /* Add cases */
  for (i_list_move_head(list); (cas=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    xmlNodePtr case_node;
    case_node = l_case_xml (cas);
    xmlAddChild (root_node, case_node);
  }

  /* Deliver */
  req->xml_out = xml;
  i_xml_deliver (self, req);

  /* Cleanup */
  if (reqdata->caselist) i_list_free (reqdata->caselist);
  free (reqdata);

  return -1;  /* Dont keep case list */
}
