typedef struct l_snmp_hrcpu_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  struct i_metric_s *load_pc;

} l_snmp_hrcpu_item;

/* snmp_hrcpu.c */

struct i_container_s* i_snmp_hrcpu_cnt ();
int l_snmp_hrcpu_enable (i_resource *self);
int l_snmp_hrcpu_disable (i_resource *self);

/* snmp_hrcpu_item.c */

l_snmp_hrcpu_item* l_snmp_hrcpu_item_create ();
void l_snmp_hrcpu_item_free (void *itemptr);

/* snmp_hrcpu_objfact.c */

int l_snmp_hrcpu_objfact_fab (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata);
int l_snmp_hrcpu_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int l_snmp_hrcpu_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* snmp_hrcpu_cntform.c */

int l_snmp_hrcpu_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* snmp_hrcpu_objform.c */

int l_snmp_hrcpu_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int l_snmp_hrcpu_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

