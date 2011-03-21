typedef struct l_snmp_users_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  struct i_metric_s *user_count;

} l_snmp_users_item;

/* snmp_users.c */

struct i_container_s* l_snmp_users_cnt ();
int l_snmp_users_enable (i_resource *self);
int l_snmp_users_disable (i_resource *self);
struct i_object_s* l_snmp_users_masterobj ();

/* snmp_users_item.c */

l_snmp_users_item* l_snmp_users_item_create ();
void l_snmp_users_item_free (void *itemptr);

/* snmp_users_cntform.c */

int l_snmp_users_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* snmp_users_objform.c */

int l_snmp_users_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int l_snmp_users_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

