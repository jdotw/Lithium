/* src/bdb.h.  Generated from bdb.h.in by configure.  */
#include <db.h>

typedef struct
{
  DB *db;
  char *file;
} i_db;

typedef struct
{
  int keysize;
  void *key;
  int datasize;
  void *data;
} i_db_record;

void i_db_free (i_db *db);
i_db* i_db_open (i_resource *self, char *dbname);
void i_db_free_record (void *recordptr);
i_db_record* i_db_record_create (void *key, int keysize, void *data, int datasize);
int i_db_put (i_resource *self, i_db *db, void *key, int key_size, void *data, int data_size);
i_db_record* i_db_get (i_resource *self, i_db *db, void *key, int key_size);
struct i_list_s* i_db_get_all_list (i_resource *self, i_db *db);
int i_db_del (i_resource *self, i_db *db, void *key, int key_size);

