/* File System Item */

typedef struct v_filesys_item_s
{
  /* item variables */
  struct i_object_s *obj;

  /* kilobytes */
  struct i_metric_s *bytes_total;
  struct i_metric_s *bytes_used;
  struct i_metric_s *bytes_avail;
  struct i_metric_s *bytes_used_pc;

  /* inodes */
  struct i_metric_s *inodes_total;
  struct i_metric_s *inodes_used;
  struct i_metric_s *inodes_avail;
  struct i_metric_s *inodes_used_pc;

  /* files */
  struct i_metric_s *files_maxavail;
  struct i_metric_s *files_maxpossible;
  struct i_metric_s *files_used;
  struct i_metric_s *files_avail;
  struct i_metric_s *files_used_pc;

  /* mountpoint */
  struct i_metric_s *mountpoint;

  /* CGraphs */
  struct i_metric_cgraph_s *capacity_cg;
  
} v_filesys_item;

/* filesys.c */
struct i_container_s* v_filesys_cnt ();
int v_filesys_enable (i_resource *self);
int v_filesys_disable (i_resource *self);

/* filesys_item.c */
v_filesys_item* v_filesys_item_create ();
void v_filesys_item_free (void *itemptr);

/* filesys_objfact.c */
int v_filesys_objfact_fab ();
int v_filesys_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_filesys_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* filesys_cntform.c */
int v_filesys_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* filesys_objform.c */
int v_filesys_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
int v_filesys_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata);

