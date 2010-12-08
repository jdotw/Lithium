/* Real Server Item */

typedef struct v_vport_item_s
{
  /* vport variables */
  struct i_object_s *obj;

  struct i_metric_s *vservname;
  struct i_metric_s *ipaddr;
  struct i_metric_s *adminstate;
  struct i_metric_s *curconn;
  struct i_metric_s *curconn_delta;
  struct i_metric_s *conncount;
  struct i_metric_s *conncount_delta;
  struct i_metric_s *connps;
  struct i_metric_s *peakconn;
  struct i_metric_s *connretainpc;
  
  struct i_metric_s *weight;
  struct i_metric_cgraph_s *tput_cg;

} v_vport_item;

/* vport.c */
int v_vport_enable (i_resource *self, struct v_vserv_item_s *vserv, char *index_oidstr);
int v_vport_disable (i_resource *self, struct v_vserv_item_s *vserv);
int v_vport_cnt_validate (struct i_container_s *cnt);

/* vport_item.c */
v_vport_item* v_vport_item_create ();
void v_vport_item_free (void *itemptr);

/* vport_objfact.c */
int v_vport_objfact_fab ();
int v_vport_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_vport_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* vport_cntform.c */
int v_vport_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
struct i_form_item_s* v_vport_cntform_portlist (i_resource *self, struct i_form_reqdata_s *reqdata, struct i_list_s *item_list, struct i_form_item_s *table);

/* vport_objform.c */
int v_vport_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
int v_vport_objform_hist (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

