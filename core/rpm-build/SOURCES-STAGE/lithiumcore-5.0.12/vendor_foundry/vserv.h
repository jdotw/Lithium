/* Real Server Item */

typedef struct v_vserv_item_s
{
  /* vserv variables */
  struct i_object_s *obj;
  struct i_metric_s *ipaddr;
  struct i_metric_s *adminstate;
  struct i_metric_s *conncount;
  struct i_metric_s *connps;
  struct i_metric_s *sdatype;
  struct i_metric_s *symstate;
  struct i_metric_s *symprio;

  /* vport variables */
  struct i_container_s *vport_cnt;
  struct l_snmp_objfact_s *vport_objfact;
    
} v_vserv_item;

/* vserv.c */
struct i_container_s* v_vserv_cnt ();
int v_vserv_enable (i_resource *self);
int v_vserv_disable (i_resource *self);

/* vserv_item.c */
v_vserv_item* v_vserv_item_create ();
void v_vserv_item_free (void *itemptr);

/* vserv_objfact.c */
int v_vserv_objfact_fab ();
int v_vserv_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_vserv_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* vserv_cntform.c */
int v_vserv_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
i_form_item* v_vserv_cntform_servlist (i_resource *self, i_form_reqdata *reqdata, i_list *item_list, i_form_item *table);

/* vserv_objform.c */
int v_vserv_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
int v_vserv_objform_hist (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

