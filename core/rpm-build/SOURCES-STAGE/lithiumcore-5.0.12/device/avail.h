/* avail.c */

struct i_container_s* l_avail_cnt ();
struct i_object_s* l_avail_masterobj ();
int l_avail_enable (i_resource *self);
int l_avail_disable (i_resource *self);

/* avail_item.c */

typedef struct l_avail_item_s
{
  struct i_object_s *obj;
  
  /* Successful/Failed Operations Counters 
   * These counters are refreshed by the
   * l_avail_refresh external refresh function. 
   * The value of these metrics will be set to 
   * the current values of the ok and failed 
   * running totals at the time the refresh is 
   * called
   */
  struct i_metric_s *ok_ops;
  struct i_metric_s *fail_ops;

  /* Total of successful and failed operations.
   * This metric is autocalculated as a sum of
   * the ok_ops and fail_ops metrics
   */
  struct i_metric_s *total_ops;

  /* Successful/Failed Percentages.
   * These metrics are autocalculated as a
   * percent of either ok_ops and total_ops
   * or fail_ops and total_ops.
   */
  struct i_metric_s *ok_pc;
  struct i_metric_s *fail_pc;

  /* Response time. 
   * This counter is refreshed by the l_avail_refresh
   * external refresh function. The value of this 
   * metric will be set to the current value of the
   * running refresh-time average (i.e the average
   * response time record since the last refresh).
   */
  struct i_metric_s *resptime;

  /* Interim variables. These variables are
   * used as counters or current variables 
   * inbetween refresh operations. When a 
   * refresh operation occurs, the values of
   * these interim variables are used to 
   * set the metrics above
   */
  unsigned long interim;
  unsigned long ok_interim;
  unsigned long fail_interim;
  struct i_list_s *rt_list;

  /* CGraphs for representing availability
   * metrics. The avail_cg shows the successful
   * operation percentage against the failed percentage
   */
  struct i_metric_cgraph_s *avail_cg;

} l_avail_item;

l_avail_item* l_avail_item_create ();
void l_avail_item_free (void *itemptr);

/* avail_cntform.c */

int l_avail_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int l_avail_cntform_summary (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int l_avail_cntform_rtsummary (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* avail_objform.c */

int l_avail_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int l_avail_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* avail_record.c */

int l_avail_record_ok (struct i_object_s *obj, struct timeval *start, struct timeval *end);
int l_avail_record_fail (struct i_object_s *obj);

/* avail_refresh.c */
int l_avail_refresh_master_obj (i_resource *self, struct i_object_s *masterobj, int opcode);

/* avail_object.c */

struct i_object_s* l_avail_object_add (i_resource *self, char *name_str, char *desc_str);
int l_avail_object_remove (i_resource *self, struct i_object_s *obj);

