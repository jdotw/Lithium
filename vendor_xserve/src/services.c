#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>
#include <induction/name.h>
#include <induction/str.h>
#include <lithium/snmp.h>
#include <lithium/record.h>

#include "osx_server.h"
#include "data.h"
#include "services.h"

/* OS X / Xserve Services Info */

static i_container *static_cnt = NULL;

/* Variable Retrieval */

i_container* v_services_cnt ()
{ return static_cnt; }

v_services_item* v_services_get (char *name_str)
{
  v_services_item *item;
  for (i_list_move_head(static_cnt->item_list); (item=i_list_restore(static_cnt->item_list))!=NULL; i_list_move_next(static_cnt->item_list))
  {
    if (strcmp(item->obj->name_str, name_str) == 0)
    { return item; }
  }

  return NULL;
}

/* Enable / Disable */

int v_services_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("xservices", "OS X Services");
  if (!static_cnt)
  { i_printf (1, "v_services_enable failed to create container"); v_services_disable (self); return -1; }
  static_cnt->mainform_func = v_services_cntform;
  static_cnt->sumform_func = v_services_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_services_enable failed to register container"); v_services_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_services_enable failed to load and apply container refresh config"); v_services_disable (self); return -1; }

  /*
   * Triggers
   */
  i_triggerset *tset;
  
  tset = i_triggerset_create ("state", "State", "state");
  i_triggerset_addtrg (self, tset, "stopped", "Stopped", VALTYPE_STRING, TRGTYPE_NOTEQUAL, 0, "RUNNING", 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  tset->default_applyflag = 0;
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("volume", "Volume", "volume");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_GAUGE, TRGTYPE_GT, 100, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  tset->default_applyflag = 0;
  i_triggerset_assign (self, static_cnt, tset);

  /* 
   * Item and objects 
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_services_enable failed to create item_list"); v_services_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* AFP */
  v_services_create (self, "afp", "AFP", "Current Connections", NULL, 1);

  /* App Server */
  v_services_create (self, "appserver", "Application Server", NULL, NULL, 0);

  /* DHCP */
  v_services_create (self, "dhcp", "DHCP", "Active Leases", NULL, 0);

  /* Directory Server */
  v_services_create (self, "dirserv", "Directory Server", NULL, NULL, 0);

  /* DNS */
  v_services_create (self, "dns", "DNS", NULL, NULL, 0);

  /* FTP */
  v_services_create (self, "ftp", "FTP", "Current Connections", NULL, 0);

  /* Firewall */
  v_services_create (self, "ipfilter", "Firewall", NULL, NULL, 0);

  /* iChat/Jabber */
  v_services_create (self, "ichat", "iChat", "Current Connections", NULL, 0);

  /* Mail */
  v_services_create (self, "mail", "Mail", "Current Connections", NULL, 0);

  /* NAT */
  v_services_create (self, "nat", "NAT", "Active Translations", NULL, 0);

  /* Netboot */
  v_services_create (self, "netboot", "NetBoot", NULL, NULL, 0);

  /* NFS */
  v_services_create (self, "nfs", "NFS", NULL, NULL, 0);

  /* Print */
  v_services_create (self, "print", "Print", "Current Jobs", NULL, 0);

  /* Quick Time SS */
  v_services_create (self, "qtss", "QuickTime Streaming", "Current Connections", NULL, 1);

  /* SW Update */
  v_services_create (self, "swupdate", "Software Update", "Mirrored Packages", NULL, 0);

  /* VPN */
  v_services_create (self, "vpn", "VPN", "Current Connections", NULL, 0);

  /* Web */
  v_services_create (self, "web", "Web", "Current Connections", "Requests per Second", 1);

  /* Web Objects */
  v_services_create (self, "webobjects", "Web Objects", NULL, NULL, 0);

  /* Windows / SMB */
  v_services_create (self, "smb", "Windows", "Current Connections", NULL, 1);

  /* Xsan */
  v_services_create (self, "xsan", "Xsan", NULL, NULL, 1);

  /*
   * Create Data Metrics 
   */

  v_data_item *dataitem = v_data_static_item();

  /* Standard refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* AFP Data */
  dataitem->afp = i_metric_create ("afp", "AFP", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->afp, 0, "Invalid");
  i_metric_enumstr_add (dataitem->afp, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->afp));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->afp), &defrefconfig);
  dataitem->afp->refresh_func = v_data_afp_state_refresh;
  
  /* AppServer Data */
  dataitem->appserver = i_metric_create ("appserver", "appserver", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->appserver, 0, "Invalid");
  i_metric_enumstr_add (dataitem->appserver, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->appserver));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->appserver), &defrefconfig);
  dataitem->appserver->refresh_func = v_data_appserver_state_refresh;

  /* DHCP Data */
  dataitem->dhcp = i_metric_create ("dhcp", "dhcp", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->dhcp, 0, "Invalid");
  i_metric_enumstr_add (dataitem->dhcp, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->dhcp));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->dhcp), &defrefconfig);
  dataitem->dhcp->refresh_func = v_data_dhcp_state_refresh;
  
  /* Open Directory Data */
  dataitem->dirserv = i_metric_create ("dirserv", "dirserv", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->dirserv, 0, "Invalid");
  i_metric_enumstr_add (dataitem->dirserv, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->dirserv));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->dirserv), &defrefconfig);
  dataitem->dirserv->refresh_func = v_data_dirserv_state_refresh;

  /* DNS */
  dataitem->dns = i_metric_create ("dns", "dns", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->dns, 0, "Invalid");
  i_metric_enumstr_add (dataitem->dns, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->dns));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->dns), &defrefconfig);
  dataitem->dns->refresh_func = v_data_dns_state_refresh;

  /* FTP */
  dataitem->ftp = i_metric_create ("ftp", "ftp", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->ftp, 0, "Invalid");
  i_metric_enumstr_add (dataitem->ftp, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->ftp));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->ftp), &defrefconfig);
  dataitem->ftp->refresh_func = v_data_ftp_state_refresh;

  /* IP Filter / Firewall */
  dataitem->ipfilter = i_metric_create ("ipfilter", "ipfilter", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->ipfilter, 0, "Invalid");
  i_metric_enumstr_add (dataitem->ipfilter, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->ipfilter));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->ipfilter), &defrefconfig);
  dataitem->ipfilter->refresh_func = v_data_ipfilter_state_refresh;

  /* iChat */
  dataitem->ichat = i_metric_create ("ichat", "ichat", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->ichat, 0, "Invalid");
  i_metric_enumstr_add (dataitem->ichat, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->ichat));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->ichat), &defrefconfig);
  dataitem->ichat->refresh_func = v_data_ichat_state_refresh;

  /* Mail */
  dataitem->mail = i_metric_create ("mail", "mail", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->mail, 0, "Invalid");
  i_metric_enumstr_add (dataitem->mail, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->mail));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->mail), &defrefconfig);
  dataitem->mail->refresh_func = v_data_mail_state_refresh;

  /* NAT */
  dataitem->nat = i_metric_create ("nat", "nat", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->nat, 0, "Invalid");
  i_metric_enumstr_add (dataitem->nat, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->nat));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->nat), &defrefconfig);
  dataitem->nat->refresh_func = v_data_nat_state_refresh;

  /* Netboot */
  dataitem->netboot = i_metric_create ("netboot", "netboot", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->netboot, 0, "Invalid");
  i_metric_enumstr_add (dataitem->netboot, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->netboot));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->netboot), &defrefconfig);
  dataitem->netboot->refresh_func = v_data_netboot_state_refresh;

  /* NFS */
  dataitem->nfs = i_metric_create ("nfs", "nfs", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->nfs, 0, "Invalid");
  i_metric_enumstr_add (dataitem->nfs, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->nfs));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->nfs), &defrefconfig);
  dataitem->nfs->refresh_func = v_data_nfs_state_refresh;

  /* Print */
  dataitem->print = i_metric_create ("print", "print", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->print, 0, "Invalid");
  i_metric_enumstr_add (dataitem->print, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->print));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->print), &defrefconfig);
  dataitem->print->refresh_func = v_data_print_state_refresh;

  /* QTSS */
  dataitem->qtss = i_metric_create ("qtss", "qtss", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->qtss, 0, "Invalid");
  i_metric_enumstr_add (dataitem->qtss, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->qtss));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->qtss), &defrefconfig);
  dataitem->qtss->refresh_func = v_data_qtss_state_refresh;

  /* smb */
  dataitem->smb = i_metric_create ("smb", "smb", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->smb, 0, "Invalid");
  i_metric_enumstr_add (dataitem->smb, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->smb));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->smb), &defrefconfig);
  dataitem->smb->refresh_func = v_data_smb_state_refresh;

  /* swupdate */
  dataitem->swupdate = i_metric_create ("swupdate", "swupdate", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->swupdate, 0, "Invalid");
  i_metric_enumstr_add (dataitem->swupdate, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->swupdate));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->swupdate), &defrefconfig);
  dataitem->swupdate->refresh_func = v_data_swupdate_state_refresh;

  /* vpn */
  dataitem->vpn = i_metric_create ("vpn", "vpn", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->vpn, 0, "Invalid");
  i_metric_enumstr_add (dataitem->vpn, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->vpn));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->vpn), &defrefconfig);
  dataitem->vpn->refresh_func = v_data_vpn_state_refresh;

  /* web */
  dataitem->web = i_metric_create ("web", "web", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->web, 0, "Invalid");
  i_metric_enumstr_add (dataitem->web, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->web));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->web), &defrefconfig);
  dataitem->web->refresh_func = v_data_web_state_refresh;

  /* webobjects */
  dataitem->webobjects = i_metric_create ("webobjects", "webobjects", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->webobjects, 0, "Invalid");
  i_metric_enumstr_add (dataitem->webobjects, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->webobjects));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->webobjects), &defrefconfig);
  dataitem->webobjects->refresh_func = v_data_webobjects_state_refresh;

  /* xsan */
  dataitem->xsan = i_metric_create ("xsan", "Xsan", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->xsan, 0, "Invalid");
  i_metric_enumstr_add (dataitem->xsan, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->xsan));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->xsan), &defrefconfig);
  dataitem->xsan->refresh_func = v_data_xsan_state_refresh;

  return 0;
}

