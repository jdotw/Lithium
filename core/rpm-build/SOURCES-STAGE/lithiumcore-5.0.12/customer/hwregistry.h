/* Definitions */

#define DEFAULT_VENDOR_TABLE_SIZE 20
#define DEFAULT_PRODUCT_TABLE_SIZE 500
#define DEFAULT_SITE_TABLE_SIZE 100
#define DEFAULT_DEVICE_TABLE_SIZE 500

/* hwregistry.c */

int l_hwregistry_enable (i_resource *self);
int l_hwregistry_disable (i_resource *self);
i_hashtable* l_hwregistry_table_device ();
i_hashtable* l_hwregistry_table_vendor ();
i_hashtable* l_hwregistry_table_site ();

