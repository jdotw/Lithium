/* Real Server Item */

typedef struct v_rport_item_s
{
  /* rport variables */
  struct i_object_s *obj;

  struct i_metric_s *rservname;
  struct i_metric_s *ipaddr;
  struct i_metric_s *adminstate;
  struct i_metric_s *opstate;
  struct i_metric_s *reassigncount;
  struct i_metric_s *reassignps;
  struct i_metric_s *reassignlimit;
  struct i_metric_s *curconn;
  struct i_metric_s *curconn_delta;
  struct i_metric_s *conncount;
  struct i_metric_s *conncount_delta;
  struct i_metric_s *connps;
  struct i_metric_s *peakconn;
  struct i_metric_s *connretainpc;
  struct i_metric_cgraph_s *tput_cg;

} v_rport_item;

/* rport.c */
int v_rport_enable (i_resource *self, struct v_rserv_item_s *rserv, char *index_oidstr);
int v_rport_disable (i_resource *self, struct v_rserv_item_s *rserv);
int v_rport_cnt_validate (struct i_container_s *cnt);

/* rport_item.c */
v_rport_item* v_rport_item_create ();
void v_rport_item_free (void *itemptr);

/* rport_objfact.c */
int v_rport_objfact_fab ();
int v_rport_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_rport_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* rport_cntform.c */
int v_rport_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
struct i_form_item_s* v_rport_cntform_portlist (i_resource *self, struct i_form_reqdata_s *reqdata, struct i_list_s *item_list, struct i_form_item_s *table);

/* rport_objform.c */
int v_rport_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
int v_rport_objform_hist (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

