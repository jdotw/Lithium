/* Item types */

#define FORM_ITEM_UNKNOWN 0
#define FORM_ITEM_STRING 1
#define FORM_ITEM_ENTRY 2
#define FORM_ITEM_DROPDOWN 3
#define FORM_ITEM_HIDDEN 4
#define FORM_ITEM_TABLE 5
#define FORM_ITEM_SPACER 6
#define FORM_ITEM_IMAGE 7
#define FORM_ITEM_TEXTAREA 8
#define FORM_ITEM_FRAMESTART 9
#define FORM_ITEM_FRAMEEND 10
#define FORM_ITEM_HRLINE 11
#define FORM_ITEM_PASSWORD 12

/* Item states */

#define ITEMSTATE_READY 0
#define ITEMSTATE_NOTREADY 1

/* Form states */

#define FORMSTATE_NOTREADY 0
#define FORMSTATE_DELIVERABLE 1

/* Option Types */

#define FORM_OPTION_UNKNOWN 0
#define FORM_OPTION_TITLE 1
#define FORM_OPTION_SUBMIT_RESADDR 2
#define FORM_OPTION_SUBMIT_ENTADDR 3
#define FORM_OPTION_SUBMIT_NAME 4
#define FORM_OPTION_SUBMIT_REFSEC 5
#define FORM_OPTION_SUBMIT_PASSDATA 6
#define FORM_OPTION_SUBMIT_LABEL 7
#define FORM_OPTION_SOURCE_RESADDR 8
#define FORM_OPTION_SOURCE_ENTADDR 9
#define FORM_OPTION_SOURCE_FORM 10
#define FORM_OPTION_SOURCE_REFSEC 11
#define FORM_OPTION_SOURCE_PASSDATA 12
#define FORM_OPTION_DEFAULTFRAME_WIDTH 13
#define FORM_OPTION_DEFAULTFRAME_HEIGHT 14

#define ITEM_OPTION_UNKNOWN 0
#define ITEM_OPTION_VALUE 1
#define ITEM_OPTION_ORIGINAL_VALUE 2
#define ITEM_OPTION_LABEL 3
#define ITEM_OPTION_SELECTED 4
#define ITEM_OPTION_ROW_COUNT 5
#define ITEM_OPTION_COL_COUNT 6
#define ITEM_OPTION_LINK_RESADDR 7
#define ITEM_OPTION_LINK_ENTADDR 8
#define ITEM_OPTION_LINK_NAME 9
#define ITEM_OPTION_LINK_REFSEC 10
#define ITEM_OPTION_LINK_PASSDATA 11
#define ITEM_OPTION_SPACING_PROP 12
#define ITEM_OPTION_WIDTH 13
#define ITEM_OPTION_DATA 14
#define ITEM_OPTION_FILENAME 15
#define ITEM_OPTION_BGCOLOR 16
#define ITEM_OPTION_FGCOLOR 17
#define ITEM_OPTION_NOHEADERS 18
#define ITEM_OPTION_ALIGN 19
#define ITEM_OPTION_VALIGN 20

#define FORM_REQ_GET 1
#define FORM_REQ_SEND 2

#define FORM_RESULT_OK 1
#define FORM_RESULT_DENIED 2
#define FORM_RESULT_FAILED 3

/* Rendering Definitions */
#define LABEL_COL_WIDTH "140"

/* Structs */

typedef struct i_form_s
{
  struct i_list_s *items;
  struct i_list_s *options;
  struct i_form_reqdata_s *reqdata;

  int state;
  int notready_count;
  int submit;
} i_form;

typedef struct i_form_option_s
{
  int type;
  void *data;
  int datasize;
} i_form_option;

typedef struct i_form_item_s
{
  int type;
  int state;
  char *name;
  struct i_list_s *options;
  struct i_form_s *form;
} i_form_item;

typedef struct i_form_item_option_s
{
	int type;

	int i;								/* Generic counter */
	int x;								/* Generic coordinate */
	int y;								/* Generic coordinate */

	void *data;
	int datasize;
} i_form_item_option;

