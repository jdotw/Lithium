typedef struct l_group_s
{
  int id;
  int parent_id;
  char *desc_str;
} l_group;

typedef struct l_group_entity_s
{
  int parent_id;
  i_entity_descriptor *entdesc;
} l_group_entity;

/* group.c */
int l_group_enable (i_resource *self);
l_group* l_group_create ();
void l_group_free (void *docptr);
l_group_entity* l_group_entity_create ();
void l_group_entity_free (void *docptr);

/* group_id.c */
long l_group_id_assign ();
void l_group_id_setcurrent (long currentid);

/* group_xml_list.c */
int l_group_xml_list_sqlcb (i_resource *self, i_list *list, void *passdata);

/* group_xml_edit.c */

/* group_sql.c */
int l_group_sql_insert (i_resource *self, l_group *group);
int l_group_sql_update (i_resource *self, l_group *group);
int l_group_sql_update_editor (i_resource *self, int group_id, int state, char *editor_str);
int l_group_sql_delete (i_resource *self, int group_id);
int l_group_sql_entity_insert (i_resource *self, l_group_entity *g_ent);
int l_group_sql_entity_delete (i_resource *self, l_group_entity *g_ent);
int l_group_sql_entity_delete_device (i_resource *self, struct i_device_s *dev);
int l_group_sql_entity_move (i_resource *self, int prev_parent, l_group_entity *g_ent);
struct i_callback_s* l_group_sql_load_list (i_resource *self, int group_id, int use_group_id, int (*cbfunc) (), void *passdata);
int l_group_sql_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_group_sql_load_entity_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_group_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);

