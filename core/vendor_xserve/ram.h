typedef struct v_ram_item_s
{
  struct i_object_s *obj;

  /* Xserve - Common */
  struct i_metric_s *size;
  struct i_metric_s *speed;
  struct i_metric_s *type;

  /* Xserve - Intel */
  struct i_metric_s *temp;
  struct i_metric_s *ecc_errors;
  
} v_ram_item;

/* ram.c */
struct i_container_s* v_ram_cnt ();
v_ram_item* v_ram_get (char *desc_str);
int v_ram_enable (i_resource *self);
v_ram_item* v_ram_create (i_resource *self, char *desc_str);
int v_ram_disable (i_resource *self);

/* ram_item.c */
v_ram_item* v_ram_item_create ();
void v_ram_item_free (void *itemptr);

/* ram_cntform.c */
int v_ram_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* ram_objform.c */
int v_ram_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);