typedef struct i_form_get_cbdata_s
{
  i_resource_address *resaddr;
  struct i_entity_address_s *entaddr;
  char *form_name;
  time_t ref_sec;
  int (*callback_func) ();
  void *passdata;
  struct i_msgproc_callback_s *msg_callback;
  struct i_socket_s *socket;
} i_form_get_cbdata;

typedef struct i_form_send_cbdata_s
{
  int (*callback_func) ();
  void *passdata;
  struct i_msgproc_callback_s *msg_callback;
  struct i_socket_s *socket;
} i_form_send_cbdata;
  
typedef struct i_form_reqdata_s
{
  int type;                               /* Get / Send */

  struct i_entity_address_s *entaddr;     /* Entity Address */
  
  struct i_message_s *msg_in;             /* Inbound message */
  struct i_authentication_s *auth;        /* Authentication data */
  void *form_passdata;                    /* Form passdata */
  int form_passdata_size;                 /* Form passdata_size */

  int (*form_func) (struct i_resource_s *self, struct i_form_reqdata_s *reqdata);

  char *form_name;                        /* Requested form */
  time_t ref_sec;                         /* Requested reference time */

  struct i_form_s *form_in;               /* Inbound form */
  struct i_form_s *form_out;              /* Outbound form */
  int result;                             /* Result of request */

  struct timeval req_time;                /* Request time */
  struct timeval deliver_time;            /* Delivery time */
} i_form_reqdata;

/* Prototypes */

void i_form_free_item (void *data);
void i_form_free (void *formptr);
i_form* i_form_create (i_form_reqdata *reqdata, int submit);
i_form* i_form_duplicate (i_form *form);
i_form_item* i_form_create_item (int type, char *name);
i_form* i_form_add_item (i_form *form, i_form_item *item);
i_form* i_form_delete_item (i_form *form, char *name);
int i_form_set_title (i_form *form, char *title);
int i_form_set_submit (i_form *form, int submit);
int i_form_merge (i_form *base, i_form *overlay);

char* i_form_form_to_data (i_form *form, int *datasizeptr);
i_form* i_form_data_to_form (char *data, int datasize);
char* i_form_item_to_data (i_form_item *item, int *datasizeptr);
i_form_item* i_form_data_to_item (char *data, int datasize);

i_form_item* i_form_find_item (i_form *form, char *name);
i_form_item_option* i_form_item_add_option (i_form_item *item, int type, int i, int x, int y, void *data, int datasize);
i_form_item_option* i_form_item_append_option (i_form_item *item, int type, int x, int y, void *data, int datasize);
i_form_item_option* i_form_item_find_option (i_form_item *item, int type, int i, int x, int y);
int i_form_item_delete_option (i_form_item *item, int type, int i, int x, int y);
i_form_item_option* i_form_get_value_for_item (i_form *form, char *name);
void i_form_item_add_link (i_form_item *item, int i, int x, int y, i_resource_address *resaddr, struct i_entity_address_s *entaddr, char *form_name, time_t ref_sec, char *pass_data, int pass_datasize);
int i_form_handler_failed (i_resource *self, struct i_message_s *msg, i_form_reqdata *reqdata);

int i_form_item_ready (i_resource *self, i_form_item *item);
int i_form_item_notready (i_resource *self, i_form_item *item);

i_form_option* i_form_option_create (int type, void *data, int datasize);
i_form_option* i_form_option_add (i_form *form, int type, char *data, int datasize);
i_form_option* i_form_option_find (i_form *form, int type);
int i_form_option_remove (i_form *form, int type);

/* form_get.c */

