typedef struct l_procregistry_entry_s
{
  /* Object Entity Descriptor */
  struct i_entity_descriptor_s *objent;

  /* Device Resource Address */
  struct i_resource_address_s *devaddr;

  /* Time stamps */
  struct timeval register_time;

} l_procregistry_entry;

#define DEFAULT_DEVICE_TABLE_SIZE 500
#define DEFAULT_OBJECT_TABLE_SIZE 500

/* procregistry.c */
int l_procregistry_enable (i_resource *self);
int l_procregistry_disable (i_resource *self);
i_hashtable* l_procregistry_table_device ();
i_hashtable* l_procregistry_table_object ();

/* procregistry_entry.c */
l_procregistry_entry* l_procregistry_entry_create ();
void l_procregistry_entry_free (void *entryptr);

/* procregistry_add.c */
int l_procregistry_add (i_resource *self, l_procregistry_entry *entry);

/* procregistry_remove.c */
int l_procregistry_remove (i_resource *self, i_resource_address *devaddr, struct i_entity_descriptor_s *objent);

/* procregistry_handler.c */
int l_procregistry_handler_register (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
int l_procregistry_handler_deregister (i_resource *self, i_socket *sock, i_message *msg, void *passdata);

/* procregistry_xml_list.c */
void l_procregistry_xml_list_iterate (i_resource *self, i_hashtable *obj_table, void *data, void *passdata);

