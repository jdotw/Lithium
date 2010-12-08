/* CPU Item */

typedef struct l_snmp_nscpu_item_s
{
  /* id variables */
  struct i_object_s *obj;

  /* cpu variables -- ticks */
  struct i_metric_s *user_ticks;
  struct i_metric_s *nice_ticks;
  struct i_metric_s *system_ticks;
  struct i_metric_s *idle_ticks;
  struct i_metric_s *wait_ticks;
  struct i_metric_s *kernel_ticks;
  struct i_metric_s *interrupt_ticks;

  /* cpu variables -- percent */
  struct i_metric_s *user;
  struct i_metric_s *nice;
  struct i_metric_s *system;
  struct i_metric_s *idle;
  struct i_metric_s *wait;
  struct i_metric_s *kernel;
  struct i_metric_s *interrupt;

  /* cgraph */
  struct i_metric_cgraph_s *cpu_cg;

} l_snmp_nscpu_item;

/* cpu.c */
struct i_container_s* l_snmp_nscpu_cnt ();
int l_snmp_nscpu_enable (i_resource *self);
int l_snmp_nscpu_disable (i_resource *self);

/* cpu_refcb.c */
int l_snmp_nscpu_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);

/* cpu_item.c */
l_snmp_nscpu_item* l_snmp_nscpu_item_create ();
void l_snmp_nscpu_item_free (void *itemptr);

/* cpu_cntform.c */
int l_snmp_nscpu_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* cpu_objform.c */
int l_snmp_nscpu_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
int l_snmp_nscpu_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

