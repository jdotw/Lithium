#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <libxml/parser.h>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/module.h>
#include <induction/auth.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/construct.h>
#include <induction/list.h>
#include <induction/configfile.h>
#include <induction/files.h>
#include <induction/vendor.h>
#include <induction/timer.h>
#include <induction/xml.h>

#include "lic.h"
#include "demorego.h"

typedef struct l_demorego_req_s
{
  char *data;
  size_t datasize;
} l_demorego_req;

int form_demorego_register (i_resource *self, i_form_reqdata *reqdata)
{
  i_form_item *item;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN) 
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_demorego_register failed to create form"); return -1; }

  i_form_frame_start (reqdata->form_out, "demorego", "FREE Lithium 30-Day Trial Registration"); 
  
  i_form_string_add (reqdata->form_out, "error", "Registration", "To receive a Free 30-Day Trial License for Lithium, simply fill out the form below and click 'Submit'. Your registration will be sent to LithiumCorp's registration server and a Free Trial license key will be downloaded and installed for you.");
  i_form_spacer_add (reqdata->form_out);

  i_form_entry_add (reqdata->form_out, "firstname", "First Name", NULL);
  i_form_entry_add (reqdata->form_out, "lastname", "Last Name", NULL);
  i_form_entry_add (reqdata->form_out, "company", "Company", NULL);
  i_form_entry_add (reqdata->form_out, "email", "Email Address", NULL);
  
  /* Vendor */
  item = i_form_dropdown_create ("licindex", "Network Size");
  i_form_dropdown_add_option (item, "0", "Please select one...", 1);
  i_form_dropdown_add_option (item, "1", "1-25 Devices", 0);
  i_form_dropdown_add_option (item, "2", "25-50 Devices", 0);
  i_form_dropdown_add_option (item, "3", "50-200 Devices", 0);
  i_form_dropdown_add_option (item, "4", "200+ Devices", 0);
  i_form_add_item (reqdata->form_out, item);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, "demorego");

  return 1;
}

int form_demorego_register_submit (i_resource *self, i_form_reqdata *reqdata)
{
  i_form_item_option *firstname_opt;
  i_form_item_option *lastname_opt;
  i_form_item_option *company_opt;
  i_form_item_option *email_opt;
  i_form_item_option *licindex_opt;

  /* Auth checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }
  
  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_device_add_submit unable to create form"); return -1; }

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
  licindex_opt = i_form_get_value_for_item (reqdata->form_in, "licindex");
  if (!licindex_opt || !licindex_opt->data)
  { i_form_string_add (reqdata->form_out, "error", "Error", "License index not found in form"); return 1; }

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
  char *licindex_esc = curl_escape ((char *)licindex_opt->data, 0);

  /* URL */
  char *url_str;
  asprintf (&url_str, "https://secure.lithiumcorp.com.au/vince/register_demo.php?firstname=%s&lastname=%s&company=%s&email=%s&licindex=%s&custname=%s&host=%s",
    firstname_esc, lastname_esc, company_esc, email_esc, licindex_esc, custname_esc, host_esc);

  /* Setup req data */
  l_demorego_req req;
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
  curl_easy_setopt (handle, CURLOPT_WRITEFUNCTION, l_demorego_curlcb);
  curl_easy_setopt (handle, CURLOPT_WRITEDATA, &req);

  /* Start HTML frame */
  i_form_frame_start (reqdata->form_out, "demorego", "Lithium 30-Day Free Trial Registration");

  /* Perform */
  CURLcode success = curl_easy_perform(handle);
  if (success != 0 || req.datasize < 1 || !req.data) 
  {
    i_form_string_add (reqdata->form_out, "msg", "ERROR", "Failed to contact Lithium Registration Server");
    i_form_string_add (reqdata->form_out, "msg", "", "If this problem persists, please email support@lithiumcorp.com");
    return 1;
  }

  /* Parse XML */
  int result = 0;
  char *message_str = NULL;
  char *skey_str = NULL;
  i_xml *xml = i_xml_struct (req.data, req.datasize, "registration");
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
    else i_form_string_add (reqdata->form_out, "msg", "ERROR", "Trial registration failed.");
    i_form_string_add (reqdata->form_out, "msg", "", "Contact support@lithiumcorp.com for assistance.");
    return 1;
  }

  /* Add Key */
  l_lic_key_add (self, skey_str);
  
  /* Report Status */
  i_form_string_add (reqdata->form_out, "msg", "Success", "Your Free Lithium 30-Day Trial License has been installed successfully. Thank you for registering.");
  i_form_frame_end (reqdata->form_out, "demorego");

  return 1;
}

size_t l_demorego_curlcb (void *buffer, size_t size, size_t nmemb, void *userp)
{
  l_demorego_req *req = userp;
  size_t realsize = size * nmemb;

  if (req->data)
  { req->data = realloc (req->data, req->datasize + realsize); }
  else
  { req->data = malloc (realsize); }

  memcpy (req->data + req->datasize, buffer, realsize);
  req->datasize += realsize;

  return realsize;
}
