#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "userdb.h"
#include "user.h"
#include "bdb.h"
#include "list.h"
#include "files.h"
#include "auth.h"

static i_list *static_userlist_cache = NULL;

/* Get */

i_user* i_userdb_get (i_resource *self, char *username)
{
  /* Retrieve the given username's user record from
   * the specified userdb resource 
   */

  i_user *user;
  i_db *db;
  i_db_record *record;

  if (!self || !username) return NULL;

  /* Check cache */
  if (static_userlist_cache)
  {
    /* Loop through each cached user */
    for (i_list_move_head(static_userlist_cache); (user=i_list_restore(static_userlist_cache))!=NULL; i_list_move_next(static_userlist_cache))
    {
      /* Check if this user matches */
      if (!strcmp(user->auth->username, username))
      {
        /* Match found! Return duplicate */
        return i_user_duplicate (user);
      }
    }
  }
  else
  { 
    static_userlist_cache = i_list_create (); 
    i_list_set_destructor (static_userlist_cache, i_user_free);
  }
  
  /* Get the user record from the DB */

  db = i_db_open (self, USER_DB_FILE);
  if (!db)
  { i_printf (1, "i_userdb_get failed to open userdb"); return NULL; }
  
  record = i_db_get (self, db, username, strlen(username)+1);
  i_db_free (db);
  if (!record)
  {
    i_printf (2, "i_userdb_get failed to get userdb record for username %s", username);
    return NULL;
  }

  user = i_user_data_to_struct (record->data, record->datasize);
  i_db_free_record (record);
  if (!user)
  {
    i_printf (1, "l_userdb_get failed to convert to userdb data to user struct for username %s", username);
    return NULL;
  }

  /* Cache user */
  if (static_userlist_cache)
  { i_list_enqueue (static_userlist_cache, i_user_duplicate(user)); }

  return user;
}

/* Cache */

void i_userdb_clear_cache ()
{
  if (static_userlist_cache)
  { 
    i_list_free (static_userlist_cache);
    static_userlist_cache = NULL;
  }
}

/* Get all */

i_list* i_userdb_get_all (i_resource *self)
{
  i_db *db;
  i_db_record *record;
  i_list *records;
  i_list *users;

  if (!self) return NULL;

  db = i_db_open (self, USER_DB_FILE);
  if (!db)
  { i_printf (1, "i_userdb_get_all_list failed to open db"); return NULL; }

  records = i_db_get_all_list (self, db);
  i_db_free (db);
  if (!records)
  { i_printf (1, "i_userdb_get_all_list to get all user records from DB"); return NULL; }

  users = i_list_create ();
  if (!users)
  { i_printf (1, "i_userdb_get_all_list failed to create users list"); i_list_free (records); return NULL; }
  i_list_set_destructor (users, i_user_free);
  
  for (i_list_move_head(records); (record=i_list_restore(records))!=NULL; i_list_move_next(records))
  {
    /* Loop through each record */
    i_user *user;

    user = i_user_data_to_struct (record->data, record->datasize);
    if (!user)
    { i_printf (1, "i_userdb_get_all_list failed to decode a users data from the db. continuing"); continue; }

    i_list_enqueue (users, user);
  }

  i_list_free (records);

  return users;
}

/* Put */

int i_userdb_put (i_resource *self, i_user *user)
{
  int num;
  int datasize;
  char *data;
  i_db *db;

  data = i_user_struct_to_data (user, &datasize);
  if (!data)
  { i_printf (1, "i_userdb_put failed to convert user struct to data"); return -1; }

  db = i_db_open (self, USER_DB_FILE);
  if (!db)
  { i_printf (1, "i_userdb_put failed to open db"); free (data); return -1; }      
  
  num = i_db_put (self, db, user->auth->username, strlen(user->auth->username)+1, data, datasize);
  i_db_free (db);
  free (data);
  if (num != 0)
  { i_printf (1, "i_userdb_put failed to store updated user record"); return -1; }

  i_userdb_clear_cache ();
  
  return 0;
}

/* Del */

int i_userdb_del (i_resource *self, char *username)
{
  int num;
  i_db *db;

  db = i_db_open (self, USER_DB_FILE);
  if (!db)
  { i_printf (1, "i_userdb_del failed to open db"); return -1; }      
   
  num = i_db_del (self, db, username, strlen(username)+1);
  i_db_free (db);
  if (num != 0)
  { i_printf (1, "i_userdb_del failed to remove user record from db"); return -1; }

  i_userdb_clear_cache ();

  return 0;
}
