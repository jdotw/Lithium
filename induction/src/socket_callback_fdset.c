#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>


#include "induction.h"
#include "socket.h"
#include "list.h"
#include "loop.h"

static i_list *static_candidate_list = NULL;
static fd_set static_read_fdset;
static fd_set static_write_fdset;
static fd_set static_except_fdset;
static int static_highest_sockfd = 0;
static int static_fdset_invalidate = 1;     /* Set to 1 to force recalculation of FDSETs */

/* Accessors */

void i_socket_callback_fdset_invalidate ()
{ 
  if (static_candidate_list)
  { i_list_free (static_candidate_list); }
  static_candidate_list = NULL;
  static_fdset_invalidate = 1; 
}

/* Socket fdset Pre/Post Processors */

int i_socket_callback_fdset_preprocessor (i_resource *self, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data)
{
  /*
   * Return highest sockfd and sets up fdsets
   */

  /*
   * If the static_fdset_invalidate is set to 1, there has been a 
   * change in the socket callbacks and the list should be parsed
   * to build the fdsets and candidate lists
   */

  if (static_fdset_invalidate == 1)
  {
    FD_ZERO (&static_read_fdset);
    FD_ZERO (&static_write_fdset);
    FD_ZERO (&static_except_fdset);

    i_list *list = i_socket_callback_list ();

    if (static_candidate_list) i_list_free (static_candidate_list);
    static_candidate_list = i_list_create ();
    if (!static_candidate_list)
    { i_printf (1, "i_socket_callback_fdset_preprocessor failed to create static_candidate_list list"); return 0; }
    static_highest_sockfd = 0;

    i_socket_callback *cb;
    for (i_list_move_head(list); (cb=i_list_restore(list))!=NULL; i_list_move_next(list))
    {
      if (cb->type == 0 || !cb->socket || !cb->callback_func)
      { i_list_delete (list); continue; }

      switch (cb->type)
      {
        case SOCKET_CALLBACK_READ_PREEMPT: 
          if (cb->socket->read_pending && cb->socket->read_pending->size > 0) break;    /* No break if read_pending is present, hence added to read_fdset */
        case SOCKET_CALLBACK_READ: 
          FD_SET (cb->socket->sockfd, &static_read_fdset);
          if (cb->socket->sockfd > static_highest_sockfd) static_highest_sockfd = cb->socket->sockfd;
          i_list_enqueue (static_candidate_list, cb);
          break;
        case SOCKET_CALLBACK_WRITE_PREEMPT: 
          if (cb->socket->write_pending && cb->socket->write_pending->size > 0) break; /* No break if write_pending is present, hence added to write_fdset */
        case SOCKET_CALLBACK_WRITE: 
          FD_SET (cb->socket->sockfd, &static_write_fdset);
          if (cb->socket->sockfd > static_highest_sockfd) static_highest_sockfd = cb->socket->sockfd;
          i_list_enqueue (static_candidate_list, cb);
          break;
        case SOCKET_CALLBACK_EXCEPT: 
          FD_SET (cb->socket->sockfd, &static_except_fdset);
          i_list_enqueue (static_candidate_list, cb);
          break;
        default: 
          i_socket_callback_remove (cb);
          continue;
      }
    }
  }

  /* Set fdset values based on master fdsets */
#ifdef FD_COPY
  if (read_fdset) FD_COPY (&static_read_fdset, read_fdset);
  if (write_fdset) FD_COPY (&static_write_fdset, write_fdset);
  if (except_fdset) FD_COPY (&static_except_fdset, except_fdset);
#else
  if (read_fdset) memcpy(&static_read_fdset, read_fdset, sizeof(fd_set));
  if (write_fdset) memcpy(&static_write_fdset, write_fdset, sizeof(fd_set));
  if (except_fdset) memcpy(&static_except_fdset, except_fdset, sizeof(fd_set));
#endif

  return static_highest_sockfd;
}

int i_socket_callback_fdset_postprocessor (i_resource *self, int select_num, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *passdata)
{
  int num;
  i_list *list;
  i_socket_callback *cb;

  list = i_socket_callback_list ();

  for (i_list_move_head(static_candidate_list); (cb=i_list_restore(static_candidate_list))!=NULL; i_list_move_next(static_candidate_list))
  {
    int (*callback_func) () = NULL;
    void *callback_passdata = NULL;

    if (cb->type == 0 || !cb->socket || !cb->callback_func)
    { continue; }
    
    /* Check if socket is 'set' */
    if (cb->socket)
    {
      switch (cb->type)
      {
        case SOCKET_CALLBACK_READ_PREEMPT: 
            if (cb->socket->read_pending && cb->socket->read_pending->size > 0) break;    /* No break if read_pending is present, hence added to read_fdset */
        case SOCKET_CALLBACK_READ: 
            if (FD_ISSET(cb->socket->sockfd, read_fdset))
            { callback_func = cb->callback_func; callback_passdata = cb->passdata; }
            break;
        case SOCKET_CALLBACK_WRITE_PREEMPT: 
            if (cb->socket->write_pending && cb->socket->write_pending->size > 0) break; /* No break if write_pending is present, hence added to write_fdset */
        case SOCKET_CALLBACK_WRITE: 
            if (FD_ISSET(cb->socket->sockfd, write_fdset))
            { callback_func = cb->callback_func; callback_passdata = cb->passdata; }
            break;
        case SOCKET_CALLBACK_EXCEPT:
            if (FD_ISSET(cb->socket->sockfd, except_fdset))
            { callback_func = cb->callback_func; callback_passdata = cb->passdata; }
            break;
        default: 
            i_socket_callback_remove (cb);
            continue;
      }
    }

    if (callback_func)
    {
      /* Socket is set, run callback */
      void *p;

      p = list->p;
      
      num = callback_func (cb->resource, cb->socket, callback_passdata);
      if (num != 0)
      { i_socket_callback_remove (cb); }

      list->p = p;
    }
  }

  return 0;
}
