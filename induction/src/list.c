#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include "induction.h"
#include "list.h"

#define I_LIST_HEAD 0
#define I_LIST_TAIL 1
#define I_LIST_NEXT 2
#define I_LIST_PREV 3
#define I_LIST_LAST 4

extern i_resource *global_self;

i_list* i_list_create (void)
{
  i_list *list;

  list = (i_list *) malloc (sizeof(i_list));
  memset(list, 0, sizeof(i_list));
  list->size = 0;

  list->start = (i_list_cell *) malloc(sizeof(i_list_cell));
  memset(list->start, 0, sizeof(i_list_cell));

  list->end = (i_list_cell *) malloc(sizeof(i_list_cell));
  memset(list->end, 0, sizeof(i_list_cell));

  list->start->next = list->end;
  list->end->prev = list->start;

  list->p = list->start;
  
  return list;
}

void i_list_free (void *listptr)
{
  i_list *list = listptr;

  if (!list) return;

  for (i_list_move_head(list); (i_list_restore(list)) != NULL; i_list_move_next(list))
  {
    i_list_delete (list);
  }
  free (list->start);
  free (list->end);
  
  free (list);
}

int i_list_set_destructor (i_list *list, void (*destructor)(void *data))
{
  if (!list) return -1;
  list->destructor = destructor;
  return 0;
}

int i_list_delete (i_list *list) 
{
  i_list_cell *r;

  if (!list || !list->p || !list->p->next || !list->p->prev) return -1;

  r = list->p;
  list->p = r->prev;

  r->prev->next = r->next;      /* Adjust the previous record */
  r->next->prev = r->prev;      /* Adjust the next record */

  list->size--;
  if (list->destructor) list->destructor (r->data);                                       
  free (r);

  return 0;
}

int i_list_shunt (i_list *list)
{
  /* Pushes the current item (p) to the 
   * end of the list
   */

  i_list_cell *r;
  void *data;

  if (!list || !list->p || !list->p->next || !list->p->prev) return -1;

  r = list->p;
  data = r->data;
  list->p = r->prev;

  r->prev->next = r->next;      /* Adjust the previous record */
  r->next->prev = r->prev;      /* Adjust the next record */

  list->size--;
  free (r);
  
  i_list_enqueue (list, data);  

  return 0;
}

int i_list_move (i_list *list, int where)
{
  if (!list) return -1;
  
  switch (where) 
  {
    case I_LIST_HEAD:
      list->p = list->start->next;
      return 0;
    case I_LIST_LAST:
      list->p = list->end->prev;
      return 0;
    case I_LIST_NEXT:
      if (list->p->next) { list->p = list->p->next; return 0; }
      break;
    case I_LIST_PREV:
      if (list->p->prev) { list->p = list->p->prev; return 0; }
      break;
  }

  return -1;		
}

int i_list_move_head (i_list *list)
{
  return i_list_move(list, I_LIST_HEAD);
}

int i_list_move_next (i_list *list)
{
  return i_list_move(list, I_LIST_NEXT);
}

int i_list_move_prev (i_list *list)
{
  return i_list_move(list, I_LIST_PREV);
}

int i_list_move_last (i_list *list)
{
  return i_list_move (list, I_LIST_LAST);
}

int i_list_store (i_list *list, const void *data)
{
  i_list_cell *p;

  if (!list || !data) return -1;

  list->sorted = 0;

  p = (i_list_cell *) malloc(sizeof(i_list_cell));
  memset(p, 0, sizeof(i_list_cell));
  p->data = (void *) data;

  if (list->p == list->end)
  {
    /* Store in the cell before list->end */

    p->prev = list->end->prev;
    list->end->prev->next = p;
    list->end->prev = p;
    p->next = list->end;
  }
  else
  {
    /* Otherwise, store after the current cell */

    p->prev = list->p;
    p->next = list->p->next;
    
    list->p->next->prev = p;
    list->p->next = p;
  }

  list->size++;

  return 0;
}

void* i_list_restore (i_list *list)
{
  if (!list) return NULL;
  return (list->p ? list->p->data : NULL);
}

int i_list_search (i_list *list, const void *data)
{
  void *p;
  void *d;

  if (!list || !data) return -1;

  p = list->p;      /* Remeber the initial list->p pointer */
	
  for (i_list_move_head(list); (d = i_list_restore(list)) != NULL; i_list_move_next(list)) 
  {
    if (d == data) return 0;
  }

  list->p = p;      /* Restore the original list->p if no search match */

  return -1;
}

int i_list_push (i_list *list, const void *data)
{
  int num;
  void *p;
  if (!list || !data) return -1;

  p = list->p;
  list->p = list->start;
  num = i_list_store (list, data);
  list->p = p;
  
  return num;
}

void* i_list_poplast (i_list *list)
{
  void *r;

  if (!list || !list->size) return NULL;
  i_list_move_last (list);
  r = i_list_restore (list);
  i_list_delete (list);
  return r;
}

void* i_list_peak (i_list *list)
{
  if (!list || !list->size) return NULL;
  i_list_move_head (list);
  return (i_list_restore(list));
}	

int i_list_enqueue (i_list *list, const void  *data)
{
  int num;
  void *p;
  if (!list || !data) return -1;

  p = list->p;
  list->p = list->end;
  num = i_list_store (list, data);
  list->p = p;

  return num;
}

int i_list_sort (i_list *list, int (*cmp_func) (void *cur, void *next))
{
  unsigned long i;
  void *cur_data;
  void *next_data;
  void *p;

  if (!list || !cmp_func) return -1;

  if (list->size < 2) return 0;     /* Nothing to sort */

  p = list->p;
  
  for (i=0; i < list->size-1; i++)
  {
    /* Loop through the list */

    unsigned long x;

    for (x=0; x < list->size-1-i; x++)
    {
      unsigned long y;

      /* Get the current cell */

      i_list_move_head(list);
      for (y=0; y < x; y++)
      { i_list_move_next(list); }
      cur_data = i_list_restore (list);
      if (!cur_data) continue;

      /* Get the next cell in line */

      i_list_move_next(list);
      next_data = i_list_restore (list);
      i_list_move_prev(list);
      if (!next_data) continue;

      /* Compare */

      if (cmp_func(cur_data, next_data) == 1)
      {
        /* Swap cur and cur_next if the cmp_func returns 1
         *
         * ub ----> cur ----> cur_next ----> lb
         *
         */

        i_list_cell *ub;
        i_list_cell *cur_next;
        i_list_cell *cur;
        i_list_cell *lb;

        cur = list->p;
        cur_next = cur->next;
        ub = cur->prev;         /* upper list bound pointer */
        lb = cur_next->next;    /* lower list bound pointer */

        ub->next = cur_next;        /* Adjust the boundary cell pointers */
        lb->prev = cur;

        cur->prev = cur_next;       /* Adjust the current pointer */
        cur->next = lb;

        cur_next->prev = ub;    /* Adjust the cur_next pointer */
        cur_next->next = cur;
      }
    }
  }

  list->sorted = 1;
  list->p = p;

  return 0;
}

