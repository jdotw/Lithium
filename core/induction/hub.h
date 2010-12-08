typedef struct
{
  int type;

  void *data;
  int datasize;

  i_resource_address *src;    /* Original hub_msg src */
  
  char *owner_str;
  char *customer_id;
  char *id_str;

  char *module_name;        /* The module name that created/decodes the data */
  char *decode_func_str;    /* The function to decode the data in the module */
} i_hub_msg;

typedef struct
{
  i_resource_address *owner;
  char *id_str;

  int (*hub_callback) (i_resource *self, i_hub_msg *hub_msg, i_form *config_form, void *data);
  void *callback_data;
} i_hub_subscription;

typedef struct
{
  char *id_str;
  char *owner_str;
  char *customer_id;
} i_hub_desc;

/* Hub message types */

#define HUB_MSG_UNKNOWN 0
#define HUB_MSG_STRING 1
#define HUB_MSG_DATA 2
#define HUB_MSG_STATUS 3
#define HUB_MSG_ADDED 4     /* Announce a hub being added */
#define HUB_MSG_REMOVED 5  /* Announce a hub being removed */

/* General functions */

int i_hub_create (i_resource *self, char *id_str);
int i_hub_send_data (i_resource *self, i_resource_address *owner, char *id_str, int type, void *hub_data, int hub_datasize, char *module_name, char *decode_func_str);
int i_hub_send_string (i_resource *self, i_resource_address *owner, char *id_str, char *str);
int i_hub_relay (i_resource *self, i_resource_address *owner, char *id_str, i_hub_msg *hub_msg);

/* Message functions */

void i_hub_free_msg (i_hub_msg *msg);
void i_hub_address_message (i_resource *self, i_message *msg);
char* i_hub_msg_struct_to_data (i_hub_msg *msg, int *datasizeptr);
i_hub_msg* i_hub_msg_data_to_struct (char *data, int datasize);
i_hub_msg* i_hub_msg_duplicate (i_hub_msg *msg);
i_hub_msg* i_hub_msg_create (i_resource *self, i_resource_address *owner, char *id_str, int type, void *hub_data, int hub_datasize, char *module_name, char *decode_func_str);
int i_hub_msg_send (i_resource *self, i_hub_msg *hub_msg);

/* Hub Descriptions */

void i_hub_free_desc (i_hub_desc *desc);
i_hub_desc* i_hub_desc_data_to_struct (char *data, int datasize);

/* Hub Subscriptions */

void i_hub_free_subscription (void *hub_sub_ptr);
i_hub_subscription* i_hub_subscribe (i_resource *self, i_resource_address *owner, char *id_str, int (*hub_callback) (), void *callback_data);
int i_hub_unsubscribe (i_resource *self, i_hub_subscription *hub_sub);
int i_hub_unsubcribe_by_owner_id (i_resource *self, i_resource_address *owner, char *id_str);
void i_hub_process_broadcast (i_resource *self, i_message *msg, i_form *config_form);
