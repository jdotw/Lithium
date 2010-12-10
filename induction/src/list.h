#include <stdint.h>

typedef struct i_list_s
{
  unsigned long size;
  struct i_list_cell_s *start;
  struct i_list_cell_s *end;
  struct i_list_cell_s *p;
  void (*destructor) (void *);
  uint8_t sorted;   /* 1 = no change since last sort, 0 = change since last sort */
} i_list;

typedef struct i_list_cell_s
{
  void *data;
  struct i_list_cell_s *next;
  struct i_list_cell_s *prev;
} i_list_cell;

i_list* i_list_create (void);
void i_list_free (void *listptr);
int i_list_set_destructor (i_list *list, void (*destructor)(void *data));
int i_list_delete (i_list *list);
int i_list_shunt (i_list *list);
int i_list_move_head (i_list *list);
int i_list_move_tail (i_list *list);
int i_list_move_next (i_list *list);
int i_list_move_prev (i_list *list);
void* i_list_restore (i_list *list);
int i_list_move_last (i_list *list);
int i_list_store (i_list *list, const void  *data);
int i_list_search (i_list *list, const void *data);
int i_list_push (i_list *list, const void *data);
void* i_list_poplast (i_list *list);
void* i_list_peak (i_list *list);
int i_list_enqueue (i_list *list, const void  *data);
int i_list_sort (i_list *list, int (*cmp_func) (void *cur, void *next));
