/* Real Server Item */

typedef struct v_rserv_item_s
{
  /* rserv variables */
  struct i_object_s *obj;
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
  struct i_metric_s *maxconn;
  struct i_metric_s *connretainpc;
  struct i_metric_s *weight;
  struct i_metric_cgraph_s *tput_cg;

  /* rport variables */
  struct i_container_s *rport_cnt;
  struct l_snmp_objfact_s *rport_objfact;
} v_rserv_item;

/* rserv.c */
struct i_container_s* v_rserv_cnt ();
int v_rserv_enable (i_resource *self);
int v_rserv_disable (i_resource *self);

/* rserv_item.c */
v_rserv_item* v_rserv_item_create ();
void v_rserv_item_free (void *itemptr);

/* rserv_objfact.c */
int v_rserv_objfact_fab ();
int v_rserv_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_rserv_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* rserv_cntform.c */
int v_rserv_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
struct i_form_item_s* v_rserv_cntform_servlist (i_resource *self, struct i_form_reqdata_s *reqdata, struct i_list_s *item_list, struct i_form_item_s *table);

/* rserv_objform.c */
int v_rserv_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
int v_rserv_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata);


