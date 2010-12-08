typedef struct v_enclosure_item_s
{
  struct i_object_s *obj;

  /* Xserve */
  struct i_metric_s *int_amb_temp;
  struct i_metric_s *sc_amb_temp;
  struct i_metric_s *sc_int_temp;
  struct i_metric_s *memory_temp;
  struct i_metric_s *pci_temp;

} v_enclosure_item;

/* enclosure.c */
struct i_container_s* v_enclosure_cnt ();
struct v_enclosure_item_s* v_enclosure_static_item ();
int v_enclosure_enable (i_resource *self);
int v_enclosure_disable (i_resource *self);

/* enclosure_item.c */
v_enclosure_item* v_enclosure_item_create ();
void v_enclosure_item_free (void *itemptr);

/* enclosure_cntform.c */
int v_enclosure_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* enclosure_objform.c */
int v_enclosure_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

