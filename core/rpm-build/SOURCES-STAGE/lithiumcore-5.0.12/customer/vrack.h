/* Data Structures */

typedef struct l_vrack_s 
{
  int doc_id;
  int ru_count;
  char *thumbnail_data;     /* Base64 Encoded */
  i_list *device_list;
  i_list *cable_list;
  i_list *cablegroup_list;
} l_vrack;

typedef struct l_vrack_device_s
{
  int id;
  char *uuid_str;
  int ru_index;
  int size;
  struct i_entity_descriptor_s *entdesc;
  float color_red;
  float color_green;
  float color_blue;
} l_vrack_device;

typedef struct l_vrack_cable_s
{
  int id;
  char *uuid_str;
  char *group_uuid_str;
  struct i_entity_descriptor_s *a_int_entdesc;
  struct i_entity_descriptor_s *b_int_entdesc;
  char *vlans_str;
  char *notes_str;
  int locked;
  float color_red;
  float color_green;
  float color_blue;
} l_vrack_cable;

typedef struct l_rack_cablegroup_s
{
  int id;
  char *uuid_str;
  char *desc_str;
} l_vrack_cablegroup;

/* vrack.c */
int l_vrack_enable (i_resource *self);
l_vrack* l_vrack_create ();
void l_vrack_free (void *vrackptr);

/* vrack_device.c */
l_vrack_device* l_vrack_device_create ();
void l_vrack_device_free (void *vrack_deviceptr);

/* vrack_cable.c */
l_vrack_cable* l_vrack_cable_create ();
void l_vrack_cable_free (void *vrack_cableptr);

/* vrack_cablegroup.c */
l_vrack_cablegroup* l_vrack_cablegroup_create ();
void l_vrack_cablegroup_free (void *vrack_cablegroupptr);

/* vrack_get.c */
struct i_callback_s* l_vrack_get (i_resource *self, int doc_id, int (*cbfunc) (), void *passdata);
int l_vrack_get_sqlcb (i_resource *self, l_vrack *vrack, void *passdata);

/* vrack_commit.c */
int l_vrack_commit (i_resource *self, int doc_id, struct i_xml_s *xml, xmlNodePtr data_node);

/* vrack_delete.c */
int l_vrack_delete (i_resource *self, int doc_id);

/* vrack_sql.c */
struct i_callback_s* l_vrack_sql_get (i_resource *self, int doc_id, int (*cbfunc) (), void *passdata);
int l_vrack_sql_get_vrackcb (i_resource *self, struct i_pg_async_conn_s *conn, int operation, PGresult *res, void *passdata);
int l_vrack_sql_get_devicecb (i_resource *self, struct i_pg_async_conn_s *conn, int operation, PGresult *res, void *passdata);
int l_vrack_sql_get_cablecb (i_resource *self, struct i_pg_async_conn_s *conn, int operation, PGresult *res, void *passdata);
int l_vrack_sql_get_cablegroupcb (i_resource *self, struct i_pg_async_conn_s *conn, int operation, PGresult *res, void *passdata);
int l_vrack_sql_insert (i_resource *self, l_vrack *vrack);
int l_vrack_sql_delete (i_resource *self, int doc_id);
int l_vrack_sql_device_insert (i_resource *self, l_vrack *vrack, l_vrack_device *device);
int l_vrack_sql_device_delete (i_resource *self, long doc_id);
int l_vrack_sql_device_delete_device (i_resource *self, struct i_device_s *dev);
int l_vrack_sql_cable_insert (i_resource *self, l_vrack *vrack, l_vrack_cable *cable);
int l_vrack_sql_cable_delete (i_resource *self, long doc_id);
int l_vrack_sql_cable_delete_device (i_resource *self, struct i_device_s *dev);
int l_vrack_sql_cablegroup_insert (i_resource *self, l_vrack *vrack, l_vrack_cablegroup *cablegroup);
int l_vrack_sql_cablegroup_delete (i_resource *self, long doc_id);
int l_vrack_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);

