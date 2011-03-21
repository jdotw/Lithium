/* Data Structures */

typedef struct l_scene_s 
{
  int doc_id;
  char *background_data;       /* Base64 Encoded Image */
  i_list *overlay_list;
} l_scene;

typedef struct l_scene_overlay_s
{
  char *uuid_str;
  float x, y, width, height;
  int autosize;
  struct i_entity_descriptor_s *entdesc;
} l_scene_overlay;

/* scene.c */
int l_scene_enable (i_resource *self);
l_scene* l_scene_create ();
void l_scene_free (void *sceneptr);

/* scene_overlay.c */
l_scene_overlay* l_scene_overlay_create ();
void l_scene_overlay_free (void *scene_overlayptr);

/* scene_get.c */
struct i_callback_s* l_scene_get (i_resource *self, int doc_id, int (*cbfunc) (), void *passdata);
int l_scene_get_sqlcb (i_resource *self, l_scene *scene, void *passdata);

/* scene_commit.c */
int l_scene_commit (i_resource *self, int doc_id, struct i_xml_s *xml, xmlNodePtr data_node);

/* scene_delete.c */
int l_scene_delete (i_resource *self, int doc_id);

/* scene_sql.c */
struct i_callback_s* l_scene_sql_get (i_resource *self, int doc_id, int (*cbfunc) (), void *passdata);
int l_scene_sql_get_scenecb (i_resource *self, struct i_pg_async_conn_s *conn, int operation, PGresult *res, void *passdata);
int l_scene_sql_get_overlaycb (i_resource *self, struct i_pg_async_conn_s *conn, int operation, PGresult *res, void *passdata);
int l_scene_sql_insert (i_resource *self, l_scene *scene);
int l_scene_sql_delete (i_resource *self, int doc_id);
int l_scene_sql_overlay_insert (i_resource *self, l_scene *scene, l_scene_overlay *overlay);
int l_scene_sql_overlay_delete (i_resource *self, long doc_id);
int l_scene_sql_overlay_delete_device (i_resource *self, struct i_device_s *dev);
int l_scene_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);



