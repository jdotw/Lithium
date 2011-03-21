/* Definitions */

#define DEFAULT_VENDOR_TABLE_SIZE 20
#define DEFAULT_PRODUCT_TABLE_SIZE 500
#define DEFAULT_SITE_TABLE_SIZE 100
#define DEFAULT_DEVICE_TABLE_SIZE 500

/* inventory.c */

int l_inventory_enable (i_resource *self);
int l_inventory_disable (i_resource *self);
i_hashtable* l_inventory_table_device ();
i_hashtable* l_inventory_table_vendor ();
i_hashtable* l_inventory_table_site ();

/* inventory_add.c */

int l_inventory_add (i_resource *self, i_inventory_item *item);
int l_inventory_add_sqlget_callback (i_resource *self, i_list *list, void *passdata);

/* inventory_get.c */

i_hashtable* l_inventory_get_vendor (i_resource *self, char *vendor_str);
i_list* l_inventory_get_product (i_resource *self, i_hashtable *product_table, char *product_str);
i_list* l_inventory_get_device_str (i_resource *self, char *device_addr_str);
i_list* l_inventory_get_device (i_resource *self, i_resource_address *device_addr);
i_list* l_inventory_get_site (i_resource *self, char *site_name);

/* inventory_handler.c */

int l_inventory_handler_register (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
int l_inventory_handler_deregister (i_resource *self, i_socket *sock, i_message *msg, void *passdata);

/* inventory_listsort.c */

int l_inventory_listsort_vendor_func (void *curptr, void *nextptr);
int l_inventory_listsort_product_func (void *curptr, void *nextptr);
int l_inventory_listsort_version_func (void *curptr, void *nextptr);
int l_inventory_listsort_serial_func (void *curptr, void *nextptr);
int l_inventory_listsort_devicedesc_func (void *curptr, void *nextptr);
int l_inventory_listsort_sitedesc_func (void *curptr, void *nextptr);
int l_inventory_listsort_type_func (void *curptr, void *nextptr);
int l_inventory_listsort_typevendor_func (void *curptr, void *nextptr);
int l_inventory_listsort_typeproduct_func (void *curptr, void *nextptr);
int l_inventory_listsort_typeverser_func (void *curptr, void *nextptr);

/* inventory_remove.c */

int l_inventory_remove (i_resource *self, i_resource_address *device_addr, int type, char *vendor_str, char *product_str, char *verser_str);
int l_inventory_remove_item (i_resource *self, i_inventory_item *invitem);

/* inventory_static.c */

int l_inventory_static_add (i_resource *self, i_inventory_item *invitem);
int l_inventory_static_remove_item (i_resource *self, i_inventory_item *invitem);
int l_inventory_static_registerall (i_resource *self, struct i_device_s *dev);
int l_inventory_static_registerall_sqlcb (i_resource *self, i_list *list, void *passdata);

/* inventory_sql.c */

#define INV_HISTORY_TABLE "inventory_item_history"
#define INV_STATIC_TABLE "inventory_static_items"

int l_inventory_sql_get_list (i_resource *self, char *table_str, char *site_id, char *device_id, char *vendor_str, char *product_str, char *version_str, char *serial_str,int (*callback_func) (i_resource *self, i_list *list, void *passdata), void *passdata);
int l_inventory_sql_get_list_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);
int l_inventory_sql_insert (i_resource *self, char *table_str, i_inventory_item *item);
int l_inventory_sql_delete (i_resource *self, char *table_str, char *site_id, char *device_id, char *vendor_str, char *product_str, char *version_str, char *serial_str);
int l_inventory_sql_callback (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);

/* inventory_itemlist.c */

int l_inventory_itemlist_table (i_resource *self, i_list *list, char *name, char *title, i_form_reqdata *reqdata);

/* inventory_form_links.c */

int l_inventory_form_links (i_resource *self, i_form_reqdata *reqdata);

/* inventory_form_vendorlist.c */

void l_inventory_vendorlist_vendortable_iterate (i_resource *self, i_hashtable *vendor_table, void *data, void *passdata);
void l_inventory_vendorlist_producttable_iterate (i_resource *self, i_hashtable *product_table, void *data, void *passdata);

/* inventory_form_vendormain.c */

void l_inventory_vendormain_iterate (i_resource *self, i_hashtable *product_table, void *data, void *passdata);
int l_inventory_vendormain_prodlist_sortfunc (void *curptr, void *nextptr);

/* inventory_form_devicelist.c */

void l_inventory_devicelist_iterate (i_resource *self, i_hashtable *device_table, void *data, void *passdata);

/* inventory_form_sitelist.c */

void l_inventory_sitelist_iterate (i_resource *self, i_hashtable *site_table, void *data, void *passdata);

/* inventory_form_historymain.c */

int l_inventory_form_historymain_sqlcb (i_resource *self, i_list *list, void *passdata);

/* inventory_form_static_edit.c */

int l_inventory_static_form_add_hiercb (i_resource *self, i_hierarchy *hierarchy, void *passdata);
int l_inventory_static_form_add_submit_hiercb (i_resource *self, i_hierarchy *hierarchy, void *passdata);
