typedef struct l_xsnmp_raiddrive_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  struct i_metric_s *raidset;
  struct i_metric_s *size;
  struct i_metric_s *status;
  struct i_metric_s *status_message;

} l_xsnmp_raiddrive_item;

/* xsnmp_raiddrive.c */
struct i_container_s* i_xsnmp_raiddrive_cnt ();
int l_xsnmp_raiddrive_enable (i_resource *self);
l_xsnmp_raiddrive_item* l_xsnmp_raiddrive_item_create ();
void l_xsnmp_raiddrive_item_free (void *itemptr);

/* xsnmp_raiddrive_objfact.c */
int l_xsnmp_raiddrive_objfact_fab (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata);
int l_xsnmp_raiddrive_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int l_xsnmp_raiddrive_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);


