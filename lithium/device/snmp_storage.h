typedef struct l_snmp_storage_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  struct i_metric_s *type;
  struct i_metric_s *typeoid;
  struct i_metric_s *alloc_units;
  struct i_metric_s *size;
  struct i_metric_s *used;
  struct i_metric_s *used_pc;
  struct i_metric_s *free;
  struct i_metric_s *free_pc;
  struct i_metric_s *alloc_failures;
  struct i_metric_s *writeable;
  struct i_metric_s *removable;
  struct i_metric_s *smart_status;
  struct i_metric_s *smart_message;

  unsigned short tset_applied;
  
  /* From hrFSTable snmp_hrfilesys.c */
  struct i_metric_s *remote_mp;
  struct i_metric_s *access;
  struct i_metric_s *bootable;

  /* Triggers */
  int usedpc_trigger_applied;
  
} l_snmp_storage_item;

/* snmp_storage.c */

struct i_container_s* l_snmp_storage_cnt ();
int l_snmp_storage_enable (i_resource *self);
int l_snmp_storage_disable (i_resource *self);
void l_snmp_storage_monitor_memory_set (int value);
int l_snmp_storage_monitor_memory ();

/* snmp_storage_item.c */

l_snmp_storage_item* l_snmp_storage_item_create ();
void l_snmp_storage_item_free (void *itemptr);

/* snmp_storage_objfact.c */

void l_snmp_storage_apply_tset (i_resource *self, struct i_object_s *obj);
int l_snmp_storage_objfact_fab (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata);
int l_snmp_storage_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int l_snmp_storage_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* snmp_storage_cntform.c */

int l_snmp_storage_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* snmp_storage_objform.c */

int l_snmp_storage_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int l_snmp_storage_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* snmp_storage_refcb.c */
int l_snmp_storage_obj_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);

