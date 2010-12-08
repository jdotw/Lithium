typedef struct l_ipregistry_entry_s
{
  /* IP Address */
  char *ip_str;
  struct in_addr ipaddr;

  /* Netmask */
  char *mask_str;
  struct in_addr maskaddr;

  /* Network */
  char *network_str;
  struct in_addr networkaddr;

  /* DNS */
  char *dns_str;

  /* Device */
  struct i_hashtable_s *device_table;     /* Keyed by device address */

  /* State */
  int bound;                              /* 0 = Known but not bound.. 1= Known and bound */

  /* Timer */
  struct i_timer_s *dead_timer;           /* Self-review timer */

} l_ipregistry_entry;

typedef struct l_ipregistry_entry_device_s
{
  struct i_entity_descriptor_s *ipent;
  struct i_entity_descriptor_s *iface;
} l_ipregistry_entry_device;

typedef struct l_ipregistry_network_s
{
  char *network_str;
  struct in_addr networkaddr;
  char *mask_str;
  struct in_addr maskaddr;
  
  struct i_list_s *entry_list;

  struct i_timer_s *rescan_timer;
} l_ipregistry_network;

#define DEFAULT_IP_TABLE_SIZE 2000
#define DEFAULT_NETWORK_TABLE_SIZE 1000
#define DEFAULT_DEVICE_TABLE_SIZE 500

/* ipregistry.c */
int l_ipregistry_enable (i_resource *self);
int l_ipregistry_disable (i_resource *self);
i_hashtable* l_ipregistry_table_ip ();
i_hashtable* l_ipregistry_table_network ();
i_hashtable* l_ipregistry_table_device ();

/* ipregistry_entry.c */
l_ipregistry_entry* l_ipregistry_entry_create ();
void l_ipregistry_entry_free (void *entryptr);
l_ipregistry_entry_device* l_ipregistry_entry_device_create ();
void l_ipregistry_entry_device_free (void *devptr);

/* ipregistry_network.c */
l_ipregistry_network* l_ipregistry_network_create (i_resource *self, char *network_str, char *mask_str);
void l_ipregistry_network_free (void *networkptr);

/* ipregistry_add.c */
l_ipregistry_entry* l_ipregistry_add (i_resource *self, char *ip_str, char *mask_str);

/* ipregistry_set.c */
int l_ipregistry_set_netmask (i_resource *self, char *ip_str, char *mask_str);
int l_ipregistry_set_dns (i_resource *self, l_ipregistry_entry *entry, char *dns_str);

/* ipregistry_get.c */
l_ipregistry_entry* l_ipregistry_get_ip_str (i_resource *self, char *ip_str);
l_ipregistry_entry* l_ipregistry_get_ip (i_resource *self, unsigned long int ip);
l_ipregistry_entry_device* l_ipregistry_get_ip_device (i_resource *self, l_ipregistry_entry *entry, struct i_resource_address_s *devaddr);
l_ipregistry_network* l_ipregistry_get_network (i_resource *self, char *network_str, char *mask_str);
i_list* l_ipregistry_get_device_str (i_resource *self, char *device_addr_str);
i_list* l_ipregistry_get_device (i_resource *self, i_resource_address *device_addr);

/* ipregistry_remove.c */
int l_ipregistry_remove (i_resource *self, char *ip_str);

/* ipregistry_handler.c */
int l_ipregistry_handler_register (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
int l_ipregistry_handler_setnetmask (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
int l_ipregistry_handler_setiface (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
int l_ipregistry_handler_deregister (i_resource *self, i_socket *sock, i_message *msg, void *passdata);

/* ipregistry_entrylist.c */
int l_ipregistry_entrylist_table (i_resource *self, i_list *list, char *name, char *title, i_form_reqdata *reqdata);

/* ipregistry_listsort.c */
int l_ipregistry_listsort_ip_func (void *curptr, void *nextptr);

/* ipregistry_xml_network_list.c */
int xml_ipregistry_network_list ();
