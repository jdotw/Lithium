typedef struct v_xsanaffinity_item_s
{
  struct i_object_s *obj;
  int index;

  /* State */
  struct i_metric_s *bytes_total;
  struct i_metric_s *bytes_free;
  struct i_metric_s *bytes_used;
  struct i_metric_s *used_pc;

} v_xsanaffinity_item;

v_xsanaffinity_item* v_xsanaffinity_item_create ();
void v_xsanaffinity_item_free (void *itemptr);

int v_xsanaffinity_objfact_fab (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata);
int v_xsanaffinity_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_xsanaffinity_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);
