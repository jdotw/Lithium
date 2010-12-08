typedef struct l_document_s
{
  int id;
  char *type_str;
  char *desc_str;
  int state;          /* 0=Normal 1=Editing */
  char *editor_str;   /* Username of editor */
  time_t version;
} l_document;

/* document.c */
int l_document_enable (i_resource *self);
l_document* l_document_create ();
void l_document_free (void *docptr);

/* document_id.c */
long l_document_id_assign ();
void l_document_id_setcurrent (long currentid);

/* document_xml_list.c */
int l_document_xml_list_sqlcb (i_resource *self, i_list *list, void *passdata);

/* document_xml_edit.c */


/* document_sql.c */
int l_document_sql_insert (i_resource *self, l_document *document);
int l_document_sql_update (i_resource *self, l_document *document);
int l_document_sql_update_editor (i_resource *self, int doc_id, int state, char *editor_str);
int l_document_sql_delete (i_resource *self, int document_id);
struct i_callback_s* l_document_sql_load_list (i_resource *self, char *type_str, int doc_id, int (*cbfunc) (), void *passdata);
int l_document_sql_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_document_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);

