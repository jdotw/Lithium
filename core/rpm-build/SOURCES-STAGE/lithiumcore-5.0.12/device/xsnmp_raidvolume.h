typedef struct l_xsnmp_raidvolume_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  struct i_metric_s *raidset;
  struct i_metric_s *type;
  struct i_metric_s *size;
  struct i_metric_s *status;
  struct i_metric_s *status_message;
  struct i_metric_s *comments;

} l_xsnmp_raidvolume_item;

/* xsnmp_raidvolume.c */
struct i_container_s* i_xsnmp_raidvolume_cnt ();
int l_xsnmp_raidvolume_enable (i_resource *self);
l_xsnmp_raidvolume_item* l_xsnmp_raidvolume_item_create ();
void l_xsnmp_raidvolume_item_free (void *itemptr);

/* xsnmp_raidvolume_objfact.c */
int l_xsnmp_raidvolume_objfact_fab (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata);
int l_xsnmp_raidvolume_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int l_xsnmp_raidvolume_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);


