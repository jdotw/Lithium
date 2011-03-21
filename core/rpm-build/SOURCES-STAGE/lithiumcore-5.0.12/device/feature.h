#define FEATURE_DB_FILE "features.db"
#define FEATURE_CONF_FILE "features.conf"

typedef struct
{
  int type;
  char *id_str;
  char *desc_str;
  char *module_name;
  i_module *module;

  int enabled;
} l_feature;

/* feature.c */

void l_feature_free (void *featptr);
l_feature* l_feature_create ();
l_feature* l_feature_get_by_id (i_resource *self, char *id_str);
int l_feature_remove_by_id (i_resource *self, char *id_str);
char* l_feature_struct_to_data (l_feature *feat, int *datasizeptr);
l_feature* l_feature_data_to_struct (char *data, int datasize);

/* feature_list.c */

int l_feature_list_init (i_resource *self);
i_list* l_feature_list ();

/* feature_control.c */

#define FEATURE_DISABLE 0
#define FEATURE_ENABLE 1

l_feature_enable (i_resource *self, l_feature *feat);
l_feature_disable (i_resource *self, l_feature *feat);

