/* slot.c */
struct i_container_s* v_slot_cnt ();
int v_slot_enable (i_resource *self, char *prefix_str);

/* slot_objfact.c */
int v_slot_objfact_fab ();
int v_slot_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_slot_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

