/* lic.c */

#define KEY_NOTPROCESSED 0
#define KEY_DECODED 1
#define KEY_INVALID 2
#define KEY_INVALID_CUSTOMER 3
#define KEY_INVALID_EXPIRED 4
#define KEY_INVALID_DUPLICATE 5
#define KEY_INVALID_DUPLICATE_EXCLUSIVE 6
#define KEY_INVALID_WRONGTYPE 7
#define KEY_VALID 128

#define KEY_FLAG_NFR 8
#define KEY_FLAG_DEMO 16
#define KEY_FLAG_FREE_UNREG 32
#define KEY_FLAG_FREE_EXPIRED 64

struct l_lic_entitlement_s* l_lic_static_entitlement ();
int l_lic_reset_entitlement (i_resource *self);
int l_lic_entitlement_reset_timercb (i_resource *self, struct i_timer_s *timer, void *passdata);
struct l_lic_entitlement_s* l_lic_load_entitlement (i_resource *self);

i_list* l_lic_loadkeys (i_resource *self);
struct l_lic_key_s* l_lic_validate_key (i_resource *self, char *enckey_str);
int b64decode(unsigned char* str);

/* lic_entitlement.c */

typedef struct l_lic_entitlement_s
{
  int customer_licensed;   /* 0=No core license present / 1=Core license present */
  
  long devices_max;        /* Maximum devices */
  long devices_used;       /* Device licenses used */
  long devices_excess;     /* Excess configured devices */

  char *type_str;          /* License type string */
  time_t expiry_sec;       /* Primary license expiry */

  int paid;                /* 1=Paid License Present */
  int demo;                /* 0=Not Demo 1=Active Demo 2=Expired Demo */
  int nfr;                 /* 0=Not NFR 1=Active NFR 2=Expired NFR */
  int free;                /* 0=Not Free 1=Free Mode */
  int limited;             /* 1=Limited, no Xserve or Xsan */
  int acn;                 /* 1=ACN License, Max 5xCust 5xDevs */
  int msp;                 /* 1=MSP License */

  /* Legacy helpers */
  int enterprise_48;       /* 1 = Old-style 4.8 Enterprise license present */
  int modules_48;          /* Number of 4.8 device module licenses present */
  
} l_lic_entitlement;

l_lic_entitlement* l_lic_entitlement_create ();
void l_lic_entitlement_free (void *ementptr);

/* lic_key.c */

typedef struct l_lic_key_s
{
  long id;
  int status;
  char *raw_str;
  char *enc_str;

  long serial;
  char *type_str;
  char *custhash_str;
  char *product_str;
  long version;
  unsigned long volume;
  unsigned long flags;
  time_t expiry_sec;
  
} l_lic_key;

#define KEY_48BUNDLE_SITE_MAX 0xFF000000
#define KEY_48BUNDLE_SITE_VOLUME 0x00FF0000
#define KEY_48BUNDLE_DEV_MAX 0x0000FF00
#define KEY_48BUNDLE_DEV_VOLUME 0x000000FF

l_lic_key* l_lic_key_create ();
void l_lic_key_free (void *keyptr);
int l_lic_key_add (i_resource *self, char *key_str);
int l_lic_key_remove (i_resource *self, long id);
int l_lic_key_sqlcb ();
char* l_lic_key_status (int status);

/* lic_take.c */
int l_lic_take (i_resource *self, struct i_entity_s *ent);
void l_lic_rescind (i_resource *self, struct i_entity_s *ent);

/* lic_form_main.c */
int form_lic_main (i_resource *self, i_form_reqdata *reqdata);
size_t l_lic_curlcb (void *buffer, size_t size, size_t nmemb, void *userp);

/* lic_key_list.c */
int xml_lic_key_list ();
