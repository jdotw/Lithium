typedef struct i_module_s
{
  int flags;
  char *name;
  char *file;
  void *handle;

  int (*init) (void *res_ptr);
  int (*entry) (void *res_ptr);
  int (*shutdown) (void *res_ptr);
} i_module;

struct i_list_s* i_module_list ();
i_module* i_module_open (char *module_path, char *module_name);
int i_module_check_construct ();
void i_module_free (i_module *module);
