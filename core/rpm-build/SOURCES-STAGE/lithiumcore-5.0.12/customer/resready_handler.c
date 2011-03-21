#include <stdlib.h>

#include <induction.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/form.h>
#include <induction/site.h>

#include "resready_handler.h"

int l_resready_handler (i_resource *self, i_socket *sock, i_message *msg, void *data)
{
  if (!msg || !msg->src) return 0;

  /* Function is called when a resource that
   * this module has spawned is ready
   */

  return 0;
}