i_form_get_cbdata* i_form_get_cbdata_create ();
void i_form_get_cbdata_free (void *cbdataptr);
i_form_get_cbdata* i_form_get (i_resource *self, i_resource_address *resaddr, struct i_entity_address_s *entaddr, char *form_name, time_t ref_sec, void *form_passdata, int form_passdatasize, int (*callback_func) (), void *passdata);
int i_form_get_callback (i_resource *self, struct i_socket_s *sock, struct i_message_s *msg, void *passdata);
int i_form_get_handler (i_resource *self, struct i_socket_s *sock, struct i_message_s *msg, void *passdata);

/* form_send.c */

i_form_send_cbdata* i_form_send_cbdata_create ();
void i_form_send_cbdata_free (void *cbdataptr);
i_form_send_cbdata* i_form_send (i_resource *self, i_resource_address *resaddr, struct i_entity_address_s *entaddr, char *form_name, time_t ref_sec, i_form *form, int (*callback_func) (i_resource *self, i_form *form, void *passdata), void *passdata);
int i_form_send_msg_callback (i_resource *self, struct i_socket_s *sock, struct i_message_s *msg, void *passdata);
int i_form_send_failed (i_resource *self, i_form_send_cbdata *cbdata);
int i_form_send_handler (i_resource *self, struct i_socket_s *sock, struct i_message_s *msg, void *passdata);

/* Form item creators */

i_form_item* i_form_string_add (i_form *form, char *name, char *label, char *value);
int i_form_string_set_fgcolor (i_form_item *item, char *label_color_str, char *value_color_str);
int i_form_string_set_bgcolor (i_form_item *item, char *label_color_str, char *value_color_str);

i_form_item* i_form_entry_add (i_form *form, char *name, char *label, char *value);
i_form_item* i_form_password_add (i_form *form, char *name, char *label, char *value);
i_form_item*i_form_textarea_add (i_form *form, char *name, char *label, char *value);
i_form_item* i_form_hidden_add (i_form *form, char *name, char *value);

i_form_item* i_form_dropdown_create (char *name, char *label);
void i_form_dropdown_add_option (i_form_item *item, char *value, char *label, int selected);
int i_form_dropdown_set_selected (i_form_item *item, char *value);
i_form_item_option* i_form_dropdown_get_selected_value (i_form_item *item);

i_form_item* i_form_table_create (i_form *form, char *name, char *label, int cols);
int i_form_table_add_row (i_form_item *item, char *labels[]);
void i_form_table_add_link (i_form_item *item, int col, int row, i_resource_address *resaddr, struct i_entity_address_s *entaddr, char *form_name, time_t ref_sec, char *pass_data, int pass_datasize);
int i_form_table_rowcount (i_form_item *item);
int i_form_table_colcount (i_form_item *item);

void i_form_generic_add (i_form *form, char *name, void *data, int datasize);
void* i_form_generic_get (i_form *form, char *name);
int i_form_generic_item_set_value (i_form_item *item, void *data, int datasize);

i_form_item* i_form_spacer_add (i_form *form);
i_form_item* i_form_hrline_add (i_form *form);

i_form_item* i_form_image_add (i_form *form, char *name, char *label, char *desc, char *imagefullpath);
int i_form_image_setimage (i_form_item *item, char *fullpath);

i_form_item* i_form_frame_start (i_form *form, char *name_str, char *desc_str);
i_form_item* i_form_frame_end (i_form *form, char *name_str);
int i_form_frame_setdesc (i_form_item *item, char *desc_str);

/* Form fabricators */

i_form* i_form_create_denied ();

/* form_deliver.c */

int i_form_deliver (i_resource *self, i_form_reqdata *reqdata);
int i_form_deliver_denied (i_resource *self, i_form_reqdata *reqdata);

/* form_reqdata.c */

i_form_reqdata* i_form_reqdata_create ();
void i_form_reqdata_free (void *reqdataptr);

/* form_func.c */

void* i_form_func_get (i_resource *self, char *form_name, int *resultptr);

/* form_metgraph.c */

i_form_item* i_form_metgraph_add ();
i_form_item* i_form_metcgraph_add ();
int i_form_metgraph_rrdcb ();

