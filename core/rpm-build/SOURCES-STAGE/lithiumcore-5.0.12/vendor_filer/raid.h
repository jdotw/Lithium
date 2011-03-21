/* RAID Item */

typedef struct v_raid_item_s
{
  /* item variables */
  struct i_object_s *obj;

  /* id variables */
  struct i_metric_s *opstate;
  struct i_metric_s *diskid;
  struct i_metric_s *scsiadaptor;
  struct i_metric_s *scsiid;

  /* byte capacity/used */
  struct i_metric_s *bytes_total;
  struct i_metric_s *bytes_used;
  struct i_metric_s *bytes_avail;
  struct i_metric_s *bytes_used_pc;

  /* block capacity/used */
  struct i_metric_s *blocks_total;
  struct i_metric_s *blocks_used;
  struct i_metric_s *blocks_avail;
  struct i_metric_s *blocks_used_pc;

  /* progress */
  struct i_metric_s *complete_pc;

  /* raid info */
  struct i_metric_s *vol;
  struct i_metric_s *group;
  struct i_metric_s *diskcount;
  struct i_metric_s *groupcount;

  /* CGraphs */
  struct i_metric_cgraph_s *capacity_cg;

} v_raid_item;

/* raid.c */
struct i_container_s* v_raid_cnt ();
int v_raid_enable (i_resource *self);
int v_raid_disable (i_resource *self);

/* raid_item.c */
v_raid_item* v_raid_item_create ();
void v_raid_item_free (void *itemptr);

/* raid_objfact.c */
int v_raid_objfact_fab ();
int v_raid_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_raid_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* raid_cntform.c */
int v_raid_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* raid_objform.c */
int v_raid_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
int v_raid_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata);

