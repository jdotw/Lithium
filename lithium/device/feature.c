#include <stdlib.h>

#include <induction.h>

#include "feature.h"
#include "featuredb.h"

/* Struct manipulation */

l_feature* l_feature_create ()
{
  l_feature *feat;

  feat = (l_feature *) malloc (sizeof(l_feature));
  if (!feat)
  { i_printf (1, "l_feature_create failed to malloc feat"); return NULL; }
  memset (feat, 0, sizeof(l_feature));

  return feat;
}

void l_feature_free (void *featptr)
{
  l_feature *feat = featptr;

  if (!feat) return;

  if (feat->id_str) free (feat->id_str);
  if (feat->desc_str) free (feat->desc_str);
  if (feat->module_name) free (feat->module_name);
  if (feat->module) i_module_free (feat->module);

  free (feat);
}

/* Feature manipulation */

l_feature* l_feature_get_by_id (i_resource *self, char *id_str)
{
  void *p;
  i_list *list;
  l_feature *feature;

  list = l_feature_list ();

  p = list->p;
  for (i_list_move_head(list); (feature=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    if (!strcmp(id_str, feature->id_str))
    {
      list->p = p;
      return feature;
    }
  }
  
  list->p = p;
  return NULL;
}

int l_feature_remove_by_id (i_resource *self, char *id_str)
{
  int num;
  i_list *list;
  l_feature *feat;

  list = l_feature_list ();

  for (i_list_move_head(list); (feat=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    if (!strcmp(feat->id_str, id_str))
    {
      int dbresult;
      int resresult;

      dbresult = l_featuredb_del (self, feat->id_str);
      if (dbresult != 0)
      { i_printf (1, "l_feature_remove_by_id failed to remove feature %s from the feature database", feat->id_str); }

      num = l_feature_disable (self, feat);
      if (num != 0)
      { i_printf (1, "l_feature_remove_by_id failed to disable feature %s", feat->id_str); }
  
      i_list_delete (list);

      return 0;   /* Returns after first match */
    }
  }
  return -1;
}

/* Feature structure conversion functions */

char* l_feature_struct_to_data (l_feature *feat, int *datasizeptr)
{
  /* Data format is :
   *
   * int type;
   * int id_str_size;
   * char *id_str;
   * int desc_str_size;
   * char *desc_str;
   * int module_name_sizs;
   * char *module_name;
   */

  int datasize;
  char *data;
  char *dataptr;
  int num;

  if (!datasizeptr) return NULL;
  memset (datasizeptr, 0, sizeof(int));

  if (!feat) return NULL;

  datasize = (4*sizeof(int));
  if (feat->id_str) datasize += strlen(feat->id_str)+1;
  if (feat->desc_str) datasize += strlen(feat->desc_str)+1;
  if (feat->module_name) datasize += strlen(feat->module_name)+1;

  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "l_feature_struct_to_data failed to malloc data"); return NULL; }
  memset (data, 0, datasize);
  dataptr = data;

  dataptr = i_data_add_int (data, dataptr, datasize, &feat->type);
  if (!dataptr)
  { i_printf (1, "l_feature_struct_to_data failed to add feat->type to data"); free (data); return NULL; }

  dataptr = i_data_add_string (data, dataptr, datasize, feat->id_str);
  if (!dataptr)
  { i_printf (1, "l_feature_struct_to_data failed to add feat->id_str to data"); free (data); return NULL; }

  dataptr = i_data_add_string (data, dataptr, datasize, feat->desc_str);
  if (!dataptr)
  { i_printf (1, "l_feature_struct_to_data failed to add feat->desc_str to data"); free (data); return NULL; }

  dataptr = i_data_add_string (data, dataptr, datasize, feat->module_name);
  if (!dataptr)
  { i_printf (1, "l_feature_struct_to_data failed to add feat->module_name to data"); free (data); return NULL; }

  memcpy (datasizeptr, &datasize, sizeof(int));

  return data;
}

l_feature* l_feature_data_to_struct (char *data, int datasize)
{
  int offset;
  l_feature *feat;
  char *dataptr = data;

  feat = l_feature_create ();
  if (!feat) { i_printf (1, "l_feature_data_to_struct failed to create feat"); return NULL; }

  feat->type = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_feature_data_to_struct failed to get feat->type from data"); l_feature_free (feat); return NULL; }
  dataptr += offset;

  feat->id_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_feature_data_to_struct failed to get feat->id_str from data"); l_feature_free (feat); return NULL; }
  dataptr += offset;

  feat->desc_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_feature_data_to_struct failed to get feat->desc_str from data"); l_feature_free (feat); return NULL; }
  dataptr += offset;

  feat->module_name = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_feature_data_to_struct failed to get feat->module_name from data"); l_feature_free (feat); return NULL; }
  dataptr += offset;

  return feat;
}
