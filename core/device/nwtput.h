/* nwtput.c */
struct i_container_s* l_nwtput_cnt ();
struct i_object_s* l_nwtput_bpsobj ();
struct i_object_s* l_nwtput_ppsobj ();
int l_nwtput_enable (i_resource *self);
int l_nwtput_disable (i_resource *self);

/* nwtput_item.c */
typedef struct l_nwtput_item_s
{
  struct i_object_s *obj;
  
  struct i_metric_s *input;
  struct i_metric_s *output;
  
  struct i_metric_cgraph_s *tput_cg;

} l_nwtput_item;

l_nwtput_item* l_nwtput_item_create ();
void l_nwtput_item_free (void *itemptr);

/* nwtput_cntform.c */
int l_nwtput_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* nwtput_objform.c */
int l_nwtput_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int l_nwtput_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* nwtput_metset.c */
int l_nwtput_metset_add (i_resource *self, struct i_metric_s *input_bps, struct i_metric_s *output_bps, struct i_metric_s *input_pps, struct i_metric_s *output_pps);
int l_nwtput_metset_remove (i_resource *self, struct i_metric_s *input_bps, struct i_metric_s *output_bps, struct i_metric_s *input_pps, struct i_metric_s *output_pps);
