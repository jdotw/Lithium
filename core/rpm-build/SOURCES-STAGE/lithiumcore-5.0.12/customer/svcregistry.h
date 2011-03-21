typedef struct l_svcregistry_entry_s
{
  /* Object Entity Descriptor */
  struct i_entity_descriptor_s *objent;

  /* Device Resource Address */
  struct i_resource_address_s *devaddr;

  /* Time stamps */
  struct timeval register_time;

} l_svcregistry_entry;

#define DEFAULT_DEVICE_TABLE_SIZE 500
#define DEFAULT_OBJECT_TABLE_SIZE 500

/* svcregistry.c */
int l_svcregistry_enable (i_resource *self);
int l_svcregistry_disable (i_resource *self);
i_hashtable* l_svcregistry_table_device ();
i_hashtable* l_svcregistry_table_object ();

/* svcregistry_entry.c */
l_svcregistry_entry* l_svcregistry_entry_create ();
void l_svcregistry_entry_free (void *entryptr);

/* svcregistry_add.c */
int l_svcregistry_add (i_resource *self, l_svcregistry_entry *entry);

/* svcregistry_remove.c */
int l_svcregistry_remove (i_resource *self, i_resource_address *devaddr, struct i_entity_descriptor_s *objent);

/* svcregistry_handler.c */
int l_svcregistry_handler_register (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
int l_svcregistry_handler_deregister (i_resource *self, i_socket *sock, i_message *msg, void *passdata);

/* svcregistry_xml_list.c */
void l_svcregistry_xml_list_iterate (i_resource *self, i_hashtable *obj_table, void *data, void *passdata);

