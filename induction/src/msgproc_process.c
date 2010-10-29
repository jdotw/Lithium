#include <stdlib.h>
#include <dlfcn.h>

#include "induction.h"
#include "socket.h"
#include "message.h"
#include "msgproc.h"
#include "hashtable.h"
#include "construct.h"
#include "module.h"
#include "memcheck.h"

/* Message Processing Functions */

int i_msgproc_process (i_resource *self, i_socket *sock, i_message *msg)
{
  /* Called by i_msgproc_read_callback when a message has been recvd
   * for processing on a msgproc-enabled socket
   */

  if (!self || !sock || !msg) return -1;
  
  if (!msg)
  {
    /* Failed to recv message, assume core process is dead */
    i_printf (2, "i_msgproc_process called with NULL msg. Assuming core disconnect/termination. exiting");
    if (self->construct->module->shutdown) self->construct->module->shutdown (self);
    exit (1);
  }

  switch ((msg->flags & (MSG_FLAG_REQ|MSG_FLAG_RESP)))
  {
    case MSG_FLAG_REQ:  i_printf (2, "i_msgproc_process received a request msg (%li)", msg->msgid);
                        i_msgproc_process_request (self, sock, msg);
                        break;
    case MSG_FLAG_RESP: i_printf (2, "i_msgproc_process received a response msg (%li)", msg->msgid);
                        i_msgproc_process_response (self, sock, msg);
                        break;
    default: i_printf (0, "i_msgproc_process received a message (%li) with neither MSG_FLAG_REQ or MSG_FLAG_RESP flag set", msg->msgid);
  }

  return 0;
}

int i_msgproc_process_request (i_resource *self, i_socket *sock, i_message *msg)
{
  /*
   * Processing a message with the MSG_FLAG_REQ flag set.
   * Hash the msg->type int and attempt to find a suitable
   * handler in the handler_table
   */

  i_msgproc_handler *hdlr;

  if (!self || !sock || !msg) return -1;

  if (i_message_verify_self_destination(self, msg) == 0)
  {
    /* Message is destined for this resource, attempt to
     * find a handler in the handler_table
     */
  
    i_hashtable_key *key;
  
    key = i_hashtable_create_key_int (msg->type, sock->msgproc->handler_table->size);
    if (!key)
    { i_printf (1, "i_msgproc_process_request failed to create hashtable key for msg->type %i", msg->type); return -1; }
  
    hdlr = i_hashtable_get (sock->msgproc->handler_table, key);
    i_hashtable_free_key (key);
  }
  else
  {
    /* Message is not destined for this resource, no handler possible
     * except the default, if present 
     */
    hdlr = NULL;
  }


  /* Record mem usage */
  float vss_before = 0.0;
  if (self->perflog) vss_before = i_memcheck_rss ();

  if (hdlr)
  {
    /* Handler found */
    if (!hdlr->callback_func)
    { i_printf (2, "i_msgproc_process_request found appropriate handler but the callback was NULL"); return -1; }
  
    hdlr->uses++;
    hdlr->callback_func (self, sock, msg, hdlr->passdata);
    
    /* Check mem usage */
    if (self->perflog)
    {
      float vss_after = i_memcheck_rss ();
      i_printf (1, "PERF: After processing request message type %i using handler %p vss memory usage increased %.0fbytes", msg->type, hdlr->callback_func, (vss_after - vss_before));
    }
  }
  else
  {
    /* No handler, try the default */
    if (!sock->msgproc->default_handler || !sock->msgproc->default_handler->callback_func)
    {
      i_printf (2, "i_msgproc_process_request received msg->type %i but found no handler (no default set)", msg->type);
      return -1;
    }

    sock->msgproc->default_handler->uses++;
    sock->msgproc->default_handler->callback_func (self, sock, msg, sock->msgproc->default_handler->passdata);
    
    /* Check mem usage */
    if (self->perflog)
    {
      float vss_after = i_memcheck_rss ();
      i_printf (1, "PERF: After processing request message type %i using DEFAULT handler %p vss memory usage increased %.0fbytes", 
        msg->type, sock->msgproc->default_handler->callback_func, (vss_after - vss_before));
    }
  }

  return 0;
}

int i_msgproc_process_response (i_resource *self, i_socket *sock, i_message *msg)
{
  /*
   * Processing a response message. We assume there is a reason
   * why we are receiving this response in an asynchronous manner so
   * the msg->reqid will be hashed and a search done on the callback_table
   * to try and find an appropriate callback.
   *
   * If one is found, the callback is run and then removed 
   */

  i_hashtable_key *key = NULL; 
  i_msgproc_callback *cb;

  if (!self || !sock || !msg || msg->reqid < 1) return -1;

  if (i_message_verify_self_destination(self, msg) == 0)
  {
    /* Message is destined for this resource, attempt to find a callback in
     * the callback_table
     */
    key = i_hashtable_create_key_long (msg->reqid, sock->msgproc->callback_table->size);
    if (!key)
    { i_printf (1, "i_msgproc_process_response failed to create hashtable key for msg->reqid %li", msg->reqid); return -1; }
  
    cb = i_hashtable_get (sock->msgproc->callback_table, key);
  }
  else
  {
    /* Message is not destined for this resource, no callback possible except
     * for the default if present
     */
    cb = NULL;
  }
  
  /* Record mem usage */
  float vss_before = 0.0;
  if (self->perflog) vss_before = i_memcheck_rss ();
  
  if (cb)
  {
    /* Callback present */
    
    if (cb->callback_func)
    { cb->callback_func (self, sock, msg, cb->passdata); }
    else
    { i_printf (2, "i_msgproc_process_response found appropriate callback but the callback_func was NULL"); }
  
    /* Check mem usage */
    if (self->perflog)
    {
      float vss_after = i_memcheck_rss ();
      i_printf (1, "PERF: After processing response message type %i using handler %p vss memory usage increased %.0fbytes", msg->type, cb->callback_func, (vss_after - vss_before));
    }

    i_hashtable_remove (sock->msgproc->callback_table, key);
  }
  else
  {
    /* No callback, try the default */
    if (sock->msgproc->default_callback && sock->msgproc->default_callback->callback_func)
    { sock->msgproc->default_callback->callback_func (self, sock, msg, sock->msgproc->default_callback->passdata); }
    else
    { i_printf (2, "i_msgproc_process_response received msg->reqid %li (msg->type=%i) but found no callback (no default set)", msg->reqid, msg->type); }
    
    /* Check mem usage */
    if (self->perflog)
    {
      float vss_after = i_memcheck_rss ();
      i_printf (1, "PERF: After processing response message type %i using DEFAULT handler %p vss memory usage increased %.0fbytes", msg->type, sock->msgproc->default_callback->passdata, (vss_after - vss_before));
    }
  }

  if (key) i_hashtable_free_key (key);  

  return 0;
}

