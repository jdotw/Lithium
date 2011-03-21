#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libxml/parser.h>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/opstate.h>
#include <induction/colour.h>
#include <induction/auth.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/timer.h>
#include <induction/hierarchy.h>
#include <induction/xml.h>

#include "restart.h"
#include "lic.h"

typedef struct l_lic_activation_req_s
{
  char *data;
  size_t datasize;
} l_lic_activation_req;

int form_lic_add (i_resource *self, i_form_reqdata *reqdata)
{
  /* Authentication checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN)
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_lic_add failed to create form"); return -1; }

  /* Start frame */
  i_form_frame_start (reqdata->form_out, "add_key", "Add License Key");

  /* Key */
  i_form_textarea_add (reqdata->form_out, "key", "License Key", NULL);
  i_form_string_add (reqdata->form_out, "note", "Note", "License key must be entered exactly as you received it.");
  i_form_string_add (reqdata->form_out, "note", "Note", "License/Entitlement changes may not take effect until the customer process is restarted");

  /* End frame */
  i_form_frame_end (reqdata->form_out, "add_key");

  return 1;
}

int form_lic_add_submit (i_resource *self, i_form_reqdata *reqdata)
{
  i_form_item_option *key_opt;

  /* Auth checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN)
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_lic_add_submit unable to create form"); return -1; }

  /* Fields */
  key_opt = i_form_get_value_for_item (reqdata->form_in, "key");
  if (!key_opt || !key_opt->data) { i_form_string_add (reqdata->form_out, "error", "Error", "Key not found"); return 1; }

  /* Check type of key */
  if (strlen((char *)key_opt->data) > 30 && strlen((char *)key_opt->data) < 50)
  {
    /* eKey that requires activation */
    i_form_set_submit (reqdata->form_out, 1);
    i_form_option_add (reqdata->form_out, FORM_OPTION_SUBMIT_NAME, "lic_activate", strlen("lic_activate")+1);

    /* Add fields */
    i_form_frame_start (reqdata->form_out, "activate", "License Key Activation"); 
  
    i_form_string_add (reqdata->form_out, "msg", "Registration", "The license key you have entered requires Activation before it can be used. Please fill in the values below and then click Submit to Activate your license.");
    i_form_spacer_add (reqdata->form_out);

    i_form_entry_add (reqdata->form_out, "firstname", "First Name", NULL);
    i_form_entry_add (reqdata->form_out, "lastname", "Last Name", NULL);
    i_form_entry_add (reqdata->form_out, "company", "Company", NULL);
    i_form_entry_add (reqdata->form_out, "email", "Email Address", NULL);
    i_form_hidden_add (reqdata->form_out, "ekey", key_opt->data);

    i_form_frame_end (reqdata->form_out, "activate");

    return 1;
  }
  else
  {
    /* Signed key */

    /* Validate license key */
    l_lic_key *key;
    key = l_lic_validate_key (self, (char *)key_opt->data);
    if (!key || key->status != KEY_VALID)
    {
      /* Invalid KEY */
      i_form_frame_start (reqdata->form_out, "lic_add", "License Key Error");
      switch (key->status)
      {
        case KEY_INVALID:
          i_form_string_add (reqdata->form_out, "error", "Error", "Key format is invalid"); 
          break;
        case KEY_INVALID_CUSTOMER:
          i_form_string_add (reqdata->form_out, "error", "Error", "The key entered is not valid for this Customer. Please contact support@lithiumcorp.com to have this license key re-issued."); 
          break;
        case KEY_INVALID_EXPIRED:
          i_form_string_add (reqdata->form_out, "error", "Error", "Key has expired."); 
          break;
        case KEY_INVALID_WRONGTYPE:
          i_form_string_add (reqdata->form_out, "error", "Error", "Key is of the wrong type for this deployment of Lithium"); 
          break;
        default:
          i_form_string_add (reqdata->form_out, "error", "Error", "Key is not valid"); 
          break;
      }
      i_form_frame_end (reqdata->form_out, "lic_add");
      return 1;
    }
    
    /* Add license key */
    l_lic_key_add (self, (char *)key_opt->data);
  }

  /* Return license main */
  return form_lic_main (self, reqdata);
}

