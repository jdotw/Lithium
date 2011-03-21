typedef struct l_xsnmp_raidset_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  struct i_metric_s *type;
  struct i_metric_s *size;
  struct i_metric_s *unused;
  struct i_metric_s *comments;

} l_xsnmp_raidset_item;

/* xsnmp_raidset.c */
struct i_container_s* i_xsnmp_raidset_cnt ();
int l_xsnmp_raidset_enable (i_resource *self);
l_xsnmp_raidset_item* l_xsnmp_raidset_item_create ();
void l_xsnmp_raidset_item_free (void *itemptr);

/* xsnmp_raidset_objfact.c */
int l_xsnmp_raidset_objfact_fab (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata);
int l_xsnmp_raidset_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int l_xsnmp_raidset_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);


