typedef struct i_vendor_s
{
  char *name_str;
  char *desc_str;
  char *module_str;
  char *detect_str;

  struct i_module_s *module;
} i_vendor;

/* vendor.c */

int i_vendor_init ();
i_vendor* i_vendor_create ();
void i_vendor_free (void *vendorptr);
void i_vendor_set (i_vendor *vendor);
i_vendor* i_vendor_get ();
struct i_list_s* i_vendor_list (i_resource *self);