int form_lic_activate_submit (i_resource *self, i_form_reqdata *reqdata)
{
  i_form_item_option *firstname_opt;
  i_form_item_option *lastname_opt;
  i_form_item_option *company_opt;
  i_form_item_option *email_opt;
  i_form_item_option *ekey_opt;

  /* Auth checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }
  
  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);

  /* Values */
  firstname_opt = i_form_get_value_for_item (reqdata->form_in, "firstname");
  if (!firstname_opt || !firstname_opt->data)
  { i_form_string_add (reqdata->form_out, "error", "Error", "First name not found in form"); return 1; }
  lastname_opt = i_form_get_value_for_item (reqdata->form_in, "lastname");
  if (!lastname_opt || !lastname_opt->data)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Last name not found in form"); return 1; }
  company_opt = i_form_get_value_for_item (reqdata->form_in, "company");
  if (!company_opt || !company_opt->data)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Company not found in form"); return 1; }
  email_opt = i_form_get_value_for_item (reqdata->form_in, "email");
  if (!email_opt || !email_opt->data)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Email not found in form"); return 1; }
  ekey_opt = i_form_get_value_for_item (reqdata->form_in, "ekey");
  if (!ekey_opt || !ekey_opt->data)
  { i_form_string_add (reqdata->form_out, "error", "Error", "eKey not found in form"); return 1; }

  /* Init Curl */
  CURL *handle = curl_easy_init ();

  /* Escaped */
  char hostbuf[256];
  gethostname (hostbuf, 255);
  char *firstname_esc = curl_escape ((char *)firstname_opt->data, 0);
  char *lastname_esc = curl_escape ((char *)lastname_opt->data, 0);
  char *company_esc = curl_escape ((char *)company_opt->data, 0);
  char *email_esc = curl_escape ((char *)email_opt->data, 0);
  char *host_esc = curl_escape (hostbuf, 0);
  char *custname_esc = curl_escape (self->hierarchy->cust->name_str, 0);
  char *ekey_esc = curl_escape ((char *)ekey_opt->data, 0);

  /* URL */
  char *url_str;
  asprintf (&url_str, "https://secure.lithiumcorp.com.au/vince/activate_license.php?firstname=%s&lastname=%s&company=%s&email=%s&custname=%s&host=%s&key=%s",
    firstname_esc, lastname_esc, company_esc, email_esc, custname_esc, host_esc, ekey_esc);

  /* Setup req data */
  l_lic_activation_req req;
  req.data = NULL;
  req.datasize = 0;

  /* Set options */    
  curl_easy_setopt (handle, CURLOPT_URL, url_str);
  curl_easy_setopt (handle, CURLOPT_SSL_VERIFYPEER, 0);
  curl_easy_setopt (handle, CURLOPT_SSL_VERIFYHOST, 0);
  curl_easy_setopt (handle, CURLOPT_CONNECTTIMEOUT, 6);
  curl_easy_setopt (handle, CURLOPT_TIMEOUT, 10);
  curl_easy_setopt (handle, CURLOPT_FRESH_CONNECT, 1);
  curl_easy_setopt (handle, CURLOPT_FORBID_REUSE, 1);
  curl_easy_setopt (handle, CURLOPT_WRITEFUNCTION, l_lic_curlcb);
  curl_easy_setopt (handle, CURLOPT_WRITEDATA, &req);

  /* Start HTML frame */
  i_form_frame_start (reqdata->form_out, "activation", "Lithium License Activation");

  /* Perform */
  CURLcode success = curl_easy_perform(handle);
  if (success != 0 || req.datasize < 1 || !req.data) 
  {
    i_form_string_add (reqdata->form_out, "msg", "ERROR", "Failed to contact License Activation Server");
    i_form_string_add (reqdata->form_out, "msg", "", "If this problem persists, please email support@lithiumcorp.com");
    return 1;
  }

  /* Parse XML */
  int result = 0;
  char *message_str = NULL;
  char *skey_str = NULL;
  i_xml *xml = i_xml_struct (req.data, req.datasize, "activation");
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;
  root_node = xmlDocGetRootElement (xml->doc);
  for (node = root_node->children; node; node = node->next)
  {
    char *str;

    str = (char *) xmlNodeListGetString (xml->doc, node->xmlChildrenNode, 1);

    if (!strcmp((char *)node->name, "result") && str) result = atoi(str);
    else if (!strcmp((char *)node->name, "message") && str) message_str = strdup(str);
    else if (!strcmp((char *)node->name, "key") && str) skey_str = strdup(str);

    xmlFree (str);
  }

  /* Check result */
  if (result != 1)
  {
    if (message_str) i_form_string_add (reqdata->form_out, "msg", "ERROR", message_str);
    else i_form_string_add (reqdata->form_out, "msg", "ERROR", "License key activation failed.");
    i_form_string_add (reqdata->form_out, "msg", "", "Contact support@lithiumcorp.com for assistance.");
    return 1;
  }

  /* Add Key */
  l_lic_key_add (self, skey_str);
  
  /* Report Status */
  i_form_string_add (reqdata->form_out, "msg", "Success", "License activated successfully. Thank you.");
  i_form_frame_end (reqdata->form_out, "activation");

  return 1;
}

size_t l_lic_curlcb (void *buffer, size_t size, size_t nmemb, void *userp)
{
  l_lic_activation_req *req = userp;
  size_t realsize = size * nmemb;

  if (req->data)
  { req->data = realloc (req->data, req->datasize + realsize); }
  else
  { req->data = malloc (realsize); }

  memcpy (req->data + req->datasize, buffer, realsize);
  req->datasize += realsize;

  return realsize;
}


