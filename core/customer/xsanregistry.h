typedef struct l_xsanregistry_device_s
{
  /* Device Resource Address */
  struct i_resource_address_s *devaddr;

  /* Device Entity Descriptor */
  struct i_entity_descriptor_s *devent;
  
  /* Type */
  int type;           /* 1=Controller 2=Client */

  /* Time stamps */
  struct timeval register_time;

  /* Volume entry list */
  struct i_list_s *entry_list;

} l_xsanregistry_device;

typedef struct l_xsanregistry_entry_s
{
  /* Volume Object Entity Descriptor */
  struct i_entity_descriptor_s *objent;

  /* Device Resource Address */
  struct i_resource_address_s *devaddr;

  /* Time stamps */
  struct timeval register_time;

  /* Received device info */
  struct i_entity_descriptor_s *devent;
  int devtype;

} l_xsanregistry_entry;

#define DEFAULT_DEVICE_TABLE_SIZE 500
#define DEFAULT_OBJECT_TABLE_SIZE 500

/* xsanregistry.c */
int l_xsanregistry_enable (i_resource *self);
int l_xsanregistry_disable (i_resource *self);
i_hashtable* l_xsanregistry_table_device ();
i_hashtable* l_xsanregistry_table_object ();

/* xsanregistry_entry.c */
l_xsanregistry_entry* l_xsanregistry_entry_create ();
void l_xsanregistry_entry_free (void *entryptr);

/* xsanregistry_device.c */
l_xsanregistry_device* l_xsanregistry_device_create (struct i_resource_address_s *addr, struct i_entity_descriptor_s *ent, int type);
void l_xsanregistry_device_free (void *deviceptr);

/* xsanregistry_add.c */
int l_xsanregistry_add (i_resource *self, l_xsanregistry_entry *entry);

/* xsanregistry_remove.c */
int l_xsanregistry_remove (i_resource *self, i_resource_address *devaddr, struct i_entity_descriptor_s *objent);

/* xsanregistry_handler.c */
int l_xsanregistry_handler_register (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
int l_xsanregistry_handler_deregister (i_resource *self, i_socket *sock, i_message *msg, void *passdata);

/* xsanregistry_xml_list.c */
void l_xsanregistry_xml_list_iterate (i_resource *self, i_hashtable *obj_table, void *data, void *passdata);