v_services_item* v_services_create (i_resource *self, char *name_str, char *desc_str, char *volume_desc, char *rate_desc, int tput_flag)
{
  v_services_item *item;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Create object */
  obj = i_object_create (name_str, desc_str);
  obj->mainform_func = v_services_objform;
  obj->histform_func = v_services_objform_hist;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Create item */
  item = v_services_item_create ();
  item->obj = obj;
  obj->itemptr = item;
  i_list_enqueue (static_cnt->item_list, item);

  /*
   * Create Metrics 
   */

  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  item->state = i_metric_create ("state", "State", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->state));
  i_entity_refresh_config_apply (self, ENTITY(item->state), &refconfig);

  item->start_time = i_metric_create ("start_time", "Start Time", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->start_time));
  i_entity_refresh_config_apply (self, ENTITY(item->start_time), &refconfig);

  if (tput_flag == 1)
  {
    item->throughput = i_metric_create ("throughput", "Throughput", METRIC_GAUGE);
    item->throughput->record_method = RECMETHOD_RRD;
    item->throughput->record_defaultflag = 1;
    i_entity_register (self, ENTITY(obj), ENTITY(item->throughput));
    i_entity_refresh_config_apply (self, ENTITY(item->throughput), &refconfig);
  }

  if (volume_desc)
  {
    item->volume = i_metric_create ("volume", volume_desc, METRIC_GAUGE);
    item->volume->record_method = RECMETHOD_RRD;
    item->volume->record_defaultflag = 1;
    i_entity_register (self, ENTITY(obj), ENTITY(item->volume));
    i_entity_refresh_config_apply (self, ENTITY(item->volume), &refconfig);
  }

  if (rate_desc)
  {
    item->rate = i_metric_create ("rate", rate_desc, METRIC_FLOAT);
    item->rate->record_method = RECMETHOD_RRD;
    item->rate->record_defaultflag = 1;
    i_entity_register (self, ENTITY(obj), ENTITY(item->rate));
    i_entity_refresh_config_apply (self, ENTITY(item->rate), &refconfig);
  }

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  return item;
}

int v_services_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

