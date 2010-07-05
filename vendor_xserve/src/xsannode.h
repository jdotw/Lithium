typedef struct v_xsannode_item_s
{
  int index;
  struct i_container_s *cnt;
  struct i_object_s *obj;
  struct v_xsansp_item_s *sp_item;

  /* Config */
  struct i_metric_s *noderotationindex;
  struct i_metric_s *blockdev;            /* /dev/rdisk(etc) */
  struct i_metric_s *hostnumber;
  struct i_metric_s *hostinquiry;         /* Info label */
  struct i_metric_s *lun;                 
  struct i_metric_s *sectors;
  struct i_metric_s *maxsectors;
  struct i_metric_s *sectorsize;
  struct i_metric_s *wwn;                 /* 'Serial' from Xsnmp */
  struct i_metric_s *size;                /* In bytes */
  struct i_metric_s *unlabeled;
  struct i_metric_s *visible;

  int lun_registered;

} v_xsannode_item;

v_xsannode_item* v_xsannode_get (struct i_container_s *node_cnt, char *node_name);
v_xsannode_item* v_xsannode_get_global (char *node_name);
v_xsannode_item* v_xsannode_create (i_resource *self, struct v_xsansp_item_s *sp, char *node_name);

v_xsannode_item* v_xsannode_item_create ();
void v_xsannode_item_free (void *itemptr);

int v_xsannode_objfact_fab (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata);
int v_xsannode_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_xsannode_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);
