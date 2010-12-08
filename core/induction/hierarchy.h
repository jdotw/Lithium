typedef struct i_hierarchy_s
{
  char *customer_id;                              /* FIX LEGACY */
  char *customer_desc;                            /* FIX LEGACY */
  struct i_resource_address_s *customer_addr;     /* FIX LEGACY */
  char *cust_name;
  char *cust_desc;
  struct i_customer_s *cust;
  struct i_resource_address_s *cust_addr;
  
  char *site_id;                                  /* FIX LEGACY */
  char *site_name;
  char *site_desc;
  struct i_site_s *site;
  
  char *device_id;                                /* FIX LEGACY */
  char *device_desc;                              /* FIX LEGACY */
  struct i_resource_address_s *device_addr;       /* FIX LEGACY */
  char *dev_name;
  char *dev_desc;
  struct i_device_s *dev;
  struct i_resource_address_s *dev_addr;

  char *entity_str;
} i_hierarchy;

/* heirarchy.c */

void i_hierarchy_free (void *hierptr);
i_hierarchy* i_hierarchy_create ();
i_hierarchy* i_hierarchy_build (i_resource *self, i_resource *parent);
