typedef struct l_lunregistry_entry_s
{
  /* Type */
  int type;       /* 1=Target 2=Use */

  /* WWN */
  char *wwn_str;

  /* LUN */
  int lun;

  /* Object Entity Descriptor */
  struct i_entity_descriptor_s *objent;

  /* Device Resource Address */
  struct i_resource_address_s *devaddr;

  /* Time stamps */
  struct timeval register_time;

  /* Uses -- A list of objects that use/mount this LUN */

} l_lunregistry_entry;

#define DEFAULT_DEVICE_TABLE_SIZE 500
#define DEFAULT_OBJECT_TABLE_SIZE 500

/* lunregistry.c */
int l_lunregistry_enable (i_resource *self);
int l_lunregistry_disable (i_resource *self);
i_hashtable* l_lunregistry_table_device ();
i_hashtable* l_lunregistry_table_object ();

/* lunregistry_entry.c */
l_lunregistry_entry* l_lunregistry_entry_create ();
void l_lunregistry_entry_free (void *entryptr);

/* lunregistry_add.c */
int l_lunregistry_add (i_resource *self, l_lunregistry_entry *entry);

/* lunregistry_remove.c */
int l_lunregistry_remove (i_resource *self, i_resource_address *devaddr, struct i_entity_descriptor_s *objent);

/* lunregistry_handler.c */
int l_lunregistry_handler_register (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
int l_lunregistry_handler_deregister (i_resource *self, i_socket *sock, i_message *msg, void *passdata);

/* lunregistry_xml_list.c */
void l_lunregistry_xml_list_iterate (i_resource *self, i_hashtable *obj_table, void *data, void *passdata);

