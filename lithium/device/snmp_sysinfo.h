typedef struct l_snmp_sysinfo_item_s
{
  struct i_object_s *obj;

  struct i_metric_s *uptime;
  struct i_metric_s *descr;
  struct i_metric_s *contact;
  struct i_metric_s *name;
  struct i_metric_s *location;
  struct i_metric_s *services;

} l_snmp_sysinfo_item;

/* snmp_sysinfo.c */

int l_snmp_sysinfo_enabled ();
struct i_container_s* l_snmp_sysinfo_cnt ();
struct i_object_s* l_snmp_sysinfo_obj ();
int l_snmp_sysinfo_enable (i_resource *self);
int l_snmp_sysinfo_disable (i_resource *self);

/* snmp_sysinfo_refcb.c */
int l_snmp_sysinfo_uptime_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);
int l_snmp_sysinfo_descr_refcb (i_resource *self, i_entity *ent, void *passdata);

/* snmp_sysinfo_item.c */
l_snmp_sysinfo_item* l_snmp_sysinfo_item_create ();
void l_snmp_sysinfo_item_free (void *itemptr);

/* snmp_sysinfo_cntform.c */
int l_snmp_sysinfo_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* snmp_sysinfo_objform.c */
int l_snmp_sysinfo_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

