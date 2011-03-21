typedef struct v_pdrive_item_s
{
  struct i_object_s *obj;
  
  struct i_metric_s *index;
  struct i_metric_s *logical_channel;
  struct i_metric_s *physical_channel;
  struct i_metric_s *scsi_id;
  struct i_metric_s *scsi_lun;
  struct i_metric_s *ldrive_id;
  struct i_metric_s *size_blocks;
  struct i_metric_s *blocksize_index;
  struct i_metric_s *blocksize;
  struct i_metric_s *size;
  struct i_metric_s *speed;
  struct i_metric_s *data_width;
  struct i_metric_s *status;
  struct i_metric_s *state;
  struct i_metric_s *slot;
  struct i_metric_s *model;
  struct i_metric_s *firmware;
  struct i_metric_s *serial;

} v_pdrive_item;

/* pdrive.c */
struct i_container_s* v_pdrive_cnt ();
int v_pdrive_enable (i_resource *self);
int v_pdrive_disable (i_resource *self);

/* pdrive_item.c */
v_pdrive_item* v_pdrive_item_create ();
void v_pdrive_item_free (void *itemptr);

/* pdrive_objfact.c */
int v_pdrive_objfact_fab ();
int v_pdrive_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_pdrive_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* pdrive_cntform.c */
int v_pdrive_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* pdrive_objform.c */
int v_pdrive_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_pdrive_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

