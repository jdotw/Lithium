typedef struct l_snmp_swrun_s
{
  struct i_object_s *obj;
  unsigned long index;

  struct i_metric_s *procname;
  struct i_metric_s *path;
  struct i_metric_s *params;
  struct i_metric_s *type;
  struct i_metric_s *status;
  struct i_metric_s *cpu_csec;
  struct i_metric_s *cpu_pc;
  struct i_metric_s *mem;

} l_snmp_swrun;

/* snmp_swrun.c */

struct i_container_s* l_snmp_swrun_cnt ();
int l_snmp_swrun_enable (i_resource *self);
int l_snmp_swrun_disable (i_resource *self);
l_snmp_swrun* l_snmp_swrun_create ();
void l_snmp_swrun_free (void *swrunptr);

/* snmp_swrun_objfact.c */

int l_snmp_swrun_objfact_fab ();
int l_snmp_swrun_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int l_snmp_swrun_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* snmp_swrun_cntform.c */

int l_snmp_swrun_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* snmp_swrun_refcb.c */

int l_snmp_swrun_cnt_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);

