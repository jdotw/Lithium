typedef struct
{
  /* Basic info */
  int type;                 /* Hardware, software, etc */
  char *vendor_str;
  char *product_str;

  /* Detailed info */
  char *version_str;
  char *serial_str;
  char *platform_str;
  char *feature_str;
  int v_major;
  int v_minor;
  int v_micro;
  int v_patch;
  int v_build;

  /* Info not recorded in SQL */
  long index;

  /* Inventory item info */
  int flags;
  struct i_device_s *dev;
  struct timeval register_time;
} i_inventory_item;

/* Type Codes */

/* Base codes */
#define INV_RESERVED 0        /* Reserved   0-63  */
#define INV_HARDWARE 64       /* Software 64-127 */
#define INV_SOFTWARE 128      /* Software 128-191 */

/* 0-63    == Reserved */
#define INV_ALL 1

/* 64-127 == Hardware */
#define INV_CHASSIS 65
#define INV_MAINBOARD 66
#define INV_INTCARD 67
#define INV_STORAGE 68
#define INV_PERIPHERAL 69

/* 128-191  == Software */
#define INV_OS 129
#define INV_APP 130
#define INV_DRIVER 131

/* Flags */

#define INV_FLAG_STATIC 1     /* Item is a static entry */
#define INV_FLAG_SUSPECT 2    /* Item is suspect (i.e may not be correct */

/* inventory.c */
long i_inventory_register (i_resource *self, i_inventory_item *item);
long i_inventory_deregister (i_resource *self, i_resource_address *customer_addr, i_resource_address *device_addr, int type, char *vendor_str, char *product_str, char *verser_str);
long i_inventory_deregister_item (i_resource *self, i_resource_address *customer_addr, i_resource_address *device_addr, i_inventory_item *item);

/* inventory_item.c */
i_inventory_item* i_inventory_item_create ();
void i_inventory_item_free (void *itemptr);

/* inventory_static.c */
long i_inventory_static_register_all (i_resource *self, i_resource_address *customer_addr, struct i_hierarchy_s *device_hierarchy);

/* inventory_str.c */

char* i_inventory_type_str (int type);
